/*
 * ildasm_internal.h - Internal definitions for the disassembler.
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

#ifndef	_ILDASM_INTERNAL_H
#define	_ILDASM_INTERNAL_H

#include <stdio.h>
#include "il_system.h"
#include "il_image.h"
#include "il_dumpasm.h"
#include "il_program.h"
#include "il_opcodes.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Extra flags for altering the output, in addition to "IL_DUMP_xxx".
 */
#define	ILDASM_REAL_OFFSETS			(1 << 8)
#define	ILDASM_SUPPRESS_PREFIX		(1 << 9)
#define	ILDASM_NO_IL				(1 << 10)
#define	ILDASM_INSTRUCTION_BYTES	(1 << 11)
#define ILDASM_RESOLVE_ALL			(1 << 12)

/*
 * Dump a binary blob to an output stream.
 */
void ILDAsmDumpBinaryBlob(FILE *outstream, ILImage *image,
						  const void *blob, ILUInt32 blobLen);

/*
 * Walk a list of tokens and call a supplied callback for each one.
 */
typedef void (*ILDAsmWalkFunc)(ILImage *image, FILE *outstream, int flags,
							   unsigned long token, void *data,
							   unsigned long refToken);
void ILDAsmWalkTokens(ILImage *image, FILE *outstream, int flags,
					  unsigned long tokenKind, ILDAsmWalkFunc callback,
					  unsigned long refToken);

/*
 * Dump the contents of a method.
 */
void ILDAsmDumpMethod(ILImage *image, FILE *outstream,
					  ILMethod *method, int flags,
					  int isEntryPoint);

/*
 * Dump the contents of a Java method.
 */
void ILDAsmDumpJavaMethod(ILImage *image, FILE *outstream,
					      ILMethod *method, int flags);

/*
 * Dump custom attributes for a program item.
 */
void ILDAsmDumpCustomAttrs(ILImage *image, FILE *outstream, int flags,
						   int indent, ILProgramItem *item);

/*
 * Dump global definitions such as modules, assemblies, etc.
 */
void ILDAsmDumpGlobal(ILImage *image, FILE *outstream, int flags);

/*
 * Dump all class definitions.
 */
void ILDAsmDumpClasses(ILImage *image, FILE *outstream, int flags);

/*
 * Dump the security information associated with a program item.
 */
void ILDAsmDumpSecurity(ILImage *image, FILE *outstream,
						ILProgramItem *item, int flags);

/*
 * Dump the ".data" and ".tls" sections.
 */
void ILDAsmDumpDataSections(FILE *outstream, ILImage *image);

#ifdef	__cplusplus
};
#endif

#endif	/* _ILDASM_INTERNAL_H */
