/*
 * lib_platform.c - Internalcall methods for misc "Platform" classes.
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
#include "lib_defs.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static UnicodeCategory GetUnicodeCategory(char ch);
 */
static ILInt32 Platform_SysCharInfo_GetUnicodeCategory(ILExecThread *thread,
													   ILUInt16 ch)
{
	return (ILInt32)(ILGetUnicodeCategory((unsigned)ch));
}

/*
 * public static double GetNumericValue(char ch);
 */
static ILDouble Platform_SysCharInfo_GetNumericValue(ILExecThread *thread,
													 ILUInt16 ch)
{
	return (ILDouble)(ILGetUnicodeValue((unsigned)ch));
}

/*
 * public static String GetNewLine();
 */
static ILString *Platform_SysCharInfo_GetNewLine(ILExecThread *thread)
{
#ifdef IL_NATIVE_WIN32
	return ILStringCreate(thread, "\r\n");
#else
	return ILStringCreate(thread, "\n");
#endif
}

/*
 * Method table for the "Platform.SysCharInfo" class.
 */
IL_METHOD_BEGIN(_ILPlatformSysCharInfoMethods)
	IL_METHOD("GetUnicodeCategory", "(c)vSystem.Globalization.UnicodeCategory;",
					Platform_SysCharInfo_GetUnicodeCategory)
	IL_METHOD("GetNumericValue", "(c)d",
					Platform_SysCharInfo_GetNumericValue)
	IL_METHOD("GetNewLine", "()oSystem.String;",
					Platform_SysCharInfo_GetNewLine)
IL_METHOD_END

/*
 * public static long GetCurrentTime();
 *
 * This returns the time since 12:00 Jan 1, 0001 in tenths of a microsecond.
 */
static ILInt64 Platform_TimeMethods_GetCurrentTime(ILExecThread *thread)
{
	ILCurrTime timeValue;
	ILGetCurrTime(&timeValue);
	timeValue.secs -= (ILInt64)(ILGetTimeZoneAdjust());
	return (timeValue.secs * (ILInt64)10000000) +
				(ILInt64)(timeValue.nsecs / (ILUInt32)100);
}

/*
 * public static long GetCurrentUtcTime();
 *
 * This returns UTC time since 12:00 Jan 1, 0001 in tenths of a microsecond.
 */
static ILInt64 Platform_TimeMethods_GetCurrentUtcTime(ILExecThread *thread)
{
	ILCurrTime timeValue;
	ILGetCurrTime(&timeValue);
	return (timeValue.secs * (ILInt64)10000000) +
				(ILInt64)(timeValue.nsecs / (ILUInt32)100);
}

/*
 * public static int GetTimeZoneAdjust();
 *
 * This returns the number of seconds West of GMT for the local timezone.
 */
static ILInt32 Platform_TimeMethods_GetTimeZoneAdjust(ILExecThread *thread)
{
	return ILGetTimeZoneAdjust();
}

/*
 * public static int GetUpTime();
 *
 * Note: this is supposed to return the number of milliseconds
 * since the system was rebooted.  Reboot times aren't usually
 * available on Unix-style systems, so we may have to use the
 * number of milliseconds since the engine was started instead.
 */
static ILInt32 Platform_TimeMethods_GetUpTime(ILExecThread *thread)
{
	ILCurrTime timeValue;
	if(!ILGetSinceRebootTime(&timeValue))
	{
		ILGetCurrTime(&timeValue);
		if(timeValue.nsecs < thread->process->startTime.nsecs)
		{
			timeValue.nsecs =
				timeValue.nsecs - thread->process->startTime.nsecs +
								1000000000;
			timeValue.secs =
				timeValue.secs - thread->process->startTime.secs + 1;
		}
		else
		{
			timeValue.nsecs =
				timeValue.nsecs - thread->process->startTime.nsecs;
			timeValue.secs =
				timeValue.secs - thread->process->startTime.secs;
		}
	}
	return (ILInt32)((timeValue.secs * (ILInt64)1000) +
				(ILInt64)(timeValue.nsecs / (ILUInt32)1000));
}

/*
 * Method table for the "Platform.TimeMethods" class.
 */
IL_METHOD_BEGIN(_ILPlatformTimeMethods)
	IL_METHOD("GetCurrentTime", "()l", Platform_TimeMethods_GetCurrentTime)
	IL_METHOD("GetCurrentUtcTime", "()l",
			  Platform_TimeMethods_GetCurrentUtcTime)
	IL_METHOD("GetTimeZoneAdjust", "()i",
			  Platform_TimeMethods_GetTimeZoneAdjust)
	IL_METHOD("GetUpTime", "()i", Platform_TimeMethods_GetUpTime)
IL_METHOD_END

/*
 * public static void Exit(int exitCode);
 */
static void Platform_TaskMethods_Exit(ILExecThread *thread, ILInt32 exitCode)
{
	exit((int)(exitCode & 0xFF));
}

/*
 * public static void SetExitCode(int exitCode);
 */
