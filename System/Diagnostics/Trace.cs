/*
 * Trace.cs - Implementation of "System.Diagnostics.Trace" 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * 
 * Contributed by Gopal.V
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

using System;

namespace System.Diagnostics
{
	public sealed class Trace
	{
		[TODO]
		public static void Assert(bool condition)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Assert(bool condition, String message)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Assert(bool condition, String message, String detailMessage)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Close()
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Fail(String message)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Fail(String message, String detailMessage)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Flush()
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Indent()
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Unindent()
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Write(Object value)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Write(String message)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Write(Object value, String category)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void Write(String message, String category)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteIf(bool condition, Object value)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteIf(bool condition, String message)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteIf(bool condition, Object value, String category)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteIf(bool condition, String message, String category)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteLine(Object value)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteLine(String message)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteLine(Object value, String category)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteLine(String message, String category)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteLineIf(bool condition, Object value)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteLineIf(bool condition, String message)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteLineIf(bool condition, Object value, String category)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static void WriteLineIf(bool condition, String message, String category)
		{
			 /*TODO: Implement this */
		}

		[TODO]
		public static bool AutoFlush 
		{
 			get
			{
				return false;
				/*TODO: Implement this */
			}
 			set
			{
				/*TODO: Implement this */
			}
 		}

		[TODO]
		public static int IndentLevel 
		{
 			get
			{
				return 0;
				/*TODO: Implement this */
			}
 			set
			{
				/*TODO: Implement this */
			}
 		}

		[TODO]
		public static int IndentSize 
		{
 			get
			{
				return 4;
				/*TODO: Implement this */
			}
 			set
			{
				/*TODO: Implement this */
			}
 		}
#if false
		[TODO]
		public static TraceListenerCollection Listeners 
		{
 			get
			{
				/*TODO: Implement this */
			}
 		}
#endif

	}
}//namespace
