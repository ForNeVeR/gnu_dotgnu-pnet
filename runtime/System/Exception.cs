/*
 * Exception.cs - Implementation of the "System.Exception" class.
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

namespace System
{

using System.Reflection;
using System.Diagnostics;

/*

Note on Exception messages:
--------------------------

This class library takes a slightly different approach for determining
what the default message text should be for internal exceptions.  Other
implementations override "Message" and "ToString()", and then back-patch
the "message" field in the base class if it is null.  This is a pain to
implement, especially for "ToString()" which must include the stack trace
in the result, amongst other things.

Instead, we provide two "protected internal" properties that only classes
in this library can access.  "MessageDefault" provides a default message
if "message" is null.  "MessageExtra" provides extra information to be
inserted into the "ToString()" result just after the message and before
the stack trace.

This design is cleaner to implement throughout the library.  Because the
extra properties are "protected internal", they will not pollute the
name space of applications that didn't expect them to be present.

*/

public class Exception
{

	// Private members.
	private String message;
	private Exception innerException;
	private String helpLink;
	private String source;
	private PackedStackFrame[] stackTrace;

	// Constructors.
	public Exception()
		{
			message = null;
			innerException = null;
			stackTrace = StackFrame.GetExceptionStackTrace();
		}
	public Exception(String msg)
		{
			message = msg;
			innerException = null;
			stackTrace = StackFrame.GetExceptionStackTrace();
		}
	public Exception(String msg, Exception inner)
		{
			message = msg;
			innerException = inner;
			stackTrace = StackFrame.GetExceptionStackTrace();
		}

	// Private constructor that is used for subclasses that
	// don't want stack traces.  e.g. OutOfMemoryException.
	internal Exception(String msg, Exception inner, bool wantTrace)
		{
			message = msg;
			innerException = inner;
			if(wantTrace)
			{
				stackTrace = StackFrame.GetExceptionStackTrace();
			}
		}

	// Get the base exception upon which this exception is based.
	public virtual Exception GetBaseException()
		{
			Exception result = this;
			Exception inner;
			while((inner = result.InnerException) != null)
			{
				result = inner;
			}
			return result;
		}

	// Set the help link for this exception.
	public virtual Exception SetHelpLink(String help)
		{
			helpLink = help;
			return this;
		}

	// Convert the exception into a string.
	public override String ToString()
		{
			String className = GetType().ToString();
			String result;
			String temp;
			String message = Message;
			if(message != null && message.Length > 0)
			{
				result = className + ": " + message;
			}
			else
			{
				result = className;
			}
			temp = MessageExtra;
			if(temp != null)
			{
				result = result + Environment.NewLine + temp;
			}
			if(innerException != null)
			{
				result = result + " ---> " + innerException.ToString();
			}
			temp = StackTrace;
			if(temp != null)
			{
				result = result + Environment.NewLine + temp;
			}
			return result;
		}

	// Properties.
	public virtual String HelpLink
		{
			get
			{
				return helpLink;
			}
		}
	public virtual Exception InnerException
		{
			get
			{
				return innerException;
			}
		}
	public virtual String Message
		{
			get
			{
				if(message != null)
				{
					return message;
				}
				else if((message = MessageDefault) != null)
				{
					return message;
				}
				else
				{
					return String.Format
						(_("Exception_WasThrown"), GetType().ToString());
				}
			}
		}
	public virtual String Source
		{
			get
			{
				return source;
			}
			set
			{
				source = value;
			}
		}
	public virtual String StackTrace
		{
			get
			{
				return stackTrace.ToString();
			}
		}
	public virtual MethodBase TargetSite
		{
			get
			{
				if(stackTrace.Length > 0)
				{
					return MethodBase.GetMethodFromHandle
								(stackTrace[0].method);
				}
				else
				{
					return null;
				}
			}
		}

	// Get the packed stack trace information from this exception.
	internal PackedStackFrame[] GetPackedStackTrace()
		{
			return stackTrace;
		}

	// Get the extra data to be inserted into the "ToString" representation.
	protected internal virtual String MessageExtra
		{
			get
			{
				return null;
			}
		}

	// Get the default message to use if "message" was initialized to null.
	protected internal virtual String MessageDefault
		{
			get
			{
				return null;
			}
		}

}; // class Exception

}; // namespace System
