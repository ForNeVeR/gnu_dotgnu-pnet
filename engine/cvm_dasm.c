/*
 * cvm_dasm.c - Mini-disassembler for debugging the CVM bytecode converter.
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

#include "il_dumpasm.h"
#include "engine.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Operand types for CVM instructions.
 */
#define	CVM_OPER_NONE				0
#define	CVM_OPER_UINT8				1
#define	CVM_OPER_TWO_UINT8			2
#define	CVM_OPER_WIDE_UINT			3
#define	CVM_OPER_WIDE_TWO_UINT		4
#define	CVM_OPER_UINT32				5
#define	CVM_OPER_INT8				6
#define	CVM_OPER_INT32				7
#define	CVM_OPER_UINT64				8
#define	CVM_OPER_FLOAT32			9
#define	CVM_OPER_FLOAT64			10
#define	CVM_OPER_BRANCH				11
#define	CVM_OPER_BRANCH_LONG		12
#define	CVM_OPER_SWITCH				13
#define	CVM_OPER_CALL				14
#define	CVM_OPER_CALL_EXTERN		15
#define	CVM_OPER_CALL_NATIVE		16
#define	CVM_OPER_CALL_INTERFACE		17
#define	CVM_OPER_CLASS				18
#define	CVM_OPER_UINT_AND_CLASS		19
#define	CVM_OPER_ITEM				20
#define	CVM_OPER_STRING				21
#define	CVM_OPER_WIDE				22
#define	CVM_OPER_PREFIX				23
#define	CVM_OPER_ENTER_TRY			24
#define	CVM_OPER_METHOD				25
#define	CVM_OPER_LD_INTERFACE		26

/*
 * Table of CVM opcodes.  This must be kept in sync with "cvm.h".
 */
