/*
 * cs_internal.h - Internal definitions for the C# compiler front end.
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

#ifndef	_CSCC_CS_INTERNAL_H
#define	_CSCC_CS_INTERNAL_H

#include "cs_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Modifier mask bits.
 */
#define	CS_MODIFIER_PUBLIC			(1<<0)
#define	CS_MODIFIER_PRIVATE			(1<<1)
#define	CS_MODIFIER_PROTECTED		(1<<2)
#define	CS_MODIFIER_INTERNAL		(1<<3)
#define	CS_MODIFIER_ACCESS_MASK		(CS_MODIFIER_PUBLIC | \
									 CS_MODIFIER_PRIVATE | \
									 CS_MODIFIER_PROTECTED | \
									 CS_MODIFIER_INTERNAL)
#define	CS_MODIFIER_NEW				(1<<4)
#define	CS_MODIFIER_ABSTRACT		(1<<5)
#define	CS_MODIFIER_SEALED			(1<<6)
#define	CS_MODIFIER_STATIC			(1<<7)
#define	CS_MODIFIER_READONLY		(1<<8)
#define	CS_MODIFIER_VIRTUAL			(1<<9)
#define	CS_MODIFIER_OVERRIDE		(1<<10)
#define	CS_MODIFIER_EXTERN			(1<<11)
#define	CS_MODIFIER_UNSAFE			(1<<12)
#define	CS_MODIFIER_VOLATILE		(1<<13)

/*
 * Special attribute flags.
 */
#define	CS_SPECIALATTR_NEW			0x08000000
#define	CS_SPECIALATTR_UNSAFE		0x04000000
#define	CS_SPECIALATTR_EXTERN		0x02000000
#define	CS_SPECIALATTR_OVERRIDE		0x01000000
#define	CS_SPECIALATTR_VOLATILE		0x00800000

/*
 * Flag bit that is used to distinguish args from locals.
 */
#define	CS_LOCAL_IS_ARG				0x80000000

/*
 * Type values that are used to classify the size of numeric values.
 */
#define	CS_NUMTYPE_INT32			0
#define	CS_NUMTYPE_UINT32			1
#define	CS_NUMTYPE_INT64			2
#define	CS_NUMTYPE_UINT64			3
#define	CS_NUMTYPE_FLOAT32			4
#define	CS_NUMTYPE_FLOAT64			5

/*
 * Pre-processor object that is being used by the lexer.
 */
extern CSPreProc CSPreProcessorStream;

/*
 * A flag that is set to 1 when "get" and "set" keywords
 * should be recognized by the lexical analyser.
 */
extern int CSGetSetKeywords;

/*
 * Error and warning indicators.
 */
extern int CSHaveErrors;
extern int CSHaveWarnings;

/*
 * Code generator control object.
 */
extern ILGenInfo CSCodeGen;

/*
 * Final parse tree for a compilation unit.
 */
extern ILNode *CSParseTree;

/*
 * Global scope.
 */
extern ILScope *CSGlobalScope;

/*
 * Get the current line and filename being processed by the lexer.
 */
unsigned long CSCurrentLine(void);
char *CSCurrentFilename(void);

/*
 * Report an error on the current line.
 */
void CSError(const char *format, ...);

/*
 * Report an error on a specific line.
 */
void CSErrorOnLine(char *filename, unsigned long linenum,
				   const char *format, ...);

/*
 * Report a warning on the current line.
 */
void CSWarning(const char *format, ...);

/*
 * Report a typed warning on the current line.  The warning
 * will only be reported if the "type" is enabled.
 */
void CSTypedWarning(const char *type, const char *format, ...);

/*
 * Report a warning on a specific line.
 */
void CSWarningOnLine(char *filename, unsigned long linenum,
				     const char *format, ...);

/*
 * Report a typed warning on a specific line.  The warning
 * will only be reported if the "type" is enabled.
 */
void CSTypedWarningOnLine(char *filename, unsigned long linenum,
				     	  const char *type, const char *format, ...);

/*
 * Report either a warning or an error about unsafe constructs.
 */
void CSUnsafeMessage(ILGenInfo *info, ILNode *node, const char *format, ...);

/*
 * Report either a warning or an error about unsafe types.
 */
void CSUnsafeTypeMessage(ILGenInfo *info, ILNode *node);

/*
 * Enter an unsafe context, reporting a warning or error
 * about the context as necessary.
 */
void CSUnsafeEnter(ILGenInfo *info, ILNode *node, const char *format, ...);

/*
 * Leave an unsafe context.
 */
void CSUnsafeLeave(ILGenInfo *info);

/*
 * Determine if a type or parameter node contains unsafe types.
 */
int CSHasUnsafeType(ILNode *node);

/*
 * Report that some modifiers have been specified more than once.
 */
void CSModifiersUsedTwice(char *filename, long linenum, ILUInt32 modifiers);

/*
 * Convert modifiers into attribute flag masks for program elements.
 */
ILUInt32 CSModifiersToTypeAttrs(ILNode *node, ILUInt32 modifiers, int isNested);
ILUInt32 CSModifiersToConstAttrs(ILNode *node, ILUInt32 modifiers);
ILUInt32 CSModifiersToFieldAttrs(ILNode *node, ILUInt32 modifiers);
ILUInt32 CSModifiersToMethodAttrs(ILNode *node, ILUInt32 modifiers);
ILUInt32 CSModifiersToEventAttrs(ILNode *node, ILUInt32 modifiers);
ILUInt32 CSModifiersToPropertyAttrs(ILNode *node, ILUInt32 modifiers);
ILUInt32 CSModifiersToOperatorAttrs(ILNode *node, ILUInt32 modifiers);
ILUInt32 CSModifiersToConstructorAttrs(ILNode *node, ILUInt32 modifiers);

/*
 * Convert a built-in constant name into a constant node.
 */
ILNode *CSBuiltinConstant(char *name);

/*
 * Gather information about all types in the program.
 * Returns a new top-level list for the program with
 * the classes re-organised so that parent classes and
 * interfaces precede classes that inherit them.
 */
ILNode *CSTypeGather(ILGenInfo *info, ILScope *globalScope, ILNode *tree);

/*
 * Validate a block of documentation comments to ensure
 * that all XML tags are properly balanced.
 */
void CSValidateDocs(ILNode *docList);

/*
 * Resolve a simple name to a semantic value.
 */
CSSemValue CSResolveSimpleName(ILGenInfo *genInfo, ILNode *node,
							   const char *name);

/*
 * Resolve a member name to a semantic value. 
 */
CSSemValue CSResolveMemberName(ILGenInfo *genInfo, ILNode *node,
							   CSSemValue value, const char *name);

/*
 * Get the n'th member from a method group.
 * Returns NULL at the end of the group.
 */
ILMethod *CSGetGroupMember(void *group, unsigned long n);

/*
 * Remove the n'th member from a method group.
 * Returns the new group.
 */
void *CSRemoveGroupMember(void *group, unsigned long n);

/*
 * Set the candidate form for the n'th member of a method group.
 */
void CSSetGroupMemberForm(void *group, unsigned long n, int form);

/*
 * Get the candidate form for the n'th member of a method group.
 */
int CSGetGroupMemberForm(void *group, unsigned long n);

#ifdef	__cplusplus
};
#endif

#endif	/* _CSCC_CS_INTERNAL_H */
