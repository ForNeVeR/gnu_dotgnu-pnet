%{
/*
 * ilasm_scanner.l - Input file for lex that defines the ILASM token syntax.
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
#include "ilasm_rename.h"

#include <stdio.h>
#include "il_system.h"
#include "il_opcodes.h"
#include "il_program.h"
#include "il_utils.h"
#include "ilasm_build.h"
#include "ilasm_output.h"
#include "ilasm_grammar.h"

extern YYSTYPE yylval;

char *ILAsmFilename = 0;
long  ILAsmLineNum = 1;
int   ILAsmErrors = 0;
int   ILAsmParseHexBytes = 0;

/*
 * Parse a floating point value.
 */
static ILDouble ParseFloat(const char *text)
{
#ifdef HAVE_STRTOD
	return (ILDouble)(strtod(text, (char **)0));
#else
	double val;
	if(sscanf(text, "%lf", &val) == 1)
	{
		return (ILDouble)val;
	}
	else
	{
		return (ILDouble)0.0;
	}
#endif
}

/*
 * Parse a decimal integer value.
 */
static ILInt64 ParseDecInt(const char *text)
{
	ILInt64 value = 0;
	while(*text != '\0')
	{
		value = (value * (ILInt64)10) + (ILInt64)(*text - '0');
		++text;
	}
	return value;
}

/*
 * Parse a hexadecimal integer value.
 */
static ILInt64 ParseHexInt(const char *text)
{
	ILInt64 value = 0;
	if(*text == '0' && (text[1] == 'x' || text[1] == 'X'))
	{
		text += 2;
	}
	while(*text != '\0')
	{
		if(*text >= '0' && *text <= '9')
			value = (value * (ILInt64)16) + (ILInt64)(*text - '0');
		else if(*text >= 'A' && *text <= 'F')
			value = (value * (ILInt64)16) + (ILInt64)(*text - 'A' + 10);
		else
			value = (value * (ILInt64)16) + (ILInt64)(*text - 'a' + 10);
		++text;
	}
	return value;
}

/*
 * Parse an octal integer value.
 */
static ILInt64 ParseOctInt(const char *text)
{
	ILInt64 value = 0;
	while(*text != '\0')
	{
		value = (value * (ILInt64)8) + (ILInt64)(*text - '0');
		++text;
	}
	return value;
}

/*
 * Parse a quoted string.
 */
static ILIntString ParseString(char *text, int *len)
{
	char *save = text;
	char *out = text;
	int ch;
	static char const escapes[] =
			"\007\010cd\033\014ghijklm\012opq\015s\011u\013wxyz";

	/* Remove the quote characters */
	++text;
	text[strlen(text) - 1] = '\0';

	/* Collapse escape sequences in the string */
	while(*text != '\0')
	{
		if(*text == '\\')
		{
			++text;
			if(*text == '\0')
			{
				/* Truncated escape sequence */
				break;
			}
			else if(*text == 'x')
			{
				/* Hex character */
				++text;
				ch = 0;
				while(*text != '\0')
				{
					if(*text >= '0' && *text <= '9')
						ch = ch * 16 + (*text++ - '0');
					else if(*text >= 'A' && *text <= 'F')
						ch = ch * 16 + (*text++ - 'A' + 10);
					else if(*text >= 'a' && *text <= 'f')
						ch = ch * 16 + (*text++ - 'a' + 10);
					else
						break;
				}
				*out++ = (char)ch;
			}
			else if(*text >= 'a' && *text <= 'z')
			{
				/* Ordinary C-style escape */
				*out++ = escapes[*text - 'a'];
				++text;
			}
			else if(*text >= '0' && *text <= '7')
			{
				/* Octal character */
				ch = *text++ - '0';
				while(*text >= '0' && *text <= '7')
				{
					ch = ch * 8 + (*text++ - '0');
				}
				*out++ = (char)ch;
			}
			else
			{
				/* Normal escaped character */
				*out++ = *text++;
			}
		}
		else
		{
			/* Normal character */
			*out++ = *text++;
		}
	}

	/* Internalise the string */
	return ILInternString(save, (int)(out - save));
}

/*
 * Helper macros for returning opcodes of various types.
 */
#define	OPCODE_NONE(name)	yylval.opcode = IL_OP_##name; \
							return I_NONE
#define	OPCODE_NONE_P(name)	yylval.opcode = 0xFE00 | IL_PREFIX_OP_##name; \
							return I_NONE
#define	OPCODE_VAR(name)	yylval.opcode = IL_OP_##name; \
							return I_VAR
#define	OPCODE_VAR_P(name)	yylval.opcode = 0xFE00 | IL_PREFIX_OP_##name; \
							return I_VAR
#define	OPCODE_BRANCH(name)	yylval.opcode = IL_OP_##name; \
							return I_BRANCH
#define	OPCODE_BRANCH_P(name) yylval.opcode = 0xFE00 | IL_PREFIX_OP_##name; \
							return I_BRANCH
#define	OPCODE_METHOD(name)	yylval.opcode = IL_OP_##name; \
							return I_METHOD
#define	OPCODE_METHOD_P(name) yylval.opcode = 0xFE00 | IL_PREFIX_OP_##name; \
							return I_METHOD
#define	OPCODE_FIELD(name)	yylval.opcode = IL_OP_##name; \
							return I_FIELD
#define	OPCODE_FIELD_P(name) yylval.opcode = 0xFE00 | IL_PREFIX_OP_##name; \
							return I_FIELD
#define	OPCODE_TYPE(name)	yylval.opcode = IL_OP_##name; \
							return I_TYPE
#define	OPCODE_TYPE_P(name) yylval.opcode = 0xFE00 | IL_PREFIX_OP_##name; \
							return I_TYPE

%}

