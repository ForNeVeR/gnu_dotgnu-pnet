/*
 * jv_output.c - Assembly code output routines for Java bytecode.
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

#include "cg_nodes.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Definition of a Java constant pool hash entry.
 */
typedef struct _tagILJavaHashEntry ILJavaHashEntry;
struct _tagILJavaHashEntry
{
	ILJavaHashEntry *next;
	ILJavaHashEntry *nextInOrder;
	int				 index;
	int				 valueLen;
	unsigned char    value[1];

};
#define	IL_JAVA_CONST_HASH_SIZE		256

/*
 * Java code generation control structure.
 */
#define	IL_JAVA_MAX_CODE_SIZE		65535
struct _tagILJavaGenInfo
{
	/* Class information */
	unsigned		 accessFlags;
	int				 className;
	int				 parentName;
	int				 numInterfaces;
	int				*interfaces;

	/* Code area */
	unsigned		 javaPC;
	unsigned char	 javaCode[IL_JAVA_MAX_CODE_SIZE];

	/* Storage for the constant pool */
	int				 javaConstIndex;
	ILJavaHashEntry *javaConstHash[IL_JAVA_CONST_HASH_SIZE];
	ILJavaHashEntry *javaConstFirst;
	ILJavaHashEntry *javaConstLast;

};

/*
 * Output a single byte to the Java method code buffer.
 */
static void AbortJava(ILGenInfo *info)
{
	fprintf(stderr, "Java method too large - aborting\n");
	exit(1);
}
#define	OUT_BYTE(byte)	\
			do { \
				if(info->javaInfo->javaPC < IL_JAVA_MAX_CODE_SIZE) \
				{ \
					info->javaInfo->javaCode[(info->javaInfo->javaPC)++] = \
							(unsigned char)(byte); \
				} \
				else \
				{ \
					AbortJava(info); \
				} \
			} while (0)

/*
 * Mark the end of an instruction for peephole optimisation purposes.
 */
#define	MARK_END()	do {} while (0)

void JavaGenInit(ILGenInfo *info)
{
	if((info->javaInfo = (ILJavaGenInfo *)ILCalloc(1, sizeof(ILJavaGenInfo)))
				== 0)
	{
		ILGenOutOfMemory(info);
	}
	info->javaInfo->javaConstIndex = 1;
}

/*
 * Clear the Java code generation state.
 */
static void GenClear(ILJavaGenInfo *jinfo)
{
	ILJavaHashEntry *entry, *nextEntry;
	int hash;

	/* Clear the class meta-information */
	if(jinfo->interfaces)
	{
		ILFree(jinfo->interfaces);
		jinfo->interfaces = 0;
		jinfo->numInterfaces = 0;
	}

	/* Clear the contents of the constant pool */
	for(hash = 0; hash < IL_JAVA_CONST_HASH_SIZE; ++hash)
	{
		entry = jinfo->javaConstHash[hash];
		while(entry != 0)
		{
			nextEntry = entry->next;
			ILFree(entry);
			entry = nextEntry;
		}
		jinfo->javaConstHash[hash] = 0;
	}
	jinfo->javaConstFirst = 0;
	jinfo->javaConstLast = 0;
	jinfo->javaConstIndex = 1;
}

void JavaGenDestroy(ILGenInfo *info)
{
	ILJavaGenInfo *jinfo = info->javaInfo;
	if(jinfo)
	{
		GenClear(jinfo);
		ILFree(jinfo);
		info->javaInfo = 0;
	}
}

void JavaGenClear(ILGenInfo *info)
{
	GenClear(info->javaInfo);
}

void JavaGenSimple(ILGenInfo *info, int opcode)
{
	OUT_BYTE(opcode);
	MARK_END();
}

/*
 * Look for an existing constant pool hash entry.
 * Creates a new entry if necessary.
 */
