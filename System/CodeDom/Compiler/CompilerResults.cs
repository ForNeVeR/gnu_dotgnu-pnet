/*
 * CompilerResults.cs - Implementation of the
 *		System.CodeDom.Compiler.CompilerResults class.
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

using System.Collections.Specialized;
using System.Reflection;

public class CompilerResults
{
	// Internal state.
	private TempFileCollection tempFiles;
	private Assembly compiledAssembly;
	private CompilerErrorCollection errors;
	private int nativeCompilerReturnValue;
	private StringCollection output;
	private String pathToAssembly;

	// Constructor.
	public CompilerResults(TempFileCollection tempFiles)
			{
				this.tempFiles = tempFiles;
			}

	// Properties.
	public Assembly CompiledAssembly
			{
				get
				{
					return compiledAssembly;
				}
				set
				{
					compiledAssembly = value;
				}
			}
	public CompilerErrorCollection Errors
			{
				get
				{
					if(errors == null)
					{
						errors = new CompilerErrorCollection();
					}
					return errors;
				}
			}
	public int NativeCompilerReturnValue
			{
				get
				{
					return nativeCompilerReturnValue;
				}
				set
				{
					nativeCompilerReturnValue = value;
				}
			}
	public StringCollection Output
			{
				get
				{
					if(output == null)
					{
						output = new StringCollection();
					}
					return output;
				}
			}
	public String PathToAssembly
			{
				get
				{
					return pathToAssembly;
				}
				set
				{
					pathToAssembly = value;
				}
			}
	public TempFileCollection TempFiles
			{
				get
				{
					return tempFiles;
				}
				set
				{
					tempFiles = value;
				}
			}

}; // class CompilerResults

#endif // !ECMA_COMPAT

}; // namespace System.CodeDom.Compiler