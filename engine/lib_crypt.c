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
#include "il_bignum.h"

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
#define	IL_ALG_DSA_SIGN			9
#define	IL_ALG_RSA_ENCRYPT		10
#define	IL_ALG_RSA_SIGN			11

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
 * Symmetric algorithm context header.
 */
typedef void (*SymResetFunc)(void *ctx);
typedef void (*SymCryptFunc)(void *ctx, unsigned char *input,
							 unsigned char *output);
typedef struct
{
	SymResetFunc	reset;
	SymCryptFunc	encrypt;
	SymCryptFunc	decrypt;

} SymContext;

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
 * Symmetric context for DES.
 */
typedef struct
{
	SymContext		sym;
	ILDESContext	des;

} DESContext;

/*
 * Symmetric context for Triple-DES.
 */
typedef struct
{
	SymContext		sym;
	ILDES3Context	des3;

} DES3Context;

/*
 * Symmetric context for RC2.
 */
typedef struct
{
	SymContext		sym;
	ILRC2Context	rc2;

} RC2Context;

/*
 * Symmetric context for AES/Rijndael.
 */
typedef struct
{
	SymContext		sym;
	ILAESContext	aes;

} AESContext;

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
		case IL_ALG_SHA512:
		case IL_ALG_DES:
		case IL_ALG_TRIPLE_DES:
		case IL_ALG_RC2:
		case IL_ALG_RIJNDAEL: return 1;
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
	return ILDESIsSemiWeakKey(((unsigned char *)ArrayToBuffer(key)) + offset);
}

/*
 * public static bool IsWeakKey(byte[] key, int offset);
 */
ILBool _IL_CryptoMethods_IsWeakKey(ILExecThread *_thread, System_Array *key,
								   ILInt32 offset)
{
	return ILDESIsWeakKey(((unsigned char *)ArrayToBuffer(key)) + offset);
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
	SymContext *context;
	switch(algorithm)
	{
		case IL_ALG_DES:
		{
			/* Create and initialize a DES encryption context */
			context = (SymContext *)ILMalloc(sizeof(DESContext));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (SymResetFunc)ILDESFinalize;
			context->encrypt = (SymCryptFunc)ILDESProcess;
			context->decrypt = (SymCryptFunc)ILDESProcess;
			ILDESInit(&(((DESContext *)context)->des),
					  ArrayToBuffer(key), 0);
			return (ILNativeInt)context;
		}
		/* Not reached */

		case IL_ALG_TRIPLE_DES:
		{
			/* Create and initialize a Triple-DES encryption context */
			context = (SymContext *)ILMalloc(sizeof(DES3Context));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (SymResetFunc)ILDES3Finalize;
			context->encrypt = (SymCryptFunc)ILDES3Process;
			context->decrypt = (SymCryptFunc)ILDES3Process;
			ILDES3Init(&(((DES3Context *)context)->des3),
					   ArrayToBuffer(key), (int)(key->length * 8), 0);
			return (ILNativeInt)context;
		}
		/* Not reached */

		case IL_ALG_RC2:
		{
			/* Create and initialize an RC2 encryption context */
			context = (SymContext *)ILMalloc(sizeof(RC2Context));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (SymResetFunc)ILRC2Finalize;
			context->encrypt = (SymCryptFunc)ILRC2Encrypt;
			context->decrypt = (SymCryptFunc)ILRC2Decrypt;
			ILRC2Init(&(((RC2Context *)context)->rc2),
					  ArrayToBuffer(key), (int)(key->length * 8));
			return (ILNativeInt)context;
		}
		/* Not reached */

		case IL_ALG_RIJNDAEL:
		{
			/* Create and initialize an AES/Rijndael encryption context */
			context = (SymContext *)ILMalloc(sizeof(AESContext));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (SymResetFunc)ILAESFinalize;
			context->encrypt = (SymCryptFunc)ILAESEncrypt;
			context->decrypt = (SymCryptFunc)ILAESDecrypt;
			ILAESInit(&(((AESContext *)context)->aes),
					  ArrayToBuffer(key), (int)(key->length * 8));
			return (ILNativeInt)context;
		}
		/* Not reached */
	}
	ILExecThreadSetException(_thread,
			_ILSystemException(_thread, "System.NotImplementedException"));
	return 0;
}

/*
 * public static IntPtr EncryptCreate(int algorithm, byte[] key);
 */