static ILJavaHashEntry *LookupConst(ILGenInfo *info, unsigned char *value,
									int valueLen, int size)
{
	ILJavaHashEntry *entry;
	unsigned hash = 0;
	int posn;
	for(posn = 0; posn < valueLen; ++posn)
	{
		hash = hash + (hash << 5) + (unsigned)(value[posn]);
	}
	hash &= (IL_JAVA_CONST_HASH_SIZE - 1);
	entry = info->javaInfo->javaConstHash[hash];
	while(entry != 0)
	{
		if(entry->valueLen == valueLen &&
		   !ILMemCmp(entry->value, value, valueLen))
		{
			return entry;
		}
		entry = entry->next;
	}
	if((entry = (ILJavaHashEntry *)ILMalloc
				(sizeof(ILJavaHashEntry) - 1 + (unsigned)valueLen)) == 0)
	{
		ILGenOutOfMemory(info);
	}
	entry->next = info->javaInfo->javaConstHash[hash];
	entry->nextInOrder = 0;
	info->javaInfo->javaConstHash[hash] = entry;
	entry->index = info->javaInfo->javaConstIndex;
	info->javaInfo->javaConstIndex += size;
	entry->valueLen = valueLen;
	ILMemCpy(entry->value, value, valueLen);
	if(info->javaInfo->javaConstLast)
	{
		info->javaInfo->javaConstLast->nextInOrder = entry;
	}
	else
	{
		info->javaInfo->javaConstFirst = entry;
	}
	info->javaInfo->javaConstLast = entry;
	return entry;
}

/*
 * Load a value from the constant pool.
 */
static void LoadConst(ILGenInfo *info, unsigned char *value, int valueLen)
{
	ILJavaHashEntry *entry = LookupConst(info, value, valueLen, 1);
	if(entry->index < 256)
	{
		OUT_BYTE(JAVA_OP_LDC);
		OUT_BYTE(entry->index);
	}
	else
	{
		OUT_BYTE(JAVA_OP_LDC_W);
		OUT_BYTE(entry->index >> 8);
		OUT_BYTE(entry->index);
	}
}

/*
 * Load a double-width value from the constant pool.
 */
static void LoadDConst(ILGenInfo *info, unsigned char *value, int valueLen)
{
	ILJavaHashEntry *entry = LookupConst(info, value, valueLen, 2);
	OUT_BYTE(JAVA_OP_LDC2_W);
	OUT_BYTE(entry->index >> 8);
	OUT_BYTE(entry->index);
}

void JavaGenInt32(ILGenInfo *info, ILInt32 value)
{
	if(value >= ((ILInt32)(-1)) && value <= ((ILInt32)5))
	{
		OUT_BYTE(JAVA_OP_ICONST_0 + value);
	}
	else if(value >= ((ILInt32)(-128)) && value <= ((ILInt32)127))
	{
		OUT_BYTE(JAVA_OP_BIPUSH);
		OUT_BYTE(value);
	}
	else if(value >= ((ILInt32)(-32768)) && value <= ((ILInt32)32767))
	{
		OUT_BYTE(JAVA_OP_SIPUSH);
		OUT_BYTE(value >> 8);
		OUT_BYTE(value);
	}
	else
	{
		unsigned char entry[5];
		entry[0] = (unsigned char)JAVA_CONST_INTEGER;
		entry[1] = (unsigned char)(value >> 24);
		entry[2] = (unsigned char)(value >> 16);
		entry[3] = (unsigned char)(value >> 8);
		entry[4] = (unsigned char)value;
		LoadConst(info, entry, 5);
	}
	MARK_END();
}

void JavaGenUInt32(ILGenInfo *info, ILUInt32 value)
{
	JavaGenInt32(info, (ILInt32)value);
}

void JavaGenInt64(ILGenInfo *info, ILInt64 value)
{
	if(value == 0)
	{
		OUT_BYTE(JAVA_OP_LCONST_0);
	}
	else if(value == 1)
	{
		OUT_BYTE(JAVA_OP_LCONST_1);
	}
	else if(value >= ((ILInt64)IL_MIN_INT32) &&
	        value <= ((ILInt64)IL_MAX_INT32))
	{
		JavaGenInt32(info, (ILInt32)value);
		OUT_BYTE(JAVA_OP_I2L);
	}
	else
	{
		unsigned char entry[9];
		entry[0] = (unsigned char)JAVA_CONST_LONG;
		entry[1] = (unsigned char)(value >> 56);
		entry[2] = (unsigned char)(value >> 48);
		entry[3] = (unsigned char)(value >> 40);
		entry[4] = (unsigned char)(value >> 32);
		entry[5] = (unsigned char)(value >> 24);
		entry[6] = (unsigned char)(value >> 16);
		entry[7] = (unsigned char)(value >> 8);
		entry[8] = (unsigned char)value;
		LoadDConst(info, entry, 9);
	}
	MARK_END();
}

