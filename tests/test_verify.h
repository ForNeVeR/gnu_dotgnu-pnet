/*
 * test_verify.h - Test the bytecode verifier in the runtime engine.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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

#ifndef	_TEST_VERIFY_H
#define	_TEST_VERIFY_H

#include "ilunit.h"
#include "../engine/engine.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Import the null coder from "null_coder.c".
 */
extern ILCoderClass const _ILNullCoderClass;
extern ILCoder _ILNullCoder;

/*
 * Build the assembly stream for a simple method test.
 */
void buildSimpleStream(void *arg);

/*
 * Verify all methods within the assembly image.  Returns zero on failure.
 */
int verify(int allowUnsafe);

/*
 * Verify all methods within the assembly image, with the expectation
 * that they must all fail.  Returns zero if something succeeds.
 */
int verifyAllFail(int allowUnsafe);

/*
 * Test a simple method defined by a list of instructions.
 */
void testSimple(void *arg);

/*
 * Test a simple method for verify failure defined by a list of instructions.
 */
void testSimpleFail(void *arg);

/*
 * Test a simple method defined by a list of instructions that is unsafe.
 */
void testSimpleUnsafe(void *arg);

/*
 * Register tests for various instruction categories.
 */
void registerArithTests(void);
void registerConvTests(void);
void registerCompareTests(void);
void registerBranchTests(void);
void registerConstantTests(void);
void registerVarTests(void);
void registerMiscTests(void);

/*
 * Register a simple test.
 */
#define	RegisterSimple(name)	   ILUnitRegister(#name, testSimple, name)
#define	RegisterSimpleFail(name)   ILUnitRegister(#name, testSimpleFail, name)
#define	RegisterSimpleUnsafe(name) ILUnitRegister(#name, testSimpleUnsafe, name)

/*
 * Test the binary nature of an arithmetic operator.
 */
