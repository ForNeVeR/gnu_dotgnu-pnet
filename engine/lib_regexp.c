/*
 * lib_regexp.c - Internalcall methods for "Platform.Regexp"
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * 
 * Contributions by Gopal V, Rhys Weatherley
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
#include "il_regex.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILNativeInt _IL_RegexpMethods_CompileInternal(ILExecThread * _thread, 
										ILString * pattern, ILInt32 flags)
{
	char *pat;
	int error;
	regex_t *result;
	pat=ILStringToAnsi(_thread,pattern);
	if(!pat)
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return 0;
	}
	result=ILCalloc(1,sizeof(regex_t));
	if(!result)
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return 0;
	}
	error=IL_regcomp(result,pat,flags);
	if(error)
	{
		/* TODO: use regerror to add a descriptive message to the exception */
		ILExecThreadThrowSystem(_thread, "System.ArgumentException",0);
		ILFree(result);
		return 0;
	}
	return (ILNativeInt)result;
}

ILInt32 _IL_RegexpMethods_ExecInternal(ILExecThread * _thread,
									   ILNativeInt compiled,
									   ILString * input, ILInt32 flags)
{
	char *pat;
	pat= ILStringToAnsi(_thread,input);
	if(!pat)
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return -1;
	}
	return IL_regexec((regex_t*)compiled,pat,flags,0,0);
}

void _IL_RegexpMethods_FreeInternal(ILExecThread * _thread, 
									ILNativeInt compiled)
{
	if(compiled)
	{
		IL_regfree((regex_t*)compiled);
		ILFree((void*)compiled);
	}
}
#ifdef	__cplusplus
};
#endif
