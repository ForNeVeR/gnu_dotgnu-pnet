%{
/*
 * c_grammar.y - Input file for yacc that defines the syntax of the C language.
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

/* Rename the lex/yacc symbols to support multiple parsers */
#include "c_rename.h"

#include <stdio.h>
#include <cscc/c/c_internal.h>

#define	YYERROR_VERBOSE

/*
 * Current context.
 */
static char *functionName = "";
static ILType *currentStruct = 0;
static ILType *currentEnum = 0;
static ILInt32 currentEnumValue = 0;

/*
 * Imports from the lexical analyser.
 */
extern int yylex(void);
#ifdef YYTEXT_POINTER
extern char *c_text;
#else
extern char c_text[];
#endif

static void yyerror(char *msg)
{
	CCPluginParseError(msg, c_text);
}

%}

/*
 * Define the structure of yylval.
 */
%union {

	CLexIntConst		integer;
	CLexFloatConst		real;
	ILIntString			string;
	char	           *name;
	ILNode             *node;
	ILType			   *type;
	CDeclSpec			declSpec;
	int					kind;
	struct {
		ILType		   *type;
		ILType		   *parent;
	}					structInfo;

}

/*
 * Basic lexical values.
 */
%token IDENTIFIER		"an identifier"
%token INTEGER_CONSTANT	"an integer value"
%token FLOAT_CONSTANT	"a floating point value"
%token STRING_LITERAL	"a string literal"
%token TYPE_NAME		"a type identifier"

/*
 * Operators.
 */
%token PTR_OP			"`->'"
%token INC_OP			"`++'"
%token DEC_OP			"`--'"
%token LEFT_OP			"`<<'"
%token RIGHT_OP			"`>>'"
%token LE_OP			"`<='"
%token GE_OP			"`>='"
%token EQ_OP			"`=='"
%token NE_OP			"`!='"
%token AND_OP			"`&&'"
%token OR_OP			"`||'"
%token MUL_ASSIGN_OP	"`*='"
%token DIV_ASSIGN_OP	"`/='"
%token MOD_ASSIGN_OP	"`%='"
%token ADD_ASSIGN_OP	"`+='"
%token SUB_ASSIGN_OP	"`-='"
%token LEFT_ASSIGN_OP	"`<<='"
%token RIGHT_ASSIGN_OP	"`>>='"
%token AND_ASSIGN_OP	"`&='"
%token XOR_ASSIGN_OP	"`^='"
%token OR_ASSIGN_OP		"`|='"

/*
 * Reserved words.
 */
%token K_ASM			"`asm'"
%token K_AUTO			"`auto'"
%token K_BREAK			"`break'"
%token K_CASE			"`case'"
%token K_CHAR			"`char'"
%token K_CONST			"`const'"
%token K_CONTINUE		"`continue'"
%token K_DEFAULT		"`default'"
%token K_DO				"`do'"
%token K_DOUBLE			"`double'"
%token K_ELSE			"`else'"
%token K_ENUM			"`enum'"
%token K_EXTERN			"`extern'"
%token K_FLOAT			"`float'"
%token K_FOR			"`for'"
%token K_GOTO			"`goto'"
%token K_IF				"`if'"
%token K_INLINE			"`inline'"
%token K_INT			"`int'"
%token K_LONG			"`long'"
%token K_NATIVE			"`__native__'"
%token K_REGISTER		"`register'"
%token K_RETURN			"`return'"
%token K_SHORT			"`short'"
%token K_SIGNED			"`signed'"
%token K_SIZEOF			"`sizeof'"
%token K_STATIC			"`static'"
%token K_STRUCT			"`struct'"
%token K_SWITCH			"`switch'"
%token K_TYPEDEF		"`typedef'"
%token K_TYPEOF			"`typeof'"
%token K_UNION			"`union'"
%token K_UNSIGNED		"`unsigned'"
%token K_VOID			"`void'"
%token K_VOLATILE		"`volatile'"
%token K_WHILE			"`while'"
%token K_ELIPSIS		"`...'"
%token K_VA_LIST		"`__builtin_va_list'"
%token K_VA_START		"`__builtin_va_start'"
%token K_VA_ARG			"`__builtin_va_arg'"
%token K_VA_END			"`__builtin_va_end'"
%token K_JMP_BUF		"`__builtin_jmp_buf'"
%token K_SETJMP			"`__builtin_setjmp'"
%token K_LONGJMP		"`__builtin_longjmp'"
%token K_ALLOCA			"`__builtin_alloca'"
%token K_ATTRIBUTE		"`__attribute__'"
%token K_BOOL			"`__bool__'"
%token K_WCHAR			"`__wchar__'"
%token K_NINT			"`__nint__'"
%token K_TRUE			"`__true__'"
%token K_FALSE			"`__false__'"
%token K_FUNCTION		"`__FUNCTION__'"
%token K_FUNC			"`__func__'"

