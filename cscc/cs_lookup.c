/*
 * cs_lookup.c - Member lookup routines.
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

#include "cs_internal.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Extra member kinds.
 */
#define	CS_MEMBERKIND_TYPE			20
#define	CS_MEMBERKIND_TYPE_NODE		21
#define	CS_MEMBERKIND_NAMESPACE		22

/*
 * A list of members that results from a lookup on a type.
 */
typedef struct _tagCSMemberInfo CSMemberInfo;
struct _tagCSMemberInfo
{
	ILProgramItem *member;
	ILClass       *owner;
	int			   kind;
	CSMemberInfo  *next;
};
#define	CS_MEMBER_LOOKUP_MAX	4
typedef struct _tagCSMemberLookupInfo CSMemberLookupInfo;
struct _tagCSMemberLookupInfo
{
	int			   num;
	CSMemberInfo  *members;
	CSMemberInfo  *lastMember;

};

/*
 * Iterator control structure for CSMemberLookupInfo.
 */
typedef struct
{
	CSMemberLookupInfo *info;
	CSMemberInfo       *current;
	CSMemberInfo       *last;

} CSMemberLookupIter;

/*
 * Initialize a member results set.
 */
static void InitMembers(CSMemberLookupInfo *results)
{
	results->num = 0;
	results->members = 0;
	results->lastMember = 0;
}

/*
 * Add a member to a results set.
 */
static void AddMember(CSMemberLookupInfo *results,
					  ILProgramItem *member, ILClass *owner,
					  int kind)
{
	CSMemberInfo *info = (CSMemberInfo *)ILMalloc(sizeof(CSMemberInfo));
	if(!info)
	{
		CSOutOfMemory();
	}
	info->member = member;
	info->owner = owner;
	info->kind = kind;
	info->next = 0;
	if(results->lastMember)
	{
		results->lastMember->next = info;
	}
	else
	{
		results->members = info;
	}
	results->lastMember = info;
	++(results->num);
}

/*
 * Free the contents of a member lookup results list.
 */
static void FreeMembers(CSMemberLookupInfo *results)
{
	CSMemberInfo *info, *next;
	info = results->members;
	while(info != 0)
	{
		next = info->next;
		ILFree(info);
		info = next;
	}
	results->num = 0;
	results->members = 0;
	results->lastMember = 0;
}

/*
 * Initialize a member iterator.
 */
static void MemberIterInit(CSMemberLookupIter *iter,
						   CSMemberLookupInfo *results)
{
	iter->info = results;
	iter->current = 0;
	iter->last = 0;
}

/*
 * Get the next item from a member iterator.
 */
static CSMemberInfo *MemberIterNext(CSMemberLookupIter *iter)
{
	if(iter->current)
	{
		iter->last = iter->current;
		iter->current = iter->current->next;
	}
	else
	{
		iter->current = iter->info->members;
		iter->last = 0;
	}
	return iter->current;
}

/*
 * Remove the current item from a member iterator.
 */
static void MemberIterRemove(CSMemberLookupIter *iter)
{
	if(iter->current == iter->info->lastMember)
	{
		iter->info->lastMember = iter->last;
	}
	if(iter->last)
	{
		iter->last->next = iter->current->next;
		ILFree(iter->current);
		iter->current = iter->last;
	}
	else
	{
		iter->info->members = iter->current->next;
		ILFree(iter->current);
		iter->current = 0;
		iter->last = 0;
	}
	--(iter->info->num);
}

/*
 * Process a class and add all members called "name" to a set
 * of member lookup results.
 */
