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

public abstract class CodeCompiler : CodeGenerator, ICodeCompiler
{

	// Constructor.
	protected CodeCompiler() : base() {}

	// Join an array of strings with a separator.
	protected static String JoinStringArray(String[] sa, String separator)
			{
				// TODO
				return null;
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
				// TODO
				return null;
			}

	// Compile an array of CodeDom compile units.
	protected virtual CompilerResults FromDomBatch
				(CompilerParameters options, CodeCompileUnit[] ea)
			{
				// TODO
				return null;
			}

	// Compile a file.
	protected virtual CompilerResults FromFile
				(CompilerParameters options, String fileName)
			{
				// TODO
				return null;
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
				// TODO
				return null;
			}

	// Compile an array of source strings.
	protected virtual CompilerResults FromSourceBatch
				(CompilerParameters options, String[] sources)
			{
				// TODO
				return null;
			}

	// Get the response file command arguments.
	protected virtual String GetResponseFileCmdArgs
				(CompilerParameters options, String cmdArgs)
			{
				// TODO
				return null;
			}

	// Process an output line from the compiler.
	protected abstract void ProcessCompilerOutputLine
				(CompilerResults results, String line);

	// Compile an assembly from a CodeDom compile unit.
	CompilerResults ICodeCompiler.CompileAssemblyFromDom
			(CompilerParameters options, CodeCompileUnit compilationUnit)
			{
				// TODO
				return null;
			}

	// Compile an assembly from an array of CodeDom compile units.
	CompilerResults ICodeCompiler.CompileAssemblyFromDomBatch
			(CompilerParameters options, CodeCompileUnit[] compilationUnits)
			{
				// TODO
				return null;
			}

	// Compile an assembly from the contents of a source file.
	CompilerResults ICodeCompiler.CompileAssemblyFromFile
			(CompilerParameters options, String fileName)
			{
				// TODO
				return null;
			}

	// Compile an assembly from the contents of an array of source files.
	CompilerResults ICodeCompiler.CompileAssemblyFromFileBatch
			(CompilerParameters options, String[] fileNames)
			{
				// TODO
				return null;
			}

	// Compile an assembly from the contents of a source string.
	CompilerResults ICodeCompiler.CompileAssemblyFromSource
			(CompilerParameters options, String source)
			{
				// TODO
				return null;
			}

	// Compile an assembly from the contents of an array of source strings.
	CompilerResults ICodeCompiler.CompileAssemblyFromSourceBatch
			(CompilerParameters options, String[] sources)
			{
				// TODO
				return null;
			}

}; // class CodeCompiler

#endif // !ECMA_COMPAT

}; // namespace System.CodeDom.Compiler