/*
 * Define the yylval types of the various non-terminals.
 */
%type <name>		IDENTIFIER TYPE_NAME
%type <integer>		INTEGER_CONSTANT
%type <real>		FLOAT_CONSTANT
%type <string>		STRING_LITERAL StringLiteral

%type <name>		AnyIdentifier Identifier

%type <node>		PrimaryExpression PostfixExpression PostfixExpression2
%type <node>		ArgumentExpressionList UnaryExpression CastExpression
%type <node>		MultiplicativeExpression AdditiveExpression ShiftExpression
%type <node>		RelationalExpression EqualityExpression AndExpression
%type <node>		XorExpression OrExpression LogicalAndExpression
%type <node>		LogicalOrExpression ConditionalExpression
%type <node>		AssignmentExpression Expression ConstantExpression
%type <node>		BoolExpression ConstantAttributeExpression
%type <node>		AttributeArgs

%type <node>		Statement Statement2 LabeledStatement CompoundStatement
%type <node>		OptDeclarationList DeclarationList Declaration
%type <node>		OptStatementList StatementList ExpressionStatement
%type <node>		SelectionStatement IterationStatement JumpStatement

%type <node>		FunctionBody

%type <type>		TypeName StructOrUnionSpecifier EnumSpecifier

%type <declSpec>	StorageClassSpecifier TypeSpecifier DeclarationSpecifiers

%type <kind>		StructOrUnion

%expect 1

%start File
%%

AnyIdentifier
	: IDENTIFIER
	| TYPE_NAME
	;

Identifier
	: IDENTIFIER
	;

