%{
/*
 * cs_grammar.y - Input file for yacc that defines the syntax of C#.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

/* Rename the lex/yacc symbols to support multiple parsers */
#include "cs_rename.h"

/*#define YYDEBUG 1*/

#include <stdio.h>
#include "il_system.h"
#include "il_opcodes.h"
#include "il_meta.h"
#include "il_utils.h"
#include "cs_internal.h"
#ifdef HAVE_STDARG_H
	#include <stdarg.h>
#else
	#ifdef HAVE_VARARGS_H
		#include <varargs.h>
	#endif
#endif

#define	YYERROR_VERBOSE

/*
 * Imports from the lexical analyser.
 */
extern int yylex(void);
#ifdef YYTEXT_POINTER
extern char *cs_text;
#else
extern char cs_text[];
#endif

/*
 * Global code generator object.
 */
ILGenInfo CSCodeGen;

/*
 * Global parse tree return variable.
 */
ILNode *CSParseTree = 0;
static ILNode *ParseTreeEnd = 0;

/*
 * Global scope object.
 */
ILScope *CSGlobalScope = 0;

/*
 * Global state used by the parser.
 */
static unsigned long NestingLevel = 0;
static ILIntString CurrNamespace = {"", 0};
static int HaveDecls = 0;

/*
 * Get the global scope.
 */
static ILScope *GlobalScope(void)
{
	if(CSGlobalScope)
	{
		return CSGlobalScope;
	}
	else
	{
		CSGlobalScope = ILScopeCreate(&CSCodeGen, 0);
		return CSGlobalScope;
	}
}

/*
 * Reset the global state ready for the next file to be parsed.
 */
static void ResetState(void)
{
	NestingLevel = 0;
	CurrNamespace = ILInternString("", 0);
	HaveDecls = 0;
	ILScopeClearUsing(GlobalScope());
}

static void
yyerror(char *msg)
{
	if(!strcmp(msg, "parse error"))
	{
		/* This error is pretty much useless at telling the user
		   what is happening.  Try to print a better message
		   based on the current input token */
		CSError("parse error at or near `%s'", cs_text);
	}
	else
	{
		/* The parser has probably included information as to what
		   is expected in this context, so report that */
		CSError("%s", msg);
	}
}

/*
 * Determine if an extension has been enabled using "-f".
 */
#define	HaveExtension(name)	\
	(CSStringListContains(extension_flags, num_extension_flags, (name)))

/*
 * Get the filename and line number information for node creation.
 */
char *yycurrfilename(void)
{
	return CSPreProcessorStream.lexerFileName;
}
long yycurrlinenum(void)
{
	return CSPreProcessorStream.lexerLineNumber;
}

/*
 * Abort the compiler if we run out of memory in the node creation routines.
 */
void yynodefailed(void)
{
	CSOutOfMemory();
}

/*
 * Make a simple node and put it into $$.
 */
#define	MakeSimple(classSuffix)	\
	do {	\
		yyval.node = \
			ILNode_##classSuffix##_create(); \
	} while (0)

/*
 * Make a unary node and put it into $$.
 */
#define	MakeUnary(classSuffix,expr)	\
	do {	\
		yyval.node = ILNode_##classSuffix##_create((expr)); \
	} while (0)

/*
 * Make a binary node and put it into $$.
 */
#define	MakeBinary(classSuffix,expr1,expr2)	\
	do {	\
		yyval.node = ILNode_##classSuffix##_create((expr1), (expr2)); \
	} while (0)

/*
 * Make a ternary node and put it into $$.
 */
#define	MakeTernary(classSuffix,expr1,expr2,expr3)	\
	do {	\
		yyval.node = ILNode_##classSuffix##_create((expr1), (expr2), (expr3)); \
	} while (0)

/*
 * Make a quaternary node and put it into $$.
 */
#define	MakeQuaternary(classSuffix,expr1,expr2,expr3,expr4)	\
	do {	\
		yyval.node = ILNode_##classSuffix##_create \
							((expr1), (expr2), (expr3), (expr4)); \
	} while (0)

/*
 * Make a system type name node and put it into $$.
 */
#define	MakeSystemType(name)	(ILQualIdentTwo("System", #name))

/*
 * Add a node to the top-level parse tree.
 */
static void AddTopLevel(ILNode *node)
{
	if(!node)
	{
		return;
	}
	if(!CSParseTree)
	{
		CSParseTree = ILNode_List_create();
		ParseTreeEnd = 0;
	}
	if(!ParseTreeEnd)
	{
		ParseTreeEnd = CSParseTree;
	}
	ILNode_List_Add(ParseTreeEnd, node);
	if(((ILNode_List *)ParseTreeEnd)->rest)
	{
		ParseTreeEnd = (ILNode *)(((ILNode_List *)ParseTreeEnd)->rest);
	}
}

/*
 * Negate an integer node.
 */
static ILNode *NegateInteger(ILNode_Integer *node)
{
	if(node->canneg)
	{
		if(yyisa(node, ILNode_Int32))
		{
			node->isneg = !(node->isneg);
			return (ILNode *)node;
		}
		else if(yyisa(node, ILNode_UInt32))
		{
			return ILNode_Int32_create(node->value, 1, 0);
		}
		else if(yyisa(node, ILNode_Int64))
		{
			node->isneg = !(node->isneg);
			return (ILNode *)node;
		}
		else if(yyisa(node, ILNode_UInt64))
		{
			return ILNode_Int64_create(node->value, 1, 0);
		}
	}
	return ILNode_Neg_create((ILNode *)node);
}

/*
 * The class name stack, which is used to verify the names
 * of constructors and destructors against the name of their
 * enclosing classes.
 */
static ILNode **classNameStack = 0;
static int		classNameStackSize = 0;
static int		classNameStackMax = 0;

/*
 * Push an item onto the class name stack.
 */
static void ClassNamePush(ILNode *name)
{
	if(classNameStackSize >= classNameStackMax)
	{
		classNameStack = (ILNode **)ILRealloc
			(classNameStack, sizeof(ILNode *) * (classNameStackMax + 4));
		if(!classNameStack)
		{
			CSOutOfMemory();
		}
		classNameStackMax += 4;
	}
	classNameStack[classNameStackSize++] = name;
}

/*
 * Pop an item from the class name stack.
 */
static void ClassNamePop(void)
{
	--classNameStackSize;
}

/*
 * Determine if an identifier is identical to
 * the top of the class name stack.
 */
static int ClassNameSame(ILNode *name)
{
	return (strcmp(((ILNode_Identifier *)name)->name,
	   ((ILNode_Identifier *)(classNameStack[classNameStackSize - 1]))->name)
	   			== 0);
}

%}

/*
 * Define the structure of yylval.
 */
%union {
	struct
	{
		ILUInt64	value;
		int			type;
		int			canneg;
	}					integer;
	struct
	{
		ILDouble	value;
		int			type;
	}					real;
	ILDecimal  			decimal;
	ILUInt16			charValue;
	ILIntString			string;
	char			   *name;
	ILUInt32			count;
	ILUInt32			mask;
	ILNode			   *node;
	struct
	{
		ILNode		   *type;
		char		   *id;
	} catchinfo;
	struct
	{
		ILNode		   *item1;
		ILNode		   *item2;
	} pair;
	ILParameterModifier	pmod;
	struct
	{
		char           *binary;
		char           *unary;

	} opName;
	struct
	{
		ILNode		   *type;
		ILNode		   *ident;
		ILNode		   *params;

	} indexer;
}

/*
 * Primitive lexical tokens.
 */
%token INTEGER_CONSTANT		"integer value"
%token CHAR_CONSTANT		"character constant"
%token IDENTIFIER			"identifier"
%token STRING_LITERAL		"string literal"
%token FLOAT_CONSTANT		"floating point value"
%token DECIMAL_CONSTANT		"decimal value"
%token DOC_COMMENT			"documentation comment"

/*
 * Keywords.
 */
%token ABSTRACT				"`abstract'"
%token ADD					"`add'"
%token AS					"`as'"
%token BASE					"`base'"
%token BOOL					"`bool'"
%token BREAK				"`break'"
%token BUILTIN_CONSTANT		"`__builtin_constant'"
%token BYTE					"`byte'"
%token CASE					"`case'"
%token CATCH				"`catch'"
%token CHAR					"`char'"
%token CHECKED				"`checked'"
%token CLASS				"`class'"
%token CONST				"`const'"
%token CONTINUE				"`continue'"
%token DECIMAL				"`decimal'"
%token DEFAULT				"`default'"
%token DELEGATE				"`delegate'"
%token DO					"`do'"
%token DOUBLE				"`double'"
%token ELSE					"`else'"
%token ENUM					"`enum'"
%token EVENT				"`event'"
%token EXPLICIT				"`explicit'"
%token EXTERN				"`extern'"
%token FALSE				"`false'"
%token FINALLY				"`finally'"
%token FIXED				"`fixed'"
%token FLOAT				"`float'"
%token FOR					"`for'"
%token FOREACH				"`foreach'"
%token GET					"`get'"
%token GOTO					"`goto'"
%token IF					"`if'"
%token IMPLICIT				"`implicit'"
%token IN					"`in'"
%token INT					"`int'"
%token INTERFACE			"`interface'"
%token INTERNAL				"`internal'"
%token IS					"`is'"
%token LOCK					"`lock'"
%token LONG					"`long'"
%token NAMESPACE			"`namespace'"
%token NEW					"`new'"
%token NULL_TOK				"`null'"
%token OBJECT				"`object'"
%token OPERATOR				"`operator'"
%token OUT					"`out'"
%token OVERRIDE				"`override'"
%token PARAMS				"`params'"
%token PRIVATE				"`private'"
%token PROTECTED			"`protected'"
%token PUBLIC				"`public'"
%token READONLY				"`readonly'"
%token REMOVE				"`remove'"
%token REF					"`ref'"
%token RETURN				"`return'"
%token SBYTE				"`sbyte'"
%token SEALED				"`sealed'"
%token SET					"`set'"
%token SHORT				"`short'"
%token SIZEOF				"`sizeof'"
%token STACKALLOC			"`stackalloc'"
%token STATIC				"`static'"
%token STRING				"`string'"
%token STRUCT				"`struct'"
%token SWITCH				"`switch'"
%token THIS					"`this'"
%token THROW				"`throw'"
%token TRUE					"`true'"
%token TRY					"`try'"
%token TYPEOF				"`typeof'"
%token UINT					"`uint'"
%token ULONG				"`ulong'"
%token UNCHECKED			"`unchecked'"
%token UNSAFE				"`unsafe'"
%token USHORT				"`ushort'"
%token USING				"`using'"
%token VIRTUAL				"`virtual'"
%token VOID					"`void'"
%token WHILE				"`while'"

