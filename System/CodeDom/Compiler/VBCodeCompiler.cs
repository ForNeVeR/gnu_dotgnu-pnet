/*
 * VBCodeCompiler.cs - Implementation of the
 *		System.CodeDom.Compiler.VBCodeCompiler class.
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

namespace System.CodeDom.Compiler
{

#if CONFIG_CODEDOM

using System.IO;
using System.Reflection;
using System.Globalization;

internal class VBCodeCompiler : CodeCompiler
{

	// List of reserved words in VB.
	private static readonly String[] reservedWords = {
			"addhandler", "addressof", "alias", "and", "andalso",
			"ansi", "as", "assembly", "auto", "boolean", "byref", "byte",
			"byval", "call", "case", "catch", "cbool", "cbyte", "cchar",
			"cdate", "cdec", "cdbl", "char", "cint", "class", "clng",
			"cobj", "const", "cshort", "csng", "cstr", "ctype", "date",
			"decimal", "declare", "default", "delegate", "dim", "directcast",
			"do", "double", "each", "else", "elseif", "end", "enum",
			"erase", "error", "event", "exit", "false", "finally", "for",
			"friend", "function", "get", "gettype", "gosub", "goto",
			"handles", "if", "implements", "imports", "in", "inherits",
			"integer", "interface", "is", "let", "lib", "like", "long",
			"loop", "me", "mod", "module", "mustinherit", "mustoverride",
			"mybase", "myclass", "namespace", "new", "next", "not", "nothing",
			"notinheritable", "notoverridable", "object", "on", "option",
			"optional", "or", "orelse", "overloads", "overridable",
			"overrides", "paramarray", "preserve", "private", "property",
			"protected", "public", "raiseevent", "readonly", "redim",
			"removehandler", "resume", "return", "select", "set",
			"shadows", "shared", "short", "single", "static", "step", "stop",
			"string", "structure", "sub", "synclock", "then", "throw",
			"to", "true", "try", "typeof", "unicode", "until", "variant",
			"when", "while", "with", "withevents", "writeonly", "xor"
		};

	// Constructor.
	public VBCodeCompiler() {}

	// Get the name of the compiler.
	protected override String CompilerName
			{
				get
				{
					// Use the Portable.NET VB compiler.
					String cscc = Environment.GetEnvironmentVariable("CSCC");
					if(cscc != null)
					{
						return cscc;
					}
					else
					{
						return "cscc";
					}
				}
			}

	// Get the file extension to use for source files.
	protected override String FileExtension
			{
				get
				{
					return ".vb";
				}
			}

	// Convert compiler parameters into compiler arguments.
	protected override String CmdArgsFromParameters
				(CompilerParameters options)
			{
				return CSharpCodeCompiler.CmdArgsFromParameters(options, "vb");
			}

	// Process an output line from the compiler.
	protected override void ProcessCompilerOutputLine
				(CompilerResults results, String line)
			{
				// TODO
			}

	// Get the token for "null".
	protected override String NullToken
			{
				get
				{
					return "Nothing";
				}
			}

	// Determine if a string is a reserved word.
	private static bool IsReservedWord(String value)
			{
				if(value != null)
				{
					value = value.ToLower(CultureInfo.InvariantCulture);
					return (Array.IndexOf(reservedWords, value) != -1);
				}
				else
				{
					return false;
				}
			}

	// Create an escaped identifier if "value" is a language keyword.
	protected override String CreateEscapedIdentifier(String value)
			{
				if(IsReservedWord(value))
				{
					return "[" + value + "]";
				}
				else
				{
					return value;
				}
			}

	// Create a valid identifier if "value" is a language keyword.
	protected override String CreateValidIdentifier(String value)
			{
				if(IsReservedWord(value))
				{
					return "_" + value;
				}
				else
				{
					return value;
				}
			}

	// Normalize a type name to its keyword form.
	private String NormalizeTypeName(String type)
			{
				// TODO: VB arrays use "()", not "[]".
				switch(type)
				{
					case "System.Boolean":	type = "Boolean"; break;
					case "System.Char":		type = "Char"; break;
					case "System.Byte":		type = "Byte"; break;
					case "System.Int16":	type = "Short"; break;
					case "System.Int32":	type = "Integer"; break;
					case "System.Int64":	type = "Long"; break;
					case "System.Single":	type = "Single"; break;
					case "System.Double":	type = "Double"; break;
					case "System.Decimal":	type = "Decimal"; break;
					case "System.String":	type = "String"; break;
					case "System.DateTime":	type = "Date"; break;
					case "System.Object":	type = "Object"; break;
					default:				break;
				}
				return type;
			}

	// Generate various expression categories.
	protected override void GenerateArgumentReferenceExpression
				(CodeArgumentReferenceExpression e)
			{
				// TODO
			}
	protected override void GenerateArrayCreateExpression
				(CodeArrayCreateExpression e)
			{
				// TODO
			}
	protected override void GenerateArrayIndexerExpression
				(CodeArrayIndexerExpression e)
			{
				// TODO
			}
	protected override void GenerateBaseReferenceExpression
				(CodeBaseReferenceExpression e)
			{
				Output.Write("MyBase");
			}
	protected override void GenerateCastExpression
				(CodeCastExpression e)
			{
				// TODO
			}
	protected override void GenerateDelegateCreateExpression
				(CodeDelegateCreateExpression e)
			{
				// TODO
			}
	protected override void GenerateDelegateInvokeExpression
				(CodeDelegateInvokeExpression e)
			{
				// TODO
			}
	protected override void GenerateEventReferenceExpression
				(CodeEventReferenceExpression e)
			{
				// TODO
			}
	protected override void GenerateFieldReferenceExpression
				(CodeFieldReferenceExpression e)
			{
				// TODO
			}
	protected override void GenerateIndexerExpression
				(CodeIndexerExpression e)
			{
				// TODO
			}
	protected override void GenerateMethodInvokeExpression
				(CodeMethodInvokeExpression e)
			{
				// TODO
			}
	protected override void GenerateMethodReferenceExpression
				(CodeMethodReferenceExpression e)
			{
				// TODO
			}
	protected override void GenerateObjectCreateExpression
				(CodeObjectCreateExpression e)
			{
				// TODO
			}
	protected override void GenerateParameterDeclarationExpression
				(CodeParameterDeclarationExpression e)
			{
				// TODO
			}
	protected override void GeneratePropertyReferenceExpression
				(CodePropertyReferenceExpression e)
			{
				// TODO
			}
	protected override void GeneratePropertySetValueReferenceExpression
				(CodePropertySetValueReferenceExpression e)
			{
				// TODO
			}
	protected override void GenerateSnippetExpression
				(CodeSnippetExpression e)
			{
				// TODO
			}
	protected override void GenerateThisReferenceExpression
				(CodeThisReferenceExpression e)
			{
				Output.Write("Me");
			}
	protected override void GenerateVariableReferenceExpression
				(CodeVariableReferenceExpression e)
			{
				// TODO
			}

	// Generate various statement categories.
	protected override void GenerateAssignStatement
				(CodeAssignStatement e)
			{
				// TODO
			}
	protected override void GenerateAttachEventStatement
				(CodeAttachEventStatement e)
			{
				// TODO
			}
	protected override void GenerateConditionStatement
				(CodeConditionStatement e)
			{
				// TODO
			}
	protected override void GenerateExpressionStatement
				(CodeExpressionStatement e)
			{
				// TODO
			}
	protected override void GenerateGotoStatement
				(CodeGotoStatement e)
			{
				Output.Write("Goto ");
				Output.Write(e.Label);
				Output.WriteLine();
			}
	protected override void GenerateIterationStatement
				(CodeIterationStatement e)
			{
				// TODO
			}
	protected override void GenerateLabeledStatement
				(CodeLabeledStatement e)
			{
				// TODO
			}
	protected override void GenerateMethodReturnStatement
				(CodeMethodReturnStatement e)
			{
				// TODO
			}
	protected override void GenerateRemoveEventStatement
				(CodeRemoveEventStatement e)
			{
				// TODO
			}
	protected override void GenerateThrowExceptionStatement
				(CodeThrowExceptionStatement e)
			{
				// TODO
			}
	protected override void GenerateTryCatchFinallyStatement
				(CodeTryCatchFinallyStatement e)
			{
				// TODO
			}
	protected override void GenerateVariableDeclarationStatement
				(CodeVariableDeclarationStatement e)
			{
				// TODO
			}

	// Generate various declaration categories.
	protected override void GenerateAttributeDeclarationsStart
				(CodeAttributeDeclarationCollection attributes)
			{
				// TODO
			}
	protected override void GenerateAttributeDeclarationsEnd
				(CodeAttributeDeclarationCollection attributes)
			{
				// TODO
			}
	protected override void GenerateConstructor
				(CodeConstructor e, CodeTypeDeclaration c)
			{
				// TODO
			}
	protected override void GenerateEntryPointMethod
				(CodeEntryPointMethod e, CodeTypeDeclaration c)
			{
				// TODO
			}
	protected override void GenerateEvent
				(CodeMemberEvent e, CodeTypeDeclaration c)
			{
				// TODO
			}
	protected override void GenerateField(CodeMemberField e)
			{
				// TODO
			}
	protected override void GenerateMethod
				(CodeMemberMethod e, CodeTypeDeclaration c)
			{
				// TODO
			}
	protected override void GenerateProperty
				(CodeMemberProperty e, CodeTypeDeclaration c)
			{
				// TODO
			}
	protected override void GenerateNamespaceStart(CodeNamespace e)
			{
				// TODO
			}
	protected override void GenerateNamespaceEnd(CodeNamespace e)
			{
				// TODO
			}
	protected override void GenerateNamespaceImport(CodeNamespaceImport e)
			{
				// TODO
			}
	protected override void GenerateSnippetMember
				(CodeSnippetTypeMember e)
			{
				// TODO
			}
	protected override void GenerateTypeConstructor
				(CodeTypeConstructor e)
			{
				// TODO
			}
	protected override void GenerateTypeStart(CodeTypeDeclaration e)
			{
				// TODO
			}
	protected override void GenerateTypeEnd(CodeTypeDeclaration e)
			{
				// TODO
			}

	// Generate various misc categories.
	protected override void GenerateComment(CodeComment e)
			{
				// TODO
			}
	protected override void GenerateLinePragmaStart(CodeLinePragma e)
			{
				// TODO
			}
	protected override void GenerateLinePragmaEnd(CodeLinePragma e)
			{
				// TODO
			}
	protected override String GetTypeOutput(CodeTypeReference value)
			{
				// TODO
				return null;
			}

	// Determine if "value" is a valid identifier.
	protected override bool IsValidIdentifier(String value)
			{
				// TODO
				return true;
			}

	// Output a type.
	protected override void OutputType(CodeTypeReference typeRef)
			{
				Output.Write(GetTypeOutput(typeRef));
			}

	// Quote a snippet string.
	protected override String QuoteSnippetString(String value)
			{
				// TODO
				return value;
			}

	// Determine if this code generator supports a particular
	// set of generator options.
	protected override bool Supports(GeneratorSupport supports)
			{
				return ((supports & (GeneratorSupport)0x001FFFFF) == supports);
			}

}; // class VBCodeCompiler

#endif // CONFIG_CODEDOM

}; // namespace System.CodeDom.Compiler