static void FindMembers(ILClass *info, const char *name,
					    ILClass *accessedFrom,
					    CSMemberLookupInfo *results)
{
	ILImplements *impl;
	ILMember *member;
	ILNestedInfo *nested;
	ILClass *nestedChild;
	int kind;

	/* Scan up the parent hierarchy until we run out of parents */
	while(info != 0)
	{
		/* Resolve the class to its actual image */
		info = ILClassResolve(info);

		/* If this is an interface, then scan its base interfaces */
		if(ILClass_IsInterface(info))
		{
			impl = 0;
			while((impl = ILClassNextImplements(info, impl)) != 0)
			{
				FindMembers(ILImplementsGetInterface(impl),
						    name, accessedFrom, results);
			}
		}

		/* Look for all accessible members with the given name */
		member = 0;
		while((member = ILClassNextMember(info, member)) != 0)
		{
			if(!strcmp(ILMember_Name(member), name) &&
			   ILMemberAccessible(member, accessedFrom))
			{
				kind = ILMemberGetKind(member);
				if(kind == IL_META_MEMBERKIND_METHOD &&
				   ILMethod_IsVirtual((ILMethod *)member) &&
				   !ILMethod_IsNewSlot((ILMethod *)member))
				{
					/* This is a virtual override, so skip it */
					continue;
				}
				if(kind != IL_META_MEMBERKIND_METHOD &&
				   kind != IL_META_MEMBERKIND_FIELD &&
				   kind != IL_META_MEMBERKIND_PROPERTY &&
				   kind != IL_META_MEMBERKIND_EVENT)
				{
					/* This is PInvoke or override, which we don't need */
					continue;
				}
				AddMember(results, (ILProgramItem *)member, info, kind);
			}
		}

		/* Look for all accessible nested classes with the given name */
		nested = 0;
		while((nested = ILClassNextNested(info, nested)) != 0)
		{
			nestedChild = ILNestedInfoGetChild(nested);
			if(!strcmp(ILClass_Name(nestedChild), name) &&
			   ILClassAccessible(nestedChild, accessedFrom))
			{
				AddMember(results, (ILProgramItem *)nestedChild,
						  info, CS_MEMBERKIND_TYPE);
			}
		}

		/* Move up to the parent */
		info = ILClass_ParentRef(info);
	}
}

/*
 * Perform a member lookup on a type.
 */
static int MemberLookup(ILGenInfo *genInfo, ILClass *info, const char *name,
				        ILClass *accessedFrom, CSMemberLookupInfo *results)
{
	CSMemberLookupIter iter;
	CSMemberInfo *firstMember;
	CSMemberInfo *member;

	/* Initialize the results */
	InitMembers(results);

	/* Collect up all members with the specified name */
	if(info)
	{
		FindMembers(info, name, accessedFrom, results);
	}

	/* If the list is empty, then we are done */
	if(!(results->num))
	{
		return CS_SEMKIND_VOID;
	}

	/* Trim the list based on the type of the first member */
	firstMember = results->members;
	if(firstMember->kind == IL_META_MEMBERKIND_METHOD)
	{
		/* This is a method, so remove non-methods from the base types */
		MemberIterInit(&iter, results);
		while((member = MemberIterNext(&iter)) != 0)
		{
			if(member->kind != IL_META_MEMBERKIND_METHOD &&
			   member->owner != firstMember->owner)
			{
				MemberIterRemove(&iter);
			}
		}

		/* Filter the remaining methods by signature */
	}
	else
	{
		/* This is not a method, so remove members from the base types */
		MemberIterInit(&iter, results);
		while((member = MemberIterNext(&iter)) != 0)
		{
			if(member->owner != firstMember->owner)
			{
				MemberIterRemove(&iter);
			}
		}
	}

	/* Determine whether we have a method list, a non-method,
	   or a list of members which is ambiguous */
	if(firstMember->kind == IL_META_MEMBERKIND_METHOD)
	{
		/* All members must be methods, or the list is ambiguous */
		MemberIterInit(&iter, results);
		while((member = MemberIterNext(&iter)) != 0)
		{
			if(member->kind != IL_META_MEMBERKIND_METHOD)
			{
				return CS_SEMKIND_AMBIGUOUS;
			}
		}
		return CS_SEMKIND_METHOD_GROUP;
	}
	else if(results->num == 1)
	{
		/* Singleton list with a non-method */
		switch(firstMember->kind)
		{
			case CS_MEMBERKIND_TYPE:
			{
				return CS_SEMKIND_TYPE;
			}
			/* Not reached */

			case IL_META_MEMBERKIND_FIELD:
			{
				return CS_SEMKIND_FIELD;
			}
			/* Not reached */

			case IL_META_MEMBERKIND_PROPERTY:
			{
				return CS_SEMKIND_PROPERTY;
			}
			/* Not reached */

			case IL_META_MEMBERKIND_EVENT:
			{
				return CS_SEMKIND_EVENT;
			}
			/* Not reached */
		}
		return CS_SEMKIND_AMBIGUOUS;
	}
	else
	{
		/* The list is ambiguous */
		return CS_SEMKIND_AMBIGUOUS;
	}
}

