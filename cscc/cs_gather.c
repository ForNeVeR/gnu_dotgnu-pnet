/*
 * cs_gather.c - "Type gathering" support for the C# compiler.
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

Type gathering is a two-phase pass that happens just before semantic
analysis.

The first phase collects up the names of all of the types that are
declared in the program, as opposed to types declared in libraries.

The second phase creates the type structures (ILClass, ILMethod, etc)
to represent the entire type hierarchy of the program.  During this
process, we check that everything has been declared correctly.

*/

/*
 * Count the number of classes in a base class list.
 */
static int CountBaseClasses(ILNode *node)
{
	int count = 0;
	while(node != 0)
	{
		++count;
		if(yykind(node) != yykindof(ILNode_ArgList))
		{
			break;
		}
		node = ((ILNode_ArgList *)node)->expr1;
	}
	return count;
}

/*
 * Create the program structure for a type and all of its base types.
 * Returns the new end of the top-level type list.
 */
static void CreateType(ILGenInfo *info, ILScope *globalScope,
					   ILNode_List *list, ILNode *systemObjectName,
					   ILNode *type)
{
	const char *name;
	const char *namespace;
	int numBases;
	ILClass **baseList;
	int base;
	ILNode *baseNodeList;
	ILNode *baseNode;
	ILNode *baseTypeNode;
	char *tempName;
	ILClass *parent;
	ILClass *classInfo;
	int errorReported;
	ILNode_ClassDefn *defn;

	/* Get the name and namespace for the type, for error reporting */
	defn = (ILNode_ClassDefn *)type;
	name = defn->name;
	namespace = defn->namespace;
	if(namespace && *namespace == '\0')
	{
		namespace = 0;
	}

	/* If the type is already created, then bail out early */
	if(defn->classInfo != 0)
	{
		if(defn->classInfo == (ILClass *)2)
		{
			CSErrorOnLine(yygetfilename(defn), yygetlinenum(defn),
						  "`%s%s%s' is defined recursively",
						  (namespace ? namespace : ""),
						  (namespace ? "." : ""), name);
		}
		return;
	}

	/* Mark this type as already seen so that we can detect recursion */
	defn->classInfo = (ILClass *)2;

	/* Create all of the base classes */
	numBases = CountBaseClasses(defn->baseClass);
	if(numBases > 0)
	{
		baseList = (ILClass **)ILCalloc(numBases, sizeof(ILClass *));
		if(!baseList)
		{
			CSOutOfMemory();
		}
	}
	else
	{
		baseList = 0;
	}
	baseNodeList = defn->baseClass;
	for(base = 0; base < numBases; ++base)
	{
		/* Get the name of the class to be inherited or implemented */
		if(yykind(baseNodeList) == yykindof(ILNode_ArgList))
		{
			baseNode = ((ILNode_ArgList *)baseNodeList)->expr2;
			baseNodeList = ((ILNode_ArgList *)baseNodeList)->expr1;
		}
		else
		{
			baseNode = baseNodeList;
		}

		/* Look in the scope for the base class */
		if(ILIsQualIdent(baseNode))
		{
			if(ILScopeResolveType(globalScope, baseNode, namespace,
								  &(baseList[base]), &baseTypeNode))
			{
				if(baseList[base] == 0)
				{
					CreateType(info, globalScope, list,
							   systemObjectName, baseTypeNode);
					if(((ILNode_ClassDefn *)baseTypeNode)->classInfo
								!= ((ILClass *)1) &&
					   ((ILNode_ClassDefn *)baseTypeNode)->classInfo
					   			!= ((ILClass *)2))
					{
						baseList[base] =
							((ILNode_ClassDefn *)baseTypeNode)->classInfo;
					}
				}
			}
			else
			{
				/* The base class does not exist */
				tempName = ILQualIdentName(baseNode, 0);
				CSErrorOnLine(yygetfilename(baseNode), yygetlinenum(baseNode),
							  "`%s' is not declared as a type", tempName);
			}
		}
		else
		{
			/* This is not a valid base class specification */
			CSErrorOnLine(yygetfilename(baseNode), yygetlinenum(baseNode),
						  "invalid base type");
		}
	}

	/* Find the parent class within the base list */
	parent = 0;
	errorReported = 0;
	for(base = 0; base < numBases; ++base)
	{
		if(baseList[base] && !ILClass_IsInterface(baseList[base]))
		{
			if(parent)
			{
				if(!errorReported)
				{
					CSErrorOnLine(yygetfilename(type), yygetlinenum(type),
					  "class inherits from two or more non-interface classes");
					errorReported = 1;
				}
			}
			else
			{
				parent = baseList[base];
			}
		}
	}
	if(!parent)
	{
		if(!strcmp(name, "Object") && namespace != 0 &&
		   !strcmp(namespace, "System"))
		{
			/* Special case: we are compiling "System.Object" itself */
			parent = 0;
		}
		else
		{
			/* Compiling something else that inherits "System.Object" */
			ILScopeResolveType(globalScope, systemObjectName, 0,
							   &parent, &baseTypeNode);
		}
	}

	/* Output an error if attempting to inherit from a sealed class */
	if(parent && ILClass_IsSealed(parent))
	{
		CSErrorOnLine(yygetfilename(type), yygetlinenum(type),
					  "inheriting from a sealed parent class");
	}

	/* Create the class information block */
	classInfo = ILClassCreate(ILClassGlobalScope(info->image), 0,
							  name, namespace, parent);
	if(!classInfo)
	{
		CSOutOfMemory();
	}
	ILClassSetAttrs(classInfo, ~0, defn->modifiers);
	defn->classInfo = classInfo;

	/* Add the interfaces to the class */
	for(base = 0; base < numBases; ++base)
	{
		if(baseList[base] && ILClass_IsInterface(baseList[base]))
		{
			if(!ILClassAddImplements(classInfo, baseList[base], 0))
			{
				CSOutOfMemory();
			}
		}
	}

	/* Clean up */
	if(baseList)
	{
		ILFree(baseList);
	}

	/* Add the type to the new top-level list in create order */
	ILNode_List_Add(list, type);
}

