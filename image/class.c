/*
 * class.c - Process class information from an image file.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "program.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Hash a class name.
 */
static int HashClassName(const char *name, const char *namespace)
{
	unsigned long hash;
	if(namespace)
	{
		hash = ILHashString(0, namespace, strlen(namespace));
		hash = ILHashString(hash, ".", 1);
	}
	else
	{
		hash = 0;
	}
	hash = ILHashString(hash, name, strlen(name));
	return (int)(hash & (IL_CONTEXT_HASH_SIZE - 1));
}

/*
 * Add a nesting relationship between two classes.
 */
static int AddNested(ILClass *parent, ILClass *child)
{
	ILImage *image = parent->programItem.image;
	ILNestedInfo *nested;
	ILNestedInfo *last;
	ILToken token;
	int imageType;

	/* Allocate and initialize a new nesting block */
	nested = ILMemStackAlloc(&(image->memStack), ILNestedInfo);
	if(!nested)
	{
		return 0;
	}
	nested->programItem.image = image;
	nested->child = child;
	nested->parent = parent;

	/* Set the token code for the nesting clause.  We use
	   the pseudo type "NESTED_INFO" because we will be
	   creating tokens for nested TypeRef's and TypeDef's,
	   but "NESTED_CLASS" is only used for TypeDef's */
	token = (IL_META_TOKEN_NESTED_INFO |
				(image->tokenCount[IL_META_TOKEN_NESTED_INFO >> 24] + 1));
	imageType = image->type;
	image->type = IL_IMAGETYPE_BUILDING;
	if(!_ILImageSetToken(image, &(nested->programItem),
						 token, IL_META_TOKEN_NESTED_INFO))
	{
		image->type = imageType;
		return 0;
	}
	image->type = imageType;

	/* Add the block to the end of the parent nesting list */
	nested->next = 0;
	if(parent->nestedChildren)
	{
		last = parent->nestedChildren;
		while(last->next != 0)
		{
			last = last->next;
		}
		last->next = nested;
	}
	else
	{
		parent->nestedChildren = nested;
	}

	/* Done */
	return 1;
}

/*
 * Move a nested class to the end of its parent's nesting list.
 */
static void MoveNestedToEnd(ILClass *parent, ILClass *info)
{
	ILNestedInfo *nested;
	ILNestedInfo *temp;

	/* Remove the child from its current position in the list */
	nested = parent->nestedChildren;
	if(nested->child == info)
	{
		parent->nestedChildren = nested->next;
	}
	else
	{
		while(nested->next->child != info)
		{
			nested = nested->next;
		}
		temp = nested->next;
		nested->next = temp->next;
		nested = temp;
	}

	/* Move the nesting information block to the end of the list */
	nested->next = 0;
	if(parent->nestedChildren)
	{
		temp = parent->nestedChildren;
		while(temp->next != 0)
		{
			temp = temp->next;
		}
		temp->next = nested;
	}
	else
	{
		parent->nestedChildren = nested;
	}
}

/*
 * Create a new class information block, which is associated
 * with an image.
 */
static ILClass *CreateClass(ILImage *image, const char *name,
							const char *namespace, ILClass *parent,
							ILProgramItem *scope)
{
	ILClass *info;
	int hash;

	/* Allocate space for the class information block */
	info = ILMemStackAlloc(&(image->memStack), ILClass);
	if(!info)
	{
		return 0;
	}

	/* Initialize the class fields */
	info->programItem.image = image;
	info->parent = parent;
	info->scope = scope;

	/* Create a nesting relationship with the scope if necessary */
	if(ILClassIsNestingScope(scope))
	{
		if(!AddNested((ILClass *)scope, info))
		{
			return 0;
		}
	}

	/* Create persistent versions of the name and namespace */
	info->name = _ILContextPersistString(image, name);
	if(!(info->name))
	{
		return 0;
	}
	if(namespace)
	{
		info->namespace = _ILContextPersistString(image, namespace);
		if(!(info->namespace))
		{
			return 0;
		}
	}

	/* Add the class to the context's hash table */
	hash = HashClassName(info->name, info->namespace);
	info->nextHash = image->context->classHash[hash];
	image->context->classHash[hash] = info;

	/* Done */
	return info;
}

