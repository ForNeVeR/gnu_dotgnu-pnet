/*
 * test_verify.c - Test the bytecode verifier in the runtime engine.
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
static void buildSimpleStream(void *arg)
{
	char **insns;

	/* Create the assembly stream and turn it into an image */
	insns = (char **)arg;
	ILUnitAsmCreate();
	if(insns[0][0] != '\t' && insns[0][0] != ' ')
	{
		if(insns[0][0] >= '0' && insns[0][0] <= '9')
		{
			ILUnitAsmMethod("public static void test()", atoi(insns[0]));
			++insns;
		}
		else if(insns[1][0] >= '0' && insns[1][0] <= '9')
		{
			ILUnitAsmMethod(insns[0], atoi(insns[1]));
			insns += 2;
		}
		else
		{
			ILUnitAsmMethod(insns[0], 8);
			++insns;
		}
	}
	else
	{
		ILUnitAsmMethod("public static void test()", 8);
	}
	while(*insns != 0)
	{
		ILUnitAsmWrite(*insns);
		ILUnitAsmWrite("\n");
		++insns;
	}
	ILUnitAsmEndMethod();
	ILUnitAsmClose();
}

/*
 * Verify all methods within the assembly image.  Returns zero on failure.
 */
static int verify(int allowUnsafe)
{
	ILImage *image = ILUnitAsmImage();
	ILMethod *method;
	ILMethodCode code;
	int result;
	unsigned char *start;

	/* Scan the entire MethodDef table and verify everything we find */
	method = 0;
	while((method = (ILMethod *)ILImageNextToken
				(image, IL_META_TOKEN_METHOD_DEF, (void *)method)) != 0)
	{
		/* Skip this method if it does not have IL bytecode */
		if(!ILMethod_RVA(method))
		{
			continue;
		}

		/* Get the IL bytecode for the method */
		if(!ILMethodGetCode(method, &code))
		{
			ILUnitFailed("malformed code");
		}

		/* Verify the method */
		result = _ILVerify(&_ILNullCoder, &start, method, &code, allowUnsafe);
		if(!result)
		{
			return 0;
		}
	}
	return 1;
}

/*
 * Verify all methods within the assembly image, with the expectation
 * that they must all fail.  Returns zero if something succeeds.
 */
static int verifyAllFail(int allowUnsafe)
{
	ILImage *image = ILUnitAsmImage();
	ILMethod *method;
	ILMethodCode code;
	int result;
	unsigned char *start;

	/* Scan the entire MethodDef table and verify everything we find */
	method = 0;
	while((method = (ILMethod *)ILImageNextToken
				(image, IL_META_TOKEN_METHOD_DEF, (void *)method)) != 0)
	{
		/* Skip this method if it does not have IL bytecode */
		if(!ILMethod_RVA(method))
		{
			continue;
		}

		/* Get the IL bytecode for the method */
		if(!ILMethodGetCode(method, &code))
		{
			ILUnitFailed("malformed code");
		}

		/* Verify the method */
		result = _ILVerify(&_ILNullCoder, &start, method, &code, allowUnsafe);
		if(result)
		{
			return 0;
		}
	}
	return 1;
}

/*
 * Test a simple method defined by a list of instructions.
 */
static void testSimple(void *arg)
{
	/* Build the stream */
	buildSimpleStream(arg);

	/* Verify that the image is ok in unsafe mode */
	if(!verify(1))
	{
		ILUnitFailed("unsafe mode did not verify when it should have");
	}

	/* Verify that the image is ok in safe mode */
	if(!verify(0))
	{
		ILUnitFailed("safe mode did not verify when it should have");
	}
}

/*
 * Test a simple method for verify failure defined by a list of instructions.
 */
static void testSimpleFail(void *arg)
{
	/* Build the stream */
	buildSimpleStream(arg);

	/* Verify that the image is not ok in unsafe mode */
	if(verify(1))
	{
		ILUnitFailed("unsafe mode verified when it should not have");
	}

	/* Verify that the image is not ok in safe mode */
	if(verify(0))
	{
		ILUnitFailed("safe mode verified when it should not have");
	}
}

/*
 * Test a simple method defined by a list of instructions that is unsafe.
 */