void JavaGenUInt64(ILGenInfo *info, ILUInt64 value)
{
	JavaGenInt64(info, (ILInt64)value);
}

void JavaGenFloat32(ILGenInfo *info, ILFloat value)
{
	if(value == (ILFloat)0.0)
	{
		OUT_BYTE(JAVA_OP_FCONST_0);
	}
	else if(value == (ILFloat)1.0)
	{
		OUT_BYTE(JAVA_OP_FCONST_1);
	}
	else
	{
		unsigned char entry[5];
		union
		{
			unsigned char bytes[4];
			ILFloat		  value;
		} convert;
		entry[0] = (unsigned char)JAVA_CONST_FLOAT;
		convert.value = (ILFloat)1.0;
		if(convert.bytes[3] == (unsigned char)0x3F)
		{
			/* Little-endian host CPU */
			convert.value = value;
			entry[1] = convert.bytes[3];
			entry[2] = convert.bytes[2];
			entry[3] = convert.bytes[1];
			entry[4] = convert.bytes[0];
		}
		else
		{
			/* Big-endian host CPU */
			convert.value = value;
			entry[1] = convert.bytes[0];
			entry[2] = convert.bytes[1];
			entry[3] = convert.bytes[2];
			entry[4] = convert.bytes[3];
		}
		LoadConst(info, entry, 5);
	}
	MARK_END();
}

void JavaGenFloat64(ILGenInfo *info, ILDouble value)
{
	if(value == (ILDouble)0.0)
	{
		OUT_BYTE(JAVA_OP_DCONST_0);
	}
	else if(value == (ILDouble)1.0)
	{
		OUT_BYTE(JAVA_OP_DCONST_1);
	}
	else
	{
		unsigned char entry[9];
		union
		{
			unsigned char bytes[8];
			struct {
				unsigned long first, second;
			} words;
			ILDouble	  value;
		} convert;
		entry[0] = (unsigned char)JAVA_CONST_DOUBLE;
		convert.value = (ILDouble)1.0;
		if(convert.words.first == 0 && convert.words.second == 0x3FF00000)
		{
			/* Little-endian host CPU */
			convert.value = value;
			entry[1] = convert.bytes[7];
			entry[2] = convert.bytes[6];
			entry[3] = convert.bytes[5];
			entry[4] = convert.bytes[4];
			entry[5] = convert.bytes[3];
			entry[6] = convert.bytes[2];
			entry[7] = convert.bytes[1];
			entry[8] = convert.bytes[0];
		}
		else
		{
			/* Big-endian host CPU */
			convert.value = value;
			entry[1] = convert.bytes[0];
			entry[2] = convert.bytes[1];
			entry[3] = convert.bytes[2];
			entry[4] = convert.bytes[3];
			entry[5] = convert.bytes[4];
			entry[6] = convert.bytes[5];
			entry[7] = convert.bytes[6];
			entry[8] = convert.bytes[7];
		}
		LoadDConst(info, entry, 9);
	}
	MARK_END();
}

/*
 * Get the index related to a UTF-8 entry in the constant pool.
 */