ILNativeInt _IL_CryptoMethods_DecryptCreate(ILExecThread *_thread,
											ILInt32 algorithm,
											System_Array *key)
{
	SymContext *context;

	/* DES and Triple-DES need to set up the key schedule
	   in a different manner for decryption */
	switch(algorithm)
	{
		case IL_ALG_DES:
		{
			/* Create and initialize a DES encryption context */
			context = (SymContext *)ILMalloc(sizeof(DESContext));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (SymResetFunc)ILDESFinalize;
			context->encrypt = (SymCryptFunc)ILDESProcess;
			context->decrypt = (SymCryptFunc)ILDESProcess;
			ILDESInit(&(((DESContext *)context)->des),
					  ArrayToBuffer(key), 1);
			return (ILNativeInt)context;
		}
		/* Not reached */

		case IL_ALG_TRIPLE_DES:
		{
			/* Create and initialize a Triple-DES encryption context */
			context = (SymContext *)ILMalloc(sizeof(DES3Context));
			if(!context)
			{
				ILExecThreadThrowOutOfMemory(_thread);
				return 0;
			}
			context->reset = (SymResetFunc)ILDES3Finalize;
			context->encrypt = (SymCryptFunc)ILDES3Process;
			context->decrypt = (SymCryptFunc)ILDES3Process;
			ILDES3Init(&(((DES3Context *)context)->des3),
					   ArrayToBuffer(key), (int)(key->length * 8), 1);
			return (ILNativeInt)context;
		}
		/* Not reached */
	}

	/* The other algorithms use the same key schedule for
	   encryption and decryption */
	return _IL_CryptoMethods_EncryptCreate(_thread, algorithm, key);
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
	(*(((SymContext *)state)->encrypt))
			(&(((RC2Context *)state)->rc2),
			 ((unsigned char *)(ArrayToBuffer(inBuffer))) + inOffset,
			 ((unsigned char *)(ArrayToBuffer(outBuffer))) + outOffset);
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
	(*(((SymContext *)state)->decrypt))
			(&(((RC2Context *)state)->rc2),
			 ((unsigned char *)(ArrayToBuffer(inBuffer))) + inOffset,
			 ((unsigned char *)(ArrayToBuffer(outBuffer))) + outOffset);
}

/*
 * public static void SymmetricFree(IntPtr state);
 */
void _IL_CryptoMethods_SymmetricFree(ILExecThread *_thread, ILNativeInt state)
{
	(*(((SymContext *)state)->reset))(&(((RC2Context *)state)->rc2));
	ILFree((void *)state);
}

/*
 * Convert 1 to 3 "byte[]" arrays into "ILBigNum" values.
 * Returns zero if out of memory.
 */
static int ByteArraysToBigNums(ILExecThread *_thread,
							   System_Array *x, ILBigNum **xbig,
							   System_Array *y, ILBigNum **ybig,
							   System_Array *z, ILBigNum **zbig)
{
	if(x)
	{
		*xbig = ILBigNumFromBytes((unsigned char *)(ArrayToBuffer(x)),
								  x->length);
		if(!(*xbig))
		{
			ILExecThreadThrowOutOfMemory(_thread);
			return 0;
		}
	}
	if(y)
	{
		*ybig = ILBigNumFromBytes((unsigned char *)(ArrayToBuffer(y)),
								  y->length);
		if(!(*ybig))
		{
			if(x)
			{
				ILBigNumFree(*xbig);
			}
			ILExecThreadThrowOutOfMemory(_thread);
			return 0;
		}
	}
	if(z)
	{
		*zbig = ILBigNumFromBytes((unsigned char *)(ArrayToBuffer(z)),
								  z->length);
		if(!(*zbig))
		{
			if(x)
			{
				ILBigNumFree(*xbig);
			}
			if(y)
			{
				ILBigNumFree(*ybig);
			}
			ILExecThreadThrowOutOfMemory(_thread);
			return 0;
		}
	}
	return 1;
}

/*
 * Convert a big number into a byte array.
 */
static System_Array *BigNumToByteArray(ILExecThread *_thread, ILBigNum *x)
{
	if(x)
	{
		ILInt32 size = ILBigNumByteCount(x);
		System_Array *array =
			(System_Array *)ILExecThreadNew(_thread, "[B", "(Ti)V",
											(ILVaInt)size);
		if(array)
		{
			ILBigNumToBytes(x, (unsigned char *)(ArrayToBuffer(array)));
		}
		ILBigNumFree(x);
		return array;
	}
	else
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return 0;
	}
}

/*
 * public static byte[] NumAdd(byte[] x, byte[] y, byte[] modulus);
 */
System_Array *_IL_CryptoMethods_NumAdd(ILExecThread *_thread,
									   System_Array *x,
									   System_Array *y,
									   System_Array *modulus)
{
	ILBigNum *xbig, *ybig, *modbig, *result;
	if(!ByteArraysToBigNums(_thread, x, &xbig, y, &ybig, modulus, &modbig))
	{
		return 0;
	}
	result = ILBigNumAdd(xbig, ybig, modbig);
	ILBigNumFree(xbig);
	ILBigNumFree(ybig);
	ILBigNumFree(modbig);
	return BigNumToByteArray(_thread, result);
}

