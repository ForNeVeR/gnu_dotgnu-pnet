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

#ifdef	__cplusplus
};
#endif
