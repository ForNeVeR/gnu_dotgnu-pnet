/*
 * test_varith.c - Test the bytecode verifier on arithmetic instructions
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
 * Test the result types for a binary operator.
 */
#define	declareBinaryResultType(name, iname) \
static char *name##_type_1[] = { \
	/* i4 and i4 */ \
	"	ldc.i4.3", \
	"	ldc.i4.5", \
	"	" iname, \
	VERIFY_TYPE_I4, \
	"	ret", \
	0, \
}; \
static char *name##_type_2[] = { \
	/* i4 and i */ \
	"	ldc.i4.3", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	VERIFY_TYPE_I, \
	"	ret", \
	0, \
}; \
static char *name##_type_3[] = { \
	/* i8 and i8 */ \
	"	ldc.i8	3", \
	"	ldc.i8	5", \
	"	" iname, \
	VERIFY_TYPE_I8, \
	"	ret", \
	0, \
}; \
static char *name##_type_4[] = { \
	/* i and i4 */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i4.5", \
	"	" iname, \
	VERIFY_TYPE_I, \
	"	ret", \
	0, \
}; \
static char *name##_type_5[] = { \
	/* i and i */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	VERIFY_TYPE_I, \
	"	ret", \
	0, \
}; \
static char *name##_type_6[] = { \
	/* f and f */ \
	"	ldc.r4	3.0", \
	"	ldc.r4	5.0", \
	"	" iname, \
	VERIFY_TYPE_F, \
	"	ret", \
	0, \
}
#define	testBinaryResultType(name)	\
	do { \
		RegisterSimple(name##_type_1); \
		RegisterSimple(name##_type_2); \
		RegisterSimple(name##_type_3); \
		RegisterSimple(name##_type_4); \
		RegisterSimple(name##_type_5); \
		RegisterSimple(name##_type_6); \
	} while (0)
#define	testBinaryIntegerResultType(name)	\
	do { \
		RegisterSimple(name##_type_1); \
		RegisterSimple(name##_type_2); \
		RegisterSimple(name##_type_3); \
		RegisterSimple(name##_type_4); \
		RegisterSimple(name##_type_5); \
		RegisterSimpleFail(name##_type_6); \
	} while (0)

/*
 * Test the result types for a binary shift operator.
 */
#define	declareBinaryShiftResultType(name, iname) \
static char *name##_type_1[] = { \
	/* i4 and i4 */ \
	"	ldc.i4.3", \
	"	ldc.i4.5", \
	"	" iname, \
	VERIFY_TYPE_I4, \
	"	ret", \
	0, \
}; \
static char *name##_type_2[] = { \
	/* i4 and i */ \
	"	ldc.i4.3", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	VERIFY_TYPE_I4, \
	"	ret", \
	0, \
}; \
static char *name##_type_3[] = { \
	/* i8 and i4 */ \
	"	ldc.i8	3", \
	"	ldc.i4.5", \
	"	" iname, \
	VERIFY_TYPE_I8, \
	"	ret", \
	0, \
}; \
static char *name##_type_4[] = { \
	/* i8 and i */ \
	"	ldc.i8	3", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	VERIFY_TYPE_I8, \
	"	ret", \
	0, \
}; \
static char *name##_type_5[] = { \
	/* i and i4 */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i4.5", \
	"	" iname, \
	VERIFY_TYPE_I, \
	"	ret", \
	0, \
}; \
static char *name##_type_6[] = { \
	/* i and i */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	VERIFY_TYPE_I, \
	"	ret", \
	0, \
}
#define	testBinaryShiftResultType(name)	\
	do { \
		RegisterSimple(name##_type_1); \
		RegisterSimple(name##_type_2); \
		RegisterSimple(name##_type_3); \
		RegisterSimple(name##_type_4); \
		RegisterSimple(name##_type_5); \
		RegisterSimple(name##_type_6); \
	} while (0)

/*
 * Declare test blocks for the arithmetic instructions.
 */
declareBinaryNature(add, "add");
declareBinaryOperands(add, "add");
declarePointerOperands(add, "add");
declareBinaryResultType(add, "add");

declareBinaryNature(add_ovf, "add.ovf");
declareBinaryOperands(add_ovf, "add.ovf");
declarePointerOperands(add_ovf, "add.ovf");
declareBinaryResultType(add_ovf, "add.ovf");

declareBinaryNature(add_ovf_un, "add.ovf.un");
declareBinaryOperands(add_ovf_un, "add.ovf.un");
declarePointerOperands(add_ovf_un, "add.ovf.un");
declareBinaryResultType(add_ovf_un, "add.ovf.un");

declareBinaryNature(sub, "sub");
declareBinaryOperands(sub, "sub");
declarePointerOperands(sub, "sub");
declareBinaryResultType(sub, "sub");

declareBinaryNature(sub_ovf, "sub.ovf");
declareBinaryOperands(sub_ovf, "sub.ovf");
declarePointerOperands(sub_ovf, "sub.ovf");
declareBinaryResultType(sub_ovf, "sub.ovf");

declareBinaryNature(sub_ovf_un, "sub.ovf.un");
declareBinaryOperands(sub_ovf_un, "sub.ovf.un");
declarePointerOperands(sub_ovf_un, "sub.ovf.un");
declareBinaryResultType(sub_ovf_un, "sub.ovf.un");

declareBinaryNature(mul, "mul");
declareBinaryOperands(mul, "mul");
declarePointerOperands(mul, "mul");
declareBinaryResultType(mul, "mul");

declareBinaryNature(mul_ovf, "mul.ovf");
declareBinaryOperands(mul_ovf, "mul.ovf");
declarePointerOperands(mul_ovf, "mul.ovf");
declareBinaryResultType(mul_ovf, "mul.ovf");

declareBinaryNature(mul_ovf_un, "mul.ovf.un");
declareBinaryOperands(mul_ovf_un, "mul.ovf.un");
declarePointerOperands(mul_ovf_un, "mul.ovf.un");
declareBinaryResultType(mul_ovf_un, "mul.ovf.un");

declareBinaryNature(div, "div");
declareBinaryOperands(div, "div");
declarePointerOperands(div, "div");
declareBinaryResultType(div, "div");

declareBinaryNature(div_un, "div.un");
declareBinaryOperands(div_un, "div.un");
declarePointerOperands(div_un, "div.un");
declareBinaryResultType(div_un, "div.un");

declareBinaryNature(rem, "rem");
declareBinaryOperands(rem, "rem");
declarePointerOperands(rem, "rem");
declareBinaryResultType(rem, "rem");

declareBinaryNature(rem_un, "rem.un");
declareBinaryOperands(rem_un, "rem.un");
declarePointerOperands(rem_un, "rem.un");
declareBinaryResultType(rem_un, "rem.un");

declareBinaryNature(and, "and");
declareBinaryOperands(and, "and");
declarePointerOperands(and, "and");
declareBinaryResultType(and, "and");

declareBinaryNature(or, "or");
declareBinaryOperands(or, "or");
declarePointerOperands(or, "or");
declareBinaryResultType(or, "or");

declareBinaryNature(xor, "xor");
declareBinaryOperands(xor, "xor");
declarePointerOperands(xor, "xor");
declareBinaryResultType(xor, "xor");

declareBinaryNature(shl, "shl");
declareBinaryOperands(shl, "shl");
declarePointerOperands(shl, "shl");
declareBinaryShiftResultType(shl, "shl");

declareBinaryNature(shr, "shr");
declareBinaryOperands(shr, "shr");
declarePointerOperands(shr, "shr");
declareBinaryShiftResultType(shr, "shr");

declareBinaryNature(shr_un, "shr.un");
declareBinaryOperands(shr_un, "shr.un");
declarePointerOperands(shr_un, "shr.un");
declareBinaryShiftResultType(shr_un, "shr.un");

declareUnaryNature(neg, "neg");
declareUnaryOperands(neg, "neg");

declareUnaryNature(not, "not");
declareUnaryOperands(not, "not");

declareUnaryFloatNature(ckfinite, "ckfinite");
declareUnaryOperands(ckfinite, "ckfinite");

/*
 * Declare test blocks for unary operator result types.
 */
static char *neg_type_1[] = {
	/* Unary i4 */
	"	ldc.i4.5",
	"	neg",
	VERIFY_TYPE_I4,
	"	ret",
	0,
};
static char *neg_type_2[] = {
	/* Unary i8 */
	"	ldc.i8	5",
	"	neg",
	VERIFY_TYPE_I8,
	"	ret",
	0,
};
static char *neg_type_3[] = {
	/* Unary i */
	"	ldc.i4.5",
	"	conv.i",
	"	neg",
	VERIFY_TYPE_I,
	"	ret",
	0,
};
static char *neg_type_4[] = {
	/* Unary f */
	"	ldc.r4	5.0",
	"	neg",
	VERIFY_TYPE_F,
	"	ret",
	0,
};
static char *not_type_1[] = {
	/* Unary i4 */
	"	ldc.i4.5",
	"	not",
	VERIFY_TYPE_I4,
	"	ret",
	0,
};
static char *not_type_2[] = {
	/* Unary i8 */
	"	ldc.i8	5",
	"	not",
	VERIFY_TYPE_I8,
	"	ret",
	0,
};
static char *not_type_3[] = {
	/* Unary i */
	"	ldc.i4.5",
	"	conv.i",
	"	not",
	VERIFY_TYPE_I,
	"	ret",
	0,
};
static char *ckfinite_type_1[] = {
	/* Unary f */
	"	ldc.r4	5.0",
	"	ckfinite",
	VERIFY_TYPE_F,
	"	ret",
	0,
};

/*
 * Declare extra test blocks for pointer type tests.
 */
static char *add_type_7[] = {
	"	.locals (int32)",
	"	ldloca	0",
	"	ldc.i4.5",
	"	add",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *add_type_8[] = {
	"	.locals (int32)",
	"	ldloca	0",
	"	ldc.i4.5",
	"	conv.i",
	"	add",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *add_type_9[] = {
	"	.locals (int32)",
	"	ldc.i4.5",
	"	ldloca	0",
	"	add",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *add_type_10[] = {
	"	.locals (int32)",
	"	ldc.i4.5",
	"	conv.i",
	"	ldloca	0",
	"	add",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *add_ovf_un_type_7[] = {
	"	.locals (int32)",
	"	ldloca	0",
	"	ldc.i4.5",
	"	add.ovf.un",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *add_ovf_un_type_8[] = {
	"	.locals (int32)",
	"	ldloca	0",
	"	ldc.i4.5",
	"	conv.i",
	"	add.ovf.un",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *add_ovf_un_type_9[] = {
	"	.locals (int32)",
	"	ldc.i4.5",
	"	ldloca	0",
	"	add.ovf.un",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *add_ovf_un_type_10[] = {
	"	.locals (int32)",
	"	ldc.i4.5",
	"	conv.i",
	"	ldloca	0",
	"	add.ovf.un",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *sub_type_7[] = {
	"	.locals (int32)",
	"	ldloca	0",
	"	ldc.i4.5",
	"	sub",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *sub_type_8[] = {
	"	.locals (int32)",
	"	ldloca	0",
	"	ldc.i4.5",
	"	conv.i",
	"	sub",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *sub_type_9[] = {
	"	.locals (int32)",
	"	ldloca	0",
	"	ldloca	0",
	"	sub",
	VERIFY_TYPE_I,
	"	ret",
	0
};
static char *sub_ovf_un_type_7[] = {
	"	.locals (int32)",
	"	ldloca	0",
	"	ldc.i4.5",
	"	sub.ovf.un",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *sub_ovf_un_type_8[] = {
	"	.locals (int32)",
	"	ldloca	0",
	"	ldc.i4.5",
	"	conv.i",
	"	sub.ovf.un",
	VERIFY_TYPE_M,
	"	ret",
	0
};
static char *sub_ovf_un_type_9[] = {
	"	.locals (int32)",
	"	ldloca	0",
	"	ldloca	0",
	"	sub.ovf.un",
	VERIFY_TYPE_I,
	"	ret",
	0
};

/*
 * Register the arithmetic tests.
 */
void registerArithTests(void)
{
	ILUnitRegisterSuite("Arithmetic Instructions");

	testBinaryNature(add);
	testBinaryOperands(add);
	testPointerOperandsAdd(add);
	testBinaryResultType(add);
	RegisterSimpleUnsafe(add_type_7);
	RegisterSimpleUnsafe(add_type_8);
	RegisterSimpleUnsafe(add_type_9);
	RegisterSimpleUnsafe(add_type_10);

	testBinaryNature(add_ovf);
	testBinaryIntegerOperands(add_ovf);
	testPointerOperandsNone(add_ovf);
	testBinaryIntegerResultType(add_ovf);

	testBinaryNature(add_ovf_un);
	testBinaryIntegerOperands(add_ovf_un);
	testPointerOperandsAdd(add_ovf_un);
	testBinaryIntegerResultType(add_ovf_un);
	RegisterSimpleUnsafe(add_ovf_un_type_7);
	RegisterSimpleUnsafe(add_ovf_un_type_8);
	RegisterSimpleUnsafe(add_ovf_un_type_9);
	RegisterSimpleUnsafe(add_ovf_un_type_10);

	testBinaryNature(sub);
	testBinaryOperands(sub);
	testPointerOperandsSub(sub);
	testBinaryResultType(sub);
	RegisterSimpleUnsafe(sub_type_7);
	RegisterSimpleUnsafe(sub_type_8);
	RegisterSimpleUnsafe(sub_type_9);

	testBinaryNature(sub_ovf);
	testBinaryIntegerOperands(sub_ovf);
	testPointerOperandsNone(sub_ovf);
	testBinaryIntegerResultType(sub_ovf);

	testBinaryNature(sub_ovf_un);
	testBinaryIntegerOperands(sub_ovf_un);
	testPointerOperandsSub(sub_ovf_un);
	testBinaryIntegerResultType(sub_ovf_un);
	RegisterSimpleUnsafe(sub_ovf_un_type_7);
	RegisterSimpleUnsafe(sub_ovf_un_type_8);
	RegisterSimpleUnsafe(sub_ovf_un_type_9);

	testBinaryNature(mul);
	testBinaryOperands(mul);
	testPointerOperandsNone(mul);
	testBinaryResultType(mul);

	testBinaryNature(mul_ovf);
	testBinaryIntegerOperands(mul_ovf);
	testPointerOperandsNone(mul_ovf);
	testBinaryIntegerResultType(mul_ovf);

	testBinaryNature(mul_ovf_un);
	testBinaryIntegerOperands(mul_ovf_un);
	testPointerOperandsNone(mul_ovf_un);
	testBinaryIntegerResultType(mul_ovf_un);

	testBinaryNature(div);
	testBinaryOperands(div);
	testPointerOperandsNone(div);
	testBinaryResultType(div);

	testBinaryNature(div_un);
	testBinaryIntegerOperands(div_un);
	testPointerOperandsNone(div_un);
	testBinaryIntegerResultType(div_un);

	testBinaryNature(rem);
	testBinaryOperands(rem);
	testPointerOperandsNone(rem);
	testBinaryResultType(rem);

	testBinaryNature(rem_un);
	testBinaryIntegerOperands(rem_un);
	testPointerOperandsNone(rem_un);
	testBinaryIntegerResultType(rem_un);

	testBinaryNature(and);
	testBinaryIntegerOperands(and);
	testPointerOperandsNone(and);
	testBinaryIntegerResultType(and);

	testBinaryNature(or);
	testBinaryIntegerOperands(or);
	testPointerOperandsNone(or);
	testBinaryIntegerResultType(or);

	testBinaryNature(xor);
	testBinaryIntegerOperands(xor);
	testPointerOperandsNone(xor);
	testBinaryIntegerResultType(xor);

	testBinaryNature(shl);
	testBinaryShiftOperands(shl);
	testPointerOperandsNone(shl);
	testBinaryShiftResultType(shl);

	testBinaryNature(shr);
	testBinaryShiftOperands(shr);
	testPointerOperandsNone(shr);
	testBinaryShiftResultType(shr);

	testBinaryNature(shr_un);
	testBinaryShiftOperands(shr_un);
	testPointerOperandsNone(shr_un);
	testBinaryShiftResultType(shr_un);

	testUnaryNature(neg);
	testUnaryNumericOperands(neg);
	RegisterSimple(neg_type_1);
	RegisterSimple(neg_type_2);
	RegisterSimple(neg_type_3);
	RegisterSimple(neg_type_4);

	testUnaryNature(not);
	testUnaryIntegerOperands(not);
	RegisterSimple(not_type_1);
	RegisterSimple(not_type_2);
	RegisterSimple(not_type_3);

	testUnaryNature(ckfinite);
	testUnaryFloatOperands(ckfinite);
	RegisterSimple(ckfinite_type_1);
}

#ifdef	__cplusplus
};
#endif
