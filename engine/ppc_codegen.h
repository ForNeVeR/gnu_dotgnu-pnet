/*
 * ppc_codegen.h - Code generation macros for the PPC processor.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

#ifndef	_PPC_CODEGEN_H
#define	_PPC_CODEGEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Register numbers.
 */
typedef enum
{
	PPC_R0   = 0,
	PPC_R1   = 1,
	PPC_R2   = 2,
	PPC_R3   = 3,
	PPC_R4   = 4,
	PPC_R5   = 5,
	PPC_R6   = 6,
	PPC_R7   = 7,
	PPC_R8   = 8,
	PPC_R9   = 9,
	PPC_R10  = 10,
	PPC_R11  = 11,
	PPC_R12  = 12,
	PPC_R13  = 13,
	PPC_R14  = 14,
	PPC_R15  = 15,
	PPC_R16  = 16,
	PPC_R17  = 17,
	PPC_R18  = 18,
	PPC_R19  = 19,
	PPC_R20  = 20,
	PPC_R21  = 21,
	PPC_R22  = 22,
	PPC_R23  = 23,
	PPC_R24  = 24,
	PPC_R25  = 25,
	PPC_R26  = 26,
	PPC_R27  = 27,
	PPC_R28  = 28,
	PPC_R29  = 29,
	PPC_R30  = 30,
	PPC_R31  = 31,
	PPC_SP   = PPC_R1,
	PPC_WORK = PPC_R12,

} PPC_REG;

/*
 * Floating-point register numbers.
 */
typedef enum
{
	PPC_F0   = 0,
	PPC_F1   = 1,
	PPC_F2   = 2,
	PPC_F3   = 3,
	PPC_F4   = 4,
	PPC_F5   = 5,
	PPC_F6   = 6,
	PPC_F7   = 7,
	PPC_F8   = 8,
	PPC_F9   = 9,
	PPC_F10  = 10,
	PPC_F11  = 11,
	PPC_F12  = 12,
	PPC_F13  = 13,
	PPC_F14  = 14,
	PPC_F15  = 15,
	PPC_F16  = 16,
	PPC_F17  = 17,
	PPC_F18  = 18,
	PPC_F19  = 19,
	PPC_F20  = 20,
	PPC_F21  = 21,
	PPC_F22  = 22,
	PPC_F23  = 23,
	PPC_F24  = 24,
	PPC_F25  = 25,
	PPC_F26  = 26,
	PPC_F27  = 27,
	PPC_F28  = 28,
	PPC_F29  = 29,
	PPC_F30  = 30,
	PPC_F31  = 31,

} PPC_FREG;

/*
 * Condition codes for branch instructions.
 */
typedef enum
{
	PPC_CC_BRFALSE	= (4 << 5),				/* Branch if condition false */
	PPC_CC_BRTRUE	= (12 << 5),			/* Branch if condition true */
	PPC_CC_EQ		= PPC_CC_BRTRUE | 2,	/* Equal */
	PPC_CC_NE		= PPC_CC_BRFALSE | 2,	/* Not equal */
	PPC_CC_LT		= PPC_CC_BRTRUE | 0,	/* Less than */
	PPC_CC_LE		= PPC_CC_BRFALSE | 1,	/* Less than or equal */
	PPC_CC_GT		= PPC_CC_BRTRUE | 1,	/* Greater than */
	PPC_CC_GE		= PPC_CC_BRFALSE | 0,	/* Greater than or equal */

} PPC_CC;

/*
 * Arithmetic and logical operations.
 */
typedef enum
{
	PPC_ADD		= 266,			/* Add */
	PPC_ADDC	= 10,			/* Add with carry out */
	PPC_ADDE	= 138,			/* Add with extended carry in */
	PPC_SUBF	= 40,			/* Subtract from */
	PPC_SUBFC	= 8,			/* Subtract from with carry out */
	PPC_SUBFE	= 136,			/* Subtract from with extended carry in */
	PPC_MUL		= 235,			/* Multiply */
	PPC_DIV		= 491,			/* Signed division */
	PPC_DIV_UN	= 459,			/* Unsigned division */
	PPC_AND		= 28,			/* And (sds) */
	PPC_ANDC	= 60,			/* And with complement (sds) */
	PPC_NAND	= 476,			/* Nand (sds) */
	PPC_NOR		= 124,			/* Nor (sds) */
	PPC_OR		= 444,			/* Or (sds) */
	PPC_ORC		= 412,			/* Or with complement (sds) */
	PPC_XOR		= 316,			/* Xor (sds) */
	PPC_CMP		= 0,			/* Signed compare */
	PPC_CMPL	= 32,			/* Unsigned (logical) compare */
	PPC_NEG		= 104,			/* Negate */
	PPC_SL		= 24,			/* Shift left (sds) */
	PPC_SRA		= 792,			/* Shift right algebraic (sds) */
	PPC_SR		= 536,			/* Shift right (sds) */
	PPC_EXTSB	= 954,			/* Extend signed byte */
	PPC_EXTSH	= 922,			/* Extend signed half word */

} PPC_OP;

