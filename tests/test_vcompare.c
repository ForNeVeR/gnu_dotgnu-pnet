/*
 * test_vcompare.c - Test the bytecode verifier for comparison instructions.
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
 * Test the result types for a comparison operator.
 */
#define	declareCompareResultType(name, iname) \
static char *name##_type_1[] = { \
	/* i4 and i4 */ \
	"	ldc.i4.3", \
	"	ldc.i4.5", \
	"	" iname, \
	VERIFY_TYPE_BOOL, \
	"	ret", \
	0, \
}; \
static char *name##_type_2[] = { \
	/* i4 and i */ \
	"	ldc.i4.3", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	VERIFY_TYPE_BOOL, \
	"	ret", \
	0, \
}; \
static char *name##_type_3[] = { \
	/* i8 and i8 */ \
	"	ldc.i8	3", \
	"	ldc.i8	5", \
	"	" iname, \
	VERIFY_TYPE_BOOL, \
	"	ret", \
	0, \
}; \
static char *name##_type_4[] = { \
	/* i and i4 */ \
	"	ldc.i4.3", \
	"	conv.i", \
	"	ldc.i4.5", \
	"	" iname, \
	VERIFY_TYPE_BOOL, \
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
	VERIFY_TYPE_BOOL, \
	"	ret", \
	0, \
}; \
static char *name##_type_6[] = { \
	/* f and f */ \
	"	ldc.r4	3.0", \
	"	ldc.r4	5.0", \
	"	" iname, \
	VERIFY_TYPE_BOOL, \
	"	ret", \
	0, \
}; \
static char *name##_type_7[] = { \
	/* m and m */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldloca	0", \
	"	" iname, \
	VERIFY_TYPE_BOOL, \
	"	ret", \
	0, \
}; \
static char *name##_type_8[] = { \
	/* m and i */ \
	"	.locals (int32)", \
	"	ldloca	0", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	VERIFY_TYPE_BOOL, \
	"	ret", \
	0, \
}; \
static char *name##_type_9[] = { \
	/* i and m */ \
	"	.locals (int32)", \
	"	ldc.i4.5", \
	"	conv.i", \
	"	ldloca	0", \
	"	" iname, \
	VERIFY_TYPE_BOOL, \
	"	ret", \
	0, \
}; \
static char *name##_type_10[] = { \
	/* o and o */ \
	"	ldnull", \
	"	ldnull", \
	"	" iname, \
	VERIFY_TYPE_BOOL, \
	"	ret", \
	0, \
}
#define	testCompareResultType(name)	\
	do { \
		RegisterSimple(name##_type_1); \
		RegisterSimple(name##_type_2); \
		RegisterSimple(name##_type_3); \
		RegisterSimple(name##_type_4); \
		RegisterSimple(name##_type_5); \
		RegisterSimple(name##_type_6); \
		RegisterSimple(name##_type_7); \
		RegisterSimpleFail(name##_type_8); \
		RegisterSimpleFail(name##_type_9); \
		RegisterSimpleFail(name##_type_10); \
	} while (0)
#define	testCompareResultTypeEq(name)	\
	do { \
		RegisterSimple(name##_type_1); \
		RegisterSimple(name##_type_2); \
		RegisterSimple(name##_type_3); \
		RegisterSimple(name##_type_4); \
		RegisterSimple(name##_type_5); \
		RegisterSimple(name##_type_6); \
		RegisterSimple(name##_type_7); \
		RegisterSimpleUnsafe(name##_type_8); \
		RegisterSimpleUnsafe(name##_type_9); \
		RegisterSimple(name##_type_10); \
	} while (0)
#define	testCompareResultTypeGtUn(name)	\
	do { \
		RegisterSimple(name##_type_1); \
		RegisterSimple(name##_type_2); \
		RegisterSimple(name##_type_3); \
		RegisterSimple(name##_type_4); \
		RegisterSimple(name##_type_5); \
		RegisterSimple(name##_type_6); \
		RegisterSimple(name##_type_7); \
		RegisterSimpleFail(name##_type_8); \
		RegisterSimpleFail(name##_type_9); \
		RegisterSimple(name##_type_10); \
	} while (0)

/*
 * Declare test blocks for comparison instructions.
 */
declareBinaryNature(ceq, "ceq");
declareBinaryOperands(ceq, "ceq");
declarePointerOperands(ceq, "ceq");
declareCompareResultType(ceq, "ceq");

declareBinaryNature(cgt, "cgt");
declareBinaryOperands(cgt, "cgt");
declarePointerOperands(cgt, "cgt");
declareCompareResultType(cgt, "cgt");

declareBinaryNature(cgt_un, "cgt.un");
declareBinaryOperands(cgt_un, "cgt.un");
declarePointerOperands(cgt_un, "cgt.un");
declareCompareResultType(cgt_un, "cgt.un");

declareBinaryNature(clt, "clt");
declareBinaryOperands(clt, "clt");
declarePointerOperands(clt, "clt");
declareCompareResultType(clt, "clt");

declareBinaryNature(clt_un, "clt.un");
declareBinaryOperands(clt_un, "clt.un");
declarePointerOperands(clt_un, "clt.un");
declareCompareResultType(clt_un, "clt.un");

/*
 * Register comparison unit tests.
 */
void registerCompareTests(void)
{
	ILUnitRegisterSuite("Comparison Instructions");

	testBinaryNature(ceq);
	testBinaryOperands(ceq);
	testPointerOperandsCompareEq(ceq);
	testCompareResultTypeEq(ceq);

	testBinaryNature(cgt);
	testBinaryOperands(cgt);
	testPointerOperandsCompare(cgt);
	testCompareResultType(cgt);

	testBinaryNature(cgt_un);
	testBinaryOperands(cgt_un);
	testPointerOperandsCompareGtUn(cgt_un);
	testCompareResultTypeGtUn(cgt_un);

	testBinaryNature(clt);
	testBinaryOperands(clt);
	testPointerOperandsCompare(clt);
	testCompareResultType(clt);

	testBinaryNature(clt_un);
	testBinaryOperands(clt_un);
	testPointerOperandsCompare(clt_un);
	testCompareResultType(clt_un);
}

#ifdef	__cplusplus
};
#endif
