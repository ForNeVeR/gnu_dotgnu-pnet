/*
 * il_crypt.h - Implementation of useful cryptographic algorithms.
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

#ifndef	_IL_CRYPT_H
#define	_IL_CRYPT_H

#include "il_values.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * The size of SHA1 hash values.
 */
#define	IL_SHA_HASH_SIZE		20

/*
 * Context block for SHA1.
 */
typedef struct _tagILSHAContext
{
	unsigned char	input[64];
	ILUInt32		inputLen;
	ILUInt32		A, B, C, D, E;
	ILUInt64		totalLen;

} ILSHAContext;

/*
 * Initialize a SHA1 context block.
 */
void ILSHAInit(ILSHAContext *sha);

/*
 * Input more data into a SHA1 context block.
 */
void ILSHAData(ILSHAContext *sha, const void *buffer, unsigned long len);

/*
 * Finalize a SHA1 context block and output the hash.
 */
void ILSHAFinalize(ILSHAContext *sha, unsigned char hash[IL_SHA_HASH_SIZE]);

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_CRYPT_H */