typedef struct
{
	const char *name;
	int			operands;

} CVMOpcode;
static CVMOpcode const opcodes[256] = {

	/*
	 * Simple opcodes.
	 */
	{"nop",				CVM_OPER_NONE},

	/*
	 * Local variable opcodes.
	 */
	{"iload_0",			CVM_OPER_NONE},
	{"iload_1",			CVM_OPER_NONE},
	{"iload_2",			CVM_OPER_NONE},
	{"iload_3",			CVM_OPER_NONE},
	{"iload",			CVM_OPER_WIDE_UINT},
	{"pload_0",			CVM_OPER_NONE},
	{"pload_1",			CVM_OPER_NONE},
	{"pload_2",			CVM_OPER_NONE},
	{"pload_3",			CVM_OPER_NONE},
	{"pload",			CVM_OPER_WIDE_UINT},
	{"istore_0",		CVM_OPER_NONE},
	{"istore_1",		CVM_OPER_NONE},
	{"istore_2",		CVM_OPER_NONE},
	{"istore_3",		CVM_OPER_NONE},
	{"istore",			CVM_OPER_WIDE_UINT},
	{"pstore_0",		CVM_OPER_NONE},
	{"pstore_1",		CVM_OPER_NONE},
	{"pstore_2",		CVM_OPER_NONE},
	{"pstore_3",		CVM_OPER_NONE},
	{"pstore",			CVM_OPER_WIDE_UINT},
	{"mload",			CVM_OPER_WIDE_TWO_UINT},
	{"mstore",			CVM_OPER_WIDE_TWO_UINT},
	{"waddr",			CVM_OPER_WIDE_UINT},
	{"maddr",			CVM_OPER_WIDE_UINT},

	/*
	 * Argument fixups.
	 */
	{"bfixup",			CVM_OPER_WIDE_UINT},
	{"sfixup",			CVM_OPER_WIDE_UINT},
	{"ffixup",			CVM_OPER_WIDE_UINT},
	{"dfixup",			CVM_OPER_WIDE_UINT},

	/*
	 * Local variable allocation.
	 */
	{"mk_local_1",		CVM_OPER_NONE},
	{"mk_local_2",		CVM_OPER_NONE},
	{"mk_local_3",		CVM_OPER_NONE},
	{"mk_local_n",		CVM_OPER_WIDE_UINT},

	/*
	 * Pointer reads and writes.
	 */
	{"bread",			CVM_OPER_NONE},
	{"ubread",			CVM_OPER_NONE},
	{"sread",			CVM_OPER_NONE},
	{"usread",			CVM_OPER_NONE},
	{"iread",			CVM_OPER_NONE},
	{"fread",			CVM_OPER_NONE},
	{"dread",			CVM_OPER_NONE},
	{"pread",			CVM_OPER_NONE},
	{"mread",			CVM_OPER_WIDE_UINT},
	{"bwrite",			CVM_OPER_NONE},
	{"swrite",			CVM_OPER_NONE},
	{"iwrite",			CVM_OPER_NONE},
	{"fwrite",			CVM_OPER_NONE},
	{"dwrite",			CVM_OPER_NONE},
	{"pwrite",			CVM_OPER_NONE},
	{"mwrite",			CVM_OPER_WIDE_UINT},
	{"bwrite_r",		CVM_OPER_NONE},
	{"swrite_r",		CVM_OPER_NONE},
	{"iwrite_r",		CVM_OPER_NONE},
	{"fwrite_r",		CVM_OPER_NONE},
	{"dwrite_r",		CVM_OPER_NONE},
	{"pwrite_r",		CVM_OPER_NONE},
	{"mwrite_r",		CVM_OPER_WIDE_UINT},

	/*
	 * Stack handling.
	 */
	{"dup",				CVM_OPER_NONE},
	{"dup2",			CVM_OPER_NONE},
	{"dup_n",			CVM_OPER_WIDE_UINT},
	{"dup_word_n",		CVM_OPER_WIDE_UINT},
	{"pop",				CVM_OPER_NONE},
	{"pop2",			CVM_OPER_NONE},
	{"pop_n",			CVM_OPER_WIDE_UINT},
	{"squash",			CVM_OPER_WIDE_TWO_UINT},
	{"ckheight",		CVM_OPER_NONE},
	{"ckheight_n",		CVM_OPER_UINT32},
	{"set_num_args",	CVM_OPER_WIDE_UINT},

	/*
	 * Arithmetic operators.
	 */
	{"iadd",			CVM_OPER_NONE},
	{"iadd_ovf",		CVM_OPER_NONE},
	{"iadd_ovf_un",		CVM_OPER_NONE},
	{"isub",			CVM_OPER_NONE},
	{"isub_ovf",		CVM_OPER_NONE},
	{"isub_ovf_un",		CVM_OPER_NONE},
	{"imul",			CVM_OPER_NONE},
	{"imul_ovf",		CVM_OPER_NONE},
	{"imul_ovf_un",		CVM_OPER_NONE},
	{"idiv",			CVM_OPER_NONE},
	{"idiv_un",			CVM_OPER_NONE},
	{"irem",			CVM_OPER_NONE},
	{"irem_un",			CVM_OPER_NONE},
	{"ineg",			CVM_OPER_NONE},
	{"ladd",			CVM_OPER_NONE},
	{"ladd_ovf",		CVM_OPER_NONE},
	{"ladd_ovf_un",		CVM_OPER_NONE},
	{"lsub",			CVM_OPER_NONE},
	{"lsub_ovf",		CVM_OPER_NONE},
	{"lsub_ovf_un",		CVM_OPER_NONE},
	{"lmul",			CVM_OPER_NONE},
	{"lmul_ovf",		CVM_OPER_NONE},
	{"lmul_ovf_un",		CVM_OPER_NONE},
	{"ldiv",			CVM_OPER_NONE},
	{"ldiv_un",			CVM_OPER_NONE},
	{"lrem",			CVM_OPER_NONE},
	{"lrem_un",			CVM_OPER_NONE},
	{"lneg",			CVM_OPER_NONE},
	{"fadd",			CVM_OPER_NONE},
	{"fsub",			CVM_OPER_NONE},
	{"fmul",			CVM_OPER_NONE},
	{"fdiv",			CVM_OPER_NONE},
	{"frem",			CVM_OPER_NONE},
	{"fneg",			CVM_OPER_NONE},

	/*
	 * Bitwise operators.
	 */
	{"iand",			CVM_OPER_NONE},
	{"ior",				CVM_OPER_NONE},
	{"ixor",			CVM_OPER_NONE},
	{"inot",			CVM_OPER_NONE},
	{"ishl",			CVM_OPER_NONE},
	{"ishr",			CVM_OPER_NONE},
	{"ishr_un",			CVM_OPER_NONE},
	{"land",			CVM_OPER_NONE},
	{"lor",				CVM_OPER_NONE},
	{"lxor",			CVM_OPER_NONE},
	{"lnot",			CVM_OPER_NONE},
	{"lshl",			CVM_OPER_NONE},
	{"lshr",			CVM_OPER_NONE},
	{"lshr_un",			CVM_OPER_NONE},

	/*
	 * Conversion operators.
	 */
	{"i2b",				CVM_OPER_NONE},
	{"i2ub",			CVM_OPER_NONE},
	{"i2s",				CVM_OPER_NONE},
	{"i2us",			CVM_OPER_NONE},
	{"i2l",				CVM_OPER_NONE},
	{"iu2l",			CVM_OPER_NONE},
	{"i2f",				CVM_OPER_NONE},
	{"iu2f",			CVM_OPER_NONE},
	{"l2i",				CVM_OPER_NONE},
	{"l2f",				CVM_OPER_NONE},
	{"lu2f",			CVM_OPER_NONE},
	{"f2i",				CVM_OPER_NONE},
	{"f2iu",			CVM_OPER_NONE},
	{"f2l",				CVM_OPER_NONE},
	{"f2lu",			CVM_OPER_NONE},
	{"f2f",				CVM_OPER_NONE},
	{"f2d",				CVM_OPER_NONE},
	{"i2p_lower",		CVM_OPER_WIDE_UINT},

	/*
	 * Binary value fixups.
	 */
	{"fix_i4_i",		CVM_OPER_NONE},
	{"fix_i4_u",		CVM_OPER_NONE},

	/*
	 * Pointer arithmetic and manipulation.
	 */
	{"padd_offset",		CVM_OPER_UINT8},
	{"padd_offset_n",	CVM_OPER_WIDE_TWO_UINT},
	{"padd_i4",			CVM_OPER_NONE},
	{"padd_i4_r",		CVM_OPER_NONE},
	{"padd_i8",			CVM_OPER_NONE},
	{"padd_i8_r",		CVM_OPER_NONE},
	{"psub",			CVM_OPER_NONE},
	{"psub_i4",			CVM_OPER_NONE},
	{"psub_i8",			CVM_OPER_NONE},
	{"cknull",			CVM_OPER_NONE},
	{"cknull_n",		CVM_OPER_WIDE_UINT},
	{"ldrva",			CVM_OPER_UINT32},

	/*
	 * Constant opcodes.
	 */
	{"ldnull",			CVM_OPER_NONE},
	{"ldc_i4_m1",		CVM_OPER_NONE},
	{"ldc_i4_0",		CVM_OPER_NONE},
	{"ldc_i4_1",		CVM_OPER_NONE},
	{"ldc_i4_2",		CVM_OPER_NONE},
	{"ldc_i4_3",		CVM_OPER_NONE},
	{"ldc_i4_4",		CVM_OPER_NONE},
	{"ldc_i4_5",		CVM_OPER_NONE},
	{"ldc_i4_6",		CVM_OPER_NONE},
	{"ldc_i4_7",		CVM_OPER_NONE},
	{"ldc_i4_8",		CVM_OPER_NONE},
	{"ldc_i4_s",		CVM_OPER_INT8},
	{"ldc_i4",			CVM_OPER_INT32},
	{"ldc_i8",			CVM_OPER_UINT64},
	{"ldc_r4",			CVM_OPER_FLOAT32},
	{"ldc_r8",			CVM_OPER_FLOAT64},

	/*
	 * Branch opcodes.
	 */
	{"br",				CVM_OPER_BRANCH},
	{"beq",				CVM_OPER_BRANCH},
	{"bne",				CVM_OPER_BRANCH},
	{"blt",				CVM_OPER_BRANCH},
	{"blt_un",			CVM_OPER_BRANCH},
	{"ble",				CVM_OPER_BRANCH},
	{"ble_un",			CVM_OPER_BRANCH},
	{"bgt",				CVM_OPER_BRANCH},
	{"bgt_un",			CVM_OPER_BRANCH},
	{"bge",				CVM_OPER_BRANCH},
	{"bge_un",			CVM_OPER_BRANCH},
	{"brtrue",			CVM_OPER_BRANCH},
	{"brfalse",			CVM_OPER_BRANCH},
	{"brnull",			CVM_OPER_BRANCH},
	{"brnonnull",		CVM_OPER_BRANCH},
	{"br_peq",			CVM_OPER_BRANCH},
	{"br_pne",			CVM_OPER_BRANCH},
	{"br_long",			CVM_OPER_BRANCH_LONG},
	{"switch",			CVM_OPER_SWITCH},

	/*
	 * Array opcodes.
	 */
	{"bread_elem",		CVM_OPER_NONE},
	{"ubread_elem",		CVM_OPER_NONE},
	{"sread_elem",		CVM_OPER_NONE},
	{"usread_elem",		CVM_OPER_NONE},
	{"iread_elem",		CVM_OPER_NONE},
	{"pread_elem",		CVM_OPER_NONE},
	{"bwrite_elem",		CVM_OPER_NONE},
	{"swrite_elem",		CVM_OPER_NONE},
	{"iwrite_elem",		CVM_OPER_NONE},
	{"pwrite_elem",		CVM_OPER_NONE},
	{"ckarray_load_i4",	CVM_OPER_NONE},
	{"ckarray_load_i8",	CVM_OPER_NONE},
	{"ckarray_store_i8", CVM_OPER_TWO_UINT8},
	{"array_len",		CVM_OPER_NONE},

	/*
	 * Field opcodes.
	 */
	{"bread_field",		CVM_OPER_UINT8},
	{"ubread_field",	CVM_OPER_UINT8},
	{"sread_field",		CVM_OPER_UINT8},
	{"usread_field",	CVM_OPER_UINT8},
	{"iread_field",		CVM_OPER_UINT8},
	{"pread_field",		CVM_OPER_UINT8},
	{"bwrite_field",	CVM_OPER_UINT8},
	{"swrite_field",	CVM_OPER_UINT8},
	{"iwrite_field",	CVM_OPER_UINT8},
	{"pwrite_field",	CVM_OPER_UINT8},

	/*
	 * Call management opcodes.
	 */
	{"call",			CVM_OPER_CALL},
	{"call_extern",		CVM_OPER_CALL_EXTERN},
	{"call_ctor",		CVM_OPER_CALL_EXTERN},
	{"call_native",		CVM_OPER_CALL_NATIVE},
	{"call_native_void", CVM_OPER_CALL_NATIVE},
	{"call_virtual",	CVM_OPER_WIDE_TWO_UINT},
	{"call_interface",	CVM_OPER_CALL_INTERFACE},
	{"return",			CVM_OPER_NONE},
	{"return_1",		CVM_OPER_NONE},
	{"return_2",		CVM_OPER_NONE},
	{"return_n",		CVM_OPER_UINT32},
	{"jsr",				CVM_OPER_BRANCH},
	{"ret_jsr",			CVM_OPER_NONE},
	{"push_thread",		CVM_OPER_NONE},
	{"pushdown",		CVM_OPER_UINT32},
	{"cctor_once",		CVM_OPER_NONE},
	{"calli",			CVM_OPER_NONE},
	{"jmpi",			CVM_OPER_NONE},

	/*
	 * Class-related opcodes.
	 */
	{"castclass",		CVM_OPER_CLASS},
	{"isinst",			CVM_OPER_CLASS},
	{"castinterface",	CVM_OPER_CLASS},
	{"isinterface",		CVM_OPER_CLASS},
	{"get_static",		CVM_OPER_CLASS},
	{"new",				CVM_OPER_NONE},
	{"new_value",		CVM_OPER_WIDE_TWO_UINT},
	{"ldstr",			CVM_OPER_STRING},
	{"ldtoken",			CVM_OPER_ITEM},
	{"box",				CVM_OPER_UINT_AND_CLASS},
	{"box_ptr",			CVM_OPER_UINT_AND_CLASS},

	/*
	 * Memory-related opcodes.
	 */
	{"memcpy",			CVM_OPER_WIDE_UINT},
	{"memmove",			CVM_OPER_NONE},
	{"memzero",			CVM_OPER_WIDE_UINT},
	{"memset",			CVM_OPER_NONE},

	/*
	 * Reserved opcodes.
	 */
	{"reserved_ef",		CVM_OPER_NONE},
	{"reserved_f0",		CVM_OPER_NONE},
	{"reserved_f1",		CVM_OPER_NONE},
	{"reserved_f2",		CVM_OPER_NONE},
	{"reserved_f3",		CVM_OPER_NONE},
	{"reserved_f4",		CVM_OPER_NONE},
	{"reserved_f5",		CVM_OPER_NONE},
	{"reserved_f6",		CVM_OPER_NONE},
	{"reserved_f7",		CVM_OPER_NONE},
	{"reserved_f8",		CVM_OPER_NONE},
	{"reserved_f9",		CVM_OPER_NONE},
	{"reserved_fa",		CVM_OPER_NONE},
	{"reserved_fb",		CVM_OPER_NONE},
	{"reserved_fc",		CVM_OPER_NONE},

	/*
	 * Make the next instruction wider.
	 */
	{"wide",			CVM_OPER_WIDE},

	/*
	 * Reserved opcodes.
	 */
	{"reserved_fe",		CVM_OPER_NONE},

	/*
	 * Prefix for two-byte instruction opcodes.
	 */
	{"prefix",			CVM_OPER_PREFIX},
};
static CVMOpcode const prefixOpcodes[64] = {
	/*
	 * Reserved opcodes.
	 */
	{"preserved_00",	CVM_OPER_NONE},

	/*
	 * Prefixed comparison opcodes.
	 */
	{"icmp",			CVM_OPER_NONE},
	{"icmp_un",			CVM_OPER_NONE},
	{"lcmp",			CVM_OPER_NONE},
	{"lcmp_un",			CVM_OPER_NONE},
	{"fcmpl",			CVM_OPER_NONE},
	{"fcmpg",			CVM_OPER_NONE},
	{"pcmp",			CVM_OPER_NONE},
	{"seteq",			CVM_OPER_NONE},
	{"setne",			CVM_OPER_NONE},
	{"setlt",			CVM_OPER_NONE},
	{"setle",			CVM_OPER_NONE},
	{"setgt",			CVM_OPER_NONE},
	{"setge",			CVM_OPER_NONE},

	/*
	 * Reserved opcodes.
	 */
	{"preserved_0e",	CVM_OPER_NONE},
	{"preserved_0f",	CVM_OPER_NONE},

	/*
	 * Prefixed array opcodes.
	 */
	{"lread_elem",		CVM_OPER_NONE},
	{"fread_elem",		CVM_OPER_NONE},
	{"dread_elem",		CVM_OPER_NONE},
	{"lwrite_elem",		CVM_OPER_NONE},
	{"fwrite_elem",		CVM_OPER_NONE},
	{"dwrite_elem",		CVM_OPER_NONE},

	/*
	 * Prefixed call management opcodes.
	 */
	{"tail",			CVM_OPER_NONE},
	{"ldftn",			CVM_OPER_METHOD},
	{"ldvirtftn",		CVM_OPER_UINT32},
	{"ldinterfftn",		CVM_OPER_LD_INTERFACE},

	/*
	 * Reserved opcodes.
	 */
	{"preserved_1a",	CVM_OPER_NONE},
	{"preserved_1b",	CVM_OPER_NONE},

	/*
	 * Prefixed exception handling opcodes.
	 */
	{"enter_try",		CVM_OPER_ENTER_TRY},
	{"exit_try",		CVM_OPER_NONE},
	{"throw",			CVM_OPER_NONE},

	/*
	 * Prefixed typedref handling opcodes.
	 */
	{"mkrefany",		CVM_OPER_CLASS},
	{"refanyval",		CVM_OPER_CLASS},

	/*
	 * Prefixed conversion opcodes.
	 */
	{"i2b_ovf",			CVM_OPER_NONE},
	{"i2ub_ovf",		CVM_OPER_NONE},
	{"iu2b_ovf",		CVM_OPER_NONE},
	{"iu2ub_ovf",		CVM_OPER_NONE},
	{"i2s_ovf",			CVM_OPER_NONE},
	{"i2us_ovf",		CVM_OPER_NONE},
	{"iu2s_ovf",		CVM_OPER_NONE},
	{"iu2us_ovf",		CVM_OPER_NONE},
	{"i2iu_ovf",		CVM_OPER_NONE},
	{"iu2i_ovf",		CVM_OPER_NONE},
	{"i2ul_ovf",		CVM_OPER_NONE},
	{"l2i_ovf",			CVM_OPER_NONE},
	{"l2ui_ovf",		CVM_OPER_NONE},
	{"lu2i_ovf",		CVM_OPER_NONE},
	{"lu2iu_ovf",		CVM_OPER_NONE},
	{"l2ul_ovf",		CVM_OPER_NONE},
	{"lu2l_ovf",		CVM_OPER_NONE},
	{"f2i_ovf",			CVM_OPER_NONE},
	{"f2iu_ovf",		CVM_OPER_NONE},
	{"f2l_ovf",			CVM_OPER_NONE},
	{"f2lu_ovf",		CVM_OPER_NONE},
	{"i2b_aligned",		CVM_OPER_NONE},
	{"i2s_aligned",		CVM_OPER_NONE},
	{"f2f_aligned",		CVM_OPER_NONE},
	{"f2d_aligned",		CVM_OPER_NONE},

	/*
	 * Prefixed arithmetic opcodes.
	 */
	{"ckfinite",		CVM_OPER_NONE},

	/*
	 * Reserved opcodes.
	 */
	{"preserved_3b",	CVM_OPER_NONE},
	{"preserved_3c",	CVM_OPER_NONE},
	{"preserved_3d",	CVM_OPER_NONE},
	{"preserved_3e",	CVM_OPER_NONE},
	{"preserved_3f",	CVM_OPER_NONE},
};

