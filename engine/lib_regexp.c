/*
 * lib_regexp.c - Internalcall methods for "Platform.Regexp"
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
 * 
 * Contributed by Gopal V
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

ILNativeInt _IL_RegexpMethods_CompileInternal(ILExecThread * _thread, 
										ILString * pattern, ILInt32 flags)
{
	char *pat;
	int error;
	ILRegexpHandle result;
	pat=ILStringToAnsi(_thread,pattern);
	if(!pat)
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return 0;
	}
	result=ILRegexpCompile(pat,flags,&(error));
	if(!result)
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return 0;
	}
	if(error)
	{
		pat=ILRegexpError(error,result);
		/* TODO: throw correct exception here */
		ILExecThreadThrowSystem(_thread, "System.ArgumentException",0);
		ILRegexpFree(result);
		return 0;
	}
	return (ILNativeInt)result;
}

ILInt32 _IL_RegexpMethods_ExecInternal(ILExecThread * _thread,
						ILNativeInt compiled, ILString * input, ILInt32 flags)
{
	char *pat;
	ILRegexpMatch *match;
	pat= ILStringToAnsi(_thread,input);
	if(!pat)
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return -1;
	}
	return ILRegexpExec((ILRegexpHandle)compiled,pat,flags,&match);
}

void _IL_RegexpMethods_FreeInternal(ILExecThread * _thread, 
											ILNativeInt compiled)
{
	ILRegexpFree((ILRegexpHandle)compiled);
	return;
}
#ifdef	__cplusplus
};
#endif
