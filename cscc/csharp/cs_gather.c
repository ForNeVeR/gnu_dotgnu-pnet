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
#include <codegen/cg_nodemap.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/*

Type gathering is a three-phase pass that happens just before semantic
analysis.

The first phase collects up the names of all of the types that are
declared in the program, as opposed to types declared in libraries.

The second phase creates the ILClass type structures to represent the
entire type hierarchy of the program.  During this process, we check
that all types have been declared correctly.

The third phase creates the fields, methods, etc that appear within
each class.  The types of parameters and return values are checked,
but duplicates and access permissions are not.  That is left until
semantic analysis.

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
 * Convert a class definition node into an ILClass value.
 */
static ILClass *NodeToClass(ILNode *node)
{
	if(node)
	{
		ILNode_ClassDefn *defn = (ILNode_ClassDefn *)node;
		if(defn->classInfo != ((ILClass *)1) &&
		   defn->classInfo != ((ILClass *)2))
		{
			return defn->classInfo;
		}
	}
	return 0;
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
	ILClass *parent;
	ILClass *classInfo;
	int errorReported;
	ILNode_ClassDefn *defn;
	ILNode *savedNamespace;
	ILNode *savedClass;
	ILProgramItem *nestedScope;
	ILNode *node;
	ILNode_ListIter iter;

	/* Get the name and namespace for the type, for error reporting */
	defn = (ILNode_ClassDefn *)type;
	name = defn->name;
	namespace = defn->namespace;
	if(defn->nestedParent || (namespace && *namespace == '\0'))
	{
		namespace = 0;
	}

	/* If the type is already created, then bail out early */
	if(defn->classInfo != 0)
	{
		if(defn->classInfo == (ILClass *)2)
		{
			CCErrorOnLine(yygetfilename(defn), yygetlinenum(defn),
						  "`%s%s%s' is defined recursively",
						  (namespace ? namespace : ""),
						  (namespace ? "." : ""), name);
		}
		return;
	}

	/* Mark this type as already seen so that we can detect recursion */
	defn->classInfo = (ILClass *)2;

	/* If this is a nested type, then create its nesting parent first */
	if(defn->nestedParent)
	{
		CreateType(info, globalScope, list,
				   systemObjectName, (ILNode *)(defn->nestedParent));
		nestedScope = (ILProgramItem *)(defn->nestedParent->classInfo);
		if(!nestedScope || nestedScope == (ILProgramItem *)1 ||
		   nestedScope == (ILProgramItem *)1)
		{
			nestedScope = ILClassGlobalScope(info->image);
		}
	}
	else
	{
		nestedScope = ILClassGlobalScope(info->image);
	}

	/* Set the namespace and class to use for resolving type names */
	savedNamespace = info->currentNamespace;
	info->currentNamespace = defn->namespaceNode;
	savedClass = info->currentClass;
	info->currentClass = (ILNode *)(defn->nestedParent);

	/* Create all of the base classes */
	numBases = CountBaseClasses(defn->baseClass);
	if(numBases > 0)
	{
		baseList = (ILClass **)ILCalloc(numBases, sizeof(ILClass *));
		if(!baseList)
		{
			CCOutOfMemory();
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
		if(CSSemBaseType(baseNode, info, &baseNode,
						 &baseTypeNode, &(baseList[base])))
		{
			if(baseList[base] == 0)
			{
				baseList[base] = NodeToClass(baseTypeNode);
				if(baseList[base] == 0)
				{
					CreateType(info, globalScope, list,
							   systemObjectName, baseTypeNode);
					baseList[base] = NodeToClass(baseTypeNode);
				}
			}
		}
		else
		{
			/* This is not a valid base class specification */
			CCErrorOnLine(yygetfilename(baseNode), yygetlinenum(baseNode),
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
					CCErrorOnLine(yygetfilename(type), yygetlinenum(type),
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

	/* Change to the global namespace to resolve "System.Object" */
	while(((ILNode_Namespace *)(info->currentNamespace))->enclosing != 0)
	{
		info->currentNamespace = (ILNode *)
			((ILNode_Namespace *)(info->currentNamespace))->enclosing;
	}

	/* Test for interfaces, or find "System.Object" if no parent yet */
   	if((defn->modifiers & IL_META_TYPEDEF_CLASS_SEMANTICS_MASK)
	   		== IL_META_TYPEDEF_INTERFACE)
	{
		if(parent)
		{
			CCErrorOnLine(yygetfilename(type), yygetlinenum(type),
						  "interface inherits from non-interface class");
			parent = 0;
		}
	}
	else if(!parent)
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
			if(CSSemBaseType(systemObjectName, info, &systemObjectName,
							 &baseTypeNode, &parent))
			{
				if(!parent)
				{
					parent = NodeToClass(baseTypeNode);
					if(!parent)
					{
						CreateType(info, globalScope, list,
								   systemObjectName, baseTypeNode);
						parent = NodeToClass(baseTypeNode);
					}
				}
			}
			else
			{
				/* Use the builtin library's "System.Object" */
				parent = ILType_ToClass(ILFindSystemType(info, "Object"));
			}
		}
	}

	/* Restore the namespace and class */
	info->currentNamespace = savedNamespace;
	info->currentClass = savedClass;

	/* Output an error if attempting to inherit from a sealed class */
	if(parent && ILClass_IsSealed(parent))
	{
		CCErrorOnLine(yygetfilename(type), yygetlinenum(type),
					  "inheriting from a sealed parent class");
	}

	/* Create the class information block */
	classInfo = ILClassCreate(nestedScope, 0, name, namespace, parent);
	if(!classInfo)
	{
		CCOutOfMemory();
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
				CCOutOfMemory();
			}
		}
	}

	/* Clean up */
	if(baseList)
	{
		ILFree(baseList);
	}

	/* Record the node on the class as user data */
	ILSetProgramItemMapping(info, (ILNode *)defn);

	/* Process the nested types */
	node = defn->body;
	if(node && yyisa(node, ILNode_ScopeChange))
	{
		node = ((ILNode_ScopeChange *)node)->body;
	}
	ILNode_ListIter_Init(&iter, node);
	while((node = ILNode_ListIter_Next(&iter)) != 0)
	{
		if(yyisa(node, ILNode_ClassDefn))
		{
			CreateType(info, globalScope, list, systemObjectName, node);
		}
	}

	/* Add the type to the new top-level list in create order */
	if(!(defn->nestedParent))
	{
		ILNode_List_Add(list, type);
	}
}

/*
 * Add a class member to a scope and report errors.
 */
static void AddMemberToScope(ILScope *scope, int memberKind,
							 const char *name, ILMember *member,
							 ILNode *node)
{
	int error = ILScopeDeclareMember(scope, name, memberKind, member, node);
	if(error != IL_SCOPE_ERROR_OK)
	{
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
					  "member conflicts with a type name in the same scope");
	}
}

