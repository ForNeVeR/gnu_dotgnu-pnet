%{
/*
 * cs_scanner.l - Input file for lex that defines the C# token syntax.
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

#include <stdio.h>
#include "cs_internal.h"
#include "cs_grammar.h"

extern YYSTYPE yylval;
#ifdef YYLTYPE
extern YYLTYPE yylloc;
#endif

CSPreProc CSPreProcessorStream;
int       CSGetSetKeywords = 0;

/*
 * Get the next line of input from the C# pre-processor.
 */
#define	YY_INPUT(buf,result,maxSize)	\
	{ \
		result = CSPreProcGetBuffer(&CSPreProcessorStream, buf, maxSize); \
	}

/*
 * Update the line number information and return a token code.
 */
#ifdef YYLTYPE
#define	RETURNTOK(x)		\
			do { \
				yylloc.first_line = CSPreProcessorStream.lexerLineNumber; \
				return (x); \
			} while (0)
#else
#define	RETURNTOK(x)		return (x)
#endif

unsigned long CSCurrentLine(void)
{
	return CSPreProcessorStream.lexerLineNumber;
}

char *CSCurrentFilename(void)
{
	return CSPreProcessorStream.lexerFileName;
}

/*
 * Parse a floating point quantity into yylval.
 */
static int ParseFloat(char *text)
{
	char *endptr;

#ifdef HAVE_STRTOD
	/* Use "strtod" to parse the value */
	yylval.real.value = (ILDouble)(strtod(text, &endptr));
#else
	/* Use "sscanf" to parse the value */
	double result;
	int num;
	if(sscanf(text, "%lf%n", &result, &num) <= 0)
	{
		/* Shouldn't happen, but do something reasonable */
		yylval.real.value = (ILDouble)0.0;
		yylval.real.type = CS_NUMTYPE_FLOAT64;
		return FLOAT_CONSTANT;
	}
	yylval.real.value = (ILDouble)result;
	endptr = text + num;
#endif

	/* Process the floating point suffix, if present */
	if(*endptr == 'f' || *endptr == 'F')
	{
		yylval.real.type = CS_NUMTYPE_FLOAT32;
		return FLOAT_CONSTANT;
	}
	else if(*endptr == 'm' || *endptr == 'M')
	{
		/* Decimal values cannot be represented by the ILDouble
		   type, so we use a different conversion routine */
		if(!ILDecimalParse(&(yylval.decimal), text, IL_DECIMAL_ROUND_HALF_EVEN))
		{
			CSError("decimal constant is out of range");
			ILDecimalFromInt32(&(yylval.decimal), 0);
		}
		return DECIMAL_CONSTANT;
	}
	else
	{
		yylval.real.type = CS_NUMTYPE_FLOAT64;
		return FLOAT_CONSTANT;
	}
}

/*
 * Parse an integer quantity into yylval.
 */
static void ParseInt(char *text)
{
	ILUInt64 value = 0;
	ILUInt64 digit;
	int errorType = 0;

	/* Parse the main parse of the value */
	if(*text == '0' && (text[1] == 'x' || text[1] == 'X'))
	{
		/* Hexadecimal integer constant */
		text += 2;
		while(*text != '\0')
		{
			if(*text >= 'A' && *text <= 'F')
			{
				digit = (ILUInt64)(*text - 'A' + 10);
			}
			else if(*text >= 'a' && *text <= 'f')
			{
				digit = (ILUInt64)(*text - 'a' + 10);
			}
			else if(*text >= '0' && *text <= '9')
			{
				digit = (ILUInt64)(*text - '0');
			}
			else
			{
				break;
			}
			if((value & 0xF000000000000000ULL) != 0)
			{
				errorType |= 1;
			}
			value = (value << 4) + digit;
			++text;
		}
	}
	else if(*text == '0' && text[1] != '\0')
	{
		/* The C# standard does not support octal integer constants,
		   but we have provided an option to turn them back on */
		if(CSStringListContains(extension_flags, num_extension_flags,
								"octal-constants"))
		{
			/* Octal integer constant: only active with "-foctal-constants" */
			while(*text != '\0')
			{
				if(*text >= '0' && *text <= '7')
				{
					digit = (ILUInt64)(*text - '0');
				}
				else if(*text == '8' || *text == '9')
				{
					/* Cannot use '8' and '9' in octal constants! */
					errorType |= 2;
					digit = 0;
				}
				else
				{
					break;
				}
				if((value & 0xE000000000000000ULL) != 0)
				{
					errorType |= 1;
				}
				value = (value << 3) + digit;
				++text;
			}
		}
		else
		{
			/* Parse as a decimal constant, but warn the user first */
			if(!CSStringListContains(extension_flags, num_extension_flags,
									 "decimal-constants"))
			{
				CSTypedWarning("-octal-constants",
							   "parsing decimal constant, but it may be octal");
				CSTypedWarning("-octal-constants",
							   "(use -foctal-constants to parse as octal, or");
				CSTypedWarning("-octal-constants",
					   "-fdecimal-constants to force the use of decimal)");
			}
			goto decimal;
		}
	}
	else
	{
		/* Decimal integer constant */
	decimal:
		while(*text != '\0')
		{
			if(*text >= '0' && *text <= '9')
			{
				if((value & 0x8000000000000000ULL) != 0)
				{
					errorType |= 1;
				}
				value <<= 1;
				if((value & 0xC000000000000000ULL) != 0)
				{
					errorType |= 1;
				}
				digit = value << 2;
				if((value += digit) < digit)
				{
					errorType |= 1;
				}
				digit = (ILUInt64)(*text - '0');
				if((value += digit) < digit)
				{
					errorType |= 1;
				}
			}
			else
			{
				break;
			}
			++text;
		}
	}

	/* Report errors */
	if(errorType)
	{
		if((errorType & 1) != 0)
		{
			CSError("integer constant is too large");
		}
		if((errorType & 2) != 0)
		{
			CSError("invalid octal constant");
		}
		value = 0;
	}

	/* Set the return value */
	yylval.integer.value = value;

	/* Parse the type information and determine if the value can be negated */
	if((*text == 'u' || *text == 'U') && text[1] == '\0')
	{
		/* Unsigned constant */
		if((value & 0xFFFFFFFF00000000ULL) != 0)
		{
			yylval.integer.type = CS_NUMTYPE_UINT64;
			yylval.integer.canneg = 0;
		}
		else
		{
			yylval.integer.type = CS_NUMTYPE_UINT32;
			yylval.integer.canneg = 0;
		}
	}
	else if((*text == 'l' || *text == 'L') && text[1] == '\0')
	{
		/* Long constant */
		if(value == 0x8000000000000000ULL)
		{
			yylval.integer.type = CS_NUMTYPE_UINT64;
			yylval.integer.canneg = 1;
		}
		else if((value & 0x8000000000000000ULL) != 0)
		{
			yylval.integer.type = CS_NUMTYPE_UINT64;
			yylval.integer.canneg = 0;
		}
		else
		{
			yylval.integer.type = CS_NUMTYPE_INT64;
			yylval.integer.canneg = 1;
		}
	}
	else if(*text != '\0')
	{
		/* Unsigned long constant */
		yylval.integer.type = CS_NUMTYPE_UINT64;
		yylval.integer.canneg = 0;
	}
	else
	{
		/* Untyped constant */
		if(value == 0x8000000000000000ULL)
		{
			yylval.integer.type = CS_NUMTYPE_UINT64;
			yylval.integer.canneg = 1;
		}
		else if((value & 0x8000000000000000ULL) != 0)
		{
			yylval.integer.type = CS_NUMTYPE_UINT64;
			yylval.integer.canneg = 0;
		}
		else if((value & 0xFFFFFFFF00000000ULL) != 0)
		{
			yylval.integer.type = CS_NUMTYPE_INT64;
			yylval.integer.canneg = 1;
		}
		else if(value == 0x0000000080000000ULL)
		{
			yylval.integer.type = CS_NUMTYPE_UINT32;
			yylval.integer.canneg = 1;
		}
		else if((value & 0x0000000080000000ULL) != 0)
		{
			yylval.integer.type = CS_NUMTYPE_UINT32;
			yylval.integer.canneg = 0;
		}
		else
		{
			yylval.integer.type = CS_NUMTYPE_INT32;
			yylval.integer.canneg = 1;
		}
	}
}

/*
 * Parse a single hex digit.
 */
#define	PARSE_HEX_DIGIT		\
			do { \
				if(*temp >= '0' && *temp <= '9') \
				{ \
					*value = *value * 16 + (ILUInt32)(*temp - '0'); \
					++temp; \
				} \
				else if(*temp >= 'A' && *temp <= 'F') \
				{ \
					*value = *value * 16 + (ILUInt32)(*temp - 'A' + 10); \
					++temp; \
				} \
				else if(*temp >= 'a' && *temp <= 'f') \
				{ \
					*value = *value * 16 + (ILUInt32)(*temp - 'a' + 10); \
					++temp; \
				} \
			} while (0)

/*
 * Parse an escape sequence within a character or string.
 */
static int ParseEscape(const char *temp, ILUInt32 *value)
{
	const char *begin = temp;
	static char escapechars[] =
		"\007\010cde\014ghijklm\012opq\015s\011u\013wxyz";

	if(*temp == 'x')
	{
		/* Simple Latin-1 hex character */
		++temp;
		*value = 0;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
	}
	else if(*temp == 'u')
	{
		/* 16-bit Unicode character */
		++temp;
		*value = 0;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
	}
	else if(*temp == 'U')
	{
		/* 32-bit Unicode character */
		++temp;
		*value = 0;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
		PARSE_HEX_DIGIT;
	}
	else if(*temp >= 'a' && *temp <= 'z')
	{
		*value = (ILUInt32)(escapechars[*temp - 'a']);
		if(*value == (ILUInt32)(*temp))
		{
			CSTypedWarning("-unknown-escapes",
						   "unknown escape sequence `\\%c'", *temp);
		}
		++temp;
	}
	else if(*temp >= '0' && *temp <= '7')
	{
		/* Octal character constant.  The C# standard only supports
		   '\0', but we allow others to be used also, with a warning */
		if(*temp != '0' || (temp[1] >= '0' && temp[1] <= '7'))
		{
			CSTypedWarning("-octal-chars",
			               "non-standard octal character escape used");
		}
		*value = 0;
		do
		{
			*value = *value * 8 + (ILUInt32)(*temp - '0');
			++temp;
		}
		while(*temp >= '0' && *temp <= '7');
	}
	else
	{
		if(*temp != '\'' && *temp != '"' && *temp != '\\')
		{
			CSTypedWarning("-unknown-escapes",
						   "unknown escape sequence `\\%c'", *temp);
		}
		*value = (((ILUInt32)(*temp)) & (ILUInt32)0xFF);
		++temp;
	}
	return (int)(temp - begin);
}

/*
 * Parse a character according to the prevailing charset encoding.
 * Returns the length of the sequence.
 */
static int ParseEncodedChar(const char *text, ILUInt32 *value)
{
	const char *begin = text;

	if(CSStringListContains(extension_flags, num_extension_flags,
							"latin1-charset"))
	{
		/* Latin-1 character */
		*value = (((ILUInt32)(*text)) & (ILUInt32)0xFF);
		++text;
	}
	else if((*text & (char)0xE0) == (char)0xC0 &&
	        (text[1] & (char)0xC0) == (char)0x80)
	{
		/* Two-byte UTF-8 character */
		*value = ((((ILUInt32)(*text)) & 0x1F) << 6) |
		          (((ILUInt32)(text[1])) & 0x3F);
	}
	else if((*text & (char)0xF0) == (char)0xE0 &&
	        (text[1] & (char)0xC0) == (char)0x80 &&
	        (text[2] & (char)0xC0) == (char)0x80)
	{
		/* Three-byte UTF-8 character */
		*value = ((((ILUInt32)(*text)) & 0x0F) << 12) |
		         ((((ILUInt32)(text[1])) & 0x3F) << 6) |
		          (((ILUInt32)(text[2])) & 0x3F);
	}
	else if((*text & (char)0xF8) == (char)0xF0 &&
	        (text[1] & (char)0xC0) == (char)0x80 &&
	        (text[2] & (char)0xC0) == (char)0x80 &&
	        (text[3] & (char)0xC0) == (char)0x80)
	{
		/* Four-byte UTF-8 character */
		*value = ((((ILUInt32)(*text)) & 0x07) << 18) |
		         ((((ILUInt32)(text[1])) & 0x3F) << 12) |
		         ((((ILUInt32)(text[2])) & 0x3F) << 6) |
		          (((ILUInt32)(text[3])) & 0x3F);
	}
	else if((*text & (char)0xFC) == (char)0xF8 &&
	        (text[1] & (char)0xC0) == (char)0x80 &&
	        (text[2] & (char)0xC0) == (char)0x80 &&
	        (text[3] & (char)0xC0) == (char)0x80 &&
	        (text[4] & (char)0xC0) == (char)0x80)
	{
		/* Five-byte UTF-8 character */
		*value = ((((ILUInt32)(*text)) & 0x03) << 24) |
		         ((((ILUInt32)(text[1])) & 0x3F) << 18) |
		         ((((ILUInt32)(text[2])) & 0x3F) << 12) |
		         ((((ILUInt32)(text[3])) & 0x3F) << 6) |
		          (((ILUInt32)(text[4])) & 0x3F);
	}
	else if((*text & (char)0xFC) == (char)0xFC &&
	        (text[1] & (char)0xC0) == (char)0x80 &&
	        (text[2] & (char)0xC0) == (char)0x80 &&
	        (text[3] & (char)0xC0) == (char)0x80 &&
	        (text[4] & (char)0xC0) == (char)0x80)
	{
		/* Six-byte UTF-8 character */
		*value = ((((ILUInt32)(*text)) & 0x03) << 30) |
		         ((((ILUInt32)(text[1])) & 0x3F) << 24) |
		         ((((ILUInt32)(text[2])) & 0x3F) << 18) |
		         ((((ILUInt32)(text[3])) & 0x3F) << 12) |
		         ((((ILUInt32)(text[4])) & 0x3F) << 6) |
		          (((ILUInt32)(text[5])) & 0x3F);
	}
	else
	{
		/* Invalid sequence: treat it as an 8-bit character and skip it */
		CSTypedWarning("-invalid-utf8", "invalid UTF-8 sequence");
		*value = (((ILUInt32)(*text)) & (ILUInt32)0xFF);
		++text;
	}

	return (int)(text - begin);
}

/*
 * Parse a wide character value.  Returns the length of the parsed data.
 */
static int ParseWChar(const char *text, ILUInt32 *result)
{
	const char *begin = text;

	/* Parse the character */
	if(*text == '\\')
	{
		/* Escaped character */
		++text;
		text += ParseEscape(text, result);
	}
	else if((*text & (char)0x80) == 0)
	{
		/* Normal 7-bit character */
		*result = (((ILUInt32)(*text)) & (ILUInt32)0xFF);
		++text;
	}
	else
	{
		/* Character in the prevailing charset encoding */
		text += ParseEncodedChar(text, result);
	}

	/* Return the length of the character to the caller */
	return (int)(text - begin);
}

/*
 * Parse a character constant value.
 */
static ILUInt32 ParseCharConstant(const char *text)
{
	ILUInt32 value;

	/* Skip the leading "'" */
	++text;

	/* Parse the character */
	text += ParseWChar(text, &value);

	/* Warn if the character is greater than 16-bits */
	if(value >= (ILUInt32)0x00010000)
	{
		CSTypedWarning("-large-unicode-chars",
					   "32-bit Unicode character `\\U%08lX' used: truncating "
				  	   "to 16 bits", (unsigned long)value);
	}

	/* Make sure that the constant is terminated correctly */
	if(*text != '\'')
	{
		CSTypedWarning("-multi-char-constant", "multi-character constant");
	}

	/* Return the value to the caller */
	return value;
}

/*
 * Parse a string value, converting it into UTF-8.
 */
static ILIntString ParseString(char *text)
{
	char buffer[128];
	ILUInt32 ch;
	int posn;
	ILIntString intern;
	ILIntString append;

	/* Initialize the value to be returned */
	intern.string = 0;
	intern.len = 0;
	posn = 0;

	/* Read characters from the string and encode them in UTF-8 */
	while(*text != '"' && *text != '\0')
	{
		/* Get the next character */
		text += ParseWChar(text, &ch);

		/* Encode it in UTF-8 */
		if(!ch)
		{
			/* Encode embedded NUL's using 2 bytes to protect
			   them from higher-level code that may try to use
			   the string as a normal C string */
			buffer[posn++] = (char)0xC0;
			buffer[posn++] = (char)0x80;
		}
		else if(ch < 0x80)
		{
			buffer[posn++] = (char)ch;
		}
		else if(ch < (1 << 11))
		{
			buffer[posn++] = (char)((ch >> 6) | 0xC0);
			buffer[posn++] = (char)((ch & 0x3F) | 0x80);
		}
		else if(ch < (1 << 16))
		{
			buffer[posn++] = (char)((ch >> 12) | 0xE0);
			buffer[posn++] = (char)(((ch >> 6) & 0x3F) | 0x80);
			buffer[posn++] = (char)((ch & 0x3F) | 0x80);
		}
		else if(ch < (1 << 21))
		{
			buffer[posn++] = (char)((ch >> 18) | 0xF0);
			buffer[posn++] = (char)(((ch >> 12) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 6) & 0x3F) | 0x80);
			buffer[posn++] = (char)((ch & 0x3F) | 0x80);
		}
		else if(ch < (1 << 26))
		{
			buffer[posn++] = (char)((ch >> 24) | 0xF8);
			buffer[posn++] = (char)(((ch >> 18) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 12) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 6) & 0x3F) | 0x80);
			buffer[posn++] = (char)((ch & 0x3F) | 0x80);
		}
		else
		{
			buffer[posn++] = (char)((ch >> 30) | 0xFC);
			buffer[posn++] = (char)(((ch >> 24) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 18) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 12) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 6) & 0x3F) | 0x80);
			buffer[posn++] = (char)((ch & 0x3F) | 0x80);
		}

		/* Flush the buffer into "intern" if it is nearly full */
		if(posn >= 120)
		{
			if(intern.len)
			{
				append.string = buffer;
				append.len = posn;
				intern = ILInternAppendedString(intern, append);
			}
			else
			{
				intern = ILInternString(buffer, posn);
			}
			posn = 0;
		}
	}

	/* Flush the remainder of the buffer into "intern" */
	if(intern.len)
	{
		append.string = buffer;
		append.len = posn;
		intern = ILInternAppendedString(intern, append);
	}
	else
	{
		intern = ILInternString(buffer, posn);
	}

	/* Return the final string to the caller */
	return intern;
}

