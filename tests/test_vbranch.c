/*
 * test_vbranch.c - Test the bytecode verifier for branch instructions.
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

#include "test_verify.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Test the binary nature of a branch instruction.
 */
#define	declareBinaryBranchNature(name,iname)	\
static char *name##_binary_1[] = { \
	/* FAIL: needs 2 or more stack arguments */ \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0 \
}; \
static char *name##_binary_2[] = { \
	/* FAIL: needs 2 or more stack arguments */ \
	"	ldc.i4.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0 \
}; \
static char *name##_binary_3[] = { \
	/* OK: needs 2 or more stack arguments */ \
	"	ldc.i4.0", \
	"	ldc.i4.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0 \
}; \
static char *name##_binary_4[] = { \
	/* OK: does not push any values */ \
	"2", \
	"	.locals (int32)", \
	"	ldc.i4.0", \
	"	ldc.i4.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0 \
}; \
static char *name##_binary_5[] = { \
	/* FAIL: pops two values */ \
	"	ldc.i4.0", \
	"	ldc.i4.0", \
	"	" iname " label", \
	"	pop", \
	"	ret", \
	"label:", \
	"	pop", \
	"	ret", \
	0 \
}
#define	testBinaryBranchNature(name)	\
	do { \
		RegisterSimpleFail(name##_binary_1); \
		RegisterSimpleFail(name##_binary_2); \
		RegisterSimple(name##_binary_3); \
		RegisterSimple(name##_binary_4); \
		RegisterSimpleFail(name##_binary_5); \
	} while (0)

/*
 * Test the unary nature of a branch instruction.
 */
#define	declareUnaryBranchNature(name,iname)	\
static char *name##_unary_1[] = { \
	/* FAIL: needs 1 or more stack arguments */ \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0 \
}; \
static char *name##_unary_2[] = { \
	/* OK: needs 1 or more stack arguments */ \
	"	ldc.i4.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0 \
}; \
static char *name##_unary_3[] = { \
	/* OK: does not push any values */ \
	"1", \
	"	.locals (int32)", \
	"	ldc.i4.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0 \
}; \
static char *name##_unary_4[] = { \
	/* FAIL: pops 1 value */ \
	"	ldc.i4.0", \
	"	" iname " label", \
	"	pop", \
	"	ret", \
	"label:", \
	"	pop", \
	"	ret", \
	0 \
}
#define	testUnaryBranchNature(name)	\
	do { \
		RegisterSimpleFail(name##_unary_1); \
		RegisterSimple(name##_unary_2); \
		RegisterSimple(name##_unary_3); \
		RegisterSimpleFail(name##_unary_4); \
	} while (0)

/*
 * Test the safe arithmetic combinations for a binary branch instruction.
 */
#define	declareBinaryBranchOperands(name, iname) \
static char *name##_operands_1[] = { \
	/* OK: i4 and i4 */ \
	"	ldc.i4.3", \
	"	ldc.i4.5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_2[] = { \
	/* FAIL: i4 and i8 */ \
	"	ldc.i4.3", \
	"	ldc.i8	5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_3[] = { \
	/* OK: i4 and i */ \
	"	ldc.i4.3", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_4[] = { \
	/* FAIL: i8 and i4 */ \
	"	ldc.i8	5", \
	"	ldc.i4.3", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_5[] = { \
	/* OK: i8 and i8 */ \
	"	ldc.i8	3", \
	"	ldc.i8	5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_6[] = { \
	/* FAIL: i8 and i */ \
	"	ldc.i8	3", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_7[] = { \
	/* OK: i and i4 */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i4.5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_8[] = { \
	/* FAIL: i and i8 */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i8	5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_9[] = { \
	/* OK: i and i */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_10[] = { \
	/* FAIL: i4 and f */ \
	"	ldc.i4.3", \
	"	ldc.r4	5.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_11[] = { \
	/* FAIL: i8 and f */ \
	"	ldc.i8	3", \
	"	ldc.r4	5.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_12[] = { \
	/* FAIL: i and f */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.r4	5.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_13[] = { \
	/* FAIL: f and i4 */ \
	"	ldc.r4	5.0", \
	"	ldc.i4.3", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_14[] = { \
	/* FAIL: f and i8 */ \
	"	ldc.r4	5.0", \
	"	ldc.i8	3", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_15[] = { \
	/* FAIL: f and i */ \
	"	ldc.r4	5.0", \
	"	ldc.i4.3", \
	"	conv.i", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_16[] = { \
	/* OK: f and f */ \
	"	ldc.r4	3.0", \
	"	ldc.r4	5.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}
#define	testBinaryBranchOperands(name)	\
	do { \
		RegisterSimple(name##_operands_1); \
		RegisterSimpleFail(name##_operands_2); \
		RegisterSimple(name##_operands_3); \
		RegisterSimpleFail(name##_operands_4); \
		RegisterSimple(name##_operands_5); \
		RegisterSimpleFail(name##_operands_6); \
		RegisterSimple(name##_operands_7); \
		RegisterSimpleFail(name##_operands_8); \
		RegisterSimple(name##_operands_9); \
		RegisterSimpleFail(name##_operands_10); \
		RegisterSimpleFail(name##_operands_11); \
		RegisterSimpleFail(name##_operands_12); \
		RegisterSimpleFail(name##_operands_13); \
		RegisterSimpleFail(name##_operands_14); \
		RegisterSimpleFail(name##_operands_15); \
		RegisterSimple(name##_operands_16); \
	} while (0)

/*
 * Test the pointer combinations for a binary branch operator.
 */
#define	declareBranchPointerOperands(name, iname) \
static char *name##_ptr_1[] = { \
	/* OK: m and i4 */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldc.i4.5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_2[] = { \
	/* FAIL: m and i8 */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldc.i8	5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_3[] = { \
	/* OK: m and i */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_4[] = { \
	/* FAIL: m and f */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldc.r4	5.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_5[] = { \
	/* FAIL: m and o */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldnull", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_6[] = { \
	/* FAIL for add and none, OK for sub: m and m */ \
	"	.locals (int32, int32)", \
	"	ldloca	0", \
	"	ldloca	1", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_7[] = { \
	/* FAIL: o and i4 */ \
	"	ldnull", \
	"	ldc.i4.5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_8[] = { \
	/* FAIL: o and i8 */ \
	"	ldnull", \
	"	ldc.i8	5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_9[] = { \
	/* FAIL: o and i */ \
	"	ldnull", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_10[] = { \
	/* FAIL: o and f */ \
	"	ldnull", \
	"	ldc.r4	5.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_11[] = { \
	/* FAIL: o and m */ \
	"	.locals (int32)", \
	"	ldnull", \
	"	ldloca	0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_12[] = { \
	/* FAIL: o and o */ \
	"	ldnull", \
	"	ldnull", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_13[] = { \
	/* OK: i4 and m */ \
	"	.locals (int32)", \
	"	ldc.i4.5", \
	"	ldloca	0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_14[] = { \
	/* FAIL: i8 and m */ \
	"	.locals (int32)", \
	"	ldc.i8	5", \
	"	ldloca	0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_15[] = { \
	/* OK: i and m */ \
	"	.locals (int32)", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	ldloca	0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_16[] = { \
	/* FAIL: f and m */ \
	"	.locals (int32)", \
	"	ldc.r4	5.0", \
	"	ldloca	0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_17[] = { \
	/* FAIL: i4 and o */ \
	"	ldc.i4.5", \
	"	ldnull", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_18[] = { \
	/* FAIL: i8 and o */ \
	"	ldc.i8	5", \
	"	ldnull", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_19[] = { \
	/* FAIL: i and o */ \
	"	ldc.i4.5", \
	"	conv.i", \
	"	ldnull", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_20[] = { \
	/* FAIL: f and o */ \
	"	ldc.r4	5.0", \
	"	ldnull", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_21[] = { \
	/* FAIL: mv and i4 */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldc.i4.5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_22[] = { \
	/* FAIL: mv and i8 */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldc.i8	5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_23[] = { \
	/* FAIL: mv and i */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_24[] = { \
	/* FAIL: mv and f */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldc.r4	5.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_25[] = { \
	/* FAIL: mv and m */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	.locals (int32)", \
	"	ldarg.0", \
	"	ldloca	0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_26[] = { \
	/* FAIL: mv and o */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldnull", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_27[] = { \
	/* FAIL: mv and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldarg.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_28[] = { \
	/* FAIL: i4 and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldc.i4.3", \
	"	ldarg.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_29[] = { \
	/* FAIL: i8 and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldc.i8	3", \
	"	ldarg.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_30[] = { \
	/* FAIL: i and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldarg.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_31[] = { \
	/* FAIL: f and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldc.r4	3.0", \
	"	ldarg.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_32[] = { \
	/* FAIL: m and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldarg.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_ptr_33[] = { \
	/* FAIL: o and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldnull", \
	"	ldarg.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}
#define	testBranchPointerOperands(name) \
	do { \
		RegisterSimpleFail(name##_ptr_1); \
		RegisterSimpleFail(name##_ptr_2); \
		RegisterSimpleFail(name##_ptr_3); \
		RegisterSimpleFail(name##_ptr_4); \
		RegisterSimpleFail(name##_ptr_5); \
		RegisterSimple(name##_ptr_6); \
		RegisterSimpleFail(name##_ptr_7); \
		RegisterSimpleFail(name##_ptr_8); \
		RegisterSimpleFail(name##_ptr_9); \
		RegisterSimpleFail(name##_ptr_10); \
		RegisterSimpleFail(name##_ptr_11); \
		RegisterSimpleFail(name##_ptr_12); \
		RegisterSimpleFail(name##_ptr_13); \
		RegisterSimpleFail(name##_ptr_14); \
		RegisterSimpleFail(name##_ptr_15); \
		RegisterSimpleFail(name##_ptr_16); \
		RegisterSimpleFail(name##_ptr_17); \
		RegisterSimpleFail(name##_ptr_18); \
		RegisterSimpleFail(name##_ptr_19); \
		RegisterSimpleFail(name##_ptr_20); \
		RegisterSimpleFail(name##_ptr_21); \
		RegisterSimpleFail(name##_ptr_22); \
		RegisterSimpleFail(name##_ptr_23); \
		RegisterSimpleFail(name##_ptr_24); \
		RegisterSimpleFail(name##_ptr_25); \
		RegisterSimpleFail(name##_ptr_26); \
		RegisterSimpleFail(name##_ptr_27); \
		RegisterSimpleFail(name##_ptr_28); \
		RegisterSimpleFail(name##_ptr_29); \
		RegisterSimpleFail(name##_ptr_30); \
		RegisterSimpleFail(name##_ptr_31); \
		RegisterSimpleFail(name##_ptr_32); \
		RegisterSimpleFail(name##_ptr_33); \
	} while (0)
#define	testBranchPointerOperandsEq(name) \
	do { \
		RegisterSimpleFail(name##_ptr_1); \
		RegisterSimpleFail(name##_ptr_2); \
		RegisterSimpleUnsafe(name##_ptr_3); \
		RegisterSimpleFail(name##_ptr_4); \
		RegisterSimpleFail(name##_ptr_5); \
		RegisterSimple(name##_ptr_6); \
		RegisterSimpleFail(name##_ptr_7); \
		RegisterSimpleFail(name##_ptr_8); \
		RegisterSimpleFail(name##_ptr_9); \
		RegisterSimpleFail(name##_ptr_10); \
		RegisterSimpleFail(name##_ptr_11); \
		RegisterSimple(name##_ptr_12); \
		RegisterSimpleFail(name##_ptr_13); \
		RegisterSimpleFail(name##_ptr_14); \
		RegisterSimpleUnsafe(name##_ptr_15); \
		RegisterSimpleFail(name##_ptr_16); \
		RegisterSimpleFail(name##_ptr_17); \
		RegisterSimpleFail(name##_ptr_18); \
		RegisterSimpleFail(name##_ptr_19); \
		RegisterSimpleFail(name##_ptr_20); \
		RegisterSimpleFail(name##_ptr_21); \
		RegisterSimpleFail(name##_ptr_22); \
		RegisterSimpleFail(name##_ptr_23); \
		RegisterSimpleFail(name##_ptr_24); \
		RegisterSimpleFail(name##_ptr_25); \
		RegisterSimpleFail(name##_ptr_26); \
		RegisterSimpleFail(name##_ptr_27); \
		RegisterSimpleFail(name##_ptr_28); \
		RegisterSimpleFail(name##_ptr_29); \
		RegisterSimpleFail(name##_ptr_30); \
		RegisterSimpleFail(name##_ptr_31); \
		RegisterSimpleFail(name##_ptr_32); \
		RegisterSimpleFail(name##_ptr_33); \
	} while (0)
#define	testBranchPointerOperandsGtUn(name) \
	do { \
		RegisterSimpleFail(name##_ptr_1); \
		RegisterSimpleFail(name##_ptr_2); \
		RegisterSimpleFail(name##_ptr_3); \
		RegisterSimpleFail(name##_ptr_4); \
		RegisterSimpleFail(name##_ptr_5); \
		RegisterSimple(name##_ptr_6); \
		RegisterSimpleFail(name##_ptr_7); \
		RegisterSimpleFail(name##_ptr_8); \
		RegisterSimpleFail(name##_ptr_9); \
		RegisterSimpleFail(name##_ptr_10); \
		RegisterSimpleFail(name##_ptr_11); \
		RegisterSimple(name##_ptr_12); \
		RegisterSimpleFail(name##_ptr_13); \
		RegisterSimpleFail(name##_ptr_14); \
		RegisterSimpleFail(name##_ptr_15); \
		RegisterSimpleFail(name##_ptr_16); \
		RegisterSimpleFail(name##_ptr_17); \
		RegisterSimpleFail(name##_ptr_18); \
		RegisterSimpleFail(name##_ptr_19); \
		RegisterSimpleFail(name##_ptr_20); \
		RegisterSimpleFail(name##_ptr_21); \
		RegisterSimpleFail(name##_ptr_22); \
		RegisterSimpleFail(name##_ptr_23); \
		RegisterSimpleFail(name##_ptr_24); \
		RegisterSimpleFail(name##_ptr_25); \
		RegisterSimpleFail(name##_ptr_26); \
		RegisterSimpleFail(name##_ptr_27); \
		RegisterSimpleFail(name##_ptr_28); \
		RegisterSimpleFail(name##_ptr_29); \
		RegisterSimpleFail(name##_ptr_30); \
		RegisterSimpleFail(name##_ptr_31); \
		RegisterSimpleFail(name##_ptr_32); \
		RegisterSimpleFail(name##_ptr_33); \
	} while (0)

/*
 * Test the operand combinations for a unary branch instruction.
 */
#define	declareUnaryBranchOperands(name, iname) \
static char *name##_operands_1[] = { \
	/* OK: i4 */ \
	"	ldc.i4.5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_2[] = { \
	/* OK: i8 */ \
	"	ldc.i8	5", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_3[] = { \
	/* OK: i */ \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_4[] = { \
	/* OK: f */ \
	"	ldc.r4	5.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_5[] = { \
	/* OK: m */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_6[] = { \
	/* OK: o */ \
	"	ldnull", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}; \
static char *name##_operands_7[] = { \
	/* FAIL: mv */ \
	"public static void test(valuetype [mscorlib]System.Decimal x)", \
	"	ldarg.0", \
	"	" iname " label", \
	"	ret", \
	"label:", \
	"	ret", \
	0, \
}
#define	testUnaryBranchOperands(name)	\
	do { \
		RegisterSimple(name##_operands_1); \
		RegisterSimple(name##_operands_2); \
		RegisterSimple(name##_operands_3); \
		RegisterSimple(name##_operands_4); \
		RegisterSimple(name##_operands_5); \
		RegisterSimple(name##_operands_6); \
		RegisterSimpleFail(name##_operands_7); \
	} while (0)

/*
 * Declare test blocks for branch instructions.
 */
static char *br_1[] = {
	/* OK: needs 0 stack arguments */
	"	br label",
	"	ret",
	"label:",
	"	ret",
	0
};
static char *br_2[] = {
	/* OK: does not push any values */
	"0",
	"	.locals (int32)",
	"	br label",
	"	ret",
	"label:",
	"	ret",
	0
};
static char *br_3[] = {
	/* OK: does not pop any values */
	"	ldc.i4.0",
	"	br label",
	"	ret",
	"label:",
	"	pop",
	"	ret",
	0
};

declareBinaryBranchNature(beq, "beq");
declareBinaryBranchOperands(beq, "beq");
declareBranchPointerOperands(beq, "beq");

declareBinaryBranchNature(bge, "bge");
declareBinaryBranchOperands(bge, "bge");
declareBranchPointerOperands(bge, "bge");

declareBinaryBranchNature(bgt, "bgt");
declareBinaryBranchOperands(bgt, "bgt");
declareBranchPointerOperands(bgt, "bgt");

declareBinaryBranchNature(ble, "ble");
declareBinaryBranchOperands(ble, "ble");
declareBranchPointerOperands(ble, "ble");

declareBinaryBranchNature(blt, "blt");
declareBinaryBranchOperands(blt, "blt");
declareBranchPointerOperands(blt, "blt");

declareBinaryBranchNature(bne_un, "bne.un");
declareBinaryBranchOperands(bne_un, "bne.un");
declareBranchPointerOperands(bne_un, "bne.un");

declareBinaryBranchNature(bge_un, "bge.un");
declareBinaryBranchOperands(bge_un, "bge.un");
declareBranchPointerOperands(bge_un, "bge.un");

declareBinaryBranchNature(bgt_un, "bgt.un");
declareBinaryBranchOperands(bgt_un, "bgt.un");
declareBranchPointerOperands(bgt_un, "bgt.un");

declareBinaryBranchNature(ble_un, "ble.un");
declareBinaryBranchOperands(ble_un, "ble.un");
declareBranchPointerOperands(ble_un, "ble.un");

declareBinaryBranchNature(blt_un, "blt.un");
declareBinaryBranchOperands(blt_un, "blt.un");
declareBranchPointerOperands(blt_un, "blt.un");

declareUnaryBranchNature(brfalse, "brfalse");
declareUnaryBranchOperands(brfalse, "brfalse");

declareUnaryBranchNature(brtrue, "brtrue");
declareUnaryBranchOperands(brtrue, "brtrue");

/*
 * Register branch unit tests.
 */
void registerBranchTests(void)
{
	ILUnitRegisterSuite("Branch Instructions");

	RegisterSimple(br_1);
	RegisterSimple(br_2);
	RegisterSimple(br_3);

	testBinaryBranchNature(beq);
	testBinaryBranchOperands(beq);
	testBranchPointerOperandsEq(beq);

	testBinaryBranchNature(bge);
	testBinaryBranchOperands(bge);
	testBranchPointerOperands(bge);

	testBinaryBranchNature(bgt);
	testBinaryBranchOperands(bgt);
	testBranchPointerOperands(bgt);

	testBinaryBranchNature(ble);
	testBinaryBranchOperands(ble);
	testBranchPointerOperands(ble);

	testBinaryBranchNature(blt);
	testBinaryBranchOperands(blt);
	testBranchPointerOperands(blt);

	testBinaryBranchNature(bne_un);
	testBinaryBranchOperands(bne_un);
	testBranchPointerOperandsEq(bne_un);

	testBinaryBranchNature(bge_un);
	testBinaryBranchOperands(bge_un);
	testBranchPointerOperands(bge_un);

	testBinaryBranchNature(bgt_un);
	testBinaryBranchOperands(bgt_un);
	testBranchPointerOperandsGtUn(bgt_un);

	testBinaryBranchNature(ble_un);
	testBinaryBranchOperands(ble_un);
	testBranchPointerOperands(ble_un);

	testBinaryBranchNature(blt_un);
	testBinaryBranchOperands(blt_un);
	testBranchPointerOperands(blt_un);

	testUnaryBranchNature(brfalse);
	testUnaryBranchOperands(brfalse);

	testUnaryBranchNature(brtrue);
	testUnaryBranchOperands(brtrue);
}

#ifdef	__cplusplus
};
#endif
