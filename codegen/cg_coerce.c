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
	/* Class to use to box or unbox the value */
	ILClass		*boxClass;
	ILClass		*unboxClass;
	int			 boxIsEnum;

	/* Method to call to perform the conversion */
	ILMethod	*method;

	/* Explicit type to cast to using a checked cast instruction */
	ILType	    *castType;

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
 * Forward declaration.
 */
static int GetConvertRules(ILGenInfo *info, ILType *fromType,
						   ILType *toType, int explicit,
						   int kinds, ConvertRules *rules);

/*
 * Get the rules to be used to convert from one reference type to another.
 */
static int GetReferenceConvertRules(ILGenInfo *info, ILType *fromType,
						   		    ILType *toType, int explicit,
						   		    ConvertRules *rules)
{
	ILClass *classFrom;
	ILClass *classTo;

	/* Both types must be reference types */
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
		rules->castType = toType;
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
			rules->castType = toType;
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
		else if(explicit && !ILClass_IsSealed(classFrom))
		{
			/* Explicit conversion to an interface */
			rules->castType = toType;
			return 1;
		}
	}

	/* Check for explicit interface conversions */
	if(explicit && ILClass_IsInterface(classFrom))
	{
		if(!ILClass_IsInterface(classTo))
		{
			if(!ILClass_IsSealed(classTo) ||
		       ILClassImplements(classTo, classFrom))
			{
				/* Explicit conversion from an interface to a
				   class that may implement it */
				rules->castType = toType;
				return 1;
			}
		}
		else if(!ILClassImplements(classFrom, classTo))
		{
			/* Explicit conversion between unrelated interfaces */
			rules->castType = toType;
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
				if(GetConvertRules(info, elemFrom, elemTo,
								   explicit, IL_CONVERT_REFERENCE, rules))
				{
					if(rules->castType)
					{
						/* Move the explicit cast up to the array level */
						rules->castType = toType;
					}
					return 1;
				}
			}
		}
	}

	/* This is not a valid reference conversion */
	return 0;
}

/*
 * Get the rules to be used to box or unbox a value type.
 */
static int GetBoxingConvertRules(ILGenInfo *info, ILType *fromType,
						   		 ILType *toType, int explicit,
						   		 ConvertRules *rules)
{
	ILClass *classFrom;
	ILClass *classTo;

	if(ILTypeIsValue(fromType))
	{
		/* Convert the source type into a class */
		classFrom = ILTypeToClass(info, fromType);
		if(!classFrom)
		{
			return 0;
		}

		/* Value types can always be boxed as "Object" */
		if(ILTypeIsObjectClass(toType))
		{
			rules->boxClass = classFrom;
			rules->boxIsEnum = ILTypeIsEnum(fromType);
			return 1;
		}

		/* If "toType" is not a reference type, then boxing is impossible */
		if(!ILTypeIsReference(toType))
		{
			return 0;
		}

		/* Convert the destination type into a class */
		classTo = ILTypeToClass(info, toType);
		if(!classTo)
		{
			return 0;
		}

		/* We can box the value if its class inherits from "toType",
		   or "toType" is an interface that the value type implements */
		if(ILClass_IsInterface(classTo))
		{
			if(!ILClassImplements(classFrom, classTo))
			{
				return 0;
			}
		}
		else
		{
			if(!ILClassInheritsFrom(classFrom, classTo))
			{
				return 0;
			}
		}

		/* Box the value */
		rules->boxClass = classFrom;
		rules->boxIsEnum = ILTypeIsEnum(fromType);
		return 1;
	}
	else if(explicit && ILTypeIsReference(fromType) && ILTypeIsValue(toType))
	{
		/* Convert the two types into classes */
		classFrom = ILTypeToClass(info, fromType);
		if(!classFrom)
		{
			return 0;
		}
		classTo = ILTypeToClass(info, toType);
		if(!classTo)
		{
			return 0;
		}

		/* If the source type is "Object", then unboxing is always possible */
		if(ILTypeIsObjectClass(fromType))
		{
			rules->unboxClass = classTo;
			rules->boxIsEnum = ILTypeIsEnum(toType);
			return 1;
		}

		/* We can unbox the object if the value type inherits from the
		   object's type, or if the object is an interface that the
		   value type implements */
		if(ILClass_IsInterface(classFrom))
		{
			if(!ILClassImplements(classTo, classFrom))
			{
				return 0;
			}
		}
		else
		{
			if(!ILClassInheritsFrom(classTo, classFrom))
			{
				return 0;
			}
		}

		/* Unbox the value */
		rules->unboxClass = classTo;
		rules->boxIsEnum = ILTypeIsEnum(toType);
		return 1;
	}
	else
	{
		/* No relevant boxing or unboxing conversion */
		return 0;
	}
}

