/*
 * cvmc_obj.c - Coder implementation for CVM object operations.
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

#ifdef IL_CVMC_CODE

static void CVMCoder_CastClass(ILCoder *coder, ILClass *classInfo,
							   int throwException)
{
	if(ILClass_IsInterface(classInfo))
	{
		/* We are casting to an interface */
		if(throwException)
		{
			CVM_BYTE(COP_CASTINTERFACE);
		}
		else
		{
			CVM_BYTE(COP_ISINTERFACE);
		}
	}
	else
	{
		/* We are casting to a class */
		if(throwException)
		{
			CVM_BYTE(COP_CASTCLASS);
		}
		else
		{
			CVM_BYTE(COP_ISINST);
		}
	}
	CVM_PTR(classInfo);
}

/*
 * Load a simple field relative to a pointer.
 */
static void CVMLoadSimpleField(ILCoder *coder, ILUInt32 offset,
							   int fieldOpcode, int ptrOpcode,
							   int mayBeNull)
{
	if(mayBeNull)
	{
		/* The pointer may be null, so we must check it first */
		if(offset < 256 && fieldOpcode != 0)
		{
			CVM_BYTE(fieldOpcode);
			CVM_BYTE(offset);
		}
		else if(offset < 256)
		{
			CVM_BYTE(COP_CKNULL);
			if(offset != 0)
			{
				CVM_BYTE(COP_PADD_OFFSET);
				CVM_BYTE(offset);
			}
			CVM_BYTE(ptrOpcode);
		}
		else
		{
			CVM_BYTE(COP_CKNULL);
			CVM_BYTE(COP_LDC_I4);
			CVM_WORD(offset);
			CVM_ADJUST(1);
			CVM_BYTE(COP_PADD_I4);
			CVM_ADJUST(-1);
			CVM_BYTE(ptrOpcode);
		}
	}
	else
	{
		/* We can guarantee that the pointer won't be null */
		if(offset < 256)
		{
			if(offset != 0)
			{
				CVM_BYTE(COP_PADD_OFFSET);
				CVM_BYTE(offset);
			}
		}
		else
		{
			CVM_BYTE(COP_LDC_I4);
			CVM_WORD(offset);
			CVM_ADJUST(1);
			CVM_BYTE(COP_PADD_I4);
			CVM_ADJUST(-1);
		}
		CVM_BYTE(ptrOpcode);
	}
}

/*
 * Load a managed value field relative to a pointer.
 */
static void CVMLoadValueField(ILCoder *coder, ILUInt32 offset,
							  ILUInt32 size, int mayBeNull)
{
	if(mayBeNull)
	{
		/* The pointer may be null, so we must check it first */
		CVM_BYTE(COP_CKNULL);
	}
	if(offset < 256)
	{
		CVM_BYTE(COP_PADD_OFFSET);
		CVM_BYTE(offset);
	}
	else
	{
		CVM_BYTE(COP_LDC_I4);
		CVM_WORD(offset);
		CVM_ADJUST(1);
		CVM_BYTE(COP_PADD_I4);
		CVM_ADJUST(-1);
	}
	CVM_WIDE(COP_MREAD, size);
	CVM_ADJUST(-1);
	CVM_ADJUST((size + sizeof(CVMWord) - 1) / sizeof(CVMWord));
}

/*
 * Load the contents of a field, relative to a pointer.
 */