/*
 * Operators.
 */
%token INC_OP				"++"
%token DEC_OP				"--"
%token LEFT_OP				"<<"
%token RIGHT_OP				">>"
%token LE_OP				"<="
%token GE_OP				">="
%token EQ_OP				"=="
%token NE_OP				"!="
%token AND_OP				"&&"
%token OR_OP				"||"
%token MUL_ASSIGN_OP		"*="
%token DIV_ASSIGN_OP		"/="
%token MOD_ASSIGN_OP		"%="
%token ADD_ASSIGN_OP		"+="
%token SUB_ASSIGN_OP		"-="
%token LEFT_ASSIGN_OP		"<<="
%token RIGHT_ASSIGN_OP		">>="
%token AND_ASSIGN_OP		"&="
%token XOR_ASSIGN_OP		"^="
%token OR_ASSIGN_OP			"|="
%token PTR_OP				"->"

/*
 * Define the yylval types of the various non-terminals.
 */
%type <name>		IDENTIFIER
%type <integer>		INTEGER_CONSTANT
%type <charValue>	CHAR_CONSTANT
%type <real>		FLOAT_CONSTANT
%type <decimal>		DECIMAL_CONSTANT
%type <string>		STRING_LITERAL DOC_COMMENT NamespaceIdentifier
%type <count>		DimensionSeparators DimensionSeparatorList
%type <mask>		OptModifiers Modifiers Modifier

%type <node>		Identifier QualifiedIdentifier BuiltinType

%type <node>		Type NonExpressionType LocalVariableType

%type <node>		TypeDeclaration ClassDeclaration ClassBase ClassBody
%type <node>		TypeList OptClassMemberDeclarations ClassMemberDeclarations
%type <node>		ClassMemberDeclaration StructDeclaration StructInterfaces
%type <node>		StructBody

%type <node>		PrimaryExpression UnaryExpression Expression
%type <node>		MultiplicativeExpression AdditiveExpression
%type <node>		ShiftExpression RelationalExpression EqualityExpression
%type <node>		AndExpression XorExpression OrExpression
%type <node>		LogicalAndExpression LogicalOrExpression
%type <node>		ConditionalExpression AssignmentExpression
%type <node>		ParenExpression ConstantExpression BooleanExpression
%type <node>		ParenBooleanExpression LiteralExpression
%type <node>		InvocationExpression ExpressionList
%type <node>		ObjectCreationExpression OptArgumentList ArgumentList
%type <node>		Argument PrefixedUnaryExpression

%type <node>		Statement EmbeddedStatement Block OptStatementList
%type <node>		StatementList ExpressionStatement SelectionStatement
%type <node>		SwitchBlock OptSwitchSections SwitchSections
%type <node>		SwitchSection SwitchLabels SwitchLabel IterationStatement
%type <node>		ForInitializer ForInitializerInner ForCondition
%type <node>		ForIterator ForeachExpression ExpressionStatementList
%type <node>		JumpStatement TryStatement CatchClauses
%type <node>		OptSpecificCatchClauses SpecificCatchClauses
%type <node>		SpecificCatchClause OptGeneralCatchClause
%type <node>		GeneralCatchClause FinallyClause LockStatement
%type <node>		UsingStatement ResourceAcquisition FixedStatement
%type <node>		FixedPointerDeclarators FixedPointerDeclarator

%type <node>		ConstantDeclaration ConstantDeclarators ConstantDeclarator
%type <node>		FieldDeclaration VariableDeclarators VariableDeclarator
%type <node>		VariableInitializer LocalVariableDeclaration
%type <node>		LocalConstantDeclaration
%type <node>		EventFieldDeclaration EventDeclaration 
%type <node>		EventPropertyDeclaration 
%type <pair>		EventAccessorBlock EventAccessorDeclarations

%type <node>		ReturnType MethodDeclaration MethodBody
%type <node>		OptFormalParameterList FormalParameterList FormalParameter
%type <pmod>		ParameterModifier
%type <node>		PropertyDeclaration 
%type <pair>		AccessorBlock AccessorDeclarations
%type <node>		OptGetAccessorDeclaration GetAccessorDeclaration
%type <node>		OptSetAccessorDeclaration SetAccessorDeclaration
%type <node>		AccessorBody
%type <node>		OptAddAccessorDeclaration AddAccessorDeclaration
%type <node>		OptRemoveAccessorDeclaration RemoveAccessorDeclaration
%type <node>		IndexerDeclaration
%type <node>		FormalIndexParameters FormalIndexParameter
%type <node>		FormalIndexParameterList
%type <node>		InterfaceDeclaration InterfaceBase InterfaceBody
%type <node>		OptInterfaceMemberDeclarations InterfaceMemberDeclarations
%type <node>		InterfaceMemberDeclarations InterfaceMemberDeclaration
%type <node>		InterfaceMemberDeclaration InterfaceMemberDeclarations
%type <node>		InterfaceMethodDeclaration InterfacePropertyDeclaration
%type <node>		InterfaceIndexerDeclaration InterfaceEventDeclaration
%type <mask>		InterfaceAccessors OptNew InterfaceAccessorBody
%type <node>		EnumBase EnumDeclaration EnumBody OptEnumMemberDeclarations
%type <node>		EnumMemberDeclarations EnumMemberDeclaration
%type <node>		DelegateDeclaration
%type <node>		ConstructorDeclaration ConstructorInitializer
%type <node>		DestructorDeclaration
%type <node>		OperatorDeclaration NormalOperatorDeclaration
%type <node>		ConversionOperatorDeclaration
%type <opName>		OverloadableOperator
%type <node>		TypeSuffix TypeSuffixList TypeSuffixes
%type <node>		OptAttributes AttributeSections AttributeSection
%type <node>		AttributeList Attribute AttributeArguments
%type <node>		PositionalArgumentList PositionalArgument NamedArgumentList
%type <node>		NamedArgument AttributeArgumentExpression
%type <node>		RankSpecifiers RankSpecifierList 
%type <node>		OptArrayInitializer ArrayInitializer
%type <node>		OptVariableInitializerList VariableInitializerList
%type <indexer>		IndexerDeclarator

%type <catchinfo>	CatchNameInfo
%expect 24

%start CompilationUnit
%%

/*
 * Outer level of the C# input file.
 */

CompilationUnit
	: /* empty */	{
				/* The input file is empty */
				CSTypedWarning("-empty-input",
							   "file contains no declarations");
				ResetState();
			}
	| OuterDeclarations		{
				/* Check for empty input and finalize the parse */
				if(!HaveDecls)
				{
					CSTypedWarning("-empty-input",
								   "file contains no declarations");
				}
				ResetState();
			}
	;

OuterDeclarations
	: OuterDeclaration
	| OuterDeclarations OuterDeclaration
	;

OuterDeclaration
	: UsingDirective
	| NamespaceMemberDeclaration
	| ';'
	| error			{
				/*
				 * This production recovers from errors at the outer level
				 * by skipping invalid tokens.
				 */
				yyerrok;
				yyclearin;
				NestingLevel = 0;
			}
	;

/*
 * Identifiers.
 */

Identifier
	: IDENTIFIER		{
				/* Build an undistinguished identifier node.  At this
				   point, we have no idea of the identifier's type.
				   We leave that up to the semantic analysis phase */
				$$ = ILQualIdentSimple($1);
			}
	;

QualifiedIdentifier
	: Identifier							{ $$ = $1; }
	| QualifiedIdentifier '.' Identifier	{ MakeBinary(QualIdent, $1, $3); }
	;

/*
 * Namespaces.
 */

NamespaceDeclaration
	: NAMESPACE NamespaceIdentifier {
				/* Push a new identifier onto the end of the namespace */
				if(CurrNamespace.len != 0)
				{
					CurrNamespace = ILInternAppendedString
						(CurrNamespace,
						 ILInternAppendedString
						 	(ILInternString(".", 1), $2));
				}
				else
				{
					CurrNamespace = $2;
				}
			}
			NamespaceBody OptSemiColon	{
				/* Pop the identifier from the end of the namespame */
				if(CurrNamespace.len == $2.len)
				{
					CurrNamespace = ILInternString("", 0);
				}
				else
				{
					CurrNamespace = ILInternString
						(CurrNamespace.string, CurrNamespace.len - $2.len - 1);
				}
			}
	;

NamespaceIdentifier
	: IDENTIFIER		{ $$ = ILInternString($1, strlen($1)); }
	| NamespaceIdentifier '.' IDENTIFIER	{
				$$ = ILInternAppendedString
					($1, ILInternAppendedString
					 		(ILInternString(".", 1),
							 ILInternString($3, strlen($3))));
			}
	;

OptSemiColon
	: /* empty */
	| ';'
	;

NamespaceBody
	: '{' OptUsingDirectives OptNamespaceMemberDeclarations '}'
	| '{' error '}'		{
				/*
				 * This production recovers from errors in namespace
				 * declarations.
				 */
				NestingLevel = 0;
			}
	;

OptUsingDirectives
	: /* empty */
	| UsingDirectives
	;

UsingDirectives
	: UsingDirective
	| UsingDirectives UsingDirective
	;

UsingDirective
	: USING IDENTIFIER '=' NamespaceIdentifier ';'	{
				ILScope *globalScope = GlobalScope();
				if(ILScopeLookup(globalScope, $2, 1))
				{
					CSError("`%s' is already declared", $2);
				}
				else if(!ILScopeUsing(globalScope, $4.string, $2))
				{
					CSError("`%s' is not a namespace", $4.string);
				}
			}
	| USING NamespaceIdentifier ';'		{
				ILScope *globalScope = GlobalScope();
				if(!ILScopeUsing(globalScope, $2.string, 0))
				{
					CSError("`%s' is not a namespace", $2.string);
				}
			}
	| USING error ';'	{
				/*
				 * This production recovers from errors in "using" directives.
				 */
			}
	;

OptNamespaceMemberDeclarations
	: /* empty */
	| NamespaceMemberDeclarations
	;

NamespaceMemberDeclarations
	: NamespaceMemberDeclaration
	| NamespaceMemberDeclarations NamespaceMemberDeclaration
	;

