/*
 * cs_preproc.h - Pre-processor for C# source code.
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

#ifndef	_CSCC_CS_PREPROC_H
#define	_CSCC_CS_PREPROC_H

#include <stdio.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Information about a defined symbol.  In the C# language,
 * symbols can be defined to either "true" or "false" only.
 */
typedef struct _tagCSPreProcSymbol
{
	struct _tagCSPreProcSymbol *next;
	char		name[1];

} CSPreProcSymbol;

/*
 * Information on a specific line.
 */
typedef struct _tagCSPreProcLine
{
	int					posn;
	int					len;
	int					directive;
	unsigned long		number;
	char			   *filename;

} CSPreProcLine;

/*
 * Information about a conditional inclusion scope.
 */
typedef struct _tagCSPreProcScope
{
	int					active : 1;		/* Actively including this scope */
	int					previous : 1;	/* Previous branch already taken */
	int					ancestor : 1;	/* Ancestor is actively including */
	int					sawElse : 1;	/* Already seen '#else' */
	unsigned long		number;			/* Line number of the '#if' */
	char			   *filename;		/* Filename of the '#if' */
	struct _tagCSPreProcScope *next;	/* Next outer scope level */

} CSPreProcScope;

/*
 * Control structure for the pre-processor.
 */
typedef struct _tagCSPreProc
{
	/* Input stream */
	FILE			   *stream;
	int					sawEOF;
	int					utf8;
	int					closeAtEnd;
	int					docComments;

	/* List of defined symbols */
	CSPreProcSymbol	   *symbols;

	/* Input buffer */
	char			   *buffer;
	int					bufLen;
	int					bufMax;

	/* Current set of input lines to be processed */
	CSPreProcLine      *lines;
	int					numLines;
	int					maxLines;
	int					currLine;
	int					continueLine;
	int					linePosn;

	/* Line number information for the next line of real input */
	unsigned long		lineNumber;
	char			   *filename;

	/* Pre-processor scope information for conditional inclusions */
	CSPreProcScope	   *currentScope;
	int					reportedUnmatched;

	/* Line number information reported to the lexer */
	unsigned long		lexerLineNumber;
	char			   *lexerFileName;

	/* Out of memory indicator */
	int					outOfMemory;

	/* Error indicator (i.e. a #error directive was processed) */
	int					error;

} CSPreProc;

/*
 * Initialize a pre-processor control structure.
 */
void CSPreProcInit(CSPreProc *preproc, FILE *stream, const char *filename,
				   int closeAtEnd);

/*
 * Destroy a pre-processor control structure.
 */
void CSPreProcDestroy(CSPreProc *preproc);

/*
 * Define a symbol.
 */
void CSPreProcDefine(CSPreProc *preproc, const char *name);

/*
 * Undefine a symbol.
 */
void CSPreProcUndefine(CSPreProc *preproc, const char *name);

/*
 * Determine if a symbol is currently defined.
 */
int CSPreProcIsDefined(CSPreProc *preproc, const char *name);

/*
 * Get a buffer of input for a lex-based scanner.  This is
 * typically used in the YY_INPUT macro for lex like so:
 *
 *		#define YY_INPUT(buf,result,maxSize) \
 *			{ \
 *				result = CSPreProcGetBuffer(preproc, buf, maxSize); \
 *			}
 *
 * This function will not get the next line until the current
 * line has been completely exhausted by lex.  This ensures that
 * the line number information in "*preproc" always reflects
 * the line of data being processed by lex.
 */
int CSPreProcGetBuffer(CSPreProc *preproc, char *buf, int maxSize);

#ifdef	__cplusplus
};
#endif

#endif	/* _CSCC_CS_PREPROC_H */
