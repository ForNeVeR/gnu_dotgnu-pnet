/*
 * lib_crypt.c - Internalcall methods for the CryptoMethods class.
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

#include "engine_private.h"
#include "lib_defs.h"
#include "il_crypt.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Algorithm identifiers.  Must be kept in sync with the
 * "Platform.CryptoMethods" class.
 */
#define	IL_ALG_MD5				0
#define	IL_ALG_SHA1				1
#define	IL_ALG_SHA256			2
#define	IL_ALG_SHA384			3
#define	IL_ALG_SHA512			4

/*
 * Hash context header.
 */
typedef void (*HashResetFunc)(void *ctx);
typedef void (*HashUpdateFunc)(void *ctx, const void *buffer,
							   unsigned long len);
typedef void (*HashFinalFunc)(void *ctx, unsigned char *hash);
typedef struct
{
	HashResetFunc	reset;
	HashUpdateFunc	update;
	HashFinalFunc	final;

} HashContext;

/*
 * Hash context for the MD5 algorithm.
 */
typedef struct
{
	HashContext		hash;
	ILMD5Context	md5;

} MD5HashContext;

/*
 * Hash context for the SHA1 algorithm.
 */
typedef struct
{
	HashContext		hash;
	ILSHAContext	sha1;

} SHA1HashContext;

/*
 * public static IntPtr HashNew(int algorithm);
 */
ILNativeInt _IL_CryptoMethods_HashNew(ILExecThread *_thread, ILInt32 algorithm)
{
	HashContext *context;
	switch(algorithm)
	{
		case IL_ALG_MD5:
		{
			/* Create and initialize an MD5 context */
			context = (HashContext *)ILMalloc(sizeof(MD5HashContext));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (HashResetFunc)ILMD5Init;
			context->update = (HashUpdateFunc)ILMD5Data;
			context->final = (HashFinalFunc)ILMD5Finalize;
			ILMD5Init(&(((MD5HashContext *)context)->md5));
			return (ILNativeInt)context;
		}
		/* Not reached */

		case IL_ALG_SHA1:
		{
			/* Create and initialize an SHA1 context */
			context = (HashContext *)ILMalloc(sizeof(SHA1HashContext));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (HashResetFunc)ILSHAInit;
			context->update = (HashUpdateFunc)ILSHAData;
			context->final = (HashFinalFunc)ILSHAFinalize;
			ILSHAInit(&(((SHA1HashContext *)context)->sha1));
			return (ILNativeInt)context;
		}
		/* Not reached */
	}
	ILExecThreadSetException(_thread,
			_ILSystemException(_thread, "System.NotImplementedException"));
	return 0;
}

/*
 * public static void HashReset(IntPtr state);
 */
void _IL_CryptoMethods_HashReset(ILExecThread *_thread, ILNativeInt state)
{
	(*(((HashContext *)state)->reset))(&(((MD5HashContext *)state)->md5));
}

/*
 * public static void HashUpdate(IntPtr state, byte[] buffer,
 *                               int offset, int count);
 */
void _IL_CryptoMethods_HashUpdate(ILExecThread *_thread, ILNativeInt state,
								  System_Array *buffer, ILInt32 offset,
								  ILInt32 count)
{
	(*(((HashContext *)state)->update))
			(&(((MD5HashContext *)state)->md5),
			 ((unsigned char *)(ArrayToBuffer(buffer))) + offset,
			 (unsigned long)count);
}

/*
 * void HashFinal(IntPtr state, byte[] hash);
 */
void _IL_CryptoMethods_HashFinal(ILExecThread *_thread, ILNativeInt state,
								 System_Array *hash)
{
	(*(((HashContext *)state)->final))(&(((MD5HashContext *)state)->md5),
									   (unsigned char *)(ArrayToBuffer(hash)));
}

/*
 * void HashFree(IntPtr state);
 */
void _IL_CryptoMethods_HashFree(ILExecThread *_thread, ILNativeInt state)
{
	(*(((HashContext *)state)->reset))(&(((MD5HashContext *)state)->md5));
	ILFree((void *)state);
}

#ifdef	__cplusplus
};
#endif
