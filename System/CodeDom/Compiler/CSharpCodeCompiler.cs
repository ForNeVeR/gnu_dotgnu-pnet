/*
 * CSharpCodeCompiler.cs - Implementation of the
 *		System.CodeDom.Compiler.CSharpCodeCompiler class.
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
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

internal class CSharpCodeCompiler : CodeCompiler
{

	// List of reserved words in C#.
	private static readonly String[] reservedWords = {
				"abstract", "__arglist", "as", "base", "bool",
				"break", "__builtin", "byte", "case", "catch",
				"char", "checked", "class", "const", "continue",
				"decimal", "default", "delegate", "do", "double",
				"else", "enum", "event", "explicit", "extern",
				"false", "finally", "fixed", "float", "for",
				"foreach", "goto", "if", "implicit", "in", "int",
				"interface", "internal", "is", "lock", "long",
				"__long_double", "__makeref", "__module", "namespace",
				"new", "null", "object", "operator", "out", "override",
				"params", "private", "protected", "public", "readonly",
				"ref", "__reftype", "__refvalue", "return", "sbyte",
				"sealed", "short", "sizeof", "stackalloc", "static",
				"string", "struct", "switch", "this", "throw", "true",
				"try", "typeof", "uint", "ulong", "unchecked", "unsafe",
				"ushort", "using", "virtual", "void", "volatile", "while"
			};

	// Internal state.
	private bool outputForInit;

	// Constructor.
	public CSharpCodeCompiler() : base()
			{
				outputForInit = false;
			}

	// Get the name of the compiler.
	protected override String CompilerName
			{
				get
				{
					// Use the Portable.NET C# compiler.
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
					return ".cs";
				}
			}

	// Add an argument to an argument array.
	private static void AddArgument(ref String[] args, String arg)
			{
				String[] newArgs = new String [args.Length + 1];
				Array.Copy(args, newArgs, args.Length);
				newArgs[args.Length] = arg;
			}

	// Build a list of arguments from an option string.  The arguments
	// are assumed to use the "csc" syntax, which we will convert into
	// the "cscc" syntax within "CmdArgsFromParameters".
	private static String[] ArgsFromOptions(String options)
			{
				String[] args = new String [0];
				// TODO
				return args;
			}

	// Determine if a string looks like a "csc" option.
	private static bool IsOption(String opt, String name)
			{
				return (String.Compare(opt, name, true,
									   CultureInfo.InvariantCulture) == 0);
			}

	// Add a list of "csc" defines to a "cscc" command-line.
	private static void AddDefines(ref String[] args, String defines)
			{
				if(defines != String.Empty)
				{
					String[] defs = defines.Split(',', ';');
					foreach(String def in defs)
					{
						AddArgument(ref args, "-D" + def);
					}
				}
			}

	// Convert compiler parameters into compiler arguments (common code).
	internal static String CmdArgsFromParameters
				(CompilerParameters options, String language)
			{
				String[] args = new String [0];
				int posn, posn2;
				AddArgument(ref args, "-x");
				AddArgument(ref args, language);
				if(options.OutputAssembly != null)
				{
					AddArgument(ref args, "-o");
					AddArgument(ref args, options.OutputAssembly);
				}
				if(options.IncludeDebugInformation)
				{
					AddArgument(ref args, "-g");
				}
				if(!(options.GenerateExecutable))
				{
					AddArgument(ref args, "-shared");
				}
				if(options.TreatWarningsAsErrors)
				{
					AddArgument(ref args, "-Werror");
				}
				if(options.WarningLevel >= 0)
				{
					AddArgument(ref args, "-Wall");
				}
				if(options.MainClass != null)
				{
					AddArgument(ref args, "-e");
					AddArgument(ref args, options.MainClass);
				}
				foreach(String _refAssem in options.ReferencedAssemblies)
				{
					// Strip ".dll" from the end of the assembly name.
					String refAssem = _refAssem;
					if(refAssem.Length > 4 &&
					   String.Compare(refAssem, refAssem.Length - 4,
					   				  ".dll", 0, 4, true,
									  CultureInfo.InvariantCulture) == 0)
					{
						refAssem = refAssem.Substring(0, refAssem.Length - 4);
					}

					// Split the assembly into its path and base name.
					posn = refAssem.LastIndexOf('/');
					posn2 = refAssem.LastIndexOf('\\');
					if(posn2 > posn)
					{
						posn = posn2;
					}
					if(posn != -1)
					{
						// Add "-L" and "-l" options to the command-line.
						AddArgument(ref args,
							"-L" + refAssem.Substring(0, posn));
						AddArgument(ref args,
							"-l" + refAssem.Substring(posn + 1));
					}
					else
					{
						// Add just a "-l" option to the command-line.
						AddArgument(ref args, "-l" + refAssem);
					}
				}
				if(options.Win32Resource != null)
				{
					AddArgument(ref args,
						"-fresources=" + options.Win32Resource);
				}
				if(options.CompilerOptions != null)
				{
					String[] cscArgs = ArgsFromOptions(options.CompilerOptions);
					foreach(String opt in cscArgs)
					{
						if(IsOption(opt, "/optimize") ||
						   IsOption(opt, "/optimize+") ||
						   IsOption(opt, "/o") ||
						   IsOption(opt, "/o+"))
						{
							AddArgument(ref args, "-O2");
						}
						else if(IsOption(opt, "/checked") ||
						        IsOption(opt, "/checked+"))
						{
							AddArgument(ref args, "-fchecked");
						}
						else if(IsOption(opt, "/checked-"))
						{
							AddArgument(ref args, "-funchecked");
						}
						else if(IsOption(opt, "/unsafe") ||
						        IsOption(opt, "/unsafe+"))
						{
							AddArgument(ref args, "-funsafe");
						}
						else if(IsOption(opt, "/nostdlib") ||
						        IsOption(opt, "/nostdlib+"))
						{
							AddArgument(ref args, "-fnostdlib");
						}
						else if(String.Compare(opt, 0, "/define:", 0, 8, true,
											   CultureInfo.InvariantCulture)
									== 0)
						{
							AddDefines(ref args, opt.Substring(8));
						}
						else if(String.Compare(opt, 0, "/d:", 0, 3, true,
											   CultureInfo.InvariantCulture)
									== 0)
						{
							AddDefines(ref args, opt.Substring(3));
						}
					}
				}
				return JoinStringArray(args, " ");
			}

	// Convert compiler parameters into compiler arguments.
	protected override String CmdArgsFromParameters
				(CompilerParameters options)
			{
				return CmdArgsFromParameters(options, "csharp");
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
					return "null";
				}
			}

	// Create an escaped identifier if "value" is a language keyword.
	protected override String CreateEscapedIdentifier(String value)
			{
				if(Array.IndexOf(reservedWords, value) != -1)
				{
					return "@" + value;
				}
				else
				{
					return value;
				}
			}

	// Create a valid identifier if "value" is a language keyword.
	protected override String CreateValidIdentifier(String value)
			{
				if(Array.IndexOf(reservedWords, value) != -1)
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
				switch(type)
				{
					case "System.Void":		type = "void"; break;
					case "System.Boolean":	type = "bool"; break;
					case "System.Char":		type = "char"; break;
					case "System.Byte":		type = "byte"; break;
					case "System.SByte":	type = "sbyte"; break;
					case "System.Int16":	type = "short"; break;
					case "System.UInt16":	type = "ushort"; break;
					case "System.Int32":	type = "int"; break;
					case "System.UInt32":	type = "uint"; break;
					case "System.Int64":	type = "long"; break;
					case "System.UInt64":	type = "ulong"; break;
					case "System.Single":	type = "float"; break;
					case "System.Double":	type = "double"; break;
					case "System.Decimal":	type = "decimal"; break;
					case "System.String":	type = "string"; break;
					default:				break;
				}
				return type;
			}

	// Generate various expression categories.
	protected override void GenerateArgumentReferenceExpression
				(CodeArgumentReferenceExpression e)
			{
				OutputIdentifier(e.ParameterName);
			}
	protected override void GenerateArrayCreateExpression
				(CodeArrayCreateExpression e)
			{
				Output.Write("new ");
				if(e.Initializers.Count == 0)
				{
					Output.Write(NormalizeTypeName(e.CreateType.BaseType));
					Output.Write("[");
					if(e.SizeExpression != null)
					{
						GenerateExpression(e.SizeExpression);
					}
					else
					{
						Output.Write(e.Size);
					}
					Output.Write("]");
				}
				else
				{
					OutputType(e.CreateType);
					if(e.CreateType.ArrayRank == 0)
					{
						Output.Write("[]");
					}
					Output.WriteLine(" {");
					Indent += 1;
					OutputExpressionList(e.Initializers, true);
					Indent -= 1;
					Output.Write("}");
				}
			}
	protected override void GenerateArrayIndexerExpression
				(CodeArrayIndexerExpression e)
			{
				GenerateExpression(e.TargetObject);
				Output.Write("[");
				OutputExpressionList(e.Indices);
				Output.Write("]");
			}
	protected override void GenerateBaseReferenceExpression
				(CodeBaseReferenceExpression e)
			{
				Output.Write("base");
			}
	protected override void GenerateCastExpression
				(CodeCastExpression e)
			{
				// Heavily bracket the cast to prevent the possibility
				// of ambiguity issues within the compiler.  See the
				// Portable.NET "cs_grammar.y" file for a description of
				// the possible conflicts that may arise without brackets.
				Output.Write("((");
				OutputType(e.TargetType);
				Output.Write(")(");
				GenerateExpression(e.Expression);
				Output.Write("))");
			}
	protected override void GenerateDelegateCreateExpression
				(CodeDelegateCreateExpression e)
			{
				Output.Write("new ");
				OutputType(e.DelegateType);
				Output.Write("(");
				if(e.TargetObject != null)
				{
					GenerateExpression(e.TargetObject);
					Output.Write(".");
				}
				OutputIdentifier(e.MethodName);
				Output.Write(")");
			}
	protected override void GenerateDelegateInvokeExpression
				(CodeDelegateInvokeExpression e)
			{
				if(e.TargetObject != null)
				{
					GenerateExpression(e.TargetObject);
				}
				Output.Write("(");
				OutputExpressionList(e.Parameters);
				Output.Write(")");
			}
	protected override void GenerateEventReferenceExpression
				(CodeEventReferenceExpression e)
			{
				if(e.TargetObject != null)
				{
					GenerateExpression(e.TargetObject);
					Output.Write(".");
				}
				OutputIdentifier(e.EventName);
			}
	protected override void GenerateFieldReferenceExpression
				(CodeFieldReferenceExpression e)
			{
				if(e.TargetObject != null)
				{
					GenerateExpression(e.TargetObject);
					Output.Write(".");
				}
				OutputIdentifier(e.FieldName);
			}
	protected override void GenerateIndexerExpression
				(CodeIndexerExpression e)
			{
				GenerateExpression(e.TargetObject);
				Output.Write("[");
				OutputExpressionList(e.Indices);
				Output.Write("]");
			}
	protected override void GenerateMethodInvokeExpression
				(CodeMethodInvokeExpression e)
			{
				GenerateMethodReferenceExpression(e.Method);
				Output.Write("(");
				OutputExpressionList(e.Parameters);
				Output.Write(")");
			}
	protected override void GenerateMethodReferenceExpression
				(CodeMethodReferenceExpression e)
			{
				if(e.TargetObject != null)
				{
					GenerateExpression(e.TargetObject);
					Output.Write(".");
				}
				OutputIdentifier(e.MethodName);
			}
	protected override void GenerateObjectCreateExpression
				(CodeObjectCreateExpression e)
			{
				Output.Write("new ");
				OutputType(e.CreateType);
				Output.Write("(");
				OutputExpressionList(e.Parameters);
				Output.Write(")");
			}
	protected override void GenerateParameterDeclarationExpression
				(CodeParameterDeclarationExpression e)
			{
				// TODO
			}
	protected override void GeneratePropertyReferenceExpression
				(CodePropertyReferenceExpression e)
			{
				if(e.TargetObject != null)
				{
					GenerateExpression(e.TargetObject);
					Output.Write(".");
				}
				OutputIdentifier(e.PropertyName);
			}
	protected override void GeneratePropertySetValueReferenceExpression
				(CodePropertySetValueReferenceExpression e)
			{
				Output.Write("value");
			}
	protected override void GenerateSnippetExpression
				(CodeSnippetExpression e)
			{
				Output.Write(e.Value);
			}
	protected override void GenerateThisReferenceExpression
				(CodeThisReferenceExpression e)
			{
				Output.Write("this");
			}
	protected override void GenerateVariableReferenceExpression
				(CodeVariableReferenceExpression e)
			{
				OutputIdentifier(e.VariableName);
			}

	// Start a new indented block.
	private void StartBlock()
			{
				Output.WriteLine("{");
				Indent += 1;
			}

	// End an indented block.
	private void EndBlock()
			{
				Indent -= 1;
				Output.WriteLine("}");
			}

	// Generate various statement categories.
	protected override void GenerateAssignStatement
				(CodeAssignStatement e)
			{
				GenerateExpression(e.Left);
				Output.Write(" = ");
				GenerateExpression(e.Right);
				if(!outputForInit)
				{
					Output.WriteLine(";");
				}
			}
	protected override void GenerateAttachEventStatement
				(CodeAttachEventStatement e)
			{
				GenerateExpression(e.Event);
				Output.Write(" += ");
				GenerateExpression(e.Listener);
				Output.WriteLine(";");
			}
	protected override void GenerateConditionStatement
				(CodeConditionStatement e)
			{
				Output.Write("if (");
				GenerateExpression(e.Condition);
				Output.WriteLine(")");
				StartBlock();
				GenerateStatements(e.TrueStatements);
				EndBlock();
				CodeStatementCollection stmts = e.FalseStatements;
				if(stmts.Count > 0 || Options.ElseOnClosing)
				{
					Output.WriteLine("else");
					StartBlock();
					GenerateStatements(stmts);
					EndBlock();
				}
			}
	protected override void GenerateExpressionStatement
				(CodeExpressionStatement e)
			{
				GenerateExpression(e.Expression);
				if(!outputForInit)
				{
					Output.WriteLine(";");
				}
			}
	protected override void GenerateGotoStatement
				(CodeGotoStatement e)
			{
				Output.Write("goto ");
				Output.Write(e.Label);
				Output.WriteLine(";");
			}
	protected override void GenerateIterationStatement
				(CodeIterationStatement e)
			{
				if(e.InitStatement == null &&
				   e.TestExpression != null &&
				   e.IncrementStatement == null)
				{
					// Special case - output a "while" statement.
					Output.Write("while (");
					GenerateExpression(e.TestExpression);
					Output.WriteLine(")");
					StartBlock();
					GenerateStatements(e.Statements);
					EndBlock();
				}
				else
				{
					// Output a "for" statement.
					Output.Write("for (");
					outputForInit = true;
					if(e.InitStatement != null)
					{
						GenerateStatement(e.InitStatement);
					}
					Output.Write("; ");
					if(e.TestExpression != null)
					{
						GenerateExpression(e.TestExpression);
					}
					Output.Write("; ");
					if(e.IncrementStatement != null)
					{
						GenerateStatement(e.IncrementStatement);
					}
					outputForInit = false;
					Output.WriteLine(")");
					StartBlock();
					GenerateStatements(e.Statements);
					EndBlock();
				}
			}
	protected override void GenerateLabeledStatement
				(CodeLabeledStatement e)
			{
				Indent -= 1;
				Output.Write(e.Label);
				Output.WriteLine(":");
				Indent += 1;
				GenerateStatement(e.Statement);
			}
	protected override void GenerateMethodReturnStatement
				(CodeMethodReturnStatement e)
			{
				if(e.Expression != null)
				{
					Output.Write("return ");
					GenerateExpression(e.Expression);
					Output.WriteLine(";");
				}
				else
				{
					Output.WriteLine("return;");
				}
			}
	protected override void GenerateRemoveEventStatement
				(CodeRemoveEventStatement e)
			{
				GenerateExpression(e.Event);
				Output.Write(" -= ");
				GenerateExpression(e.Listener);
				Output.WriteLine(";");
			}
	protected override void GenerateThrowExceptionStatement
				(CodeThrowExceptionStatement e)
			{
				if(e.ToThrow != null)
				{
					Output.Write("throw ");
					GenerateExpression(e.ToThrow);
					Output.WriteLine(";");
				}
				else
				{
					Output.WriteLine("throw;");
				}
			}
	protected override void GenerateTryCatchFinallyStatement
				(CodeTryCatchFinallyStatement e)
			{
				Output.WriteLine("try");
				StartBlock();
				GenerateStatements(e.TryStatements);
				EndBlock();
				CodeCatchClauseCollection clauses = e.CatchClauses;
				if(clauses.Count > 0)
				{
					foreach(CodeCatchClause clause in clauses)
					{
						if(clause.CatchExceptionType != null)
						{
							Output.Write("catch (");
							OutputType(clause.CatchExceptionType);
							if(clause.LocalName != null)
							{
								Output.Write(" ");
								OutputIdentifier(clause.LocalName);
							}
							Output.WriteLine(")");
						}
						else
						{
							Output.WriteLine("catch");
						}
						StartBlock();
						GenerateStatements(clause.Statements);
						EndBlock();
					}
				}
				CodeStatementCollection fin = e.FinallyStatements;
				if(fin.Count > 0)
				{
					Output.WriteLine("finally");
					StartBlock();
					GenerateStatements(fin);
					EndBlock();
				}
			}
	protected override void GenerateVariableDeclarationStatement
				(CodeVariableDeclarationStatement e)
			{
				OutputTypeNamePair(e.Type, e.Name);
				if(e.InitExpression != null)
				{
					Output.Write(" = ");
					GenerateExpression(e.InitExpression);
				}
				if(!outputForInit)
				{
					Output.WriteLine(";");
				}
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
				Output.WriteLine();
				Output.WriteLine("#line {0} \"{1}\"",
								 e.LineNumber, e.FileName);
			}
	protected override void GenerateLinePragmaEnd(CodeLinePragma e)
			{
				Output.WriteLine();
				Output.WriteLine("#line default");
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

}; // class CSharpCodeCompiler

#endif // !ECMA_COMPAT

}; // namespace System.CodeDom.Compiler