static void CVMLoadField(ILCoder *coder, ILEngineType ptrType,
						 ILType *objectType, ILField *field,
						 ILType *fieldType, ILUInt32 offset,
						 int mayBeNull)
{
	ILUInt32 fieldSize;
	ILUInt32 ptrSize;

	/* Convert the type into its non-enumerated form */
	fieldType = ILTypeGetEnumType(fieldType);

	/* If "ptrType" is MV, then we need to get a pointer
	   to the start of the managed value on the stack */
	if(ptrType == ILEngineType_MV)
	{
		ptrSize = GetTypeSize(objectType);
		CVM_WIDE(COP_MADDR, ptrSize);
	}
	else
	{
		ptrSize = 0;
	}

	/* Access the field relative to the pointer */
	if(ILType_IsPrimitive(fieldType))
	{
		/* Access a primitive type */
		switch(ILType_ToElement(fieldType))
		{
			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			{
				CVMLoadSimpleField(coder, offset, COP_BREAD_FIELD,
								   COP_BREAD, mayBeNull);
			}
			break;

			case IL_META_ELEMTYPE_U1:
			{
				CVMLoadSimpleField(coder, offset, COP_UBREAD_FIELD,
								   COP_UBREAD, mayBeNull);
			}
			break;

			case IL_META_ELEMTYPE_I2:
			{
				CVMLoadSimpleField(coder, offset, COP_SREAD_FIELD,
								   COP_SREAD, mayBeNull);
			}
			break;

			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			{
				CVMLoadSimpleField(coder, offset, COP_USREAD_FIELD,
								   COP_USREAD, mayBeNull);
			}
			break;

			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:
		#ifdef IL_NATIVE_INT32
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
		#endif
			{
				CVMLoadSimpleField(coder, offset, COP_IREAD_FIELD,
								   COP_IREAD, mayBeNull);
			}
			break;

			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
		#ifdef IL_NATIVE_INT64
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
		#endif
			{
				CVMLoadValueField(coder, offset, sizeof(ILInt64), mayBeNull);
			}
			break;

			case IL_META_ELEMTYPE_R4:
			{
				CVMLoadSimpleField(coder, offset, 0, COP_FREAD, mayBeNull);
				CVM_ADJUST(CVM_WORDS_PER_NATIVE_FLOAT - 1);
			}
			break;

			case IL_META_ELEMTYPE_R8:
			{
				CVMLoadSimpleField(coder, offset, 0, COP_DREAD, mayBeNull);
				CVM_ADJUST(CVM_WORDS_PER_NATIVE_FLOAT - 1);
			}
			break;

			case IL_META_ELEMTYPE_R:
			{
				CVMLoadValueField(coder, offset,
								  sizeof(ILNativeFloat), mayBeNull);
			}
			break;

			case IL_META_ELEMTYPE_TYPEDBYREF:
			{
				CVMLoadValueField(coder, offset,
								  sizeof(ILTypedRef), mayBeNull);
			}
			break;
		}
	}
	else if(ILType_IsValueType(fieldType))
	{
		/* Access a value type */
		CVMLoadValueField(coder, offset, ILSizeOfType(fieldType), mayBeNull);
	}
	else
	{
		/* Access a reference type */
		CVMLoadSimpleField(coder, offset, COP_PREAD_FIELD,
						   COP_PREAD, mayBeNull);
	}

	/* If "ptrType" is MV, then we need to squash the stack
	   to remove the managed value */
	if(ptrType == ILEngineType_MV)
	{
		fieldSize = GetStackTypeSize(fieldType);
		CVM_DWIDE(COP_SQUASH, fieldSize, ptrSize);
		CVM_ADJUST(-((ILInt32)ptrSize));
	}
}

static void CVMCoder_LoadField(ILCoder *coder, ILEngineType ptrType,
							   ILType *objectType, ILField *field,
							   ILType *fieldType)
{
#ifdef IL_NATIVE_INT64
	/* Convert I4 to I if necessary */
	if(ptrType == ILEngineType_I4)
	{
		CVM_BYTE(COP_IU2L);
	}
#endif
	CVMLoadField(coder, ptrType, objectType, field,
				 fieldType, field->offset, 1);
}

/* Forward declaration: defined in "cvmc_call.c" */
static void CallStaticConstructor(ILCoder *coder, ILClass *classInfo,
								  int isCtor);

static void CVMCoder_LoadStaticField(ILCoder *coder, ILField *field,
							         ILType *fieldType)
{
	ILClass *classInfo;

	if((field->member.attributes & IL_META_FIELDDEF_HAS_FIELD_RVA) == 0)
	{
		/* Call the static constructor if necessary */
		classInfo = ILField_Owner(field);
		CallStaticConstructor(coder, classInfo, 1);

		/* Push a pointer to the class's static data area */
		CVM_BYTE(COP_GET_STATIC);
		CVM_PTR(classInfo);
		CVM_ADJUST(1);

		/* Load the field relative to the pointer */
		CVMLoadField(coder, ILEngineType_M, 0, field,
					 fieldType, field->offset, 0);
	}
	else
	{
		/* RVA-based static field */
		ILFieldRVA *fieldRVA = ILFieldRVAGetFromOwner(field);
		ILUInt32 rva = ILFieldRVAGetRVA(fieldRVA);
		CVM_BYTE(COP_LDRVA);
		CVM_WORD(rva);
		CVM_ADJUST(1);

		/* Load the field directly from the pointer */
		CVMLoadField(coder, ILEngineType_M, 0, field, fieldType, 0, 0);
	}
}

