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
static int FindConversion(ILClass *info, ILType *fromType, ILType *toType,
						  int explicit, ConvertRules *rules)
{
	ILMethod *method;

	/* Look for an implicit operator definition */
	method = ILResolveConversionOperator(info, "op_Implicit",
										 fromType, toType);
	if(method)
	{
		rules->method = method;
		return 1;
	}

	/* Look for an explicit operator definition */
	if(explicit)
	{
		method = ILResolveConversionOperator(info, "op_Explicit",
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
static int GetConvertRules(ILContext *context, ILType *fromType,
						   ILType *toType, int explicit,
						   ConvertRules *rules)
{
	const ILConversion *conv;

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

	/* If "fromType" is null, then "toType" must be a class type */
	if(fromType == ILType_Null)
	{
		return ILType_IsClass(toType);
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
		if(FindConversion(ILType_ToClass(fromType),
					  	  fromType, toType, explicit, rules))
		{
			return 1;
		}
	}
	else if(ILType_IsValueType(fromType))
	{
		if(FindConversion(ILType_ToValueType(fromType),
					  	  fromType, toType, explicit, rules))
		{
			return 1;
		}
	}
	if(ILType_IsClass(toType))
	{
		if(FindConversion(ILType_ToClass(toType),
					  	  fromType, toType, explicit, rules))
		{
			return 1;
		}
	}
	else if(ILType_IsValueType(toType))
	{
		if(FindConversion(ILType_ToValueType(toType),
					  	  fromType, toType, explicit, rules))
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
		*parent = ILNode_UserConversion_create
						(node, ILTypeToValueType(toType), rules->method);
		yysetfilename(*parent, yygetfilename(node));
		yysetlinenum(*parent, yygetlinenum(node));
	}
}

int ILCanCoerce(ILContext *context, ILType *fromType, ILType *toType)
{
	ConvertRules rules;
	return GetConvertRules(context, fromType, toType, 0, &rules);
}

int ILCoerce(ILGenInfo *info, ILNode *node, ILNode **parent,
			 ILType *fromType, ILType *toType)
{
	ConvertRules rules;
	if(GetConvertRules(info->context, fromType, toType, 0, &rules))
	{
		ApplyRules(info, node, parent, &rules, toType);
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
	if(GetConvertRules(info->context, fromType, toType, 1, &rules))
	{
		ApplyRules(info, node, parent, &rules, toType);
		return 1;
	}
	else
	{
		return 0;
	}
}

#ifdef	__cplusplus
};
#endif
