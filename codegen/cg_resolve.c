/*
 * cg_resolve.c - Resolve methods, fields, operators, etc.
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
 * Attributes that are indicate the type of method.
 */
#define	METHOD_TYPE_ATTRS	(IL_META_METHODDEF_STATIC | \
							 IL_META_METHODDEF_SPECIAL_NAME | \
							 IL_META_METHODDEF_RT_SPECIAL_NAME)

/*
 * Internal worker function for locating methods.
 */
static ILMethod *ResolveMethod(ILGenInfo *info, ILClass *classInfo,
							   ILClass *callScope, const char *name,
							   ILType **args, int numArgs,
						       ILType *returnType, ILUInt32 attrs,
							   int normalOrVirtual, int dontInherit)
{
	ILMember *member;
	ILMethod *method;
	ILMethod *closestMatch = 0;
	ILType *signature;
	ILType *argType;
	int arg, same;

	while(classInfo != 0)
	{
		classInfo = ILClassResolve(classInfo);
		member = 0;
		while((member = ILClassNextMemberByKind
					(classInfo, member, IL_META_MEMBERKIND_METHOD)) != 0)
		{
			/* Filter out members that aren't interesting */
			if(strcmp(ILMember_Name(member), name) != 0 ||
			   (ILMember_Attrs(member) & METHOD_TYPE_ATTRS) != attrs)
			{
				continue;
			}
			method = (ILMethod *)member;
			if(!normalOrVirtual)
			{
				if(ILMethod_IsVirtual(method))
				{
					continue;
				}
			}

			/* Check that this is the signature we are interested in */
			signature = ILMethod_Signature(method);
			if(!ILType_IsMethod(signature))
			{
				continue;
			}
			if(returnType)
			{
				if(!ILTypeIdentical(returnType, ILTypeGetReturn(signature)))
				{
					continue;
				}
			}
			if(numArgs != (int)(ILTypeNumParams(signature)))
			{
				continue;
			}
			same = 1;
			for(arg = 1; arg <= numArgs; ++arg)
			{
				argType = ILTypeGetParam(signature, arg);
				if(!ILTypeIdentical(argType, args[arg - 1]))
				{
					if(!ILCanCoerce(info, argType, args[arg - 1]))
					{
						break;
					}
					same = 0;
				}
			}
			if(arg < numArgs)
			{
				continue;
			}

			/* Check the method's access level against the call scope */
			if(!ILMemberAccessible(member, callScope))
			{
				return 0;
			}

			/* We've found a candidate method */
			if(same)
			{
				/* We have an exact match, so return that */
				return method;
			}
			else if(closestMatch)
			{
				/* This match is close, but try to find a better one */
				closestMatch = method;
			}
		}

		/* Move up to the parent class */
		classInfo = (dontInherit ? 0 : ILClass_Parent(classInfo));
	}

	/* Return the closest match if we didn't find an exact match */
	return closestMatch;
}

ILMethod *ILResolveStaticMethod(ILGenInfo *info, ILClass *classInfo,
								ILClass *callScope, const char *name,
								ILType **args, int numArgs)
{
	return ResolveMethod(info, classInfo, callScope, name, args, numArgs, 0,
					     IL_META_METHODDEF_STATIC, 0, 0);
}

ILMethod *ILResolveInstanceMethod(ILGenInfo *info, ILClass *classInfo,
								  ILClass *callScope, const char *name,
								  ILType **args, int numArgs)
{
	return ResolveMethod(info, classInfo, callScope, name,
						 args, numArgs, 0, 0, 1, 0);
}

ILMethod *ILResolveConstructor(ILGenInfo *info, ILClass *classInfo,
							   ILClass *callScope, ILType **args, int numArgs)
{
	return ResolveMethod(info, classInfo, callScope, ".ctor",
						 args, numArgs, ILType_Void,
					     IL_META_METHODDEF_SPECIAL_NAME |
					     IL_META_METHODDEF_RT_SPECIAL_NAME, 0, 1);
}

ILMethod *ILResolveUnaryOperator(ILGenInfo *info, ILClass *classInfo,
							     const char *name, ILType *argType)
{
	ILType *args[1];
	args[0] = argType;
	return ResolveMethod(info, classInfo, 0, name, args, 1, 0,
					     IL_META_METHODDEF_STATIC |
					     IL_META_METHODDEF_SPECIAL_NAME, 0, 0);
}

ILMethod *ILResolveBinaryOperator(ILGenInfo *info, ILClass *classInfo,
								  const char *name, ILType *arg1Type,
								  ILType *arg2Type)
{
	ILType *args[2];
	args[0] = arg1Type;
	args[1] = arg2Type;
	return ResolveMethod(info, classInfo, 0, name, args, 2, 0,
					     IL_META_METHODDEF_STATIC |
					     IL_META_METHODDEF_SPECIAL_NAME, 0, 0);
}

ILMethod *ILResolveConversionOperator(ILGenInfo *info, ILClass *classInfo,
									  const char *name, ILType *fromType,
									  ILType *toType)
{
	ILType *args[1];
	args[0] = fromType;
	return ResolveMethod(info, classInfo, 0, name, args, 1, toType,
					     IL_META_METHODDEF_STATIC |
					     IL_META_METHODDEF_SPECIAL_NAME, 0, 0);
}

#ifdef	__cplusplus
};
#endif
