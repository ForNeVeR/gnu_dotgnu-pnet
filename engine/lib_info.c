/*
 * lib_info.c - Internalcall methods for the "Platform.InfoMethods" class.
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
#include "il_utils.h"
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static String GetRuntimeVersion();
 */
ILString *_IL_InfoMethods_GetRuntimeVersion(ILExecThread *thread)
{
	return ILStringCreate(thread, VERSION);
}

/*
 * public static String GetNetBIOSMachineName();
 */
ILString *_IL_InfoMethods_GetNetBIOSMachineName(ILExecThread *thread)
{
	/* TODO */
	return 0;
}

/*
 * public static PlatformID GetPlatformID();
 */
ILInt32 _IL_InfoMethods_GetPlatformID(ILExecThread *thread)
{
#ifdef IL_WIN32_PLATFORM
	return 2;			/* PlatformID.Win32NT */
#else
	return 128;			/* PlatformID.Unix */
#endif
}

/*
 * public static String GetUserDomainName();
 */
ILString *_IL_InfoMethods_GetUserDomainName(ILExecThread *thread)
{
	/* TODO */
	return 0;
}

/*
 * public static bool IsUserInteractive();
 */
ILBool _IL_InfoMethods_IsUserInteractive(ILExecThread * _thread)
{
#if HAVE_ISATTY
	return (isatty(0) && isatty(1));
#else
	return 0;
#endif
}

/*
 * public static String GetUserName();
 */
ILString *_IL_InfoMethods_GetUserName(ILExecThread *thread)
{
#if !defined(__palmos__)
	if(!ILImageIsSecure(_ILClrCallerImage(thread)))
	{
		/* We don't trust the caller, so don't tell them who the user is */
		return ILStringCreate(thread, "nobody");
	}
	else
	{
		char *env;
#if HAVE_GETLOGIN
		char *login = getlogin();
		if(login && *login != '\0')
		{
			return ILStringCreate(thread, login);
		}
#endif
		env = getenv("USER");
		if(env && *env != '\0')
		{
			return ILStringCreate(thread, env);
		}
		return ILStringCreate(thread, "nobody");
	}
#else
	return ILStringCreate(thread, "nobody");
#endif
}

/*
 * public static long GetWorkingSet();
 */
ILInt64 _IL_InfoMethods_GetWorkingSet(ILExecThread *thread)
{
	/* There is no reliable way to determine the working set */
	return 0;
}

/*
 * public static String GetSpecialFolder(Environment.SpecialFolder f);
 */
ILString *_IL_InfoMethods_GetSpecialFolder(ILExecThread * _thread, ILInt32 f)
{
	/* Special folders are security-sensitive, so don't return them */
	return 0;
}

#ifdef	__cplusplus
};
#endif