/*
 * Search for a member with a specific name to do duplicate testing.
 */
static ILMember *FindMemberByName(ILClass *classInfo, const char *name,
								  ILClass *scope)
{
	ILMember *member;
	ILImplements *impl;
	while(classInfo != 0)
	{
		/* Scan the members of this class */
		member = 0;
		while((member = ILClassNextMember(classInfo, member)) != 0)
		{
			if(!strcmp(ILMember_Name(member), name) &&
			   ILMemberAccessible(member, scope))
			{
				return member;
			}
		}

		/* Scan parent interfaces if this class is itself an interface */
		if(ILClass_IsInterface(classInfo))
		{
			impl = 0;
			while((impl = ILClassNextImplements(classInfo, impl)) != 0)
			{
				member = FindMemberByName
					(ILClassResolve(ILImplementsGetInterface(impl)),
					 name, scope);
				if(member)
				{
					return member;
				}
			}
		}

		/* Move up to the parent of this class */
		classInfo = ILClass_Parent(classInfo);
	}
	return 0;
}

/*
 * Search for a member with a specific name and/or signature.
 */
static ILMember *FindMemberBySignature(ILClass *classInfo, const char *name,
									   ILType *signature, ILMember *notThis,
									   ILClass *scope, int interfaceOverride)
{
	ILMember *member;
	ILImplements *impl;
	int kind = ILMemberGetKind(notThis);

	while(classInfo != 0)
	{
		/* Scan the members of this class */
		member = 0;
		while((member = ILClassNextMember(classInfo, member)) != 0)
		{
			if(member != notThis &&
			   !strcmp(ILMember_Name(member), name) &&
			   ILMemberAccessible(member, scope) &&
			   (!interfaceOverride || classInfo == scope))
			{
				if(ILMemberGetKind(member) != kind)
				{
					return member;
				}
				else if(CSSignatureIdentical(ILMemberGetSignature(member),
										     signature))
				{
					if(ILMember_IsMethod(member) &&
					   ILMethod_HasSpecialName((ILMethod *)member) &&
					   !strncmp(name, "op_", 3))
					{
						/* This is an operator, which includes the
						   return type in its signature definition */
						if(ILTypeIdentical
							  (ILTypeGetReturn(ILMemberGetSignature(member)),
							   ILTypeGetReturn(signature)))
						{
							return member;
						}
					}
					else
					{
						return member;
					}
				}
			}
		}

		/* Scan parent interfaces if this class is itself an interface */
		if(ILClass_IsInterface(classInfo))
		{
			impl = 0;
			while((impl = ILClassNextImplements(classInfo, impl)) != 0)
			{
				member = FindMemberBySignature
					(ILClassResolve(ILImplementsGetInterface(impl)),
					 name, signature, notThis, scope, interfaceOverride);
				if(member)
				{
					return member;
				}
			}
		}

		/* Move up to the parent of this class */
		classInfo = ILClass_Parent(classInfo);
	}
	return 0;
}

/*
 * Report duplicate definitions.
 */
static void ReportDuplicates(ILNode *node, ILMember *newMember,
							 ILMember *existingMember, ILClass *classInfo,
							 ILUInt32 modifiers, char *name)
{
	/* TODO: we need better error messages here */

	if(ILMember_Owner(existingMember) == classInfo)
	{
		/* The duplicate is in the same class */
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
		  			  "declaration of `%s' conflicts with an existing member",
					  name);
	}
	else if((modifiers & CS_SPECIALATTR_NEW) == 0)
	{
		/* The duplicate is in a parent class, and "new" wasn't specified */
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
		  "declaration of `%s' hides an inherited member, "
		  "and `new' was not present", name);
	}
}

/*
 * Report a warning for an unnecessary "new" keyword on a declaration.
 */
static void ReportUnnecessaryNew(ILNode *node, char *name)
{
	CCWarningOnLine(yygetfilename(node), yygetlinenum(node),
			        "declaration of `%s' includes unnecessary `new' keyword",
					name);
}

/*
 * Create a field definition.
 */
static void CreateField(ILGenInfo *info, ILClass *classInfo,
						ILNode_FieldDeclaration *field)
{
	ILNode_ListIter iterator;
	ILNode_FieldDeclarator *decl;
	ILField *fieldInfo;
	char *name;
	ILType *tempType;
	ILType *modifier;
	ILMember *member;

	/* Get the field's type */
	tempType = CSSemType(field->type, info, &(field->type));

	/* Add the "volatile" modifier if necessary */
	if((field->modifiers & CS_SPECIALATTR_VOLATILE) != 0)
	{
		modifier = ILFindNonSystemType(info, "IsVolatile",
									   "System.Runtime.CompilerServices");
		if(ILType_IsClass(modifier))
		{
			modifier = ILTypeCreateModifier(info->context, 0,
											IL_TYPE_COMPLEX_CMOD_REQD,
											ILType_ToClass(modifier));
			if(!modifier)
			{
				CCOutOfMemory();
			}
			tempType = ILTypeAddModifiers(info->context, modifier, tempType);
		}
	}

	/* Iterator over the field declarators and create each field in turn */
	ILNode_ListIter_Init(&iterator, field->fieldDeclarators);
	while((decl = (ILNode_FieldDeclarator *)
						ILNode_ListIter_Next(&iterator)) != 0)
	{
		/* Set the field's owner for later semantic analysis */
		decl->owner = field;

		/* Get the name of the field */
		name = ILQualIdentName(decl->name, 0);

		/* Look for duplicates */
		member = FindMemberByName(classInfo, name, classInfo);

		/* Create the field information block */
		fieldInfo = ILFieldCreate(classInfo, 0, name,
								  (field->modifiers & 0xFFFF));
		if(!fieldInfo)
		{
			CCOutOfMemory();
		}
		decl->fieldInfo = fieldInfo;
		ILMemberSetSignature((ILMember *)fieldInfo, tempType);
		ILSetProgramItemMapping(info, (ILNode *)decl);

		/* Report on duplicates */
		if(member)
		{
			ReportDuplicates(decl->name, (ILMember *)fieldInfo,
							 member, classInfo, field->modifiers, name);
		}
		else if((field->modifiers & CS_SPECIALATTR_NEW) != 0)
		{
			ReportUnnecessaryNew(decl->name, name);
		}

		/* Add the field to the current scope */
		AddMemberToScope(info->currentScope, IL_SCOPE_FIELD,
						 name, (ILMember *)fieldInfo, decl->name);
	}
}

