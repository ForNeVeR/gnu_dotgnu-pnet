/*
 * throw.c - External API's for throwing and managing exceptions.
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

#ifdef	__cplusplus
extern	"C" {
#endif

int ILExecThreadHasException(ILExecThread *thread)
{
	return (thread->thrownException != 0);
}

ILObject *ILExecThreadGetException(ILExecThread *thread)
{
	return thread->thrownException;
}

void ILExecThreadClearException(ILExecThread *thread)
{
	thread->thrownException = 0;
}

void ILExecThreadSetException(ILExecThread *thread, ILObject *obj)
{
	thread->thrownException = obj;
}

void ILExecThreadThrowSystem(ILExecThread *thread, const char *typeName,
							 const char *resourceName)
{
	ILMethod *method;
	ILString *resourceString = 0;
	ILObject *object;

	/* Bail out if there already is a pending exception */
	if(ILExecThreadHasException(thread))
	{
		return;
	}

	/* Look for the "System.Object._" method, which allows us to
	   look up localised error messages */
	if(resourceName)
	{
		method = ILExecThreadLookupMethod(thread, "System.Object", "_",
										  "(oSystem.String;)oSystem.String;");
		if(method)
		{
			resourceString = ILStringCreate(thread, resourceName);
			if(resourceString)
			{
				ILExecThreadCall(thread, method, &resourceString,
								 resourceString);
				if(ILExecThreadHasException(thread))
				{
					/* The string lookup threw an exception */
					return;
				}
			}
		}
	}

	/* Create the new exception object and throw it */
	if(resourceString)
	{
		object = ILExecThreadNew(thread, typeName, "(ToSystem.String;)V",
								 resourceString);
	}
	else
	{
		object = ILExecThreadNew(thread, typeName, "(T)V");
	}
	if(!ILExecThreadHasException(thread))
	{
		ILExecThreadSetException(thread, object);
	}
}

void ILExecThreadThrowArgRange(ILExecThread *thread, const char *paramName,
							   const char *resourceName)
{
	ILMethod *method;
	ILString *resourceString = 0;
	ILString *paramString;
	ILObject *object;

	/* Bail out if there already is a pending exception */
	if(ILExecThreadHasException(thread))
	{
		return;
	}

	/* Look for the "System.Object._" method, which allows us to
	   look up localised error messages */
	if(resourceName)
	{
		method = ILExecThreadLookupMethod(thread, "System.Object", "_",
										  "(oSystem.String;)oSystem.String;");
		if(method)
		{
			resourceString = ILStringCreate(thread, resourceName);
			if(resourceString)
			{
				ILExecThreadCall(thread, method, &resourceString,
								 resourceString);
			}
			if(ILExecThreadHasException(thread))
			{
				/* The string create or lookup threw an exception */
				return;
			}
		}
	}

	/* Convert the parameter name into a string */
	paramString = ILStringCreate(thread, paramName);
	if(ILExecThreadHasException(thread))
	{
		/* The string create threw an exception */
		return;
	}

	/* Create the new exception object and throw it */
	if(resourceString)
	{
		object = ILExecThreadNew(thread, "System.ArgumentOutOfRangeException",
								 "(ToSystem.String;oSystem.String;)V",
								 paramString, resourceString);
	}
	else
	{
		object = ILExecThreadNew(thread, "System.ArgumentOutOfRangeException",
								 "(ToSystem.String;)V",
								 paramString);
	}
	if(!ILExecThreadHasException(thread))
	{
		ILExecThreadSetException(thread, object);
	}
}

void ILExecThreadThrowArgNull(ILExecThread *thread, const char *paramName)
{
	ILString *paramString;
	ILObject *object;

	/* Bail out if there already is a pending exception */
	if(ILExecThreadHasException(thread))
	{
		return;
	}

	/* Convert the parameter name into a string */
	paramString = ILStringCreate(thread, paramName);
	if(ILExecThreadHasException(thread))
	{
		/* The string create threw an exception */
		return;
	}

	/* Create the new exception object and throw it */
	object = ILExecThreadNew(thread, "System.ArgumentNullException",
							 "(ToSystem.String;)V", paramString);
	if(!ILExecThreadHasException(thread))
	{
		ILExecThreadSetException(thread, object);
	}
}

void ILExecThreadThrowOutOfMemory(ILExecThread *thread)
{
	if(!ILExecThreadHasException(thread))
	{
		ILExecThreadSetException(thread, thread->process->outOfMemoryObject);
	}
}

#ifdef	__cplusplus
};
#endif
