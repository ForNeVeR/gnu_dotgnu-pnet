/*
 * cg_coerce.c - Coercions and casts.
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

#include "cg_nodes.h"
#include "cg_resolve.h"
#include "cg_coerce.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Conversion rules that result from looking for a coercion or cast.
 */
typedef struct
{
	/* Class to use to box the value prior to conversion */
	ILClass		*boxClass;

	/* Method to call to perform the conversion */
	ILMethod	*method;

	/* Builtin conversion */
	const ILConversion *builtin;

} ConvertRules;

/*
 * Find a user-defined conversion operator.
 */
static int FindConversion(ILGenInfo *info, ILClass *classInfo,
						  ILType *fromType, ILType *toType,
						  int explicit, ConvertRules *rules)
{
	ILMethod *method;

	/* Look for an implicit operator definition */
	method = ILResolveConversionOperator(info, classInfo, "op_Implicit",
										 fromType, toType);
	if(method)
	{
		rules->method = method;
		return 1;
	}

	/* Look for an explicit operator definition */
	if(explicit)
	{
		method = ILResolveConversionOperator(info, classInfo, "op_Explicit",
											 fromType, toType);
		if(method)
		{
			rules->method = method;
			return 1;
		}
	}

	/* No user-defined operator */
	return 0;
}

/*
 * Get the rules to be used to convert from one type to another.
 */
static int GetConvertRules(ILGenInfo *info, ILType *fromType,
						   ILType *toType, int explicit,
						   ConvertRules *rules)
{
	const ILConversion *conv;
	ILClass *classFrom;
	ILClass *classTo;

	/* Clear the rules */
	rules->boxClass = 0;
	rules->method = 0;
	rules->builtin = 0;

	/* Strip type prefixes before we start */
	fromType = ILTypeStripPrefixes(fromType);
	toType = ILTypeStripPrefixes(toType);

	/* If the types are identical at this point, then we are done */
	if(ILTypeIdentical(fromType, toType))
	{
		return 1;
	}

	/* We can never convert to the "null" type */
	if(toType == ILType_Null)
	{
		return 0;
	}

	/* If "fromType" is null, then "toType" must be a reference type */
	if(fromType == ILType_Null)
	{
		return ILTypeIsReference(toType);
	}

	/* Look for a builtin conversion */
	conv = ILFindConversion(fromType, toType, explicit);
	if(conv)
	{
		rules->builtin = conv;
		return 1;
	}

	/* Look for a user-defined conversion */
	if(ILType_IsClass(fromType))
	{
		if(FindConversion(info, ILType_ToClass(fromType),
					  	  fromType, toType, explicit, rules))
		{
			return 1;
		}
	}
	else if(ILType_IsValueType(fromType))
	{
		if(FindConversion(info, ILType_ToValueType(fromType),
					  	  fromType, toType, explicit, rules))
		{
			return 1;
		}
	}
	if(ILType_IsClass(toType))
	{
		if(FindConversion(info, ILType_ToClass(toType),
					  	  fromType, toType, explicit, rules))
		{
			return 1;
		}
	}
	else if(ILType_IsValueType(toType))
	{
		if(FindConversion(info, ILType_ToValueType(toType),
					  	  fromType, toType, explicit, rules))
		{
			return 1;
		}
	}

	/* We need reference types for the remainder of this function */
	if(!ILTypeIsReference(fromType) || !ILTypeIsReference(toType))
	{
		return 0;
	}

	/* Any reference type can be implicitly coerced to "Object" */
	if(ILTypeIsObjectClass(toType))
	{
		return 1;
	}

	/* "Object" can be explicitly converted into any reference type */
	if(ILTypeIsObjectClass(fromType) && explicit)
	{
		return 1;
	}

	/* Convert "fromType" and "toType" into their class versions */
	classFrom = ILTypeToClass(info, fromType);
	classTo = ILTypeToClass(info, toType);
	if(!classFrom || !classTo)
	{
		return 0;
	}

	/* See if "fromType" inherits from "toType", or vice versa */
	if(ILClassInheritsFrom(classFrom, classTo))
	{
		/* Implicit conversion to a base type */
		return 1;
	}
	if(explicit)
	{
		if(ILClassInheritsFrom(classTo, classFrom))
		{
			/* Explicit conversion to a descendent type */
			return 1;
		}
	}

	/* See if "fromType" implements "toType", or is an interface
	   that is derived from "toType" */
	if(ILClass_IsInterface(classTo))
	{
		if(ILClassImplements(classFrom, classTo))
		{
			/* Implicit conversion to an interface */
			return 1;
		}
	}

	/* Check array type compatibility */
	if(ILType_IsArray(fromType) && ILType_IsArray(toType))
	{
		/* The ranks must be equal, the element types must
		   be references, and there must be a valid conversion
		   between the element types */
		if(ILTypeGetRank(fromType) == ILTypeGetRank(toType))
		{
			ILType *elemFrom = ILTypeGetElemType(fromType);
			ILType *elemTo = ILTypeGetElemType(toType);
			if(ILTypeIsReference(elemFrom) && ILTypeIsReference(elemTo))
			{
				if(GetConvertRules(info, elemFrom, elemTo, explicit, rules))
				{
					return 1;
				}
			}
		}
	}

	/* If we get here, then we do not know how to convert */
	return 0;
}

