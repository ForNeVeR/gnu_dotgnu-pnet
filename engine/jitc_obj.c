/*
 * jitc_obj.c - Coder implementation for JIT object operations.
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

#ifdef IL_JITC_CODE

static void JITCoder_CastClass(ILCoder *coder, ILClass *classInfo,
							   int throwException)
{
}

static void JITCoder_LoadField(ILCoder *coder, ILEngineType ptrType,
							   ILType *objectType, ILField *field,
							   ILType *fieldType)
{
}

static void JITCoder_LoadThisField(ILCoder *coder, ILField *field,
							   	   ILType *fieldType)
{
}

static void JITCoder_LoadStaticField(ILCoder *coder, ILField *field,
							         ILType *fieldType)
{
}

static void JITCoder_LoadFieldAddr(ILCoder *coder, ILEngineType ptrType,
							       ILType *objectType, ILField *field,
							       ILType *fieldType)
{
}

static void JITCoder_LoadStaticFieldAddr(ILCoder *coder, ILField *field,
					 ILType *fieldType)
{
}

static void JITCoder_StoreField(ILCoder *coder, ILEngineType ptrType,
							    ILType *objectType, ILField *field,
							    ILType *fieldType, ILEngineType valueType)
{
}

static void JITCoder_StoreStaticField(ILCoder *coder, ILField *field,
				      ILType *fieldType, ILEngineType valueType)
{
}

static void JITCoder_CopyObject(ILCoder *coder, ILEngineType destPtrType,
							    ILEngineType srcPtrType, ILClass *classInfo)
{
}

static void JITCoder_CopyBlock(ILCoder *coder, ILEngineType destPtrType,
							   ILEngineType srcPtrType)
{
}

static void JITCoder_InitObject(ILCoder *coder, ILEngineType ptrType,
				ILClass *classInfo)
{
}

static void JITCoder_InitBlock(ILCoder *coder, ILEngineType ptrType)
{
}

static void JITCoder_Box(ILCoder *coder, ILClass *boxClass,
					     ILEngineType valueType, ILUInt32 size)
{
}

static void JITCoder_BoxSmaller(ILCoder *coder, ILClass *boxClass,
					   		    ILEngineType valueType, ILType *smallerType)
{
}

static void JITCoder_Unbox(ILCoder *coder, ILClass *boxClass)
{
}

static void JITCoder_MakeTypedRef(ILCoder *coder, ILClass *classInfo)
{
}

static void JITCoder_RefAnyVal(ILCoder *coder, ILClass *classInfo)
{
}

static void JITCoder_RefAnyType(ILCoder *coder)
{
}

static void JITCoder_PushToken(ILCoder *coder, ILProgramItem *item)
{
}

static void JITCoder_SizeOf(ILCoder *coder, ILType *type)
{
}

static void JITCoder_ArgList(ILCoder *coder)
{
}

#endif	/* IL_JITC_CODE */
