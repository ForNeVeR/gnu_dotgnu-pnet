/*
 * CompilerError.cs - Implementation of the
 *		System.CodeDom.Compiler.CompilerError class.
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

namespace System.CodeDom.Compiler
{

#if !ECMA_COMPAT

public class CompilerError
{
	// Internal state.
	private String fileName;
	private int line;
	private int column;
	private String errorNumber;
	private String errorText;
	private bool isWarning;

	// Constructors.
	public CompilerError()
			{
				this.fileName = String.Empty;
				this.line = 0;
				this.column = 0;
				this.errorNumber = String.Empty;
				this.errorText = String.Empty;
				this.isWarning = false;
			}
	public CompilerError(String fileName, int line, int column,
						 String errorNumber, String errorText)
			{
				this.fileName = fileName;
				this.line = line;
				this.column = column;
				this.errorNumber = errorNumber;
				this.errorText = errorText;
				this.isWarning = false;
			}

	// Properties.
	public int Column
			{
				get
				{
					return column;
				}
				set
				{
					column = value;
				}
			}
	public String ErrorNumber
			{
				get
				{
					return errorNumber;
				}
				set
				{
					errorNumber = value;
				}
			}
	public String ErrorText
			{
				get
				{
					return errorText;
				}
				set
				{
					errorText = value;
				}
			}
	public String FileName
			{
				get
				{
					return fileName;
				}
				set
				{
					fileName = value;
				}
			}
	public bool IsWarning
			{
				get
				{
					return isWarning;
				}
				set
				{
					isWarning = value;
				}
			}
	public int Line
			{
				get
				{
					return line;
				}
				set
				{
					line = value;
				}
			}

	// Convert this error into a string.
	public override String ToString()
			{
				// TODO
				return null;
			}

}; // class CompilerError

#endif // !ECMA_COMPAT

}; // namespace System.CodeDom.Compiler