/*
 * Apply a set of conversion rules to a node.
 */
static void ApplyRules(ILGenInfo *info, ILNode *node,
					   ILNode **parent, ConvertRules *rules,
					   ILType *toType)
{
	/* Box the input value if necessary */
	if(rules->boxClass)
	{
		*parent = ILNode_Box_create(node, rules->boxClass, 0);
		yysetfilename(*parent, yygetfilename(node));
		yysetlinenum(*parent, yygetlinenum(node));
		node = *parent;
	}

	/* Apply a builtin conversion */
	if(rules->builtin)
	{
		ILApplyConversion(info, node, parent, rules->builtin);
	}

	/* Call a method to perform the conversion */
	if(rules->method)
	{
		ILMethod *method = (ILMethod *)ILMemberImport
								(info->image, (ILMember *)(rules->method));
		if(!method)
		{
			ILGenOutOfMemory(info);
		}
		*parent = ILNode_UserConversion_create
						(node, ILTypeToMachineType(toType), method);
		yysetfilename(*parent, yygetfilename(node));
		yysetlinenum(*parent, yygetlinenum(node));
	}
}

/*
 * Determine if there is an implicit constant coercion
 * between two types for a particular node.  Returns the
 * new type, or ILMachineType_Void if there is no coercion.
 */
static ILMachineType CanCoerceConst(ILGenInfo *info, ILNode *node,
									ILType *fromType, ILType *toType)
{
	ILEvalValue value;
	if(ILType_IsPrimitive(fromType) && ILType_IsPrimitive(toType) &&
	   ILNode_EvalConst(node, info, &value))
	{
		/* We can implicitly down-convert some types of constants,
		   but only if the result remains the same */
		if(fromType == ILType_Int32)
		{
			if(toType == ILType_Int8)
			{
				if(value.un.i4Value >= ((ILInt32)(-128)) &&
				   value.un.i4Value <= ((ILInt32)127))
				{
					return ILMachineType_Int8;
				}
			}
			else if(toType == ILType_UInt8)
			{
				if(value.un.i4Value >= ((ILInt32)0) &&
				   value.un.i4Value <= ((ILInt32)255))
				{
					return ILMachineType_UInt8;
				}
			}
			else if(toType == ILType_Int16)
			{
				if(value.un.i4Value >= ((ILInt32)(-32768)) &&
				   value.un.i4Value <= ((ILInt32)32767))
				{
					return ILMachineType_Int16;
				}
			}
			else if(toType == ILType_UInt16)
			{
				if(value.un.i4Value >= ((ILInt32)0) &&
				   value.un.i4Value <= ((ILInt32)65535))
				{
					return ILMachineType_UInt16;
				}
			}
			else if(toType == ILType_UInt32)
			{
				if(value.un.i4Value >= 0)
				{
					return ILMachineType_UInt32;
				}
			}
			else if(toType == ILType_UInt64)
			{
				if(value.un.i4Value >= 0)
				{
					return ILMachineType_UInt64;
				}
			}
		}
		else if(fromType == ILType_Int64)
		{
			if(toType == ILType_UInt64)
			{
				if(value.un.i8Value >= 0)
				{
					return ILMachineType_UInt64;
				}
			}
		}
	}
	else if(ILType_IsPrimitive(fromType) && ILTypeIsEnum(toType) &&
	   	    ILNode_EvalConst(node, info, &value))
	{
		/* We can coerce the integer value zero to any enumerated type */
		if(fromType == ILType_Int8 || fromType == ILType_UInt8 ||
		   fromType == ILType_Int16 || fromType == ILType_UInt16 ||
		   fromType == ILType_Int32 || fromType == ILType_UInt32)
		{
			if(value.un.i4Value == 0)
			{
				return ILTypeToMachineType(toType);
			}
		}
		else if(fromType == ILType_Int64 || fromType == ILType_UInt64)
		{
			if(value.un.i8Value == 0)
			{
				return ILTypeToMachineType(toType);
			}
		}
	}
	return ILMachineType_Void;
}