static int GetString(ILGenInfo *info, const char *str, int len)
{
	ILJavaHashEntry *entry;
	unsigned hash = 0;
	int posn;
	for(posn = 0; posn < len; ++posn)
	{
		hash = hash + (hash << 5) + (((unsigned)(str[posn])) & 0xFF);
	}
	hash &= (IL_JAVA_CONST_HASH_SIZE - 1);
	entry = info->javaInfo->javaConstHash[hash];
	while(entry != 0)
	{
		if(entry->valueLen == (len + 3) &&
		   entry->value[0] == (unsigned char)JAVA_CONST_UTF8 &&
		   !ILMemCmp(entry->value + 3, str, len))
		{
			return entry->index;
		}
		entry = entry->next;
	}
	if((entry = (ILJavaHashEntry *)ILMalloc
				(sizeof(ILJavaHashEntry) + len + 2)) == 0)
	{
		ILGenOutOfMemory(info);
	}
	entry->next = info->javaInfo->javaConstHash[hash];
	entry->nextInOrder = 0;
	info->javaInfo->javaConstHash[hash] = entry;
	entry->index = info->javaInfo->javaConstIndex;
	info->javaInfo->javaConstIndex += 1;
	entry->valueLen = len + 3;
	entry->value[0] = (unsigned char)JAVA_CONST_UTF8;
	entry->value[1] = (unsigned char)(len >> 8);
	entry->value[2] = (unsigned char)len;
	if(len > 0)
	{
		ILMemCpy(entry->value + 3, str, len);
	}
	if(info->javaInfo->javaConstLast)
	{
		info->javaInfo->javaConstLast->nextInOrder = entry;
	}
	else
	{
		info->javaInfo->javaConstFirst = entry;
	}
	info->javaInfo->javaConstLast = entry;
	return entry->index;
}

void JavaGenStringConst(ILGenInfo *info, const char *str, int len)
{
	int index = GetString(info, str, len);
	unsigned char entry[3];
	entry[0] = (unsigned char)JAVA_CONST_STRING;
	entry[1] = (unsigned char)(index >> 8);
	entry[2] = (unsigned char)index;
	LoadConst(info, entry, 3);
	MARK_END();
}

static int GetClassName(ILGenInfo *info, const char *str, int len)
{
	int index = GetString(info, str, len);
	unsigned char entry[3];
	entry[0] = (unsigned char)JAVA_CONST_CLASS;
	entry[1] = (unsigned char)(index >> 8);
	entry[2] = (unsigned char)index;
	return (LookupConst(info, entry, 3, 1))->index;
}

/*
 * Helper macros for label management.
 */
#define	MAKE_FIXUP_LABEL(addr)	((ILLabel)(((addr) << 2) | 1))
#define	MAKE_REF_LABEL(addr)	((ILLabel)(((addr) << 2) | 2))
#define	IS_FIXUP_LABEL(label)	(((label) & 3) == 1)
#define	IS_REF_LABEL(label)		(((label) & 3) == 2)
#define	LABEL_TO_ADDR(label)	((label) >> 2)

void JavaGenJump(ILGenInfo *info, int opcode, ILLabel *label)
{
	ILLabel addr;
	long offset;

	/* Flush the peephole optimizer just prior to the jump */
	JavaGenFlush(info);

	/* Output the opcode for the jump */
	OUT_BYTE(opcode);

	/* Fixup or reference label? */
	if(IS_FIXUP_LABEL(*label) || *label == ILLabel_Undefined)
	{
		/* Add the current address to the list of fixups */
		addr = LABEL_TO_ADDR(*label);
		*label = MAKE_FIXUP_LABEL(info->javaInfo->javaPC);
		OUT_BYTE(addr >> 8);
		OUT_BYTE(addr);
	}
	else if(IS_REF_LABEL(*label))
	{
		/* We already know where the label is defined,
		   so output the offset for the jump now */
		addr = LABEL_TO_ADDR(*label);
		offset = (((long)addr) - (long)(info->javaInfo->javaPC - 1));
		OUT_BYTE(offset >> 8);
		OUT_BYTE(offset);
	}

	/* Mark the end of the instruction */
	MARK_END();
}

void JavaGenLabel(ILGenInfo *info, ILLabel *label)
{
	ILLabel addr;
	long offset;
	unsigned nextAddr;

	/* Flush the peephole optimizer just prior to the label */
	JavaGenFlush(info);

	/* Perform fixups on previous references to this label */
	while(IS_FIXUP_LABEL(*label))
	{
		addr = LABEL_TO_ADDR(*label);
		offset = (((long)(info->javaInfo->javaPC)) - (long)(addr - 1));
		nextAddr = (((unsigned)(info->javaInfo->javaCode[addr])) << 8) |
		            ((unsigned)(info->javaInfo->javaCode[addr + 1]));
		info->javaInfo->javaCode[addr]     = (unsigned char)(offset >> 8);
		info->javaInfo->javaCode[addr + 1] = (unsigned char)offset;
		if(nextAddr)
		{
			*label = MAKE_FIXUP_LABEL((ILLabel)nextAddr);
		}
		else
		{
			*label = ILLabel_Undefined;
		}
	}

	/* Record the current address reference in the label */
	*label = MAKE_REF_LABEL(info->javaInfo->javaPC);

	/* Mark the end of the (empty) instruction */
	MARK_END();
}

