/*
 * cs_attrs.c - Attribute handling.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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
 * Attribute target flags.
 */
#define	CS_ATTR_ASSEMBLY		0x0001
#define	CS_ATTR_MODULE			0x0002
#define	CS_ATTR_CLASS			0x0004
#define	CS_ATTR_STRUCT			0x0008
#define	CS_ATTR_ENUM			0x0010
#define	CS_ATTR_CONSTRUCTOR		0x0020
#define	CS_ATTR_METHOD			0x0040
#define	CS_ATTR_PROPERTY		0x0080
#define	CS_ATTR_FIELD			0x0100
#define	CS_ATTR_EVENT			0x0200
#define	CS_ATTR_INTERFACE		0x0400
#define	CS_ATTR_PARAMETER		0x0800
#define	CS_ATTR_DELEGATE		0x1000
#define	CS_ATTR_RETURNVALUE		0x2000

/*
 * Process the attributes on a program item.
 */
static void ProcessAttrs(ILGenInfo *info, ILProgramItem *item,
						 ILNode_AttributeTree *tree, int target)
{
	/* Bail out if there is no item or attributes */
	if(!item || !tree)
	{
		return;
	}

	/* TODO */
}

void CSClassInitAttrs(ILGenInfo *info, ILClass *classInfo)
{
	ILNode_ClassDefn *defn;
	ILNode_AttributeTree *tree;
	ILImplements *impl;
	ILNode *body, *node;
	ILScope *scope, *savedScope;
	ILScope *parentScope;
	ILNode *savedClass;
	ILNode *savedNamespace;
	ILNode_ListIter iter;
	int target;
	ILNode_MethodDeclaration *methodDecl;
	ILMethod *method;
	ILNode_PropertyDeclaration *propDecl;
	ILNode_EnumMemberDeclaration *enumDecl;

	/* Bail out if we have reached the top of the class hierarchy */
	if(!classInfo)
	{
		return;
	}

	/* Resolve the class reference */
	classInfo = ILClassResolve(classInfo);

	/* If we don't have a node associated with the class, then
	   assume that it was imported from a library and already
	   has any attributes that may be relevant */
	defn = (ILNode_ClassDefn *)ILClassGetUserData(classInfo);
	if(!defn)
	{
		return;
	}

	/* Find the attribute tree for the class */
	tree = (ILNode_AttributeTree *)(defn->attributes);
	if(!tree)
	{
		defn->attributes = ILNode_AttributeTree_create(0);
		tree = (ILNode_AttributeTree *)(defn->attributes);
		yysetfilename(tree, yygetfilename(defn));
		yysetlinenum(tree, yygetlinenum(defn));
	}

	/* Bail out if we have already visited this class, or the
	   attribute definitions are recursive */
	if(tree->mode != ILAttrMode_NotVisited)
	{
		if(tree->mode == ILAttrMode_Processing)
		{
			CCErrorOnLine(yygetfilename(tree), yygetlinenum(tree),
						  "attribute definition is recursive");
		}
		return;
	}

	/* Mark the class as being processed for attributes */
	tree->mode = ILAttrMode_Processing;

	/* Get the class body and the scope it is declared within */
	body = defn->body;
	if(body && yykind(body) == yykindof(ILNode_ScopeChange))
	{
		scope = ((ILNode_ScopeChange *)body)->scope;
		body = ((ILNode_ScopeChange *)body)->body;
		parentScope = ILScopeGetParent(scope);
	}
	else
	{
		scope = CCGlobalScope;
		parentScope = scope;
	}

	/* Set the new scope to the parent of the class's body scope */
	savedScope = info->currentScope;
	savedClass = info->currentClass;
	savedNamespace = info->currentNamespace;
	info->currentScope = parentScope;
	info->currentClass = (ILNode *)(defn->nestedParent);
	if(info->currentClass)
	{
		info->currentNamespace =
			((ILNode_ClassDefn *)(info->currentClass))->namespaceNode;
	}
	else
	{
		info->currentNamespace = 0;
	}

	/* Process the attributes for the parent class and the interfaces */
	CSClassInitAttrs(info, ILClass_ParentRef(classInfo));
	impl = 0;
	while((impl = ILClassNextImplements(classInfo, impl)) != 0)
	{
		CSClassInitAttrs(info, ILImplementsGetInterface(impl));
	}

	/* Determine the target kind for the class */
	if(ILClassIsValueType(classInfo))
	{
		if(ILTypeIsEnum(ILType_FromValueType(classInfo)))
		{
			target = CS_ATTR_ENUM;
		}
		else
		{
			target = CS_ATTR_STRUCT;
		}
	}
	else if(ILTypeIsDelegate(ILType_FromClass(classInfo)))
	{
		target = CS_ATTR_DELEGATE;
	}
	else if(ILClass_IsInterface(classInfo))
	{
		target = CS_ATTR_INTERFACE;
	}
	else
	{
		target = CS_ATTR_CLASS;
	}

	/* Process the attributes for the class itself */
	ProcessAttrs(info, (ILProgramItem *)classInfo, tree, target);

	/* Enter the member scope for the class */
	info->currentScope = scope;
	info->currentClass = (ILNode *)defn;
	info->currentNamespace = defn->namespaceNode;

	/* Scan all class members and process their attributes */
	ILNode_ListIter_Init(&iter, body);
	while((node = ILNode_ListIter_Next(&iter)) != 0)
	{
		if(yyisa(node, ILNode_MethodDeclaration))
		{
			/* Process the attributes for a method declaration */
			methodDecl = (ILNode_MethodDeclaration *)node;
			method = methodDecl->methodInfo;
			if(method)
			{
				if(ILMethodIsConstructor(method) ||
				   ILMethodIsStaticConstructor(method))
				{
					ProcessAttrs(info, (ILProgramItem *)method,
								 (ILNode_AttributeTree *)
								 		(methodDecl->attributes),
								 CS_ATTR_CONSTRUCTOR);
				}
				else
				{
					ProcessAttrs(info, (ILProgramItem *)method,
								 (ILNode_AttributeTree *)
								 		(methodDecl->attributes),
								 CS_ATTR_METHOD);
				}

				/* TODO: parameter and return attributes */
			}
		}
		else if(yyisa(node, ILNode_FieldDeclaration))
		{
			/* TODO */
		}
		else if(yyisa(node, ILNode_PropertyDeclaration))
		{
			/* Process the attributes for a property declaration */
			propDecl = (ILNode_PropertyDeclaration *)node;
			ProcessAttrs(info, (ILProgramItem *)(propDecl->propertyInfo),
						 (ILNode_AttributeTree *)(propDecl->attributes),
						 CS_ATTR_PROPERTY);

			/* TODO: parameter and return attributes */
		}
		else if(yyisa(node, ILNode_EventDeclaration))
		{
			/* TODO */
		}
		else if(yyisa(node, ILNode_EnumMemberDeclaration))
		{
			/* Process the attributes for an enumeration member declaration */
			enumDecl = (ILNode_EnumMemberDeclaration *)node;
			ProcessAttrs(info, (ILProgramItem *)(enumDecl->fieldInfo),
						 (ILNode_AttributeTree *)(enumDecl->attributes),
						 CS_ATTR_FIELD);
		}
	}

	/* Return to the original scope */
	info->currentScope = savedScope;
	info->currentClass = savedClass;
	info->currentNamespace = savedNamespace;

	/* The class is no longer being processed for attributes */
	tree->mode = ILAttrMode_Done;
}

void CSMemberInitAttrs(ILGenInfo *info, ILMember *member)
{
	if(member)
	{
		member = ILMemberResolve(member);
		CSClassInitAttrs(info, ILMember_Owner(member));
	}
}

#ifdef	__cplusplus
};
#endif
