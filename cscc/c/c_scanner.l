%{
/*
 * c_scanner.l - Input file for lex that defines the C token syntax.
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
#include <cscc/c/c_grammar.h>

extern YYSTYPE yylval;
#ifdef YYLTYPE
extern YYLTYPE yylloc;
#endif

/*
 * Get the next line of input from the pre-processor.
 */
#define	YY_INPUT(buf,result,maxSize)	\
	{ \
		result = CCPluginInput((buf), (maxSize)); \
	}

/*
 * Update the line number information and return a token code.
 */
#ifdef YYLTYPE
#define	RETURNTOK(x)		\
			do { \
				yylloc.first_line = yycurrlinenum(); \
				return (x); \
			} while (0)
#else
#define	RETURNTOK(x)		return (x)
#endif

%}

%option outfile="lex.yy.c"
%option prefix="c_"
%option noyywrap
%option nounput

DIGIT			[0-9]
IDALPHA			[a-zA-Z_$]
HEX				[a-fA-F0-9]
EXPONENT		[Ee][+-]?{DIGIT}+
FTYPE			(f|F|l|L)
ITYPE			(u|U|l|L)*
WHITE			[ \t\v\r\f]

%%

^{WHITE}*"#"{WHITE}+{DIGIT}+{WHITE}+["](\\.|[^\\"])*["].*\n	  {
			CLexLineDirective(yytext);
		}
^{WHITE}*"#line"{WHITE}+{DIGIT}+{WHITE}+["](\\.|[^\\"])*["].*\n {
			CLexLineDirective(yytext);
		}
^{WHITE}*"#pragma"[^\n]*\n		{ /* Ignore pragma directives */ }
^{WHITE}*"#ident"[^\n]*\n		{ /* Ignore ident directives */ }
^{WHITE}*"#"{IDALPHA}({DIGIT}|{IDALPHA})*[^\n]*\n	{
			yytext[strlen(yytext) - 1] = '\0';
			CCWarningOnLine(yycurrfilename(), yycurrlinenum(),
							_("unknown directive %s"), yytext);
		}

"++"					{ return INC_OP; }
"--"					{ return DEC_OP; }
"<<"					{ return LEFT_OP; }
">>"					{ return RIGHT_OP; }
"<="					{ return LE_OP; }
">="					{ return GE_OP; }
"=="					{ return EQ_OP; }
"!="					{ return NE_OP; }
"&&"					{ return AND_OP; }
"||"					{ return OR_OP; }
"*="					{ return MUL_ASSIGN_OP; }
"/="					{ return DIV_ASSIGN_OP; }
"%="					{ return MOD_ASSIGN_OP; }
"+="					{ return ADD_ASSIGN_OP; }
"-="					{ return SUB_ASSIGN_OP; }
"<<="					{ return LEFT_ASSIGN_OP; }
">>="					{ return RIGHT_ASSIGN_OP; }
"&="					{ return AND_ASSIGN_OP; }
"^="					{ return XOR_ASSIGN_OP; }
"|="					{ return OR_ASSIGN_OP; }
"->"					{ return PTR_OP; }

