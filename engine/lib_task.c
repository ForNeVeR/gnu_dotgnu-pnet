/*
 * lib_task.c - Internalcall methods for the "Platform.TaskMethods" class.
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

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static void Exit(int exitCode);
 */
void _IL_TaskMethods_Exit(ILExecThread *thread, ILInt32 exitCode)
{
	exit((int)(exitCode & 0xFF));
}

/*
 * public static void SetExitCode(int exitCode);
 */
void _IL_TaskMethods_SetExitCode(ILExecThread *thread, ILInt32 exitCode)
{
	thread->process->exitStatus = exitCode;
}

/*
 * public static String[] GetCommandLineArgs();
 */
System_Array *_IL_TaskMethods_GetCommandLineArgs(ILExecThread *thread)
{
	/* TODO */
	return 0;
}

/*
 * public static String GetEnvironmentVariable(String name);
 */
ILString *_IL_TaskMethods_GetEnvironmentVariable
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
ILInt32 _IL_TaskMethods_GetEnvironmentCount(ILExecThread *thread)
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
ILString *_IL_TaskMethods_GetEnvironmentKey(ILExecThread *thread, ILInt32 posn)
{
	ILInt32 count = _IL_TaskMethods_GetEnvironmentCount(thread);
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
ILString *_IL_TaskMethods_GetEnvironmentValue(ILExecThread *thread,
											  ILInt32 posn)
{
	ILInt32 count = _IL_TaskMethods_GetEnvironmentCount(thread);
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

#ifdef	__cplusplus
};
#endif