PrimaryExpression
	: Identifier			{ /* TODO: resolve the identifier to a node */ }
	| INTEGER_CONSTANT		{
				/* Convert the integer value into a node */
				switch($1.type)
				{
					case ILMachineType_Int8:
					{
						$$ = ILNode_Int8_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_UInt8:
					{
						$$ = ILNode_UInt8_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_Int16:
					{
						$$ = ILNode_Int16_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_UInt16:
					{
						$$ = ILNode_UInt16_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_Int32:
					{
						$$ = ILNode_Int32_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_UInt32:
					{
						$$ = ILNode_UInt32_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_Int64:
					{
						$$ = ILNode_Int64_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_UInt64:
					{
						$$ = ILNode_UInt64_create($1.value, $1.isneg, 1);
					}
					break;

					default:
					{
						/* Shouldn't happen */
						$$ = ILNode_Error_create();
					}
					break;
				}
			}
	| FLOAT_CONSTANT		{
				/* Convert the floating-point value into a node */
				/* TODO */
			}
	| K_TRUE				{ $$ = ILNode_True_create(); }
	| K_FALSE				{ $$ = ILNode_False_create(); }
	| StringLiteral			{
				$$ = ILNode_CString_create($1.string, $1.len);
			}
	| K_FUNC				{
				/* Create a reference to the local "__func__" array */
				/* TODO */
				$$ = ILNode_Error_create();
			}
	| '(' Expression ')'		{ $$ = $2; }
	| '(' CompoundStatement ')'	{ $$ = $2; }
	;

StringLiteral
	: STRING_LITERAL	{ $$ = $1; }
	| K_FUNCTION		{
				$$ = ILInternString(functionName, strlen(functionName));
			}
	| StringLiteral STRING_LITERAL	{
				$$ = ILInternAppendedString($1, $2);
			}
	| StringLiteral K_FUNCTION		{
				$$ = ILInternAppendedString
					($1, ILInternString(functionName, strlen(functionName)));
			}
	;

PostfixExpression
	: PostfixExpression2
	;

PostfixExpression2
	: PrimaryExpression
	| PostfixExpression '[' Expression ']'	{
				$$ = ILNode_ArrayAccess_create($1, $3);
			}
	| PostfixExpression2 '(' ')'	{
				$$ = ILNode_InvocationExpression_create($1, 0);
			}
	| PostfixExpression2 '(' ArgumentExpressionList ')'	{
				$$ = ILNode_InvocationExpression_create($1, $3);
			}
	| PostfixExpression '.' AnyIdentifier	{
				$$ = ILNode_MemberAccess_create($1, ILQualIdentSimple($3));
			}
	| PostfixExpression PTR_OP AnyIdentifier	{
				$$ = ILNode_DerefField_create($1, ILQualIdentSimple($3));
			}
	| PostfixExpression INC_OP		{
				$$ = ILNode_PostInc_create($1);
			}
	| PostfixExpression DEC_OP		{
				$$ = ILNode_PostDec_create($1);
			}
	| K_VA_ARG '(' UnaryExpression ',' TypeName ')'	{
				/* TODO */
				$$ = ILNode_Error_create();
			}
	| K_VA_START '(' UnaryExpression ',' Identifier ')'	{
				/* TODO */
				$$ = ILNode_Error_create();
			}
	| K_VA_START '(' UnaryExpression ')'	{
				/* TODO */
				$$ = ILNode_Error_create();
			}
	| K_VA_END '(' UnaryExpression ')'	{
				/* TODO */
				$$ = ILNode_Error_create();
			}
	| K_SETJMP '(' UnaryExpression ')'	{
				/* TODO */
				$$ = ILNode_Error_create();
			}
	| K_LONGJMP '(' AssignmentExpression ',' AssignmentExpression ')'	{
				/* TODO */
				$$ = ILNode_Error_create();
			}
	| K_ALLOCA '(' AssignmentExpression ')'	{
				/* TODO */
				$$ = ILNode_Error_create();
			}
	;

ArgumentExpressionList
	: AssignmentExpression
	| ArgumentExpressionList ',' AssignmentExpression	{
				$$ = ILNode_ArgList_create($1, $3);
			}
	;

UnaryExpression
	: PostfixExpression
	| INC_OP UnaryExpression	{ $$ = ILNode_PreInc_create($2); }
	| DEC_OP UnaryExpression	{ $$ = ILNode_PreDec_create($2); }
	| '-' CastExpression		{
				/* Negate simple integer and floating point values in-place */
				$$ = ILNode_Neg_create($2);
			}
	| '+' CastExpression		{ $$ = ILNode_UnaryPlus_create($2); }
	| '~' CastExpression		{ $$ = ILNode_Not_create($2); }
	| '!' CastExpression		{ $$ = ILNode_LogicalNot_create($2); }
	| '&' CastExpression		{ $$ = ILNode_AddressOf_create($2); }
	| '*' CastExpression		{ $$ = ILNode_Deref_create($2); }
	| K_SIZEOF UnaryExpression	{ $$ = ILNode_SizeOfExpr_create($2); }
	| K_SIZEOF '(' TypeName ')'	{ $$ = ILNode_SizeOfType_create($3); }
	;

CastExpression
	: UnaryExpression
	| '(' TypeName ')' CastExpression	{
				$$ = ILNode_CastType_create($4, $2);
			}
	;

MultiplicativeExpression
	: CastExpression
	| MultiplicativeExpression '*' CastExpression	{
				$$ = ILNode_Mul_create($1, $3);
			}
	| MultiplicativeExpression '/' CastExpression	{
				$$ = ILNode_Div_create($1, $3);
			}
	| MultiplicativeExpression '%' CastExpression	{
				$$ = ILNode_Rem_create($1, $3);
			}
	;

AdditiveExpression
	: MultiplicativeExpression
	| AdditiveExpression '+' MultiplicativeExpression	{
				$$ = ILNode_Add_create($1, $3);
			}
	| AdditiveExpression '-' MultiplicativeExpression	{
				$$ = ILNode_Sub_create($1, $3);
			}
	;

ShiftExpression
	: AdditiveExpression
	| ShiftExpression LEFT_OP AdditiveExpression	{
				$$ = ILNode_Shl_create($1, $3);
			}
	| ShiftExpression RIGHT_OP AdditiveExpression	{
				$$ = ILNode_Shr_create($1, $3);
			}
	;

RelationalExpression
	: ShiftExpression
	| RelationalExpression '<' ShiftExpression	{
				$$ = ILNode_Lt_create($1, $3);
			}
	| RelationalExpression '>' ShiftExpression	{
				$$ = ILNode_Gt_create($1, $3);
			}
	| RelationalExpression LE_OP ShiftExpression	{
				$$ = ILNode_Le_create($1, $3);
			}
	| RelationalExpression GE_OP ShiftExpression	{
				$$ = ILNode_Ge_create($1, $3);
			}
	;

EqualityExpression
	: RelationalExpression
	| EqualityExpression EQ_OP RelationalExpression	{
				$$ = ILNode_Eq_create($1, $3);
			}
	| EqualityExpression NE_OP RelationalExpression	{
				$$ = ILNode_Ne_create($1, $3);
			}
	;

AndExpression
	: EqualityExpression
	| AndExpression '&' EqualityExpression	{
				$$ = ILNode_And_create($1, $3);
			}
	;

XorExpression
	: AndExpression
	| XorExpression '^' AndExpression	{
				$$ = ILNode_Xor_create($1, $3);
			}
	;

OrExpression
	: XorExpression
	| OrExpression '|' XorExpression	{
				$$ = ILNode_Or_create($1, $3);
			}
	;

LogicalAndExpression
	: OrExpression
	| LogicalAndExpression AND_OP OrExpression	{
				$$ = ILNode_LogicalAnd_create($1, $3);
			}
	;

LogicalOrExpression
	: LogicalAndExpression
	| LogicalOrExpression OR_OP LogicalAndExpression	{
				$$ = ILNode_LogicalOr_create($1, $3);
			}
	;

ConditionalExpression
	: LogicalOrExpression
	| LogicalOrExpression '?' LogicalOrExpression ':' ConditionalExpression	{
				$$ = ILNode_Conditional_create($1, $3, $5);
			}
	| LogicalOrExpression '?' ':' ConditionalExpression	{
				$$ = ILNode_TwoConditional_create($1, $4);
			}
	;

AssignmentExpression
	: ConditionalExpression
	| UnaryExpression '=' AssignmentExpression	{
				$$ = ILNode_Assign_create($1, $3);
			}
	| UnaryExpression MUL_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignMul_create(ILNode_Mul_create($1, $3));
			}
	| UnaryExpression DIV_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignDiv_create(ILNode_Div_create($1, $3));
			}
	| UnaryExpression MOD_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignRem_create(ILNode_Rem_create($1, $3));
			}
	| UnaryExpression ADD_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignAdd_create(ILNode_Add_create($1, $3));
			}
	| UnaryExpression SUB_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignSub_create(ILNode_Sub_create($1, $3));
			}
	| UnaryExpression LEFT_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignShl_create(ILNode_Shl_create($1, $3));
			}
	| UnaryExpression RIGHT_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignShr_create(ILNode_Shr_create($1, $3));
			}
	| UnaryExpression AND_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignAnd_create(ILNode_And_create($1, $3));
			}
	| UnaryExpression XOR_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignXor_create(ILNode_Xor_create($1, $3));
			}
	| UnaryExpression OR_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignOr_create(ILNode_Or_create($1, $3));
			}
	;