/*
 * Parse a literal string value, converting it into UTF-8.
 */
static ILIntString ParseLitString(char *text)
{
	char buffer[128];
	ILUInt32 ch;
	int posn;
	ILIntString intern;
	ILIntString append;

	/* Initialize the value to be returned */
	intern.string = 0;
	intern.len = 0;
	posn = 0;

	/* Skip the leading '@"' */
	++text;

	/* Read characters from the string and encode them in UTF-8 */
	while(*text != '"' || text[1] == '"')
	{
		/* Get the next character */
		if(*text == '"')
		{
			/* Skip a '""' sequence that quotes a '"' */
			text += 2;
			ch = (ILUInt32)'"';
		}
		else
		{
			/* Regular character */
			text += ParseEncodedChar(text, &ch);
		}

		/* Encode it in UTF-8 */
		if(!ch)
		{
			/* Encode embedded NUL's using 2 bytes to protect
			   them from higher-level code that may try to use
			   the string as a normal C string */
			buffer[posn++] = (char)0xC0;
			buffer[posn++] = (char)0x80;
		}
		else if(ch < 0x80)
		{
			buffer[posn++] = (char)ch;
		}
		else if(ch < (1 << 11))
		{
			buffer[posn++] = (char)((ch >> 6) | 0xC0);
			buffer[posn++] = (char)((ch & 0x3F) | 0x80);
		}
		else if(ch < (1 << 16))
		{
			buffer[posn++] = (char)((ch >> 12) | 0xE0);
			buffer[posn++] = (char)(((ch >> 6) & 0x3F) | 0x80);
			buffer[posn++] = (char)((ch & 0x3F) | 0x80);
		}
		else if(ch < (1 << 21))
		{
			buffer[posn++] = (char)((ch >> 18) | 0xF0);
			buffer[posn++] = (char)(((ch >> 12) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 6) & 0x3F) | 0x80);
			buffer[posn++] = (char)((ch & 0x3F) | 0x80);
		}
		else if(ch < (1 << 26))
		{
			buffer[posn++] = (char)((ch >> 24) | 0xF8);
			buffer[posn++] = (char)(((ch >> 18) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 12) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 6) & 0x3F) | 0x80);
			buffer[posn++] = (char)((ch & 0x3F) | 0x80);
		}
		else
		{
			buffer[posn++] = (char)((ch >> 30) | 0xFC);
			buffer[posn++] = (char)(((ch >> 24) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 18) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 12) & 0x3F) | 0x80);
			buffer[posn++] = (char)(((ch >> 6) & 0x3F) | 0x80);
			buffer[posn++] = (char)((ch & 0x3F) | 0x80);
		}

		/* Flush the buffer into "intern" if it is nearly full */
		if(posn >= 120)
		{
			if(intern.len)
			{
				append.string = buffer;
				append.len = posn;
				intern = ILInternAppendedString(intern, append);
			}
			else
			{
				intern = ILInternString(buffer, posn);
			}
			posn = 0;
		}
	}

	/* Flush the remainder of the buffer into "intern" */
	if(intern.len)
	{
		append.string = buffer;
		append.len = posn;
		intern = ILInternAppendedString(intern, append);
	}
	else
	{
		intern = ILInternString(buffer, posn);
	}

	/* Return the final string to the caller */
	return intern;
}

