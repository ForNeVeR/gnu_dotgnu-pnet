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
using Microsoft.VisualBasic.CompilerServices;

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

	// Set the exception within this error object.
	internal void SetException(Exception e)
			{
				exception = e;
			}
	internal void SetException(Exception e, int erl)
			{
				exception = e;
				this.erl = erl;
			}

	// Raise a particular error.
	public void Raise(int Number,
					  [Optional] [DefaultValue(null)] Object Source,
					  [Optional] [DefaultValue(null)] Object Description,
					  [Optional] [DefaultValue(null)] Object HelpFile,
					  [Optional] [DefaultValue(null)] Object HelpContext)
			{
				if(Number == 0)
				{
					throw new ArgumentException
						(S._("VB_InvalidErrorNumber"), "Number");
				}
				this.Number = Number;
				this.Source = StringType.FromObject(Source);
				this.Description = StringType.FromObject(Description);
				this.HelpFile = StringType.FromObject(HelpFile);
				this.HelpContext = IntegerType.FromObject(HelpContext);
				this.exception = CreateExceptionFromNumber
						(Number, this.Description);
			#if !ECMA_COMPAT
				this.exception.Source = this.Source;
				this.exception.HelpLink = this.HelpFile;
			#endif
				throw this.exception;
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
				#if !ECMA_COMPAT
					if(exception != null)
					{
						exception.HelpLink = helpFile;
					}
				#endif
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
	internal static int GetNumberForException(Exception exception)
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
				   exception is RankException ||
				   exception is ArrayTypeMismatchException)
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
				if(exception is StackOverflowException)
				{
					return 28;
				}
			#if !ECMA_COMPAT
				if(exception is DllNotFoundException)
				{
					return 53;
				}
			#endif
				if(exception is FileNotFoundException)
				{
					return 53;
				}
				if(exception is IOException)
				{
					return 57;
				}
				if(exception is EndOfStreamException)
				{
					return 62;
				}
				if(exception is DirectoryNotFoundException)
				{
					return 76;
				}
				if(exception is NullReferenceException)
				{
					return 91;
				}
#if CONFIG_COM_INTEROP
				if(exception is COMException)
				{
					return ((COMException)exception).ErrorCode;
				}
				if(exception is SEHException)
				{
					return 99;
				}
				if(exception is InvalidOleVariantTypeException)
				{
					return 458;
				}
#endif
				if(exception is TypeLoadException)
				{
					return 429;
				}
				if(exception is MissingFieldException)
				{
					return 422;
				}
				if(exception is MissingMemberException)
				{
					return 438;
				}
				if(exception is EntryPointNotFoundException)
				{
					return 453;
				}
				return 5;
			}

	// Convert a number into an exception.
	internal static Exception CreateExceptionFromNumber
				(int number, String message)
			{
				switch(number)
				{
					case 3: case 20: case 94: case 100:
						return new InvalidOperationException(message);

					case 5: case 446: case 448: case 449:
						return new ArgumentException(message);

					case 6:
						return new OverflowException(message);

					case 7: case 14:
						return new OutOfMemoryException(message);

					case 9:
						return new IndexOutOfRangeException(message);

					case 11:
						return new DivideByZeroException(message);

					case 13:
						return new InvalidCastException(message);

					case 28:
						return new StackOverflowException(message);

					case 48: case 429:
						return new TypeLoadException(message);

					case 52: case 54: case 55: case 57: case 58:
					case 59: case 61: case 63: case 67: case 68:
					case 70: case 71: case 74: case 75:
						return new IOException(message);

					case 53: case 432:
						return new FileNotFoundException(message);

					case 62:
						return new EndOfStreamException(message);

					case 76:
						return new DirectoryNotFoundException(message);

					case 91:
						return new NullReferenceException(message);

#if CONFIG_COM_INTEROP
					case 99:
						return new SEHException(message);

					case 458:
						return new InvalidOleVariantTypeException(message);
#endif

					case 422:
						return new MissingFieldException(message);

					case 438:
						return new MissingMemberException(message);

					case 453:
						return new EntryPointNotFoundException(message);

					default:
						return new Exception(message);
				}
			}

	// Convert a HRESULT value into an error number.
	[TODO]
	internal static int HResultToNumber(int hr)
			{
				// TODO
				return hr;
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
				#if !ECMA_COMPAT
					if(exception != null)
					{
						exception.Source = value;
					}
				#endif
				}
			}

}; // class ErrObject

}; // namespace Microsoft.VisualBasic
