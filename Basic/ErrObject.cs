/*
 * ErrObject.cs - Implementation of the
 *			"Microsoft.VisualBasic.ErrObject" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace Microsoft.VisualBasic
{

using System;
using System.IO;
using System.ComponentModel;
using System.Runtime.InteropServices;

public sealed class ErrObject
{
	// Internal state.
	private Exception exception;
	private String description;
	private int erl;
	private int helpContext;
	private String helpFile;
	private int number;
	private String source;

	// Constructor.
	internal ErrObject()
			{
				Clear();
			}

	// Clear the error information.
	public void Clear()
			{
				exception = null;
				description = String.Empty;
				erl = 0;
				helpContext = 0;
				helpFile = String.Empty;
				number = -1;
				source = String.Empty;
			}

	// Get the exception within this error object.
	public Exception GetException()
			{
				return exception;
			}

	// Raise a particular error.
	[TODO]
	public void Raise(int Number,
					  [Optional] [DefaultValue(null)] Object Source,
					  [Optional] [DefaultValue(null)] Object Description,
					  [Optional] [DefaultValue(null)] Object HelpFile,
					  [Optional] [DefaultValue(null)] Object HelpContext)
			{
				// TODO
			}

	// Get or set the error's description.
	public String Description
			{
				get
				{
					return description;
				}
				set
				{
					description = value;
				}
			}

	// Get the error's ERL value.
	public int Erl
			{
				get
				{
					return erl;
				}
			}

	// Get or set the error's help context.
	public int HelpContext
			{
				get
				{
					return helpContext;
				}
				set
				{
					helpContext = value;
				}
			}

	// Get or set the error's help file.
	public String HelpFile
			{
				get
				{
					return helpFile;
				}
				set
				{
					helpFile = value;
					if(exception != null)
					{
						exception.HelpLink = helpFile;
					}
				}
			}

	// Get the last-occurring Win32 error code.
	public int LastDllError
			{
				get
				{
					return Marshal.GetLastWin32Error();
				}
			}

	// Get a standard number for an exception.
	private static int GetNumberForException(Exception exception)
			{
				if(exception is OverflowException)
				{
					return 6;
				}
				if(exception is OutOfMemoryException)
				{
					return 7;
				}
				if(exception is IndexOutOfRangeException ||
				   exception is RankException)
				{
					return 9;
				}
				if(exception is DivideByZeroException)
				{
					return 11;
				}
				if(exception is InvalidCastException ||
				   exception is NotSupportedException ||
				   exception is FormatException)
				{
					return 13;
				}
				if(exception is NotFiniteNumberException)
				{
					if(((NotFiniteNumberException)exception).OffendingNumber
							!= 0.0)
					{
						return 6;
					}
					else
					{
						return 11;
					}
				}
				if(exception is DllNotFoundException ||
				   exception is FileNotFoundException)
				{
					return 53;
				}
				if(exception is IOException)
				{
					return 57;
				}
				if(exception is DirectoryNotFoundException)
				{
					return 76;
				}
				if(exception is NullReferenceException)
				{
					return 91;
				}
				if(exception is COMException)
				{
					return ((COMException)exception).ErrorCode;
				}
				if(exception is SEHException)
				{
					return 99;
				}
				if(exception is TypeLoadException)
				{
					return 429;
				}
				if(exception is MissingMemberException)
				{
					return 438;
				}
				if(exception is EntryPointNotFoundException)
				{
					return 453;
				}
				if(exception is InvalidOleVariantTypeException)
				{
					return 458;
				}
				return 5;
			}

	// Get or set the error's number.
	public int Number
			{
				get
				{
					if(number == -1)
					{
						if(exception != null)
						{
							number = GetNumberForException(exception);
						}
						else
						{
							return 0;
						}
					}
					return number;
				}
				set
				{
					number = value;
				}
			}

	// Get or set the error's source.
	public String Source
			{
				get
				{
					return source;
				}
				set
				{
					source = value;
					if(exception != null)
					{
						exception.Source = value;
					}
				}
			}

}; // class ErrObject

}; // namespace Microsoft.VisualBasic