/*
 * Type for instruction pointers (word-based, not byte-based).
 */
typedef unsigned int *ppc_inst_ptr;

/*
 * Perform an arithmetic or logical operation.
 */
#define	ppc_alu_reg_dss(inst,opc,dreg,sreg1,sreg2)	\
			do { \
				*(inst)++ = ((31 << 26) | \
							 (((unsigned int)(dreg)) << 21) | \
							 (((unsigned int)(sreg1)) << 16) | \
							 (((unsigned int)(sreg2)) << 11) | \
							 (((unsigned int)(opc)) << 1)); \
			} while (0)
#define	ppc_alu_reg_sds(inst,opc,dreg,sreg1,sreg2)	\
			do { \
				*(inst)++ = ((31 << 26) | \
							 (((unsigned int)(sreg1)) << 21) | \
							 (((unsigned int)(dreg)) << 16) | \
							 (((unsigned int)(sreg2)) << 11) | \
							 (((unsigned int)(opc)) << 1)); \
			} while (0)
#define	ppc_alu_reg_ds(inst,opc,dreg,sreg)	\
			do { \
				*(inst)++ = ((31 << 26) | \
							 (((unsigned int)(dreg)) << 21) | \
							 (((unsigned int)(sreg)) << 16) | \
							 (((unsigned int)(opc)) << 1)); \
			} while (0)
#define	ppc_alu_reg_sd(inst,opc,dreg,sreg)	\
			do { \
				*(inst)++ = ((31 << 26) | \
							 (((unsigned int)(sreg)) << 21) | \
							 (((unsigned int)(dreg)) << 16) | \
							 (((unsigned int)(opc)) << 1)); \
			} while (0)

/*
 * Perform an arithmetic or logical operation and also set the
 * condition codes.
 */
#define	ppc_alu_cc_reg_dss(inst,opc,dreg,sreg1,sreg2)	\
			do { \
				*(inst)++ = ((31 << 26) | \
							 (((unsigned int)(dreg)) << 21) | \
							 (((unsigned int)(sreg1)) << 16) | \
							 (((unsigned int)(sreg2)) << 11) | \
							 (((unsigned int)(opc)) << 1) | 1); \
			} while (0)
#define	ppc_alu_cc_reg_sds(inst,opc,dreg,sreg1,sreg2)	\
			do { \
				*(inst)++ = ((31 << 26) | \
							 (((unsigned int)(sreg1)) << 21) | \
							 (((unsigned int)(dreg)) << 16) | \
							 (((unsigned int)(sreg2)) << 11) | \
							 (((unsigned int)(opc)) << 1) | 1); \
			} while (0)
#define	ppc_alu_cc_reg_ds(inst,opc,dreg,sreg)	\
			do { \
				*(inst)++ = ((31 << 26) | \
							 (((unsigned int)(dreg)) << 21) | \
							 (((unsigned int)(sreg)) << 16) | \
							 (((unsigned int)(opc)) << 1) | 1); \
			} while (0)
#define	ppc_alu_cc_reg_sd(inst,opc,dreg,sreg)	\
			do { \
				*(inst)++ = ((31 << 26) | \
							 (((unsigned int)(sreg)) << 21) | \
							 (((unsigned int)(dreg)) << 16) | \
							 (((unsigned int)(opc)) << 1) | 1); \
			} while (0)

/*
 * Perform a comparison on two registers.  The result is put into CR0.
 */
#define	ppc_cmp_reg_reg(inst,opc,reg1,reg2)	\
			do { \
				*(inst)++ = ((31 << 26) | \
							 (((unsigned int)(reg1)) << 16) | \
							 (((unsigned int)(reg2)) << 11) | \
							 (((unsigned int)(opc)) << 1)); \
			} while (0)

/*
 * Add an immediate value to a register.  If "sreg" is R0, then this
 * instruction acts like "ppc_mov_reg_imm" instead.
 */
