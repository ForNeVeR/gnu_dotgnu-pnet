/*
 * lib_gc.c - Internalcall methods for the classes "System.GC" and
 *					"System.Runtime.InteropServices.GCHandle".
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static void KeepAlive(Object obj);
 */
void _IL_GC_KeepAlive(ILExecThread *_thread, ILObject *obj)
{
	/* TODO */
}

/*
 * public static void ReRegisterForFinalize(Object obj);
 */
void _IL_GC_ReRegisterForFinalize(ILExecThread *_thread, ILObject *obj)
{
	/* TODO */
}

/*
 * public static void SuppressFinalize(Object obj);
 */
void _IL_GC_SuppressFinalize(ILExecThread *_thread, ILObject *obj)
{
	/* TODO */
}

/*
 * public static void WaitForPendingFinalizers();
 */
void _IL_GC_WaitForPendingFinalizers(ILExecThread * _thread)
{
	/* TODO */
}

ILNativeInt _IL_GCHandle_GCAddrOfPinnedObject(ILExecThread *_thread,
											  ILInt32 handle)
{
	/* TODO */
	return 0;
}

ILInt32 _IL_GCHandle_GCAlloc(ILExecThread *_thread, ILObject *value,
							 ILInt32 type)
{
	/* TODO */
	return 0;
}

void _IL_GCHandle_GCFree(ILExecThread *_thread, ILInt32 handle)
{
	/* TODO */
}

ILBool _IL_GCHandle_GCValidate(ILExecThread *_thread, ILInt32 handle)
{
	/* TODO */
	return 0;
}

ILObject *_IL_GCHandle_GCGetTarget(ILExecThread *_thread, ILInt32 handle)
{
	/* TODO */
	return 0;
}

void _IL_GCHandle_GCSetTarget(ILExecThread *_thread, ILInt32 handle,
							  ILObject *value, ILBool isPinned)
{
	/* TODO */
}

#ifdef	__cplusplus
};
#endif
