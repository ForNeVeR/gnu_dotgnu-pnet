/*
 * lib_misc.c - Internalcall methods for misc "System" classes.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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
#include "lib_defs.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static Guid NewGuid();
 */
void _IL_Guid_NewGuid(ILExecThread *thread, void *result)
{
	ILGUIDGenerate((unsigned char *)result);
}

/*
 * private static bool GetLittleEndian();
 */
ILBool _IL_BitConverter_GetLittleEndian(ILExecThread *thread)
{
#if defined(__i386) || defined(__i386__)
	return 1;
#else
	union
	{
		unsigned char bytes[4];
		ILUInt32      value;

	} convert;
	convert.value = (ILUInt32)0x01020304;
	return (convert.bytes[0] == (unsigned char)0x04);
#endif
}

/*
 * public static long DoubleToInt64Bits(double value);
 */
ILInt64 _IL_BitConverter_DoubleToInt64Bits(ILExecThread *thread,
										   ILDouble value)
{
	union
	{
		ILDouble input;
		ILInt64  output;

	} convert;
	convert.input = value;
	return convert.output;
}

/*
 * public static double Int64BitsToDouble(long value);
 */
ILDouble _IL_BitConverter_Int64BitsToDouble(ILExecThread *thread,
											ILInt64 value)
{
	union
	{
		ILInt64  input;
		ILDouble output;

	} convert;
	convert.input = value;
	return convert.output;
}

/*
 * public static int FloatToInt32Bits(float value);
 */
ILInt32 _IL_BitConverter_FloatToInt32Bits(ILExecThread *thread,
										  ILFloat value)
{
	union
	{
		ILFloat input;
		ILInt32 output;

	} convert;
	convert.input = value;
	return convert.output;
}

/*
 * public static float Int32BitsToFloat(int value);
 */
ILFloat _IL_BitConverter_Int32BitsToFloat(ILExecThread *thread,
										  ILInt32 value)
{
	union
	{
		ILInt32 input;
		ILFloat output;

	} convert;
	convert.input = value;
	return convert.output;
}

/*
 * public ArgIterator(RuntimeArgumentHandle argList);
 */
void _IL_ArgIterator_ctor_RuntimeArgumentHandle(ILExecThread *_thread,
												void *_this, void *argList)
{
	/* TODO */
}

/*
 * public ArgIterator(RuntimeArgumentHandle argList, void *ptr);
 */
void _IL_ArgIterator_ctor_RuntimeArgumentHandlepV(ILExecThread *_thread,
												  void *_this, void *argList,
												  void *ptr)
{
	/* TODO */
}

/*
 * public TypedReference GetNextArg();
 */
ILTypedRef _IL_ArgIterator_GetNextArg_(ILExecThread *_thread, void *_this)
{
	/* TODO */
	ILTypedRef ref;
	ref.type = 0;
	ref.value = 0;
	return ref;
}

/*
 * public TypedReference GetNextArg(RuntimeTypeHandle type);
 */
ILTypedRef _IL_ArgIterator_GetNextArg_RuntimeTypeHandle(ILExecThread *_thread,
														void *_this,
														void *type)
{
	/* TODO */
	ILTypedRef ref;
	ref.type = 0;
	ref.value = 0;
	return ref;
}

/*
 * public RuntimeTypeHandle GetNextArgType();
 */
void _IL_ArgIterator_GetNextArgType(ILExecThread *_thread,
									void *_result, void *_this)
{
	/* TODO */
}

/*
 * public int GetNextArgType();
 */
ILInt32 _IL_ArgIterator_GetRemainingCount(ILExecThread *_thread,
										  void *_this)
{
	/* TODO */
	return 0;
}

/*
 * private static Delegate CreateBlankDelegate(Type type, ClrMethod method);
 */
ILObject *_IL_Delegate_CreateBlankDelegate(ILExecThread *_thread,
										   ILObject *type,
										   ILObject *method)
{
	/* TODO */
	return 0;
}

/*
 * private static TypedReference ClrMakeTypedReference(Object target,
 *													   FieldInfo[] flds);
 */
ILTypedRef _IL_TypedReference_ClrMakeTypedReference(ILExecThread *_thread,
													ILObject *target,
													System_Array *flds)
{
	/* TODO */
	ILTypedRef ref;
	ref.type = 0;
	ref.value = 0;
	return ref;
}

/*
 * public static void SetTypedReference(TypedReference target,
 *										Object value);
 */
void _IL_TypedReference_SetTypedReference(ILExecThread *_thread,
										  ILTypedRef target,
										  ILObject *value)
{
	/* TODO */
}

/*
 * public static Object ToObject();
 */
ILObject *_IL_TypedReference_ToObject(ILExecThread *_thread, ILTypedRef value)
{
	/* TODO */
	return 0;
}

#ifdef	__cplusplus
};
#endif