ILNode *CSTypeGather(ILGenInfo *info, ILScope *globalScope, ILNode *tree)
{
	ILNode_ListIter iterator;
	ILNode *child;
	ILNode_ClassDefn *defn;
	ILNode_List *list;
	ILNode *origDefn;
	int error;
	const char *name;
	const char *namespace;
	ILScope *scope;
	ILClass *tempClass;
	ILNode *systemObject;

	/* Create a new top-level list for the program */
	list = (ILNode_List *)ILNode_List_create();

	/* Scan all top-level types to declare them */
	ILNode_ListIter_Init(&iterator, tree);
	while((child = ILNode_ListIter_Next(&iterator)) != 0)
	{
		if(yykind(child) == yykindof(ILNode_ClassDefn))
		{
			defn = (ILNode_ClassDefn *)child;
			name = defn->name;
			namespace = defn->namespace;
			if(namespace && *namespace == '\0')
			{
				namespace = 0;
			}
			error = ILScopeDeclareType(globalScope, child,
								   	   name, namespace, &scope,
								   	   &origDefn);
			if(error != IL_SCOPE_ERROR_OK)
			{
				/* Could not declare the type in the global scope */
				switch(error)
				{
					case IL_SCOPE_ERROR_IMPORT_CONFLICT:
					{
						CSErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"`%s%s%s' conflicts with imported type",
								(namespace ? namespace : ""),
								(namespace ? "." : ""), name);
					}
					break;

					case IL_SCOPE_ERROR_REDECLARED:
					{
						CSErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"`%s%s%s' already declared",
								(namespace ? namespace : ""),
								(namespace ? "." : ""), name);
						CSErrorOnLine(yygetfilename(origDefn),
									  yygetlinenum(origDefn),
									  "previous declaration here");
					}
					break;

					case IL_SCOPE_ERROR_CANT_CREATE_NAMESPACE:
					{
						CSErrorOnLine(yygetfilename(child), yygetlinenum(child),
									  "`%s' is not a valid namespace",
									  namespace);
					}
					break;

					case IL_SCOPE_ERROR_NAME_IS_NAMESPACE:
					{
						CSErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"`%s%s%s' cannot be declared as a type",
								(namespace ? namespace : ""),
								(namespace ? "." : ""), name);
						CSErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"because it is already declared as namespace");
					}
					break;

					default:
					{
						CSErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"`%s%s%s' cannot be declared as a type",
								(namespace ? namespace : ""),
								(namespace ? "." : ""), name);
						CSErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"because it is already declared elsewhere");
					}
					break;
				}
			}
			else
			{
				/* Declare nested types also */

				/* Replace the class body with a scoped body */
				defn->body = ILNode_ScopeChange_create(scope, defn->body);

				/* Add the type to the end of the new top-level list */
				ILNode_List_Add(list, child);
			}
		}
	}

	/* Create the top-level types, and re-order them so that the
	   base types are listed before types that inherit them */
	tree = (ILNode *)list;
	list = (ILNode_List *)ILNode_List_create();
	systemObject = ILQualIdentTwo("System", "Object");
	if(ILScopeResolveType(globalScope, systemObject, 0, &tempClass, &child) &&
	   child != 0)
	{
		/* The programmer has declared "System.Object", so we must
		   create it first.  This is necessary because other classes
		   do not need to explicitly inherit this and so we won't
		   pick it up during normal type creation otherwise */
		CreateType(info, globalScope, list, systemObject, child);
	}
	ILNode_ListIter_Init(&iterator, tree);
	while((child = ILNode_ListIter_Next(&iterator)) != 0)
	{
		CreateType(info, globalScope, list, systemObject, child);
	}

	/* Return the new top-level list to the caller */
	return (ILNode *)list;
}

#ifdef	__cplusplus
};
#endif