NamespaceMemberDeclaration
	: NamespaceDeclaration
	| TypeDeclaration			{ AddTopLevel($1); }
	;

TypeDeclaration
	: ClassDeclaration			{ $$ = $1; }
	| StructDeclaration			{ $$ = $1; }
	| InterfaceDeclaration		{ $$ = $1; }
	| EnumDeclaration			{ $$ = $1; }
	| DelegateDeclaration		{ $$ = $1; }
	;

/*
 * Types.
 */

Type
	: QualifiedIdentifier	{ $$ = $1; }
	| BuiltinType			{ $$ = $1; }
	| Type '[' DimensionSeparators ']'	{
				MakeBinary(ArrayType, $1, $3);
			}
	| Type '*'			{
				MakeUnary(PtrType, $1);
			}
	| VOID '*'			{
				MakeSimple(VoidPtrType);
			}
	;

NonExpressionType
	: BuiltinType			{ $$ = $1; }
	| NonExpressionType '[' DimensionSeparators ']'	{
				MakeBinary(ArrayType, $1, $3);
			}
	| Expression '[' DimensionSeparators ']'	{
				MakeBinary(ArrayType, $1, $3);
			}
	| NonExpressionType '*'	{
				MakeUnary(PtrType, $1);
			}
	| Expression '*'		{ 
				MakeUnary(PtrType, $1);
			}
	| VOID '*'				{
				MakeSimple(VoidPtrType);
			}
	;

/*
 * Types in local variable declarations must be recognized as
 * expressions to prevent reduce/reduce errors in the grammar.
 * The expressions are converted into types during semantic analysis.
 */
LocalVariableType
	: PrimaryExpression TypeSuffixes	{
				MakeBinary(LocalVariableType, $1, $2);
			}
	| BuiltinType TypeSuffixes			{
				MakeBinary(LocalVariableType, $1, $2);
			}
	| VOID TypeSuffixes					{
				MakeBinary(LocalVariableType, ILNode_VoidType_create(), $2);
			}
	;

TypeSuffixes
	: /* empty */		{ $$ = 0; }
	| TypeSuffixList	{ $$ = $1; }
	;

TypeSuffixList
	: TypeSuffix						{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| TypeSuffixList TypeSuffix			{
				ILNode_List_Add($1, $2);
				$$ = $1;
			}
	;

TypeSuffix
	: '[' DimensionSeparators ']'	{ MakeUnary(TypeSuffix, $2); }
	| '*'							{ MakeUnary(TypeSuffix, 0); }
	;

DimensionSeparators
	: /* empty */					{ $$ = 1; }
	| DimensionSeparatorList		{ $$ = $1; }
	;

DimensionSeparatorList
	: ','							{ $$ = 2; }
	| DimensionSeparatorList ','	{ $$ = $1 + 1; }
	;

BuiltinType
	: BOOL			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_BOOLEAN); }
	| SBYTE			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I1); }
	| BYTE			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U1); }
	| SHORT			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I2); }
	| USHORT		{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U2); }
	| INT			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I4); }
	| UINT			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U4); }
	| LONG			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I8); }
	| ULONG			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U8); }
	| CHAR			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_CHAR); }
	| FLOAT			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_R4); }
	| DOUBLE		{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_R8); }
	| DECIMAL		{ MakeSimple(DecimalType); }
	| OBJECT		{ MakeSimple(ObjectType); }
	| STRING		{ MakeSimple(StringType); }
	;

/*
 * Expressions.
 */

PrimaryExpression
	: LiteralExpression				{ $$ = $1; }
	| Identifier					{ $$ = $1; }
	| '(' Expression ')'			{ $$ = $2; }
	| PrimaryExpression '.' Identifier	{
				if(ILIsQualIdent($1))
				{
					MakeBinary(QualIdent, $1, $3);
				}
				else
				{
					MakeBinary(MemberAccess, $1, $3);
				}
			}
	| BuiltinType '.' Identifier	{ MakeBinary(QualIdent, $1, $3); }
	| InvocationExpression			{ $$ = $1; }
	| PrimaryExpression '[' ExpressionList ']'	{ /* safe and unsafe */ }
	| THIS							{ MakeSimple(This); }
	| BASE '.' Identifier			{ MakeUnary(BaseAccess, $3); }
	| BASE '[' ExpressionList ']'	{ MakeUnary(BaseElement, $3); }
	| PrimaryExpression INC_OP		{ MakeUnary(PostInc, $1); }
	| PrimaryExpression DEC_OP		{ MakeUnary(PostDec, $1); }
	| ObjectCreationExpression		{ $$ = $1; }
	| NEW Type '[' ExpressionList ']' RankSpecifiers OptArrayInitializer	{
				$$ = ILNode_NewExpression_create($2, $4, $6, $7);
			}
	| NEW Type ArrayInitializer		{
				$$ = ILNode_NewExpression_create($2, 0, 0, $3);
			}
	| TYPEOF '(' ReturnType ')'		{ MakeUnary(TypeOf, $3); }
	| SIZEOF '(' Type ')'			{
				/*
				 * This is only safe if it is used on one of the following
				 * builtin types: sbyte, byte, short, ushort, int, uint,
				 * long, ulong, float, double, char, bool.  We leave the
				 * check to the semantic analysis phase.
				 */
				MakeUnary(SizeOf, $3);
			}
	| CHECKED '(' Expression ')'	{ MakeUnary(Overflow, $3); }
	| UNCHECKED '(' Expression ')'	{ MakeUnary(NoOverflow, $3); }
	| PrimaryExpression PTR_OP Identifier	{
				MakeBinary(DerefField, $1, $3);
			}
	| STACKALLOC Type '[' Expression ']'	{
				MakeBinary(StackAlloc, $2, $4);
			}
	| BUILTIN_CONSTANT '(' STRING_LITERAL ')'	{
				/*
				 * Get the value of a builtin constant.
				 */
				$$ = CSBuiltinConstant($3.string);
			}
	;

LiteralExpression
	: TRUE						{ MakeSimple(True); }
	| FALSE						{ MakeSimple(False); }
	| NULL_TOK					{ MakeSimple(Null); }
	| INTEGER_CONSTANT			{
				switch($1.type)
				{
					case IL_TYPE_INT32:
					{
						$$ = ILNode_Int32_create($1.value, 0, $1.canneg);
					}
					break;

					case IL_TYPE_UINT32:
					{
						$$ = ILNode_UInt32_create($1.value, 0, $1.canneg);
					}
					break;

					case IL_TYPE_INT64:
					{
						$$ = ILNode_Int64_create($1.value, 0, $1.canneg);
					}
					break;

					default:
					{
						$$ = ILNode_UInt64_create($1.value, 0, $1.canneg);
					}
					break;
				}
			}
	| FLOAT_CONSTANT			{
				if($1.type == IL_TYPE_FLOAT32)
				{
					$$ = ILNode_Float32_create($1.value);
				}
				else
				{
					$$ = ILNode_Float64_create($1.value);
				}
			}
	| DECIMAL_CONSTANT			{
				$$ = ILNode_Decimal_create($1);
			}
	| CHAR_CONSTANT				{
				$$ = ILNode_Char_create((ILUInt64)($1), 0, 1);
			}
	| STRING_LITERAL			{
				$$ = ILNode_String_create($1.string, $1.len);
			}
	;

InvocationExpression
	: PrimaryExpression '(' OptArgumentList ')'		{ 
				MakeBinary(InvocationExpression, $1, $3); 
			}
	;

ObjectCreationExpression
	: NEW Type '(' OptArgumentList ')'	{ 
				MakeBinary(ObjectCreationExpression, $2, $4); 
			}
	;

OptArgumentList
	: /* empty */						{ $$ = 0; }
	| ArgumentList						{ $$ = $1; }
	;

ArgumentList
	: Argument							{ $$ = $1; }
	| ArgumentList ',' Argument			{ MakeBinary(ArgList, $1, $3); }
	;

Argument
	: Expression			{ MakeBinary(Argument, ILParamMod_empty, $1); }
	| OUT Expression		{ MakeBinary(Argument, ILParamMod_out, $2); }
	| REF Expression		{ MakeBinary(Argument, ILParamMod_ref, $2); }
	;

ExpressionList
	: Expression						{ $$ = $1; }
	| ExpressionList ',' Expression		{ MakeBinary(ArgList, $1, $3); }
	;

RankSpecifiers
	: /* empty */			{ $$ = 0;}
	| RankSpecifierList		{ $$ = $1;}
	;

RankSpecifierList
	: '[' DimensionSeparators ']'			{
					$$ = ILNode_List_create();
					ILNode_List_Add($$, $2);
				}
	| RankSpecifierList '[' DimensionSeparators ']'	{
					ILNode_List_Add($1, $3);
					$$ = $1;
				}
	;

/*
 * There is a slight ambiguity in the obvious definition of
 * UnaryExpression that creates shift/reduce conflicts when
 * casts are employed.  For example, the parser cannot tell
 * the diference between the following two cases:
 *
 *		(Expr1) - Expr2		-- parse as subtraction.
 *		(Type) -Expr		-- parse as negation and cast.
 *
 * Splitting the definition into two parts fixes the conflict.
 * It is not possible to use one of the operators '-', '+',
 * '*', '&', '++', or '--' after a cast type unless parentheses
 * are involved:
 *
 *		(Type)(-Expr)
 *
 * As a special exception, if the cast involves a builtin type
 * name such as "int", "double", "bool", etc, then the prefix
 * operators can be used.  i.e. the following will be parsed
 * correctly:
 *
 *		(int)-Expr
 *
 * whereas the following requires parentheses because "System"
 * may have been redeclared with a new meaning in a local scope:
 *
 *		(System.Int32)(-Expr)
 *
 * It is very difficult to resolve this in any other way because
 * the compiler does not know if an identifier is a type or not
 * until later.
 */
UnaryExpression
	: PrimaryExpression					{ $$ = $1; }
	| '!' PrefixedUnaryExpression		{ MakeUnary(LogicalNot, $2); }
	| '~' PrefixedUnaryExpression		{ MakeUnary(Not, $2); }
	| '(' Expression ')' UnaryExpression	{
				/*
				 * Note: we need to use a full "Expression" for the type,
				 * so that we don't get a reduce/reduce conflict with the
				 * rule "PrimaryExpression: '(' Expression ')'".  We later
				 * filter out expressions that aren't really types.
				 */
				MakeBinary(UserCast, $2, $4);
			}
	| '(' NonExpressionType ')' PrefixedUnaryExpression	{
				/*
				 * This rule recognizes types that involve non-expression
				 * identifiers such as "int", "bool", "string", etc.
				 */
				MakeBinary(UserCast, $2, $4);
			}
	;