/*
 * Find an interface member match in a particular interface.
 */
static ILMember *FindInterfaceMatch(ILClass *interface,
									const char *name,
									ILType *signature,
									int kind)
{
	ILMember *member = 0;
	while((member = ILClassNextMemberByKind(interface, member, kind)) != 0)
	{
		if(strcmp(ILMember_Name(member), name) != 0)
		{
			continue;
		}
		if(kind == IL_META_MEMBERKIND_METHOD ||
		   kind == IL_META_MEMBERKIND_PROPERTY)
		{
			if(ILTypeIdentical(ILMember_Signature(member), signature))
			{
				return member;
			}
		}
		else if(kind == IL_META_MEMBERKIND_EVENT)
		{
			if(ILTypeIdentical(ILEvent_Type((ILEvent *)member), signature))
			{
				return member;
			}
		}
	}
	return 0;
}

/*
 * Find an interface member match in the interface parents
 * of a specified class.
 */
static ILMember *FindInterfaceMatchInParents(ILClass *classInfo,
											 const char *name,
											 ILType *signature,
											 int kind)
{
	ILImplements *impl = 0;
	ILMember *member;
	ILClass *interface;

	while((impl = ILClassNextImplements(classInfo, impl)) != 0)
	{
		interface = ILClassResolve(ILImplementsGetInterface(impl));
		member = FindInterfaceMatch(interface, name, signature, kind);
		if(member)
		{
			return member;
		}
		member = FindInterfaceMatchInParents(interface, name, signature, kind);
		if(member)
		{
			return member;
		}
	}
	return 0;
}

/*
 * Find the interface member that corresponds to a pariticular
 * member declaration.  Returns NULL if not found.
 */
static ILMember *FindInterfaceDecl(ILNode *node, ILClass *classInfo,
								   ILClass *interface, const char *name,
								   ILType *signature, int kind,
								   ILUInt32 *attrs)
{
	ILUInt32 newAttrs = *attrs;
	ILMember *member;

	/* Check the access modifiers */
	if(interface)
	{
		/* Explicit interface declaration */
		if((newAttrs & IL_META_METHODDEF_STATIC) != 0)
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
			  "explicit interface member implementation cannot be `static'");
		}
		else if((newAttrs & IL_META_METHODDEF_ABSTRACT) != 0)
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
			  "explicit interface member implementation cannot be `abstract'");
		}
		else if((newAttrs & CS_SPECIALATTR_OVERRIDE) != 0)
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
			  "explicit interface member implementation cannot be `override'");
		}
		else if((newAttrs & IL_META_METHODDEF_VIRTUAL) != 0)
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
			  "explicit interface member implementation cannot be `virtual'");
		}
		if((newAttrs & CS_SPECIALATTR_NEW) != 0)
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
			  "explicit interface member implementation cannot be `new'");
		}
		if((newAttrs & IL_META_METHODDEF_MEMBER_ACCESS_MASK) !=
				IL_META_METHODDEF_PRIVATE)
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
			  "explicit interface member implementation must be `private'");
		}

		/* Set the correct attributes on the explicit implementation */
		newAttrs &= ~(IL_META_METHODDEF_MEMBER_ACCESS_MASK |
					  IL_META_METHODDEF_STATIC |
					  IL_META_METHODDEF_ABSTRACT |
					  CS_SPECIALATTR_NEW |
					  CS_SPECIALATTR_OVERRIDE);
		newAttrs |= IL_META_METHODDEF_PRIVATE |
					IL_META_METHODDEF_FINAL |
					IL_META_METHODDEF_VIRTUAL |
					IL_META_METHODDEF_NEW_SLOT;
	}
	else
	{
		/* Implicit interface declaration */
		if((newAttrs & IL_META_METHODDEF_STATIC) != 0)
		{
			/* Static members cannot implement interfaces */
			return 0;
		}
		if((newAttrs & IL_META_METHODDEF_VIRTUAL) != 0 &&
		   (newAttrs & IL_META_METHODDEF_NEW_SLOT) == 0)
		{
			/* "override" members do not implement interfaces:
			   the parent class's virtual method does */
			return 0;
		}
		if((newAttrs & IL_META_METHODDEF_MEMBER_ACCESS_MASK) !=
				IL_META_METHODDEF_PUBLIC)
		{
			/* Implicit interface mappings must be "public" */
			return 0;
		}

		/* Make sure that the final method is virtual */
		newAttrs |= IL_META_METHODDEF_VIRTUAL | IL_META_METHODDEF_NEW_SLOT;
	}

	/* Search for a match amongst the class's interfaces */
	if(interface)
	{
		member = FindInterfaceMatch(interface, name, signature, kind);
		if(!member)
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
						  "specified member is not present in `%s'",
						  CSTypeToName(ILClassToType(interface)));
		}
	}
	else
	{
		member = FindInterfaceMatchInParents(classInfo, name, signature, kind);
	}

	/* Adjust the final attributes and return */
	if(member)
	{
		*attrs = newAttrs;
	}
	return member;
}

/*
 * Create a method definition.
 */