%option outfile="lex.yy.c"
%option prefix="ilasm_"
%option noyywrap
%option nounput

DIGIT			[0-9]
IDALPHA			[a-zA-Z_$@?]
HEX				[a-fA-F0-9]
EXPONENT		[Ee][+-]?{DIGIT}+
WHITE			[ \t\v\r\f\032]
SIGN			[+-]

%x HEXMODE
%%
	if(ILAsmParseHexBytes)
		BEGIN(HEXMODE);
	else
		BEGIN(0);

<HEXMODE>{HEX}+			{ yylval.integer = ParseHexInt(yytext);
						  return HEX_BYTE; }
<HEXMODE>")"			{ return ')'; }

".addon"				{ return D_ADDON; }			/* Directives */
".algorithm"			{ return D_ALGORITHM; }
".assembly"				{ return D_ASSEMBLY; }
".backing"				{ return D_BACKING; }
".blob"					{ return D_BLOB; }
".capability"			{ return D_CAPABILITY; }
".cctor"				{ return D_CCTOR; }
".class"				{ return D_CLASS; }
".comtype"				{ return D_COMTYPE; }
".config"				{ return D_CONFIG; }
".corflags"				{ return D_CORFLAGS; }
".ctor"					{ return D_CTOR; }
".custom"				{ return D_CUSTOM; }
".data"					{ return D_DATA; }
".emitbyte"				{ return D_EMITBYTE; }
".entrypoint"			{ return D_ENTRYPOINT; }
".event"				{ return D_EVENT; }
".exeloc"				{ return D_EXELOC; }
".export"				{ return D_EXPORT; }
".field"				{ return D_FIELD; }
".file"					{ return D_FILE; }
".fire"					{ return D_FIRE; }
".get"					{ return D_GET; }
".hash"					{ return D_HASH; }
".imagebase"			{ return D_IMAGEBASE; }
".implicitcom"			{ return D_IMPLICITCOM; }
".line"					{ return D_LINE; }
"#line"					{ return D_LINE; }
".locale"				{ return D_LOCALE; }
".locals"				{ return D_LOCALS; }
".manifestres"			{ return D_MANIFESTRES; }
".maxstack"				{ return D_MAXSTACK; }
".method"				{ return D_METHOD; }
".mime"					{ return D_MIME; }
".module"				{ return D_MODULE; }
".mresource"			{ return D_MRESOURCE; }
".namespace"			{ return D_NAMESPACE; }
".originator"			{ return D_ORIGINATOR; }
".os"					{ return D_OS; }
".other"				{ return D_OTHER; }
".override"				{ return D_OVERRIDE; }
".pack"					{ return D_PACK; }
".param"				{ return D_PARAM; }
".permission"			{ return D_PERMISSION; }
".permissionset"		{ return D_PERMISSIONSET; }
".processor"			{ return D_PROCESSOR; }
".property"				{ return D_PROPERTY; }
".publickey"			{ return D_PUBLICKEY; }
".publickeytoken"		{ return D_PUBLICKEYTOKEN; }
".removeon"				{ return D_REMOVEON; }
".set"					{ return D_SET; }
".size"					{ return D_SIZE; }
".subsystem"			{ return D_SUBSYSTEM; }
".title"				{ return D_TITLE; }
".try"					{ return D_TRY; }
".ver"					{ return D_VER; }
".vtable"				{ return D_VTABLE; }
".vtentry"				{ return D_VTENTRY; }
".vtfixup"				{ return D_VTFIXUP; }
".zeroinit"				{ return D_ZEROINIT; }