PrefixedUnaryExpression
	: UnaryExpression				{ $$ = $1; }
	| '+' PrefixedUnaryExpression	{ MakeUnary(UnaryPlus, $2); }
	| '-' PrefixedUnaryExpression			{
				/* We create negate nodes carefully so that integer
				   and float constants can be negated in-place */
				if(yyisa($2, ILNode_Integer))
				{
					$$ = NegateInteger((ILNode_Integer *)$2);
				}
				else if(yyisa($2, ILNode_Real))
				{
					((ILNode_Real *)($2))->value =
							-(((ILNode_Real *)($2))->value);
					$$ = $2;
				}
				else
				{
					MakeUnary(Neg, $2);
				}
			}
	| INC_OP PrefixedUnaryExpression	{ MakeUnary(PreInc, $2); }
	| DEC_OP PrefixedUnaryExpression	{ MakeUnary(PreDec, $2); }
	| '*' PrefixedUnaryExpression		{ MakeUnary(Deref, $2); }
	| '&' PrefixedUnaryExpression		{ MakeUnary(AddressOf, $2); }
	;

MultiplicativeExpression
	: PrefixedUnaryExpression				{ $$ = $1; }
	| MultiplicativeExpression '*' PrefixedUnaryExpression	{
				MakeBinary(Mul, $1, $3);
			}
	| MultiplicativeExpression '/' PrefixedUnaryExpression	{
				MakeBinary(Div, $1, $3);
			}
	| MultiplicativeExpression '%' PrefixedUnaryExpression	{
				MakeBinary(Rem, $1, $3);
			}
	;

AdditiveExpression
	: MultiplicativeExpression		{ $$ = $1; }
	| AdditiveExpression '+' MultiplicativeExpression	{
				MakeBinary(Add, $1, $3);
			}
	| AdditiveExpression '-' MultiplicativeExpression	{
				MakeBinary(Sub, $1, $3);
			}
	;

ShiftExpression
	: AdditiveExpression			{ $$ = $1; }
	| ShiftExpression LEFT_OP AdditiveExpression	{
				MakeBinary(Shl, $1, $3);
			}
	| ShiftExpression RIGHT_OP AdditiveExpression	{
				MakeBinary(Shr, $1, $3);
			}
	;

RelationalExpression
	: ShiftExpression				{ $$ = $1; }
	| RelationalExpression '<' ShiftExpression		{
				MakeBinary(Lt, $1, $3);
			}
	| RelationalExpression '>' ShiftExpression		{
				MakeBinary(Gt, $1, $3);
			}
	| RelationalExpression LE_OP ShiftExpression	{
				MakeBinary(Le, $1, $3);
			}
	| RelationalExpression GE_OP ShiftExpression	{
				MakeBinary(Ge, $1, $3);
			}
	| RelationalExpression IS Type					{
				MakeBinary(IsUntyped, $1, $3);
			}
	| RelationalExpression AS Type					{
				MakeBinary(AsUntyped, $1, $3);
			}
	;

EqualityExpression
	: RelationalExpression			{ $$ = $1; }
	| EqualityExpression EQ_OP RelationalExpression	{
				MakeBinary(Eq, $1, $3);
			}
	| EqualityExpression NE_OP RelationalExpression	{
				MakeBinary(Ne, $1, $3);
			}
	;

AndExpression
	: EqualityExpression			{ $$ = $1; }
	| AndExpression '&' EqualityExpression	{
				MakeBinary(And, $1, $3);
			}
	;

XorExpression
	: AndExpression					{ $$ = $1; }
	| XorExpression '^' AndExpression		{
				MakeBinary(Xor, $1, $3);
			}
	;

OrExpression
	: XorExpression					{ $$ = $1; }
	| OrExpression '|' XorExpression		{
				MakeBinary(Or, $1, $3);
			}
	;

LogicalAndExpression
	: OrExpression					{ $$ = $1; }
	| LogicalAndExpression AND_OP OrExpression	{
				MakeBinary(LogicalAnd, $1, $3);
			}
	;

LogicalOrExpression
	: LogicalAndExpression			{ $$ = $1; }
	| LogicalOrExpression OR_OP LogicalAndExpression	{
				MakeBinary(LogicalOr, $1, $3);
			}
	;

ConditionalExpression
	: LogicalOrExpression			{ $$ = $1; }
	| LogicalOrExpression '?' Expression ':' Expression	{
				MakeTernary(Conditional, $1, $3, $5);
			}
	;

AssignmentExpression
	: PrefixedUnaryExpression '=' Expression	{
				MakeBinary(Assign, $1, $3);
			}
	| PrefixedUnaryExpression ADD_ASSIGN_OP Expression {
				MakeUnary(AssignAdd, ILNode_Add_create($1, $3));
			}
	| PrefixedUnaryExpression SUB_ASSIGN_OP Expression {
				MakeUnary(AssignSub, ILNode_Sub_create($1, $3));
			}
	| PrefixedUnaryExpression MUL_ASSIGN_OP Expression {
				MakeUnary(AssignMul, ILNode_Mul_create($1, $3));
			}
	| PrefixedUnaryExpression DIV_ASSIGN_OP Expression {
				MakeUnary(AssignDiv, ILNode_Div_create($1, $3));
			}
	| PrefixedUnaryExpression MOD_ASSIGN_OP Expression {
				MakeUnary(AssignRem, ILNode_Rem_create($1, $3));
			}
	| PrefixedUnaryExpression AND_ASSIGN_OP Expression {
				MakeUnary(AssignAnd, ILNode_And_create($1, $3));
			}
	| PrefixedUnaryExpression OR_ASSIGN_OP Expression {
				MakeUnary(AssignOr, ILNode_Or_create($1, $3));
			}
	| PrefixedUnaryExpression XOR_ASSIGN_OP Expression {
				MakeUnary(AssignXor, ILNode_Xor_create($1, $3));
			}
	| PrefixedUnaryExpression LEFT_ASSIGN_OP Expression {
				MakeUnary(AssignShl, ILNode_Shl_create($1, $3));
			}
	| PrefixedUnaryExpression RIGHT_ASSIGN_OP Expression {
				MakeUnary(AssignShr, ILNode_Shr_create($1, $3));
			}
	;

Expression
	: ConditionalExpression		{ $$ = $1; }
	| AssignmentExpression		{ $$ = $1; }
	;

ParenExpression
	: '(' Expression ')'		{ $$ = $2; }
	| '(' error ')'		{
				/*
				 * This production recovers from errors in expressions
				 * that are used with "switch".  Return 0 as the value.
				 */
				MakeTernary(Int32, 0, 0, 1);
			}
	;

ConstantExpression
	: Expression		{ MakeUnary(ToConst, $1); }
	;

BooleanExpression
	: Expression		{ MakeUnary(ToBool, $1); }
	;

ParenBooleanExpression
	: '(' BooleanExpression ')'		{ $$ = $2; }
	| '(' error ')'		{
				/*
				 * This production recovers from errors in boolean
				 * expressions that are used with "if", "while", etc.
				 * Default to "false" as the error condition's value.
				 */
				MakeSimple(False);
			}
	;

/*
 * Array initialization.
 */

OptArrayInitializer
	: /* empty */			{ $$ = 0;}
	| ArrayInitializer		{ $$ = $1;}
	;

ArrayInitializer
	: '{' OptVariableInitializerList OptComma '}' {$$ = $2;}
	;

OptVariableInitializerList
	: /* empty */				{ $$ = 0;}
	| VariableInitializerList	{ $$ = $1;}
	;