static void CreateMethod(ILGenInfo *info, ILClass *classInfo,
						 ILNode_MethodDeclaration *method)
{
	char *name;
	char *basicName;
	ILType *tempType;
	ILMethod *methodInfo;
	ILType *signature;
	ILNode_ListIter iterator;
	ILNode *param;
	ILNode_FormalParameter *fparam;
	ILUInt32 paramNum;
	ILUInt32 argListParam;
	ILParameter *parameter;
	ILMember *member;
	ILClass *interface;
	ILMember *interfaceMember;
	
	/* Get the name of the method, and the interface member (if any) */
	interface = 0;
	interfaceMember = 0;
	if(yykind(method->name) == yykindof(ILNode_Identifier))
	{
		/* Simple method name */
		name = ILQualIdentName(method->name, 0);
		basicName = name;
	}
	else
	{
		/* Qualified method name that overrides some interface method */
		name = ILQualIdentName(method->name, 0);
		basicName = ILQualIdentName
			(((ILNode_QualIdent *)(method->name))->right, 0);
		signature = CSSemType(((ILNode_QualIdent *)(method->name))->left, info,
							  &(((ILNode_QualIdent *)(method->name))->left));
		if(signature)
		{
			if(!ILType_IsClass(signature) ||
			   !ILClass_IsInterface(ILType_ToClass(signature)))
			{
				CCErrorOnLine(yygetfilename(method), yygetlinenum(method),
							  "`%s' is not an interface",
							  CSTypeToName(signature));
			}
			else
			{
				interface = ILType_ToClass(signature);
			}
		}
		if(ILClass_IsInterface(classInfo))
		{
			CCErrorOnLine(yygetfilename(method), yygetlinenum(method),
				  "cannot use explicit interface member implementations "
				  "within interfaces");
		}
	}

	/* Create the method information block */
	methodInfo = ILMethodCreate(classInfo, 0, name,
								(method->modifiers & 0xFFFF));
	if(!methodInfo)
	{
		CCOutOfMemory();
	}
	method->methodInfo = methodInfo;
	ILSetProgramItemMapping(info, (ILNode *)method);

	/* Get the return type */
	tempType = CSSemTypeVoid(method->type, info, &(method->type));

	/* Create the method signature type */
	signature = ILTypeCreateMethod(info->context, tempType);
	if(!signature)
	{
		CCOutOfMemory();
	}
	if((method->modifiers & IL_META_METHODDEF_STATIC) == 0)
	{
		ILTypeSetCallConv(signature, IL_META_CALLCONV_HASTHIS);
		ILMethodSetCallConv(methodInfo, IL_META_CALLCONV_HASTHIS);
	}

	/* Create the parameters for the method */
	argListParam = 0;
	paramNum = 1;
	ILNode_ListIter_Init(&iterator, method->params);
	while((param = ILNode_ListIter_Next(&iterator)) != 0)
	{
		/* Get the type of the parameter */
		fparam = (ILNode_FormalParameter *)param;
		if(fparam->pmod == ILParamMod_arglist)
		{
			argListParam = paramNum;
			++paramNum;
			continue;
		}
		tempType = CSSemType(fparam->type, info, &(fparam->type));

		/* Add a "byref" node to the type if "out" or "ref" */
		if(fparam->pmod == ILParamMod_out ||
		   fparam->pmod == ILParamMod_ref)
		{
			tempType = ILTypeCreateRef(info->context,
									   IL_TYPE_COMPLEX_BYREF, tempType);
			if(!tempType)
			{
				CCOutOfMemory();
			}
			if(info->outputIsJava)
			{
				CCErrorOnLine(yygetfilename(method), yygetlinenum(method),
				  	"`%s' parameters not permitted when compiling "
						"to Java bytecode",
				    (fparam->pmod == ILParamMod_out ? "out" : "ref"));
			}
		}

		/* Add the parameter type to the method signature */
		if(!ILTypeAddParam(info->context, signature, tempType))
		{
			CCOutOfMemory();
		}

		/* Create a parameter definition in the metadata to record the name */
		parameter = ILParameterCreate
				(methodInfo, 0, ILQualIdentName(fparam->name, 0),
			     ((fparam->pmod == ILParamMod_out) ? IL_META_PARAMDEF_OUT : 0),
				 paramNum);
		if(!parameter)
		{
			CCOutOfMemory();
		}

		/* Add "System.ParamArrayAttribute" if the parameter is "params" */
		if(fparam->pmod == ILParamMod_params)
		{
			ILGenItemAddAttribute(info, (ILProgramItem *)parameter,
								  "ParamArrayAttribute");
		}

		/* Advance to the next parameter */
		++paramNum;
	}

	/* Mark the method as "vararg" if "__arglist" was present */
	if(argListParam != 0)
	{
		if(info->outputIsJava)
		{
			CCErrorOnLine(yygetfilename(method), yygetlinenum(method),
				  "`__arglist' is disallowed when compiling to Java bytecode");
		}
		if((argListParam + 1) != paramNum)
		{
			CCErrorOnLine(yygetfilename(method), yygetlinenum(method),
						  "`__arglist' must be the last formal parameter");
		}
		ILTypeSetCallConv(signature, ILType_CallConv(signature) |
									 IL_META_CALLCONV_VARARG);
		ILMethodSetCallConv(methodInfo, ILType_CallConv(signature));
	}

	/* Set the signature for the method */
	ILMemberSetSignature((ILMember *)methodInfo, signature);

	/* Add the method to the current scope */
	AddMemberToScope(info->currentScope, IL_SCOPE_METHOD,
					 name, (ILMember *)methodInfo, method->name);

	/* Process interface overrides */
	if(!ILClass_IsInterface(classInfo))
	{
		paramNum = method->modifiers;
		interfaceMember = FindInterfaceDecl
			((ILNode *)method, classInfo, interface,
			 basicName, signature, IL_META_MEMBERKIND_METHOD,
			 &paramNum);
		if(interfaceMember)
		{
			ILMemberSetAttrs((ILMember *)methodInfo, 0xFFFF,
							 (paramNum & 0xFFFF));
			if(interface)
			{
				/* Create an "ILOverride" block to associate the
				   explicit member implementation with the method
				   in the interface that it is implementing */
				interfaceMember = ILMemberImport(info->image, interfaceMember);
				if(!interfaceMember)
				{
					CCOutOfMemory();
				}
				if(!ILOverrideCreate(classInfo, 0,
									 (ILMethod *)interfaceMember,
									 methodInfo))
				{
					CCOutOfMemory();
				}
			}
		}
	}

	/* Ignore property methods with "specialname", as they are
	   tested elsewhere for duplicates */
	if(!ILMethod_HasSpecialName(methodInfo) ||
	   (strncmp(ILMethod_Name(methodInfo), "get_", 4) != 0 &&
	    strncmp(ILMethod_Name(methodInfo), "set_", 4) != 0))
	{
		/* Look for duplicates and report on them */
		member = FindMemberBySignature(classInfo, name, signature,
									   (ILMember *)methodInfo, classInfo,
									   (interface != 0));
		if(member)
		{
			if(ILMember_IsMethod(member) &&
			   ILMethod_IsVirtual((ILMethod *)member) &&
			   !ILMethod_IsNewSlot(methodInfo))
			{
				/* Check for the correct form of virtual method overrides */
				if((method->modifiers & CS_SPECIALATTR_OVERRIDE) == 0)
				{
					CCErrorOnLine(yygetfilename(method), yygetlinenum(method),
			  			"declaration of `%s' overrides an inherited member, "
						"and `override' was not present", name);
				}
			}
			else if(ILMember_Owner(member) == classInfo ||
			        (!ILMethodIsConstructor(methodInfo) &&
					 !ILMethodIsStaticConstructor(methodInfo)))
			{
				ReportDuplicates(method->name, (ILMember *)methodInfo,
								 member, classInfo, method->modifiers, name);
			}
		}
		else if((method->modifiers & CS_SPECIALATTR_NEW) != 0)
		{
			ReportUnnecessaryNew(method->name, name);
		}
	}
}