/*
 * Print an error message that describes an ambiguous name.
 */
static void AmbiguousError(ILNode *node, const char *name,
						   CSMemberLookupInfo *results)
{
	CSMemberLookupIter iter;
	CSMemberInfo *member;
	const char *typeName;
	const char *typeName2;

	/* Print the first line of the error message */
	CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
				  "`%s' is ambiguous; possibilities are:", name);

	/* Print the names and signatures of all members */
	MemberIterInit(&iter, results);
	while((member = MemberIterNext(&iter)) != 0)
	{
		switch(member->kind)
		{
			case CS_MEMBERKIND_TYPE:
			{
				typeName = CSTypeToName
						(ILClassToType((ILClass *)(member->member)));
				CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
				              "  type %s", typeName);
			}
			break;

			case CS_MEMBERKIND_TYPE_NODE:
			{
			}
			break;

			case CS_MEMBERKIND_NAMESPACE:
			{
			}
			break;

			case IL_META_MEMBERKIND_FIELD:
			{
				typeName = CSTypeToName
						(ILField_Type((ILField *)(member->member)));
				typeName2 = CSTypeToName(ILClassToType
						(ILField_Owner((ILField *)(member->member))));
				CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
				              "  field %s %s.%s", typeName, typeName2, name);
			}
			break;

			case IL_META_MEMBERKIND_METHOD:
			{
			}
			break;

			case IL_META_MEMBERKIND_PROPERTY:
			{
			}
			break;

			case IL_META_MEMBERKIND_EVENT:
			{
			}
			break;
		}
	}
}

/*
 * Convert a set of member lookup results into a semantic value.
 */
static CSSemValue LookupToSem(ILNode *node, const char *name,
							  CSMemberLookupInfo *results, int kind)
{
	CSSemValue value;
	if(kind == CS_SEMKIND_METHOD_GROUP)
	{
		/* This is a method group.  Fix later: this will leak memory! */
		value.kind = kind;
		value.type = (ILType *)(results->members);
		return value;
	}
	else if(kind == CS_SEMKIND_TYPE)
	{
		/* This is a type */
		value.kind = kind;
		value.type = ILClassToType((ILClass *)(results->members->member));
		FreeMembers(results);
		return value;
	}
	else if(kind != CS_SEMKIND_AMBIGUOUS)
	{
		/* Type node, field, property, or event */
		value.kind = kind;
		value.type = (ILType *)(results->members->member);
		FreeMembers(results);
		return value;
	}
	else
	{
		/* Ambiguous lookup */
		AmbiguousError(node, name, results);
		FreeMembers(results);
		return CSSemValueDefault;
	}
}

/*
 * Find the type with a specific name within a namespace.
 */
static int FindTypeInNamespace(ILGenInfo *genInfo, const char *name,
							   char *namespace, ILClass *accessedFrom,
							   CSMemberLookupInfo *results)
{
	ILClass *type;
	ILScopeData *data;
	int scopeKind;
	char *fullName;
	ILNode_ClassDefn *node;

	/* Look in the current image for the type */
	type = ILClassLookup(ILClassGlobalScope(genInfo->image),
						 name, namespace);
	if(type)
	{
		type = ILClassResolve(type);
	}
	if(type && ILClassAccessible(type, accessedFrom))
	{
		AddMember(results, (ILProgramItem *)type, 0, CS_MEMBERKIND_TYPE);
		return CS_SEMKIND_TYPE;
	}

	/* Look in any image for the type */
	type = ILClassLookupGlobal(genInfo->context, name, namespace);
	if(type)
	{
		type = ILClassResolve(type);
	}
	if(type && ILClassAccessible(type, accessedFrom))
	{
		AddMember(results, (ILProgramItem *)type, 0, CS_MEMBERKIND_TYPE);
		return CS_SEMKIND_TYPE;
	}

	/* Look in the global scope for a declared type */
	data = ILScopeLookupInNamespace(CSGlobalScope, namespace, name);
	if(data)
	{
		scopeKind = ILScopeDataGetKind(data);
		if(scopeKind == IL_SCOPE_SUBSCOPE)
		{
			if(namespace)
			{
				fullName = ILInternAppendedString
								(ILInternString(namespace, -1),
								 ILInternAppendedString
								 	(ILInternString(".", 1),
									 ILInternString((char *)name, -1))).string;
			}
			else
			{
				fullName = (char *)name;
			}
			AddMember(results, (ILProgramItem *)fullName,
					  0, CS_MEMBERKIND_NAMESPACE);
			return CS_SEMKIND_NAMESPACE;
		}
		else if(scopeKind == IL_SCOPE_DECLARED_TYPE)
		{
			node = (ILNode_ClassDefn *)(ILScopeDataGetNode(data));
			if(!(genInfo->typeGather))
			{
				if(node->classInfo != 0 &&
				   node->classInfo != ((ILClass *)1) &&
				   node->classInfo != ((ILClass *)2))
				{
					AddMember(results, (ILProgramItem *)(node->classInfo),
						      0, CS_MEMBERKIND_TYPE);
					return CS_SEMKIND_TYPE;
				}
			}
			AddMember(results, (ILProgramItem *)node,
				      0, CS_MEMBERKIND_TYPE_NODE);
			return CS_SEMKIND_TYPE_NODE;
		}
	}

	/* Could not find a type or namespace with the specified name */
	return CS_SEMKIND_VOID;
}

