/*
 * cs_ident.c - Semantic analysis for C# identifier node types.
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
 * Perform semantic analysis for qualified identifiers.
 */
DECLARE_SEM(QualIdent)
{
}

/*
 * Perform semantic analysis for member access operators.
 */
DECLARE_SEM(MemberAccess)
{
}

/*
 * Perform semantic analysis for base member access operators.
 */
DECLARE_SEM(BaseAccess)
{
}

/*
 * Perform semantic analysis for base element access operators.
 */
DECLARE_SEM(BaseElement)
{
}

/*
 * Perform semantic analysis for an unsafe pointer field dereference.
 */
DECLARE_SEM(DerefField)
{
}

/*
 * Perform semantic analysis for an array type.
 */
DECLARE_SEM(ArrayType)
{
}

/*
 * Perform semantic analysis for a pointer type.
 */
DECLARE_SEM(PtrType)
{
}

/*
 * Perform semantic analysis for a void pointer type.
 */
DECLARE_SEM(VoidPtrType)
{
}

/*
 * Node class for qualified identifiers.  All of the functions
 * are unimplemented because the node will be replaced with a
 * new node that reflects its real type (local, parameter,
 * method, field, etc) before it is passed to the code generator.
 */
ILNodeClass const ILNodeClass_QualIdent = {
	"qualified_identifier",
	ILNodeNotImpGetType,
	ILNodeNotImpGenDiscard,
	ILNodeNotImpGenValue,
	ILNodeNotImpGenThen,
	ILNodeNotImpGenElse,
	ILNodeNotImpPrepare,
	ILNodeNotImpGetAndPrepare,
	ILNodeNotImpStore,
	ILNodeNotImpEvalConst,
};

/*
 * Node class for member access.  Similar implementation comments apply.
 */
ILNodeClass const ILNodeClass_MemberAccess = {
	"member_access",
	ILNodeNotImpGetType,
	ILNodeNotImpGenDiscard,
	ILNodeNotImpGenValue,
	ILNodeNotImpGenThen,
	ILNodeNotImpGenElse,
	ILNodeNotImpPrepare,
	ILNodeNotImpGetAndPrepare,
	ILNodeNotImpStore,
	ILNodeNotImpEvalConst,
};

/*
 * Node class for base member access.  Similar implementation comments apply.
 */
ILNodeClass const ILNodeClass_BaseAccess = {
	"base_access",
	ILNodeNotImpGetType,
	ILNodeNotImpGenDiscard,
	ILNodeNotImpGenValue,
	ILNodeNotImpGenThen,
	ILNodeNotImpGenElse,
	ILNodeNotImpPrepare,
	ILNodeNotImpGetAndPrepare,
	ILNodeNotImpStore,
	ILNodeNotImpEvalConst,
};

/*
 * Node class for base element access.  Similar implementation comments apply.
 */
ILNodeClass const ILNodeClass_BaseElement = {
	"base_element",
	ILNodeNotImpGetType,
	ILNodeNotImpGenDiscard,
	ILNodeNotImpGenValue,
	ILNodeNotImpGenThen,
	ILNodeNotImpGenElse,
	ILNodeNotImpPrepare,
	ILNodeNotImpGetAndPrepare,
	ILNodeNotImpStore,
	ILNodeNotImpEvalConst,
};

/*
 * Node class for pointer field dereferences.
 */
ILNodeClass const ILNodeClass_DerefField = {
	"deref_field",
	ILNodeNotImpGetType,
	ILNodeNotImpGenDiscard,
	ILNodeNotImpGenValue,
	ILNodeNotImpGenThen,
	ILNodeNotImpGenElse,
	ILNodeNotImpPrepare,
	ILNodeNotImpGetAndPrepare,
	ILNodeNotImpStore,
	ILNodeNotImpEvalConst,
};

/*
 * Node class for array types.
 */
ILNodeClass const ILNodeClass_ArrayType = {
	"array_type",
	ILNodeNotImpGetType,
	ILNodeNotImpGenDiscard,
	ILNodeNotImpGenValue,
	ILNodeNotImpGenThen,
	ILNodeNotImpGenElse,
	ILNodeNotImpPrepare,
	ILNodeNotImpGetAndPrepare,
	ILNodeNotImpStore,
	ILNodeNotImpEvalConst,
};

/*
 * Node class for pointer types.
 */
ILNodeClass const ILNodeClass_PtrType = {
	"ptr_type",
	ILNodeNotImpGetType,
	ILNodeNotImpGenDiscard,
	ILNodeNotImpGenValue,
	ILNodeNotImpGenThen,
	ILNodeNotImpGenElse,
	ILNodeNotImpPrepare,
	ILNodeNotImpGetAndPrepare,
	ILNodeNotImpStore,
	ILNodeNotImpEvalConst,
};

/*
 * Node class for void pointer types.
 */
ILNodeClass const ILNodeClass_VoidPtrType = {
	"void_ptr_type",
	ILNodeNotImpGetType,
	ILNodeNotImpGenDiscard,
	ILNodeNotImpGenValue,
	ILNodeNotImpGenThen,
	ILNodeNotImpGenElse,
	ILNodeNotImpPrepare,
	ILNodeNotImpGetAndPrepare,
	ILNodeNotImpStore,
	ILNodeNotImpEvalConst,
};

char *CSQualIdentName(ILNode *node, int asmForm)
{
	if(!node || node->nodeClass != &ILNodeClass_QualIdent)
	{
		return 0;
	}
	else if((node->flags & IL_NODE_NAME) != 0)
	{
		if(asmForm && node->un.name[0] >= 'a' && node->un.name[0] <= 'z')
		{
			/* The name might clash with an assembler keyword
			   or instruction name, so we must quote it */
			ILIntString quote = ILInternString("'", 1);
			ILIntString name;
			name.string = node->un.name;
			name.len = strlen(node->un.name);
			return (ILInternAppendedString
						(quote, ILInternAppendedString(name, quote))).string;
		}
		return node->un.name;
	}
	else if((node->flags & 3) != IL_NODE_TWO_SUBNODES)
	{
		return 0;
	}
	else if(!ILNodeExpr2(node) ||
			(ILNodeExpr2(node)->flags & IL_NODE_NAME) == 0)
	{
		return 0;
	}
	else
	{
		return CSQualIdentAppend(CSQualIdentName(ILNodeExpr1(node), asmForm),
								 ILNodeExpr2(node)->un.name);
	}
}

char *CSQualIdentAppend(char *name1, char *name2)
{
	ILIntString str1;
	ILIntString str2;
	ILIntString str3;
	if(!name1 || *name1 == '\0' || !name2 || *name2 == '\0')
	{
		return 0;
	}
	str1.string = name1;
	str1.len = strlen(name1);
	str2.string = ".";
	str2.len = 1;
	str3.string = name2;
	str3.len = strlen(name2);
	return (ILInternAppendedString
				(str1, ILInternAppendedString(str2, str3))).string;
}

int CSIsQualIdent(ILNode *node)
{
	if(!node || node->nodeClass != &ILNodeClass_QualIdent)
	{
		return 0;
	}
	else if((node->flags & IL_NODE_NAME) != 0)
	{
		return 1;
	}
	else if((node->flags & 3) != IL_NODE_TWO_SUBNODES)
	{
		return 0;
	}
	else if(!ILNodeExpr2(node) ||
			(ILNodeExpr2(node)->flags & IL_NODE_NAME) == 0)
	{
		return 0;
	}
	else
	{
		return CSIsQualIdent(ILNodeExpr1(node));
	}
}

#ifdef	__cplusplus
};
#endif