"at"					{ return K_AT; }			/* Keywords */
"as"					{ return K_AS; }
"implicitcom"			{ return K_IMPLICITCOM; }
"implicitres"			{ return K_IMPLICITRES; }
"noappdomain"			{ return K_NOAPPDOMAIN; }
"noprocess"				{ return K_NOPROCESS; }
"nomachine"				{ return K_NOMACHINE; }
"extern"				{ return K_EXTERN; }
"instance"				{ return K_INSTANCE; }
"explicit"				{ return K_EXPLICIT; }
"default"				{ return K_DEFAULT; }
"vararg"				{ return K_VARARG; }
"unmanaged"				{ return K_UNMANAGED; }
"cdecl"					{ return K_CDECL; }
"stdcall"				{ return K_STDCALL; }
"thiscall"				{ return K_THISCALL; }
"fastcall"				{ return K_FASTCALL; }
"marshal"				{ return K_MARSHAL; }
"in"					{ return K_IN; }
"out"					{ return K_OUT; }
"opt"					{ return K_OPT; }
"lcid"					{ return K_LCID; }
"retval"				{ return K_RETVAL; }
"static"				{ return K_STATIC; }
"public"				{ return K_PUBLIC; }
"private"				{ return K_PRIVATE; }
"family"				{ return K_FAMILY; }
"initonly"				{ return K_INITONLY; }
"rtspecialname"			{ return K_RTSPECIALNAME; }
"specialname"			{ return K_SPECIALNAME; }
"assembly"				{ return K_ASSEMBLY; }
"famandassem"			{ return K_FAMANDASSEM; }
"famorassem"			{ return K_FAMORASSEM; }
"privatescope"			{ return K_PRIVATESCOPE; }
"literal"				{ return K_LITERAL; }
"notserialized"			{ return K_NOTSERIALIZED; }
"value"					{ return K_VALUE; }
"not_in_gc_heap"		{ return K_NOT_IN_GC_HEAP; }
"interface"				{ return K_INTERFACE; }
"sealed"				{ return K_SEALED; }
"abstract"				{ return K_ABSTRACT; }
"auto"					{ return K_AUTO; }
"sequential"			{ return K_SEQUENTIAL; }
"ansi"					{ return K_ANSI; }
"unicode"				{ return K_UNICODE; }
"autochar"				{ return K_AUTOCHAR; }
"import"				{ return K_IMPORT; }
"serializable"			{ return K_SERIALIZABLE; }
"nested"				{ return K_NESTED; }
"lateinit"				{ return K_LATEINIT; }
"extends"				{ return K_EXTENDS; }
"implements"			{ return K_IMPLEMENTS; }
"final"					{ return K_FINAL; }
"virtual"				{ return K_VIRTUAL; }
"hidebysig"				{ return K_HIDEBYSIG; }
"newslot"				{ return K_NEWSLOT; }
"unmanagedexp"			{ return K_UNMANAGEDEXP; }
"pinvokeimpl"			{ return K_PINVOKEIMPL; }
"nomangle"				{ return K_NOMANGLE; }
"ole"					{ return K_OLE; }
"lasterr"				{ return K_LASTERR; }
"winapi"				{ return K_WINAPI; }
"native"				{ return K_NATIVE; }
"il"					{ return K_IL; }
"optil"					{ return K_OPTIL; }
"managed"				{ return K_MANAGED; }
"forwardref"			{ return K_FORWARDREF; }
"runtime"				{ return K_RUNTIME; }
"internalcall"			{ return K_INTERNALCALL; }
"synchronized"			{ return K_SYNCHRONIZED; }
"noinlining"			{ return K_NOINLINING; }
"custom"				{ return K_CUSTOM; }
"fixed"					{ return K_FIXED; }
"sysstring"				{ return K_SYSSTRING; }
"array"					{ return K_ARRAY; }
"variant"				{ return K_VARIANT; }
"currency"				{ return K_CURRENCY; }
"syschar"				{ return K_SYSCHAR; }
"void"					{ return K_VOID; }
"bool"					{ return K_BOOL; }
"int8"					{ return K_INT8; }
"int16"					{ return K_INT16; }
"int32"					{ return K_INT32; }
"int64"					{ return K_INT64; }
"float32"				{ return K_FLOAT32; }
"float64"				{ return K_FLOAT64; }
"error"					{ return K_ERROR; }
"unsigned"				{ return K_UNSIGNED; }
"decimal"				{ return K_DECIMAL; }
"date"					{ return K_DATE; }
"bstr"					{ return K_BSTR; }
"lpstr"					{ return K_LPSTR; }
"lpwstr"				{ return K_LPWSTR; }
"lptstr"				{ return K_LPTSTR; }
"objectref"				{ return K_OBJECTREF; }
"iunknown"				{ return K_IUNKNOWN; }
"idispatch"				{ return K_IDISPATCH; }
"struct"				{ return K_STRUCT; }
"safearray"				{ return K_SAFEARRAY; }
"int"					{ return K_INT; }
"byvalstr"				{ return K_BYVALSTR; }
"tbstr"					{ return K_TBSTR; }
"lpvoid"				{ return K_LPVOID; }
"any"					{ return K_ANY; }
"float"					{ return K_FLOAT; }
"lpstruct"				{ return K_LPSTRUCT; }
"null"					{ return K_NULL; }
"ptr"					{
			/* "ptr" used to be a reserved word, but not any more */
			yylval.strValue = ILInternString(yytext, strlen(yytext));
			return IDENTIFIER;
		}