void JavaGenStoreLocal(ILGenInfo *info, unsigned varNum,
					   ILMachineType type)
{
	switch(type)
	{
		case ILMachineType_Void:	break;

		case ILMachineType_Boolean:
		case ILMachineType_Int8:
		case ILMachineType_UInt8:
		case ILMachineType_Int16:
		case ILMachineType_UInt16:
		case ILMachineType_Char:
		case ILMachineType_Int32:
		case ILMachineType_UInt32:
		case ILMachineType_NativeInt:
		case ILMachineType_NativeUInt:
		{
			if(varNum < 4)
			{
				OUT_BYTE(JAVA_OP_ISTORE_0 + varNum);
			}
			else if(varNum < 256)
			{
				OUT_BYTE(JAVA_OP_ISTORE);
				OUT_BYTE(varNum);
			}
			else
			{
				OUT_BYTE(JAVA_OP_WIDE);
				OUT_BYTE(JAVA_OP_ISTORE);
				OUT_BYTE(varNum >> 8);
				OUT_BYTE(varNum);
			}
		}
		break;

		case ILMachineType_Int64:
		case ILMachineType_UInt64:
		{
			if(varNum < 4)
			{
				OUT_BYTE(JAVA_OP_LSTORE_0 + varNum);
			}
			else if(varNum < 256)
			{
				OUT_BYTE(JAVA_OP_LSTORE);
				OUT_BYTE(varNum);
			}
			else
			{
				OUT_BYTE(JAVA_OP_WIDE);
				OUT_BYTE(JAVA_OP_LSTORE);
				OUT_BYTE(varNum >> 8);
				OUT_BYTE(varNum);
			}
		}
		break;

		case ILMachineType_Float32:
		{
			if(varNum < 4)
			{
				OUT_BYTE(JAVA_OP_FSTORE_0 + varNum);
			}
			else if(varNum < 256)
			{
				OUT_BYTE(JAVA_OP_FSTORE);
				OUT_BYTE(varNum);
			}
			else
			{
				OUT_BYTE(JAVA_OP_WIDE);
				OUT_BYTE(JAVA_OP_FSTORE);
				OUT_BYTE(varNum >> 8);
				OUT_BYTE(varNum);
			}
		}
		break;

		case ILMachineType_Float64:
		case ILMachineType_NativeFloat:
		{
			if(varNum < 4)
			{
				OUT_BYTE(JAVA_OP_DSTORE_0 + varNum);
			}
			else if(varNum < 256)
			{
				OUT_BYTE(JAVA_OP_DSTORE);
				OUT_BYTE(varNum);
			}
			else
			{
				OUT_BYTE(JAVA_OP_WIDE);
				OUT_BYTE(JAVA_OP_DSTORE);
				OUT_BYTE(varNum >> 8);
				OUT_BYTE(varNum);
			}
		}
		break;

		case ILMachineType_Decimal:
		case ILMachineType_String:
		case ILMachineType_ObjectRef:
		case ILMachineType_UnmanagedPtr:
		case ILMachineType_ManagedPtr:
		case ILMachineType_ManagedValue:
		case ILMachineType_TransientPtr:
		{
			if(varNum < 4)
			{
				OUT_BYTE(JAVA_OP_ASTORE_0 + varNum);
			}
			else if(varNum < 256)
			{
				OUT_BYTE(JAVA_OP_ASTORE);
				OUT_BYTE(varNum);
			}
			else
			{
				OUT_BYTE(JAVA_OP_WIDE);
				OUT_BYTE(JAVA_OP_ASTORE);
				OUT_BYTE(varNum >> 8);
				OUT_BYTE(varNum);
			}
		}
		break;
	}
	MARK_END();
}

