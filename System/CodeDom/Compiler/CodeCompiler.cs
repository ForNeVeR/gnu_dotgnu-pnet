/*
 * CodeCompiler.cs - Implementation of the
 *		System.CodeDom.Compiler.CodeCompiler class.
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

using System.IO;
using System.Reflection;
using System.Globalization;
using System.Text;

public abstract class CodeCompiler : CodeGenerator, ICodeCompiler
{

	// Constructor.
	protected CodeCompiler() : base() {}

	// Join an array of strings with a separator.
	protected static String JoinStringArray(String[] sa, String separator)
			{
				if(sa == null || sa.Length == 0)
				{
					return String.Empty;
				}
				StringBuilder sb = new StringBuilder();
				int posn;
				for(posn = 0; posn < sa.Length; ++posn)
				{
					if(posn > 0)
					{
						sb.Append(separator);
					}
					sb.Append('"');
					sb.Append(sa[posn]);
					sb.Append('"');
				}
				return sb.ToString();
			}

	// Get the name of the compiler.
	protected abstract String CompilerName { get; }

	// Get the file extension to use for source files.
	protected abstract String FileExtension { get; }

	// Convert compiler parameters into compiler arguments.
	protected abstract String CmdArgsFromParameters
				(CompilerParameters options);

	// Compile a CodeDom compile unit.
	protected virtual CompilerResults FromDom
				(CompilerParameters options, CodeCompileUnit e)
			{
				CodeCompileUnit[] list = new CodeCompileUnit [1];
				list[0] = e;
				return FromDomBatch(options, list);
			}

	// Compile an array of CodeDom compile units.
	protected virtual CompilerResults FromDomBatch
				(CompilerParameters options, CodeCompileUnit[] ea)
			{
				// Write all of the CodeDom units to temporary files.
				String[] tempFiles = new String [ea.Length];
				int src;
				Stream stream;
				StreamWriter writer;
				for(src = 0; src < ea.Length; ++src)
				{
					tempFiles[src] = options.TempFiles.AddExtension
							(src + FileExtension);
					stream = new FileStream(tempFiles[src], FileMode.Create,
											FileAccess.Write, FileShare.Read);
					try
					{
						writer = new StreamWriter(stream, Encoding.UTF8);
						((ICodeGenerator)this).GenerateCodeFromCompileUnit
								(ea[src], writer, Options);
						writer.Flush();
						writer.Close();
					}
					finally
					{
						stream.Close();
					}
				}
				
				// Compile the temporary files.
				return FromFileBatch(options, tempFiles);
			}

	// Compile a file.
	protected virtual CompilerResults FromFile
				(CompilerParameters options, String fileName)
			{
				// Verify that the file can be read, throwing an
				// exception to the caller if it cannot.
				(new FileStream(fileName, FileMode.Open, FileAccess.Read))
						.Close();

				// Pass the filename to "FromFileBatch".
				String[] list = new String [1];
				list[0] = fileName;
				return FromFileBatch(options, list);
			}

	// Compile an array of files.
	protected virtual CompilerResults FromFileBatch
				(CompilerParameters options, String[] fileNames)
			{
				// TODO
				return null;
			}

	// Compile a source string.
	protected virtual CompilerResults FromSource
				(CompilerParameters options, String source)
			{
				String[] list = new String [1];
				list[0] = source;
				return FromSourceBatch(options, list);
			}

	// Compile an array of source strings.
	protected virtual CompilerResults FromSourceBatch
				(CompilerParameters options, String[] sources)
			{
				// Write all of the sources to temporary files.
				String[] tempFiles = new String [sources.Length];
				int src;
				Stream stream;
				StreamWriter writer;
				for(src = 0; src < sources.Length; ++src)
				{
					tempFiles[src] = options.TempFiles.AddExtension
							(src + FileExtension);
					stream = new FileStream(tempFiles[src], FileMode.Create,
											FileAccess.Write, FileShare.Read);
					try
					{
						writer = new StreamWriter(stream, Encoding.UTF8);
						writer.Write(sources[src]);
						writer.Flush();
						writer.Close();
					}
					finally
					{
						stream.Close();
					}
				}
				
				// Compile the temporary files.
				return FromFileBatch(options, tempFiles);
			}

	// Get the response file command arguments.
	protected virtual String GetResponseFileCmdArgs
				(CompilerParameters options, String cmdArgs)
			{
				// Get a temporary file to use for the response file.
				String responseFile = options.TempFiles.AddExtension("cmdline");

				// Write the arguments to the response file.
				Stream stream = new FileStream(responseFile, FileMode.Create,
											   FileAccess.Write,
											   FileShare.Read);
				try
				{
					StreamWriter writer = new StreamWriter
							(stream, Encoding.UTF8);
					writer.Write(cmdArgs);
					writer.Flush();
					writer.Close();
				}
				finally
				{
					stream.Close();
				}

				// Build the new command-line containing the response file.
				return "@\"" + responseFile + "\"";
			}

	// Process an output line from the compiler.
	protected abstract void ProcessCompilerOutputLine
				(CompilerResults results, String line);

	// Compile an assembly from a CodeDom compile unit.
	CompilerResults ICodeCompiler.CompileAssemblyFromDom
			(CompilerParameters options, CodeCompileUnit compilationUnit)
			{
				try
				{
					return FromDom(options, compilationUnit);
				}
				finally
				{
					options.TempFiles.Delete();
				}
			}

	// Compile an assembly from an array of CodeDom compile units.
	CompilerResults ICodeCompiler.CompileAssemblyFromDomBatch
			(CompilerParameters options, CodeCompileUnit[] compilationUnits)
			{
				try
				{
					return FromDomBatch(options, compilationUnits);
				}
				finally
				{
					options.TempFiles.Delete();
				}
			}

	// Compile an assembly from the contents of a source file.
	CompilerResults ICodeCompiler.CompileAssemblyFromFile
			(CompilerParameters options, String fileName)
			{
				try
				{
					return FromFile(options, fileName);
				}
				finally
				{
					options.TempFiles.Delete();
				}
			}

	// Compile an assembly from the contents of an array of source files.
	CompilerResults ICodeCompiler.CompileAssemblyFromFileBatch
			(CompilerParameters options, String[] fileNames)
			{
				try
				{
					return FromFileBatch(options, fileNames);
				}
				finally
				{
					options.TempFiles.Delete();
				}
			}

	// Compile an assembly from the contents of a source string.
	CompilerResults ICodeCompiler.CompileAssemblyFromSource
			(CompilerParameters options, String source)
			{
				try
				{
					return FromSource(options, source);
				}
				finally
				{
					options.TempFiles.Delete();
				}
			}

	// Compile an assembly from the contents of an array of source strings.
	CompilerResults ICodeCompiler.CompileAssemblyFromSourceBatch
			(CompilerParameters options, String[] sources)
			{
				try
				{
					return FromSourceBatch(options, sources);
				}
				finally
				{
					options.TempFiles.Delete();
				}
			}

}; // class CodeCompiler

#endif // !ECMA_COMPAT

}; // namespace System.CodeDom.Compiler