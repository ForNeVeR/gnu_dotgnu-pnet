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

	/* Set the namespace to use for resolving type names */
	savedNamespace = info->currentNamespace;
	info->currentNamespace = defn->namespaceNode;

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
			CSErrorOnLine(yygetfilename(type), yygetlinenum(type),
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

	/* Restore the namespace */
	info->currentNamespace = savedNamespace;

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
		CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
					  "member conflicts with a type name in the same scope");
	}
}

/*
 * Search for a member with a specific name to do duplicate testing.
 */
static ILMember *FindMemberByName(ILClass *classInfo, const char *name)
{
	ILClass *scope = classInfo;
	ILMember *member;
	while(classInfo != 0)
	{
		member = 0;
		while((member = ILClassNextMember(classInfo, member)) != 0)
		{
			if(!strcmp(ILMember_Name(member), name) &&
			   ILMemberAccessible(member, scope))
			{
				return member;
			}
		}
		classInfo = ILClass_Parent(classInfo);
	}
	return 0;
}

/*
 * Report duplicate definitions.
 */
static void ReportDuplicates(ILNode *node, ILMember *newMember,
							 ILMember *existingMember, ILClass *classInfo,
							 ILUInt32 modifiers)
{
	/* TODO: we need better error messages here */

	if(ILMember_Owner(existingMember) == classInfo)
	{
		/* The duplicate is in the same class */
		CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
		  			  "declaration conflicts with an existing member");
	}
	else if((modifiers & CS_SPECIALATTR_NEW) == 0)
	{
		/* The duplicate is in a parent class, and "new" wasn't specified */
		CSErrorOnLine(yygetfilename(node), yygetlinenum(node),
		  "declaration hides an inherited member, and `new' was not present");
	}
}

/*
 * Report a warning for an unnecessary "new" keyword on a declaration.
 */
static void ReportUnnecessaryNew(ILNode *node)
{
	CSWarningOnLine(yygetfilename(node), yygetlinenum(node),
			        "declaration includes unnecessary `new' keyword");
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
				CSOutOfMemory();
			}
			tempType = ILTypeAddModifiers(info->context, modifier, tempType);
		}
	}

	/* Iterator over the field declarators and create each field in turn */
	ILNode_ListIter_Init(&iterator, field->fieldDeclarators);
	while((decl = (ILNode_FieldDeclarator *)
						ILNode_ListIter_Next(&iterator)) != 0)
	{
		/* Get the name of the field */
		name = ILQualIdentName(decl->name, 0);

		/* Look for duplicates */
		member = FindMemberByName(classInfo, name);

		/* Create the field information block */
		fieldInfo = ILFieldCreate(classInfo, 0, name,
								  (field->modifiers & 0xFFFF));
		if(!fieldInfo)
		{
			CSOutOfMemory();
		}
		decl->fieldInfo = fieldInfo;
		ILMemberSetSignature((ILMember *)fieldInfo, tempType);

		/* Report on duplicates */
		if(member)
		{
			ReportDuplicates((ILNode *)decl, (ILMember *)fieldInfo,
							 member, classInfo, field->modifiers);
		}
		else if((field->modifiers & CS_SPECIALATTR_NEW) != 0)
		{
			ReportUnnecessaryNew((ILNode *)decl);
		}

		/* Add the field to the current scope */
		AddMemberToScope(info->currentScope, IL_SCOPE_FIELD,
						 name, (ILMember *)fieldInfo, decl->name);
	}
}

/*
 * Create a method definition.
 */
