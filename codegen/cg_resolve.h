/*
 * cg_resolve.h - Resolve methods, fields, operators, etc.
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

#ifndef	__CODEGEN_CG_RESOLVE_H__
#define	__CODEGEN_CG_RESOLVE_H__

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Special return values for resolution functions.
 */
#define	IL_METHOD_NOT_FOUND			((ILMethod *)0)
#define	IL_METHOD_ACCESS_DENIED		((ILMethod *)1)

/*
 * Resolve a call to a static method.
 */
ILMethod *ILResolveStaticMethod(ILClass *info, ILClass *callScope,
							    const char *name, ILType **args, int numArgs);

/*
 * Resolve a call to an instance method.
 */
ILMethod *ILResolveInstanceMethod(ILClass *info, ILClass *callScope,
							      const char *name, ILType **args, int numArgs);

/*
 * Resolve a call to a constructor.
 */
ILMethod *ILResolveConstructor(ILClass *info, ILClass *callScope,
							   ILType **args, int numArgs);

/*
 * Resolve a call to a unary operator.
 */
ILMethod *ILResolveUnaryOperator(ILClass *info, const char *name,
						 	     ILType *argType);

/*
 * Resolve a call to a binary operator.
 */
ILMethod *ILResolveBinaryOperator(ILClass *info, const char *name,
						 	      ILType *arg1Type, ILType *arg2Type);

/*
 * Resolve a call to a conversion operator.
 */
ILMethod *ILResolveConversionOperator(ILClass *info, const char *name,
						 	          ILType *fromType, ILType *toType);

#ifdef	__cplusplus
};
#endif

#endif	/* __CODEGEN_CG_RESOLVE_H__ */