%}

%option outfile="lex.yy.c"
%option prefix="cs_"
%option noyywrap
%option nounput

DIGIT			[0-9]
HEX				[a-fA-F0-9]
IDALPHA			([a-zA-Z_\200-\377]|\\u{HEX}{HEX}{HEX}{HEX}|\\U{HEX}{HEX}{HEX}{HEX}{HEX}{HEX}{HEX}{HEX})
EXPONENT		[Ee][+-]?{DIGIT}+
FTYPE			(f|F|d|D|m|M)
ITYPE			(U|u|L|l|UL|Ul|uL|ul|LU|Lu|lU|lu)
WHITE			[ \t\r\f\v\032]

%%

"++"					{ RETURNTOK(INC_OP); }
"--"					{ RETURNTOK(DEC_OP); }
"<<"					{ RETURNTOK(LEFT_OP); }
">>"					{ RETURNTOK(RIGHT_OP); }
"<="					{ RETURNTOK(LE_OP); }
">="					{ RETURNTOK(GE_OP); }
"=="					{ RETURNTOK(EQ_OP); }
"!="					{ RETURNTOK(NE_OP); }
"&&"					{ RETURNTOK(AND_OP); }
"||"					{ RETURNTOK(OR_OP); }
"*="					{ RETURNTOK(MUL_ASSIGN_OP); }
"/="					{ RETURNTOK(DIV_ASSIGN_OP); }
"%="					{ RETURNTOK(MOD_ASSIGN_OP); }
"+="					{ RETURNTOK(ADD_ASSIGN_OP); }
"-="					{ RETURNTOK(SUB_ASSIGN_OP); }
"<<="					{ RETURNTOK(LEFT_ASSIGN_OP); }
">>="					{ RETURNTOK(RIGHT_ASSIGN_OP); }
"&="					{ RETURNTOK(AND_ASSIGN_OP); }
"^="					{ RETURNTOK(XOR_ASSIGN_OP); }
"|="					{ RETURNTOK(OR_ASSIGN_OP); }
"->"					{ RETURNTOK(PTR_OP); }