void JavaGenLoadLocal(ILGenInfo *info, unsigned varNum,
					  ILMachineType type)
{
	switch(type)
	{
		case ILMachineType_Void:	break;

		case ILMachineType_Boolean:
		case ILMachineType_Int8:
		case ILMachineType_UInt8:
		case ILMachineType_Int16:
		case ILMachineType_UInt16:
		case ILMachineType_Char:
		case ILMachineType_Int32:
		case ILMachineType_UInt32:
		case ILMachineType_NativeInt:
		case ILMachineType_NativeUInt:
		{
			if(varNum < 4)
			{
				OUT_BYTE(JAVA_OP_ILOAD_0 + varNum);
			}
			else if(varNum < 256)
			{
				OUT_BYTE(JAVA_OP_ILOAD);
				OUT_BYTE(varNum);
			}
			else
			{
				OUT_BYTE(JAVA_OP_WIDE);
				OUT_BYTE(JAVA_OP_ILOAD);
				OUT_BYTE(varNum >> 8);
				OUT_BYTE(varNum);
			}
		}
		break;

		case ILMachineType_Int64:
		case ILMachineType_UInt64:
		{
			if(varNum < 4)
			{
				OUT_BYTE(JAVA_OP_LLOAD_0 + varNum);
			}
			else if(varNum < 256)
			{
				OUT_BYTE(JAVA_OP_LLOAD);
				OUT_BYTE(varNum);
			}
			else
			{
				OUT_BYTE(JAVA_OP_WIDE);
				OUT_BYTE(JAVA_OP_LLOAD);
				OUT_BYTE(varNum >> 8);
				OUT_BYTE(varNum);
			}
		}
		break;

		case ILMachineType_Float32:
		{
			if(varNum < 4)
			{
				OUT_BYTE(JAVA_OP_FLOAD_0 + varNum);
			}
			else if(varNum < 256)
			{
				OUT_BYTE(JAVA_OP_FLOAD);
				OUT_BYTE(varNum);
			}
			else
			{
				OUT_BYTE(JAVA_OP_WIDE);
				OUT_BYTE(JAVA_OP_FLOAD);
				OUT_BYTE(varNum >> 8);
				OUT_BYTE(varNum);
			}
		}
		break;

		case ILMachineType_Float64:
		case ILMachineType_NativeFloat:
		{
			if(varNum < 4)
			{
				OUT_BYTE(JAVA_OP_DLOAD_0 + varNum);
			}
			else if(varNum < 256)
			{
				OUT_BYTE(JAVA_OP_DLOAD);
				OUT_BYTE(varNum);
			}
			else
			{
				OUT_BYTE(JAVA_OP_WIDE);
				OUT_BYTE(JAVA_OP_DLOAD);
				OUT_BYTE(varNum >> 8);
				OUT_BYTE(varNum);
			}
		}
		break;

		case ILMachineType_Decimal:
		case ILMachineType_String:
		case ILMachineType_ObjectRef:
		case ILMachineType_UnmanagedPtr:
		case ILMachineType_ManagedPtr:
		case ILMachineType_ManagedValue:
		case ILMachineType_TransientPtr:
		{
			if(varNum < 4)
			{
				OUT_BYTE(JAVA_OP_ALOAD_0 + varNum);
			}
			else if(varNum < 256)
			{
				OUT_BYTE(JAVA_OP_ALOAD);
				OUT_BYTE(varNum);
			}
			else
			{
				OUT_BYTE(JAVA_OP_WIDE);
				OUT_BYTE(JAVA_OP_ALOAD);
				OUT_BYTE(varNum >> 8);
				OUT_BYTE(varNum);
			}
		}
		break;
	}
	MARK_END();
}

int JavaGenTypeSize(ILMachineType type)
{
	if(type == ILMachineType_Void)
	{
		return 0;
	}
	else if(type == ILMachineType_Int64 ||
			type == ILMachineType_UInt64 ||
			type == ILMachineType_Float64 ||
			type == ILMachineType_NativeFloat)
	{
		return 2;
	}
	else
	{
		return 1;
	}
}