/*
 * Create an enumerated type member definition.
 */
static void CreateEnumMember(ILGenInfo *info, ILClass *classInfo,
						     ILNode_EnumMemberDeclaration *enumMember)
{
	ILField *fieldInfo;
	char *name;
	ILType *tempType;
	ILMember *member;

	/* Get the field's type, which is the same as its enclosing class */
	tempType = ILType_FromValueType(classInfo);

	/* Get the name of the field */
	name = ILQualIdentName(enumMember->name, 0);

	/* Check for the reserved field name "value__" */
	if(!strcmp(name, "value__"))
	{
		CCErrorOnLine(yygetfilename(enumMember), yygetlinenum(enumMember),
			  "the identifier `value__' is reserved in enumerated types");
		return;
	}

	/* Look for duplicates */
	member = FindMemberByName(classInfo, name, classInfo);

	/* Create the field information block */
	fieldInfo = ILFieldCreate(classInfo, 0, name,
							  IL_META_FIELDDEF_PUBLIC |
							  IL_META_FIELDDEF_STATIC |
							  IL_META_FIELDDEF_LITERAL);
	if(!fieldInfo)
	{
		CCOutOfMemory();
	}
	enumMember->fieldInfo = fieldInfo;
	ILMemberSetSignature((ILMember *)fieldInfo, tempType);
	ILSetProgramItemMapping(info, (ILNode *)enumMember);

	/* Report on duplicates within this class only */
	if(member && ILMember_Owner(member) == classInfo)
	{
		ReportDuplicates((ILNode *)enumMember, (ILMember *)fieldInfo,
						 member, classInfo, 0, name);
	}
}

/*
 * Determine if a property is virtual by inspecting its get or set methods.
 */
static int PropertyIsVirtual(ILProperty *property)
{
	ILMethod *method;

	/* Check the "get" method */
	method = ILPropertyGetGetter(property);
	if(method)
	{
		return ILMethod_IsVirtual(method);
	}

	/* Check the "set" method */
	method = ILPropertyGetSetter(property);
	if(method)
	{
		return ILMethod_IsVirtual(method);
	}

	/* No "get" or "set", so assume that it isn't virtual */
	return 0;
}

/*
 * Determine if an event is virtual by inspecting its add or remove methods.
 */
static int EventIsVirtual(ILEvent *event)
{
	ILMethod *method;

	/* Check the "add" method */
	method = ILEventGetAddOn(event);
	if(method)
	{
		return ILMethod_IsVirtual(method);
	}

	/* Check the "remove" method */
	method = ILEventGetRemoveOn(event);
	if(method)
	{
		return ILMethod_IsVirtual(method);
	}

	/* No "add" or "remove", so assume that it isn't virtual */
	return 0;
}

/*
 * Create a property definition.
 */