CSSemValue CSResolveSimpleName(ILGenInfo *genInfo, ILNode *node,
							   const char *name)
{
	ILClass *startType;
	ILClass *accessedFrom;
	CSMemberLookupInfo results;
	ILNode_Namespace *namespace;
	ILNode_UsingAlias *alias;
	ILNode_UsingNamespace *using;
	CSSemValue value;
	int result;

	/* Find the type to start looking at */
	if(genInfo->currentMethod)
	{
		startType = ILMethod_Owner
		  (((ILNode_MethodDeclaration *)(genInfo->currentMethod))->methodInfo);
	}
	else if(genInfo->currentClass)
	{
		startType = ((ILNode_ClassDefn *)(genInfo->currentClass))
							->classInfo;
	}
	else
	{
		startType = 0;
	}

	/* The start type is also the access scope */
	accessedFrom = startType;
	if(!accessedFrom)
	{
		accessedFrom = ILClassLookup(ILClassGlobalScope(genInfo->image),
									 "<Module>", (const char *)0);
	}
	accessedFrom = ILClassResolve(accessedFrom);

	/* Scan the start type and its nested parents */
	while(startType != 0)
	{
		/* Resolve cross-image references */
		startType = ILClassResolve(startType);

		/* Look for members */
		result = MemberLookup(genInfo, startType, name,
							  accessedFrom, &results);
		if(result != CS_SEMKIND_VOID)
		{
			return LookupToSem(node, name, &results, result);
		}

		/* Move up to the nested parent */
		startType = ILClass_NestedParent(startType);
	}

	/* Clear the results buffer */
	InitMembers(&results);

	/* Scan all namespaces that enclose the current context */
	namespace = (ILNode_Namespace *)(genInfo->currentNamespace);
	while(namespace != 0 && !(results.num))
	{
		/* Look for the type in the current namespace */
		result = FindTypeInNamespace(genInfo, name, namespace->name,
									 accessedFrom, &results);
		if(result != CS_SEMKIND_VOID)
		{
			break;
		}

		/* Look for an alias directive for the name */
		alias = namespace->aliases;
		while(alias != 0)
		{
			if(!strcmp(alias->name, name))
			{
				if(alias->classInfo)
				{
					value.kind = CS_SEMKIND_TYPE;
					value.type = ILClassToType(alias->classInfo);
				}
				else
				{
					value.kind = CS_SEMKIND_NAMESPACE;
					value.type = (ILType *)(alias->ref);
				}
				return value;
			}
			alias = alias->next;
		}

		/* Find the types in all using namespaces */
		using = namespace->using;
		while(using != 0)
		{
			FindTypeInNamespace(genInfo, name, using->name,
								accessedFrom, &results);
			using = using->next;
		}

		/* Move up to the enclosing namespace */
		namespace = namespace->enclosing;
	}

	/* We should have 0, 1, or many types at this point */
	if(results.num > 1)
	{
		/* The result is ambiguous */
		AmbiguousError(node, name, &results);
	}
	if(results.num != 0)
	{
		/* Return the first type in the results list */
		if(results.members->kind == CS_MEMBERKIND_TYPE)
		{
			return LookupToSem(node, name, &results, CS_SEMKIND_TYPE);
		}
		else if(results.members->kind == CS_MEMBERKIND_TYPE_NODE)
		{
			return LookupToSem(node, name, &results, CS_SEMKIND_TYPE_NODE);
		}
		else
		{
			return LookupToSem(node, name, &results, CS_SEMKIND_NAMESPACE);
		}
	}
	FreeMembers(&results);

	/* Could not resolve the name */
	CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
				  "`%s' is not declared in the current scope", name);
	return CSSemValueDefault;
}

