/*
 * lib_emit.c - Internalcall methods for the "Reflection.Emit" classes.
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
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
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

#ifdef IL_CONFIG_REFLECTION

/*
 * private static IntPtr ClrAssemblyCreate(String name, int v1, int v2,
 *										   int v3, int v4,
 *                                         AssemblyBuilderAccess access,
 *										   out IntPtr writer);
 */
ILNativeInt _IL_AssemblyBuilder_ClrAssemblyCreate
		(ILExecThread *_thread, ILString *name, ILInt32 v1,
		 ILInt32 v2, ILInt32 v3, ILInt32 v4, ILInt32 access,
		 ILNativeInt *writer)
{
	/* TODO */
	*writer = 0;
	return 0;
}

/*
 * private void ClrSetEntryPoint(IntPtr nativeMethod, PEFileKinds fileKind);
 */
void _IL_AssemblyBuilder_ClrSetEntryPoint(ILExecThread *thread,
										  ILObject *_this,
										  ILNativeInt clrMethod,
										  ILInt32 fileKind)
{
	/* TODO */
}

/*
 * internal static int ClrGetItemToken(IntPtr item);
 */
ILInt32 _IL_AssemblyBuilder_ClrGetItemToken(ILExecThread *_thread,
											ILNativeInt item)
{
	if(item)
	{
		return ILProgramItem_Token((ILProgramItem *)item);
	}
	else
	{
		return 0;
	}
}

/*
 * private static IntPtr ClrEventCreate(IntPtr classInfo, String name,
 *									    Type type, EventAttributes attrs);
 */
ILNativeInt _IL_EventBuilder_ClrEventCreate(ILExecThread *_thread,
											ILNativeInt classInfo,
											ILString *name, ILObject *type,
											ILInt32 attrs)
{
	/* TODO */
	return 0;
}

/*
 * private static ClrEventAddSemantics(IntPtr eventInfo,
 *									   MethodSemanticsAttributes attr,
 *									   MethodToken token);
 */
void _IL_EventBuilder_ClrEventAddSemantics(ILExecThread *_thread,
										   ILNativeInt eventInfo,
										   ILInt32 attr, void *token)
{
	/* TODO */
}

/*
 * private static IntPtr ClrFieldCreate(IntPtr classInfo, String name,
 *										Type type, FieldAttributes attrs);
 */
ILNativeInt _IL_FieldBuilder_ClrFieldCreate(ILExecThread *_thread,
											ILNativeInt classInfo,
											ILString *name, ILObject *type,
											ILInt32 attrs)
{
	/* TODO */
	return 0;
}

/*
 * private static void ClrFieldSetConstant(IntPtr item, Object value);
 */
void _IL_FieldBuilder_ClrFieldSetConstant(ILExecThread *_thread,
										  ILNativeInt item,
										  ILObject *value)
{
	/* TODO */
}

/*
 * private static void ClrFieldSetMarshal(IntPtr item, byte[] data);
 */
void _IL_FieldBuilder_ClrFieldSetMarshal(ILExecThread *_thread,
										 ILNativeInt item,
										 System_Array *data)
{
	/* TODO */
}

/*
 * private static void ClrFieldSetOffset(IntPtr item, int offset);
 */
void _IL_FieldBuilder_ClrFieldSetOffset(ILExecThread *_thread,
										ILNativeInt item, ILInt32 offset)
{
	/* TODO */
}

/*
 * private IntPtr ClrModuleCreate(IntPtr assembly, String name);
 */
ILNativeInt _IL_ModuleBuilder_ClrModuleCreate(ILExecThread *_thread,
											  ILNativeInt assembly,
											  ILString *name)
{
	ILImage *image = ILProgramItem_Image(assembly);
	ILToken token = ILProgramItem_Token(assembly);
	const char *str = (const char *)ILStringToAnsi(_thread, name);
	return (ILNativeInt)ILModuleCreate(image, token, str, NULL);
}

/*
 * private static IntPtr ClrPropertyCreate(IntPtr classInfo, String name,
 *										   PropertyAttributes attrs,
 *										   Type returnType,
 *										   Type[] paramTypes);
 */
ILNativeInt _IL_PropertyBuilder_ClrPropertyCreate(ILExecThread *_thread,
												  ILNativeInt classInfo,
												  ILString *name,
												  ILInt32 attrs,
												  ILObject *returnType,
												  System_Array *paramTypes)
{
	/* TODO */
	return 0;
}

/*
 * private static void ClrPropertyAddSemantics(IntPtr item,
 *											   MethodSemanticsAttributes attr,
 *											   MethodToken token);
 */
void _IL_PropertyBuilder_ClrPropertyAddSemantics(ILExecThread *_thread,
												 ILNativeInt item,
												 ILInt32 attr,
												 void *token)
{
	/* TODO */
}

/*
 * private static void ClrPropertySetConstant(IntPtr item, Object value);
 */
void _IL_PropertyBuilder_ClrPropertySetConstant(ILExecThread *_thread,
												ILNativeInt item,
												ILObject *value)
{
	/* TODO */
}

#endif /* IL_CONFIG_REFLECTION */

#ifdef	__cplusplus
};
#endif