static void GenCall(ILGenInfo *info, int opcode, const char *className,
				    const char *methodName, const char *signature)
{
	int cindex, mindex, sindex;
	unsigned char value[5];
	ILJavaHashEntry *entry;

	/* Flush the peephole optimisation queue */
	JavaGenFlush(info);

	/* Enter the strings into the constant pool */
	cindex = GetString(info, className, strlen(className));
	mindex = GetString(info, methodName, strlen(methodName));
	sindex = GetString(info, signature, strlen(signature));

	/* Create the ClassInfo constant pool entry */
	value[0] = (unsigned char)JAVA_CONST_CLASS;
	value[1] = (unsigned char)(cindex >> 8);
	value[2] = (unsigned char)cindex;
	entry = LookupConst(info, value, 3, 1);
	cindex = entry->index;

	/* Create the NameAndType constant pool entry */
	value[0] = (unsigned char)JAVA_CONST_NAMEANDTYPE;
	value[1] = (unsigned char)(mindex >> 8);
	value[2] = (unsigned char)mindex;
	value[3] = (unsigned char)(sindex >> 8);
	value[4] = (unsigned char)sindex;
	entry = LookupConst(info, value, 5, 1);
	mindex = entry->index;

	/* Create the MethodRef constant pool entry */
	value[0] = (unsigned char)JAVA_CONST_METHODREF;
	value[1] = (unsigned char)(cindex >> 8);
	value[2] = (unsigned char)cindex;
	value[3] = (unsigned char)(mindex >> 8);
	value[4] = (unsigned char)mindex;
	entry = LookupConst(info, value, 5, 1);
	mindex = entry->index;

	/* Output the "invokestatic" instruction */
	OUT_BYTE(opcode);
	OUT_BYTE(mindex >> 8);
	OUT_BYTE(mindex);
	MARK_END();
}

void JavaGenCallByName(ILGenInfo *info, const char *className,
					   const char *methodName, const char *signature)
{
	GenCall(info, JAVA_OP_INVOKESTATIC, className, methodName, signature);
}

void JavaGenCallIntrinsic(ILGenInfo *info, const char *methodName,
						  const char *signature)
{
	GenCall(info, JAVA_OP_INVOKESTATIC, "System/Intrinsics/Operations",
			methodName, signature);
}

void JavaGenCallVirtual(ILGenInfo *info, const char *className,
						const char *methodName, const char *signature)
{
	GenCall(info, JAVA_OP_INVOKEVIRTUAL, className, methodName, signature);
}

void JavaGenCallVirtIntrinsic(ILGenInfo *info, const char *className,
						      const char *methodName, const char *signature)
{
	char cname[64];
	strcpy(cname, "System/Intrinsics/");
	strcat(cname, className);
	GenCall(info, JAVA_OP_INVOKEVIRTUAL, cname, methodName, signature);
}

void JavaGenCallByMethod(ILGenInfo *info, ILMethod *method)
{
}

void JavaGenNewObj(ILGenInfo *info, const char *className,
				   const char *initName, const char *signature)
{
	int cindex;
	unsigned char value[3];

	/* Flush the peephole optimisation queue */
	JavaGenFlush(info);

	/* Create the ClassInfo constant pool entry */
	cindex = GetString(info, className, strlen(className));
	value[0] = (unsigned char)JAVA_CONST_CLASS;
	value[1] = (unsigned char)(cindex >> 8);
	value[2] = (unsigned char)cindex;
	cindex = (LookupConst(info, value, 3, 1))->index;

	/* Output the "new" instruction to create the object */
	OUT_BYTE(JAVA_OP_NEW);
	OUT_BYTE(cindex >> 8);
	OUT_BYTE(cindex);

	/* Call the constructor */
	GenCall(info, JAVA_OP_INVOKESTATIC, className, initName, signature);
}

void JavaGenNewIntrinsic(ILGenInfo *info, const char *className,
				   		 const char *initName, const char *signature)
{
	char cname[64];
	strcpy(cname, "System/Intrinsics/");
	strcat(cname, className);
	JavaGenNewObj(info, cname, initName, signature);
}

void JavaGenClassRef(ILGenInfo *info, int opcode, ILClass *classInfo)
{
}