/*
 * Filter a member lookup results list to include only static entries.
 */
static int FilterStatic(CSMemberLookupInfo *results, int kind)
{
	ILProgramItem *first = results->members->member;
	ILMethod *method;
	CSMemberLookupIter iter;
	CSMemberInfo *member;

	switch(kind)
	{
		case CS_SEMKIND_TYPE:
		{
			/* Nested types are always static members */
		}
		break;

		case CS_SEMKIND_FIELD:
		{
			/* Bail out if the field is not static */
			if(!ILField_IsStatic((ILField *)first))
			{
				return CS_SEMKIND_VOID;
			}
		}
		break;

		case CS_SEMKIND_METHOD_GROUP:
		{
			/* Remove all non-static methods from the group */
			MemberIterInit(&iter, results);
			while((member = MemberIterNext(&iter)) != 0)
			{
				if(member->kind == IL_META_MEMBERKIND_METHOD &&
				   !ILMethod_IsStatic((ILMethod *)(member->member)))
				{
					MemberIterRemove(&iter);
				}
			}
			if(!(results->num))
			{
				return CS_SEMKIND_VOID;
			}
		}
		break;

		case CS_SEMKIND_PROPERTY:
		{
			/* Bail out if the property is not static */
			method = ILProperty_Getter((ILProperty *)first);
			if(!method)
			{
				method = ILProperty_Setter((ILProperty *)first);
			}
			if(!method || !ILMethod_IsStatic(method))
			{
				return CS_SEMKIND_VOID;
			}
		}
		break;

		case CS_SEMKIND_EVENT:
		{
			/* Bail out if the event is not static */
			method = ILEvent_AddOn((ILEvent *)first);
			if(!method)
			{
				method = ILEvent_RemoveOn((ILEvent *)first);
			}
			if(!method || !ILMethod_IsStatic(method))
			{
				return CS_SEMKIND_VOID;
			}
		}
		break;
	}

	return kind;
}

/*
 * Filter a member lookup results list to include only non-static entries.
 */
static int FilterNonStatic(CSMemberLookupInfo *results, int kind)
{
	ILProgramItem *first = results->members->member;
	ILMethod *method;
	CSMemberLookupIter iter;
	CSMemberInfo *member;

	switch(kind)
	{
		case CS_SEMKIND_TYPE:
		{
			/* Nested types are always static members */
			return CS_SEMKIND_VOID;
		}
		/* Not reached */

		case CS_SEMKIND_FIELD:
		{
			/* Bail out if the field is static */
			if(ILField_IsStatic((ILField *)first))
			{
				return CS_SEMKIND_VOID;
			}
		}
		break;

		case CS_SEMKIND_METHOD_GROUP:
		{
			/* Remove all static methods from the group */
			MemberIterInit(&iter, results);
			while((member = MemberIterNext(&iter)) != 0)
			{
				if(member->kind == IL_META_MEMBERKIND_METHOD &&
				   ILMethod_IsStatic((ILMethod *)(member->member)))
				{
					MemberIterRemove(&iter);
				}
			}
			if(!(results->num))
			{
				return CS_SEMKIND_VOID;
			}
		}
		break;

		case CS_SEMKIND_PROPERTY:
		{
			/* Bail out if the property is static */
			method = ILProperty_Getter((ILProperty *)first);
			if(!method)
			{
				method = ILProperty_Setter((ILProperty *)first);
			}
			if(!method || ILMethod_IsStatic(method))
			{
				return CS_SEMKIND_VOID;
			}
		}
		break;

		case CS_SEMKIND_EVENT:
		{
			/* Bail out if the event is static */
			method = ILEvent_AddOn((ILEvent *)first);
			if(!method)
			{
				method = ILEvent_RemoveOn((ILEvent *)first);
			}
			if(!method || ILMethod_IsStatic(method))
			{
				return CS_SEMKIND_VOID;
			}
		}
		break;
	}

	return kind;
}

