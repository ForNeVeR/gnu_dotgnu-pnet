/*
 * c_internal.h - Internal definitions for the C compiler front end.
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

#ifndef	_CSCC_C_INTERNAL_H
#define	_CSCC_C_INTERNAL_H

#include <cscc/common/cc_main.h>
#include <cscc/c/c_defs.h>
#include <cscc/c/c_lexutils.h>
#include <cscc/c/c_types.h>
#include <cscc/c/c_declspec.h>
#include <cscc/c/c_function.h>
#include <codegen/cg_scope.h>
#include <cscc/c/c_scope.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Flag that is used to indicate a 32-bit only compile.
 */
extern int gen_32bit_only;

/*
 * Duplicate a block of memory to be stored as an extra field
 * in a semantic analysis value block.
 */
void *CSemDupExtra(const void *buf, unsigned int len);

/*
 * Generate "crt0" glue logic if the current module has "main".
 */
void CGenCrt0(ILGenInfo *info, FILE *stream);

/*
 * End the code generation process, flushing remaining definitions.
 */
void CGenEndCode(ILGenInfo *info);

/*
 * Output the attributes that are attached to a program item.
 */
void CGenOutputAttributes(ILGenInfo *info, FILE *stream, ILProgramItem *item);

#ifdef	__cplusplus
};
#endif

#endif	/* _CSCC_C_INTERNAL_H */