static void Platform_TaskMethods_SetExitCode(ILExecThread *thread,
											 ILInt32 exitCode)
{
	thread->process->exitStatus = exitCode;
}

/*
 * public static String[] GetCommandLineArgs();
 */
static ILObject *Platform_TaskMethods_GetCommandLineArgs(ILExecThread *thread)
{
	/* TODO */
	return 0;
}

/*
 * public static String GetEnvironmentVariable(String name);
 */
static ILString *Platform_TaskMethods_GetEnvironmentVariable
			(ILExecThread *thread, ILString *name)
{
	char *nameAnsi = ILStringToAnsi(thread, name);
	char *env;
	if(nameAnsi)
	{
		env = getenv(nameAnsi);
		if(env)
		{
			return ILStringCreate(thread, env);
		}
	}
	return 0;
}

/*
 * Import the "environ" variable, so that we can walk the environment.
 */
extern char **environ;

/*
 * public static int GetEnvironmentCount();
 */
static ILInt32 Platform_TaskMethods_GetEnvironmentCount(ILExecThread *thread)
{
	ILInt32 count = 0;
	char **env = environ;
	while(env && *env != 0)
	{
		++count;
		++env;
	}
	return count;
}

/*
 * public static String GetEnvironmentKey(int posn);
 */
static ILString *Platform_TaskMethods_GetEnvironmentKey
			(ILExecThread *thread, ILInt32 posn)
{
	ILInt32 count = Platform_TaskMethods_GetEnvironmentCount(thread);
	char *env;
	int len;
	if(posn >= 0 && posn < count)
	{
		env = environ[posn];
		len = 0;
		while(env[len] != '\0' && env[len] != '=')
		{
			++len;
		}
		return ILStringCreateLen(thread, env, len);
	}
	else
	{
		return 0;
	}
}

/*
 * public static String GetEnvironmentValue(int posn);
 */
static ILString *Platform_TaskMethods_GetEnvironmentValue
			(ILExecThread *thread, ILInt32 posn)
{
	ILInt32 count = Platform_TaskMethods_GetEnvironmentCount(thread);
	char *env;
	int len;
	if(posn >= 0 && posn < count)
	{
		env = environ[posn];
		len = 0;
		while(env[len] != '\0' && env[len] != '=')
		{
			++len;
		}
		if(env[len] == '=')
		{
			++len;
		}
		return ILStringCreate(thread, env + len);
	}
	else
	{
		return 0;
	}
}

/*
 * Method table for the "Platform.TaskMethods" class.
 */
IL_METHOD_BEGIN(_ILPlatformTaskMethods)
	IL_METHOD("Exit", "(i)V", Platform_TaskMethods_Exit)
	IL_METHOD("SetExitCode", "(i)V", Platform_TaskMethods_SetExitCode)
	IL_METHOD("GetCommandLineArgs", "()[oSystem.String;",
			  Platform_TaskMethods_GetCommandLineArgs)
	IL_METHOD("GetEnvironmentVariable", "(oSystem.String;)oSystem.String;",
			  Platform_TaskMethods_GetEnvironmentVariable)
	IL_METHOD("GetEnvironmentCount", "()i",
			  Platform_TaskMethods_GetEnvironmentCount)
	IL_METHOD("GetEnvironmentKey", "(i)oSystem.String;",
			  Platform_TaskMethods_GetEnvironmentKey)
	IL_METHOD("GetEnvironmentValue", "(i)oSystem.String;",
			  Platform_TaskMethods_GetEnvironmentValue)
IL_METHOD_END

/*
 * Structure of the "Platform.PathInfo" class.
 */
typedef struct
{
	ILUInt16	dirSeparator;
	ILUInt16	altDirSeparator;
	ILUInt16	volumeSeparator;
	ILUInt16	pathSeparator;
	ILObject   *invalidPathChars;

} Platform_PathInfo;

/*
 * public static PathInfo GetPathInfo();
 */
static void Platform_DirMethods_GetPathInfo(ILExecThread *thread,
											void *result)
{
	Platform_PathInfo *info = (Platform_PathInfo *)result;
#if defined(_WIN32) || defined(WIN32)
	info->dirSeparator = '\\';
	info->altDirSeparator = '/';
	info->volumeSeparator = ':';
	info->pathSeparator = ';';
	info->invalidPathChars = 0;
#else
	info->dirSeparator = '/';
	info->altDirSeparator = 0;
	info->volumeSeparator = 0;
	info->pathSeparator = ':';
	info->invalidPathChars = 0;
#endif
}

/*
 * public static String GetSystemDirectory();
 */
static ILString *Platform_DirMethods_GetSystemDirectory(ILExecThread *thread)
{
	/* We don't support system directories on this platform */
	return 0;
}

/*
 * Method table for the "Platform.DirMethods" class.
 */
IL_METHOD_BEGIN(_ILPlatformDirMethods)
	IL_METHOD("GetPathInfo", "()vPlatform.PathInfo;",
			  Platform_DirMethods_GetPathInfo)
	IL_METHOD("GetSystemDirectory", "()oSystem.String;",
			  Platform_DirMethods_GetSystemDirectory)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