VariableInitializerList
	: VariableInitializer {	
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| VariableInitializerList ',' VariableInitializer {
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

VariableInitializer
	: Expression				{ $$ = $1;}
	| ArrayInitializer			{ $$ = $1;}
	;

OptComma
	: /* empty */
	| ','
	;

/*
 * Statements.
 */

Statement
	: Identifier ':' Statement		{
				/* Convert the identifier into a "GotoLabel" node */
				ILNode *label = ILNode_GotoLabel_create(ILQualIdentName($1, 0));

				/* Build a compound statement */
				$$ = ILNode_Compound_CreateFrom(label, $3);
			}
	| LocalVariableDeclaration ';'	{ $$ = $1; }
	| LocalConstantDeclaration ';'	{ $$ = $1; }
	| EmbeddedStatement				{ $$ = $1; }
	;

EmbeddedStatement
	: Block							{ $$ = $1; }
	| ';'							{ MakeSimple(Empty); }
	| ExpressionStatement ';'		{ $$ = $1; }
	| SelectionStatement			{ $$ = $1; }
	| IterationStatement			{ $$ = $1; }
	| JumpStatement					{ $$ = $1; }
	| TryStatement					{ $$ = $1; }
	| CHECKED Block					{ MakeUnary(Overflow, $2); }
	| UNCHECKED Block				{ MakeUnary(NoOverflow, $2); }
	| LockStatement					{ $$ = $1; }
	| UsingStatement				{ $$ = $1; }
	| FixedStatement				{ $$ = $1; }
	| error ';'		{
				/*
				 * This production recovers from parse errors in statements,
				 * by seaching for the end of the current statement.
				 */
				MakeSimple(Empty);
			}
	;

LocalVariableDeclaration
	: LocalVariableType VariableDeclarators		{
				$$ = ILNode_VarDeclaration_create(0, 0, $1, $2);
			}
	;

VariableDeclarators
	: VariableDeclarator						{
				$$ = ILNode_VarDeclarators_create ();
				ILNode_List_Add($$, $1);
			}	
	| VariableDeclarators ',' VariableDeclarator{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
		
	;

VariableDeclarator
	: Identifier							{MakeBinary(VarDeclarator, $1, 0);}
	| Identifier '=' VariableInitializer	{MakeBinary(VarDeclarator, $1, $3);}
	;

LocalConstantDeclaration
	: CONST Type ConstantDeclarators		{
				$$ = ILNode_ConstDeclaration_create(0, 0, $2, $3);
			}
	;

Block
	: '{' OptStatementList '}'	{ $$ = $2; }
	| '{' error '}'		{
				/*
				 * This production recovers from parse errors in
				 * a block, by closing off the block on error.
				 */
				MakeSimple(Empty);
			}
	;

OptStatementList
	: /* empty */				{ MakeSimple(Empty); }
	| StatementList				{ $$ = $1; }
	;

StatementList
	: Statement					{ $$ = $1; }
	| StatementList Statement	{ $$ = ILNode_Compound_CreateFrom($1, $2); }
	;

ExpressionStatement
	: InvocationExpression				{ $$ = $1; }
	| ObjectCreationExpression			{ $$ = $1; }
	| AssignmentExpression				{ $$ = $1; }
	| PrimaryExpression INC_OP			{ MakeUnary(PostInc, $1); }
	| PrimaryExpression DEC_OP			{ MakeUnary(PostDec, $1); }
	| INC_OP PrefixedUnaryExpression	{ MakeUnary(PreInc, $2); }
	| DEC_OP PrefixedUnaryExpression	{ MakeUnary(PreDec, $2); }
	;

SelectionStatement
	: IF ParenBooleanExpression EmbeddedStatement	{
				MakeTernary(If, ILNode_ToBool_create($2), $3,
							ILNode_Empty_create());
			}
	| IF ParenBooleanExpression EmbeddedStatement ELSE EmbeddedStatement	{
				MakeTernary(If, ILNode_ToBool_create($2), $3, $5);
			}
	| SWITCH ParenExpression SwitchBlock	{
				MakeBinary(Switch, $2, $3);
			}
	;

SwitchBlock
	: '{' OptSwitchSections '}'		{ $$ = $2; }
	| '{' error '}'		{
				/*
				 * This production recovers from parse errors in the
				 * body of a switch statement.
				 */
				$$ = 0;
			}
	;

OptSwitchSections
	: /* empty */				{ $$ = 0; }
	| SwitchSections			{ $$ = $1; }
	;

SwitchSections
	: SwitchSection					{ 
				$$ = ILNode_SwitchSectList_create();
				ILNode_List_Add($$, $1);
			}
	| SwitchSections SwitchSection	{
				/* Append the new section to the list */
				ILNode_List_Add($1, $2);
				$$ = $1;
			}
	;

SwitchSection
	: SwitchLabels StatementList	{ MakeBinary(SwitchSection, $1, $2); }
	;

SwitchLabels
	: SwitchLabel					{
				/* Create a new label list with one element */
				$$ = ILNode_CaseList_create();
				ILNode_List_Add($$, $1);
			}
	| SwitchLabels SwitchLabel		{
				/* Append the new label to the list */
				ILNode_List_Add($1, $2);
				$$ = $1;
			}
	;

SwitchLabel
	: CASE ConstantExpression ':'	{ MakeUnary(CaseLabel, $2); }
	| DEFAULT ':'					{ MakeSimple(DefaultLabel); }
	;

IterationStatement
	: WHILE ParenBooleanExpression EmbeddedStatement	{
				MakeBinary(While, ILNode_ToBool_create($2), $3);
			}
	| DO EmbeddedStatement WHILE ParenBooleanExpression	{
				MakeBinary(Do, $2, ILNode_ToBool_create($4));
			}
	| FOR '(' ForInitializer ForCondition ForIterator EmbeddedStatement	{
				MakeQuaternary(For, $3, ILNode_ToBool_create($4), $5, $6);
			}
	| FOREACH '(' Type Identifier IN ForeachExpression EmbeddedStatement	{
				MakeQuaternary(Foreach, $3, ILQualIdentName($4, 0), $6, $7);
			}
	;

ForInitializer
	: ForInitializerInner ';'	{ $$ = $1; }
	| ';'						{ MakeSimple(Empty); }
	| error ';'		{
				/*
				 * This production recovers from errors in the initializer
				 * of a "for" statement.
				 */
				MakeSimple(Empty);
			}
	;

ForInitializerInner
	: LocalVariableDeclaration	{ $$ = $1; }
	| ExpressionStatementList	{ $$ = $1; }
	;

ForCondition
	: BooleanExpression ';'		{ $$ = $1; }
	| ';'						{ MakeSimple(True); }
	| error ';'		{
				/*
				 * This production recovers from errors in the condition
				 * of a "for" statement.
				 */
				MakeSimple(False);
			}
	;

ForIterator
	: ExpressionStatementList ')'	{ $$ = $1; }
	| ')'							{ MakeSimple(Empty); }
	| error ')'		{
				/*
				 * This production recovers from errors in the interator
				 * of a "for" statement.
				 */
				MakeSimple(Empty);
			}
	;

ForeachExpression
	: Expression ')'			{ $$ = $1; }
	| error ')'		{
				/*
				 * This production recovers from errors in the expression
				 * used within a "foreach" statement.
				 */
				MakeSimple(Null);
			}
	;

ExpressionStatementList
	: ExpressionStatement		{ $$ = $1; }
	| ExpressionStatementList ',' ExpressionStatement	{
				$$ = ILNode_Compound_CreateFrom($1, $3);
			}
	;

JumpStatement
	: BREAK ';'					{ MakeSimple(Break); }
	| CONTINUE ';'				{ MakeSimple(Continue); }
	| GOTO Identifier ';'		{
				/* Convert the identifier node into a "Goto" node */
				$$ = ILNode_Goto_create(ILQualIdentName($2, 0));
			}
	| GOTO CASE ConstantExpression ';'	{ MakeUnary(GotoCase, $3); }
	| GOTO DEFAULT ';'					{ MakeSimple(GotoDefault); }
	| RETURN ';'						{ MakeSimple(Return); }
	| RETURN Expression ';'				{ MakeUnary(ReturnExpr, $2); }
	| THROW ';'							{ MakeSimple(Throw); }
	| THROW Expression ';'				{ MakeUnary(ThrowExpr, $2); }
	;

TryStatement
	: TRY Block CatchClauses				{ MakeTernary(Try, $2, $3, 0); }
	| TRY Block FinallyClause				{ MakeTernary(Try, $2, 0, $3); }
	| TRY Block CatchClauses FinallyClause	{ MakeTernary(Try, $2, $3, $4); }
	;

CatchClauses
	: SpecificCatchClauses OptGeneralCatchClause	{
				if($2)
				{
					ILNode_List_Add($1, $2);
				}
				$$ = $1;
			}
	| OptSpecificCatchClauses GeneralCatchClause	{
				if($1)
				{
					ILNode_List_Add($1, $2);
					$$ = $1;
				}
				else
				{
					$$ = ILNode_CatchClauses_create();
					ILNode_List_Add($$, $2);
				}
			}
	;

OptSpecificCatchClauses
	: /* empty */				{ $$ = 0; }
	| SpecificCatchClauses		{ $$ = $1; }
	;

SpecificCatchClauses
	: SpecificCatchClause		{
				$$ = ILNode_CatchClauses_create();
				ILNode_List_Add($$, $1);
			}
	| SpecificCatchClauses SpecificCatchClause	{
				ILNode_List_Add($1, $2);
				$$ = $1;
			}
	;

SpecificCatchClause
	: CATCH CatchNameInfo Block	{
				MakeTernary(CatchClause, $2.type, $2.id, $3);
			}
	;

CatchNameInfo
	: '(' Type Identifier ')' { $$.type = $2; $$.id = ILQualIdentName($3, 0); }
	| '(' Type ')'			  { $$.type = $2; $$.id = 0; }
	| '(' error ')'	{
				/*
				 * This production recovers from errors in catch
				 * variable name declarations.
				 */
				$$.type = ILNode_Error_create();
				$$.id = 0;
			}
	;

OptGeneralCatchClause
	: /* empty */				{ $$ = 0; }
	| GeneralCatchClause		{ $$ = $1; }
	;

GeneralCatchClause
	: CATCH Block		{ MakeTernary(CatchClause, 0, 0, $2); }
	;

FinallyClause
	: FINALLY Block		{ MakeUnary(FinallyClause, $2); }
	;

LockStatement
	: LOCK ParenExpression EmbeddedStatement	{
				MakeBinary(Lock, $2, $3);
			}
	;

UsingStatement
	: USING ResourceAcquisition EmbeddedStatement	{
				MakeBinary(Using, $2, $3);
			}
	;

ResourceAcquisition
	: '(' LocalVariableDeclaration ')'	{ $$ = 0; }
	| '(' Expression ')'				{ $$ = $2; }
	| '(' error ')'		{
				/*
				 * This production recovers from errors in resource
				 * acquisition declarations.
				 */
				MakeSimple(Error);
			}
	;

/* unsafe code */
FixedStatement
	: FIXED '(' Type FixedPointerDeclarators ')' EmbeddedStatement	{
				MakeTernary(Fixed, $3, $4, $6);
			}
	;

FixedPointerDeclarators
	: FixedPointerDeclarator		{
				$$ = ILNode_FixedDeclList_create();
				ILNode_List_Add($$, $1);
			}
	| FixedPointerDeclarators ',' FixedPointerDeclarator	{
				$$ = $1;
				ILNode_List_Add($1, $3);
			}
	;

FixedPointerDeclarator
	: Identifier '=' Expression	{
				/*
				 * Note: we have to handle two cases here.  One where
				 * the expression has the form "&expr", and the other
				 * where it doesn't have that form.  We cannot express
				 * these as two different rules, or it creates a
				 * reduce/reduce conflict with "UnaryExpression".
				 */
				if(yykind($3) == yykindof(ILNode_AddressOf))
				{
					MakeBinary(FixAddress, ILQualIdentName($1, 0),
							   ((ILNode_UnaryExpression *)($3))->expr);
				}
				else
				{
					MakeBinary(FixExpr, ILQualIdentName($1, 0), $3);
				}
			}
	;

/*
 * Attributes.
 */

OptAttributes
	: /* empty */ 		{ $$ = 0; }
	| AttributeSections	{ $$ = $1; }
	;

AttributeSections
	: AttributeSection	{
				$$ = ILNode_List_create();
				if ($1)
				{
					ILNode_List_Add($$, $1);
				}
			}
	| AttributeSections AttributeSection	{
				$$ = $1;
				if ($2)
				{
					ILNode_List_Add($1, $2);
				}
			}
	;

AttributeSection
	: '[' AttributeList ']'		{ $$ = $2; }
	| '[' AttributeList ',' ']'	{ $$ = $2; }
	| DOC_COMMENT				{ $$ = 0; }
	| '[' error ']'		{
				/*
				 * This production recovers from errors in attributes.
				 */
				$$ = 0;
			}
	;

AttributeList
	: Attribute	{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| AttributeList ',' Attribute	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

Attribute
	: QualifiedIdentifier						{ 
				MakeBinary(Attribute, $1, 0);
			}
	| QualifiedIdentifier AttributeArguments	{ 
				MakeBinary(Attribute, $1, $2);
			}
	;

AttributeArguments
	: '(' PositionalArgumentList ')'			{
				MakeBinary(AttrArgs, $2, 0);
			}
	| '(' PositionalArgumentList ',' NamedArgumentList ')'	{
				MakeBinary(AttrArgs, $2, $4);
			}
	| '(' NamedArgumentList ')'	{
				MakeBinary(AttrArgs, 0, $2);
			}
	;

PositionalArgumentList
	: PositionalArgument		{
				$$ = ILNode_List_create ();
				ILNode_List_Add ($$, $1);
			}
	| PositionalArgumentList ',' PositionalArgument	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

PositionalArgument
	: AttributeArgumentExpression {$$ = $1;}
	;

NamedArgumentList
	: NamedArgument		{
				$$ = ILNode_List_create ();
				ILNode_List_Add($$, $1);
			}
	| NamedArgumentList ',' NamedArgument	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

NamedArgument
	: Identifier '=' AttributeArgumentExpression	{
				MakeBinary(NamedArg, $1, $3);
			}
	;

AttributeArgumentExpression
	: Expression			{ $$ = $1; }
	;

/*
 * Modifiers.
 */

OptModifiers
	: /* empty */			{ $$ = 0; }
	| Modifiers				{ $$ = $1; }
	;

Modifiers
	: Modifier				{ $$ = $1; }
	| Modifiers Modifier	{
				if(($1 & $2) != 0)
				{
					/* A modifier was used more than once in the list */
					CSModifiersUsedTwice($1 & $2);
				}
				$$ = ($1 | $2);
			}
	;

Modifier
	: NEW			{ $$ = CS_MODIFIER_NEW; }
	| PUBLIC		{ $$ = CS_MODIFIER_PUBLIC; }
	| PROTECTED		{ $$ = CS_MODIFIER_PROTECTED; }
	| INTERNAL		{ $$ = CS_MODIFIER_INTERNAL; }
	| PRIVATE		{ $$ = CS_MODIFIER_PRIVATE; }
	| ABSTRACT		{ $$ = CS_MODIFIER_ABSTRACT; }
	| SEALED		{ $$ = CS_MODIFIER_SEALED; }
	| STATIC		{ $$ = CS_MODIFIER_STATIC; }
	| READONLY		{ $$ = CS_MODIFIER_READONLY; }
	| VIRTUAL		{ $$ = CS_MODIFIER_VIRTUAL; }
	| OVERRIDE		{ $$ = CS_MODIFIER_OVERRIDE; }
	| EXTERN		{ $$ = CS_MODIFIER_EXTERN; }
	| UNSAFE		{ $$ = CS_MODIFIER_UNSAFE; }
	;

/*
 * Class declarations.
 */

ClassDeclaration
	: OptAttributes OptModifiers CLASS Identifier ClassBase {
				/* Enter a new nesting level */
				++NestingLevel;

				/* Push the identifier onto the class name stack */
				ClassNamePush($4);
			}
			ClassBody OptSemiColon	{
				/* Validate the modifiers */
				ILUInt32 attrs =
					CSModifiersToTypeAttrs($2, (NestingLevel > 1));

				/* Exit the current nesting level */
				--NestingLevel;

				/* Create the class definition */
				$$ = ILNode_ClassDefn_create
							($1,					/* OptAttributes */
							 attrs,					/* OptModifiers */
							 ILQualIdentName($4, 0),/* Identifier */
							 CurrNamespace.string,	/* Namespace */
							 $5,					/* ClassBase */
							 $7);					/* ClassBody */

				/* Pop the class name stack */
				ClassNamePop();

				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
			}
	;

ClassBase
	: /* empty */		{ $$ = 0; }
	| ':' TypeList		{ $$ = $2; }
	;

TypeList
	: Type					{ $$ = $1; }
	| TypeList ',' Type		{ MakeBinary(ArgList, $1, $3); }
	;

ClassBody
	: '{' OptClassMemberDeclarations '}'	{ $$ = $2; }
	| '{' error '}'		{
				/*
				 * This production recovers from errors in class bodies.
				 */
				$$ = 0;
			}
	;

OptClassMemberDeclarations
	: /* empty */					{ $$ = 0; }
	| ClassMemberDeclarations		{ $$ = $1; }
	;

ClassMemberDeclarations
	: ClassMemberDeclaration		{ $$ = $1; }
	| ClassMemberDeclarations ClassMemberDeclaration	{
				$$ = ILNode_Compound_CreateFrom($1, $2);
			}
	;

ClassMemberDeclaration
	: ConstantDeclaration		{ $$ = $1; }
	| FieldDeclaration			{ $$ = $1; }
	| MethodDeclaration			{ $$ = $1; }
	| PropertyDeclaration		{ $$ = $1; }
	| EventDeclaration			{ $$ = $1; }
	| IndexerDeclaration		{ $$ = $1; }
	| OperatorDeclaration		{ $$ = $1; }
	| ConstructorDeclaration	{ $$ = $1; }
	| DestructorDeclaration		{ $$ = $1; }
	| TypeDeclaration			{ $$ = $1; }
	;

/*
 * Constants.
 */

ConstantDeclaration
	: OptAttributes OptModifiers CONST Type ConstantDeclarators ';' {
				ILUInt32 attrs = CSModifiersToConstAttrs($2);
				$$ = ILNode_ConstDeclaration_create($1, attrs, $4, $5);
			}
	;

ConstantDeclarators
	: ConstantDeclarator							{
				$$ = ILNode_ConstDeclarators_create();
				ILNode_List_Add($$, $1);
			}
	| ConstantDeclarators ',' ConstantDeclarator    {
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

ConstantDeclarator
	: Identifier '=' ConstantExpression				{
				MakeBinary(ConstDeclarator, $1, $3);
			}
	;

/*
 * Fields.
 */

FieldDeclaration
	: OptAttributes OptModifiers Type VariableDeclarators ';'	{
				ILUInt32 attrs = CSModifiersToFieldAttrs($2);
				$$ = ILNode_FieldDeclaration_create($1, attrs, $3, $4);
			}
	;

/*
 * Methods.
 */

MethodDeclaration
	: OptAttributes OptModifiers Type QualifiedIdentifier
			'(' OptFormalParameterList ')' MethodBody	{
				ILUInt32 attrs = CSModifiersToMethodAttrs($2);
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $3, $4, $6, 0, $8);
			}
	| OptAttributes OptModifiers VOID QualifiedIdentifier
			'(' OptFormalParameterList ')' MethodBody	{
				ILUInt32 attrs = CSModifiersToMethodAttrs($2);
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, ILNode_VoidType_create(), $4, $6, 0, $8);
			}
	;

ReturnType
	: Type			{ $$ = $1; }
	| VOID			{ MakeSimple(VoidType); }
	;

MethodBody
	: Block			{ $$ = $1; }
	| ';'			{ $$ = 0; }
	;

OptFormalParameterList
	: /* empty */			{ MakeSimple(Empty); }
	| FormalParameterList	{ $$ = $1; }
	;

FormalParameterList
	: FormalParameter							{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| FormalParameterList ',' FormalParameter	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

FormalParameter
	: OptAttributes ParameterModifier Type Identifier		{
				$$ = ILNode_FormalParameter_create($1, $2, $3, $4);
			}
	;

ParameterModifier
	: /* empty */	{ $$ = ILParamMod_empty;}
	| REF			{ $$ = ILParamMod_ref;}
	| OUT			{ $$ = ILParamMod_out;}
	| PARAMS		{ $$ = ILParamMod_params;}
	;

/*
 * Properties.
 */

PropertyDeclaration
	: OptAttributes OptModifiers Type QualifiedIdentifier
			StartAccessorBlock AccessorBlock	{
				ILUInt32 attrs = CSModifiersToPropertyAttrs($2);
				$$ = ILNode_PropertyDeclaration_create($1,
								   attrs, $3, $4, 0, $6.item1, $6.item2,
								   (($6.item1 ? 1 : 0) |
								    ($6.item2 ? 2 : 0)));
			}
	;

StartAccessorBlock
	: '{'	{ CSGetSetKeywords = 1; }
	;

AccessorBlock
	: AccessorDeclarations '}'	{
				CSGetSetKeywords = 0;
				$$ = $1;
			}
	| error '}'		{
				/*
				 * This production recovers from errors in accessor blocks.
				 */
				CSGetSetKeywords = 0;
				$$.item1 = 0;
				$$.item2 = 0;
			}
	;

AccessorDeclarations
	: GetAccessorDeclaration OptSetAccessorDeclaration		{
				$$.item1 = $1; 
				$$.item2 = $2;
			}
	| SetAccessorDeclaration OptGetAccessorDeclaration		{
				$$.item1 = $2; 
				$$.item2 = $1;
			}
	;

OptGetAccessorDeclaration
	: /* empty */				{ $$ = 0; }
	| GetAccessorDeclaration	{ $$ = $1;}
	;

GetAccessorDeclaration
	: OptAttributes GET TurnOffGetSet AccessorBody TurnOnGetSet		{
				MakeBinary(AccessorDeclaration, $1, $4);
			}
	;

OptSetAccessorDeclaration
	: /* empty */				{ $$ = 0; }
	| SetAccessorDeclaration	{ $$ = $1; }
	;

SetAccessorDeclaration
	: OptAttributes SET TurnOffGetSet AccessorBody TurnOnGetSet		{
				MakeBinary(AccessorDeclaration, $1, $4);
			}
	;

AccessorBody
	: Block				{ $$ = $1;}
	| ';'				{ MakeSimple(Empty);}
	;

TurnOffGetSet
	: /* empty */		{ CSGetSetKeywords = 0; }
	;

TurnOnGetSet
	: /* empty */		{ CSGetSetKeywords = 1; }
	;

/*
 * Events.
 */

EventDeclaration
	: EventFieldDeclaration
	| EventPropertyDeclaration
	;

EventFieldDeclaration
	: OptAttributes OptModifiers EVENT Type VariableDeclarators ';'	{
				ILUInt32 attrs = CSModifiersToEventAttrs($2);
				$$ = ILNode_EventFieldDeclaration_create($1, attrs, $4, $5);
			}
	;

EventPropertyDeclaration
	: OptAttributes OptModifiers EVENT Type QualifiedIdentifier
			StartAccessorBlock EventAccessorBlock	{
				ILUInt32 attrs = CSModifiersToEventAttrs($2);
				$$ = ILNode_EventPropertyDeclaration_create($1, 
									attrs, $4, $5, $7.item1, $7.item2);
			}
	;

EventAccessorBlock
	: EventAccessorDeclarations '}'	{
				CSGetSetKeywords = 0;
				$$ = $1;
			}
	| error '}'		{
				/*
				 * This production recovers from errors in accessor blocks.
				 */
				CSGetSetKeywords = 0;
				$$.item1 = 0;
				$$.item2 = 0;
			}
	;

EventAccessorDeclarations
	: AddAccessorDeclaration OptRemoveAccessorDeclaration {
				$$.item1 = $1;
				$$.item2 = $2;
			}
	| RemoveAccessorDeclaration OptAddAccessorDeclaration {
				$$.item1 = $2;
				$$.item2 = $1;
			}
	;

OptAddAccessorDeclaration
	: /* empty */				{ $$ = 0; }
	| AddAccessorDeclaration	{ $$ = $1; }
	;

AddAccessorDeclaration
	: OptAttributes ADD TurnOffGetSet AccessorBody TurnOnGetSet		{
				MakeBinary(AccessorDeclaration, $1, $4);
			}
	;

OptRemoveAccessorDeclaration
	: /* empty */				{ $$ = 0; }
	| RemoveAccessorDeclaration	{ $$ = $1; }
	;

RemoveAccessorDeclaration
	: OptAttributes REMOVE TurnOffGetSet AccessorBody TurnOnGetSet	{
				MakeBinary(AccessorDeclaration, $1, $4);
			}
	;

/*
 * Indexers.
 */

IndexerDeclaration
	: OptAttributes OptModifiers IndexerDeclarator
			StartAccessorBlock AccessorBlock		{
				ILUInt32 attrs = CSModifiersToPropertyAttrs($2);
				$$ = ILNode_PropertyDeclaration_create($1,
								   attrs, $3.type, $3.ident, $3.params,
								   $5.item1, $5.item2,
								   (($5.item1 ? 1 : 0) |
								    ($5.item2 ? 2 : 0)));
			}
	;

IndexerDeclarator
	: Type THIS FormalIndexParameters		{
				$$.type = $1;
				$$.ident = ILQualIdentSimple
								(ILInternString("Item", 4).string);
				$$.params = $3;
			}
	| Type QualifiedIdentifier '.' THIS FormalIndexParameters	{
				$$.type = $1;
				$$.ident = ILNode_QualIdent_create($2,
							ILQualIdentSimple
								(ILInternString("Item", 4).string));
				$$.params = $5;
			}
	;

FormalIndexParameters
	: '[' FormalIndexParameterList ']'		{ $$ = $2; }
	| '[' error ']'		{
				/*
				 * This production recovers from errors in indexer parameters.
				 */
				$$ = 0;
			}
	;

FormalIndexParameterList
	: FormalIndexParameter								{
				$$ = ILNode_List_create ();
				ILNode_List_Add($$, $1);
			}
	| FormalIndexParameterList ',' FormalIndexParameter	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

FormalIndexParameter
	: OptAttributes Type Identifier 					{
				$$ = ILNode_FormalParameter_create($1, ILParamMod_empty, $2, $3);
			}
	;

/*
 * Operators.
 */

OperatorDeclaration
	: NormalOperatorDeclaration		{ $$ = $1; }
	| ConversionOperatorDeclaration	{ $$ = $1; }
	;

NormalOperatorDeclaration
	: OptAttributes OptModifiers Type OPERATOR OverloadableOperator
			'(' Type Identifier ')'	Block {
				ILUInt32 attrs;
				ILNode *params;

				/* Validate the name of the unary operator */
				if($5.unary == 0)
				{
					CSError("overloadable unary operator expected");
					$5.unary = $5.binary;
				}

				/* Get the operator attributes */
				attrs = CSModifiersToOperatorAttrs($2);

				/* Build the formal parameter list */
				params = ILNode_List_create();
				ILNode_List_Add(params,
					ILNode_FormalParameter_create(0, ILParamMod_empty, $7, $8));

				/* Create a method definition for the operator */
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $3,
						 ILQualIdentSimple(ILInternString($5.unary, -1).string),
						 params, 0, $10);
			}
	| OptAttributes OptModifiers Type OPERATOR OverloadableOperator
			'(' Type Identifier ',' Type Identifier ')' Block	{
				ILUInt32 attrs;
				ILNode *params;

				/* Validate the name of the binary operator */
				if($5.binary == 0)
				{
					CSError("overloadable binary operator expected");
					$5.binary = $5.unary;
				}

				/* Get the operator attributes */
				attrs = CSModifiersToOperatorAttrs($2);

				/* Build the formal parameter list */
				params = ILNode_List_create();
				ILNode_List_Add(params,
					ILNode_FormalParameter_create
						(0, ILParamMod_empty, $7, $8));
				ILNode_List_Add(params,
					ILNode_FormalParameter_create
						(0, ILParamMod_empty, $10, $11));

				/* Create a method definition for the operator */
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $3,
						 ILQualIdentSimple
						 	(ILInternString($5.binary, -1).string),
						 params, 0, $13);
			}
	;

OverloadableOperator
	: '+'		{ $$.binary = "op_Addition"; $$.unary = "op_UnaryPlus"; }
	| '-'		{ $$.binary = "op_Subtraction"; $$.unary = "op_UnaryNegation"; }
	| '!'		{ $$.binary = 0; $$.unary = "op_LogicalNot"; }
	| '~'		{ $$.binary = 0; $$.unary = "op_OnesComplement"; }
	| INC_OP	{ $$.binary = 0; $$.unary = "op_Increment"; }
	| DEC_OP	{ $$.binary = 0; $$.unary = "op_Decrement"; }
	| TRUE		{ $$.binary = 0; $$.unary = "op_True"; }
	| FALSE		{ $$.binary = 0; $$.unary = "op_False"; }
	| '*'		{ $$.binary = "op_Multiply"; $$.unary = 0; }
	| '/'		{ $$.binary = "op_Division"; $$.unary = 0; }
	| '%'		{ $$.binary = "op_Modulus"; $$.unary = 0; }
	| '&'		{ $$.binary = "op_BitwiseAnd"; $$.unary = 0; }
	| '|'		{ $$.binary = "op_BitwiseOr"; $$.unary = 0; }
	| '^'		{ $$.binary = "op_ExclusiveOr"; $$.unary = 0; }
	| LEFT_OP	{ $$.binary = "op_LeftShift"; $$.unary = 0; }
	| RIGHT_OP	{ $$.binary = "op_RightShift"; $$.unary = 0; }
	| EQ_OP		{ $$.binary = "op_Equality"; $$.unary = 0; }
	| NE_OP		{ $$.binary = "op_Inequality"; $$.unary = 0; }
	| '>'		{ $$.binary = "op_GreaterThan"; $$.unary = 0; }
	| '<'		{ $$.binary = "op_LessThan"; $$.unary = 0; }
	| GE_OP		{ $$.binary = "op_GreaterThanOrEqual"; $$.unary = 0; }
	| LE_OP		{ $$.binary = "op_LessThanOrEqual"; $$.unary = 0; }
	;

ConversionOperatorDeclaration
	: OptAttributes OptModifiers IMPLICIT OPERATOR Type
			'(' Type Identifier ')' Block	{
				ILUInt32 attrs;
				ILNode *params;

				/* Get the operator attributes */
				attrs = CSModifiersToOperatorAttrs($2);

				/* Build the formal parameter list */
				params = ILNode_List_create();
				ILNode_List_Add(params,
					ILNode_FormalParameter_create(0, ILParamMod_empty, $7, $8));

				/* Create a method definition for the operator */
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $5,
						 ILQualIdentSimple
						 	(ILInternString("op_Implicit", -1).string),
						 params, 0, $10);
			}
	| OptAttributes OptModifiers EXPLICIT OPERATOR Type
			'(' Type Identifier ')' Block	{
				ILUInt32 attrs;
				ILNode *params;

				/* Get the operator attributes */
				attrs = CSModifiersToOperatorAttrs($2);

				/* Build the formal parameter list */
				params = ILNode_List_create();
				ILNode_List_Add(params,
					ILNode_FormalParameter_create(0, ILParamMod_empty, $7, $8));

				/* Create a method definition for the operator */
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $5,
						 ILQualIdentSimple
						 	(ILInternString("op_Explicit", -1).string),
						 params, 0, $10);
			}
	;

