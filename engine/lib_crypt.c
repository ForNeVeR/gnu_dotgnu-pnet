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
#define	IL_ALG_DES				5
#define	IL_ALG_TRIPLE_DES		6
#define	IL_ALG_RC2				7
#define	IL_ALG_RIJNDAEL			8

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
 * Hash context for the SHA-256 algorithm.
 */
typedef struct
{
	HashContext		hash;
	ILSHA256Context	sha256;

} SHA256HashContext;

/*
 * Hash context for the SHA-384 algorithm.
 */
typedef struct
{
	HashContext		hash;
	ILSHA384Context	sha384;

} SHA384HashContext;

/*
 * Hash context for the SHA-512 algorithm.
 */
typedef struct
{
	HashContext		hash;
	ILSHA512Context	sha512;

} SHA512HashContext;

/*
 * public static bool AlgorithmSupported(int algorithm);
 */
ILBool _IL_CryptoMethods_AlgorithmSupported(ILExecThread *_thread,
											ILInt32 algorithm)
{
	switch(algorithm)
	{
		case IL_ALG_MD5:
		case IL_ALG_SHA1:
		case IL_ALG_SHA256:
		case IL_ALG_SHA384:
		case IL_ALG_SHA512: return 1;
	}
	return 0;
}

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

		case IL_ALG_SHA256:
		{
			/* Create and initialize an SHA-256 context */
			context = (HashContext *)ILMalloc(sizeof(SHA256HashContext));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (HashResetFunc)ILSHA256Init;
			context->update = (HashUpdateFunc)ILSHA256Data;
			context->final = (HashFinalFunc)ILSHA256Finalize;
			ILSHA256Init(&(((SHA256HashContext *)context)->sha256));
			return (ILNativeInt)context;
		}
		/* Not reached */

		case IL_ALG_SHA384:
		{
			/* Create and initialize an SHA-384 context */
			context = (HashContext *)ILMalloc(sizeof(SHA384HashContext));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (HashResetFunc)ILSHA384Init;
			context->update = (HashUpdateFunc)ILSHA512Data;	/* Same as 512 */
			context->final = (HashFinalFunc)ILSHA384Finalize;
			ILSHA384Init(&(((SHA384HashContext *)context)->sha384));
			return (ILNativeInt)context;
		}
		/* Not reached */

		case IL_ALG_SHA512:
		{
			/* Create and initialize an SHA-512 context */
			context = (HashContext *)ILMalloc(sizeof(SHA512HashContext));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (HashResetFunc)ILSHA512Init;
			context->update = (HashUpdateFunc)ILSHA512Data;
			context->final = (HashFinalFunc)ILSHA512Finalize;
			ILSHA512Init(&(((SHA512HashContext *)context)->sha512));
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

/*
 * public static bool IsSemiWeakKey(byte[] key, int offset);
 */
ILBool _IL_CryptoMethods_IsSemiWeakKey(ILExecThread *_thread,
									   System_Array *key, ILInt32 offset)
{
	/* TODO */
	return 0;
}

/*
 * public static bool IsWeakKey(byte[] key, int offset);
 */
ILBool _IL_CryptoMethods_IsWeakKey(ILExecThread *_thread, System_Array *key,
								   ILInt32 offset)
{
	/* TODO */
	return 0;
}

/*
 * public static bool SameKey(byte[] key1, int offset1,
 *							  byte[] key2, int offset2);
 */
ILBool _IL_CryptoMethods_SameKey(ILExecThread *_thread, System_Array *key1,
								 ILInt32 offset1, System_Array *key2,
								 ILInt32 offset2)
{
	return !ILMemCmp(((unsigned char *)(ArrayToBuffer(key1))) + offset1,
					 ((unsigned char *)(ArrayToBuffer(key2))) + offset2, 8);
}

/*
 * pubilc static void GenerateRandom(byte[] buf, int offset, int count);
 */
void _IL_CryptoMethods_GenerateRandom(ILExecThread *_thread,
									  System_Array *buf, ILInt32 offset,
									  ILInt32 count)
{
	/* TODO */
}

/*
 * public static IntPtr EncryptCreate(int algorithm, byte[] key);
 */
ILNativeInt _IL_CryptoMethods_EncryptCreate(ILExecThread *_thread,
											ILInt32 algorithm,
											System_Array *key)
{
	/* TODO */
	return 0;
}

/*
 * public static IntPtr EncryptCreate(int algorithm, byte[] key);
 */
ILNativeInt _IL_CryptoMethods_DecryptCreate(ILExecThread *_thread,
											ILInt32 algorithm,
											System_Array *key)
{
	/* TODO */
	return 0;
}

/*
 * public static void Encrypt(IntPtr state, byte[] inBuffer, int inOffset,
 *							  byte[] outBuffer, int outOffset);
 */
void _IL_CryptoMethods_Encrypt(ILExecThread *_thread,
							   ILNativeInt state, System_Array *inBuffer,
							   ILInt32 inOffset, System_Array *outBuffer,
							   ILInt32 outOffset)
{
	/* TODO */
}

/*
 * public static void Decrypt(IntPtr state, byte[] inBuffer, int inOffset,
 *							  byte[] outBuffer, int outOffset);
 */
void _IL_CryptoMethods_Decrypt(ILExecThread *_thread,
							   ILNativeInt state, System_Array *inBuffer,
							   ILInt32 inOffset, System_Array *outBuffer,
							   ILInt32 outOffset)
{
	/* TODO */
}

/*
 * public static void SymmetricFree(IntPtr state);
 */
void _IL_CryptoMethods_SymmetricFree(ILExecThread *_thread, ILNativeInt state)
{
	/* TODO */
}

#ifdef	__cplusplus
};
#endif