ILClass *ILClassCreate(ILProgramItem *scope, ILToken token, const char *name,
					   const char *namespace, ILClass *parent)
{
	ILImage *image = scope->image;
	ILClass *info;

	/* Import the parent class into this image */
	if(parent)
	{
		parent = ILClassImport(image, parent);
		if(!parent)
		{
			return 0;
		}
	}

	/* Look up the class in the current image */
	info = ILClassLookup(scope, name, namespace);
	if(info)
	{
		if((info->attributes & IL_META_TYPEDEF_REFERENCE) != 0)
		{
			/* Convert the reference into a normal class */
			info->attributes &= ~IL_META_TYPEDEF_REFERENCE;

			/* Attach the class to its new parent */
			info->parent = parent;

			/* There are no interfaces on the class at present */
			info->implements = 0;

			/* Set the token code for the class */
			if(!_ILImageSetToken(image, &(info->programItem),
								 token, IL_META_TOKEN_TYPE_DEF))
			{
				return 0;
			}

			/* If this is a nested class, then move it to the
			   end of the nesting list so that the fields and
			   methods will appear in the correct order in the
			   final output file */
			if(ILClassIsNestingScope(info->scope))
			{
				MoveNestedToEnd((ILClass *)(info->scope), info);
			}

			/* Return the class to the caller */
			return info;
		}
		else
		{
			/* The class already exists, so we cannot recreate it */
			return 0;
		}
	}

	/* Create a new class information block */
	info = CreateClass(image, name, namespace, parent, scope);
	if(info)
	{
		/* Set the token code for the class */
		if(!_ILImageSetToken(image, &(info->programItem),
							 token, IL_META_TOKEN_TYPE_DEF))
		{
			return 0;
		}
	}
	return info;
}

ILClass *ILClassCreateRef(ILProgramItem *scope, ILToken token,
						  const char *name, const char *namespace)
{
	ILClass *info;

	/* Create a new class information block */
	info = CreateClass(scope->image, name, namespace, 0, scope);
	if(!info)
	{
		return 0;
	}

	/* Mark the class as a reference */
	info->attributes |= IL_META_TYPEDEF_REFERENCE;

	/* Associate the class with a TypeRef token */
	if(token != 0 || scope->image->type == IL_IMAGETYPE_BUILDING)
	{
		if(!_ILImageSetToken(scope->image, &(info->programItem),
							 token, IL_META_TOKEN_TYPE_REF))
		{
			return 0;
		}
	}

	/* Ready to go */
	return info;
}

ILProgramItem *ILClassGlobalScope(ILImage *image)
{
	/* The global scope is the first module definition record */
	if(image->tokenCount[IL_META_TOKEN_MODULE >> 24] > 0)
	{
		return (ILProgramItem *)ILImageTokenInfo
					(image, (IL_META_TOKEN_MODULE | 1));
	}
	else
	{
		return 0;
	}
}