/*
 * Constructors and destructors.
 */

ConstructorDeclaration
	: OptAttributes OptModifiers Identifier '(' OptFormalParameterList ')'
			ConstructorInitializer MethodBody	{
				ILUInt32 attrs = CSModifiersToConstructorAttrs($2);
				ILNode *cname;
				if((attrs & IL_META_METHODDEF_STATIC) != 0)
				{
					cname = ILQualIdentSimple
								(ILInternString(".cctor", 6).string);
				}
				else
				{
					cname = ILQualIdentSimple
								(ILInternString(".ctor", 5).string);
				}
				if(!ClassNameSame($3))
				{
					CSErrorOnLine(yygetfilename($3), yygetlinenum($3),
						"constructor name does not match class name");
				}
				$$ = ILNode_MethodDeclaration_create
					  ($1, attrs, ILNode_VoidType_create(), cname, $5, $7, $8);
			}
	;

ConstructorInitializer
	: /* empty */							{ $$ = 0; }
	| ':' BASE '(' OptArgumentList ')'		{
				MakeBinary(InvocationExpression, ILNode_BaseInit_create(), $4);
			}
	| ':' THIS '(' OptArgumentList ')'		{
				MakeBinary(InvocationExpression, ILNode_ThisInit_create(), $4);
			}
	;