"vector"				{ return K_VECTOR; }
"hresult"				{ return K_HRESULT; }
"carray"				{ return K_CARRAY; }
"userdefined"			{ return K_USERDEFINED; }
"record"				{ return K_RECORD; }
"filetime"				{ return K_FILETIME; }
"blob"					{ return K_BLOB; }
"stream"				{ return K_STREAM; }
"storage"				{ return K_STORAGE; }
"streamed_object"		{ return K_STREAMED_OBJECT; }
"stored_object"			{ return K_STORED_OBJECT; }
"blob_object"			{ return K_BLOB_OBJECT; }
"cf"					{ return K_CF; }
"clsid"					{ return K_CLSID; }
"method"				{ return K_METHOD; }
"class"					{ return K_CLASS; }
"pinned"				{ return K_PINNED; }
"modreq"				{ return K_MODREQ; }
"modopt"				{ return K_MODOPT; }
"typedref"				{ return K_TYPEDREF; }
"wchar"					{ return K_WCHAR; }
"char"					{ return K_CHAR; }
"fromunmanaged"			{ return K_FROMUNMANAGED; }
"callmostderived"		{ return K_CALLMOSTDERIVED; }
"bytearray"				{ return K_BYTEARRAY; }
"with"					{ return K_WITH; }
"init"					{ return K_INIT; }
"to"					{ return K_TO; }
"catch"					{ return K_CATCH; }
"filter"				{ return K_FILTER; }
"finally"				{ return K_FINALLY; }
"fault"					{ return K_FAULT; }
"handler"				{ return K_HANDLER; }
"tls"					{ return K_TLS; }
"field"					{ return K_FIELD; }
"request"				{ return K_REQUEST; }
"demand"				{ return K_DEMAND; }
"assert"				{ return K_ASSERT; }
"deny"					{ return K_DENY; }
"permitonly"			{ return K_PERMITONLY; }
"linkcheck"				{ return K_LINKCHECK; }
"inheritcheck"			{ return K_INHERITCHECK; }
"reqmin"				{ return K_REQMIN; }
"reqopt"				{ return K_REQOPT; }
"reqrefuse"				{ return K_REQREFUSE; }
"prejitgrant"			{ return K_PREJITGRANT; }
"prejitdeny"			{ return K_PREJITDENY; }
"noncasdemand"			{ return K_NONCASDEMAND; }
"noncaslinkdemand"		{ return K_NONCASLINKDEMAND; }
"noncasinheritance"		{ return K_NONCASINHERITANCE; }
"readonly"				{ return K_READONLY; }
"nometadata"			{ return K_NOMETADATA; }
"algorithm"				{ return K_ALGORITHM; }
"fullorigin"			{ return K_FULLORIGIN; }
"nan"					{ return K_NAN; }
"inf"					{ return K_INF; }
"publickey"				{ return K_PUBLICKEY; }
"enablejittracking"		{ return K_ENABLEJITTRACKING; }
"disablejitoptimizer"	{ return K_DISABLEJITOPTIMIZER; }
"preservesig"			{ return K_PRESERVESIG; }
"beforefieldinit"		{ return K_BEFOREFIELDINIT; }
"compilercontrolled"	{ return K_COMPILERCONTROLLED; }
"cil"					{ return K_CIL; }
"valuetype"				{ return K_VALUETYPE; }
"boxed"					{ return K_BOXED; }
"string"				{ return K_STRING; }
"object"				{ return K_OBJECT; }
"alignment"				{ return K_ALIGNMENT; }
"reqsecobj"				{ return K_REQSECOBJ; }
"true"					{ return K_TRUE; }
"false"					{ return K_FALSE; }

"::"					{ return COLON_COLON; }
"..."					{ return DOT_DOT_DOT; }
".."					{ return DOT_DOT; }