static void CVMCoder_LoadFieldAddr(ILCoder *coder, ILEngineType ptrType,
							       ILType *objectType, ILField *field,
							       ILType *fieldType)
{
#ifdef IL_NATIVE_INT64
	/* Convert I4 to I if necessary */
	if(ptrType == ILEngineType_I4)
	{
		CVM_BYTE(COP_IU2L);
	}
#endif
	CVM_BYTE(COP_CKNULL);
	if(field->offset < 256)
	{
		if(field->offset != 0)
		{
			CVM_BYTE(COP_PADD_OFFSET);
			CVM_BYTE(field->offset);
		}
	}
	else
	{
		CVM_BYTE(COP_LDC_I4);
		CVM_WORD(field->offset);
		CVM_ADJUST(1);
		CVM_BYTE(COP_PADD_I4);
		CVM_ADJUST(-1);
	}
}

static void CVMCoder_LoadStaticFieldAddr(ILCoder *coder, ILField *field,
							             ILType *fieldType)
{
	ILClass *classInfo;

	/* Call the static constructor if necessary */
	classInfo = ILField_Owner(field);
	CallStaticConstructor(coder, classInfo, 1);

	/* Regular or RVA field? */
	if((field->member.attributes & IL_META_FIELDDEF_HAS_FIELD_RVA) == 0)
	{
		/* Push a pointer to the class's static data area */
		CVM_BYTE(COP_GET_STATIC);
		CVM_PTR(classInfo);
		CVM_ADJUST(1);

		/* Add the offset to the pointer */
		if(field->offset < 256)
		{
			if(field->offset != 0)
			{
				CVM_BYTE(COP_PADD_OFFSET);
				CVM_BYTE(field->offset);
			}
		}
		else
		{
			CVM_BYTE(COP_LDC_I4);
			CVM_WORD(field->offset);
			CVM_ADJUST(1);
			CVM_BYTE(COP_PADD_I4);
			CVM_ADJUST(-1);
		}
	}
	else
	{
		/* RVA-based static field */
		ILFieldRVA *fieldRVA = ILFieldRVAGetFromOwner(field);
		ILUInt32 rva = ILFieldRVAGetRVA(fieldRVA);
		CVM_BYTE(COP_LDRVA);
		CVM_WORD(rva);
		CVM_ADJUST(1);
	}
}

/*
 * Store into a field with reversed pointer and value arguments.
 */
static void CVMStoreFieldReverse(ILCoder *coder, ILField *field,
								 ILType *fieldType, ILUInt32 offset,
								 int mayBeNull)
{
	ILUInt32 size;

	/* Check for null and adjust the pointer for the offset */
	if(mayBeNull)
	{
		CVM_BYTE(COP_CKNULL);
	}
	if(offset < 256)
	{
		if(offset != 0)
		{
			CVM_BYTE(COP_PADD_OFFSET);
			CVM_BYTE(offset);
		}
	}
	else
	{
		CVM_BYTE(COP_LDC_I4);
		CVM_WORD(offset);
		CVM_ADJUST(1);
		CVM_BYTE(COP_PADD_I4);
		CVM_ADJUST(-1);
	}

	/* Convert the type into its non-enumerated form */
	fieldType = ILTypeGetEnumType(fieldType);

	/* Store the value into the pointer */
	if(ILType_IsPrimitive(fieldType))
	{
		/* Store a primitive value */
		switch(ILType_ToElement(fieldType))
		{
			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			{
				CVM_BYTE(COP_BWRITE_R);
			}
			break;

			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			{
				CVM_BYTE(COP_SWRITE_R);
			}
			break;

			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:
		#ifdef IL_NATIVE_INT32
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
		#endif
			{
				CVM_BYTE(COP_IWRITE_R);
			}
			break;

			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
		#ifdef IL_NATIVE_INT64
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
		#endif
			{
				CVM_WIDE(COP_MWRITE_R, sizeof(ILInt64));
			}
			break;

			case IL_META_ELEMTYPE_R4:
			{
				CVM_BYTE(COP_FWRITE_R);
			}
			break;

			case IL_META_ELEMTYPE_R8:
			{
				CVM_BYTE(COP_DWRITE_R);
			}
			break;

			case IL_META_ELEMTYPE_R:
			{
				CVM_WIDE(COP_MWRITE_R, sizeof(ILNativeFloat));
			}
			break;

			case IL_META_ELEMTYPE_TYPEDBYREF:
			{
				CVM_WIDE(COP_MWRITE_R, sizeof(ILTypedRef));
			}
			break;
		}
	}
	else if(ILType_IsValueType(fieldType))
	{
		size = ILSizeOfType(fieldType);
		CVM_WIDE(COP_MWRITE_R, size);
	}
	else
	{
		CVM_BYTE(COP_PWRITE_R);
	}
}

