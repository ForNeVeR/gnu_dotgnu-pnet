/*
 * coder.c - Common helper methods for code generation.
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

#include "engine_private.h"
#include "coder.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Get the type of a parameter to the current method.
 * Returns 0 if the parameter number is invalid.
 */
ILType *_ILCoderGetParamType(ILType *signature, ILMethod *method,
							 ILUInt32 num)
{
	if(ILType_HasThis(signature))
	{
		/* This method has a "this" parameter */
		if(!num)
		{
			ILClass *owner;
			ILType *synthetic;

			owner = ILMethod_Owner(method);
			if(ILClassIsValueType(owner))
			{
				/* The "this" parameter is a value type, which is
				   being passed as a managed pointer.  Return
				   ILType_Invalid to tell the caller that special
				   handling is required */
				return ILType_Invalid;
			}
			synthetic = ILClassGetSynType(owner);
			if(synthetic)
			{
				return synthetic;
			}
			else
			{
				return ILType_FromClass(owner);
			}
		}
		else
		{
			return ILTypeGetParam(signature, num);
		}
	}
	else
	{
		return ILTypeGetParam(signature, num + 1);
	}
}

/*
 * Load the arguments from fromArg to toArg on the verification and
 * coder stack.
 * fromArg ... toArg must be valid argument numbers for the given
 * method and signature.
 */
void _ILCoderLoadArgs(ILCoder *coder, ILEngineStackItem *stack,
					  ILMethod *method, ILType *signature,
					  ILUInt32 fromArg, ILUInt32 toArg)
{
	ILUInt32 current;
	ILUInt32 stackTop;

	stackTop = 0;
	current = fromArg;
	while(current <= toArg)
	{
		ILType *paramType;

		paramType = _ILCoderGetParamType(signature, method, current);
		stack[stackTop].typeInfo = paramType;
		stack[stackTop].engineType = _ILTypeToEngineType(paramType);
		ILCoderLoadArg(coder, current, paramType);
		++stackTop;
		++current;
	}
}

/*
 * Set return type information within a stack item.
 */
void _ILCoderSetReturnType(ILEngineStackItem *item, ILType *returnType)
{
	if(returnType != ILType_Void)
	{
		item->engineType = _ILTypeToEngineType(returnType);
		if(item->engineType != ILEngineType_M)
		{
			item->typeInfo = returnType;
		}
		else
		{
			item->typeInfo = ILType_Ref(ILTypeStripPrefixes(returnType));
		}
	}
	else
	{
		item->engineType = ILEngineType_Invalid;
	}
}

/*
 * Process a "box" operation on a value.  Returns zero if
 * invalid parameters.
 */
int _ILCoderBoxValue(ILExecProcess *process, ILEngineType valueType,
					 ILType *typeInfo, ILClass *boxClass)
{
	ILUInt32 size;
	ILType *rawType;

	/* Determine the raw version of the boxing type */
	rawType = ILTypeGetEnumType(ILClassToType(boxClass));

	/* Get the size of the value type */
	size = _ILSizeOfTypeLocked(process, rawType);

	/* Determine how to box the value */
	if(ILType_IsPrimitive(rawType))
	{
		if(valueType == ILEngineType_I4)
		{
			/* Determine if we are boxing a byte, short, or int
			   based on the raw type */
			switch(ILType_ToElement(rawType))
			{
				case IL_META_ELEMTYPE_BOOLEAN:
				case IL_META_ELEMTYPE_I1:
				case IL_META_ELEMTYPE_U1:
				{
					ILCoderBoxSmaller(process->coder, boxClass, valueType, ILType_Int8);
					return 1;
				}
				/* Not reached */
	
				case IL_META_ELEMTYPE_I2:
				case IL_META_ELEMTYPE_U2:
				case IL_META_ELEMTYPE_CHAR:
				{
					ILCoderBoxSmaller(process->coder, boxClass, valueType, ILType_Int16);
					return 1;
				}
				/* Not reached */
	
				case IL_META_ELEMTYPE_I4:
				case IL_META_ELEMTYPE_U4:
			#ifdef IL_NATIVE_INT32
				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
			#endif
				{
					ILCoderBox(process->coder, boxClass, valueType, size);
					return 1;
				}
				/* Not reached */
			}
		}
		else if(valueType == ILEngineType_I)
		{
			/* Box a native integer */
			switch(ILType_ToElement(rawType))
			{
				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
				{
					ILCoderBox(process->coder, boxClass, valueType, size);
					return 1;
				}
				/* Not reached */
			}
		}
		else if(valueType == ILEngineType_I8)
		{
			/* Box a 64-bit integer */
			switch(ILType_ToElement(rawType))
			{
				case IL_META_ELEMTYPE_I8:
				case IL_META_ELEMTYPE_U8:
				{
					ILCoderBox(process->coder, boxClass, valueType, size);
					return 1;
				}
				/* Not reached */
			}
		}
		else if(valueType == ILEngineType_F)
		{
			/* Determine if we are boxing a float or double
			   based on the size of the value type */
			if(rawType == ILType_Float32)
			{
				ILCoderBoxSmaller(process->coder, boxClass, valueType, ILType_Float32);
				return 1;
			}
			else if(rawType == ILType_Float64 ||
				    rawType == ILType_Float)
			{
				ILCoderBoxSmaller(process->coder, boxClass, valueType, ILType_Float64);
				return 1;
			}
		}
	}
	else if(valueType == ILEngineType_MV ||
			valueType == ILEngineType_TypedRef)
	{
		if(ILTypeIdentical(typeInfo, ILClassToType(boxClass)))
		{
			ILCoderBox(process->coder, boxClass, valueType, size);
			return 1;
		}
	}
	return 0;
}

/*
 * Process a "box" operation on a value.  Returns zero if
 * invalid parameters.
 */
int _ILCoderBoxPtr(ILExecProcess *process, ILType *typeInfo, 
				   ILClass *boxClass, ILUInt32 pos)
{
	ILUInt32 size;
	ILType *rawType;

	/* Determine the raw version of the boxing type */
	rawType = ILTypeGetEnumType(ILClassToType(boxClass));

	/* Get the size of the value type */
	size = _ILSizeOfTypeLocked(process, rawType);

	if(ILTypeIdentical(typeInfo, ILClassToType(boxClass)))
	{
		ILCoderBoxPtr(process->coder, boxClass, size, pos);
		return 1;
	}

	return 0;
}

#ifdef	__cplusplus
};
#endif