#define	ppc_add_reg_imm(inst,dreg,sreg,imm)	\
			do { \
				int __value = (int)(imm); \
				if(__value >= -0x8000 && __value <= 0x7FFF) \
				{ \
					*(inst)++ = ((14 << 26) | \
								 (((unsigned int)(dreg)) << 21) | \
								 (((unsigned int)(sreg)) << 16) | \
								 (((unsigned int)(__value)) & 0xFFFF)); \
				} \
				else if((__value & 0xFFFF) == 0) \
				{ \
					*(inst)++ = ((15 << 26) | \
								 (((unsigned int)(dreg)) << 21) | \
								 (((unsigned int)(sreg)) << 16) | \
								 (((unsigned int)(__value >> 16)) & 0xFFFF)); \
				} \
				else if((__value & 0x8000) == 0) \
				{ \
					*(inst)++ = ((14 << 26) | \
								 (((unsigned int)PPC_WORK) << 21) | \
								 (((unsigned int)(sreg)) << 16) | \
								 (((unsigned int)(__value)) & 0xFFFF)); \
					*(inst)++ = ((15 << 26) | \
								 (((unsigned int)(dreg)) << 21) | \
								 (((unsigned int)PPC_WORK) << 16) | \
								 (((unsigned int)(__value >> 16)) & 0xFFFF)); \
				} \
				else \
				{ \
					*(inst)++ = ((14 << 26) | \
								 (((unsigned int)PPC_WORK) << 21) | \
								 (((unsigned int)(sreg)) << 16) | \
								 (((unsigned int)(__value)) & 0xFFFF)); \
					*(inst)++ = ((15 << 26) | \
								 (((unsigned int)(dreg)) << 21) | \
								 (((unsigned int)PPC_WORK) << 16) | \
								 (((unsigned int)((__value >> 16) + 1)) \
								 		& 0xFFFF)); \
				} \
			} while (0)

/*
 * Subtract an immediate value from a register.  "sreg" and "dreg"
 * must not be R0.
 */
#define	ppc_sub_reg_imm(inst,dreg,sreg,imm)	\
			do { \
				ppc_add_reg_imm((inst), (dreg), (sreg), -((int)(imm))); \
			} while (0)

/*
 * Move an immediate value into a register.
 */
#define	ppc_mov_reg_imm(inst,reg,imm)	\
			do { \
				ppc_add_reg_imm((inst), (reg), PPC_R0, (imm)); \
			} while (0)

/*
 * And an immediate value with a register.
 */
#define	ppc_and_reg_imm(inst,dreg,sreg,imm)	\
			do { \
				int __and_value = (int)(imm); \
				if(__and_value >= 0 && __and_value <= 0xFFFF) \
				{ \
					*(inst)++ = ((28 << 26) | \
								 (((unsigned int)(sreg)) << 21) | \
								 (((unsigned int)(dreg)) << 16) | \
								 (((unsigned int)(__and_value)) & 0xFFFF)); \
				} \
				else if((__and_value & 0xFFFF) == 0) \
				{ \
					*(inst)++ = ((29 << 26) | \
								 (((unsigned int)(sreg)) << 21) | \
								 (((unsigned int)(dreg)) << 16) | \
								 ((((unsigned int)(__and_value)) >> 16) & \
								 		0xFFFF)); \
				} \
				else \
				{ \
					ppc_mov_reg_imm((inst), PPC_WORK, __and_value); \
					ppc_alu_reg_sds((inst), PPC_AND, (dreg), \
									(sreg), PPC_WORK); \
				} \
			} while (0)

/*
 * Move a value from one register to another.
 */
#define	ppc_mov_reg_reg(inst,dreg,sreg)	\
			do { \
				if((sreg) != PPC_R0) \
				{ \
					*(inst)++ = ((14 << 26) | \
								 (((unsigned int)(dreg)) << 21) | \
								 (((unsigned int)(sreg)) << 16)); \
				} \
				else \
				{ \
					*(inst)++ = ((14 << 26) | \
								 (((unsigned int)(dreg)) << 21) | \
								 (((unsigned int)PPC_R0) << 16)); \
					ppc_alu_reg_dss((inst), PPC_ADD, (dreg), (dreg), (sreg)); \
				} \
			} while (0)

/*
 * Compare a register against an immediate value.  The result ends up in CR0.
 */