"add"					{ OPCODE_NONE(ADD); }	/* Opcodes with no args */
"add.ovf"				{ OPCODE_NONE(ADD_OVF); }
"add.ovf.un"			{ OPCODE_NONE(ADD_OVF_UN); }
"and"					{ OPCODE_NONE(AND); }
"ann.catch"				{ OPCODE_NONE(ANN_CATCH); }
"ann.def"				{ OPCODE_NONE(ANN_DEF); }
"ann.hoisted"			{ OPCODE_NONE(ANN_HOISTED); }
"ann.lab"				{ OPCODE_NONE(ANN_LAB); }
"arglist"				{ OPCODE_NONE_P(ARGLIST); }
"break"					{ OPCODE_NONE(BREAK); }
"ceq"					{ OPCODE_NONE_P(CEQ); }
"cgt"					{ OPCODE_NONE_P(CGT); }
"cgt.un"				{ OPCODE_NONE_P(CGT_UN); }
"ckfinite"				{ OPCODE_NONE(CKFINITE); }
"clt"					{ OPCODE_NONE_P(CLT); }
"clt.un"				{ OPCODE_NONE_P(CLT_UN); }
"conv.i"				{ OPCODE_NONE(CONV_I); }
"conv.i1"				{ OPCODE_NONE(CONV_I1); }
"conv.i2"				{ OPCODE_NONE(CONV_I2); }
"conv.i4"				{ OPCODE_NONE(CONV_I4); }
"conv.i8"				{ OPCODE_NONE(CONV_I8); }
"conv.ovf.i"			{ OPCODE_NONE(CONV_OVF_I); }
"conv.ovf.i.un"			{ OPCODE_NONE(CONV_OVF_I_UN); }
"conv.ovf.i1"			{ OPCODE_NONE(CONV_OVF_I1); }
"conv.ovf.i1.un"		{ OPCODE_NONE(CONV_OVF_I1_UN); }
"conv.ovf.i2"			{ OPCODE_NONE(CONV_OVF_I2); }
"conv.ovf.i2.un"		{ OPCODE_NONE(CONV_OVF_I2_UN); }
"conv.ovf.i4"			{ OPCODE_NONE(CONV_OVF_I4); }
"conv.ovf.i4.un"		{ OPCODE_NONE(CONV_OVF_I4_UN); }
"conv.ovf.i8"			{ OPCODE_NONE(CONV_OVF_I8); }
"conv.ovf.i8.un"		{ OPCODE_NONE(CONV_OVF_I8_UN); }
"conv.ovf.u"			{ OPCODE_NONE(CONV_OVF_U); }
"conv.ovf.u.un"			{ OPCODE_NONE(CONV_OVF_U_UN); }
"conv.ovf.u1"			{ OPCODE_NONE(CONV_OVF_U1); }
"conv.ovf.u1.un"		{ OPCODE_NONE(CONV_OVF_U1_UN); }
"conv.ovf.u2"			{ OPCODE_NONE(CONV_OVF_U2); }
"conv.ovf.u2.un"		{ OPCODE_NONE(CONV_OVF_U2_UN); }
"conv.ovf.u4"			{ OPCODE_NONE(CONV_OVF_U4); }
"conv.ovf.u4.un"		{ OPCODE_NONE(CONV_OVF_U4_UN); }
"conv.ovf.u8"			{ OPCODE_NONE(CONV_OVF_U8); }
"conv.ovf.u8.un"		{ OPCODE_NONE(CONV_OVF_U8_UN); }
"conv.r.un"				{ OPCODE_NONE(CONV_R_UN); }
"conv.r4"				{ OPCODE_NONE(CONV_R4); }
"conv.r8"				{ OPCODE_NONE(CONV_R8); }
"conv.u"				{ OPCODE_NONE(CONV_U); }
"conv.u1"				{ OPCODE_NONE(CONV_U1); }
"conv.u2"				{ OPCODE_NONE(CONV_U2); }
"conv.u4"				{ OPCODE_NONE(CONV_U4); }
"conv.u8"				{ OPCODE_NONE(CONV_U8); }
"cpblk"					{ OPCODE_NONE_P(CPBLK); }
"div"					{ OPCODE_NONE(DIV); }
"div.un"				{ OPCODE_NONE(DIV_UN); }
"dup"					{ OPCODE_NONE(DUP); }
"endfault"				{ OPCODE_NONE(ENDFINALLY); }	/* Alias */
"endfilter"				{ OPCODE_NONE_P(ENDFILTER); }
"endfinally"			{ OPCODE_NONE(ENDFINALLY); }
"initblk"				{ OPCODE_NONE_P(INITBLK); }
"ldarg.0"				{ OPCODE_NONE(LDARG_0); }
"ldarg.1"				{ OPCODE_NONE(LDARG_1); }
"ldarg.2"				{ OPCODE_NONE(LDARG_2); }
"ldarg.3"				{ OPCODE_NONE(LDARG_3); }
"ldc.i4.0"				{ OPCODE_NONE(LDC_I4_0); }
"ldc.i4.1"				{ OPCODE_NONE(LDC_I4_1); }
"ldc.i4.2"				{ OPCODE_NONE(LDC_I4_2); }
"ldc.i4.3"				{ OPCODE_NONE(LDC_I4_3); }
"ldc.i4.4"				{ OPCODE_NONE(LDC_I4_4); }
"ldc.i4.5"				{ OPCODE_NONE(LDC_I4_5); }
"ldc.i4.6"				{ OPCODE_NONE(LDC_I4_6); }
"ldc.i4.7"				{ OPCODE_NONE(LDC_I4_7); }
"ldc.i4.8"				{ OPCODE_NONE(LDC_I4_8); }
"ldc.i4.m1"				{ OPCODE_NONE(LDC_I4_M1); }
"ldc.i4.M1"				{ OPCODE_NONE(LDC_I4_M1); }	/* Alias */
"ldelem.i"				{ OPCODE_NONE(LDELEM_I); }
"ldelem.i1"				{ OPCODE_NONE(LDELEM_I1); }
"ldelem.i2"				{ OPCODE_NONE(LDELEM_I2); }
"ldelem.i4"				{ OPCODE_NONE(LDELEM_I4); }
"ldelem.i8"				{ OPCODE_NONE(LDELEM_I8); }
"ldelem.r4"				{ OPCODE_NONE(LDELEM_R4); }
"ldelem.r8"				{ OPCODE_NONE(LDELEM_R8); }
"ldelem.ref"			{ OPCODE_NONE(LDELEM_REF); }
"ldelem.u1"				{ OPCODE_NONE(LDELEM_U1); }
"ldelem.u2"				{ OPCODE_NONE(LDELEM_U2); }
"ldelem.u4"				{ OPCODE_NONE(LDELEM_U4); }
"ldelem.u8"				{ OPCODE_NONE(LDELEM_I8); }	/* Alias */
"ldind.i"				{ OPCODE_NONE(LDIND_I); }
"ldind.i1"				{ OPCODE_NONE(LDIND_I1); }
"ldind.i2"				{ OPCODE_NONE(LDIND_I2); }
"ldind.i4"				{ OPCODE_NONE(LDIND_I4); }
"ldind.i8"				{ OPCODE_NONE(LDIND_I8); }
"ldind.r4"				{ OPCODE_NONE(LDIND_R4); }
"ldind.r8"				{ OPCODE_NONE(LDIND_R8); }
"ldind.ref"				{ OPCODE_NONE(LDIND_REF); }
"ldind.u1"				{ OPCODE_NONE(LDIND_U1); }
"ldind.u2"				{ OPCODE_NONE(LDIND_U2); }
"ldind.u4"				{ OPCODE_NONE(LDIND_U4); }
"ldind.u8"				{ OPCODE_NONE(LDIND_I8); }	/* Alias */
"ldlen"					{ OPCODE_NONE(LDLEN); }
"ldloc.0"				{ OPCODE_NONE(LDLOC_0); }
"ldloc.1"				{ OPCODE_NONE(LDLOC_1); }
"ldloc.2"				{ OPCODE_NONE(LDLOC_2); }
"ldloc.3"				{ OPCODE_NONE(LDLOC_3); }
"ldnull"				{ OPCODE_NONE(LDNULL); }
"localloc"				{ OPCODE_NONE_P(LOCALLOC); }
"mul"					{ OPCODE_NONE(MUL); }
"mul.un"				{ OPCODE_NONE(MUL_OVF_UN); } /* Hack for missing insn */
"mul.ovf"				{ OPCODE_NONE(MUL_OVF); }
"mul.ovf.un"			{ OPCODE_NONE(MUL_OVF_UN); }
"neg"					{ OPCODE_NONE(NEG); }
"nop"					{ OPCODE_NONE(NOP); }
"not"					{ OPCODE_NONE(NOT); }
"or"					{ OPCODE_NONE(OR); }
"pop"					{ OPCODE_NONE(POP); }
"refanytype"			{ OPCODE_NONE_P(REFANYTYPE); }
"rem"					{ OPCODE_NONE(REM); }
"rem.un"				{ OPCODE_NONE(REM_UN); }
"ret"					{ OPCODE_NONE(RET); }
"rethrow"				{ OPCODE_NONE_P(RETHROW); }
"shl"					{ OPCODE_NONE(SHL); }
"shr"					{ OPCODE_NONE(SHR); }
"shr.un"				{ OPCODE_NONE(SHR_UN); }
"stelem.i"				{ OPCODE_NONE(STELEM_I); }
"stelem.i1"				{ OPCODE_NONE(STELEM_I1); }
"stelem.i2"				{ OPCODE_NONE(STELEM_I2); }
"stelem.i4"				{ OPCODE_NONE(STELEM_I4); }
"stelem.i8"				{ OPCODE_NONE(STELEM_I8); }
"stelem.r4"				{ OPCODE_NONE(STELEM_R4); }
"stelem.r8"				{ OPCODE_NONE(STELEM_R8); }
"stelem.ref"			{ OPCODE_NONE(STELEM_REF); }
"stind.i"				{ OPCODE_NONE(STIND_I); }
"stind.i1"				{ OPCODE_NONE(STIND_I1); }
"stind.i2"				{ OPCODE_NONE(STIND_I2); }
"stind.i4"				{ OPCODE_NONE(STIND_I4); }
"stind.i8"				{ OPCODE_NONE(STIND_I8); }
"stind.r4"				{ OPCODE_NONE(STIND_R4); }
"stind.r8"				{ OPCODE_NONE(STIND_R8); }
"stind.ref"				{ OPCODE_NONE(STIND_REF); }
"stloc.0"				{ OPCODE_NONE(STLOC_0); }
"stloc.1"				{ OPCODE_NONE(STLOC_1); }
"stloc.2"				{ OPCODE_NONE(STLOC_2); }
"stloc.3"				{ OPCODE_NONE(STLOC_3); }
"sub"					{ OPCODE_NONE(SUB); }
"sub.ovf"				{ OPCODE_NONE(SUB_OVF); }
"sub.ovf.un"			{ OPCODE_NONE(SUB_OVF_UN); }
"tail."					{ OPCODE_NONE_P(TAIL); }
"tail"					{ OPCODE_NONE_P(TAIL); }		/* Alias */
"throw"					{ OPCODE_NONE(THROW); }
"volatile."				{ OPCODE_NONE_P(VOLATILE); }
"volatile"				{ OPCODE_NONE_P(VOLATILE); }	/* Alias */
"xor"					{ OPCODE_NONE(XOR); }