Expression
	: AssignmentExpression
	| Expression ',' AssignmentExpression	{
				$$ = ILNode_Comma_create($1, $3);
			}
	;

ConstantExpression
	: ConditionalExpression	{
				$$ = ILNode_ToConst_create($1);
			}
	;

Declaration
	: DeclarationSpecifiers ';'						{ /* TODO */ }
	| DeclarationSpecifiers InitDeclaratorList ';'	{ /* TODO */ }
	;

DeclarationSpecifiers
	: StorageClassSpecifier
	| StorageClassSpecifier DeclarationSpecifiers	{
				$$ = CDeclSpecCombine($1, $2);
			}
	| TypeSpecifier
	| TypeSpecifier DeclarationSpecifiers	{
				$$ = CDeclSpecCombine($1, $2);
			}
	;

InitDeclaratorList
	: InitDeclarator
	| InitDeclaratorList ',' InitDeclarator
	;

InitDeclarator
	: Declarator
	| Declarator '=' Initializer
	;

StorageClassSpecifier
	: K_TYPEDEF			{ CDeclSpecSet($$, C_SPEC_TYPEDEF); }
	| K_EXTERN			{ CDeclSpecSet($$, C_SPEC_EXTERN); }
	| K_STATIC			{ CDeclSpecSet($$, C_SPEC_STATIC); }
	| K_AUTO			{ CDeclSpecSet($$, C_SPEC_AUTO); }
	| K_REGISTER		{ CDeclSpecSet($$, C_SPEC_REGISTER); }
	| K_INLINE			{ CDeclSpecSet($$, C_SPEC_INLINE); }
	;

