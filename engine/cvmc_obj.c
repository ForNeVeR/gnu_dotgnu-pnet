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
	/* TODO */
}

static void CVMCoder_LoadField(ILCoder *coder, ILEngineType ptrType,
							   ILType *objectType, ILField *field,
							   ILType *fieldType)
{
	/* TODO */
}

static void CVMCoder_LoadStaticField(ILCoder *coder, ILField *field,
							         ILType *fieldType)
{
	/* TODO */
}

static void CVMCoder_LoadFieldAddr(ILCoder *coder, ILEngineType ptrType,
							       ILType *objectType, ILField *field,
							       ILType *fieldType)
{
	/* TODO */
}

static void CVMCoder_LoadStaticFieldAddr(ILCoder *coder, ILField *field,
							             ILType *fieldType)
{
	/* TODO */
}

static void CVMCoder_StoreField(ILCoder *coder, ILEngineType ptrType,
							    ILType *objectType, ILField *field,
							    ILType *fieldType, ILEngineType valueType)
{
	/* TODO */
}

static void CVMCoder_StoreStaticField(ILCoder *coder, ILField *field,
							          ILType *fieldType, ILEngineType valueType)
{
	/* TODO */
}

static void CVMCoder_CopyObject(ILCoder *coder, ILEngineType destPtrType,
							    ILEngineType srcPtrType, ILClass *classInfo)
{
	/* TODO */
}

static void CVMCoder_CopyBlock(ILCoder *coder, ILEngineType destPtrType,
							   ILEngineType srcPtrType)
{
	/* TODO */
}

static void CVMCoder_InitObject(ILCoder *coder, ILEngineType ptrType,
							    ILClass *classInfo)
{
	/* TODO */
}

static void CVMCoder_InitBlock(ILCoder *coder, ILEngineType ptrType)
{
	/* TODO */
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
	/* TODO */
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
	/* TODO */
}

static void CVMCoder_SizeOf(ILCoder *coder, ILType *type)
{
	/* TODO */
}

static void CVMCoder_ArgList(ILCoder *coder)
{
	/* TODO */
}

#endif	/* IL_CVMC_CODE */