static void CreateProperty(ILGenInfo *info, ILClass *classInfo,
						   ILNode_PropertyDeclaration *property)
{
	char *name;
	ILType *propType;
	ILType *tempType;
	ILProperty *propertyInfo;
	ILType *signature;
	ILNode_ListIter iterator;
	ILNode *param;
	ILNode_FormalParameter *fparam;
	ILUInt32 paramNum;
	ILMember *member;
	int interfaceOverride;
	
	/* Create the get and set methods */
	if(property->getAccessor)
	{
		CreateMethod(info, classInfo,
				     (ILNode_MethodDeclaration *)(property->getAccessor));
	}
	if(property->setAccessor)
	{
		CreateMethod(info, classInfo,
				     (ILNode_MethodDeclaration *)(property->setAccessor));
	}

	/* Get the name of the property */
	if(yykind(property->name) == yykindof(ILNode_Identifier))
	{
		/* Simple property name */
		name = ILQualIdentName(property->name, 0);
		interfaceOverride = 0;
	}
	else
	{
		/* Qualified property name that overrides some interface property */
		name = ILQualIdentName(property->name, 0);
		signature = CSSemType
				(((ILNode_QualIdent *)(property->name))->left, info,
			     &(((ILNode_QualIdent *)(property->name))->left));
		if(signature)
		{
			if(!ILType_IsClass(signature) ||
			   !ILClass_IsInterface(ILType_ToClass(signature)))
			{
				CCErrorOnLine(yygetfilename(property), yygetlinenum(property),
							  "`%s' is not an interface",
							  CSTypeToName(signature));
			}
		}
		if(ILClass_IsInterface(classInfo))
		{
			CCErrorOnLine(yygetfilename(property), yygetlinenum(property),
				  "cannot use explicit interface member implementations "
				  "within interfaces");
		}
		interfaceOverride = 1;
	}

	/* Get the property type */
	propType = CSSemType(property->type, info, &(property->type));

	/* Create the property signature type */
	signature = ILTypeCreateProperty(info->context, propType);
	if(!signature)
	{
		CCOutOfMemory();
	}
	if((property->modifiers & IL_META_METHODDEF_STATIC) == 0)
	{
		ILTypeSetCallConv(signature, IL_META_CALLCONV_HASTHIS);
	}

	/* Create the parameters for the property */
	paramNum = 1;
	ILNode_ListIter_Init(&iterator, property->params);
	while((param = ILNode_ListIter_Next(&iterator)) != 0)
	{
		/* Get the type of the parameter */
		fparam = (ILNode_FormalParameter *)param;
		if(fparam->pmod == ILParamMod_arglist)
		{
			CCErrorOnLine(yygetfilename(property), yygetlinenum(property),
						  "`__arglist' cannot be used with indexers");
			++paramNum;
			continue;
		}
		tempType = CSSemType(fparam->type, info, &(fparam->type));

		/* Add the parameter type to the property signature */
		if(!ILTypeAddParam(info->context, signature, tempType))
		{
			CCOutOfMemory();
		}

		/* Move on to the next parameter */
		++paramNum;
	}

	/* Create the property information block */
	propertyInfo = ILPropertyCreate(classInfo, 0, name, 0, signature);
	if(!propertyInfo)
	{
		CCOutOfMemory();
	}
	property->propertyInfo = propertyInfo;
	ILSetProgramItemMapping(info, (ILNode *)property);

	/* Add the method semantics to the property */
	if(property->getAccessor)
	{
		if(!ILMethodSemCreate((ILProgramItem *)propertyInfo, 0,
					  IL_META_METHODSEM_GETTER,
					  ((ILNode_MethodDeclaration *)(property->getAccessor))
					  		->methodInfo))
		{
			CCOutOfMemory();
		}
	}
	if(property->setAccessor)
	{
		if(!ILMethodSemCreate((ILProgramItem *)propertyInfo, 0,
					  IL_META_METHODSEM_SETTER,
					  ((ILNode_MethodDeclaration *)(property->setAccessor))
					  		->methodInfo))
		{
			CCOutOfMemory();
		}
	}

	/* Add the property to the current scope */
	AddMemberToScope(info->currentScope, IL_SCOPE_PROPERTY,
					 name, (ILMember *)propertyInfo, property->name);

	/* Look for duplicates and report on them */
	member = FindMemberBySignature(classInfo, name, signature,
								   (ILMember *)propertyInfo, classInfo,
								   interfaceOverride);
	if(member)
	{
		if(ILMember_IsProperty(member) &&
		   PropertyIsVirtual((ILProperty *)member) &&
		   (property->modifiers & IL_META_METHODDEF_NEW_SLOT) == 0)
		{
			/* Check for the correct form of virtual method overrides */
			if((property->modifiers & CS_SPECIALATTR_OVERRIDE) == 0)
			{
				CCErrorOnLine(yygetfilename(property), yygetlinenum(property),
		  			"declaration of `%s' overrides an inherited member, "
					"and `override' was not present", name);
			}
		}
		else
		{
			ReportDuplicates(property->name, (ILMember *)propertyInfo,
							 member, classInfo, property->modifiers, name);
		}
	}
	else if((property->modifiers & CS_SPECIALATTR_NEW) != 0)
	{
		ReportUnnecessaryNew(property->name, name);
	}
}

/*
 * Create an event definition from a specific declarator.
 */
static void CreateEventDecl(ILGenInfo *info, ILClass *classInfo,
						    ILNode_EventDeclaration *event,
							ILType *eventType,
							ILNode_EventDeclarator *eventDecl)
{
	char *name;
	ILNode *eventName;
	ILEvent *eventInfo;
	ILType *signature;
	ILMember *member;
	int interfaceOverride;
	
	/* Set the back link for use by code generation */
	eventDecl->backLink = event;

	/* Create the add and remove methods */
	if(eventDecl->addAccessor)
	{
		CreateMethod(info, classInfo,
				     (ILNode_MethodDeclaration *)(eventDecl->addAccessor));
	}
	if(eventDecl->removeAccessor)
	{
		CreateMethod(info, classInfo,
				     (ILNode_MethodDeclaration *)(eventDecl->removeAccessor));
	}

	/* TODO: event initializers */

	/* Get the name of the event */
	eventName = ((ILNode_FieldDeclarator *)(eventDecl->fieldDeclarator))->name;
	if(yykind(eventName) == yykindof(ILNode_Identifier))
	{
		/* Simple event name */
		name = ILQualIdentName(eventName, 0);
		interfaceOverride = 0;
	}
	else
	{
		/* Qualified event name that overrides some interface event */
		name = ILQualIdentName(eventName, 0);
		signature = CSSemType
				(((ILNode_QualIdent *)eventName)->left, info,
			     &(((ILNode_QualIdent *)eventName)->left));
		if(signature)
		{
			if(!ILType_IsClass(signature) ||
			   !ILClass_IsInterface(ILType_ToClass(signature)))
			{
				CCErrorOnLine(yygetfilename(eventName),
							  yygetlinenum(eventName),
							  "`%s' is not an interface",
							  CSTypeToName(signature));
			}
		}
		if(ILClass_IsInterface(classInfo))
		{
			CCErrorOnLine(yygetfilename(eventName), yygetlinenum(eventName),
				  "cannot use explicit interface member implementations "
				  "within interfaces");
		}
		interfaceOverride = 1;
	}

	/* Cannot create an event called "value", because it will
	   conflict with the name of the add/remove parameter */
	if(!strcmp(name, "value"))
	{
		CCErrorOnLine(yygetfilename(eventName), yygetlinenum(eventName),
			  		  "cannot declare an event called `value'");
	}

	/* Look for duplicates */
	member = FindMemberByName(classInfo, name, classInfo);

	/* Create the event information block */
	eventInfo = ILEventCreate(classInfo, 0, name, 0,
							  ILTypeToClass(info, eventType));
	if(!eventInfo)
	{
		CCOutOfMemory();
	}
	eventDecl->eventInfo = eventInfo;
	ILSetProgramItemMapping(info, (ILNode *)eventDecl);

	/* Add the method semantics to the event */
	if(eventDecl->addAccessor)
	{
		if(!ILMethodSemCreate((ILProgramItem *)eventInfo, 0,
					  IL_META_METHODSEM_ADD_ON,
					  ((ILNode_MethodDeclaration *)(eventDecl->addAccessor))
					  		->methodInfo))
		{
			CCOutOfMemory();
		}
	}
	if(eventDecl->removeAccessor)
	{
		if(!ILMethodSemCreate((ILProgramItem *)eventInfo, 0,
					  IL_META_METHODSEM_REMOVE_ON,
					  ((ILNode_MethodDeclaration *)(eventDecl->removeAccessor))
					  		->methodInfo))
		{
			CCOutOfMemory();
		}
	}

	/* Add the event to the current scope */
	AddMemberToScope(info->currentScope, IL_SCOPE_EVENT,
					 name, (ILMember *)eventInfo, eventName);

	/* Report on the duplicates */
	if(member)
	{
		if(ILMember_IsEvent(member) &&
		   EventIsVirtual((ILEvent *)member) &&
		   (event->modifiers & IL_META_METHODDEF_NEW_SLOT) == 0)
		{
			/* Check for the correct form of virtual method overrides */
			if((event->modifiers & CS_SPECIALATTR_OVERRIDE) == 0)
			{
				CCErrorOnLine(yygetfilename(eventDecl), yygetlinenum(eventDecl),
		  			"declaration of `%s' overrides an inherited member, "
					"and `override' was not present", name);
			}
		}
		else
		{
			ReportDuplicates(eventName, (ILMember *)eventInfo,
							 member, classInfo, event->modifiers, name);
		}
	}
	else if((event->modifiers & CS_SPECIALATTR_NEW) != 0)
	{
		ReportUnnecessaryNew(eventName, name);
	}

	/* Create the hidden field for the event if necessary.  We must do
	   this after checking for duplicates so we don't get a false match */
	if(event->needFields)
	{
		ILUInt32 attrs = IL_META_FIELDDEF_PRIVATE;
		ILField *field;
		if((event->modifiers & IL_META_METHODDEF_STATIC) != 0)
		{
			attrs |= IL_META_FIELDDEF_STATIC;
		}
		field = ILFieldCreate(classInfo, 0, name, attrs);
		if(!field)
		{
			CCOutOfMemory();
		}
		ILMemberSetSignature((ILMember *)field, eventType);
	}
}