int ILCanCoerce(ILGenInfo *info, ILType *fromType, ILType *toType)
{
	ConvertRules rules;
	return GetConvertRules(info, fromType, toType, 0, &rules);
}

int ILCanCoerceNode(ILGenInfo *info, ILNode *node,
				    ILType *fromType, ILType *toType)
{
	ConvertRules rules;
	if(GetConvertRules(info, fromType, toType, 0, &rules))
	{
		return 1;
	}
	else if(CanCoerceConst(info, node, fromType, toType) != ILMachineType_Void)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int ILCoerce(ILGenInfo *info, ILNode *node, ILNode **parent,
			 ILType *fromType, ILType *toType)
{
	ConvertRules rules;
	ILMachineType constType;
	if(GetConvertRules(info, fromType, toType, 0, &rules))
	{
		ApplyRules(info, node, parent, &rules, toType);
		return 1;
	}
	else if((constType = CanCoerceConst(info, node, fromType, toType))
					!= ILMachineType_Void)
	{
		*parent = ILNode_Cast_create(node, constType);
		return 1;
	}
	else
	{
		return 0;
	}
}

int ILCast(ILGenInfo *info, ILNode *node, ILNode **parent,
		   ILType *fromType, ILType *toType)
{
	ConvertRules rules;
	if(GetConvertRules(info, fromType, toType, 1, &rules))
	{
		ApplyRules(info, node, parent, &rules, toType);
		return 1;
	}
	else
	{
		return 0;
	}
}

int ILBetterConversion(ILGenInfo *info, ILType *sType,
					   ILType *t1Type, ILType *t2Type)
{
	if(ILTypeIdentical(t1Type, t2Type))
	{
		return IL_BETTER_NEITHER;
	}
	else if(ILTypeIdentical(sType, t1Type))
	{
		return IL_BETTER_T1;
	}
	else if(ILTypeIdentical(sType, t2Type))
	{
		return IL_BETTER_T2;
	}
	else if(ILCanCoerce(info, t1Type, t2Type) &&
	        !ILCanCoerce(info, t2Type, t1Type))
	{
		return IL_BETTER_T1;
	}
	else if(ILCanCoerce(info, t2Type, t1Type) &&
	        !ILCanCoerce(info, t1Type, t2Type))
	{
		return IL_BETTER_T2;
	}
	else if(t1Type == ILType_Int8 &&
	        (t2Type == ILType_UInt8 ||
			 t2Type == ILType_UInt16 ||
			 t2Type == ILType_UInt32 ||
			 t2Type == ILType_UInt64))
	{
		return IL_BETTER_T1;
	}
	else if(t2Type == ILType_Int8 &&
	        (t1Type == ILType_UInt8 ||
			 t1Type == ILType_UInt16 ||
			 t1Type == ILType_UInt32 ||
			 t1Type == ILType_UInt64))
	{
		return IL_BETTER_T2;
	}
	else if(t1Type == ILType_Int16 &&
			(t2Type == ILType_UInt16 ||
			 t2Type == ILType_UInt32 ||
			 t2Type == ILType_UInt64))
	{
		return IL_BETTER_T1;
	}
	else if(t2Type == ILType_Int16 &&
			(t1Type == ILType_UInt16 ||
			 t1Type == ILType_UInt32 ||
			 t1Type == ILType_UInt64))
	{
		return IL_BETTER_T2;
	}
	else if(t1Type == ILType_Int32 &&
			(t2Type == ILType_UInt32 ||
			 t2Type == ILType_UInt16))
	{
		return IL_BETTER_T1;
	}
	else if(t2Type == ILType_Int32 &&
			(t1Type == ILType_UInt32 ||
			 t1Type == ILType_UInt16))
	{
		return IL_BETTER_T2;
	}
	else if(t1Type == ILType_Int64 && t2Type == ILType_UInt64)
	{
		return IL_BETTER_T1;
	}
	else if(t2Type == ILType_Int64 && t1Type == ILType_UInt64)
	{
		return IL_BETTER_T2;
	}
	else
	{
		return IL_BETTER_NEITHER;
	}
}

#ifdef	__cplusplus
};
#endif
