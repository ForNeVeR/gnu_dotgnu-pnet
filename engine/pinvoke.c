/*
 * pinvoke.c - Handle PInvoke and "internalcall" methods within the engine.
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

#include "engine.h"
#include "../libffi/include/ffi.h"

/*
 * The hard work is done using "libffi", which is a generic library
 * for packing up the arguments for an arbitrary function call,
 * making the call, and then getting the return value.
 *
 * The main different between "PInvoke" and "internalcall" is the
 * calling conventions.  All "internalcall" methods are passed an
 * extra first parameter which points to the "ILExecThread" that
 * the method was invoked within.  This is necessary because many
 * "internalcall" functions deal with runtime data structures.
 *
 * PInvoke methods are assumed to be external to the runtime engine
 * and so don't need "ILExecThread" passed to them.
 *
 * Further information on how this works can be found in "doc/pinvoke.html".
 */

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Conversion types.
 */
typedef enum
{
	Conv_Void,
	Conv_Boolean_Int8,
	Conv_Boolean_NativeInt,
	Conv_Char_WChar,
	Conv_Char_UInt16,
	Conv_Int8,
	Conv_UInt8,
	Conv_Int16_NativeShort,
	Conv_Int16,
	Conv_UInt16_NativeUShort,
	Conv_UInt16,
	Conv_Int32_NativeInt,
	Conv_Int32,
	Conv_UInt32_NativeUInt,
	Conv_UInt32,
	Conv_Int64_NativeLong,
	Conv_Int64,
	Conv_UInt64_NativeULong,
	Conv_UInt64,
	Conv_NativeInt,
	Conv_NativeUInt,
	Conv_Float,
	Conv_Double,
	Conv_LongDouble,
	Conv_ObjectRef,
	Conv_Pointer,
	Conv_String,

} PInvokeConversion;

/*
 * Get the "ffi_type" value for an IL type.
 */