/*
 * Store into a field.
 */
static void CVMStoreField(ILCoder *coder, ILField *field,
						  ILType *fieldType, ILUInt32 offset,
						  ILUInt32 valueSize)
{
	ILUInt32 size;

	/* Convert the type into its non-enumerated form */
	fieldType = ILTypeGetEnumType(fieldType);

	/* Determine if we can use a direct field write instruction */
	if(offset < 256)
	{
		if(ILType_IsPrimitive(fieldType))
		{
			switch(ILType_ToElement(fieldType))
			{
				case IL_META_ELEMTYPE_BOOLEAN:
				case IL_META_ELEMTYPE_I1:
				case IL_META_ELEMTYPE_U1:
				{
					CVM_BYTE(COP_BWRITE_FIELD);
					CVM_BYTE(offset);
					return;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_I2:
				case IL_META_ELEMTYPE_U2:
				case IL_META_ELEMTYPE_CHAR:
				{
					CVM_BYTE(COP_SWRITE_FIELD);
					CVM_BYTE(offset);
					return;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_I4:
				case IL_META_ELEMTYPE_U4:
			#ifdef IL_NATIVE_INT32
				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
			#endif
				{
					CVM_BYTE(COP_IWRITE_FIELD);
					CVM_BYTE(offset);
					return;
				}
				/* Not reached */

				default: break;
			}
		}
		else if(!ILType_IsValueType(fieldType))
		{
			/* Store a reference value */
			CVM_BYTE(COP_PWRITE_FIELD);
			CVM_BYTE(offset);
			return;
		}
	}

	/* Determine if we need to do a forward or reverse store */
	if(offset < 256)
	{
		/* Check the pointer value for null */
		CVM_WIDE(COP_CKNULL_N, valueSize);

		/* Add the offset to the pointer value */
		if(offset != 0)
		{
			CVM_BYTE(COP_PADD_OFFSET_N);
			CVM_BYTE(valueSize);
		}

		/* Perform a forward store */
		if(ILType_IsPrimitive(fieldType))
		{
			/* Store a primitive value */
			switch(ILType_ToElement(fieldType))
			{
				case IL_META_ELEMTYPE_BOOLEAN:
				case IL_META_ELEMTYPE_I1:
				case IL_META_ELEMTYPE_U1:
				{
					CVM_BYTE(COP_BWRITE);
				}
				break;

				case IL_META_ELEMTYPE_I2:
				case IL_META_ELEMTYPE_U2:
				case IL_META_ELEMTYPE_CHAR:
				{
					CVM_BYTE(COP_SWRITE);
				}
				break;

				case IL_META_ELEMTYPE_I4:
				case IL_META_ELEMTYPE_U4:
			#ifdef IL_NATIVE_INT32
				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
			#endif
				{
					CVM_BYTE(COP_IWRITE);
				}
				break;

				case IL_META_ELEMTYPE_I8:
				case IL_META_ELEMTYPE_U8:
			#ifdef IL_NATIVE_INT64
				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
			#endif
				{
					CVM_WIDE(COP_MWRITE, sizeof(ILInt32));
				}
				break;

				case IL_META_ELEMTYPE_R4:
				{
					CVM_BYTE(COP_FWRITE);
				}
				break;

				case IL_META_ELEMTYPE_R8:
				{
					CVM_BYTE(COP_DWRITE);
				}
				break;

				case IL_META_ELEMTYPE_R:
				{
					CVM_WIDE(COP_MWRITE, sizeof(ILNativeFloat));
				}
				break;

				case IL_META_ELEMTYPE_TYPEDBYREF:
				{
					CVM_WIDE(COP_MWRITE, sizeof(ILTypedRef));
				}
				break;
			}
		}
		else if(ILType_IsValueType(fieldType))
		{
			/* Store a managed value */
			size = ILSizeOfType(fieldType);
			CVM_WIDE(COP_MWRITE, size);
		}
		else
		{
			/* Store a reference value */
			CVM_BYTE(COP_PWRITE);
		}
	}
	else
	{
		/* Bring the pointer to the top of the stack */
		CVM_WIDE(COP_DUP_WORD_N, valueSize);
		CVM_ADJUST(1);

		/* Perform a reversed store */
		CVMStoreFieldReverse(coder, field, fieldType, offset, 1);
		CVM_ADJUST(-1);
	}
}

static void CVMCoder_StoreField(ILCoder *coder, ILEngineType ptrType,
							    ILType *objectType, ILField *field,
							    ILType *fieldType, ILEngineType valueType)
{
	ILUInt32 valueSize = GetStackTypeSize(fieldType);

#ifdef IL_NATIVE_INT64
	/* Convert I4 to I if necessary */
	if(ptrType == ILEngineType_I4)
	{
		CVM_WIDE(COP_I2P_LOWER, valueSize);
	}
#endif

	/* Store into the field */
	CVMStoreField(coder, field, fieldType, field->offset, valueSize);

	/* Adjust the stack to compensate for the store */
	CVM_ADJUST(-((ILInt32)(valueSize + 1)));
}

static void CVMCoder_StoreStaticField(ILCoder *coder, ILField *field,
							          ILType *fieldType, ILEngineType valueType)
{
	ILUInt32 valueSize = GetStackTypeSize(fieldType);
	ILClass *classInfo;

	/* Call the static constructor if necessary */
	classInfo = ILField_Owner(field);
	CallStaticConstructor(coder, classInfo, 1);

	/* Regular or RVA field? */
	if((field->member.attributes & IL_META_FIELDDEF_HAS_FIELD_RVA) == 0)
	{
		/* Push a pointer to the class's static data area */
		CVM_BYTE(COP_GET_STATIC);
		CVM_PTR(classInfo);
		CVM_ADJUST(1);

		/* Store the field relative to the pointer */
		CVMStoreFieldReverse(coder, field, fieldType, field->offset, 0);
	}
	else
	{
		/* RVA-based static field */
		ILFieldRVA *fieldRVA = ILFieldRVAGetFromOwner(field);
		ILUInt32 rva = ILFieldRVAGetRVA(fieldRVA);
		CVM_BYTE(COP_LDRVA);
		CVM_WORD(rva);
		CVM_ADJUST(1);

		/* Store the field directly to the pointer */
		CVMStoreFieldReverse(coder, field, fieldType, 0, 0);
	}

	/* Adjust the stack to compensate for the store */
	CVM_ADJUST(-((ILInt32)(valueSize + 1)));
}

static void CVMCoder_CopyObject(ILCoder *coder, ILEngineType destPtrType,
							    ILEngineType srcPtrType, ILClass *classInfo)
{
	ILUInt32 size;

#ifdef IL_NATIVE_INT64
	/* Normalize the pointers */
	if(destPtrType == ILEngineType_I4)
	{
		CVM_WIDE(COP_I2P_LOWER, 1);
	}
	if(srcPtrType == ILEngineType_I4)
	{
		CVM_WIDE(COP_I2P_LOWER, 0);
	}
#endif

	/* Check the values for null */
	if(destPtrType != ILEngineType_M && destPtrType != ILEngineType_T)
	{
		CVM_WIDE(COP_CKNULL_N, 1);
	}
	if(srcPtrType != ILEngineType_M && srcPtrType != ILEngineType_T)
	{
		CVM_BYTE(COP_CKNULL);
	}

	/* Copy the memory block */
	size = ILSizeOfType(ILType_FromValueType(classInfo));
	CVM_WIDE(COP_MEMCPY, size);
	CVM_ADJUST(-2);
}

static void CVMCoder_CopyBlock(ILCoder *coder, ILEngineType destPtrType,
							   ILEngineType srcPtrType)
{
#ifdef IL_NATIVE_INT64
	/* Normalize the pointers */
	if(destPtrType == ILEngineType_I4)
	{
		CVM_WIDE(COP_I2P_LOWER, 2);
	}
	if(srcPtrType == ILEngineType_I4)
	{
		CVM_WIDE(COP_I2P_LOWER, 1);
	}
#endif

	/* Check the values for null */
	if(destPtrType != ILEngineType_M && destPtrType != ILEngineType_T)
	{
		CVM_WIDE(COP_CKNULL_N, 2);
	}
	if(srcPtrType != ILEngineType_M && srcPtrType != ILEngineType_T)
	{
		CVM_WIDE(COP_CKNULL_N, 1);
	}

	/* Copy the memory block */
	CVM_BYTE(COP_MEMMOVE);
	CVM_ADJUST(-3);
}

static void CVMCoder_InitObject(ILCoder *coder, ILEngineType ptrType,
							    ILClass *classInfo)
{
	ILUInt32 size;

#ifdef IL_NATIVE_INT64
	/* Normalize the pointer */
	if(ptrType == ILEngineType_I4)
	{
		CVM_WIDE(COP_I2P_LOWER, 0);
	}
#endif

	/* Check the pointer value for null */
	if(ptrType != ILEngineType_M && ptrType != ILEngineType_T)
	{
		CVM_BYTE(COP_CKNULL);
	}

	/* Initialize the block to all-zeroes */
	size = ILSizeOfType(ILType_FromValueType(classInfo));
	CVM_WIDE(COP_MEMZERO, size);
	CVM_ADJUST(-1);
}

static void CVMCoder_InitBlock(ILCoder *coder, ILEngineType ptrType)
{
#ifdef IL_NATIVE_INT64
	/* Normalize the pointer */
	if(ptrType == ILEngineType_I4)
	{
		CVM_WIDE(COP_I2P_LOWER, 2);
	}
#endif

	/* Check the pointer value for null */
	if(ptrType != ILEngineType_M && ptrType != ILEngineType_T)
	{
		CVM_WIDE(COP_CKNULL_N, 2);
	}

	/* Initialize the block to bytes of the same value */
	CVM_BYTE(COP_MEMSET);
	CVM_ADJUST(-3);
}

static void CVMCoder_Box(ILCoder *coder, ILClass *boxClass,
					     ILEngineType valueType, ILUInt32 size)
{
	/* TODO */
}

static void CVMCoder_BoxSmaller(ILCoder *coder, ILClass *boxClass,
					   		    ILEngineType valueType, ILType *smallerType)
{
	/* TODO */
}

static void CVMCoder_Unbox(ILCoder *coder, ILClass *boxClass)
{
	/* We don't have to do anything here: the object reference
	   points at the start of the object's fields, which is
	   exactly the pointer that we need for the unboxed value */
}

static void CVMCoder_MakeTypedRef(ILCoder *coder, ILClass *classInfo)
{
	/* TODO */
}

static void CVMCoder_RefAnyVal(ILCoder *coder, ILClass *classInfo)
{
	/* TODO */
}

static void CVMCoder_RefAnyType(ILCoder *coder)
{
	/* TODO */
}

static void CVMCoder_PushToken(ILCoder *coder, ILProgramItem *item)
{
	CVM_BYTE(COP_LDTOKEN);
	CVM_PTR(item);
	CVM_ADJUST(1);
}

static void CVMCoder_SizeOf(ILCoder *coder, ILType *type)
{
	ILUInt32 size = ILSizeOfType(type);
	if(size <= 8)
	{
		CVM_BYTE(COP_LDC_I4_0 + size);
	}
	else if(size < 128)
	{
		CVM_BYTE(COP_LDC_I4_S);
		CVM_BYTE(size);
	}
	else
	{
		CVM_BYTE(COP_LDC_I4);
		CVM_WORD(size);
	}
	CVM_ADJUST(1);
}

static void CVMCoder_ArgList(ILCoder *coder)
{
	/* TODO */
}

#endif	/* IL_CVMC_CODE */