TypeSpecifier
	: K_CHAR			{ CDeclSpecSetType($$, ILType_Int8); }
	| K_SHORT			{ CDeclSpecSet($$, C_SPEC_SHORT); }
	| K_INT				{ CDeclSpecSetType($$, ILType_Int32); }
	| K_LONG			{ CDeclSpecSet($$, C_SPEC_LONG); }
	| K_SIGNED			{ CDeclSpecSet($$, C_SPEC_SIGNED); }
	| K_UNSIGNED		{ CDeclSpecSet($$, C_SPEC_UNSIGNED); }
	| K_FLOAT			{ CDeclSpecSetType($$, ILType_Float32); }
	| K_DOUBLE			{ CDeclSpecSetType($$, ILType_Float64); }
	| K_CONST			{ CDeclSpecSet($$, C_SPEC_CONST); }
	| K_VOLATILE		{ CDeclSpecSet($$, C_SPEC_VOLATILE); }
	| K_NATIVE			{ CDeclSpecSet($$, C_SPEC_NATIVE); }
	| K_VOID			{ CDeclSpecSetType($$, ILType_Void); }
	| K_BOOL			{ CDeclSpecSetType($$, ILType_Boolean); }
	| K_WCHAR			{ CDeclSpecSetType($$, ILType_Char); }
	| K_NINT			{ CDeclSpecSetType($$, ILType_Int); }
	| K_VA_LIST			{ CDeclSpecSetType($$, CTypeCreateVaList(&CCCodeGen)); }
	| K_JMP_BUF			{ CDeclSpecSetType($$, CTypeCreateJmpBuf(&CCCodeGen)); }
	| StructOrUnionSpecifier		{ CDeclSpecSetType($$, $1); }
	| EnumSpecifier					{ CDeclSpecSetType($$, $1); }
	| K_TYPEOF '(' Expression ')'	{
				/* Perform inline semantic analysis on the expression */
				CSemValue value = CSemInlineAnalysis
						(&CCCodeGen, $3, CCurrentScope);

				/* Use the type of the expression as our return value */
				CDeclSpecSetType($$, CSemGetType(value));
			}
	| K_TYPEOF '(' TypeName ')'		{ CDeclSpecSetType($$, $3); }
	| TYPE_NAME						{
				/* Look up the type in the current scope.  We know that
				   the typedef is present, because the lexer found it */
				ILType *type = CScopeGetType(CScopeLookup($1));
				CDeclSpecSetType($$, type);
			}
	;

StructOrUnionSpecifier
	: StructOrUnion AnyIdentifier '{'	{
				/* Look for a definition in the local scope */
				if(!CScopeHasStructOrUnion($2, $1))
				{
					/* Define the struct or union type */
					$<structInfo>$.type = CTypeDefineStructOrUnion
							(&CCCodeGen, $2, $1, functionName);
				}
				else
				{
					/* We've already seen a definition for this type before */
					if($1 == C_STKIND_STRUCT || $1 == C_STKIND_STRUCT_NATIVE)
					{
						CCError(_("redefinition of `struct %s'"), $2);
					}
					else
					{
						CCError(_("redefinition of `union %s'"), $2);
					}

					/* Create an anonymous type as a place-holder */
					$<structInfo>$.type = CTypeDefineAnonStructOrUnion
						(&CCCodeGen, currentStruct, functionName, $1);
				}

				/* Push in a scope level for the structure */
				$<structInfo>$.parent = currentStruct;
				currentStruct = $<structInfo>$.type;

				/* Add the type to the current scope */
				CScopeAddStructOrUnion($2, $1, $<structInfo>$.type);
			}
	  StructDeclarationList '}'	{
	  			/* Pop the structure scope */
				currentStruct = $<structInfo>4.parent;

				/* Terminate the structure definition */
				CTypeEndStruct(&CCCodeGen, $<structInfo>4.type);

				/* Return the completed type to the next higher level */
				$$ = $<structInfo>4.type;
	  		}
	| StructOrUnion '{' 	{
				/* Define an anonymous struct or union type */
				$<structInfo>$.type = CTypeDefineAnonStructOrUnion
					(&CCCodeGen, currentStruct, functionName, $1);

				/* Push in a scope level for the structure */
				$<structInfo>$.parent = currentStruct;
				currentStruct = $<structInfo>$.type;
			}
	  StructDeclarationList '}'		{
	  			/* Pop the structure scope */
				currentStruct = $<structInfo>3.parent;

				/* Terminate the structure definition */
				CTypeEndStruct(&CCCodeGen, $<structInfo>3.type);

				/* Return the completed type to the next higher level */
				$$ = $<structInfo>3.type;
	  		}
	| StructOrUnion AnyIdentifier	{
				/* Look for an existing definition for this type */
				void *data = CScopeLookupStructOrUnion($2, $1);
				if(!data)
				{
					/* Create a reference to the named struct or union type,
					   assuming that it is at the global level */
					$$ = CTypeCreateStructOrUnion
							(&CCCodeGen, $2, $1, 0);
				}
				else
				{
					/* Use the type that was registered in the scope */
					$$ = CScopeGetType(data);
				}
			}
	;

StructOrUnion
	: K_STRUCT					{ $$ = C_STKIND_STRUCT; }
	| K_STRUCT K_NATIVE			{ $$ = C_STKIND_STRUCT_NATIVE; }
	| K_UNION					{ $$ = C_STKIND_UNION; }
	| K_UNION K_NATIVE			{ $$ = C_STKIND_UNION_NATIVE; }
	;

StructDeclarationList
	: StructDeclarationList2
	;

StructDeclarationList2
	: StructDeclaration
	| StructDeclarationList2 StructDeclaration
	;