"abstract"				{ RETURNTOK(ABSTRACT); }
"as"					{ RETURNTOK(AS); }
"base"					{ RETURNTOK(BASE); }
"bool"					{ RETURNTOK(BOOL); }
"break"					{ RETURNTOK(BREAK); }
"__builtin_constant"	{ RETURNTOK(BUILTIN_CONSTANT); }
"byte"					{ RETURNTOK(BYTE); }
"case"					{ RETURNTOK(CASE); }
"catch"					{ RETURNTOK(CATCH); }
"char"					{ RETURNTOK(CHAR); }
"checked"				{ RETURNTOK(CHECKED); }
"class"					{ RETURNTOK(CLASS); }
"const"					{ RETURNTOK(CONST); }
"continue"				{ RETURNTOK(CONTINUE); }
"decimal"				{ RETURNTOK(DECIMAL); }
"default"				{ RETURNTOK(DEFAULT); }
"delegate"				{ RETURNTOK(DELEGATE); }
"do"					{ RETURNTOK(DO); }
"double"				{ RETURNTOK(DOUBLE); }
"else"					{ RETURNTOK(ELSE); }
"enum"					{ RETURNTOK(ENUM); }
"event"					{ RETURNTOK(EVENT); }
"explicit"				{ RETURNTOK(EXPLICIT); }
"extern"				{ RETURNTOK(EXTERN); }
"false"					{ RETURNTOK(FALSE); }
"finally"				{ RETURNTOK(FINALLY); }
"fixed"					{ RETURNTOK(FIXED); }
"float"					{ RETURNTOK(FLOAT); }
"for"					{ RETURNTOK(FOR); }
"foreach"				{ RETURNTOK(FOREACH); }
"goto"					{ RETURNTOK(GOTO); }
"if"					{ RETURNTOK(IF); }
"implicit"				{ RETURNTOK(IMPLICIT); }
"in"					{ RETURNTOK(IN); }
"int"					{ RETURNTOK(INT); }
"interface"				{ RETURNTOK(INTERFACE); }
"internal"				{ RETURNTOK(INTERNAL); }
"is"					{ RETURNTOK(IS); }
"lock"					{ RETURNTOK(LOCK); }
"long"					{ RETURNTOK(LONG); }
"namespace"				{ RETURNTOK(NAMESPACE); }
"new"					{ RETURNTOK(NEW); }
"null"					{ RETURNTOK(NULL_TOK); }
"object"				{ RETURNTOK(OBJECT); }
"operator"				{ RETURNTOK(OPERATOR); }
"out"					{ RETURNTOK(OUT); }
"override"				{ RETURNTOK(OVERRIDE); }
"params"				{ RETURNTOK(PARAMS); }
"private"				{ RETURNTOK(PRIVATE); }
"protected"				{ RETURNTOK(PROTECTED); }
"public"				{ RETURNTOK(PUBLIC); }
"readonly"				{ RETURNTOK(READONLY); }
"ref"					{ RETURNTOK(REF); }
"return"				{ RETURNTOK(RETURN); }
"sbyte"					{ RETURNTOK(SBYTE); }
"sealed"				{ RETURNTOK(SEALED); }
"short"					{ RETURNTOK(SHORT); }
"sizeof"				{ RETURNTOK(SIZEOF); }
"stackalloc"			{ RETURNTOK(STACKALLOC); }
"static"				{ RETURNTOK(STATIC); }
"string"				{ RETURNTOK(STRING); }
"struct"				{ RETURNTOK(STRUCT); }
"switch"				{ RETURNTOK(SWITCH); }
"this"					{ RETURNTOK(THIS); }
"throw"					{ RETURNTOK(THROW); }
"true"					{ RETURNTOK(TRUE); }
"try"					{ RETURNTOK(TRY); }
"typeof"				{ RETURNTOK(TYPEOF); }
"uint"					{ RETURNTOK(UINT); }
"ulong"					{ RETURNTOK(ULONG); }
"unchecked"				{ RETURNTOK(UNCHECKED); }
"unsafe"				{ RETURNTOK(UNSAFE); }
"ushort"				{ RETURNTOK(USHORT); }
"using"					{ RETURNTOK(USING); }
"virtual"				{ RETURNTOK(VIRTUAL); }
"void"					{ RETURNTOK(VOID); }
"while"					{ RETURNTOK(WHILE); }