#define	declareBinaryNature(name,iname)	\
static char *name##_binary_1[] = { \
	/* FAIL: needs 2 or more stack arguments */ \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_binary_2[] = { \
	/* FAIL: needs 2 or more stack arguments */ \
	"	ldc.i4.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_binary_3[] = { \
	/* OK: needs 2 or more stack arguments */ \
	"	ldc.i4.0", \
	"	ldc.i4.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_binary_4[] = { \
	/* OK: does not push any values */ \
	"2", \
	"	.locals (int32)", \
	"	ldc.i4.0", \
	"	ldc.i4.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_binary_5[] = { \
	/* OK: pops only 1 value */ \
	"	ldc.i4.0", \
	"	ldc.i4.0", \
	"	" iname, \
	"	pop", \
	"	ret", \
	0 \
}; \
static char *name##_binary_6[] = { \
	/* FAIL: pops only 1 value */ \
	"	ldc.i4.0", \
	"	ldc.i4.0", \
	"	" iname, \
	"	pop", \
	"	pop", \
	"	ret", \
	0 \
}
#define	testBinaryNature(name)	\
	do { \
		RegisterSimpleFail(name##_binary_1); \
		RegisterSimpleFail(name##_binary_2); \
		RegisterSimple(name##_binary_3); \
		RegisterSimple(name##_binary_4); \
		RegisterSimple(name##_binary_5); \
		RegisterSimpleFail(name##_binary_6); \
	} while (0)

/*
 * Test the unary nature of an arithmetic operator.
 */
#define	declareUnaryNature(name,iname)	\
static char *name##_unary_1[] = { \
	/* FAIL: needs 1 or more stack arguments */ \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_unary_2[] = { \
	/* OK: needs 1 or more stack arguments */ \
	"	ldc.i4.3", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_unary_3[] = { \
	/* OK: does not push any values */ \
	"1", \
	"	.locals (int32)", \
	"	ldc.i4.3", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_unary_4[] = { \
	/* OK: does not pop any values */ \
	"	ldc.i4.3", \
	"	" iname, \
	"	pop", \
	"	ret", \
	0 \
}; \
static char *name##_unary_5[] = { \
	/* FAIL: does not pop any values */ \
	"	ldc.i4.3", \
	"	" iname, \
	"	pop", \
	"	pop", \
	"	ret", \
	0 \
}
#define	declareUnaryFloatNature(name,iname)	\
static char *name##_unary_1[] = { \
	/* FAIL: needs 1 or more stack arguments */ \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_unary_2[] = { \
	/* OK: needs 1 or more stack arguments */ \
	"	ldc.r4	3.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_unary_3[] = { \
	/* OK: does not push any values */ \
	"1", \
	"	.locals (int32)", \
	"	ldc.r4	3.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_unary_4[] = { \
	/* OK: does not pop any values */ \
	"	ldc.r4	3.0", \
	"	" iname, \
	"	pop", \
	"	ret", \
	0 \
}; \
static char *name##_unary_5[] = { \
	/* FAIL: does not pop any values */ \
	"	ldc.r4	3.0", \
	"	" iname, \
	"	pop", \
	"	pop", \
	"	ret", \
	0 \
}
#define	testUnaryNature(name)	\
	do { \
		RegisterSimpleFail(name##_unary_1); \
		RegisterSimple(name##_unary_2); \
		RegisterSimple(name##_unary_3); \
		RegisterSimple(name##_unary_4); \
		RegisterSimpleFail(name##_unary_5); \
	} while (0)
#define	testUnaryFloatNature(name)	\
	do { \
		RegisterSimpleFail(name##_unary_1); \
		RegisterSimple(name##_unary_2); \
		RegisterSimple(name##_unary_3); \
		RegisterSimple(name##_unary_4); \
		RegisterSimpleFail(name##_unary_5); \
	} while (0)

/*
 * Test the safe arithmetic combinations for a binary operator.
 */
#define	declareBinaryOperands(name, iname) \
static char *name##_operands_1[] = { \
	/* OK: i4 and i4 */ \
	"	ldc.i4.3", \
	"	ldc.i4.5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_2[] = { \
	/* FAIL: i4 and i8 */ \
	"	ldc.i4.3", \
	"	ldc.i8	5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_3[] = { \
	/* OK: i4 and i */ \
	"	ldc.i4.3", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_4[] = { \
	/* FAIL: i8 and i4 */ \
	"	ldc.i8	5", \
	"	ldc.i4.3", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_5[] = { \
	/* OK: i8 and i8 */ \
	"	ldc.i8	3", \
	"	ldc.i8	5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_6[] = { \
	/* FAIL: i8 and i */ \
	"	ldc.i8	3", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_7[] = { \
	/* OK: i and i4 */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i4.5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_8[] = { \
	/* FAIL: i and i8 */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i8	5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_9[] = { \
	/* OK: i and i */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_10[] = { \
	/* FAIL: i4 and f */ \
	"	ldc.i4.3", \
	"	ldc.r4	5.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_11[] = { \
	/* FAIL: i8 and f */ \
	"	ldc.i8	3", \
	"	ldc.r4	5.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_12[] = { \
	/* FAIL: i and f */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.r4	5.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_13[] = { \
	/* FAIL: f and i4 */ \
	"	ldc.r4	5.0", \
	"	ldc.i4.3", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_14[] = { \
	/* FAIL: f and i8 */ \
	"	ldc.r4	5.0", \
	"	ldc.i8	3", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_15[] = { \
	/* FAIL: f and i */ \
	"	ldc.r4	5.0", \
	"	ldc.i4.3", \
	"	conv.i", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_16[] = { \
	/* OK: f and f */ \
	"	ldc.r4	3.0", \
	"	ldc.r4	5.0", \
	"	" iname, \
	"	ret", \
	0, \
}
#define	testBinaryOperands(name)	\
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
#define	testBinaryIntegerOperands(name)	\
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
		RegisterSimpleFail(name##_operands_16); \
	} while (0)
#define	testBinaryShiftOperands(name)	\
	do { \
		RegisterSimple(name##_operands_1); \
		RegisterSimpleFail(name##_operands_2); \
		RegisterSimple(name##_operands_3); \
		RegisterSimple(name##_operands_4); \
		RegisterSimpleFail(name##_operands_5); \
		RegisterSimple(name##_operands_6); \
		RegisterSimple(name##_operands_7); \
		RegisterSimpleFail(name##_operands_8); \
		RegisterSimple(name##_operands_9); \
		RegisterSimpleFail(name##_operands_10); \
		RegisterSimpleFail(name##_operands_11); \
		RegisterSimpleFail(name##_operands_12); \
		RegisterSimpleFail(name##_operands_13); \
		RegisterSimpleFail(name##_operands_14); \
		RegisterSimpleFail(name##_operands_15); \
		RegisterSimpleFail(name##_operands_16); \
	} while (0)

/*
 * Test the pointer combinations for a binary operator.
 */
#define	declarePointerOperands(name, iname) \
static char *name##_ptr_1[] = { \
	/* OK: m and i4 */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldc.i4.5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_2[] = { \
	/* FAIL: m and i8 */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldc.i8	5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_3[] = { \
	/* OK: m and i */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_4[] = { \
	/* FAIL: m and f */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldc.r4	5.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_5[] = { \
	/* FAIL: m and o */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldnull", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_6[] = { \
	/* FAIL for add and none, OK for sub: m and m */ \
	"	.locals (int32, int32)", \
	"	ldloca	0", \
	"	ldloca	1", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_7[] = { \
	/* FAIL: o and i4 */ \
	"	ldnull", \
	"	ldc.i4.5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_8[] = { \
	/* FAIL: o and i8 */ \
	"	ldnull", \
	"	ldc.i8	5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_9[] = { \
	/* FAIL: o and i */ \
	"	ldnull", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_10[] = { \
	/* FAIL: o and f */ \
	"	ldnull", \
	"	ldc.r4	5.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_11[] = { \
	/* FAIL: o and m */ \
	"	.locals (int32)", \
	"	ldnull", \
	"	ldloca	0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_12[] = { \
	/* FAIL: o and o */ \
	"	ldnull", \
	"	ldnull", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_13[] = { \
	/* OK: i4 and m */ \
	"	.locals (int32)", \
	"	ldc.i4.5", \
	"	ldloca	0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_14[] = { \
	/* FAIL: i8 and m */ \
	"	.locals (int32)", \
	"	ldc.i8	5", \
	"	ldloca	0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_15[] = { \
	/* OK: i and m */ \
	"	.locals (int32)", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	ldloca	0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_16[] = { \
	/* FAIL: f and m */ \
	"	.locals (int32)", \
	"	ldc.r4	5.0", \
	"	ldloca	0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_17[] = { \
	/* FAIL: i4 and o */ \
	"	ldc.i4.5", \
	"	ldnull", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_18[] = { \
	/* FAIL: i8 and o */ \
	"	ldc.i8	5", \
	"	ldnull", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_19[] = { \
	/* FAIL: i and o */ \
	"	ldc.i4.5", \
	"	conv.i", \
	"	ldnull", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_20[] = { \
	/* FAIL: f and o */ \
	"	ldc.r4	5.0", \
	"	ldnull", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_21[] = { \
	/* FAIL: mv and i4 */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldc.i4.5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_22[] = { \
	/* FAIL: mv and i8 */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldc.i8	5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_23[] = { \
	/* FAIL: mv and i */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_24[] = { \
	/* FAIL: mv and f */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldc.r4	5.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_25[] = { \
	/* FAIL: mv and m */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	.locals (int32)", \
	"	ldarg.0", \
	"	ldloca	0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_26[] = { \
	/* FAIL: mv and o */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldnull", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_27[] = { \
	/* FAIL: mv and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	ldarg.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_28[] = { \
	/* FAIL: i4 and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldc.i4.3", \
	"	ldarg.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_29[] = { \
	/* FAIL: i8 and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldc.i8	3", \
	"	ldarg.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_30[] = { \
	/* FAIL: i and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldarg.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_31[] = { \
	/* FAIL: f and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldc.r4	3.0", \
	"	ldarg.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_32[] = { \
	/* FAIL: m and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldarg.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_ptr_33[] = { \
	/* FAIL: o and mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldnull", \
	"	ldarg.0", \
	"	" iname, \
	"	ret", \
	0, \
}
#define	testPointerOperandsAdd(name) \
	do { \
		RegisterSimpleUnsafe(name##_ptr_1); \
		RegisterSimpleFail(name##_ptr_2); \
		RegisterSimpleUnsafe(name##_ptr_3); \
		RegisterSimpleFail(name##_ptr_4); \
		RegisterSimpleFail(name##_ptr_5); \
		RegisterSimpleFail(name##_ptr_6); \
		RegisterSimpleFail(name##_ptr_7); \
		RegisterSimpleFail(name##_ptr_8); \
		RegisterSimpleFail(name##_ptr_9); \
		RegisterSimpleFail(name##_ptr_10); \
		RegisterSimpleFail(name##_ptr_11); \
		RegisterSimpleFail(name##_ptr_12); \
		RegisterSimpleUnsafe(name##_ptr_13); \
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
#define	testPointerOperandsSub(name) \
	do { \
		RegisterSimpleUnsafe(name##_ptr_1); \
		RegisterSimpleFail(name##_ptr_2); \
		RegisterSimpleUnsafe(name##_ptr_3); \
		RegisterSimpleFail(name##_ptr_4); \
		RegisterSimpleFail(name##_ptr_5); \
		RegisterSimpleUnsafe(name##_ptr_6); \
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
#define	testPointerOperandsNone(name) \
	do { \
		RegisterSimpleFail(name##_ptr_1); \
		RegisterSimpleFail(name##_ptr_2); \
		RegisterSimpleFail(name##_ptr_3); \
		RegisterSimpleFail(name##_ptr_4); \
		RegisterSimpleFail(name##_ptr_5); \
		RegisterSimpleFail(name##_ptr_6); \
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
#define	testPointerOperandsCompare(name) \
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
#define	testPointerOperandsCompareEq(name) \
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
#define	testPointerOperandsCompareGtUn(name) \
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
 * Test the arithmetic combinations for a unary operator.
 */
#define	declareUnaryOperands(name, iname) \
static char *name##_operands_1[] = { \
	/* Unary i4 */ \
	"	ldc.i4.5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_2[] = { \
	/* Unary i8 */ \
	"	ldc.i8	5", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_3[] = { \
	/* Unary i */ \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_4[] = { \
	/* Unary f */ \
	"	ldc.r4	5.0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_5[] = { \
	/* Unary m */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_6[] = { \
	/* Unary o */ \
	"	ldnull", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static char *name##_operands_7[] = { \
	/* Unary mv */ \
	"public static void test(valuetype [mscorlib] System.Decimal x)", \
	"	ldarg.0", \
	"	" iname, \
	"	ret", \
	0, \
}
#define	testUnaryNumericOperands(name)	\
	do { \
		RegisterSimple(name##_operands_1); \
		RegisterSimple(name##_operands_2); \
		RegisterSimple(name##_operands_3); \
		RegisterSimple(name##_operands_4); \
		RegisterSimpleFail(name##_operands_5); \
		RegisterSimpleFail(name##_operands_6); \
		RegisterSimpleFail(name##_operands_7); \
	} while (0)
#define	testUnaryConvOperands(name)	\
	do { \
		RegisterSimple(name##_operands_1); \
		RegisterSimple(name##_operands_2); \
		RegisterSimple(name##_operands_3); \
		RegisterSimple(name##_operands_4); \
		RegisterSimpleUnsafe(name##_operands_5); \
		RegisterSimpleFail(name##_operands_6); \
		RegisterSimpleFail(name##_operands_7); \
	} while (0)
#define	testUnaryIntegerOperands(name)	\
	do { \
		RegisterSimple(name##_operands_1); \
		RegisterSimple(name##_operands_2); \
		RegisterSimple(name##_operands_3); \
		RegisterSimpleFail(name##_operands_4); \
		RegisterSimpleFail(name##_operands_5); \
		RegisterSimpleFail(name##_operands_6); \
		RegisterSimpleFail(name##_operands_7); \
	} while (0)
#define	testUnaryFloatOperands(name)	\
	do { \
		RegisterSimpleFail(name##_operands_1); \
		RegisterSimpleFail(name##_operands_2); \
		RegisterSimpleFail(name##_operands_3); \
		RegisterSimple(name##_operands_4); \
		RegisterSimpleFail(name##_operands_5); \
		RegisterSimpleFail(name##_operands_6); \
		RegisterSimpleFail(name##_operands_7); \
	} while (0)

/*
 * Instructions that can be used to verify the type
 * of a value on the stack.
 */
#define	VERIFY_TYPE_BOOL	"	box [mscorlib] System.Boolean"
#define	VERIFY_TYPE_I4		"	box [mscorlib] System.Int32"
#define	VERIFY_TYPE_I8		"	box [mscorlib] System.Int64"
#define	VERIFY_TYPE_I		"	box [mscorlib] System.IntPtr"
#define	VERIFY_TYPE_F		"	box [mscorlib] System.Double"
#define	VERIFY_TYPE_M		"	ldind.i4"
#define	VERIFY_TYPE_O		"	isinst [mscorlib] System.Object"
#define	VERIFY_TYPE_MV		"	box [mscorlib] System.Decimal"
#define	VERIFY_TYPE_STR		\
			"	callvirt instance int32 [mscorlib]System.String::get_Length()"

#ifdef	__cplusplus
};
#endif

#endif	/* _TEST_VERIFY_H */