static ffi_type *TypeToFfiType(ILImage *image, ILType *type,
							   ILFieldMarshal *marshal,
							   PInvokeConversion *conversion,
							   ILMethodCallInfo *callInfo)
{
	/* If we have a type marshaller, then use that information */
	if(marshal)
	{
		const unsigned char *blob;
		unsigned long blobLen;
		blob = (const unsigned char *)ILFieldMarshalGetType(marshal, &blobLen);
		if(blob && blobLen > 0)
		{
		}
	}

	/* Convert a regular type descriptor */
	if(ILType_IsPrimitive(type))
	{
		/* Primitive element type */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:
			{
				*conversion = Conv_Void;
				return &ffi_type_void;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_BOOLEAN:
			{
				if(callInfo->nativeTypeSizes)
				{
					*conversion = Conv_Boolean_NativeInt;
					return &ffi_type_sint;
				}
				else
				{
					*conversion = Conv_Boolean_Int8;
					return &ffi_type_int8;
				}
			}
			/* Not reached */

			case IL_META_ELEMTYPE_CHAR:
			{
				if(callInfo->nativeTypeSizes)
				{
					*conversion = Conv_Char_WChar;
					if(SIZEOF_WCHAR_T == sizeof(unsigned int))
					{
						return &ffi_type_uint;
					}
					else if(SIZEOF_WCHAR_T == sizeof(unsigned long))
					{
						return &ffi_type_ulong;
					}
					else if(SIZEOF_WCHAR_T == sizeof(unsigned short))
					{
						return &ffi_type_ushort;
					}
					else
					{
						return &ffi_type_uint16;
					}
				}
				else
				{
					*conversion = Conv_Char_UInt16;
					return &ffi_type_uint16;
				}
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I1:
			{
				*conversion = Conv_Int8;
				return &ffi_type_sint8;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_U1:
			{
				*conversion = Conv_UInt8;
				return &ffi_type_uint8;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I2:
			{
				if(callInfo->nativeTypeSizes)
				{
					*conversion = Conv_Int16_NativeShort;
					return &ffi_type_sshort;
				}
				else
				{
					*conversion = Conv_Int16;
					return &ffi_type_sint16;
				}
			}
			/* Not reached */

			case IL_META_ELEMTYPE_U2:
			{
				if(callInfo->nativeTypeSizes)
				{
					*conversion = Conv_UInt16_NativeUShort;
					return &ffi_type_ushort;
				}
				else
				{
					*conversion = Conv_UInt16;
					return &ffi_type_uint16;
				}
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I4:
			{
				if(callInfo->nativeTypeSizes)
				{
					*conversion = Conv_Int32_NativeInt;
					return &ffi_type_sint;
				}
				else
				{
					*conversion = Conv_Int32;
					return &ffi_type_sint32;
				}
			}
			/* Not reached */

			case IL_META_ELEMTYPE_U4:
			{
				if(callInfo->nativeTypeSizes)
				{
					*conversion = Conv_UInt32_NativeUInt;
					return &ffi_type_uint;
				}
				else
				{
					*conversion = Conv_UInt32;
					return &ffi_type_uint32;
				}
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I8:
			{
				if(callInfo->nativeTypeSizes)
				{
					*conversion = Conv_Int64_NativeLong;
					return &ffi_type_slong;
				}
				else
				{
					*conversion = Conv_Int64;
					return &ffi_type_sint64;
				}
			}
			/* Not reached */

			case IL_META_ELEMTYPE_U8:
			{
				if(callInfo->nativeTypeSizes)
				{
					*conversion = Conv_UInt64_NativeULong;
					return &ffi_type_ulong;
				}
				else
				{
					*conversion = Conv_UInt64_UInt64;
					return &ffi_type_uint64;
				}
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I:
			{
				*conversion = Conv_NativeInt;
				return &ffi_type_sint;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_U:
			{
				*conversion = Conv_NativeUInt;
				return &ffi_type_uint;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_R4:
			{
				*conversion = Conv_Float;
				return &ffi_type_float;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_R8:
			{
				*conversion = Conv_Double;
				return &ffi_type_double;
			}
			/* Not reached */

		#ifdef IL_NATIVE_FLOAT
			case IL_META_ELEMTYPE_R:
			{
				*conversion = Conv_LongDouble;
				return &ffi_type_longdouble;
			}
			/* Not reached */
		#else
			case IL_META_ELEMTYPE_R:
			{
				*conversion = Conv_Double;
				return &ffi_type_double;
			}
			/* Not reached */
		#endif

			default:						break;
		}
	}
	else if(ILType_IsClass(type))
	{
		/* Marshal a string or object reference */
		if(ILClassInheritsFromNamed(ILType_ToClass(type),
									"String", "System"))
		{
			if(callInfo->isInternalCall)
			{
				/* Internal methods pass strings around as object references */
				*conversion = Conv_ObjectRef;
			}
			else
			{
				/* PInvoke methods pass strings around as "char" arrays */
				*conversion = Conv_String;
			}
		}
		else
		{
			*conversion = Conv_ObjectRef;
		}
		return &ffi_type_pointer;
	}
	else if(ILType_IsValueType(type))
	{
		/* Marshal a struct by value */
	}
	else if(type != 0)
	{
		/* Marshal a complex type: normally a pointer */
		*conversion = Conv_Pointer;
		return &ffi_type_pointer;
	}

	/* If we get here, then we don't know what type to use */
	return 0;
}

/*
 * Convert a method signature into the information that
 * we need to call the method.  Returns NULL if something
 * went wrong during the conversion.
 */
static ILMethodCallInfo *MethodSigToCallInfo(ILMethod *method)
{
	ILImage *image = ILProgramItem_Image(method);
	ILMethodCallInfo *callInfo;
	ffi_cif *cif;
	unsigned int nargs;
	ffi_type *rtype;
	ffi_type **args;
	ILType *sig;
	ILType *paramType;
	ILPInvoke *pinv;
	unsigned int argNum;
	unsigned int paramNum;

	/* If the method already has a cif, then return that */
	callInfo = (ILMethodCallInfo *)(ILMethodGetCallInfo(method));
	if(callInfo)
	{
		return callInfo;
	}

	/* Get the method signature */
	sig = ILMethod_Signature(method);

	/* Find the PInvoke data for the method if one is present */
	pinv = ILPInvokeFind(method);

	/* Allocate space for the call information */
	callInfo = (ILMethodCallInfo *)(ILImageAllocLocal
										(image, sizeof(ILMethodCallInfo)));
	if(!callInfo)
	{
		return 0;
	}
	cif = &(callInfo->cif);

	/* Allocate space for the argument type array */
	nargs = (unsigned int)(type->num);
	if(!pinv)
	{
		/* "internalcall" methods have one extra parameter */
		++nargs;
	}
	args = (ffi_type **)(ILImageAllocLocal(image, sizeof(ffi_type *) * nargs));
	if(!args)
	{
		return 0;
	}

	/* Convert the return type into a "ffi_type" value */
	rtype = TypeToFfiType(image, sig->un.method.retType, 0);
	if(!rtype)
	{
		return 0;
	}

	/* Convert the argument types into "ffi_type" values */
	argNum = 0;
	paramNum = 1;
	if(!pinv)
	{
		/* Pass the thread as the first argument of an "internalcall" */
		args[argNum++] = &ffi_pointer;
	}
	while(argNum < nargs)
	{
		/* Get the IL type for the parameter */
		paramType = ILTypeGetParam(sig, paramNum);
		if(!paramType)
		{
			return 0;
		}

		/* Look for a type marshaller for the parameter */

		/* Convert the parameter type into a "ffi_type" value */
		args[argNum] = TypeToFfiType(image, paramType, 0);
		if(!(args[argNum]))
		{
			return 0;
		}

		/* Advance to the next parameter */
		++argNum;
		++paramNum;
	}

	/* Call "ffi_prep_cif" to set everything up for the call */
	if(ffi_prep_cif(cif, FFI_DEFAULT_ABI, nargs, rtype, args) != FFI_OK)
	{
		return 0;
	}

	/* Record the cif for the next time this method is called */
	ILMethodSetCallInfo(method, (void *)cif);

	/* Done */
	return cif;
}

/*
 * Helper macros for argument packing and return value unpacking.
 */
#define	BUF_VAL(type)	(*((type *)(buf + offset)))
#define	RET_VAL(type	(*((type *)(retbuf)))

/*
 * Call a method.
 *
 * This implementation is not terribly efficient, but it should
 * be portable to any CPU and calling conventions that are supported
 * by "libffi".
 */
void CallMethod(ILExecThread *thread, ILMethod *method, ILValue *args)
{
	ILMethodCallInfo *callInfo;
	ffi_cif *cif;
	unsigned arg;
	unsigned argAdjust;
	unsigned char *buf;
	void **ptrs;
	unsigned char *retbuf;
	ffi_type *ffiType;

	/* Get the call information block for the method */
	callInfo = MethodSigToCallInfo(method);
	if(!callInfo)
	{
		/* Something is wrong with the method: return a default value.
		   At some future point, we should make this throw an exception */
		args[0].valueType = IL_TYPE_INT32;
		args[0].un.i4Value = 0;
		return;
	}

	/* Allocate buffers to hold the argument values */
	if(cif->nargs)
	{
		buf = (unsigned char *)alloca(cif->bytes);
		ptrs = (void **)alloca(cif->nargs * sizeof(void *));
	}
	else
	{
		buf = 0;
		ptrs = 0;
	}

	/* Pack the thread pointer into the first argument if "internalcall" */
	if(callInfo->isInternalCall)
	{
		BUF_VAL(void *) = (void *)thread;
		offset = cif->arg_types[0]->size;
		argAdjust = 1;
	}
	else
	{
		offset = 0;
		argAdjust = 0;
	}

	/* Pack the arguments into "buf" and "ptrs" for the call */
	for(arg = 0; arg < (cif->nargs - argAdjust); ++arg)
	{
		/* Align the buffer on the correct boundary */
		ffiType = cif->arg_types[arg + argAdjust];
		offset = (offset + ((ILUInt32)(ffiType->alignment)) - 1) &
					~(((ILUInt32)(ffiType->alignment)) - 1);

		/* Record the current buffer position in the "ptrs" array */
		ptrs[arg + argAdjust] = (void *)buf;

		/* Copy the argument value into "buf" */
		switch(callInfo->conversions[arg])
		{
			case Conv_Void: break;

			case Conv_Boolean_Int8:
			case Conv_Int8:
			{
				/* Convert a "bool" or "int8" value into an "int8" value */
				BUF_VAL(ILInt8) = (ILInt8)(args[arg].un.i4Value);
			}
			break;

			case Conv_Boolean_NativeInt:
			{
				/* Convert a boolean value into an "int" value */
				BUF_VAL(int) = (int)(ILNativeInt)(args[arg].un.i4Value);
			}
			break;

			case Conv_Char_WChar:
			{
				/* Convert a "char" value into a "wchar_t" value */
				BUF_VAL(wchar_t) = (wchar_t)(ILUInt32)(args[arg].un.i4Value);
			}
			break;

			case Conv_Char_UInt16:
			{
				/* Convert a "char" value into a "uint16" value */
				BUF_VAL(ILUInt16) = (ILUInt16)(args[arg].un.i4Value);
			}
			break;

			case Conv_UInt8:
			{
				/* Copy a "uint8" value directly */
				BUF_VAL(ILUInt8) = (ILUInt8)(args[arg].un.i4Value);
			}
			break;

			case Conv_Int16_NativeShort:
			{
				/* Convert an "int16" value into a "short" value */
				BUF_VAL(short) = (short)(args[arg].un.i4Value);
			}
			break;

			case Conv_Int16:
			{
				/* Copy an "int16" value directly */
				BUF_VAL(ILInt16) = (ILInt16)(args[arg].un.i4Value);
			}
			break;

			case Conv_UInt16_NativeUShort:
			{
				/* Convert a "uint16" value into a "ushort" value */
				BUF_VAL(unsigned short) =
						(unsigned short)(args[arg].un.i4Value);
			}
			break;

			case Conv_UInt16:
			{
				/* Copy a "uint16" value directly */
				BUF_VAL(ILUInt16) = (ILUInt16)(args[arg].un.i4Value);
			}
			break;

			case Conv_Int32_NativeInt:
			{
				/* Convert an "int32" value into an "int" value */
				BUF_VAL(int) = (int)(args[arg].un.i4Value);
			}
			break;

			case Conv_Int32:
			{
				/* Copy an "int32" value directly */
				BUF_VAL(ILInt32) = (ILInt32)(args[arg].un.i4Value);
			}
			break;

			case Conv_UInt32_NativeUInt:
			{
				/* Convert a "uint32" value into a "uint" value */
				BUF_VAL(unsigned int) =
						(unsigned int)(ILUInt32)(args[arg].un.i4Value);
			}
			break;

			case Conv_UInt32:
			{
				/* Copy a "uint32" value directly */
				BUF_VAL(ILUInt32) = (ILUInt32)(args[arg].un.i4Value);
			}
			break;

			case Conv_Int64_NativeInt:
			{
				/* Convert an "int64" value into a "long" value */
				BUF_VAL(long) = (long)(args[arg].un.i8Value);
			}
			break;

			case Conv_Int64:
			{
				/* Copy an "int64" value directly */
				BUF_VAL(ILInt64) = (ILInt64)(args[arg].un.i8Value);
			}
			break;

			case Conv_UInt64_NativeUInt:
			{
				/* Convert a "uint64" value into a "ulong" value */
				BUF_VAL(unsigned long) =
						(unsigned long)(args[arg].un.i8Value);
			}
			break;

			case Conv_UInt64:
			{
				/* Copy a "uint64" value directly */
				BUF_VAL(ILUInt64) = (ILUInt64)(args[arg].un.i8Value);
			}
			break;

			case Conv_NativeInt:
			{
				/* Copy a "native int" value directly */
				BUF_VAL(int) = (int)(args[arg].un.iValue);
			}
			break;

			case Conv_NativeUInt:
			{
				/* Copy a "native uint" value directly */
				BUF_VAL(unsigned int) =
					(unsigned int)(ILNativeUInt)(args[arg].un.iValue);
			}
			break;

			case Conv_Float:
			{
				/* Copy a "float" value directly */
				BUF_VAL(float) = (float)(args[arg].un.fValue);
			}
			break;

			case Conv_Double:
			{
				/* Copy a "double" value directly */
				BUF_VAL(double) = (double)(args[arg].un.fValue);
			}
			break;

			case Conv_LongDouble:
			{
				/* Copy a "long double" value directly */
			#ifdef IL_NATIVE_FLOAT
				BUF_VAL(long double) = (long double)(args[arg].un.fValue);
			#else
				BUF_VAL(double) = (double)(args[arg].un.fValue);
			#endif
			}
			break;

			case Conv_ObjectRef:
			case Conv_Pointer:
			{
				/* Copy an object reference or pointer value directly */
				BUF_VAL(void *) = (void *)(args[arg].un.oValue);
			}
			break;

			case Conv_String:
			{
				/* Convert the string into a "char *" array on the stack */
				/* TODO */
				BUF_VAL(void *) = (void *)(args[arg].un.oValue);
			}
			break;
		}

		/* Advance to the next argument buffer position */
		offset += ffiType->size;
	}

	/* Allocate space for the return buffer */
	if(cif->rtype != &ffi_type_void)
	{
		retbuf = (unsigned char *)alloca(cif->rtype->size);
	}
	else
	{
		retbuf = 0;
	}

	/* Perform the call */
	ffi_call(cif, FFN_FN(callInfo->func), retbuf, ptrs);

	/* Unpack the return value into "args[0]" */
	switch(callInfo->returnConversion)
	{
		case Conv_Void:
		{
			/* No return value */
			args[0].valueType = IL_TYPE_VOID;
		}
		break;

		case Conv_Boolean_Int8:
		{
			/* Convert "int8" into "bool" */
			args[0].valueType = IL_TYPE_BOOLEAN;
			args[0].un.i4Value = (RET_VAL(ILInt8) != 0);
		}
		break;

		case Conv_Boolean_NativeInt:
		{
			/* Convert "int" into "bool" */
			args[0].valueType = IL_TYPE_BOOLEAN;
			args[0].un.i4Value = (RET_VAL(int) != 0);
		}
		break;

		case Conv_Char_WChar:
		{
			/* Convert "wchar_t" into "char" */
			args[0].valueType = IL_TYPE_CHAR;
			args[0].un.i4Value = (((ILInt32)(RET_VAL(wchar_t))) & 0xFFFF);
		}
		break;

		case Conv_Char_UInt16:
		{
			/* Convert "uint16" into "char" */
			args[0].valueType = IL_TYPE_CHAR;
			args[0].un.i4Value = ((ILInt32)(RET_VAL(ILUInt16)));
		}
		break;

		case Conv_Int8:
		{
			/* Copy "int8" directly */
			args[0].valueType = IL_TYPE_INT8;
			args[0].un.i4Value = ((ILInt32)(RET_VAL(ILInt8)));
		}
		break;

		case Conv_UInt8:
		{
			/* Copy "uint8" directly */
			args[0].valueType = IL_TYPE_UINT8;
			args[0].un.i4Value = ((ILInt32)(RET_VAL(ILUInt8)));
		}
		break;

		case Conv_Int16_NativeShort:
		{
			/* Convert "short" into "int16" */
			args[0].valueType = IL_TYPE_INT16;
			args[0].un.i4Value = ((ILInt32)(RET_VAL(short)));
		}
		break;

		case Conv_Int16:
		{
			/* Copy "int16" directly */
			args[0].valueType = IL_TYPE_INT16;
			args[0].un.i4Value = ((ILInt32)(RET_VAL(ILInt16)));
		}
		break;

		case Conv_UInt16_NativeUShort:
		{
			/* Convert "ushort" into "uint16" */
			args[0].valueType = IL_TYPE_UINT16;
			args[0].un.i4Value = ((ILInt32)(RET_VAL(unsigned short)));
		}
		break;

		case Conv_UInt16:
		{
			/* Copy "uint16" directly */
			args[0].valueType = IL_TYPE_UINT16;
			args[0].un.i4Value = ((ILInt32)(RET_VAL(ILUInt16)));
		}
		break;

		case Conv_Int32_NativeInt:
		{
			/* Convert "int" into "int32" */
			args[0].valueType = IL_TYPE_INT32;
			args[0].un.i4Value = ((ILInt32)(RET_VAL(int)));
		}
		break;

		case Conv_Int32:
		{
			/* Copy "int32" directly */
			args[0].valueType = IL_TYPE_INT32;
			args[0].un.i4Value = RET_VAL(ILInt32);
		}
		break;

		case Conv_UInt32_NativeUInt:
		{
			/* Convert "uint" into "uint32" */
			args[0].valueType = IL_TYPE_UINT32;
			args[0].un.i4Value = ((ILInt32)(ILUInt32)(RET_VAL(unsigned int)));
		}
		break;

		case Conv_UInt32:
		{
			/* Copy "uint32" directly */
			args[0].valueType = IL_TYPE_UINT32;
			args[0].un.i4Value = ((ILInt32)(RET_VAL(ILUInt32)));
		}
		break;

		case Conv_Int64_NativeLong:
		{
			/* Convert "long" into "int64" */
			args[0].valueType = IL_TYPE_INT64;
			args[0].un.i8Value = ((ILInt64)(RET_VAL(long)));
		}
		break;

		case Conv_Int64:
		{
			/* Copy "int64" directly */
			args[0].valueType = IL_TYPE_INT64;
			args[0].un.i8Value = RET_VAL(ILInt64);
		}
		break;

		case Conv_UInt64_NativeULong:
		{
			/* Convert "ulong" into "uint64" */
			args[0].valueType = IL_TYPE_UINT64;
			args[0].un.i8Value = ((ILInt64)(ILUInt64)(RET_VAL(unsigned long)));
		}
		break;

		case Conv_UInt64:
		{
			/* Copy "uint64" directly */
			args[0].valueType = IL_TYPE_UINT64;
			args[0].un.i8Value = ((ILInt64)(RET_VAL(ILUInt64)));
		}
		break;

		case Conv_NativeInt:
		{
			/* Copy "int" directly */
			args[0].valueType = IL_TYPE_NATIVE_INT;
			args[0].un.iValue = ((ILNativeInt)(RET_VAL(int)));
		}
		break;

		case Conv_NativeUInt:
		{
			/* Copy "uint" directly */
			args[0].valueType = IL_TYPE_NATIVE_UINT;
			args[0].un.iValue =
					((ILNativeInt)(ILNativeUInt)(RET_VAL(unsigned int)));
		}
		break;

		case Conv_Float:
		{
			/* Copy "float" directly */
			args[0].valueType = IL_TYPE_FLOAT32;
			args[0].un.fValue = ((ILNativeFloat)(RET_VAL(float)));
		}
		break;

		case Conv_Double:
		{
			/* Copy "double" directly */
			args[0].valueType = IL_TYPE_FLOAT64;
			args[0].un.fValue = ((ILNativeFloat)(RET_VAL(double)));
		}
		break;

		case Conv_LongDouble:
		{
			/* Copy "long double" directly */
		#ifdef IL_NATIVE_FLOAT
			args[0].valueType = IL_TYPE_NATIVE_FLOAT;
			args[0].un.fValue = ((ILNativeFloat)(RET_VAL(long double)));
		#else
			args[0].valueType = IL_TYPE_FLOAT64;
			args[0].un.fValue = ((ILNativeFloat)(RET_VAL(double)));
		#endif
		}
		break;

		case Conv_ObjectRef:
		{
			/* Copy an object reference directly */
			args[0].valueType = IL_TYPE_OBJECT_REF;
			args[0].un.oValue = RET_VAL(void *);
		}
		break;

		case Conv_Pointer:
		{
			/* Copy a pointer value directly */
			args[0].valueType = IL_TYPE_UNMANGED_PTR;
			args[0].un.oValue = RET_VAL(void *);
		}
		break;

		case Conv_String:
		{
			/* Convert the "char *" return value into a new string */
			/* TODO */
			args[0].valueType = IL_TYPE_OBJECT_REF;
			args[0].un.oValue = RET_VAL(void *);
		}
		break;
	}
}

#ifdef	__cplusplus
};
#endif