int ILClassIsNestingScope(ILProgramItem *scope)
{
	if((scope->token & IL_META_TOKEN_MASK) == IL_META_TOKEN_TYPE_REF ||
	   (scope->token & IL_META_TOKEN_MASK) == IL_META_TOKEN_TYPE_DEF)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

ILClass *ILClassImport(ILImage *image, ILClass *info)
{
	ILClass *newInfo;
	ILProgramItem *scope;
	int hash;

	/* Nothing to do if the class is already in the same image */
	if(info->programItem.image == image)
	{
		return info;
	}

	/* Determine the scope to use for the import */
	if(ILClassIsNestingScope(info->scope))
	{
		/* Importing a nested class, so import its parent first */
		newInfo = ILClassImport(image, (ILClass *)(info->scope));
		if(!newInfo)
		{
			return 0;
		}
		scope = (ILProgramItem *)newInfo;
	}
	else
	{
		/* Importing a global class from another assembly */
		scope = (ILProgramItem *)ILAssemblyCreateImport
						(image, info->programItem.image);
		if(!scope)
		{
			return 0;
		}
	}

	/* See if we already have a reference to this class */
	hash = HashClassName(info->name, info->namespace);
	newInfo = image->context->classHash[hash];
	while(newInfo != 0)
	{
		if(!strcmp(info->name, newInfo->name) &&
		   ((info->namespace && newInfo->namespace &&
		     !strcmp(info->namespace, newInfo->namespace)) ||
		    (!(info->namespace) && !(newInfo->namespace))) &&
		   newInfo->programItem.image == image &&
		   newInfo->scope == scope)
		{
			/* Link "newInfo" to "info" */
			if(!_ILProgramItemLinkedTo(&(newInfo->programItem)))
			{
				if(!_ILProgramItemLink(&(newInfo->programItem),
									   &(info->programItem)))
				{
					return 0;
				}
			}
			return newInfo;
		}
		newInfo = newInfo->nextHash;
	}

	/* Create a new reference */
	newInfo = ILClassCreateRef(scope, 0, info->name, info->namespace);
	if(!newInfo)
	{
		return 0;
	}

	/* Link the reference to the imported class */
	if(!_ILProgramItemLink(&(newInfo->programItem), &(info->programItem)))
	{
		return 0;
	}

	/* Ready to go */
	return newInfo;
}

int ILClassIsRef(ILClass *info)
{
	return ((info->attributes & IL_META_TYPEDEF_REFERENCE) != 0);
}

ILClass *ILClassGetParent(ILClass *info)
{
	if(info->parent)
	{
		return (ILClass *)(_ILProgramItemResolve
								(&(info->parent->programItem)));
	}
	else
	{
		return 0;
	}
}

ILClass *ILClassGetParentRef(ILClass *info)
{
	if(info->parent)
	{
		return (ILClass *)(_ILProgramItemResolveRef
								(&(info->parent->programItem)));
	}
	else
	{
		return 0;
	}
}

ILUInt32 ILClassGetAttrs(ILClass *info)
{
	return (info->attributes & ~IL_META_TYPEDEF_SYSTEM_MASK);
}

void ILClassSetAttrs(ILClass *info, ILUInt32 mask, ILUInt32 values)
{
	info->attributes = ((info->attributes & ~mask) | values);
}

ILProgramItem *ILClassGetScope(ILClass *info)
{
	return info->scope;
}

const char *ILClassGetName(ILClass *info)
{
	return info->name;
}

const char *ILClassGetNamespace(ILClass *info)
{
	return info->namespace;
}

ILType *ILClassGetSynType(ILClass *info)
{
	return info->synthetic;
}

ILImage *ILClassToImage(ILClass *info)
{
	return info->programItem.image;
}

ILContext *ILClassToContext(ILClass *info)
{
	return info->programItem.image->context;
}

void _ILClassRemoveAllFromHash(ILImage *image)
{
	int hash;
	ILClass *info, *prevInfo, *nextInfo;
	ILContext *context = image->context;

	/* Destroy all classes that belong to the image */
	for(hash = 0; hash < IL_CONTEXT_HASH_SIZE; ++hash)
	{
		info = context->classHash[hash];
		prevInfo = 0;
		while(info != 0)
		{
			if(info->programItem.image == image)
			{
				nextInfo = info->nextHash;
				if(prevInfo)
				{
					prevInfo->nextHash = nextInfo;
				}
				else
				{
					context->classHash[hash] = nextInfo;
				}
				info = nextInfo;
			}
			else
			{
				prevInfo = info;
				info = info->nextHash;
			}
		}
	}
}

int ILClassIsValid(ILClass *info)
{
	ILImplements *implements;
	info = (ILClass *)(_ILProgramItemResolve(&(info->programItem)));
	while(info != 0)
	{
		if((info->attributes & IL_META_TYPEDEF_REFERENCE) != 0)
		{
			return 0;
		}
		implements = info->implements;
		while(implements != 0)
		{
			if(!ILClassIsValid(implements->interface))
			{
				return 0;
			}
			implements = implements->nextInterface;
		}
		info = info->parent;
		if(info != 0)
		{
			info = (ILClass *)(_ILProgramItemResolve(&(info->programItem)));
		}
	}
	return 1;
}

ILClass *ILClassLookup(ILProgramItem *scope,
					   const char *name, const char *namespace)
{
	ILContext *context = scope->image->context;
	int hash = HashClassName(name, namespace);
	ILClass *info = context->classHash[hash];
	while(info != 0)
	{
		if(namespace)
		{
			if(info->namespace)
			{
				if(!strcmp(info->name, name) &&
				   !strcmp(info->namespace, namespace))
				{
					if(info->scope == scope)
					{
						return info;
					}
				}
			}
		}
		else if(!strcmp(info->name, name) && !(info->namespace))
		{
			if(info->scope == scope)
			{
				return info;
			}
		}
		info = info->nextHash;
	}
	return 0;
}

ILClass *ILClassLookupGlobal(ILContext *context,
					   		 const char *name, const char *namespace)
{
	int hash = HashClassName(name, namespace);
	ILClass *info = context->classHash[hash];
	while(info != 0)
	{
		if(namespace)
		{
			if(info->namespace)
			{
				if(!strcmp(info->name, name) &&
				   !strcmp(info->namespace, namespace))
				{
					if((info->scope->token & IL_META_TOKEN_MASK)
								== IL_META_TOKEN_MODULE)
					{
						return info;
					}
				}
			}
		}
		else if(!strcmp(info->name, name) && !(info->namespace))
		{
			if((info->scope->token & IL_META_TOKEN_MASK)
						== IL_META_TOKEN_MODULE)
			{
				return info;
			}
		}
		info = info->nextHash;
	}
	return 0;
}

ILImplements *ILClassAddImplements(ILClass *info, ILClass *interface,
								   ILToken token)
{
	ILImplements *impl;
	ILImplements *current, *last;

	/* Bail out if the parameters are invalid */
	if(!info || !interface)
	{
		return 0;
	}

	/* Import the interface into the class's image */
	interface = ILClassImport(info->programItem.image, interface);
	if(!interface)
	{
		return 0;
	}

	/* Ignore the request if the interface is already on the list */
	current = info->implements;
	last = 0;
	while(current != 0)
	{
		if(current->interface == interface)
		{
			return current;
		}
		last = current;
		current = current->nextInterface;
	}

	/* Allocate space for the implements clause and fill it in */
	impl = ILMemStackAlloc(&(info->programItem.image->memStack), ILImplements);
	if(!impl)
	{
		return 0;
	}
	impl->programItem.image = info->programItem.image;
	impl->implement = info;
	impl->interface = interface;

	/* Set the token code for the implements clause */
	if(!_ILImageSetToken(info->programItem.image, &(impl->programItem),
						 token, IL_META_TOKEN_INTERFACE_IMPL))
	{
		return 0;
	}

	/* Add the implements clause to the class's interface list */
	impl->nextInterface = 0;
	if(last)
	{
		last->nextInterface = impl;
	}
	else
	{
		info->implements = impl;
	}

	/* Done */
	return impl;
}

int ILClassInheritsFrom(ILClass *info, ILClass *ancestor)
{
	/* Make sure that we have the fully resolved version of the ancestor */
	if(!ancestor)
	{
		return 0;
	}
	ancestor = (ILClass *)(_ILProgramItemResolve(&(ancestor->programItem)));

	/* Search for the ancestor */
	info = (ILClass *)(_ILProgramItemResolve(&(info->programItem)));
	while(info != 0)
	{
		if(info == ancestor)
		{
			return 1;
		}
		info = info->parent;
		if(info)
		{
			info = (ILClass *)(_ILProgramItemResolve(&(info->programItem)));
		}
	}
	return 0;
}

int ILClassImplements(ILClass *info, ILClass *interface)
{
	ILImplements *temp;
	info = (ILClass *)(_ILProgramItemResolve(&(info->programItem)));
	while(info != 0)
	{
		temp = info->implements;
		while(temp != 0)
		{
			if(ILClassInheritsFrom(temp->interface, interface))
			{
				return 1;
			}
			temp = temp->nextInterface;
		}
		info = info->parent;
		if(info)
		{
			info = (ILClass *)(_ILProgramItemResolve(&(info->programItem)));
		}
	}
	return 0;
}

ILImplements *ILClassNextImplements(ILClass *info, ILImplements *last)
{
	if(last)
	{
		return last->nextInterface;
	}
	else
	{
		return info->implements;
	}
}

ILClass *ILImplementsGetClass(ILImplements *impl)
{
	return impl->implement;
}

ILClass *ILImplementsGetInterface(ILImplements *impl)
{
	return impl->interface;
}

int ILClassIsNested(ILClass *parent, ILClass *child)
{
	ILClass *temp = (child ? ILClassGetNestedParent(child) : 0);
	while(temp != 0)
	{
		if(temp == parent)
		{
			return 1;
		}
		temp = ILClassGetNestedParent(temp);
	}
	return 0;
}

int ILClassCanNest(ILClass *parent, ILClass *child)
{
	/* If the parent and child are not in the same image, then cannot nest */
	if(!parent || !child ||
	   parent->programItem.image != child->programItem.image)
	{
		return 0;
	}

	/* The child must not already be nested */
	if(ILClassGetNestedParent(child) != 0)
	{
		return 0;
	}

	/* If the parent is nested, then it must not be nested
	   either directly or indirectly within the child */
	if(ILClassIsNested(child, parent))
	{
		return 0;
	}

	/* The parent must not inherit the child */
	if(ILClassInheritsFrom(parent, child))
	{
		return 0;
	}

	/* The parent must not implement the child */
	if(ILClassImplements(parent, child))
	{
		return 0;
	}

	/* The child must have one of the "nested" visibility values */
	if(!ILClass_IsNestedPublic(child) &&
	   !ILClass_IsNestedPrivate(child) &&
	   !ILClass_IsNestedFamily(child) &&
	   !ILClass_IsNestedAssembly(child) &&
	   !ILClass_IsNestedFamAndAssem(child) &&
	   !ILClass_IsNestedFamOrAssem(child))
	{
		return 0;
	}

	/* Nesting can occur */
	return 1;
}

ILClass *ILClassGetNestedParent(ILClass *info)
{
	if(ILClassIsNestingScope(info->scope))
	{
		return (ILClass *)(info->scope);
	}
	else
	{
		return 0;
	}
}

ILNestedInfo *ILClassNextNested(ILClass *info, ILNestedInfo *last)
{
	if(last)
	{
		return last->next;
	}
	else
	{
		return info->nestedChildren;
	}
}

ILClass *ILNestedInfoGetParent(ILNestedInfo *nested)
{
	return nested->parent;
}

ILClass *ILNestedInfoGetChild(ILNestedInfo *nested)
{
	return nested->child;
}

static int AccessibleNestedPrivate(ILClass *info, ILClass *scope)
{
	if(!scope)
	{
		return 0;
	}
	if(info->scope == (ILProgramItem *)scope)
	{
		return 1;
	}
	if(ILClassIsNested(ILClassGetNestedParent(info), scope))
	{
		return 1;
	}
	return 0;
}

static int AccessibleNestedFamily(ILClass *info, ILClass *scope)
{
	if(AccessibleNestedPrivate(info, scope))
	{
		return 1;
	}
	info = ILClassGetNestedParent(info);
	while(scope != 0)
	{
		if(ILClassInheritsFrom(scope, info))
		{
			return 1;
		}
		scope = ILClassGetNestedParent(scope);
	}
	return 0;
}

int ILClassAccessible(ILClass *info, ILClass *scope)
{
	if(!info)
	{
		return 0;
	}
	info = (ILClass *)(_ILProgramItemResolve(&(info->programItem)));
	if(scope)
	{
		scope = (ILClass *)(_ILProgramItemResolve(&(scope->programItem)));
	}
	if(info == scope)
	{
		/* A class is always accessible to itself */
		return 1;
	}
	switch(info->attributes & IL_META_TYPEDEF_VISIBILITY_MASK)
	{
		case IL_META_TYPEDEF_NOT_PUBLIC:
		case IL_META_TYPEDEF_NESTED_ASSEMBLY:
		{
			/* The class must be in the same assembly as the scope */
			if(scope && ILClassToImage(info) == ILClassToImage(scope))
			{
				return 1;
			}
		}
		break;

		case IL_META_TYPEDEF_PUBLIC:
		{
			return 1;
		}
		/* Not reached */

		case IL_META_TYPEDEF_NESTED_PUBLIC:
		{
			/* The parent must be accessible */
			return ILClassAccessible(ILClassGetNestedParent(info), scope);
		}
		/* Not reached */

		case IL_META_TYPEDEF_NESTED_PRIVATE:
		{
			/* Nested class accessible from parent, siblings, or children */
			return AccessibleNestedPrivate(info, scope);
		}
		/* Not reached */

		case IL_META_TYPEDEF_NESTED_FAMILY:
		{
			/* Accessible to private or inherited scopes */
			return AccessibleNestedFamily(info, scope);
		}
		/* Not reached */

		case IL_META_TYPEDEF_NESTED_FAM_AND_ASSEM:
		{
			/* Must have both family access and be in the same assembly */
			if(!AccessibleNestedFamily(info, scope))
			{
				return 0;
			}
			if(scope && ILClassToImage(info) == ILClassToImage(scope))
			{
				return 1;
			}
		}
		break;

		case IL_META_TYPEDEF_NESTED_FAM_OR_ASSEM:
		{
			/* Must have family access or be in the same assembly */
			if(AccessibleNestedFamily(info, scope))
			{
				return 1;
			}
			if(scope && ILClassToImage(info) == ILClassToImage(scope))
			{
				return 1;
			}
		}
		break;
	}
	return 0;
}

ILMember *ILClassNextMember(ILClass *info, ILMember *last)
{
	if(last)
	{
		return last->nextMember;
	}
	else if(info)
	{
		return info->firstMember;
	}
	else
	{
		return 0;
	}
}

ILMember *ILClassNextMemberByKind(ILClass *info, ILMember *last, int kind)
{
	while((last = ILClassNextMember(info, last)) != 0)
	{
		if(((int)(last->kind)) == kind)
		{
			break;
		}
	}
	return last;
}

void ILClassMarkComplete(ILClass *info)
{
	info->attributes |= IL_META_TYPEDEF_COMPLETE;
}

int ILClassIsComplete(ILClass *info)
{
	return ((info->attributes & IL_META_TYPEDEF_COMPLETE) != 0);
}

static void FindLowestTokens(ILClass *info, ILToken *lowestField,
							 ILToken *lowestMethod, ILToken *lowestParam)
{
	ILMember *member;
	ILNestedInfo *nested;
	ILParameter *param;

	/* Scan the members of this class */
	member = info->firstMember;
	while(member != 0)
	{
		if(member->kind == IL_META_MEMBERKIND_METHOD)
		{
			if(member->programItem.token < *lowestMethod)
			{
				*lowestMethod = member->programItem.token;
			}
			param = ((ILMethod *)member)->parameters;
			while(param != 0)
			{
				if(param->programItem.token < *lowestParam)
				{
					*lowestParam = param->programItem.token;
				}
				param = param->next;
			}
		}
		else if(member->kind == IL_META_MEMBERKIND_FIELD)
		{
			if(member->programItem.token < *lowestField)
			{
				*lowestField = member->programItem.token;
			}
		}
		member = member->nextMember;
	}

	/* Scan the nested classes for token codes */
	nested = info->nestedChildren;
	while(nested != 0)
	{
		FindLowestTokens(nested->child, lowestField, lowestMethod, lowestParam);
		nested = nested->next;
	}
}

static void AssignMemberTokens(ILClass *info, ILToken *nextField,
							   ILToken *nextMethod, ILToken *nextParam)
{
	ILMember *member;
	ILNestedInfo *nested;
	ILParameter *param;

	/* Scan the members of this class */
	member = info->firstMember;
	while(member != 0)
	{
		if(member->kind == IL_META_MEMBERKIND_METHOD)
		{
			member->programItem.token = *nextMethod;
			member->programItem.image->tokenData
				[IL_META_TOKEN_METHOD_DEF >> 24]
				[(*nextMethod & ~IL_META_TOKEN_MASK) - 1] = (void *)member;
			*nextMethod += 1;
			param = ((ILMethod *)member)->parameters;
			while(param != 0)
			{
				param->programItem.token = *nextParam;
				member->programItem.image->tokenData
					[IL_META_TOKEN_PARAM_DEF >> 24]
					[(*nextParam & ~IL_META_TOKEN_MASK) - 1] = (void *)param;
				*nextParam += 1;
				param = param->next;
			}
		}
		else if(member->kind == IL_META_MEMBERKIND_FIELD)
		{
			member->programItem.token = *nextField;
			member->programItem.image->tokenData
				[IL_META_TOKEN_FIELD_DEF >> 24]
				[(*nextField & ~IL_META_TOKEN_MASK) - 1] = (void *)member;
			*nextField += 1;
		}
		member = member->nextMember;
	}

	/* Scan the nested classes for token codes */
	nested = info->nestedChildren;
	while(nested != 0)
	{
		AssignMemberTokens(nested->child, nextField, nextMethod, nextParam);
		nested = nested->next;
	}
}

void ILClassSortMembers(ILClass *info)
{
	ILToken lowestField;
	ILToken lowestMethod;
	ILToken lowestParam;

	/* Find the lowest field, method, and parameter token
	   codes across the entire nested class hierarchy */
	lowestField = IL_MAX_UINT32;
	lowestMethod = IL_MAX_UINT32;
	lowestParam = IL_MAX_UINT32;
	FindLowestTokens(info, &lowestField, &lowestMethod, &lowestParam);

	/* Assign new token codes to all fields and methods */
	AssignMemberTokens(info, &lowestField, &lowestMethod, &lowestParam);
}

#ifdef	__cplusplus
};
#endif