static void CreateMethod(ILGenInfo *info, ILClass *classInfo,
						 ILNode_MethodDeclaration *method)
{
	char *name;
	ILType *tempType;
	ILMethod *methodInfo;
	ILType *signature;
	ILNode_ListIter iterator;
	ILNode *param;
	ILNode_FormalParameter *fparam;
	ILUInt32 paramNum;
	ILParameter *parameter;
	
	/* Get the name of the method */
	if(yykind(method->name) == yykindof(ILNode_Identifier))
	{
		/* Simple method name */
		name = ILQualIdentName(method->name, 0);
	}
	else
	{
		/* Qualified method name that overrides some interface method (TODO) */
		name = ILQualIdentName(method->name, 0);
	}

	/* Create the method information block */
	methodInfo = ILMethodCreate(classInfo, 0, name,
								(method->modifiers & 0xFFFF));
	if(!methodInfo)
	{
		CSOutOfMemory();
	}
	method->methodInfo = methodInfo;

	/* Get the return type */
	tempType = CSSemTypeVoid(method->type, info, &(method->type));

	/* Create the method signature type */
	signature = ILTypeCreateMethod(info->context, tempType);
	if(!signature)
	{
		CSOutOfMemory();
	}
	if((method->modifiers & IL_META_METHODDEF_STATIC) == 0)
	{
		ILTypeSetCallConv(signature, IL_META_CALLCONV_HASTHIS);
		ILMethodSetCallConv(methodInfo, IL_META_CALLCONV_HASTHIS);
	}

	/* Create the parameters for the method */
	paramNum = 1;
	ILNode_ListIter_Init(&iterator, method->params);
	while((param = ILNode_ListIter_Next(&iterator)) != 0)
	{
		/* Get the type of the parameter */
		fparam = (ILNode_FormalParameter *)param;
		tempType = CSSemType(fparam->type, info, &(fparam->type));

		/* Add a "byref" node to the type if "out" or "ref" */
		if(fparam->pmod == ILParamMod_out ||
		   fparam->pmod == ILParamMod_ref)
		{
			tempType = ILTypeCreateRef(info->context,
									   IL_TYPE_COMPLEX_BYREF, tempType);
			if(!tempType)
			{
				CSOutOfMemory();
			}
			if(info->outputIsJava)
			{
				CSErrorOnLine(yygetfilename(method), yygetlinenum(method),
				  	"`%s' parameters not permitted when compiling "
						"to Java bytecode",
				    (fparam->pmod == ILParamMod_out ? "out" : "ref"));
			}
		}

		/* Add the parameter type to the method signature */
		if(!ILTypeAddParam(info->context, signature, tempType))
		{
			CSOutOfMemory();
		}

		/* Create a parameter definition in the metadata to record the name */
		parameter = ILParameterCreate
				(methodInfo, 0, ILQualIdentName(fparam->name, 0),
			     ((fparam->pmod == ILParamMod_out) ? IL_META_PARAMDEF_OUT : 0),
				 paramNum);
		if(!parameter)
		{
			CSOutOfMemory();
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

	/* Set the signature for the method */
	ILMemberSetSignature((ILMember *)methodInfo, signature);

	/* Add the method to the current scope */
	AddMemberToScope(info->currentScope, IL_SCOPE_METHOD,
					 name, (ILMember *)methodInfo, method->name);
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
		CSErrorOnLine(yygetfilename(enumMember), yygetlinenum(enumMember),
			  "the identifier `value__' is reserved in enumerated types");
		return;
	}

	/* Look for duplicates */
	member = FindMemberByName(classInfo, name);

	/* Create the field information block */
	fieldInfo = ILFieldCreate(classInfo, 0, name,
							  IL_META_FIELDDEF_PUBLIC |
							  IL_META_FIELDDEF_STATIC |
							  IL_META_FIELDDEF_LITERAL);
	if(!fieldInfo)
	{
		CSOutOfMemory();
	}
	enumMember->fieldInfo = fieldInfo;
	ILMemberSetSignature((ILMember *)fieldInfo, tempType);

	/* Report on duplicates within this class only */
	if(member && ILMember_Owner(member) == classInfo)
	{
		ReportDuplicates((ILNode *)enumMember, (ILMember *)fieldInfo,
						 member, classInfo, 0);
	}
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
		/* Simple method name */
		name = ILQualIdentName(property->name, 0);
	}
	else
	{
		/* Qualified property name that overrides some
		   interface property (TODO) */
		name = ILQualIdentName(property->name, 0);
	}

	/* Look for duplicates */
	member = FindMemberByName(classInfo, name);

	/* Get the property type */
	propType = CSSemType(property->type, info, &(property->type));

	/* Create the property signature type */
	signature = ILTypeCreateProperty(info->context, propType);
	if(!signature)
	{
		CSOutOfMemory();
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
		tempType = CSSemType(fparam->type, info, &(fparam->type));

		/* Add the parameter type to the property signature */
		if(!ILTypeAddParam(info->context, signature, tempType))
		{
			CSOutOfMemory();
		}

		/* Move on to the next parameter */
		++paramNum;
	}

	/* Create the property information block */
	propertyInfo = ILPropertyCreate(classInfo, 0, name, 0, signature);
	if(!propertyInfo)
	{
		CSOutOfMemory();
	}
	property->propertyInfo = propertyInfo;

	/* Add the method semantics to the property */
	if(property->getAccessor)
	{
		if(!ILMethodSemCreate((ILProgramItem *)propertyInfo, 0,
					  IL_META_METHODSEM_GETTER,
					  ((ILNode_MethodDeclaration *)(property->getAccessor))
					  		->methodInfo))
		{
			CSOutOfMemory();
		}
	}
	if(property->setAccessor)
	{
		if(!ILMethodSemCreate((ILProgramItem *)propertyInfo, 0,
					  IL_META_METHODSEM_SETTER,
					  ((ILNode_MethodDeclaration *)(property->setAccessor))
					  		->methodInfo))
		{
			CSOutOfMemory();
		}
	}

	/* Report on duplicates */
	if(member)
	{
		ReportDuplicates((ILNode *)property, (ILMember *)propertyInfo,
						 member, classInfo, property->modifiers);
	}
	else if((property->modifiers & CS_SPECIALATTR_NEW) != 0)
	{
		ReportUnnecessaryNew((ILNode *)property);
	}

	/* Add the property to the current scope */
	AddMemberToScope(info->currentScope, IL_SCOPE_PROPERTY,
					 name, (ILMember *)propertyInfo, property->name);
}

/*
 * Create an event definition.
 */
static void CreateEvent(ILGenInfo *info, ILClass *classInfo,
						ILNode_EventDeclaration *event)
{
	/* TODO */
}

/*
 * Create a delegate member definition.
 */
static void CreateDelegateMember(ILGenInfo *info, ILClass *classInfo,
								 ILNode_DelegateMemberDeclaration *member)
{
	/* TODO */
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
		else if(yykind(member) != yykindof(ILNode_ClassDefn))
		{
			/* TODO: nested classes */
			CSErrorOnLine(yygetfilename(member), yygetlinenum(member),
				  "internal error - do not know how to declare this member");
		}
	}

	/* Return to the original scope */
	info->currentScope = savedScope;
	info->currentClass = savedClass;
	info->currentNamespace = savedNamespace;
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