{IDALPHA}({DIGIT}|{IDALPHA})*		{
			if(!CSGetSetKeywords)
			{
				yylval.name = (ParseString(yytext)).string;
				RETURNTOK(IDENTIFIER);
			}
			else if(!strcmp(yytext, "get"))
			{
				RETURNTOK(GET);
			}
			else if(!strcmp(yytext, "set"))
			{
				RETURNTOK(SET);
			}
			else if(!strcmp(yytext, "add"))
			{
				RETURNTOK(ADD);
			}
			else if(!strcmp(yytext, "remove"))
			{
				RETURNTOK(REMOVE);
			}
			else
			{
				yylval.name = (ParseString(yytext)).string;
				RETURNTOK(IDENTIFIER);
			}
		}
"@"{IDALPHA}({DIGIT}|{IDALPHA})*	{
			yylval.name = (ParseString(yytext)).string;
			RETURNTOK(IDENTIFIER);
		}

{DIGIT}+{EXPONENT}{FTYPE}?					{ RETURNTOK(ParseFloat(yytext)); }
{DIGIT}*"."{DIGIT}+({EXPONENT})?{FTYPE}?	{ RETURNTOK(ParseFloat(yytext)); }
{DIGIT}+"."{DIGIT}*({EXPONENT})?{FTYPE}?	{ RETURNTOK(ParseFloat(yytext)); }
{DIGIT}+{FTYPE}								{ RETURNTOK(ParseFloat(yytext)); }

0[xX]{HEX}+{ITYPE}?			{ ParseInt(yytext); RETURNTOK(INTEGER_CONSTANT); }
{DIGIT}+{ITYPE}?			{ ParseInt(yytext); RETURNTOK(INTEGER_CONSTANT); }

'(\\.|[^\\'])+'	{
			yylval.charValue = (ILUInt16)(ParseCharConstant(yytext));
		  	RETURNTOK(CHAR_CONSTANT);
		}

\"(\\.|[^\\"])*\"			{ yylval.string = ParseString(yytext + 1);
							  RETURNTOK(STRING_LITERAL); }
"@"\"("@".|[^@"])*\"		{ yylval.string = ParseLitString(yytext);
							  RETURNTOK(STRING_LITERAL); }

{WHITE}+			;

\n					;

"///"[^\n]*\n		{ yylval.string = ILInternString(yytext + 3, -1);
					  RETURNTOK(DOC_COMMENT); }

.					{ RETURNTOK(((int)(yytext[0])) & 0xFF); }

%%
