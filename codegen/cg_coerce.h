/*
 * cg_coerce.h - Coercions and casts.
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

#ifndef	__CODEGEN_CG_COERCE_H__
#define	__CODEGEN_CG_COERCE_H__

#ifdef	__cplusplus
extern	"C" {
#endif

typedef struct _tagILBuiltinType ILBuiltinType;

struct _tagILBuiltinType
{
	const char	   *namespace;			/* Namespace for the type */
	const char	   *name;				/* Name of the type */
	ILType		   *type;				/* Built-in element type */
	ILMachineType	valueType;			/* Value type used by runtime */

};

typedef void (*ILOpFunc)(ILNode *node, ILGenInfo *info);

typedef struct
{
	const ILBuiltinType  *outtype;
	const ILBuiltinType  *intype1;
	const ILBuiltinType  *intype2;

} ILOperator;

typedef struct
{
	const ILBuiltinType  *outtype;
	int				      explicit;

} ILConversion;

/*
 * Find a standard conversion between two types.
 */
const ILConversion *ILFindConversion(ILType *fromType, ILType *toType,
								     int explicit);

/*
 * Apply a standard conversion to a node.
 */
void ILApplyConversion(ILGenInfo *info, ILNode *node, ILNode **parent,
					   const ILConversion *conv);

/*
 * Find a standard unary operator for a particular type.
 */
const ILOperator *ILFindUnaryOperator(const ILOperator *table,
								 	  ILType *argType1);

/*
 * Apply a standard unary operator to a node.
 */
void ILApplyUnaryOperator(ILGenInfo *info, ILNode *node, ILNode **parent,
					      const ILOperator *oper);

/*
 * Find a standard binary operator for a particular pair of types.
 */
const ILOperator *ILFindBinaryOperator(const ILOperator *table,
								 	   ILType *argType1, ILType *argType2);

/*
 * Apply a standard binary operator to a node.
 */
void ILApplyBinaryOperator(ILGenInfo *info, ILNode *node, ILNode **parent,
					       const ILOperator *oper);

/*
 * Determine if it is possible to coerce implicitly
 * from one type to another.
 */
int ILCanCoerce(ILContext *context, ILType *fromType, ILType *toType);

/*
 * Determine if it is possible to coerce a node implicitly
 * from one type to another.
 */
int ILCanCoerceNode(ILGenInfo *info, ILNode *node,
				    ILType *fromType, ILType *toType);

/*
 * Coerce a node from one type to another.  Returns zero if
 * the coercion is not possible.
 */
int ILCoerce(ILGenInfo *info, ILNode *node, ILNode **parent,
			 ILType *fromType, ILType *toType);

/*
 * Cast a node from one type to another.  Returns zero if the
 * cast is not possible.
 */
int ILCast(ILGenInfo *info, ILNode *node, ILNode **parent,
		   ILType *fromType, ILType *toType);

/*
 * Determine which of T1 and T2 results in a better conversion
 * from a type S.
 */
int ILBetterConversion(ILGenInfo *info, ILType *sType,
					   ILType *t1Type, ILType *t2Type);
#define	IL_BETTER_NEITHER		0
#define	IL_BETTER_T1			1
#define	IL_BETTER_T2			2

/*
 * Builtin operator tables.
 */
extern ILOperator const ILOp_Add[];
extern ILOperator const ILOp_Sub[];
extern ILOperator const ILOp_Mul[];
extern ILOperator const ILOp_Div[];
extern ILOperator const ILOp_Rem[];
extern ILOperator const ILOp_Neg[];
extern ILOperator const ILOp_UnaryPlus[];
extern ILOperator const ILOp_And[];
extern ILOperator const ILOp_Or[];
extern ILOperator const ILOp_Xor[];
extern ILOperator const ILOp_Not[];
extern ILOperator const ILOp_Shl[];
extern ILOperator const ILOp_Shr[];
extern ILOperator const ILOp_Eq[];
extern ILOperator const ILOp_Ne[];
extern ILOperator const ILOp_Lt[];
extern ILOperator const ILOp_Le[];
extern ILOperator const ILOp_Gt[];
extern ILOperator const ILOp_Ge[];

#ifdef	__cplusplus
};
#endif

#endif	/* __CODEGEN_CG_COERCE_H__ */