DestructorDeclaration
	: OptAttributes '~' Identifier '(' ')' Block		{
				ILUInt32 attrs;
				ILNode *name;
				ILNode *body;

				/* Validate the destructor name */
				if(!ClassNameSame($3))
				{
					CSErrorOnLine(yygetfilename($3), yygetlinenum($3),
						"destructor name does not match class name");
				}

				/* Build the list of attributes needed on "Finalize" */
				attrs = IL_META_METHODDEF_FAMILY |
						IL_META_METHODDEF_HIDE_BY_SIG |
						IL_META_METHODDEF_VIRTUAL;

				/* Build the name of the "Finalize" method */
				name = ILQualIdentSimple(ILInternString("Finalize", -1).string);

				/* Destructors must always call their parent finalizer
				   even if an exception occurs.  We force this to happen
				   by wrapping the method body with a try block whose
				   finally clause always calls its parent */
				body = ILNode_InvocationExpression_create
							(ILNode_BaseAccess_create(name), 0);
				body = ILNode_Try_create
							($6, 0, ILNode_FinallyClause_create(body));

				/* Construct the finalizer declaration */
				$$ = ILNode_MethodDeclaration_create
							($1, attrs, ILNode_VoidType_create(),
							 ILQualIdentSimple
							 	(ILInternString("Finalize", -1).string),
							 0, 0, body);
			}
	;