/*
 * public static byte[] NumMul(byte[] x, byte[] y, byte[] modulus);
 */
System_Array *_IL_CryptoMethods_NumMul(ILExecThread *_thread,
									   System_Array *x,
									   System_Array *y,
									   System_Array *modulus)
{
	ILBigNum *xbig, *ybig, *modbig, *result;
	if(!ByteArraysToBigNums(_thread, x, &xbig, y, &ybig, modulus, &modbig))
	{
		return 0;
	}
	result = ILBigNumMul(xbig, ybig, modbig);
	ILBigNumFree(xbig);
	ILBigNumFree(ybig);
	ILBigNumFree(modbig);
	return BigNumToByteArray(_thread, result);
}

/*
 * public static byte[] NumPow(byte[] x, byte[] y, byte[] modulus);
 */
System_Array *_IL_CryptoMethods_NumPow(ILExecThread *_thread,
									   System_Array *x,
									   System_Array *y,
									   System_Array *modulus)
{
	ILBigNum *xbig, *ybig, *modbig, *result;
	if(!ByteArraysToBigNums(_thread, x, &xbig, y, &ybig, modulus, &modbig))
	{
		return 0;
	}
	result = ILBigNumPow(xbig, ybig, modbig);
	ILBigNumFree(xbig);
	ILBigNumFree(ybig);
	ILBigNumFree(modbig);
	return BigNumToByteArray(_thread, result);
}

/*
 * public static byte[] NumInv(byte[] x, byte[] modulus);
 */
System_Array *_IL_CryptoMethods_NumInv(ILExecThread *_thread,
									   System_Array *x,
									   System_Array *modulus)
{
	ILBigNum *xbig, *modbig, *result;
	if(!ByteArraysToBigNums(_thread, x, &xbig, 0, 0, modulus, &modbig))
	{
		return 0;
	}
	result = ILBigNumInv(xbig, modbig);
	ILBigNumFree(xbig);
	ILBigNumFree(modbig);
	return BigNumToByteArray(_thread, result);
}

/*
 * public static byte[] NumMod(byte[] x, byte[] modulus);
 */
System_Array *_IL_CryptoMethods_NumMod(ILExecThread *_thread,
									   System_Array *x,
									   System_Array *modulus)
{
	ILBigNum *xbig, *modbig, *result;
	if(!ByteArraysToBigNums(_thread, x, &xbig, 0, 0, modulus, &modbig))
	{
		return 0;
	}
	result = ILBigNumMod(xbig, modbig);
	ILBigNumFree(xbig);
	ILBigNumFree(modbig);
	return BigNumToByteArray(_thread, result);
}

/*
 * public static bool NumEq(byte[] x, byte[] y);
 */
ILBool _IL_CryptoMethods_NumEq(ILExecThread *_thread,
							   System_Array *x,
							   System_Array *y)
{
	ILBigNum *xbig, *ybig;
	ILBool result;
	if(!ByteArraysToBigNums(_thread, x, &xbig, y, &ybig, 0, 0))
	{
		return 0;
	}
	result = (ILBigNumCompare(xbig, ybig) == 0);
	ILBigNumFree(xbig);
	ILBigNumFree(ybig);
	return result;
}

/*
 * public static bool NumZero(byte[] x);
 */
ILBool _IL_CryptoMethods_NumZero(ILExecThread *_thread, System_Array *x)
{
	ILBigNum *xbig;
	ILBool result;
	if(!ByteArraysToBigNums(_thread, x, &xbig, 0, 0, 0, 0))
	{
		return 0;
	}
	result = (ILBigNumIsZero(xbig) != 0);
	ILBigNumFree(xbig);
	return result;
}

/*
 * public static byte[] GetKey(int algorithm, String name,
 *							   CspProviderFlags flag, out int result);
 */
System_Array *_IL_CryptoMethods_GetKey(ILExecThread *_thread,
									   ILInt32 algorithm,
									   ILString *name,
									   ILInt32 flag,
									   ILInt32 *result)
{
	/* TODO */
	*result = 1;	/* UnknownKey */
	return 0;
}

/*
 * public static void StoreKey(int algorithm, String name, byte[] key);
 */
void _IL_CryptoMethods_StoreKey(ILExecThread *_thread,
								ILInt32 algorithm,
								ILString *name,
								System_Array *key)
{
	/* TODO */
}

#ifdef	__cplusplus
};
#endif
