/*
 * coder.h - Definitions for the code generation helpers.
 *
 * Copyright (C) 2011  Southern Storm Software, Pty Ltd.
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

#ifndef	_ENGINE_CODER_H
#define	_ENGINE_CODER_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Get the type of a parameter to the current method.
 * Returns 0 if the parameter number is invalid.
 */
ILType *_ILCoderGetParamType(ILType *signature, ILMethod *method,
							 ILUInt32 num);

/*
 * Load the arguments from fromArg to toArg on the verification and
 * coder stack.
 * fromArg ... toArg must be valid argument numbers for the given
 * method and signature.
 */
void _ILCoderLoadArgs(ILCoder *coder, ILEngineStackItem *stack,
					  ILMethod *method, ILType *signature,
					  ILUInt32 fromArg, ILUInt32 toArg);

/*
 * Set return type information within a stack item.
 */
void _ILCoderSetReturnType(ILEngineStackItem *item, ILType *returnType);

/*
 * Process a "box" operation on a value.  Returns zero if
 * invalid parameters.
 */
int _ILCoderBoxValue(ILExecProcess *process, ILEngineType valueType,
					 ILType *typeInfo, ILClass *boxClass);

/*
 * Process a "box" operation on a value.  Returns zero if
 * invalid parameters.
 */
int _ILCoderBoxPtr(ILExecProcess *process, ILType *typeInfo, 
				   ILClass *boxClass, ILUInt32 pos);

#ifdef	__cplusplus
};
#endif

#endif	/* _ENGINE_CODER_H */