"ann.dead"				{ OPCODE_VAR(ANN_DEAD); }	/* Local var opcodes */
"ann.live"				{ OPCODE_VAR_P(ANN_LIVE); }
"ann.ref"				{ OPCODE_VAR_P(ANN_REF); }
"ann.ref.s"				{ OPCODE_VAR(ANN_REF_S); }
"ldarg"					{ OPCODE_VAR_P(LDARG); }
"ldarg.s"				{ OPCODE_VAR(LDARG_S); }
"ldarga"				{ OPCODE_VAR_P(LDARGA); }
"ldarga.s"				{ OPCODE_VAR(LDARGA_S); }
"ldloc"					{ OPCODE_VAR_P(LDLOC); }
"ldloc.s"				{ OPCODE_VAR(LDLOC_S); }
"ldloca"				{ OPCODE_VAR_P(LDLOCA); }
"ldloca.s"				{ OPCODE_VAR(LDLOCA_S); }
"starg"					{ OPCODE_VAR_P(STARG); }
"starg.s"				{ OPCODE_VAR(STARG_S); }
"stloc"					{ OPCODE_VAR_P(STLOC); }
"stloc.s"				{ OPCODE_VAR(STLOC_S); }

"ldc.i4"				{ yylval.opcode = IL_OP_LDC_I4; /* Constant opcodes */
						  return I_INT; }