static void testSimpleUnsafe(void *arg)
{
	/* Build the stream */
	buildSimpleStream(arg);

	/* Verify that the image is ok in unsafe mode */
	if(!verify(1))
	{
		ILUnitFailed("unsafe mode did not verify when it should have");
	}

	/* Verify that the image is not ok in safe mode */
	if(verify(0))
	{
		ILUnitFailed("safe mode verified when it should not have");
	}
}

/*
 * Register a simple test.
 */
#define	RegisterSimple(name)	   ILUnitRegister(#name, testSimple, name)
#define	RegisterSimpleFail(name)   ILUnitRegister(#name, testSimpleFail, name)
#define	RegisterSimpleUnsafe(name) ILUnitRegister(#name, testSimpleUnsafe, name)

/*
 * Test "nop".
 */
static char *nop_1[] = {
	/* OK: "nop" does not push any values */
	"0",
	"	.locals (int32)",
	"	nop",
	"	ret",
	0
};
static char *nop_2[] = {
	/* OK: "nop" still works when stack size is non-zero */
	"	nop",
	"	ret",
	0
};
static char *nop_3[] = {
	/* OK: "nop" does not pop any values */
	"	ldc.i4.0",
	"	nop",
	"	pop",
	"	ret",
	0
};

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
	"	ldc.i4.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_unary_3[] = { \
	/* OK: does not push any values */ \
	"1", \
	"	.locals (int32)", \
	"	ldc.i4.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_unary_4[] = { \
	/* OK: does not pop any values */ \
	"	ldc.i4.0", \
	"	" iname, \
	"	pop", \
	"	ret", \
	0 \
}; \
static char *name##_unary_5[] = { \
	/* FAIL: does not pop any values */ \
	"	ldc.i4.0", \
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
}
#define	testUnaryNumericOperands(name)	\
	do { \
		RegisterSimple(name##_operands_1); \
		RegisterSimple(name##_operands_2); \
		RegisterSimple(name##_operands_3); \
		RegisterSimple(name##_operands_4); \
		RegisterSimpleFail(name##_operands_5); \
		RegisterSimpleFail(name##_operands_6); \
	} while (0)
#define	testUnaryIntegerOperands(name)	\
	do { \
		RegisterSimple(name##_operands_1); \
		RegisterSimple(name##_operands_2); \
		RegisterSimple(name##_operands_3); \
		RegisterSimpleFail(name##_operands_4); \
		RegisterSimpleFail(name##_operands_5); \
		RegisterSimpleFail(name##_operands_6); \
	} while (0)

/*
 * Declare test blocks for the arithmetic instructions.
 */
declareBinaryNature(add, "add");
declareBinaryOperands(add, "add");
declarePointerOperands(add, "add");

declareBinaryNature(add_ovf, "add.ovf");
declareBinaryOperands(add_ovf, "add.ovf");
declarePointerOperands(add_ovf, "add.ovf");

declareBinaryNature(add_ovf_un, "add.ovf.un");
declareBinaryOperands(add_ovf_un, "add.ovf.un");
declarePointerOperands(add_ovf_un, "add.ovf.un");

declareBinaryNature(sub, "sub");
declareBinaryOperands(sub, "sub");
declarePointerOperands(sub, "sub");

declareBinaryNature(sub_ovf, "sub.ovf");
declareBinaryOperands(sub_ovf, "sub.ovf");
declarePointerOperands(sub_ovf, "sub.ovf");

declareBinaryNature(sub_ovf_un, "sub.ovf.un");
declareBinaryOperands(sub_ovf_un, "sub.ovf.un");
declarePointerOperands(sub_ovf_un, "sub.ovf.un");

declareBinaryNature(mul, "mul");
declareBinaryOperands(mul, "mul");
declarePointerOperands(mul, "mul");

declareBinaryNature(mul_ovf, "mul.ovf");
declareBinaryOperands(mul_ovf, "mul.ovf");
declarePointerOperands(mul_ovf, "mul.ovf");

declareBinaryNature(mul_ovf_un, "mul.ovf.un");
declareBinaryOperands(mul_ovf_un, "mul.ovf.un");
declarePointerOperands(mul_ovf_un, "mul.ovf.un");

declareBinaryNature(div, "div");
declareBinaryOperands(div, "div");
declarePointerOperands(div, "div");

declareBinaryNature(div_un, "div.un");
declareBinaryOperands(div_un, "div.un");
declarePointerOperands(div_un, "div.un");

declareBinaryNature(rem, "rem");
declareBinaryOperands(rem, "rem");
declarePointerOperands(rem, "rem");

declareBinaryNature(rem_un, "rem.un");
declareBinaryOperands(rem_un, "rem.un");
declarePointerOperands(rem_un, "rem.un");

declareBinaryNature(and, "and");
declareBinaryOperands(and, "and");
declarePointerOperands(and, "and");

declareBinaryNature(or, "or");
declareBinaryOperands(or, "or");
declarePointerOperands(or, "or");

declareBinaryNature(xor, "xor");
declareBinaryOperands(xor, "xor");
declarePointerOperands(xor, "xor");

declareBinaryNature(shl, "shl");
declareBinaryOperands(shl, "shl");
declarePointerOperands(shl, "shl");

declareBinaryNature(shr, "shr");
declareBinaryOperands(shr, "shr");
declarePointerOperands(shr, "shr");

declareBinaryNature(shr_un, "shr.un");
declareBinaryOperands(shr_un, "shr.un");
declarePointerOperands(shr_un, "shr.un");

declareUnaryNature(neg, "neg");
declareUnaryOperands(neg, "neg");

declareUnaryNature(not, "not");
declareUnaryOperands(not, "not");

/*
 * Declare test blocks for the conversion instructions.
 */
declareUnaryNature(conv_i1, "conv.i1");
declareUnaryOperands(conv_i1, "conv.i1");

declareUnaryNature(conv_i2, "conv.i2");
declareUnaryOperands(conv_i2, "conv.i2");

declareUnaryNature(conv_i4, "conv.i4");
declareUnaryOperands(conv_i4, "conv.i4");

declareUnaryNature(conv_i8, "conv.i8");
declareUnaryOperands(conv_i8, "conv.i8");

declareUnaryNature(conv_r4, "conv.r4");
declareUnaryOperands(conv_r4, "conv.r4");

declareUnaryNature(conv_r8, "conv.r8");
declareUnaryOperands(conv_r8, "conv.r8");

declareUnaryNature(conv_u4, "conv.u4");
declareUnaryOperands(conv_u4, "conv.u4");

declareUnaryNature(conv_u8, "conv.u8");
declareUnaryOperands(conv_u8, "conv.u8");

declareUnaryNature(conv_r_un, "conv.r.un");
declareUnaryOperands(conv_r_un, "conv.r.un");

declareUnaryNature(conv_ovf_i1_un, "conv.ovf.i1.un");
declareUnaryOperands(conv_ovf_i1_un, "conv.ovf.i1.un");

declareUnaryNature(conv_ovf_i2_un, "conv.ovf.i2.un");
declareUnaryOperands(conv_ovf_i2_un, "conv.ovf.i2.un");

declareUnaryNature(conv_ovf_i4_un, "conv.ovf.i4.un");
declareUnaryOperands(conv_ovf_i4_un, "conv.ovf.i4.un");

declareUnaryNature(conv_ovf_i8_un, "conv.ovf.i8.un");
declareUnaryOperands(conv_ovf_i8_un, "conv.ovf.i8.un");

declareUnaryNature(conv_ovf_u1_un, "conv.ovf.u1.un");
declareUnaryOperands(conv_ovf_u1_un, "conv.ovf.u1.un");

declareUnaryNature(conv_ovf_u2_un, "conv.ovf.u2.un");
declareUnaryOperands(conv_ovf_u2_un, "conv.ovf.u2.un");

declareUnaryNature(conv_ovf_u4_un, "conv.ovf.u4.un");
declareUnaryOperands(conv_ovf_u4_un, "conv.ovf.u4.un");

declareUnaryNature(conv_ovf_u8_un, "conv.ovf.u8.un");
declareUnaryOperands(conv_ovf_u8_un, "conv.ovf.u8.un");

declareUnaryNature(conv_ovf_i_un, "conv.ovf.i.un");
declareUnaryOperands(conv_ovf_i_un, "conv.ovf.i.un");

declareUnaryNature(conv_ovf_u_un, "conv.ovf.u.un");
declareUnaryOperands(conv_ovf_u_un, "conv.ovf.u.un");

declareUnaryNature(conv_ovf_i1, "conv.ovf.i1");
declareUnaryOperands(conv_ovf_i1, "conv.ovf.i1");

declareUnaryNature(conv_ovf_u1, "conv.ovf.u1");
declareUnaryOperands(conv_ovf_u1, "conv.ovf.u1");

declareUnaryNature(conv_ovf_i2, "conv.ovf.i2");
declareUnaryOperands(conv_ovf_i2, "conv.ovf.i2");

declareUnaryNature(conv_ovf_u2, "conv.ovf.u2");
declareUnaryOperands(conv_ovf_u2, "conv.ovf.u2");

declareUnaryNature(conv_ovf_i4, "conv.ovf.i4");
declareUnaryOperands(conv_ovf_i4, "conv.ovf.i4");

declareUnaryNature(conv_ovf_u4, "conv.ovf.u4");
declareUnaryOperands(conv_ovf_u4, "conv.ovf.u4");

declareUnaryNature(conv_ovf_i8, "conv.ovf.i8");
declareUnaryOperands(conv_ovf_i8, "conv.ovf.i8");

declareUnaryNature(conv_ovf_u8, "conv.ovf.u8");
declareUnaryOperands(conv_ovf_u8, "conv.ovf.u8");

declareUnaryNature(conv_u2, "conv.u2");
declareUnaryOperands(conv_u2, "conv.u2");

declareUnaryNature(conv_u1, "conv.u1");
declareUnaryOperands(conv_u1, "conv.u1");

declareUnaryNature(conv_i, "conv.i");
declareUnaryOperands(conv_i, "conv.i");

declareUnaryNature(conv_ovf_i, "conv.ovf.i");
declareUnaryOperands(conv_ovf_i, "conv.ovf.i");

declareUnaryNature(conv_ovf_u, "conv.ovf.u");
declareUnaryOperands(conv_ovf_u, "conv.ovf.u");

declareUnaryNature(conv_u, "conv.u");
declareUnaryOperands(conv_u, "conv.u");

/*
 * Declare test blocks for comparison instructions.
 */
declareBinaryNature(ceq, "ceq");
declareBinaryOperands(ceq, "ceq");
declarePointerOperands(ceq, "ceq");

declareBinaryNature(cgt, "cgt");
declareBinaryOperands(cgt, "cgt");
declarePointerOperands(cgt, "cgt");

declareBinaryNature(cgt_un, "cgt.un");
declareBinaryOperands(cgt_un, "cgt.un");
declarePointerOperands(cgt_un, "cgt.un");

declareBinaryNature(clt, "clt");
declareBinaryOperands(clt, "clt");
declarePointerOperands(clt, "clt");

declareBinaryNature(clt_un, "clt.un");
declareBinaryOperands(clt_un, "clt.un");
declarePointerOperands(clt_un, "clt.un");


/*
 * Declare the illegal instructions.
 */
static int illegalInsns[] = {
	0x01, 0x24, 0x78, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa,
	0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xbb, 0xbc, 0xbd,
	0xbe, 0xbf, 0xc0, 0xc1, 0xc4, 0xc5, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5,
	0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb,
	0xfc, 0xfd, 0xff, -1
};
static int illegalPrefixInsns[] = {
	-2, 0x10, 0x1b, 0x1e,
	0x1f, 0x20, 0x21, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
	0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
	-1
};

/*
 * Test a batch of illegal instructions.
 */
static void testIllegal(void *arg)
{
	int *insns = (int *)arg;
	int usePrefix;

	/* Create the assembly stream, with one method for
	   each illegal instruction that we will be testing */
	if(*insns == -2)
	{
		usePrefix = 1;
		++insns;
	}
	else
	{
		usePrefix = 0;
	}
	ILUnitAsmCreate();
	while(*insns != -1)
	{
		ILUnitAsmPrintF(".method public static void test%02x() il managed\n",
						*insns);
		ILUnitAsmWrite("{\n\t.maxstack 8\n");
		if(usePrefix)
		{
			ILUnitAsmWrite("\t.emitbyte 0xfe\n");
		}
		ILUnitAsmPrintF("\t.emitbyte 0x%02x\n", *insns);
		ILUnitAsmWrite("\tret\n}\n");
		++insns;
	}
	ILUnitAsmClose();

	/* Verify that the image is not ok in unsafe mode */
	if(!verifyAllFail(1))
	{
		ILUnitFailed("unsafe mode verified when it should not have");
	}

	/* Verify that the image is not ok in safe mode */
	if(!verifyAllFail(0))
	{
		ILUnitFailed("safe mode verified when it should not have");
	}
}

/*
 * Register all unit tests.
 */
void ILUnitRegisterTests(void)
{
	/*
	 * Simple instruction tests.
	 */
	ILUnitRegisterSuite("Simple Instructions");
	RegisterSimple(nop_1);
	RegisterSimple(nop_2);
	RegisterSimple(nop_3);

	/*
	 * Arithmetic instruction tests.
	 */
	ILUnitRegisterSuite("Arithmetic Instructions");
	testBinaryNature(add);
	testBinaryOperands(add);
	testPointerOperandsAdd(add);

	testBinaryNature(add_ovf);
	testBinaryIntegerOperands(add_ovf);
	testPointerOperandsNone(add_ovf);

	testBinaryNature(add_ovf_un);
	testBinaryIntegerOperands(add_ovf_un);
	testPointerOperandsAdd(add_ovf_un);

	testBinaryNature(sub);
	testBinaryOperands(sub);
	testPointerOperandsSub(sub);

	testBinaryNature(sub_ovf);
	testBinaryIntegerOperands(sub_ovf);
	testPointerOperandsNone(sub_ovf);

	testBinaryNature(sub_ovf_un);
	testBinaryIntegerOperands(sub_ovf_un);
	testPointerOperandsSub(sub_ovf_un);

	testBinaryNature(mul);
	testBinaryOperands(mul);
	testPointerOperandsNone(mul);

	testBinaryNature(mul_ovf);
	testBinaryIntegerOperands(mul_ovf);
	testPointerOperandsNone(mul_ovf);

	testBinaryNature(mul_ovf_un);
	testBinaryIntegerOperands(mul_ovf_un);
	testPointerOperandsNone(mul_ovf_un);

	testBinaryNature(div);
	testBinaryOperands(div);
	testPointerOperandsNone(div);

	testBinaryNature(div_un);
	testBinaryIntegerOperands(div_un);
	testPointerOperandsNone(div_un);

	testBinaryNature(rem);
	testBinaryOperands(rem);
	testPointerOperandsNone(rem);

	testBinaryNature(rem_un);
	testBinaryIntegerOperands(rem_un);
	testPointerOperandsNone(rem_un);

	testBinaryNature(and);
	testBinaryIntegerOperands(and);
	testPointerOperandsNone(and);

	testBinaryNature(or);
	testBinaryIntegerOperands(or);
	testPointerOperandsNone(or);

	testBinaryNature(xor);
	testBinaryIntegerOperands(xor);
	testPointerOperandsNone(xor);

	testBinaryNature(shl);
	testBinaryShiftOperands(shl);
	testPointerOperandsNone(shl);

	testBinaryNature(shr);
	testBinaryShiftOperands(shr);
	testPointerOperandsNone(shr);

	testBinaryNature(shr_un);
	testBinaryShiftOperands(shr_un);
	testPointerOperandsNone(shr_un);

	testUnaryNature(neg);
	testUnaryNumericOperands(neg);

	testUnaryNature(not);
	testUnaryIntegerOperands(not);

	/*
	 * Conversion instruction tests.
	 */
	ILUnitRegisterSuite("Conversion Instructions");
	testUnaryNature(conv_i1);
	testUnaryNumericOperands(conv_i1);

	testUnaryNature(conv_i2);
	testUnaryNumericOperands(conv_i2);

	testUnaryNature(conv_i4);
	testUnaryNumericOperands(conv_i4);

	testUnaryNature(conv_i8);
	testUnaryNumericOperands(conv_i8);

	testUnaryNature(conv_r4);
	testUnaryNumericOperands(conv_r4);

	testUnaryNature(conv_r8);
	testUnaryNumericOperands(conv_r8);

	testUnaryNature(conv_u4);
	testUnaryNumericOperands(conv_u4);

	testUnaryNature(conv_u8);
	testUnaryNumericOperands(conv_u8);

	testUnaryNature(conv_r_un);
	testUnaryNumericOperands(conv_r_un);

	testUnaryNature(conv_ovf_i1_un);
	testUnaryNumericOperands(conv_ovf_i1_un);

	testUnaryNature(conv_ovf_i2_un);
	testUnaryNumericOperands(conv_ovf_i2_un);

	testUnaryNature(conv_ovf_i4_un);
	testUnaryNumericOperands(conv_ovf_i4_un);

	testUnaryNature(conv_ovf_i8_un);
	testUnaryNumericOperands(conv_ovf_i8_un);

	testUnaryNature(conv_ovf_u1_un);
	testUnaryNumericOperands(conv_ovf_u1_un);

	testUnaryNature(conv_ovf_u2_un);
	testUnaryNumericOperands(conv_ovf_u2_un);

	testUnaryNature(conv_ovf_u4_un);
	testUnaryNumericOperands(conv_ovf_u4_un);

	testUnaryNature(conv_ovf_u8_un);
	testUnaryNumericOperands(conv_ovf_u8_un);

	testUnaryNature(conv_ovf_i_un);
	testUnaryNumericOperands(conv_ovf_i_un);

	testUnaryNature(conv_ovf_u_un);
	testUnaryNumericOperands(conv_ovf_u_un);

	testUnaryNature(conv_ovf_i1);
	testUnaryNumericOperands(conv_ovf_i1);

	testUnaryNature(conv_ovf_u1);
	testUnaryNumericOperands(conv_ovf_u1);

	testUnaryNature(conv_ovf_i2);
	testUnaryNumericOperands(conv_ovf_i2);

	testUnaryNature(conv_ovf_u2);
	testUnaryNumericOperands(conv_ovf_u2);

	testUnaryNature(conv_ovf_i4);
	testUnaryNumericOperands(conv_ovf_i4);

	testUnaryNature(conv_ovf_u4);
	testUnaryNumericOperands(conv_ovf_u4);

	testUnaryNature(conv_ovf_i8);
	testUnaryNumericOperands(conv_ovf_i8);

	testUnaryNature(conv_ovf_u8);
	testUnaryNumericOperands(conv_ovf_u8);

	testUnaryNature(conv_u2);
	testUnaryNumericOperands(conv_u2);

	testUnaryNature(conv_u1);
	testUnaryNumericOperands(conv_u1);

	testUnaryNature(conv_i);
	testUnaryNumericOperands(conv_i);

	testUnaryNature(conv_ovf_i);
	testUnaryNumericOperands(conv_ovf_i);

	testUnaryNature(conv_ovf_u);
	testUnaryNumericOperands(conv_ovf_u);

	testUnaryNature(conv_u);
	testUnaryNumericOperands(conv_u);

	/*
	 * Comparison instruction tests.
	 */
	ILUnitRegisterSuite("Comparison Instructions");
	testBinaryNature(ceq);
	testBinaryOperands(ceq);
	testPointerOperandsCompareEq(ceq);

	testBinaryNature(cgt);
	testBinaryOperands(cgt);
	testPointerOperandsCompare(cgt);

	testBinaryNature(cgt_un);
	testBinaryOperands(cgt_un);
	testPointerOperandsCompareGtUn(cgt_un);

	testBinaryNature(clt);
	testBinaryOperands(clt);
	testPointerOperandsCompare(clt);

	testBinaryNature(clt_un);
	testBinaryOperands(clt_un);
	testPointerOperandsCompare(clt_un);

	/*
	 * Illegal instruction tests.
	 */
	ILUnitRegisterSuite("Illegal Instructions");
	ILUnitRegister("illegal", testIllegal, illegalInsns);
	ILUnitRegister("illegal_prefix", testIllegal, illegalPrefixInsns);
}

#ifdef	__cplusplus
};
#endif