/*
 * Structs.
 */

StructDeclaration
	: OptAttributes OptModifiers STRUCT Identifier StructInterfaces	{
				/* Enter a new nesting level */
				++NestingLevel;

				/* Push the identifier onto the class name stack */
				ClassNamePush($4);
			}
			StructBody OptSemiColon	{
				ILNode *baseList;
				ILUInt32 attrs;

				/* Validate the modifiers */
				attrs = CSModifiersToTypeAttrs($2, (NestingLevel > 1));

				/* Add extra attributes that structs need */
				attrs |= IL_META_TYPEDEF_VALUE_TYPE |
						 IL_META_TYPEDEF_LAYOUT_SEQUENTIAL |
						 IL_META_TYPEDEF_SERIALIZABLE |
						 IL_META_TYPEDEF_SEALED;

				/* Exit the current nesting level */
				--NestingLevel;

				/* Make sure that we have "ValueType" in the base list */
				baseList = MakeSystemType("ValueType");
				if($5 != 0)
				{
					baseList = ILNode_ArgList_create($5, baseList);
				}

				/* Create the class definition */
				$$ = ILNode_ClassDefn_create
							($1,					/* OptAttributes */
							 attrs,					/* OptModifiers */
							 ILQualIdentName($4, 0),/* Identifier */
							 CurrNamespace.string,	/* Namespace */
							 baseList,				/* ClassBase */
							 $7);					/* StructBody */

				/* Pop the class name stack */
				ClassNamePop();

				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
			}
	;

StructInterfaces
	: /* empty */			{ $$ = 0; }
	| ':' TypeList			{ $$ = $2; }
	;

StructBody
	: '{' OptClassMemberDeclarations '}'	{ $$ = $2; }
	| '{' error '}'		{
				/*
				 * This production recovers from errors in struct declarations.
				 */
				$$ = 0;
			}
	;

/*
 * Interfaces.
 */

InterfaceDeclaration
	: OptAttributes OptModifiers INTERFACE Identifier InterfaceBase	{
				/* Increase the nesting level */
				++NestingLevel;

				/* Push the identifier onto the class name stack */
				ClassNamePush($4);
			}
			InterfaceBody OptSemiColon	{
				/* Validate the modifiers */
				ILUInt32 attrs = CSModifiersToTypeAttrs($2, (NestingLevel > 1));

				/* Add extra attributes that interfaces need */
				attrs |= IL_META_TYPEDEF_INTERFACE;

				/* Exit from the current nesting level */
				--NestingLevel;

				/* Create the interface definition */
				$$ = ILNode_ClassDefn_create
							($1,					/* OptAttributes */
							 attrs,					/* OptModifiers */
							 ILQualIdentName($4, 0),/* Identifier */
							 CurrNamespace.string,	/* Namespace */
							 $5,					/* ClassBase */
							 $7);					/* ClassBody */

				/* Pop the class name stack */
				ClassNamePop();

				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
			}
	;

InterfaceBase
	: /* empty */	{ $$ = 0; }
	| ':' TypeList	{ $$ = $2; }
	;

InterfaceBody
	: '{' OptInterfaceMemberDeclarations '}'		{ $$ = $2;}
	| '{' error '}'		{
				/*
				 * This production recovers from errors in interface
				 * declarations.
				 */
				$$ = 0;
			}
	;

OptInterfaceMemberDeclarations
	: /* empty */						{ $$ = 0;}
	| InterfaceMemberDeclarations		{ $$ = $1;}
	;

InterfaceMemberDeclarations
	: InterfaceMemberDeclaration		{ $$ = $1; }
	| InterfaceMemberDeclarations InterfaceMemberDeclaration	{
				$$ = ILNode_Compound_CreateFrom($1, $2);
			}
	;

InterfaceMemberDeclaration
	: InterfaceMethodDeclaration		{ $$ = $1;}
	| InterfacePropertyDeclaration		{ $$ = $1;}
	| InterfaceEventDeclaration			{ $$ = $1;}
	| InterfaceIndexerDeclaration		{ $$ = $1;}
	;

InterfaceMethodDeclaration
	: OptAttributes OptNew Type Identifier '(' OptFormalParameterList ')' ';' {
				ILUInt32 attrs = ($2 ? CS_SPECIALATTR_NEW : 0) |
								 IL_META_METHODDEF_PUBLIC |
								 IL_META_METHODDEF_VIRTUAL |
								 IL_META_METHODDEF_ABSTRACT |
								 IL_META_METHODDEF_HIDE_BY_SIG;
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $3, $4, $6, 0, 0);
			}
	| OptAttributes OptNew VOID Identifier '(' OptFormalParameterList ')' ';' {
				ILUInt32 attrs = ($2 ? CS_SPECIALATTR_NEW : 0) |
								 IL_META_METHODDEF_PUBLIC |
								 IL_META_METHODDEF_VIRTUAL |
								 IL_META_METHODDEF_ABSTRACT |
								 IL_META_METHODDEF_HIDE_BY_SIG;
				$$ = ILNode_MethodDeclaration_create($1, attrs,
							ILNode_VoidType_create(), $4, $6, 0, 0);
			}
	;

OptNew
	: /* empty */	{ $$ = 0; }
	| NEW 			{ $$ = 1; }
	;

InterfacePropertyDeclaration
	: OptAttributes OptNew Type Identifier
			StartInterfaceAccessorBody InterfaceAccessorBody	{
				ILUInt32 attrs = ($2 ? CS_SPECIALATTR_NEW : 0) |
								 IL_META_METHODDEF_PUBLIC |
								 IL_META_METHODDEF_VIRTUAL |
								 IL_META_METHODDEF_ABSTRACT |
								 IL_META_METHODDEF_HIDE_BY_SIG;
				$$ = ILNode_PropertyDeclaration_create
								($1, attrs, $3, 0, $4, 0, 0, $6);
			}
	;

StartInterfaceAccessorBody
	: '{'		{ CSGetSetKeywords = 1; }
	;

InterfaceAccessorBody
	: InterfaceAccessors '}'	{
				CSGetSetKeywords = 0;
				$$ = $1;
			}
	| error '}'		{
				/*
				 * This production recovers from errors in interface
				 * accessor declarations.
				 */
				CSGetSetKeywords = 0;
				$$ = 0;
			}
	;

InterfaceAccessors
	: GET ';'				{ $$ = 1; }
	| SET ';'				{ $$ = 2; }
	| GET ';' SET ';'		{ $$ = 3; }
	| SET ';' GET ';'		{ $$ = 3; }
	;

InterfaceEventDeclaration
	: OptAttributes OptNew EVENT Type Identifier ';'		{
				ILUInt32 attrs = ($2 ? CS_SPECIALATTR_NEW : 0) |
								 IL_META_METHODDEF_PUBLIC |
								 IL_META_METHODDEF_VIRTUAL |
								 IL_META_METHODDEF_ABSTRACT |
								 IL_META_METHODDEF_HIDE_BY_SIG;
				$$ = ILNode_EventPropertyDeclaration_create
							($1, attrs, $4, $5, 0, 0);
			}
	;

InterfaceIndexerDeclaration
	: OptAttributes OptNew Type THIS FormalIndexParameters
			StartInterfaceAccessorBody InterfaceAccessorBody	{
				ILUInt32 attrs = ($2 ? CS_SPECIALATTR_NEW : 0) |
								 IL_META_METHODDEF_PUBLIC |
								 IL_META_METHODDEF_VIRTUAL |
								 IL_META_METHODDEF_ABSTRACT |
								 IL_META_METHODDEF_HIDE_BY_SIG;
				ILNode *name = ILQualIdentSimple
									(ILInternString("Item", 4).string);
				$$ = ILNode_PropertyDeclaration_create
								($1, attrs, $3, name, $5, 0, 0, $7);
			}
	;

/*
 * Enums.
 */

EnumDeclaration
	: OptAttributes OptModifiers ENUM Identifier {
				ILUInt32 attrs =
					CSModifiersToTypeAttrs($2, (NestingLevel > 0));
				$2 = attrs;
			}
			EnumBase EnumBody OptSemiColon	{
				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
				$$ = ILNode_EnumDeclaration_create ($1, $2, $4, $6, $7);
			}
	;

EnumBase
	: /* empty */	{ $$ = 0; }
	| ':' Type		{ $$ = $2; }
	;

EnumBody
	: '{' OptEnumMemberDeclarations OptComma '}'				{
				$$ = $2;
			}
	| '{' error '}'		{
				/*
				 * This production recovers from errors in enum declarations.
				 */
				$$ = 0;
			}
	;

OptEnumMemberDeclarations
	: /* empty */				{ $$ = 0;}
	| EnumMemberDeclarations	{ $$ = $1;}
	;

EnumMemberDeclarations
	: EnumMemberDeclaration		{
			$$ = ILNode_List_create ();
			ILNode_List_Add($$, $1);
		}
	| EnumMemberDeclarations ',' EnumMemberDeclaration	{
			ILNode_List_Add($1, $3);
			$$ = $1;
		}
	;

EnumMemberDeclaration
	: OptAttributes Identifier		{
			$$ = ILNode_EnumMemberDeclaration_create($1, $2, 0);
		}
	| OptAttributes Identifier '=' ConstantExpression	{
			$$ = ILNode_EnumMemberDeclaration_create($1, $2, $4);
		}
	;

/*
 * Delegates.
 */

DelegateDeclaration
	: OptAttributes OptModifiers DELEGATE ReturnType Identifier
				'(' OptFormalParameterList ')' ';'	{
				ILUInt32 attrs =
					CSModifiersToTypeAttrs($2, (NestingLevel > 0));

				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
				$$ = ILNode_DelegateDeclaration_create($1, attrs, $4, $5, $7);
			}
	;
