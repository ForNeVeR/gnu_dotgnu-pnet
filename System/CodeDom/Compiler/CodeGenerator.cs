/*
 * CodeGenerator.cs - Implementation of the
 *		System.CodeDom.Compiler.CodeGenerator class.
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

public abstract class CodeGenerator : ICodeGenerator
{
	// Internal state.
	private CodeTypeMember currentMember;
	private CodeTypeDeclaration currentType;
	private IndentedTextWriter writer;
	private CodeGeneratorOptions options;

	// Constructor.
	protected CodeGenerator()
			{
			}

	// Determine if an identifier is valid for language-independent use.
	public static bool IsValidLanguageIndependentIdentifier(String value)
			{
				// TODO
				return true;
			}

	// Get the current class member.
	protected CodeTypeMember CurrentMember
			{
				get
				{
					return currentMember;
				}
			}

	// Get the current class member's name.
	protected String CurrentMemberName
			{
				get
				{
					if(currentMember != null)
					{
						return currentMember.Name;
					}
					else
					{
						return "<% unknown %>";
					}
				}
			}

	// Get the current class type's name.
	protected String CurrentTypeName
			{
				get
				{
					if(currentType != null)
					{
						return currentType.Name;
					}
					else
					{
						return "<% unknown %>";
					}
				}
			}

	// Get or set the current indent level.
	protected int Indent
			{
				get
				{
					return writer.Indent;
				}
				set
				{
					writer.Indent = value;
				}
			}

	// Determine if the current type is a class.
	protected bool IsCurrentClass
			{
				get
				{
					return (currentType != null &&
							!(currentType is CodeTypeDelegate) &&
							currentType.IsClass);
				}
			}

	// Determine if the current type is a delegate.
	protected bool IsCurrentDelegate
			{
				get
				{
					return (currentType != null &&
							currentType is CodeTypeDelegate);
				}
			}

	// Determine if the current type is an enumeration.
	protected bool IsCurrentEnum
			{
				get
				{
					return (currentType != null &&
							!(currentType is CodeTypeDelegate) &&
							currentType.IsEnum);
				}
			}

	// Determine if the current type is an interface.
	protected bool IsCurrentInterface
			{
				get
				{
					return (currentType != null &&
							!(currentType is CodeTypeDelegate) &&
							currentType.IsInterface);
				}
			}

	// Determine if the current type is a struct.
	protected bool IsCurrentStruct
			{
				get
				{
					return (currentType != null &&
							!(currentType is CodeTypeDelegate) &&
							currentType.IsStruct);
				}
			}

	// Get the token for "null".
	protected abstract String NullToken { get; }

	// Get the current code generation options.
	protected CodeGeneratorOptions Options
			{
				get
				{
					return options;
				}
			}

	// Get the output text writer.
	protected TextWriter Output
			{
				get
				{
					return writer;
				}
			}

	// Output a continuation on a new line if the language requires it.
	protected virtual void ContinueOnNewLine(String st)
			{
				writer.WriteLine(st);
			}

	// Create an escaped identifier if "value" is a language keyword.
	protected abstract String CreateEscapedIdentifier(String value);

	// Create a valid identifier if "value" is a language keyword.
	protected abstract String CreateValidIdentifier(String value);

	// Generate various expression categories.
	protected abstract void GenerateArgumentReferenceExpression
				(CodeArgumentReferenceExpression e);
	protected abstract void GenerateArrayCreateExpression
				(CodeArrayCreateExpression e);
	protected abstract void GenerateArrayIndexerExpression
				(CodeArrayIndexerExpression e);
	protected abstract void GenerateBaseReferenceExpression
				(CodeBaseReferenceExpression e);
	protected abstract void GenerateCastExpression
				(CodeCastExpression e);
	protected abstract void GenerateDelegateCreateExpression
				(CodeDelegateCreateExpression e);
	protected abstract void GenerateDelegateInvokeExpression
				(CodeDelegateInvokeExpression e);
	protected abstract void GenerateDirectionExpression
				(CodeDirectionExpression e);
	protected abstract void GenerateEventReferenceExpression
				(CodeEventReferenceExpression e);
	protected abstract void GenerateFieldReferenceExpression
				(CodeFieldReferenceExpression e);
	protected abstract void GenerateIndexerExpression
				(CodeIndexerExpression e);
	protected abstract void GenerateMethodInvokeExpression
				(CodeMethodInvokeExpression e);
	protected abstract void GenerateMethodReferenceExpression
				(CodeMethodReferenceExpression e);
	protected abstract void GenerateObjectCreateExpression
				(CodeObjectCreateExpression e);
	protected abstract void GenerateParameterDeclarationExpression
				(CodeParameterDeclarationExpression e);
	protected abstract void GeneratePropertyReferenceExpression
				(CodePropertyReferenceExpression e);
	protected abstract void GeneratePropertySetValueReferenceExpression
				(CodePropertySetValueReferenceExpression e);
	protected abstract void GenerateSnippetExpression
				(CodeSnippetExpression e);
	protected abstract void GenerateThisReferenceExpression
				(CodeThisReferenceExpression e);
	protected abstract void GenerateVariableReferenceExpression
				(CodeVariableReferenceExpression e);

	// Generate various statement categories.
	protected abstract void GenerateAssignStatement
				(CodeAssignStatement e);
	protected abstract void GenerateAttachEventStatement
				(CodeAttachEventStatement e);
	protected abstract void GenerateConditionStatement
				(CodeConditionStatement e);
	protected abstract void GenerateExpressionStatement
				(CodeExpressionStatement e);
	protected abstract void GenerateGotoStatement
				(CodeGotoStatement e);
	protected abstract void GenerateIterationStatement
				(CodeIterationStatement e);
	protected abstract void GenerateLabeledStatement
				(CodeLabeledStatement e);
	protected abstract void GenerateMethodReturnStatement
				(CodeMethodReturnStatement e);
	protected abstract void GenerateRemoveEventStatement
				(CodeRemoveEventStatement e);
	protected abstract void GenerateThrowExceptionStatement
				(CodeThrowExceptionStatement e);
	protected abstract void GenerateTryCatchFinallyStatement
				(CodeTryCatchFinallyStatement e);
	protected abstract void GenerateVariableDeclarationStatement
				(CodeVariableDeclarationStatement e);

	// Generate various declaration categories.
	protected abstract void GenerateAttributeDeclarationsStart
				(CodeAttributeDeclarationCollection attributes);
	protected abstract void GenerateAttributeDeclarationsEnd
				(CodeAttributeDeclarationCollection attributes);
	protected abstract void GenerateConstructor
				(CodeConstructor e, CodeTypeDeclaration c);
	protected abstract void GenerateEntryPointMethod
				(CodeEntryPointMethod e, CodeTypeDeclaration c);
	protected abstract void GenerateEvent
				(CodeMemberEvent e, CodeTypeDeclaration c);
	protected abstract void GenerateField(CodeMemberField e);
	protected abstract void GenerateMethod
				(CodeMemberMethod e, CodeTypeDeclaration c);
	protected abstract void GenerateProperty
				(CodeMemberProperty e, CodeTypeDeclaration c);
	protected abstract void GenerateNamespaceStart(CodeNamespace e);
	protected abstract void GenerateNamespaceEnd(CodeNamespace e);
	protected abstract void GenerateNamespaceImport(CodeNamespaceImport e);
	protected abstract void GenerateSnippetMember
				(CodeSnippetTypeMember e);
	protected abstract void GenerateTypeConstructor
				(CodeTypeConstructor e);
	protected abstract void GenerateTypeStart(CodeTypeDeclaration e);
	protected abstract void GenerateTypeEnd(CodeTypeDeclaration e);

	// Generate various misc categories.
	protected abstract void GenerateComment(CodeComment e);
	protected abstract void GenerateLinePragmaStart(CodeLinePragma e);
	protected abstract void GenerateLinePragmaEnd(CodeLinePragma e);
	protected abstract String GetTypeOutput(CodeTypeReference value);

	// Generate code for a binary operator expression.
	protected virtual void GenerateBinaryOperatorExpression
				(CodeBinaryOperatorExpression e)
			{
				// TODO
			}

	// Generate code for comment statements.
	protected virtual void GenerateCommentStatement
				(CodeCommentStatement e)
			{
				GenerateComment(e.Comment);
			}
	protected virtual void GenerateCommentStatements
				(CodeCommentStatementCollection e)
			{
				foreach(CodeCommentStatement comment in e)
				{
					GenerateCommentStatement(comment);
				}
			}

	// Generate code for a compilation unit.
	protected virtual void GenerateCompileUnit(CodeCompileUnit e)
			{
				GenerateCompileUnitStart(e);
				GenerateNamespaces(e);
				GenerateCompileUnitEnd(e);
			}
	protected virtual void GenerateCompileUnitStart(CodeCompileUnit e)
			{
				// Nothing to do here.
			}
	protected virtual void GenerateCompileUnitEnd(CodeCompileUnit e)
			{
				// Nothing to do here.
			}

	// Generate code for constants.
	protected virtual void GenerateDecimalValue(Decimal d)
			{
				writer.Write(d.ToString(CultureInfo.InvariantCulture));
			}
	protected virtual void GenerateDoubleValue(double d)
			{
				writer.Write(d.ToString("R", CultureInfo.InvariantCulture));
			}
	protected virtual void GenerateSingleFloatValue(float s)
			{
				writer.Write(s.ToString(CultureInfo.InvariantCulture));
			}

	// Generate code for an expression.
	protected void GenerateExpression(CodeExpression e)
			{
				// TODO
			}

	// Generate code for namespace information.
	protected virtual void GenerateNamespace(CodeNamespace e)
			{
				GenerateNamespaceStart(e);
				GenerateNamespaceImports(e);
				// TODO
				GenerateNamespaceEnd(e);
			}
	protected void GenerateNamespaceImports(CodeNamespace e)
			{
				// TODO
			}
	protected void GenerateNamespaces(CodeCompileUnit e)
			{
				// TODO
			}

	// Generate code for a primitive expression.
	protected virtual void GeneratePrimitiveExpression
				(CodePrimitiveExpression e)
			{
				// TODO
			}

	// Generate a compile unit snippet.
	protected virtual void GenerateSnippetCompileUnit
				(CodeSnippetCompileUnit e)
			{
				// TODO
			}

	// Generate a code statement snippet.
	protected virtual void GenerateSnippetStatement(CodeSnippetStatement e)
			{
				// TODO
			}

	// Generate code for a statement.
	protected void GenerateStatement(CodeStatement e)
			{
				// TODO
			}

	// Generate code for a statement collection.
	protected void GenerateStatements(CodeStatementCollection e)
			{
				foreach(CodeStatement stmt in e)
				{
					GenerateStatement(stmt);
				}
			}

	// Generate a typeof expression.
	protected virtual void GenerateTypeOfExpression
				(CodeTypeOfExpression e)
			{
				// TODO
			}

	// Generate a type reference expression.
	protected virtual void GenerateTypeReferenceExpression
				(CodeTypeReferenceExpression e)
			{
				// TODO
			}

	// Generate the types in a namespace.
	protected void GenerateTypes(CodeNamespace e)
			{
				// TODO
			}

	// Determine if "value" is a valid identifier.
	protected abstract bool IsValidIdentifier(String value);

	// Output an attribute argument.
	protected virtual void OutputAttributeArgument
				(CodeAttributeArgument arg)
			{
				// TODO
			}

	// Output attribute declarations.
	protected virtual void OutputAttributeDeclarations
				(CodeAttributeDeclarationCollection attributes)
			{
				GenerateAttributeDeclarationsStart(attributes);
				foreach(CodeAttributeDeclaration attr in attributes)
				{
					// TODO
				}
				GenerateAttributeDeclarationsEnd(attributes);
			}

	// Output a field direction value.
	protected virtual void OutputDirection(FieldDirection dir)
			{
				// TODO
			}

	// Output an expression list.
	protected virtual void OutputExpressionList
				(CodeExpressionCollection expressions)
			{
				OutputExpressionList(expressions, false);
			}
	protected virtual void OutputExpressionList
				(CodeExpressionCollection expressions,
				 bool newlineBetweenItems)
			{
				// TODO
			}

	// Output a field scope modifier.
	protected virtual void OutputFieldScopeModifier
				(MemberAttributes attributes)
			{
				// TODO
			}

	// Output an identifier.
	protected virtual void OutputIdentifier(String ident)
			{
				// TODO
			}

	// Output a member access modifier.
	protected virtual void OutputMemberAccessModifier
				(MemberAttributes attributes)
			{
				// TODO
			}

	// Output a member scope modifier.
	protected virtual void OutputMemberScopeModifier
				(MemberAttributes attributes)
			{
				// TODO
			}

	// Output a binary operator.
	protected virtual void OutputOperator(CodeBinaryOperatorType op)
			{
				// TODO
			}

	// Output a list of parameters.
	protected virtual void OutputParameters
				(CodeParameterDeclarationExpressionCollection parameters)
			{
				// TODO
			}

	// Output a type.
	protected abstract void OutputType(CodeTypeReference typeRef);

	// Output the attributes for a type.
	protected virtual void OutputTypeAttributes
				(TypeAttributes attributes, bool isStruct, bool isEnum)
			{
				// TODO
			}

	// Output a type name pair.
	protected virtual void OutputTypeNamePair
				(CodeTypeReference typeRef, String name)
			{
				// TODO
			}

	// Quote a snippet string.
	protected abstract String QuoteSnippetString(String value);

	// Determine if this code generator supports a particular
	// set of generator options.
	protected abstract bool Supports(GeneratorSupport supports);

	// Validate an identifier and throw an exception if not valid.
	protected virtual void ValidateIdentifier(String value)
			{
				if(!IsValidIdentifier(value))
				{
					throw new ArgumentException(S._("Arg_InvalidIdentifier"));
				}
			}

	// Create an escaped identifier if "value" is a language keyword.
	String ICodeGenerator.CreateEscapedIdentifier(String value)
			{
				return CreateEscapedIdentifier(value);
			}

	// Create a valid identifier if "value" is a language keyword.
	String ICodeGenerator.CreateValidIdentifier(String value)
			{
				return CreateValidIdentifier(value);
			}

	// Generate code from a CodeDom compile unit.
	void ICodeGenerator.GenerateCodeFromCompileUnit(CodeCompileUnit e,
									                TextWriter w,
									                CodeGeneratorOptions o)
			{
				// TODO
			}

	// Generate code from a CodeDom expression.
	void ICodeGenerator.GenerateCodeFromExpression(CodeExpression e,
												   TextWriter w,
												   CodeGeneratorOptions o)
			{
				// TODO
			}

	// Generate code from a CodeDom namespace.
	void ICodeGenerator.GenerateCodeFromNamespace(CodeNamespace e,
								   				  TextWriter w,
								   				  CodeGeneratorOptions o)
			{
				// TODO
			}

	// Generate code from a CodeDom statement.
	void ICodeGenerator.GenerateCodeFromStatement(CodeStatement e,
								   				  TextWriter w,
								   				  CodeGeneratorOptions o)
			{
				// TODO
			}

	// Generate code from a CodeDom type declaration.
	void ICodeGenerator.GenerateCodeFromType(CodeTypeDeclaration e,
							  				 TextWriter w,
							  				 CodeGeneratorOptions o)
			{
				// TODO
			}

	// Get the type indicated by a CodeDom type reference.
	String ICodeGenerator.GetTypeOutput(CodeTypeReference type)
			{
				return GetTypeOutput(type);
			}

	// Determine if "value" is a valid identifier.
	bool ICodeGenerator.IsValidIdentifier(String value)
			{
				return IsValidIdentifier(value);
			}

	// Determine if this code generator supports a particular
	// set of generator options.
	bool ICodeGenerator.Supports(GeneratorSupport supports)
			{
				return Supports(supports);
			}

	// Validate an identifier and throw an exception if not valid.
	void ICodeGenerator.ValidateIdentifier(String value)
			{
				ValidateIdentifier(value);
			}

}; // class CodeGenerator

#endif // !ECMA_COMPAT

}; // namespace System.CodeDom.Compiler