"ldc.i4.s"				{ yylval.opcode = IL_OP_LDC_I4_S;
						  return I_INT; }
"ldc.i8"				{ yylval.opcode = IL_OP_LDC_I8;
						  return I_INT; }
"ldc.r4"				{ yylval.opcode = IL_OP_LDC_R4;
						  return I_FLOAT; }
"ldc.r8"				{ yylval.opcode = IL_OP_LDC_R8;
						  return I_FLOAT; }

"ann.data"				{ OPCODE_BRANCH_P(ANN_DATA); } /* Branch opcodes */
"ann.data.s"			{ OPCODE_BRANCH(ANN_DATA_S); }
"beq"					{ OPCODE_BRANCH(BEQ); }
"beq.s"					{ OPCODE_BRANCH(BEQ_S); }
"bge"					{ OPCODE_BRANCH(BGE); }
"bge.s"					{ OPCODE_BRANCH(BGE_S); }
"bge.un"				{ OPCODE_BRANCH(BGE_UN); }
"bge.un.s"				{ OPCODE_BRANCH(BGE_UN_S); }
"bgt"					{ OPCODE_BRANCH(BGT); }
"bgt.s"					{ OPCODE_BRANCH(BGT_S); }
"bgt.un"				{ OPCODE_BRANCH(BGT_UN); }
"bgt.un.s"				{ OPCODE_BRANCH(BGT_UN_S); }
"ble"					{ OPCODE_BRANCH(BLE); }
"ble.s"					{ OPCODE_BRANCH(BLE_S); }
"ble.un"				{ OPCODE_BRANCH(BLE_UN); }
"ble.un.s"				{ OPCODE_BRANCH(BLE_UN_S); }
"blt"					{ OPCODE_BRANCH(BLT); }
"blt.s"					{ OPCODE_BRANCH(BLT_S); }
"blt.un"				{ OPCODE_BRANCH(BLT_UN); }
"blt.un.s"				{ OPCODE_BRANCH(BLT_UN_S); }
"bne.un"				{ OPCODE_BRANCH(BNE_UN); }
"bne.un.s"				{ OPCODE_BRANCH(BNE_UN_S); }
"br"					{ OPCODE_BRANCH(BR); }
"br.s"					{ OPCODE_BRANCH(BR_S); }
"brfalse"				{ OPCODE_BRANCH(BRFALSE); }
"brfalse.s"				{ OPCODE_BRANCH(BRFALSE_S); }
"brtrue"				{ OPCODE_BRANCH(BRTRUE); }
"brtrue.s"				{ OPCODE_BRANCH(BRTRUE_S); }
"leave"					{ OPCODE_BRANCH(LEAVE); }
"leave.s"				{ OPCODE_BRANCH(LEAVE_S); }

"brnull"				{ OPCODE_BRANCH(BRFALSE); }	/* Branch aliases */
"brnull.s"				{ OPCODE_BRANCH(BRFALSE_S); }
"brzero"				{ OPCODE_BRANCH(BRFALSE); }
"brzero.s"				{ OPCODE_BRANCH(BRFALSE_S); }
"brinst"				{ OPCODE_BRANCH(BRTRUE); }
"brinst.s"				{ OPCODE_BRANCH(BRTRUE_S); }

"ann.call"				{ OPCODE_METHOD(ANN_CALL); } /* Method opcodes */
"ann.hoisted_call"		{ OPCODE_METHOD(ANN_HOISTED_CALL); }
"call"					{ OPCODE_METHOD(CALL); }
"callvirt"				{ OPCODE_METHOD(CALLVIRT); }
"jmp"					{ OPCODE_METHOD(JMP); }
"ldftn"					{ OPCODE_METHOD_P(LDFTN); }
"ldvirtftn"				{ OPCODE_METHOD_P(LDVIRTFTN); }
"newobj"				{ OPCODE_METHOD(NEWOBJ); }

"ldfld"					{ OPCODE_FIELD(LDFLD); }	/* Field opcodes */
"ldflda"				{ OPCODE_FIELD(LDFLDA); }
"ldsfld"				{ OPCODE_FIELD(LDSFLD); }
"ldsflda"				{ OPCODE_FIELD(LDSFLDA); }
"stfld"					{ OPCODE_FIELD(STFLD); }
"stsfld"				{ OPCODE_FIELD(STSFLD); }