/*
 * Create an event definition.
 */
static void CreateEvent(ILGenInfo *info, ILClass *classInfo,
						ILNode_EventDeclaration *event)
{
	ILNode_ListIter iter;
	ILNode *decl;
	ILType *eventType;

	/* Get the event type and check that it is a delegate */
	eventType = CSSemType(event->type, info, &(event->type));
	if(!ILTypeIsDelegate(eventType))
	{
		CCErrorOnLine(yygetfilename(event), yygetlinenum(event),
  			"`%s' is not a delegate type", CSTypeToName(eventType));
	}

	/* Process the event declarators */
	if(yyisa(event->eventDeclarators, ILNode_EventDeclarator))
	{
		/* Create the methods for the event declarator */
		CreateEventDecl(info, classInfo, event, eventType,
					    (ILNode_EventDeclarator *)(event->eventDeclarators));
	}
	else
	{
		/* Scan the list and create the methods that we require */
		ILNode_ListIter_Init(&iter, event->eventDeclarators);
		while((decl = ILNode_ListIter_Next(&iter)) != 0)
		{
			CreateEventDecl(info, classInfo, event, eventType,
							(ILNode_EventDeclarator *)decl);
		}
	}
}

/*
 * Create a delegate member definition.
 */
static void CreateDelegateMember(ILGenInfo *info, ILClass *classInfo,
								 ILNode_DelegateMemberDeclaration *member)
{
	ILMethod *method;
	ILType *signature;
	ILNode_MethodDeclaration *decl;

	/* Create the delegate constructor */
	method = ILMethodCreate(classInfo, 0, ".ctor",
						    IL_META_METHODDEF_PUBLIC |
						    IL_META_METHODDEF_HIDE_BY_SIG |
						    IL_META_METHODDEF_SPECIAL_NAME |
						    IL_META_METHODDEF_RT_SPECIAL_NAME);
	if(!method)
	{
		CCOutOfMemory();
	}
	member->ctorMethod = method;
	signature = ILTypeCreateMethod(info->context, ILType_Void);
	if(!signature)
	{
		CCOutOfMemory();
	}
	if(!ILTypeAddParam(info->context, signature,
					   ILFindSystemType(info, "Object")))
	{
		CCOutOfMemory();
	}
	if(!ILTypeAddParam(info->context, signature, ILType_UInt))
	{
		CCOutOfMemory();
	}
	ILTypeSetCallConv(signature, IL_META_CALLCONV_HASTHIS);
	ILMethodSetCallConv(method, IL_META_CALLCONV_HASTHIS);
	ILMemberSetSignature((ILMember *)method, signature);
	ILMethodSetImplAttrs(method, ~((ILUInt32)0),
						 IL_META_METHODIMPL_RUNTIME);
	if(!ILParameterCreate(method, 0, "object", 0, 1))
	{
		CCOutOfMemory();
	}
	if(!ILParameterCreate(method, 0, "method", 0, 2))
	{
		CCOutOfMemory();
	}

	/* Create the "Invoke" method */
	decl = (ILNode_MethodDeclaration *)ILNode_MethodDeclaration_create
		(0, IL_META_METHODDEF_PUBLIC |
			IL_META_METHODDEF_VIRTUAL |
			IL_META_METHODDEF_NEW_SLOT |
			IL_META_METHODDEF_HIDE_BY_SIG,
		 member->returnType,
		 ILQualIdentSimple(ILInternString("Invoke", -1).string),
		 member->params, 0);
	CreateMethod(info, classInfo, decl);
	method = member->invokeMethod = decl->methodInfo;
	if(method)
	{
		ILMethodSetImplAttrs(method, ~((ILUInt32)0),
							 IL_META_METHODIMPL_RUNTIME);
	}

	/* TODO: asynchronous interface for delegates */
}

/*
 * Create the members of a class node.
 */