"asm"					{ return K_ASM; }
"__asm__"				{ return K_ASM; }
"auto"					{ return K_AUTO; }
"break"					{ return K_BREAK; }
"case"					{ return K_CASE; }
"char"					{ return K_CHAR; }
"const"					{ return K_CONST; }
"__const__"				{ return K_CONST; }
"continue"				{ return K_CONTINUE; }
"default"				{ return K_DEFAULT; }
"do"					{ return K_DO; }
"double"				{ return K_DOUBLE; }
"else"					{ return K_ELSE; }
"enum"					{ return K_ENUM; }
"extern"				{ return K_EXTERN; }
"float"					{ return K_FLOAT; }
"for"					{ return K_FOR; }
"goto"					{ return K_GOTO; }
"if"					{ return K_IF; }
"inline"				{ return K_INLINE; }
"__inline__"			{ return K_INLINE; }
"int"					{ return K_INT; }
"long"					{ return K_LONG; }
"register"				{ return K_REGISTER; }
"return"				{ return K_RETURN; }
"short"					{ return K_SHORT; }
"signed"				{ return K_SIGNED; }
"__signed__"			{ return K_SIGNED; }
"sizeof"				{ return K_SIZEOF; }
"static"				{ return K_STATIC; }
"struct"				{ return K_STRUCT; }
"switch"				{ return K_SWITCH; }
"typedef"				{ return K_TYPEDEF; }
"typeof"				{ return K_TYPEOF; }
"__typeof__"			{ return K_TYPEOF; }
"union"					{ return K_UNION; }
"unsigned"				{ return K_UNSIGNED; }
"void"					{ return K_VOID; }
"volatile"				{ return K_VOLATILE; }
"__volatile__"			{ return K_VOLATILE; }
"while"					{ return K_WHILE; }

"..."					{ return K_ELIPSIS; }
"__builtin_va_list"		{ return K_VA_LIST; }
"__builtin_va_start"	{ return K_VA_START; }
"__builtin_va_arg"		{ return K_VA_ARG; }
"__builtin_va_end"		{ return K_VA_END; }
"__builtin_setjmp"		{ return K_SETJMP; }
"__builtin_alloca"		{ return K_ALLOCA; }
"__attribute__"			{ return K_ATTRIBUTE; }

"__bool__"				{ return K_BOOL; }
"__wchar__"				{ return K_WCHAR; }
"__true__"				{ return K_TRUE; }
"__false__"				{ return K_FALSE; }
"__native__"			{ return K_NATIVE; }
"__csharp__"			{ return K_CSHARP; }
"__invoke__"			{ return K_INVOKE; }

"__FUNCTION__"			{ return K_FUNCTION; }
"__PRETTY_FUNCTION__"	{ return K_FUNCTION; }
"__func__"				{ return K_FUNC; }

{IDALPHA}({DIGIT}|{IDALPHA})*	{
			/* Use the scope to determine if this is a typedef'ed name */
			yylval.name = ILInternString(yytext, strlen(yytext)).string;
			if(CScopeIsTypedef(yylval.name))
			{
				RETURNTOK(TYPE_NAME);
			}
			else
			{
				RETURNTOK(IDENTIFIER);
			}
		}

{DIGIT}+{EXPONENT}{FTYPE}?		{ CLexParseFloat(yytext, &(yylval.real));
								  RETURNTOK(FLOAT_CONSTANT); }
{DIGIT}*"."{DIGIT}+({EXPONENT})?{FTYPE}?	{
			CLexParseFloat(yytext, &(yylval.real));
			RETURNTOK(FLOAT_CONSTANT);
		}
{DIGIT}+"."{DIGIT}*({EXPONENT})?{FTYPE}?	{
			CLexParseFloat(yytext, &(yylval.real));
			RETURNTOK(FLOAT_CONSTANT);
		}

0[xX]{HEX}+{ITYPE}?				{ CLexParseInt(yytext, &(yylval.integer));
								  RETURNTOK(INTEGER_CONSTANT); }
0[0-7]*{ITYPE}?					{ CLexParseInt(yytext, &(yylval.integer));
								  RETURNTOK(INTEGER_CONSTANT); }
{DIGIT}+{ITYPE}?				{ CLexParseInt(yytext, &(yylval.integer));
								  RETURNTOK(INTEGER_CONSTANT); }
'(\\.|[^\\'])+'					{ CLexParseChar(yytext, &(yylval.integer));
								  RETURNTOK(INTEGER_CONSTANT); }

\"(\\.|[^\\"])*\"				{ CLexParseString(yytext, &(yylval.string));
								  RETURNTOK(STRING_LITERAL); }

{WHITE}+						;

\n								;

.								{ return (((int)(yytext[0])) & 0xFF); }

%%