StructDeclaration
	: TypeSpecifierList StructDeclaratorList ';'
	;

StructDeclaratorList
	: StructDeclarator
	| StructDeclaratorList ',' StructDeclarator
	;

StructDeclarator
	: Declarator
	| ':' ConstantExpression
	| Declarator ':' ConstantExpression
	;

EnumSpecifier
	: K_ENUM '{'	{
				/* Define an anonymous enum type */
				$<structInfo>$.type = CTypeDefineAnonEnum
						(&CCCodeGen, functionName);

				/* Push in a scope level for the enum */
				$<structInfo>$.parent = currentEnum;
				currentEnum = $<structInfo>$.type;
				currentEnumValue = 0;
			}
	  EnumeratorList '}'	{
	  			/* Pop the enum scope */
				currentEnum = $<structInfo>3.parent;

				/* Return the completed type to the next higher level */
				$$ = $<structInfo>3.type;
	  		}
	| K_ENUM AnyIdentifier '{'	{
				/* Look for a definition in the local scope */
				if(!CScopeHasEnum($2))
				{
					/* Define the enum type */
					$<structInfo>$.type = CTypeDefineEnum
							(&CCCodeGen, $2, functionName);
				}
				else
				{
					/* We've already seen a definition for this type before */
					CCError(_("redefinition of `enum %s'"), $2);

					/* Create an anonymous type as a place-holder */
					$<structInfo>$.type = CTypeDefineAnonEnum
							(&CCCodeGen, functionName);
				}

				/* Push in a scope level for the enum */
				$<structInfo>$.parent = currentEnum;
				currentEnum = $<structInfo>$.type;
				currentEnumValue = 0;

				/* Add the type to the current scope */
				CScopeAddEnum($2, $<structInfo>$.type);
			}
	  EnumeratorList '}'	{
	  			/* Pop the enum scope */
				currentEnum = $<structInfo>4.parent;

				/* Return the completed type to the next higher level */
				$$ = $<structInfo>4.type;
			}
	| K_ENUM AnyIdentifier		{
				/* Look for an existing definition for this type */
				void *data = CScopeLookupEnum($2);
				if(!data)
				{
					/* Create a reference to the named enum type
					   assuming that it is at the global level */
					$$ = CTypeCreateEnum(&CCCodeGen, $2, 0);
				}
				else
				{
					/* Use the type that was registered in the scope */
					$$ = CScopeGetType(data);
				}
			}
	;

EnumeratorList
	: Enumerator
	| EnumeratorList ',' Enumerator
	;

Enumerator
	: IDENTIFIER							{}
	| IDENTIFIER '=' ConstantExpression		{}
	;

Declarator
	: Declarator2
	| Pointer Declarator2
	;

Declarator2
	: IDENTIFIER			{}
	| '(' Declarator ')'
	| Declarator2 '[' ']'
	| Declarator2 '[' ConstantExpression ']'
	| Declarator2 '(' ')'
	| Declarator2 '(' ')' FuncAttributes
	| Declarator2 '(' ParameterTypeList ')'
	| Declarator2 '(' ParameterTypeList ')' FuncAttributes
	| Declarator2 '(' ParameterIdentifierList ')'
	| Declarator2 '(' ParameterIdentifierList ')' FuncAttributes
	;

FuncAttributes
	: K_ATTRIBUTE '(' '(' AttributeList ')' ')'
	;

AttributeList
	: Attribute
	| AttributeList ',' Attribute
	;

Attribute
	: AnyIdentifier								{}
	| AnyIdentifier '(' AttributeArgs ')'		{}
	;

ConstantAttributeExpression
	: ConditionalExpression		{
				$$ = ILNode_ToConst_create($1);
			}
	;

AttributeArgs
	: ConstantAttributeExpression
	| AttributeArgs ',' ConstantAttributeExpression	{
				$$ = ILNode_ArgList_create($1, $3);
			}
	;

Pointer
	: '*'
	| '*' TypeQualifierList
	| '*' Pointer
	| '*' TypeQualifierList Pointer
	;

TypeQualifier
	: K_CONST
	| K_VOLATILE
	| K_NATIVE
	;

TypeQualifierList
	: TypeQualifier
	| TypeQualifierList TypeQualifier
	;

TypeSpecifierList
	: TypeSpecifierList2
	;

TypeSpecifierList2
	: TypeSpecifier						{}
	| TypeSpecifierList2 TypeSpecifier	{}
	;

ParameterIdentifierList
	: IdentifierList
	| IdentifierList ',' K_ELIPSIS
	;

IdentifierList
	: IDENTIFIER						{}
	| IdentifierList ',' IDENTIFIER		{}
	;