#define	ppc_cmp_reg_imm(inst,opc,reg,imm)	\
			do { \
				int __value = (int)(imm); \
				if((opc) == PPC_CMP) \
				{ \
					if(__value >= -0x8000 && __value <= 0x7FFF) \
					{ \
						*(inst)++ = ((11 << 26) | \
									 (((unsigned int)(reg)) << 16) | \
									 (((unsigned int)(__value)) & 0xFFFF)); \
					} \
					else \
					{ \
						ppc_mov_reg_imm((inst), PPC_WORK, __value); \
						ppc_cmp_reg_reg((inst), PPC_CMP, (reg), PPC_WORK); \
					} \
				} \
				else \
				{ \
					if(__value >= 0 && __value <= 0xFFFF) \
					{ \
						*(inst)++ = ((10 << 26) | \
									 (((unsigned int)(reg)) << 16) | \
									  ((unsigned int)(__value))); \
					} \
					else \
					{ \
						ppc_mov_reg_imm((inst), PPC_WORK, __value); \
						ppc_cmp_reg_reg((inst), PPC_CMPL, (reg), PPC_WORK); \
					} \
				} \
			} while (0)

/*
 * Jump to a particular address.
 */
#define	ppc_jump(inst,target)	\
			do { \
				/* TODO */ \
			} while (0)

/*
 * Jump to a particular address based on a condition.
 */
#define	ppc_branch(inst,cond,target)	\
			do { \
				/* TODO */ \
			} while (0)

/*
 * Load various sized values from a memory location defined by a base
 * register and an offset.
 */
extern ppc_inst_ptr _md_ppc_get_ea(ppc_inst_ptr inst, int basereg, int offset);
#define	ppc_membase_common(inst,reg,basereg,offset,opc)	\
			do { \
				int __ld_offset = (int)(offset); \
				if(__ld_offset >= -0x8000 && __ld_offset <= 0x7FFF) \
				{ \
					*(inst)++ = (((opc) << 26) | \
								 (((unsigned int)(reg)) << 21) | \
								 (((unsigned int)(basereg)) << 16) | \
								 (((unsigned int)(__ld_offset)) & 0xFFFF)); \
				} \
				else \
				{ \
					(inst) = _md_ppc_get_ea((inst), (basereg), __ld_offset); \
					*(inst)++ = (((opc) << 26) | \
								 (((unsigned int)(reg)) << 21) | \
								 (((unsigned int)PPC_WORK) << 16)); \
				} \
			} while (0)
#define	ppc_load_membase(inst,reg,basereg,offset)	\
			ppc_membase_common((inst), (reg), (basereg), (offset), 32)
#define	ppc_load_membase_byte(inst,reg,basereg,offset)	\
			ppc_membase_common((inst), (reg), (basereg), (offset), 34)
#define	ppc_load_membase_sbyte(inst,reg,basereg,offset)	\
			do { \
				ppc_membase_common((inst), (reg), (basereg), (offset), 34); \
				ppc_alu_reg_sd((inst), PPC_EXTSB, (reg), (reg)); \
			} while (0)
#define	ppc_load_membase_short(inst,reg,basereg,offset)	\
			ppc_membase_common((inst), (reg), (basereg), (offset), 42)
#define	ppc_load_membase_ushort(inst,reg,basereg,offset)	\
			ppc_membase_common((inst), (reg), (basereg), (offset), 40)

/*
 * Store values of various sizes to a memory location defined by a
 * base register plus an offset.
 */
#define	ppc_store_membase(inst,reg,basereg,offset)	\
			ppc_membase_common((inst), (reg), (basereg), (offset), 36)
#define	ppc_store_membase_byte(inst,reg,basereg,offset)	\
			ppc_membase_common((inst), (reg), (basereg), (offset), 38)
#define	ppc_store_membase_sbyte(inst,reg,basereg,offset)	\
			ppc_membase_common((inst), (reg), (basereg), (offset), 38)
#define	ppc_store_membase_short(inst,reg,basereg,offset)	\
			ppc_membase_common((inst), (reg), (basereg), (offset), 44)
#define	ppc_store_membase_ushort(inst,reg,basereg,offset)	\
			ppc_membase_common((inst), (reg), (basereg), (offset), 44)

/*
 * Push the contents of a word register onto the stack.
 */
#define	ppc_push_reg(inst,reg)	\
			ppc_membase_common((inst), (reg), PPC_SP, -4, 37)

/*
 * Pop the contents of a word register from the stack.
 */
#define	ppc_pop_reg(inst,reg)	\
			do { \
				ppc_membase_common((inst), (reg), PPC_SP, 0, 32); \
				ppc_add_reg_imm((inst), PPC_SP, PPC_SP, 4); \
			} while (0)

#ifdef __cplusplus
};
#endif

#endif /* _PPC_CODEGEN_H */