CSSemValue CSResolveMemberName(ILGenInfo *genInfo, ILNode *node,
							   CSSemValue value, const char *name)
{
	char *fullName;
	CSMemberLookupInfo results;
	ILClass *accessedFrom;
	int result;

	/* Find the accessor scope */
	if(genInfo->currentMethod)
	{
		accessedFrom = ILMethod_Owner
		  (((ILNode_MethodDeclaration *)(genInfo->currentMethod))->methodInfo);
	}
	else if(genInfo->currentClass)
	{
		accessedFrom = ((ILNode_ClassDefn *)(genInfo->currentClass))
							->classInfo;
	}
	else
	{
		accessedFrom = ILClassLookup(ILClassGlobalScope(genInfo->image),
									 "<Module>", (const char *)0);
	}
	accessedFrom = ILClassResolve(accessedFrom);

	/* Determine how to resolve the member from its semantic kind */
	switch(value.kind)
	{
		case CS_SEMKIND_NAMESPACE:
		{
			/* Look for a type or sub-namespace with the given name
			   within the namespace */
			fullName = (char *)(value.type);
			InitMembers(&results);
			result = FindTypeInNamespace(genInfo, name, fullName,
										 accessedFrom, &results);
			if(result != CS_SEMKIND_VOID)
			{
				return LookupToSem(node, name, &results, result);
			}

			/* Could not find the member within the namespace */
			CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
						  "`%s' is not a member of the namespace `%s'",
						  name, fullName);
		}
		break;

		case CS_SEMKIND_TYPE:
		{
			/* Convert the type into a class and perform a lookup */
			result = MemberLookup(genInfo, ILTypeToClass(genInfo, value.type),
								  name, accessedFrom, &results);
			if(result != CS_SEMKIND_VOID)
			{
				/* Filter the result to only include static definitions */
				result = FilterStatic(&results, result);
			}
			if(result != CS_SEMKIND_VOID)
			{
				return LookupToSem(node, name, &results, result);
			}
			CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
						  "`%s' is not a member of the type `%s'",
						  name, CSTypeToName(value.type));
		}
		break;

		case CS_SEMKIND_LVALUE:
		case CS_SEMKIND_RVALUE:
		{
			/* Perform a member lookup based on the expression's type */
			result = MemberLookup(genInfo, ILTypeToClass(genInfo, value.type),
								  name, accessedFrom, &results);
			if(result != CS_SEMKIND_VOID)
			{
				/* Filter the result to remove static definitions */
				result = FilterNonStatic(&results, result);
			}
			if(result != CS_SEMKIND_VOID)
			{
				return LookupToSem(node, name, &results, result);
			}
			CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
						  "`%s' is not an instance member of the type `%s'",
						  name, CSTypeToName(value.type));
		}
		break;

		default:
		{
			/* This kind of semantic value does not have members */
			CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
						  "invalid left operand to `.'");
		}
		break;
	}

	/* If we get here, then something went wrong */
	return CSSemValueDefault;
}

ILMethod *CSGetGroupMember(void *group, unsigned long n)
{
	CSMemberInfo *member = (CSMemberInfo *)group;
	while(member != 0)
	{
		if(n <= 0)
		{
			return (ILMethod *)(member->member);
		}
		--n;
		member = member->next;
	}
	return 0;
}

void *CSRemoveGroupMember(void *group, unsigned long n)
{
	CSMemberInfo *member = (CSMemberInfo *)group;
	CSMemberInfo *last = 0;
	while(member != 0)
	{
		if(n <= 0)
		{
			if(last)
			{
				last->next = member->next;
				ILFree(member);
				return group;
			}
			else
			{
				last = member->next;
				ILFree(member);
				return (void *)last;
			}
		}
		--n;
		last = member;
		member = member->next;
	}
	return group;
}

#ifdef	__cplusplus
};
#endif