ParameterTypeList
	: ParameterList
	| ParameterList ',' K_ELIPSIS
	;

ParameterList
	: ParameterDeclaration
	| ParameterList ',' ParameterDeclaration
	;

ParameterDeclaration
	: TypeSpecifierList Declarator		{}
	| TypeName							{}
	;

TypeName
	: TypeSpecifierList						{ $$ = ILType_Void; /* TODO */ }
	| TypeSpecifierList AbstractDeclarator	{ $$ = ILType_Void; /* TODO */ }
	;

AbstractDeclarator
	: Pointer
	| AbstractDeclarator2
	| Pointer AbstractDeclarator2
	;

AbstractDeclarator2
	: '(' AbstractDeclarator ')'
	| '[' ']'
	| '[' ConstantExpression ']'
	| AbstractDeclarator2 '[' ']'
	| AbstractDeclarator2 '[' ConstantExpression ']'
	| '(' ')'
	| '(' ')' FuncAttributes
	| '(' ParameterTypeList ')'
	| '(' ParameterTypeList ')' FuncAttributes
	| AbstractDeclarator2 '(' ')'
	| AbstractDeclarator2 '(' ')' FuncAttributes
	| AbstractDeclarator2 '(' ParameterTypeList ')'
	| AbstractDeclarator2 '(' ParameterTypeList ')' FuncAttributes
	;

Initializer
	: AssignmentExpression				{}
	| '{' InitializerList '}'
	| '{' InitializerList ',' '}'
	;

InitializerList
	: Initializer
	| InitializerList ',' Initializer
	;

Statement
	: Statement2		{
			#ifdef YYBISON
				if(debug_flag)
				{
					$$ = ILNode_LineInfo_create($1);
					yysetlinenum($$, @1.first_line);
				}
				else
			#endif
				{
					$$ = $1;
				}
			}
	;

Statement2
	: LabeledStatement
	| CompoundStatement
	| ExpressionStatement
	| SelectionStatement
	| IterationStatement
	| JumpStatement
	;

LabeledStatement
	: IDENTIFIER ':' Statement		{
				/* TODO */
				$$ = $3;
			}
	| K_CASE ConstantExpression ':' Statement	{
				/* TODO */
				$$ = $4;
			}
	| K_DEFAULT ':' Statement	{
				/* TODO */
				$$ = $3;
			}
	;

CompoundStatement
	: '{'	{
				/* Create a new scope */
				CCurrentScope = ILScopeCreate(&CCCodeGen, CCurrentScope);
			}
	  OptDeclarationList OptStatementList '}'	{
	  			/* Build the compound statement block if it isn't empty */
				if($3 != 0 || $4 != 0)
				{
					$$ = ILNode_NewScope_create
						(ILNode_Compound_CreateFrom($3, $4));
					((ILNode_NewScope *)($$))->scope = CCurrentScope;
				}
				else
				{
					$$ = ILNode_Empty_create();
				}

				/* Fix up the line number on the compound statement node */
			#ifdef YYBISON
				yysetlinenum($$, @1.first_line);
			#endif

	  			/* Pop the scope */
				CCurrentScope = ILScopeGetParent(CCurrentScope);
			}
	;

OptDeclarationList
	: DeclarationList		{ $$ = $1; }
	| /* empty */			{ $$ = 0; }
	;

DeclarationList
	: Declaration
	| DeclarationList Declaration	{
				$$ = ILNode_Compound_CreateFrom($1, $2);
			}
	;

OptStatementList
	: StatementList			{ $$ = $1; }
	| /* empty */			{ $$ = 0; }
	;

StatementList
	: Statement
	| StatementList Statement	{
				$$ = ILNode_Compound_CreateFrom($1, $2);
			}
	;

ExpressionStatement
	: ';'					{ $$ = ILNode_Empty_create(); }
	| Expression ';'		{ $$ = $1; }
	;

BoolExpression
	: Expression			{ $$ = ILNode_ToBool_create($1); }
	;

SelectionStatement
	: K_IF '(' BoolExpression ')' Statement	{
				$$ = ILNode_If_create($3, $5, ILNode_Empty_create());
			}
	| K_IF '(' BoolExpression ')' Statement K_ELSE Statement	{
				$$ = ILNode_If_create($3, $5, $7);
			}
	| K_SWITCH '(' Expression ')' Statement	{
				$$ = ILNode_Switch_create($3, $5);
			}
	;