void JavaGenCreateClass(ILGenInfo *info, ILClass *classInfo)
{
	unsigned flags;
	char *name;
	ILClass *parentInfo;
	int posn;
	ILImplements *impl;

	/* Convert the IL access flags into JVM access flags */
	flags = JAVA_ACC_SUPER;
	if(ILClass_NestedParent(classInfo) != 0)
	{
		/* Nested class */
		if(!ILClass_IsNestedPrivate(classInfo))
		{
			flags |= JAVA_ACC_PUBLIC;
		}
	}
	else
	{
		/* Non-nested class */
		if(ILClass_IsPublic(classInfo))
		{
			flags |= JAVA_ACC_PUBLIC;
		}
	}
	if(ILClass_IsInterface(classInfo))
	{
		flags |= JAVA_ACC_INTERFACE;
	}
	if(ILClass_IsAbstract(classInfo))
	{
		flags |= JAVA_ACC_ABSTRACT;
	}
	if(ILClass_IsSealed(classInfo))
	{
		flags |= JAVA_ACC_FINAL;
	}

	/* Store the class information for later */
	info->javaInfo->accessFlags = flags;
	name = JavaGetClassName(info, classInfo);
	info->javaInfo->className = GetClassName(info, name, strlen(name));
	ILFree(name);
	parentInfo = ILClass_Parent(classInfo);
	if(parentInfo)
	{
		name = JavaGetClassName(info, parentInfo);
		info->javaInfo->className = GetClassName(info, name, strlen(name));
		ILFree(name);
	}
	else
	{
		/* All classes in Java must eventually inherit from java.lang.Object */
		info->javaInfo->parentName = GetString(info, "java/lang/Object", 16);
	}

	/* Get the indexes for the interface names */
	info->javaInfo->numInterfaces = 0;
	impl = 0;
	while((impl = ILClassNextImplements(classInfo, impl)) != 0)
	{
		++(info->javaInfo->numInterfaces);
	}
	if(info->javaInfo->numInterfaces)
	{
		info->javaInfo->interfaces =
			(int *)ILCalloc(info->javaInfo->numInterfaces, sizeof(int));
		if(!(info->javaInfo->interfaces))
		{
			ILGenOutOfMemory(info);
		}
		impl = 0;
		posn = 0;
		while((impl = ILClassNextImplements(classInfo, impl)) != 0)
		{
			name = JavaGetClassName(info, ILImplementsGetInterface(impl));
			info->javaInfo->interfaces[posn++]
					= GetClassName(info, name, strlen(name));
			ILFree(name);
		}
	}
}

void JavaGenCreateField(ILGenInfo *info, ILField *fieldInfo)
{
}

void JavaGenCreateMethod(ILGenInfo *info, ILMethod *methodInfo)
{
}

void JavaGenCreateGetMethod(ILGenInfo *info, ILProperty *propInfo)
{
}

void JavaGenCreateSetMethod(ILGenInfo *info, ILProperty *propInfo)
{
}

void JavaGenFlush(ILGenInfo *info)
{
}

char *JavaGetClassName(ILGenInfo *info, ILClass *classInfo)
{
	const char *name = ILClass_Name(classInfo);
	const char *namespace = ILClass_Namespace(classInfo);
	char *result;
	int len;
	char ch;
	if(!namespace)
	{
		return JavaStrAppend(info, 0, name);
	}
	len = strlen(namespace) + strlen(name) + 2;
	result = (char *)ILMalloc(len);
	if(!result)
	{
		ILGenOutOfMemory(info);
	}
	len = 0;
	while((ch = namespace[len]) != '\0')
	{
		if(ch == '.')
		{
			result[len++] = '/';
		}
		else
		{
			result[len++] = ch;
		}
	}
	result[len++] = '/';
	strcpy(result + len, name);
	return result;
}

char *JavaStrAppend(ILGenInfo *info, char *str1, const char *str2)
{
	int len;
	char *result;
	if(!str1)
	{
		len = strlen(str2) + 1;
		result = (char *)ILMalloc(len);
		if(!result)
		{
			ILGenOutOfMemory(info);
		}
		strcpy(result, str2);
		return result;
	}
	else
	{
		len = strlen(str1);
		result = (char *)ILRealloc(str1, len + strlen(str2) + 1);
		if(!result)
		{
			ILGenOutOfMemory(info);
		}
		strcpy(result + len, str2);
		return result;
	}
}

#ifdef	__cplusplus
};
#endif
