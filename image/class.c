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
	info->ext = 0;

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
	if(!ILHashAdd(image->context->classHash, info))
	{
		return 0;
	}

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

ILClass *ILClassResolve(ILClass *info)
{
	return (ILClass *)(_ILProgramItemResolve(&(info->programItem)));
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
	ILClassKeyInfo key;

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
	key.name = info->name;
	key.nameLen = strlen(info->name);
	key.namespace = info->namespace;
	key.namespaceLen = (info->namespace ? strlen(info->namespace) : 0);
	key.scope = scope;
	key.image = image;
	key.wantGlobal = 0;
	newInfo = ILHashFindType(image->context->classHash, &key, ILClass);
	if(newInfo != 0)
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

/*
 * Matching function that is used by "_ILClassRemoveAllFromHash".
 */
static int ClassRemove_Match(const ILClass *classInfo, ILImage *key)
{
	return (classInfo->programItem.image == key);
}

void _ILClassRemoveAllFromHash(ILImage *image)
{
	ILHashRemoveSubset(image->context->classHash,
					   (ILHashMatchFunc)ClassRemove_Match, image, 0);
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
	ILClassKeyInfo key;
	key.name = name;
	key.nameLen = strlen(name);
	key.namespace = namespace;
	key.namespaceLen = (namespace ? strlen(namespace) : 0);
	key.scope = scope;
	key.image = 0;
	key.wantGlobal = 0;
	key.ignoreCase = 0;
	return ILHashFindType(scope->image->context->classHash, &key, ILClass);
}

ILClass *ILClassLookupLen(ILProgramItem *scope,
					      const char *name, int nameLen,
						  const char *namespace, int namespaceLen)
{
	ILClassKeyInfo key;
	key.name = name;
	key.nameLen = nameLen;
	key.namespace = namespace;
	key.namespaceLen = namespaceLen;
	key.scope = scope;
	key.image = 0;
	key.wantGlobal = 0;
	key.ignoreCase = 0;
	return ILHashFindType(scope->image->context->classHash, &key, ILClass);
}

/*
 * Hash a unicode string, while ignoring case.
 */
static unsigned long HashUnicode(unsigned long start,
								 const ILUInt16 *str, int len)
{
	unsigned long hash = start;
	unsigned ch;
	while(len > 0)
	{
		ch = *str++;
		if(ch >= 'A' && ch <= 'Z')
		{
			hash = (hash << 5) + hash + (unsigned long)(ch - 'A' + 'a');
		}
		else
		{
			hash = (hash << 5) + hash + (unsigned long)ch;
		}
		--len;
	}
	return hash;
}

/*
 * Compute the hash value for a class key.
 */
static unsigned long UnicodeHashKey(const ILClassKeyInfo *key)
{
	unsigned long hash;
	if(key->namespace)
	{
		hash = HashUnicode(0, (const ILUInt16 *)key->namespace,
						   key->namespaceLen);
		hash = (hash << 5) + hash + (ILUInt16)'.';
	}
	else
	{
		hash = 0;
	}
	return HashUnicode(hash, (const ILUInt16 *)key->name,
					   key->nameLen);
}

/*
 * Compare a regular string against a Unicode string.
 */
static int CompareUnicode(const char *str,
						  const ILUInt16 *str2,
						  int str2Len, int ignoreCase)
{
	int ch;
	if(ignoreCase)
	{
		while(*str != '\0' && str2Len > 0)
		{
			ch = (*str++ & 0xFF);
			if(ch >= 'A' && ch <= 'Z')
			{
				ch = ch - 'A' + 'a';
			}
			if(ch != (int)(*str2++))
			{
				return 0;
			}
			--str2Len;
		}
		return (*str == '\0' && str2Len == 0);
	}
	else
	{
		while(*str != '\0' && str2Len > 0)
		{
			ch = (*str++ & 0xFF);
			if(ch != (int)(*str2++))
			{
				return 0;
			}
			--str2Len;
		}
		return (*str == '\0' && str2Len == 0);
	}
}

/*
 * Match a hash table element against a supplied key.
 */
static int UnicodeMatch(const ILClass *classInfo, const ILClassKeyInfo *key)
{
	/* Match the namespace */
	if(classInfo->namespace)
	{
		if(!(key->namespace))
		{
			return 0;
		}
		if(!CompareUnicode(classInfo->namespace,
						   (const ILUInt16 *)(key->namespace),
						   key->namespaceLen, key->ignoreCase))
		{
			return 0;
		}
	}

	/* Match the name */
	if(!CompareUnicode(classInfo->name, (const ILUInt16 *)(key->name),
					   key->nameLen, key->ignoreCase))
	{
		return 0;
	}

	/* Match the scope */
	if(key->scope && key->scope != classInfo->scope)
	{
		return 0;
	}

	/* Match the image */
	if(key->image && key->image != classInfo->programItem.image)
	{
		return 0;
	}

	/* Do we only want types at the global level? */
	if(key->wantGlobal)
	{
		if((classInfo->scope->token & IL_META_TOKEN_MASK) !=
					IL_META_TOKEN_MODULE)
		{
			return 0;
		}
	}

	/* We have a match */
	return 1;
}

ILClass *ILClassLookupUnicode(ILProgramItem *scope,
					          const ILUInt16 *name, int nameLen,
						  	  const ILUInt16 *namespace, int namespaceLen,
							  int ignoreCase)
{
	ILClassKeyInfo key;
	key.name = (const char *)name;
	key.nameLen = nameLen;
	key.namespace = (const char *)namespace;
	key.namespaceLen = namespaceLen;
	key.scope = scope;
	key.image = 0;
	key.wantGlobal = 0;
	key.ignoreCase = ignoreCase;
	return ILHashFindAltType(scope->image->context->classHash, &key, ILClass,
							 (ILHashKeyComputeFunc)UnicodeHashKey,
							 (ILHashMatchFunc)UnicodeMatch);
}

ILClass *ILClassLookupGlobal(ILContext *context,
					   		 const char *name, const char *namespace)
{
	ILClassKeyInfo key;
	key.name = name;
	key.nameLen = strlen(name);
	key.namespace = namespace;
	key.namespaceLen = (namespace ? strlen(namespace) : 0);
	key.scope = 0;
	key.image = 0;
	key.wantGlobal = 1;
	key.ignoreCase = 0;
	return ILHashFindType(context->classHash, &key, ILClass);
}

ILClass *ILClassLookupGlobalLen(ILContext *context,
							    const char *name, int nameLen,
								const char *namespace, int namespaceLen)
{
	ILClassKeyInfo key;
	key.name = name;
	key.nameLen = nameLen;
	key.namespace = namespace;
	key.namespaceLen = namespaceLen;
	key.scope = 0;
	key.image = 0;
	key.wantGlobal = 1;
	key.ignoreCase = 0;
	return ILHashFindType(context->classHash, &key, ILClass);
}

ILClass *ILClassLookupGlobalUnicode(ILContext *context,
					                const ILUInt16 *name, int nameLen,
						  	        const ILUInt16 *namespace, int namespaceLen,
							        int ignoreCase)
{
	ILClassKeyInfo key;
	key.name = (const char *)name;
	key.nameLen = nameLen;
	key.namespace = (const char *)namespace;
	key.namespaceLen = namespaceLen;
	key.scope = 0;
	key.image = 0;
	key.wantGlobal = 1;
	key.ignoreCase = ignoreCase;
	return ILHashFindAltType(context->classHash, &key, ILClass,
							 (ILHashKeyComputeFunc)UnicodeHashKey,
							 (ILHashMatchFunc)UnicodeMatch);
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

static int ImplementsResolved(ILClass *info, ILClass *interface)
{
	ILImplements *temp;
	ILClass *tempInterface;
	while(info != 0)
	{
		temp = info->implements;
		while(temp != 0)
		{
			tempInterface = (ILClass *)(_ILProgramItemResolve
								(&(temp->interface->programItem)));
			if(tempInterface == interface)
			{
				return 1;
			}
			else if(ImplementsResolved(tempInterface, interface))
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

int ILClassImplements(ILClass *info, ILClass *interface)
{
	info = (ILClass *)(_ILProgramItemResolve(&(info->programItem)));
	interface = (ILClass *)(_ILProgramItemResolve(&(interface->programItem)));
	return ImplementsResolved(info, interface);
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

static int InheritsFromValueType(ILClass *info)
{
	const char *namespace;
	while(info != 0)
	{
		info = ILClassResolve(info);
		if(!strcmp(ILClass_Name(info), "ValueType"))
		{
			namespace = ILClass_Namespace(info);
			if(!strcmp(namespace, "System") && !ILClass_NestedParent(info))
			{
				return 1;
			}
		}
		info = ILClass_ParentRef(info);
	}
	return 0;
}

int ILClassIsValueType(ILClass *info)
{
	ILClass *newInfo = ILClassResolve(info);
	if((newInfo->attributes & IL_META_TYPEDEF_CLASS_SEMANTICS_MASK) ==
				IL_META_TYPEDEF_VALUE_TYPE ||
	   (newInfo->attributes & IL_META_TYPEDEF_CLASS_SEMANTICS_MASK) ==
				IL_META_TYPEDEF_UNMANAGED_VALUE_TYPE ||
	   ((newInfo->attributes & IL_META_TYPEDEF_SEALED) != 0 &&
	    InheritsFromValueType(newInfo)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

ILType *ILClassToType(ILClass *info)
{
	/* If the class has a synthetic type, then return that */
	if(info->synthetic)
	{
		return info->synthetic;
	}

	/* Check for system classes with primitive equivalents */
	if(info->namespace && !strcmp(info->namespace, "System") &&
	   ILClassGetNestedParent(info) == 0)
	{
		if(!strcmp(info->name, "Boolean"))
		{
			return ILType_Boolean;
		}
		else if(!strcmp(info->name, "SByte"))
		{
			return ILType_Int8;
		}
		else if(!strcmp(info->name, "Byte"))
		{
			return ILType_UInt8;
		}
		else if(!strcmp(info->name, "Int16"))
		{
			return ILType_Int16;
		}
		else if(!strcmp(info->name, "UInt16"))
		{
			return ILType_UInt16;
		}
		else if(!strcmp(info->name, "Char"))
		{
			return ILType_Char;
		}
		else if(!strcmp(info->name, "Int32"))
		{
			return ILType_Int32;
		}
		else if(!strcmp(info->name, "UInt32"))
		{
			return ILType_UInt32;
		}
		else if(!strcmp(info->name, "Int64"))
		{
			return ILType_Int64;
		}
		else if(!strcmp(info->name, "UInt64"))
		{
			return ILType_UInt64;
		}
		else if(!strcmp(info->name, "Single"))
		{
			return ILType_Float32;
		}
		else if(!strcmp(info->name, "Double"))
		{
			return ILType_Float64;
		}
		else if(!strcmp(info->name, "IntPtr"))
		{
			return ILType_Int;
		}
		else if(!strcmp(info->name, "UIntPtr"))
		{
			return ILType_UInt;
		}
		else if(!strcmp(info->name, "Void"))
		{
			return ILType_Void;
		}
		else if(!strcmp(info->name, "TypedReference"))
		{
			return ILType_TypedRef;
		}
	}

	/* Convert into either a value type or a class type */
	return ILClassToTypeDirect(info);
}

ILType *ILClassToTypeDirect(ILClass *info)
{
	if(ILClassIsValueType(info))
	{
		return ILType_FromValueType(info);
	}
	else
	{
		return ILType_FromClass(info);
	}
}

ILClass *ILClassFromType(ILImage *image, void *data, ILType *type,
						 ILSystemTypeResolver func)
{
	const char *systemName;
	if(ILType_IsPrimitive(type))
	{
		systemName = 0;
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:
			{
				systemName = "Void";
			}
			break;

			case IL_META_ELEMTYPE_BOOLEAN:
			{
				systemName = "Boolean";
			}
			break;

			case IL_META_ELEMTYPE_I1:
			{
				systemName = "SByte";
			}
			break;

			case IL_META_ELEMTYPE_U1:
			{
				systemName = "Byte";
			}
			break;

			case IL_META_ELEMTYPE_I2:
			{
				systemName = "Int16";
			}
			break;

			case IL_META_ELEMTYPE_U2:
			{
				systemName = "UInt16";
			}
			break;

			case IL_META_ELEMTYPE_CHAR:
			{
				systemName = "Char";
			}
			break;

			case IL_META_ELEMTYPE_I4:
			{
				systemName = "Int32";
			}
			break;

			case IL_META_ELEMTYPE_U4:
			{
				systemName = "UInt32";
			}
			break;

			case IL_META_ELEMTYPE_I8:
			{
				systemName = "Int64";
			}
			break;

			case IL_META_ELEMTYPE_U8:
			{
				systemName = "UInt64";
			}
			break;

			case IL_META_ELEMTYPE_R4:
			{
				systemName = "Single";
			}
			break;

			case IL_META_ELEMTYPE_R8:
			{
				systemName = "Double";
			}
			break;

			case IL_META_ELEMTYPE_I:
			{
				systemName = "IntPtr";
			}
			break;

			case IL_META_ELEMTYPE_U:
			{
				systemName = "UIntPtr";
			}
			break;

			case IL_META_ELEMTYPE_TYPEDBYREF:
			{
				systemName = "TypedReference";
			}
			break;

			case IL_META_ELEMTYPE_STRING:
			{
				systemName = "String";
			}
			break;

			case IL_META_ELEMTYPE_OBJECT:
			{
				systemName = "Object";
			}
			break;

			default: break;
		}
		if(systemName)
		{
			if(func)
			{
				return (*func)(image, data, systemName, "System");
			}
			else
			{
				return ILClassResolveSystem(image, data, systemName, "System");
			}
		}
	}
	else if(ILType_IsValueType(type))
	{
		return ILType_ToValueType(type);
	}
	else if(ILType_IsClass(type))
	{
		return ILType_ToClass(type);
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		/* Recognise complex types */
		if(ILType_IsSimpleArray(type))
		{
			/* Single-dimensional array with no lower bound */
			return _ILTypeToSyntheticArray(image, type, 1);
		}
		else if(ILType_IsArray(type))
		{
			/* Multi-dimensional array or an array with specified bounds */
			return _ILTypeToSyntheticArray(image, type, 0);
		}
	}
	return 0;
}

ILClass *ILClassResolveSystem(ILImage *image, void *data, const char *name,
							  const char *namespace)
{
	ILProgramItem *scope;
	ILClass *classInfo;

	/* If we have a system image, then always use that.
	   This will prevent applications from replacing the
	   system types with their own definitions */
	if(image->context->systemImage)
	{
		return ILClassLookup(ILClassGlobalScope(image->context->systemImage),
							 name, namespace);
	}

	/* Try looking in the image itself */
	scope = ILClassGlobalScope(image);
	if(scope)
	{
		classInfo = ILClassLookup(scope, name, namespace);
		if(classInfo)
		{
			return classInfo;
		}
	}

	/* Look in any image within the same context */
	classInfo = ILClassLookupGlobal(image->context, name, namespace);
	if(classInfo)
	{
		return classInfo;
	}

	/* Create a reference within the current image */
	if(scope)
	{
		return ILClassCreateRef(scope, 0, name, namespace);
	}

	/* Could not resolve the system class */
	return 0;
}

void *ILClassGetUserData(ILClass *info)
{
	return info->userData;
}

void ILClassSetUserData(ILClass *info, void *data)
{
	info->userData = data;
}

ILMethod *ILClassGetMethodImpl(ILClass *info, ILMethod *method)
{
	ILMethod *method2;
	ILMethod *result = 0;
	const char *name;
	ILType *signature;
	ILClass *parent;
	ILOverride *over;

	/* Cache the name and signature information for the interface method */
	name = ILMethod_Name(method);
	signature = ILMethod_Signature(method);

	/* Search for an exact match within the class, looking for
	   a newly declared virtual method in a new slot */
	method2 = 0;
	while((method2 = (ILMethod *)ILClassNextMemberByKind
					(info, (ILMember *)method2,
				 	 IL_META_MEMBERKIND_METHOD)) != 0)
	{
		if((ILMethod_Attrs(method2) &
				(IL_META_METHODDEF_MEMBER_ACCESS_MASK |
				 IL_META_METHODDEF_VIRTUAL |
				 IL_META_METHODDEF_NEW_SLOT)) ==
						(IL_META_METHODDEF_PUBLIC |
				 		 IL_META_METHODDEF_VIRTUAL |
						 IL_META_METHODDEF_NEW_SLOT))
		{
			if(strcmp(ILMethod_Name(method2), name) != 0 ||
			   !ILTypeIdentical(ILMethod_Signature(method2), signature))
			{
				continue;
			}
			result = method2;
			break;
		}
	}

	/* If the result is still unknown, then search the class
	   hierarchy for a normal virtual method match */
	if(!result)
	{
		parent = info;
		while(parent != 0)
		{
			method2 = 0;
			while((method2 = (ILMethod *)ILClassNextMemberByKind
							(parent, (ILMember *)method2,
						 	 IL_META_MEMBERKIND_METHOD)) != 0)
			{
				if((ILMethod_Attrs(method2) &
						(IL_META_METHODDEF_MEMBER_ACCESS_MASK |
						 IL_META_METHODDEF_VIRTUAL)) ==
								(IL_META_METHODDEF_PUBLIC |
						 		 IL_META_METHODDEF_VIRTUAL))
				{
					if(strcmp(ILMethod_Name(method2), name) != 0 ||
					   !ILTypeIdentical(ILMethod_Signature(method2),
					   					signature))
					{
						continue;
					}
					result = method2;
					break;
				}
			}
			if(result)
			{
				break;
			}
			parent = ILClassGetParent(parent);
		}
	}

	/* Look for an override for the interface method */
	parent = info;
	while(parent != 0)
	{
		over = 0;
		while((over = (ILOverride *)ILClassNextMemberByKind
							(parent, (ILMember *)over,
							 IL_META_MEMBERKIND_OVERRIDE)) != 0)
		{
			if(ILMemberResolve((ILMember *)(ILOverrideGetDecl(over)))
					== (ILMember *)method)
			{
				result = ILOverrideGetBody(over);
				goto done;
			}
		}
		if(result && ILMethod_Owner(result) == parent)
		{
			/* We have a non-override method at this level */
			break;
		}
		parent = ILClassGetParent(parent);
	}
done:

	/* Return the method that we found to the caller */
	return result;
}

void ILClassDetachMember(ILMember *member)
{
	ILClass *info = member->owner;
	ILMember *current = info->firstMember;
	ILMember *prev = 0;
	while(current != 0 && current != member)
	{
		prev = current;
		current = current->nextMember;
	}
	if(current != 0)
	{
		if(prev)
		{
			prev->nextMember = member->nextMember;
		}
		else
		{
			info->firstMember = member->nextMember;
		}
		if(member->nextMember == 0)
		{
			info->lastMember = prev;
		}
	}
}

void ILClassAttachMember(ILClass *info, ILMember *member)
{
	member->owner = info;
	if(info->lastMember)
	{
		info->lastMember->nextMember = member;
	}
	else
	{
		info->firstMember = member;
	}
	member->nextMember = 0;
	info->lastMember = member;
}

#ifdef	__cplusplus
};
#endif
