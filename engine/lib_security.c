/*
 * lib_security.c - Internalcall methods for security-related classes.
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
 * public static bool Assert(CodeAccessPermission perm, int skipFrames);
 */
ILBool _IL_ClrSecurity_Assert(ILExecThread *_thread,
							  ILObject *perm,
							  ILInt32 skipFrames)
{
	/* TODO */
	return 0;
}

/*
 * public static bool Demand(CodeAccessPermission perm, int skipFrames);
 */
ILBool _IL_ClrSecurity_Demand(ILExecThread *_thread,
							  ILObject *perm,
							  ILInt32 skipFrames)
{
	/* TODO */
	return 0;
}

/*
 * public static void Deny(CodeAccessPermission perm, int skipFrames);
 */
void _IL_ClrSecurity_Deny(ILExecThread *_thread,
						  ILObject *perm,
						  ILInt32 skipFrames)
{
	/* TODO */
}

/*
 * public static void SetPermitOnlyBlock(int skipFrames);
 */
void _IL_ClrSecurity_SetPermitOnlyBlock(ILExecThread *_thread,
										ILInt32 skipFrames)
{
	/* TODO */
}

/*
 * public static void PermitOnly(CodeAccessPermission perm, int skipFrames);
 */
void _IL_ClrSecurity_PermitOnly(ILExecThread *_thread,
								ILObject *perm,
								ILInt32 skipFrames)
{
	/* TODO */
}

/*
 * public static void RevertAll();
 */
void _IL_CodeAccessPermission_RevertAll(ILExecThread *_thread)
{
	/* TODO */
}

/*
 * public static void RevertAssert();
 */
void _IL_CodeAccessPermission_RevertAssert(ILExecThread *_thread)
{
	/* TODO */
}

/*
 * public static void RevertDeny();
 */
void _IL_CodeAccessPermission_RevertDeny(ILExecThread *_thread)
{
	/* TODO */
}

/*
 * public static void RevertPermitOnly();
 */
void _IL_CodeAccessPermission_RevertPermitOnly(ILExecThread *_thread)
{
	/* TODO */
}

/*
 * public bool CanUseFileHandle(IntPtr handle);
 */
ILBool _IL_RuntimeSecurityManager_CanUseFileHandle(ILExecThread *_thread,
												   ILObject *_this,
												   ILNativeInt handle)
{
	/* TODO */
	return 0;
}

/*
 * public bool CanOpenFile(String path, FileMode mode,
 *						   FileAccess access, FileShare share);
 */
ILBool _IL_RuntimeSecurityManager_CanOpenFile(ILExecThread *_thread,
											  ILObject *_this,
											  ILString * path,
											  ILInt32 mode, ILInt32 access,
											  ILInt32 share)
{
	/* TODO */
	return 1;
}

/*
 * private static ISecurityManager GetSecurityManager();
 */
ILObject *_IL_Security_GetSecurityManager(ILExecThread *_thread)
{
	ILObject *mgr = _thread->securityManager;
	if(!mgr)
	{
		mgr = ILExecThreadNew(_thread, "Platform.RuntimeSecurityManager",
							  "(T)V");
		_thread->securityManager = mgr;
	}
	return mgr;
}

/*
 * private static void SetSecurityManager(ISecurityManager mgr);
 */
void _IL_Security_SetSecurityManager(ILExecThread *_thread, ILObject *mgr)
{
	_thread->securityManager = mgr;
}

#ifdef	__cplusplus
};
#endif
