/*
 * test_vconst.c - Test the bytecode verifier on constant values.
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
 * Test the behaviour of a constant instruction.
 */
#define	declareConstant(name, iname, verify) \
static char *name##_test_1[] = { \
	/* OK: no argument values are required on the stack */ \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_test_2[] = { \
	/* FAIL: constants push 1 element */ \
	"0", \
	"	.locals (int32)", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_test_3[] = { \
	/* OK: constants do not pop anything */ \
	"	" iname, \
	"	pop", \
	"	ret", \
	0 \
}; \
static char *name##_test_4[] = { \
	/* FAIL: constants do not pop anything */ \
	"	" iname, \
	"	pop", \
	"	pop", \
	"	ret", \
	0 \
}; \
static char *name##_test_5[] = { \
	/* OK: verify constant type */ \
	"	" iname, \
	verify, \
	"	ret", \
	0 \
}
#define	testConstant(name)	\
	do { \
		RegisterSimple(name##_test_1); \
		RegisterSimpleFail(name##_test_2); \
		RegisterSimple(name##_test_3); \
		RegisterSimpleFail(name##_test_4); \
		RegisterSimple(name##_test_5); \
	} while (0)

/*
 * Declare test blocks for the constant unit tests.
 */
declareConstant(ldnull, "ldnull", VERIFY_TYPE_O);
declareConstant(ldc_i4_m1, "ldc.i4.m1", VERIFY_TYPE_I4);
declareConstant(ldc_i4_0, "ldc.i4.0", VERIFY_TYPE_I4);
declareConstant(ldc_i4_1, "ldc.i4.1", VERIFY_TYPE_I4);
declareConstant(ldc_i4_2, "ldc.i4.2", VERIFY_TYPE_I4);
declareConstant(ldc_i4_3, "ldc.i4.3", VERIFY_TYPE_I4);
declareConstant(ldc_i4_4, "ldc.i4.4", VERIFY_TYPE_I4);
declareConstant(ldc_i4_5, "ldc.i4.5", VERIFY_TYPE_I4);
declareConstant(ldc_i4_6, "ldc.i4.6", VERIFY_TYPE_I4);
declareConstant(ldc_i4_7, "ldc.i4.7", VERIFY_TYPE_I4);
declareConstant(ldc_i4_8, "ldc.i4.8", VERIFY_TYPE_I4);
declareConstant(ldc_i4_s, "ldc.i4.s 23", VERIFY_TYPE_I4);
declareConstant(ldc_i4, "ldc.i4 2300", VERIFY_TYPE_I4);
declareConstant(ldc_i8, "ldc.i8 0x100000000", VERIFY_TYPE_I8);
declareConstant(ldc_r4, "ldc.r4 2.3", VERIFY_TYPE_F);
declareConstant(ldc_r8, "ldc.r8 2.3", VERIFY_TYPE_F);
declareConstant(ldstr, "ldstr \"Hello World!\"", VERIFY_TYPE_STR);

/*
 * Register constant unit tests.
 */
void registerConstantTests(void)
{
	ILUnitRegisterSuite("Constant Instructions");

	testConstant(ldnull);
	testConstant(ldc_i4_m1);
	testConstant(ldc_i4_0);
	testConstant(ldc_i4_1);
	testConstant(ldc_i4_2);
	testConstant(ldc_i4_3);
	testConstant(ldc_i4_4);
	testConstant(ldc_i4_5);
	testConstant(ldc_i4_6);
	testConstant(ldc_i4_7);
	testConstant(ldc_i4_8);
	testConstant(ldc_i4_s);
	testConstant(ldc_i4);
	testConstant(ldc_i8);
	testConstant(ldc_r4);
	testConstant(ldc_r8);
	testConstant(ldstr);
}

#ifdef	__cplusplus
};
#endif