IterationStatement
	: K_WHILE '(' BoolExpression ')' Statement	{
				$$ = ILNode_While_create($3, $5);
			}
	| K_DO Statement K_WHILE '(' BoolExpression ')' ';'	{
				$$ = ILNode_Do_create($2, $5);
			}
	| K_FOR '(' ';' ';' ')' Statement	{
				$$ = ILNode_For_create(ILNode_Empty_create(),
									   ILNode_True_create(),
									   ILNode_Empty_create(), $6);
			}
	| K_FOR '(' ';' ';' Expression ')' Statement	{
				$$ = ILNode_For_create(ILNode_Empty_create(),
									   ILNode_True_create(),
									   $5, $7);
			}
	| K_FOR '(' ';' BoolExpression ';' ')' Statement	{
				$$ = ILNode_For_create(ILNode_Empty_create(), $4,
									   ILNode_Empty_create(), $7);
			}
	| K_FOR '(' ';' BoolExpression ';' Expression ')' Statement	{
				$$ = ILNode_For_create(ILNode_Empty_create(), $4, $6, $8);
			}
	| K_FOR '(' Expression ';' ';' ')' Statement	{
				$$ = ILNode_For_create($3,
									   ILNode_True_create(),
									   ILNode_Empty_create(), $7);
			}
	| K_FOR '(' Expression ';' ';' Expression ')' Statement	{
				$$ = ILNode_For_create($3, ILNode_True_create(), $6, $8);
			}
	| K_FOR '(' Expression ';' BoolExpression ';' ')' Statement	{
				$$ = ILNode_For_create($3, $5, ILNode_Empty_create(), $8);
			}
	| K_FOR '(' Expression ';' BoolExpression ';' Expression ')' Statement	{
				$$ = ILNode_For_create($3, $5, $7, $9);
			}
	;

JumpStatement
	: K_GOTO IDENTIFIER ';'		{ ILNode_Goto_create($2); }
	| K_CONTINUE ';'			{ ILNode_Continue_create(); }
	| K_BREAK ';'				{ ILNode_Break_create(); }
	| K_RETURN ';'				{ ILNode_Return_create(); }
	| K_RETURN Expression ';'	{ ILNode_ReturnExpr_create($2); }
	;

File
	: File2
	| /* empty */
	;

File2
	: ExternalDefinition
	| File2 ExternalDefinition
	;

ExternalDefinition
	: FunctionDefinition	{
				/* Roll the treecc node heap back to the last check point */
				yynodepop();
			}
	| Declaration			{
				/* Roll the treecc node heap back to the last check point */
				yynodepop();
			}
	;

FunctionDefinition
	: Declarator OptParamDeclarationList '{'	{
				/* Create the function */
				/* TODO */

				/* Create a new scope to hold the function body */
				CCurrentScope = ILScopeCreate(&CCCodeGen, CCurrentScope);

				/* Declare the parameters into the new scope */
				/* TODO */
			}
	  FunctionBody '}'		{
	  			/* Wrap the function body in a new scope record */
				ILNode *body = ILNode_NewScope_create($5);
				((ILNode_NewScope *)body)->scope = CCurrentScope;

				/* Fix up the line number on the function body */
			#ifdef YYBISON
				yysetlinenum(body, @3.first_line);
			#endif

				/* Pop the scope */
				CCurrentScope = ILScopeGetParent(CCurrentScope);

				/* Output the finished function */
				/* TODO */
	  		}
	| DeclarationSpecifiers Declarator OptParamDeclarationList '{'	{
				/* Create the function */
				/* TODO */

				/* Create a new scope to hold the function body */
				CCurrentScope = ILScopeCreate(&CCCodeGen, CCurrentScope);

				/* Declare the parameters into the new scope */
				/* TODO */
			}
	  FunctionBody '}'		{
	  			/* Wrap the function body in a new scope record */
				ILNode *body = ILNode_NewScope_create($6);
				((ILNode_NewScope *)body)->scope = CCurrentScope;

				/* Fix up the line number on the function body */
			#ifdef YYBISON
				yysetlinenum(body, @4.first_line);
			#endif

				/* Pop the scope */
				CCurrentScope = ILScopeGetParent(CCurrentScope);

				/* Output the finished function */
				/* TODO */
	  		}
	;

OptParamDeclarationList
	: ParamDeclarationList
	| /* empty */
	;

ParamDeclarationList
	: ParamDeclaration
	| ParamDeclarationList ParamDeclaration
	;

ParamDeclaration
	: DeclarationSpecifiers ParamDeclaratorList ';'	{}
	;

ParamDeclaratorList
	: ParamDeclarator
	| ParamDeclaratorList ',' ParamDeclarator
	;

ParamDeclarator
	: Declarator
	;

FunctionBody
	: OptDeclarationList OptStatementList	{
				if($1 != 0 || $2 != 0)
				{
					$$ = ILNode_Compound_CreateFrom($1, $2);
				}
				else
				{
					$$ = ILNode_Empty_create();
				}
			}
	;