"box"					{ OPCODE_TYPE(BOX); }		/* Type opcodes */
"castclass"				{ OPCODE_TYPE(CASTCLASS); }
"cpobj"					{ OPCODE_TYPE(CPOBJ); }
"initobj"				{ OPCODE_TYPE_P(INITOBJ); }
"isinst"				{ OPCODE_TYPE(ISINST); }
"ldelema"				{ OPCODE_TYPE(LDELEMA); }
"ldobj"					{ OPCODE_TYPE(LDOBJ); }
"mkrefany"				{ OPCODE_TYPE(MKREFANY); }
"newarr"				{ OPCODE_TYPE(NEWARR); }
"refanyval"				{ OPCODE_TYPE(REFANYVAL); }
"sizeof"				{ OPCODE_TYPE_P(SIZEOF); }
"stobj"					{ OPCODE_TYPE(STOBJ); }
"unbox"					{ OPCODE_TYPE(UNBOX); }

"ldstr"					{ yylval.opcode = IL_OP_LDSTR;	/* String opcodes */
						  return I_STRING; }

"calli"					{ yylval.opcode = IL_OP_CALLI;	/* Signature opcodes */
						  return I_SIGNATURE; }
"jmpi"					{ yylval.opcode = 0xFE00 | IL_PREFIX_OP_JMPI;
						  return I_SIGNATURE; }

"ldptr"					{ yylval.opcode = IL_OP_LDPTR;	/* RVA opcodes */
						  return I_RVA; }

"ldtoken"				{ yylval.opcode = IL_OP_LDTOKEN; /* Metadata opcodes */
						  return I_TOKEN; }

"ann.phi"				{ yylval.opcode = IL_OP_ANN_PHI; /* SSA opcodes */
						  return I_SSA; }

"switch"				{ yylval.opcode = IL_OP_SWITCH;	/* Switch opcodes */
						  return I_SWITCH; }

{IDALPHA}({DIGIT}|{IDALPHA})*	{
			yylval.strValue = ILInternString(yytext, strlen(yytext));
			return IDENTIFIER;
		}

"."{IDALPHA}({DIGIT}|{IDALPHA})*	{
			yylval.strValue = ILInternString(yytext, strlen(yytext));
			return DOT_IDENTIFIER;
		}

{SIGN}?{DIGIT}+{EXPONENT}		{ yylval.real = ParseFloat(yytext);
								  return FLOAT_CONSTANT; }
{SIGN}?{DIGIT}*"."{DIGIT}+({EXPONENT})?	{
			yylval.real = ParseFloat(yytext);
			return FLOAT_CONSTANT;
		}
{SIGN}?{DIGIT}+"."{DIGIT}*({EXPONENT})?	{
			yylval.real = ParseFloat(yytext);
			return FLOAT_CONSTANT;
		}

0[xX]{HEX}+				{ yylval.integer = ParseHexInt(yytext);
						  return INTEGER_CONSTANT; }
-0[xX]{HEX}+			{ yylval.integer = -(ParseHexInt(yytext + 1));
						  return INTEGER_CONSTANT; }
0[0-7]*					{ yylval.integer = ParseOctInt(yytext);
						  return INTEGER_CONSTANT; }
-0[0-7]*				{ yylval.integer = -(ParseOctInt(yytext + 1));
						  return INTEGER_CONSTANT; }
{DIGIT}+				{ yylval.integer = ParseDecInt(yytext);
						  return INTEGER_CONSTANT; }
-{DIGIT}+				{ yylval.integer = -(ParseDecInt(yytext + 1));
						  return INTEGER_CONSTANT; }

'(\\.|[^\\'])+'			{ yylval.strValue =
								ParseString(yytext, &(yylval.strValue.len));
						  return SQUOTE_STRING; }
\"(\\.|[^\\"])*\"		{ yylval.strValue =
								ParseString(yytext, &(yylval.strValue.len));
						  return DQUOTE_STRING; }

<*>"//"				{
						/* Skip a single-line comment */
						int ch = input();
						while(ch != EOF && ch != '\n')
						{
							ch = input();
						}
						++ILAsmLineNum;
					}
<*>"/*"				{
						/* Skip a multi-line comment */
						int ch;
						for(;;)
						{
							ch = input();
							while(ch != '*' && ch != EOF)
							{
								if(ch == '\n')
								{
									++ILAsmLineNum;
								}
								ch = input();
							}
							if(ch == '*')
							{
								ch = input();
								while(ch == '*')
								{
									ch = input();
								}
								if(ch == '/')
								{
									break;
								}
								else if(ch == '\n')
								{
									++ILAsmLineNum;
								}
							}
							if(ch == EOF)
							{
								break;
							}
						}
					}

<*>{WHITE}+			;

"\357\273\277"		{ /* Weird Microsoft UTF-8 marker */ }

<*>\n				{ ++ILAsmLineNum; }

.					{ return (((int)(yytext[0])) & 0xFF); }

<HEXMODE>.			{ /* illegal character which hex bytes */ }

%%
