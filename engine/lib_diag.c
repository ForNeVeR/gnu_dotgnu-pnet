/*
 * lib_diag.c - Internalcall methods for "System.Diagnostics".
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

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * System.Diagnostics.Debugger class.
 */

/*
 * private static bool InternalIsAttached();
 */
ILBool _IL_Debugger_InternalIsAttached(ILExecThread *thread)
{
	/* Debugging is not yet supported */
	return 0;
}

/*
 * public static void Break();
 */
void _IL_Debugger_Break(ILExecThread *thread)
{
	/* Debugging is not yet supported */
}

/*
 * public static bool IsLogging();
 */
ILBool _IL_Debugger_IsLogging(ILExecThread *thread)
{
	/* Debugging is not yet supported */
	return 0;
}

/*
 * private static bool InternalLaunch();
 */
ILBool _IL_Debugger_InternalLaunch(ILExecThread *thread)
{
	/* Debugging is not yet supported */
	return 0;
}

/*
 * public static void Log(int level, String category, String message);
 */
void _IL_Debugger_Log(ILExecThread *thread, ILInt32 level,
					  ILString *category, ILString *message)
{
	/* Debugging is not yet supported */
}

/*
 * System.Diagnostics.StackFrame class.
 */

/*
 * private static int InternalGetTotalFrames();
 */
ILInt32 _IL_StackFrame_InternalGetTotalFrames(ILExecThread *thread)
{
	ILInt32 num = 0;
	ILCallFrame *frame = _ILGetCallFrame(thread, 0);
	while(frame != 0)
	{
		++num;
		frame = _ILGetNextCallFrame(thread, frame);
	}
	return num;
}

/*
 * private static RuntimeMethodHandle InternalGetMethod(int skipFrames);
 */
void _IL_StackFrame_InternalGetMethod(ILExecThread *thread,
									  void *result, ILInt32 skipFrames)
{
	ILCallFrame *frame = _ILGetCallFrame(thread, skipFrames);
	if(frame)
	{
		*((ILMethod **)result) = frame->method;
	}
	else
	{
		*((ILMethod **)result) = 0;
	}
}

/*
 * private static int InternalGetILOffset(ILInt32 skipFrames);
 */
ILInt32 _IL_StackFrame_InternalGetILOffset(ILExecThread *thread,
										   ILInt32 skipFrames)
{
	/* Debug symbol information is not yet supported */
	return -1;
}

/*
 * private static int InternalGetNativeOffset(ILInt32 skipFrames);
 */
ILInt32 _IL_StackFrame_InternalGetNativeOffset(ILExecThread *thread,
											   ILInt32 skipFrames)
{
	ILCallFrame *frame = _ILGetCallFrame(thread, skipFrames);
	if(frame)
	{
		return (ILInt32)(frame->pc);
	}
	else
	{
		return -1;
	}
}

/*
 * private static String InternalGetDebugInfo(RuntimeMethodHandle method,
 *											  int offset,
 *											  out int line, out int col);
 */
ILString *_IL_StackFrame_InternalGetDebugInfo
				(ILExecThread *thread, void *method, ILInt32 offset,
				 ILInt32 *line, ILInt32 *col)
{
	/* Debug symbol information is not yet supported */
	*line = 0;
	*col = 0;
	return 0;
}

/*
 * Structure of the "System.Diagnostics.PackedStackFrame" class.
 */
typedef struct
{
	ILMethod	   *method;
	ILInt32			offset;
	ILInt32			nativeOffset;

} PackedStackFrame;

/*
 * internal static PackedStackFrame[] GetExceptionStackTrace();
 */
System_Array *_IL_StackFrame_GetExceptionStackTrace(ILExecThread *thread)
{
	ILInt32 num;
	ILInt32 skipFrames;
	int sawNormal;
	ILCallFrame *frame;
	ILObject *array;
	PackedStackFrame *data;
	ILMethod *method;

	/* Get the number of frames on the stack, and also determine
	   where the exception constructors stop and real code starts */
	num = 0;
	skipFrames = 0;
	sawNormal = 0;
	frame = _ILGetCallFrame(thread, 0);
	while(frame != 0)
	{
		if(!sawNormal)
		{
			method = frame->method;
			if(frame->method)
			{
				if(ILMethod_IsConstructor(frame->method) &&
				   thread->process->exceptionClass &&
				   ILClassInheritsFrom(ILMethod_Owner(frame->method),
				   					   thread->process->exceptionClass))
				{
					++skipFrames;
				}
				else
				{
					sawNormal = 1;
					++num;
				}
			}
			else
			{
				sawNormal = 1;
				++num;
			}
		}
		else
		{
			++num;
		}
		frame = _ILGetNextCallFrame(thread, frame);
	}

	/* Allocate an array for the packed stack data */
	array = ILExecThreadNew(thread, "[vSystem.Diagnostics.PackedStackFrame;",
						    "(Ti)V", (ILVaInt)num);
	if(!array)
	{
		return 0;
	}

	/* Fill the array with the packed stack data */
	data = (PackedStackFrame *)ArrayToBuffer(array);
	frame = _ILGetCallFrame(thread, skipFrames);
	while(frame != 0)
	{
		data->method = frame->method;
		data->offset = -1;	/* Debug symbol information is not yet supported */
		data->nativeOffset = (ILInt32)(frame->pc);
		++data;
		frame = _ILGetNextCallFrame(thread, frame);
	}

	/* Done */
	return (System_Array *)array;
}

#ifdef	__cplusplus
};
#endif