/*
 * Read a pointer from an instruction stream.
 */
static void *CVMReadPointer(unsigned char *pc)
{
#if defined(__i386) || defined(__i386__)
	/* The x86 can read values from non-aligned addresses */
	return *((void **)pc);
#else
	/* We need to be careful about alignment on other platforms */
	if(sizeof(void *) == 4)
	{
		return (void *)(IL_READ_UINT32(pc));
	}
	else
	{
		return (void *)(IL_READ_UINT64(pc));
	}
#endif
}

/* internal.c */
const ILMethodTableEntry *_ILFindInternalByAddr(void *addr,
												const char **className);

int _ILDumpCVMInsn(FILE *stream, ILMethod *currMethod, unsigned char *pc)
{
	const CVMOpcode *opcode = &(opcodes[pc[0]]);
	int size = 0;
	unsigned char *dest;
	ILMethod *method;
	ILClass *classInfo;
	ILProgramItem *item;
	ILField *field;
	unsigned long token;
	const char *str;
	unsigned long strLen;
	unsigned long numCases;
	const ILMethodTableEntry *methodEntry;

	/* Dump the address of the instruction */
	fprintf(stream, "0x%08lX:  ", (unsigned long)pc);

	/* Dump the name of the instruction if it is not a prefix */
	if(opcode->operands != CVM_OPER_BRANCH_LONG &&
	   opcode->operands != CVM_OPER_WIDE &&
	   opcode->operands != CVM_OPER_PREFIX)
	{
		fprintf(stream, "%-20s", opcode->name);
	}

	/* Determine how to dump the instruction operands */
	switch(opcode->operands)
	{
		case CVM_OPER_NONE:
		{
			size = 1;
		}
		break;

		case CVM_OPER_UINT8:
		case CVM_OPER_WIDE_UINT:
		{
			fprintf(stream, "%d", (int)(pc[1]));
			size = 2;
		}
		break;

		case CVM_OPER_TWO_UINT8:
		case CVM_OPER_WIDE_TWO_UINT:
		{
			fprintf(stream, "%d, %d", (int)(pc[1]), (int)(pc[2]));
			size = 3;
		}
		break;

		case CVM_OPER_UINT32:
		{
			fprintf(stream, "%lu", (unsigned long)(IL_READ_UINT32(pc + 1)));
			size = 5;
		}
		break;

		case CVM_OPER_INT8:
		{
			fprintf(stream, "%d", (int)(ILInt8)(pc[1]));
			size = 2;
		}
		break;

		case CVM_OPER_INT32:
		{
			fprintf(stream, "%ld", (long)(IL_READ_INT32(pc + 1)));
			size = 5;
		}
		break;

		case CVM_OPER_UINT64:
		{
			fprintf(stream, "0x%08lX%08lX",
					(unsigned long)(IL_READ_INT32(pc + 5)),
					(unsigned long)(IL_READ_INT32(pc + 1)));
			size = 9;
		}
		break;

		case CVM_OPER_FLOAT32:
		{
			fprintf(stream, "%g", (double)(IL_READ_FLOAT(pc + 1)));
			size = 5;
		}
		break;

		case CVM_OPER_FLOAT64:
		{
			fprintf(stream, "%g", (double)(IL_READ_DOUBLE(pc + 1)));
			size = 9;
		}
		break;

		case CVM_OPER_BRANCH:
		{
			dest = pc + (int)(ILInt8)(pc[1]);
			fprintf(stream, "0x%08lX", (unsigned long)dest);
			size = 6;
		}
		break;

		case CVM_OPER_BRANCH_LONG:
		{
			dest = pc + IL_READ_INT32(pc + 2);
			fprintf(stream, "%-20s0x%08lX",
					opcodes[pc[1]].name, (unsigned long)dest);
			size = 6;
		}
		break;

		case CVM_OPER_SWITCH:
		{
			numCases = IL_READ_UINT32(pc + 1);
			fprintf(stream, "%lu", numCases);
			size = 5;
			while(numCases > 0)
			{
				dest = pc + IL_READ_INT32(pc + size);
				fprintf(stream, "\n             %-20s0x%08lX",
						"swlabel", (unsigned long)dest);
				size += 4;
				--numCases;
			}
		}
		break;

		case CVM_OPER_CALL:
		case CVM_OPER_CALL_EXTERN:
		{
			method = (ILMethod *)CVMReadPointer(pc + 5);
			ILDumpMethodType(stream, ILProgramItem_Image(currMethod),
							 ILMethod_Signature(method), 0,
							 ILMethod_Owner(method),
							 ILMethod_Name(method), method);
			size = 5 + sizeof(void *);
		}
		break;

		case CVM_OPER_CALL_NATIVE:
		{
			fprintf(stream, "0x%08lX (",
					(unsigned long)(CVMReadPointer(pc + 2)));
			methodEntry = _ILFindInternalByAddr(CVMReadPointer(pc + 2),
												&str);
			if(methodEntry)
			{
				fprintf(stream, "%s.%s \"%s\"", str, methodEntry->methodName,
						(methodEntry->signature ?
							methodEntry->signature :
							methodEntry[-1].signature));
			}
			else
			{
				putc('?', stream);
			}
			putc(')', stream);
			size = sizeof(void *) * 2 + 2;
		}
		break;

		case CVM_OPER_CALL_INTERFACE:
		{
			classInfo = (ILClass *)CVMReadPointer(pc + 3);
			ILDumpClassName(stream, ILProgramItem_Image(currMethod),
							classInfo, 0);
			fprintf(stream, ", %d, %d", (int)(pc[1]), (int)(pc[2]));
			size = 3 + sizeof(void *);
		}
		break;

		case CVM_OPER_CLASS:
		{
			classInfo = (ILClass *)CVMReadPointer(pc + 1);
			ILDumpClassName(stream, ILProgramItem_Image(currMethod),
							classInfo, 0);
			size = 1 + sizeof(void *);
		}
		break;

		case CVM_OPER_UINT_AND_CLASS:
		{
			classInfo = (ILClass *)CVMReadPointer(pc + 2);
			ILDumpClassName(stream, ILProgramItem_Image(currMethod),
							classInfo, 0);
			fprintf(stream, ", %d", (int)(pc[1]));
			size = 2 + sizeof(void *);
		}
		break;

		case CVM_OPER_ITEM:
		{
			item = (ILProgramItem *)CVMReadPointer(pc + 1);
			if((classInfo = ILProgramItemToClass(item)) != 0)
			{
				ILDumpClassName(stream, ILProgramItem_Image(currMethod),
								classInfo, 0);
			}
			else if((method = ILProgramItemToMethod(item)) != 0)
			{
				ILDumpMethodType(stream, ILProgramItem_Image(currMethod),
								 ILMethod_Signature(method), 0,
								 ILMethod_Owner(method),
								 ILMethod_Name(method), method);
			}
			else if((field = ILProgramItemToField(item)) != 0)
			{
				ILDumpType(stream, ILProgramItem_Image(currMethod),
						   ILField_Type(field), 0);
				putc(' ', stream);
				ILDumpClassName(stream, ILProgramItem_Image(currMethod),
								ILField_Owner(field), 0);
				fputs("::", stream);
				ILDumpIdentifier(stream, ILField_Name(field), 0, 0);
			}
			size = 1 + sizeof(void *);
		}
		break;

		case CVM_OPER_STRING:
		{
			token = IL_READ_UINT32(pc + 1) & ~IL_META_TOKEN_MASK;
			str = ILImageGetUserString(ILProgramItem_Image(currMethod),
									   token, &strLen);
			if(str)
			{
				ILDumpUnicodeString(stream, str, strLen);
			}
			size = 5;
		}
		break;

		case CVM_OPER_WIDE:
		{
			opcode = &(opcodes[pc[1]]);
			fprintf(stream, "%-20s", opcode->name);
			switch(opcode->operands)
			{
				case CVM_OPER_WIDE_UINT:
				{
					fprintf(stream, "%lu",
							(unsigned long)IL_READ_UINT32(pc + 2));
					size = 6;
				}
				break;

				case CVM_OPER_WIDE_TWO_UINT:
				{
					fprintf(stream, "%lu, %lu",
							(unsigned long)IL_READ_UINT32(pc + 2),
							(unsigned long)IL_READ_UINT32(pc + 6));
					size = 10;
				}
				break;

				case CVM_OPER_CALL_INTERFACE:
				{
					classInfo = (ILClass *)CVMReadPointer(pc + 10);
					ILDumpClassName(stream, ILProgramItem_Image(currMethod),
									classInfo, 0);
					fprintf(stream, ", %lu, %lu",
							(unsigned long)IL_READ_UINT32(pc + 2),
							(unsigned long)IL_READ_UINT32(pc + 6));
					size = 10 + sizeof(void *);
				}
				break;

				case CVM_OPER_CALL_NATIVE:
				{
					fprintf(stream, "0x%08lX (",
							(unsigned long)(CVMReadPointer(pc + 6)));
					methodEntry = _ILFindInternalByAddr(CVMReadPointer(pc + 6),
														&str);
					if(methodEntry)
					{
						fprintf(stream, "%s.%s \"%s\"",
								str, methodEntry->methodName,
								(methodEntry->signature ?
									methodEntry->signature : "()V"));
					}
					else
					{
						putc('?', stream);
					}
					putc(')', stream);
					size = sizeof(void *) * 2 + 6;
				}
				break;

				case CVM_OPER_UINT_AND_CLASS:
				{
					classInfo = (ILClass *)CVMReadPointer(pc + 6);
					ILDumpClassName(stream, ILProgramItem_Image(currMethod),
									classInfo, 0);
					fprintf(stream, ", %lu",
							(unsigned long)(IL_READ_UINT32(pc + 2)));
					size = 6 + sizeof(void *);
				}
				break;

				default:
				{
					size = 2;
				}
				break;
			}
		}
		break;

		case CVM_OPER_PREFIX:
		{
			opcode = &(prefixOpcodes[pc[1]]);
			fprintf(stream, "%-20s", opcode->name);
			switch(opcode->operands)
			{
				case CVM_OPER_NONE:
				{
					size = 2;
				}
				break;

				case CVM_OPER_ENTER_TRY:
				{
					dest = pc + IL_READ_INT32(pc + 2);
					fprintf(stream, "0x%08lX", (unsigned long)dest);
					size = 6;
				}
				break;

				case CVM_OPER_CLASS:
				{
					classInfo = (ILClass *)CVMReadPointer(pc + 2);
					ILDumpClassName(stream, ILProgramItem_Image(currMethod),
									classInfo, 0);
					size = 2 + sizeof(void *);
				}
				break;

				case CVM_OPER_UINT32:
				{
					fprintf(stream, "%lu",
							(unsigned long)(IL_READ_UINT32(pc + 2)));
					size = 6;
				}
				break;

				case CVM_OPER_METHOD:
				{
					method = (ILMethod *)CVMReadPointer(pc + 2);
					ILDumpMethodType(stream, ILProgramItem_Image(currMethod),
									 ILMethod_Signature(method), 0,
									 ILMethod_Owner(method),
									 ILMethod_Name(method), method);
					size = 2 + sizeof(void *);
				}
				break;

				case CVM_OPER_LD_INTERFACE:
				{
					classInfo = (ILClass *)CVMReadPointer(pc + 6);
					ILDumpClassName(stream, ILProgramItem_Image(currMethod),
									classInfo, 0);
					fprintf(stream, ", %lu",
							(unsigned long)(IL_READ_UINT32(pc + 2)));
					size = 6 + sizeof(void *);
				}
				break;

				default:
				{
					size = 2;
				}
				break;
			}
		}
		break;

		default:
		{
			size = 1;
		}
		break;
	}

	/* Terminate the line and return the size */
	putc('\n', stream);
	fflush(stdout);
	return size;
}

#ifdef	__cplusplus
};
#endif
