/*
 * Debug.cs - Implementation of "System.Diagnostics.Debug" 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  FSF.
 * 
 * Authors : Autogenerated using csdoc2stub 
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

namespace System.Diagnostics
{

using System;

#define	DEBUG

public sealed class Debug
{
	// Internal state.
	private static bool autoFlush = true;
	private static int indentLevel = 0;
	private static int indentSize = 4;

	// Cannot construct instances of this class.
	private Debug() {}

	// Assert a condition, exiting the application if it is false.
	[Conditional("DEBUG")]
	public static void Assert(bool condition)
			{
				if(!condition)
				{
					Fail(String.Empty);
				}
			}
	[Conditional("DEBUG")]
	public static void Assert(bool condition, String message)
			{
				if(!condition)
				{
					Fail(message);
				}
			}
	[Conditional("DEBUG")]
	public static void Assert
				(bool condition, String message, String detailMessage)
			{
				if(!condition)
				{
					Fail(message, detailMessage);
				}
			}

	// Close the trace output stream.
	[Conditional("DEBUG")]
	public static void Close()
			{
				// Nothing to do here.
			}

	// Report a debugging failure and exit the application.
	[Conditional("DEBUG")]
	public static void Fail(String message)
			{
				Fail(message, String.Empty);
			}
	[Conditional("DEBUG")]
	public static void Fail(String message, String detailMessage)
			{
				// Print the failure message details.
				if(message != null && message != String.Empty)
				{
					WriteLine(message);
				}
				if(detailMessage != null && detailMessage != String.Empty)
				{
					WriteLine(detailMessage);
				}

				// Print the stack trace for the failure.
				Write((new StackTrace()).ToString());
				Flush();

				// Abort the application.
				Environment.Exit(1);
			}

	// Flush the trace output stream.
	[Conditional("DEBUG")]
	public static void Flush()
			{
				Console.Error.Flush();
			}

	// Go in one indent level within the trace output stream.
	[Conditional("DEBUG")]
	public static void Indent()
			{
				IndentLevel += 1;
			}

	// Go out one indent level within the trace output stream.
	[Conditional("DEBUG")]
	public static void Unindent()
			{
				IndentLevel -= 1;
			}

	// Write data to the trace output stream.
	[Conditional("DEBUG")]
	public static void Write(Object value)
			{
				if(value != null)
				{
					Write(value.ToString());
				}
			}
	[TODO]
	[Conditional("DEBUG")]
	public static void Write(String message)
			{
				Console.Error.Write(message);
			}
	[Conditional("DEBUG")]
	public static void Write(Object value, String category)
			{
				if(category == null)
				{
					Write(value);
				}
				else
				{
					Write((value != null ? value.ToString() : String.Empty),
						  category);
				}
			}
	[Conditional("DEBUG")]
	public static void Write(String message, String category)
			{
				if(category == null)
				{
					Write(message);
				}
				else
				{
					Write(category + ": " +
						  (message != null ? message : String.Empty));
				}
			}

	// Write a debugging message if a condition is true.
	[Conditional("DEBUG")]
	public static void WriteIf(bool condition, Object value)
			{
				if(condition)
				{
					Write(value);
				}
			}
	[Conditional("DEBUG")]
	public static void WriteIf(bool condition, String message)
			{
				if(condition)
				{
					Write(message);
				}
			}
	[Conditional("DEBUG")]
	public static void WriteIf(bool condition, Object value, String category)
			{
				if(condition)
				{
					Write(value, category);
				}
			}
	[Conditional("DEBUG")]
	public static void WriteIf(bool condition, String message, String category)
			{
				if(condition)
				{
					Write(message, category);
				}
			}

	// Write a message to the debug output stream followed by newline.
	[Conditional("DEBUG")]
	public static void WriteLine(Object value)
			{
				if(value != null)
				{
					WriteLine(value.ToString());
				}
			}
	[TODO]
	[Conditional("DEBUG")]
	public static void WriteLine(String message)
			{
				Console.Error.WriteLine(message);
			}
	[Conditional("DEBUG")]
	public static void WriteLine(Object value, String category)
			{
				if(category == null)
				{
					WriteLine(value);
				}
				else
				{
					WriteLine((value != null ?
									value.ToString() : String.Empty),
						       category);
				}
			}
	[Conditional("DEBUG")]
	public static void WriteLine(String message, String category)
			{
				if(category == null)
				{
					WriteLine(message);
				}
				else
				{
					WriteLine(category + ": " +
						      (message != null ? message : String.Empty));
				}
			}

	// Write a debugging message if a condition is true, followed by newline.
	[Conditional("DEBUG")]
	public static void WriteLineIf(bool condition, Object value)
			{
				if(condition)
				{
					WriteLine(value);
				}
			}
	[Conditional("DEBUG")]
	public static void WriteLineIf(bool condition, String message)
			{
				if(condition)
				{
					WriteLine(message);
				}
			}
	[Conditional("DEBUG")]
	public static void WriteLineIf
					(bool condition, Object value, String category)
			{
				if(condition)
				{
					WriteLine(value, category);
				}
			}
	[Conditional("DEBUG")]
	public static void WriteLineIf
					(bool condition, String message, String category)
			{
				if(condition)
				{
					WriteLine(message, category);
				}
			}

	// Get or set the message auto-flush state.
	public static bool AutoFlush 
			{ 
				get
				{
					return autoFlush;
				} 
				set
				{
					autoFlush = value;
				} 
			}

	// Change the indent level for output messages.
	public static int IndentLevel 
			{ 
				get
				{
					return indentLevel;
				}
				set
				{
					if(value < 0)
					{
						value = 0;
					}
					indentLevel = value;
				}
			}

	// Set the size of output message indenting.
	public static int IndentSize 
			{
				get
				{
					return indentSize;
				}
				set
				{
					if(value < 0)
					{
						value = 0;
					}
					indentSize = value;
				}
			}

}; // class Debug

}; // namespace System.Diagnostics