/*
 * Get the rules to be used inside of an unsafe block.
 */
static int GetUnsafeConvertRules(ILGenInfo *info, ILType *fromType,
		ILType *toType, int explicit, int kinds, ConvertRules *rules)
{
	/* TODO : Handle the rest of the cases */
	/*  Numeric/pointer casts  */
	if (explicit)
	{
		/*  Numeric -> pointer conversion  */
		if ( 
				(ILIsBuiltinNumeric(fromType) 
				 && ILType_Kind(toType) == IL_TYPE_COMPLEX_PTR) 
				|| (ILType_Kind(fromType) == IL_TYPE_COMPLEX_PTR 
					&& ILIsBuiltinNumeric(toType))
				|| (ILType_Kind(fromType) == IL_TYPE_COMPLEX_PTR 
					&& ILType_Kind(toType) == IL_TYPE_COMPLEX_PTR)
				)
		{
			/*  Let the external operator handle the real work  */
			return 1;
		}
	}

	return 0;
}

/*
 * Get the rules to be used to convert from one type to another.
 */
static int GetConvertRules(ILGenInfo *info, ILType *fromType,
						   ILType *toType, int explicit,
						   int kinds, ConvertRules *rules)
{
	const ILConversion *conv;
	ILClass *fromClass;
	ILClass *toClass;

	/* Clear the rules */
	rules->boxClass = 0;
	rules->unboxClass = 0;
	rules->boxIsEnum = 0;
	rules->method = 0;
	rules->castType = 0;
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
		if((kinds & IL_CONVERT_REFERENCE) != 0)
		{
			return ILTypeIsReference(toType);
		}
		else
		{
			return 0;
		}
	}

	/* Look for a builtin numeric conversion */
	if((kinds & IL_CONVERT_NUMERIC) != 0)
	{
		conv = ILFindConversion(fromType, toType, explicit);
		if(conv)
		{
			rules->builtin = conv;
			return 1;
		}
	}

	/* Look for a user-defined conversion */
	if((kinds & IL_CONVERT_USER_DEFINED) != 0)
	{
		fromClass = ILTypeToClass(info, fromType);
		toClass = ILTypeToClass(info, toType);
		if(fromClass != 0)
		{
			if(FindConversion(info, fromClass, fromType, toType,
							  explicit, rules))
			{
				return 1;
			}
		}
		if(toClass != 0)
		{
			if(FindConversion(info, toClass, fromType, toType,
							  explicit, rules))
			{
				return 1;
			}
		}
	}

	/* Check for reference conversions */
	if((kinds & IL_CONVERT_REFERENCE) != 0)
	{
		if(GetReferenceConvertRules(info, fromType, toType,
									explicit, rules))
		{
			return 1;
		}
	}

	/* Check for boxing and unboxing conversions */
	if((kinds & IL_CONVERT_BOXING) != 0)
	{
		if(GetBoxingConvertRules(info, fromType, toType,
								 explicit, rules))
		{
			return 1;
		}
	}

	/* Check for explicit enumeration conversions */
	if((kinds & IL_CONVERT_ENUM) != 0 && explicit)
	{
		if(ILTypeIsEnum(fromType))
		{
			if(ILTypeIsEnum(toType))
			{
				/* Both are enumerated types */
				if(GetConvertRules(info, ILTypeGetEnumType(fromType),
								   ILTypeGetEnumType(toType), explicit,
								   IL_CONVERT_NUMERIC, rules))
				{
					return 1;
				}
			}
			else if(ILIsBuiltinNumeric(toType))
			{
				/* Converting from an enumerated type to a numeric type */
				if(GetConvertRules(info, ILTypeGetEnumType(fromType), toType,
								   explicit, IL_CONVERT_NUMERIC, rules))
				{
					return 1;
				}
			}
		}
		else if(ILTypeIsEnum(toType))
		{
			if(ILIsBuiltinNumeric(fromType))
			{
				/* Converting from a numeric type to an enumerated type */
				if(GetConvertRules(info, fromType, ILTypeGetEnumType(toType),
								   explicit, IL_CONVERT_NUMERIC, rules))
				{
					return 1;
				}
			}
		}
	}

	if (info->unsafeLevel > 0)
	{
		if(GetUnsafeConvertRules(info, fromType, toType, explicit, kinds, rules))
		{
			return 1;
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
	/* Box or unbox the input value if necessary */
	if(rules->boxClass)
	{
		*parent = ILNode_Box_create(node, rules->boxClass, rules->boxIsEnum);
		yysetfilename(*parent, yygetfilename(node));
		yysetlinenum(*parent, yygetlinenum(node));
		node = *parent;
	}
	else if(rules->unboxClass)
	{
		*parent = ILNode_Unbox_create(node, rules->unboxClass,
									  rules->boxIsEnum,
									  ILTypeToMachineType(toType));
		yysetfilename(*parent, yygetfilename(node));
		yysetlinenum(*parent, yygetlinenum(node));
		node = *parent;
	}

	/* Apply a builtin conversion */
	if(rules->builtin)
	{
		ILApplyConversion(info, node, parent, rules->builtin);
		node = *parent;
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
		node = *parent;
	}

	/* Cast to an explicit type */
	if(rules->castType)
	{
		*parent = ILNode_CastType_create(node, rules->castType);
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
	return GetConvertRules(info, fromType, toType, 0, IL_CONVERT_ALL, &rules);
}

int ILCanCoerceKind(ILGenInfo *info, ILType *fromType,
					ILType *toType, int kinds)
{
	ConvertRules rules;
	return GetConvertRules(info, fromType, toType, 0, kinds, &rules);
}

int ILCanCoerceNode(ILGenInfo *info, ILNode *node,
				    ILType *fromType, ILType *toType)
{
	ConvertRules rules;
	if(GetConvertRules(info, fromType, toType, 0, IL_CONVERT_ALL, &rules))
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

int ILCanCoerceNodeKind(ILGenInfo *info, ILNode *node,
				        ILType *fromType, ILType *toType, int kinds)
{
	ConvertRules rules;
	if(GetConvertRules(info, fromType, toType, 0, kinds, &rules))
	{
		return 1;
	}
	else if((kinds & IL_CONVERT_CONSTANT) != 0 &&
	        CanCoerceConst(info, node, fromType, toType) != ILMachineType_Void)
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
	if(GetConvertRules(info, fromType, toType, 0, IL_CONVERT_ALL, &rules))
	{
		ApplyRules(info, node, parent, &rules, toType);
		return 1;
	}
	else if((constType = CanCoerceConst(info, node, fromType, toType))
					!= ILMachineType_Void)
	{
		*parent = ILNode_CastSimple_create(node, constType);
		return 1;
	}
	else
	{
		return 0;
	}
}

int ILCoerceKind(ILGenInfo *info, ILNode *node, ILNode **parent,
			     ILType *fromType, ILType *toType, int kinds)
{
	ConvertRules rules;
	ILMachineType constType;
	if(GetConvertRules(info, fromType, toType, 0, kinds, &rules))
	{
		ApplyRules(info, node, parent, &rules, toType);
		return 1;
	}
	else if((kinds & IL_CONVERT_CONSTANT) != 0 &&
			(constType = CanCoerceConst(info, node, fromType, toType))
					!= ILMachineType_Void)
	{
		*parent = ILNode_CastSimple_create(node, constType);
		return 1;
	}
	else
	{
		return 0;
	}
}

int ILCanCast(ILGenInfo *info, ILType *fromType, ILType *toType)
{
	ConvertRules rules;
	return GetConvertRules(info, fromType, toType, 1, IL_CONVERT_ALL, &rules);
}

int ILCanCastKind(ILGenInfo *info, ILType *fromType,
				  ILType *toType, int kinds)
{
	ConvertRules rules;
	return GetConvertRules(info, fromType, toType, 1, kinds, &rules);
}

int ILCast(ILGenInfo *info, ILNode *node, ILNode **parent,
		   ILType *fromType, ILType *toType)
{
	ConvertRules rules;
	if(GetConvertRules(info, fromType, toType, 1, IL_CONVERT_ALL, &rules))
	{
		ApplyRules(info, node, parent, &rules, toType);
		return 1;
	}
	else
	{
		return 0;
	}
}

int ILCastKind(ILGenInfo *info, ILNode *node, ILNode **parent,
		       ILType *fromType, ILType *toType, int kinds)
{
	ConvertRules rules;
	if(GetConvertRules(info, fromType, toType, 1, kinds, &rules))
	{
		ApplyRules(info, node, parent, &rules, toType);
		return 1;
	}
	else
	{
		return 0;
	}
}

ILClass *ILGetExplicitConv(ILGenInfo *info, ILType *fromType,
						   ILType *toType, int kinds)
{
	ConvertRules rules;
	if(GetConvertRules(info, fromType, toType, 1, kinds, &rules))
	{
		if(rules.castType)
		{
			return ILTypeToClass(info, rules.castType);
		}
		else if(rules.unboxClass)
		{
			return rules.unboxClass;
		}
		else
		{
			return 0;
		}
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
