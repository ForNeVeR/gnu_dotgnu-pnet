/*
 * CSharpCompiler.cs - Implement the "cscompmgd" assembly.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace Microsoft.CSharp
{

using System;
using System.Text;
using System.Collections;

// Level of error that caused a message.
public enum ErrorLevel
{
	None       = 0,
	Warning    = 1,
	Error      = 2,
	FatalError = 3

} // enum ErrorLevel

// Wrapped version of a compiler error message.
public class CompilerError
{
	// Public error information.
	public ErrorLevel ErrorLevel;
	public String ErrorMessage;
	public int ErrorNumber;
	public int SourceColumn;
	public String SourceFile;
	public int SourceLine;

	// Convert the error information into a string.
	public override String ToString()
			{
				StringBuilder builder = new StringBuilder();
				if(SourceFile != null)
				{
					builder.AppendFormat("{0}({1},{2}): ", SourceFile,
										 SourceLine, SourceColumn);
				}
				switch(ErrorLevel)
				{
					case ErrorLevel.Warning:
					{
						builder.Append("warning ");
					}
					break;

					case ErrorLevel.Error:
					{
						builder.Append("error ");
					}
					break;

					case ErrorLevel.FatalError:
					{
						builder.Append("fatal error ");
					}
					break;

					default: break;
				}
				builder.AppendFormat("CS{0:D4}: ", ErrorNumber);
				if(ErrorMessage != null)
				{
					builder.Append(ErrorMessage);
				}
				return builder.ToString();
			}

} // class CompilerError

// Public interface to the C# compiler.
public class Compiler
{

	// Compile a number of source files.
	public static CompilerError[] Compile
				(String[] sourceTexts, String[] sourceTextNames,
				 String target, String[] imports, IDictionary options)
			{
				// TODO: wrap this around "System.CodeDom.Compiler".
				throw new NotImplementedException();
			}

} // class Compiler

} // namespace Microsoft.CSharp