static void CreateMembers(ILGenInfo *info, ILScope *globalScope,
						  ILNode *classNode)
{
	ILClass *classInfo;
	ILNode *body;
	ILScope *scope;
	ILScope *savedScope;
	ILNode *savedClass;
	ILNode *savedNamespace;
	ILNode_ListIter iterator;
	ILNode *member;

	/* Get the class information block, and bail out if not defined */
	classInfo = ((ILNode_ClassDefn *)classNode)->classInfo;
	if(!classInfo || classInfo == ((ILClass *)1) ||
	   classInfo == ((ILClass *)2))
	{
		return;
	}

	/* Get the class body and the scope it is declared within */
	body = ((ILNode_ClassDefn *)classNode)->body;
	if(body && yykind(body) == yykindof(ILNode_ScopeChange))
	{
		scope = ((ILNode_ScopeChange *)body)->scope;
		body = ((ILNode_ScopeChange *)body)->body;
	}
	else
	{
		scope = globalScope;
	}

	/* Set the new scope for use by the semantic analysis routines */
	savedScope = info->currentScope;
	info->currentScope = scope;
	savedClass = info->currentClass;
	info->currentClass = classNode;
	savedNamespace = info->currentNamespace;
	info->currentNamespace = ((ILNode_ClassDefn *)classNode)->namespaceNode;

	/* Iterate over the member definitions in the class body */
	ILNode_ListIter_Init(&iterator, body);
	while((member = ILNode_ListIter_Next(&iterator)) != 0)
	{
		if(yykind(member) == yykindof(ILNode_FieldDeclaration))
		{
			CreateField(info, classInfo, (ILNode_FieldDeclaration *)member);
		}
		else if(yykind(member) == yykindof(ILNode_MethodDeclaration))
		{
			CreateMethod(info, classInfo,
						 (ILNode_MethodDeclaration *)member);
		}
		else if(yykind(member) == yykindof(ILNode_EnumMemberDeclaration))
		{
			CreateEnumMember(info, classInfo,
							 (ILNode_EnumMemberDeclaration *)member);
		}
		else if(yykind(member) == yykindof(ILNode_PropertyDeclaration))
		{
			CreateProperty(info, classInfo,
						   (ILNode_PropertyDeclaration *)member);
		}
		else if(yykind(member) == yykindof(ILNode_EventDeclaration))
		{
			CreateEvent(info, classInfo,
						(ILNode_EventDeclaration *)member);
		}
		else if(yykind(member) == yykindof(ILNode_DelegateMemberDeclaration))
		{
			CreateDelegateMember(info, classInfo,
								 (ILNode_DelegateMemberDeclaration *)member);
		}
		else if(yykind(member) == yykindof(ILNode_ClassDefn))
		{
			CreateMembers(info, globalScope, member);
		}
		else
		{
			CCErrorOnLine(yygetfilename(member), yygetlinenum(member),
				  "internal error - do not know how to declare this member");
		}
	}

	/* Return to the original scope */
	info->currentScope = savedScope;
	info->currentClass = savedClass;
	info->currentNamespace = savedNamespace;
}

/*
 * Scan all types and their nested children to declare them.
 */
static void DeclareTypes(ILGenInfo *info, ILScope *parentScope,
						 ILNode *tree, ILNode_List *list,
						 ILNode_ClassDefn *nestedParent)
{
	ILNode_ListIter iterator;
	ILNode *child;
	ILNode_ClassDefn *defn;
	ILScope *scope;
	ILNode *origDefn;
	const char *name;
	const char *namespace;
	int error;

	ILNode_ListIter_Init(&iterator, tree);
	while((child = ILNode_ListIter_Next(&iterator)) != 0)
	{
		if(yykind(child) == yykindof(ILNode_ClassDefn))
		{
			defn = (ILNode_ClassDefn *)child;
			defn->nestedParent = nestedParent;
			name = defn->name;
			namespace = defn->namespace;
			if(nestedParent || (namespace && *namespace == '\0'))
			{
				namespace = 0;
			}
			error = ILScopeDeclareType(parentScope, child,
								   	   name, namespace, &scope,
								   	   &origDefn);
			if(error != IL_SCOPE_ERROR_OK)
			{
				/* Could not declare the type in the global scope */
				switch(error)
				{
					case IL_SCOPE_ERROR_IMPORT_CONFLICT:
					{
						CCErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"`%s%s%s' conflicts with imported type",
								(namespace ? namespace : ""),
								(namespace ? "." : ""), name);
					}
					break;

					case IL_SCOPE_ERROR_REDECLARED:
					{
						CCErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"`%s%s%s' already declared",
								(namespace ? namespace : ""),
								(namespace ? "." : ""), name);
						CCErrorOnLine(yygetfilename(origDefn),
									  yygetlinenum(origDefn),
									  "previous declaration here");
					}
					break;

					case IL_SCOPE_ERROR_CANT_CREATE_NAMESPACE:
					{
						CCErrorOnLine(yygetfilename(child), yygetlinenum(child),
									  "`%s' is not a valid namespace",
									  namespace);
					}
					break;

					case IL_SCOPE_ERROR_NAME_IS_NAMESPACE:
					{
						CCErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"`%s%s%s' cannot be declared as a type",
								(namespace ? namespace : ""),
								(namespace ? "." : ""), name);
						CCErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"because it is already declared as namespace");
					}
					break;

					default:
					{
						CCErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"`%s%s%s' cannot be declared as a type",
								(namespace ? namespace : ""),
								(namespace ? "." : ""), name);
						CCErrorOnLine(yygetfilename(child), yygetlinenum(child),
								"because it is already declared elsewhere");
					}
					break;
				}
			}
			else
			{
				/* Declare nested types */
				DeclareTypes(info, scope, defn->body, list, defn);

				/* Replace the class body with a scoped body */
				defn->body = ILNode_ScopeChange_create(scope, defn->body);

				/* Add the type to the end of the new top-level list */
				if(!nestedParent)
				{
					ILNode_List_Add(list, child);
				}
			}
		}
	}
}

ILNode *CSTypeGather(ILGenInfo *info, ILScope *globalScope, ILNode *tree)
{
	ILNode_ListIter iterator;
	ILNode *child;
	ILNode_List *list;
	ILNode *systemObject;

	/* Create a new top-level list for the program */
	list = (ILNode_List *)ILNode_List_create();

	/* Scan all top-level types to declare them */
	DeclareTypes(info, globalScope, tree, list, 0);

	/* Create the top-level types, and re-order them so that the
	   base types are listed before types that inherit them */
	tree = (ILNode *)list;
	list = (ILNode_List *)ILNode_List_create();
	systemObject = ILQualIdentTwo("System", "Object");
	ILNode_ListIter_Init(&iterator, tree);
	while((child = ILNode_ListIter_Next(&iterator)) != 0)
	{
		CreateType(info, globalScope, list, systemObject, child);
	}

	/* Create the class members within each type */
	ILNode_ListIter_Init(&iterator, list);
	while((child = ILNode_ListIter_Next(&iterator)) != 0)
	{
		CreateMembers(info, globalScope, child);
	}

	/* Return the new top-level list to the caller */
	return (ILNode *)list;
}

#ifdef	__cplusplus
};
#endif
