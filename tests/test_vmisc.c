/*
 * test_vmisc.c - Test miscellaneous bytecode verification.
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

#include "test_verify.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Tests for methods that contain the "arglist" instruction.
 */
static char *vararg_test_1[] = {
	/* OK: "arglist" used in vararg method */
	"public static vararg void test()",
	"	arglist",
	"	ret",
	0
};
static char *vararg_test_2[] = {
	/* FAIL: "arglist" used in non-vararg method */
	"public static void test()",
	"	arglist",
	"	pop",
	"	ret",
	0
};
static char *vararg_test_3[] = {
	/* OK: "arglist" gives an instance of RuntimeArgumentHandle */
	"public static vararg void test()",
	"	.locals (valuetype System.RuntimeArgumentHandle)",
	"	arglist",
	"	stloc.0",
	"	ret",
	0
};
static char *vararg_test_4[] = {
	/* FAIL: "arglist" does not give an instance of RuntimeArgumentHandlex */
	"public static vararg void test()",
	"	.locals (valuetype System.RuntimeArgumentHandlex)",
	"	arglist",
	"	stloc.0",
	"	ret",
	0
};
static char *vararg_test_5[] = {
	/* FAIL: "arglist" pushes only one value on the stack */
	"public static vararg void test()",
	"	arglist",
	"	pop",
	"	pop",
	"	ret",
	0
};
static char *vararg_test_6[] = {
	/* FAIL: "arglist" pushes one value on the stack */
	"public static vararg void test()",
	"0",
	"	.locals (int32)",
	"	arglist",
	"	ret",
	0
};
static char *vararg_test_7[] = {
	/* OK: "arglist" pushes only one value on the stack */
	"public static vararg void test()",
	"1",
	"	.locals (int32)",
	"	arglist",
	"	ret",
	0
};

/*
 * Test the ternary nature of a "cpblk" or "initblk" instruction.
 */
#define	declareTernaryNature(name,iname,convi)	\
static char *name##_ternary_1[] = { \
	/* FAIL: needs 3 or more stack arguments */ \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_ternary_2[] = { \
	/* FAIL: needs 3 or more stack arguments */ \
	"	ldc.i4.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_ternary_3[] = { \
	/* FAIL: needs 2 or more stack arguments */ \
	"	ldc.i4.0", \
	"	ldc.i4.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_ternary_4[] = { \
	/* OK: needs 3 or more stack arguments */ \
	"	ldc.i4.0", \
	"	conv.i", \
	"	ldc.i4.0", \
	convi \
	"	ldc.i4.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_ternary_5[] = { \
	/* OK: does not push any values */ \
	"3", \
	"	.locals (int32)", \
	"	ldc.i4.0", \
	"	conv.i", \
	"	ldc.i4.0", \
	convi \
	"	ldc.i4.0", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_ternary_6[] = { \
	/* FAIL: pops 3 values */ \
	"	ldc.i4.0", \
	"	conv.i", \
	"	ldc.i4.0", \
	convi \
	"	ldc.i4.0", \
	"	" iname, \
	"	pop", \
	"	ret", \
	0 \
}
#define	testTernaryNature(name)	\
	do { \
		RegisterSimpleFail(name##_ternary_1); \
		RegisterSimpleFail(name##_ternary_2); \
		RegisterSimpleFail(name##_ternary_3); \
		RegisterSimpleUnsafe(name##_ternary_4); \
		RegisterSimpleUnsafe(name##_ternary_5); \
		RegisterSimpleFail(name##_ternary_6); \
	} while (0)

/*
 * Test the prefixes that can be used with a "cpblk" or "initblk" instruction.
 */
#define	declarePrefixNature(name,iname,convi)	\
static char *name##_volatile_prefix[] = { \
	/* OK: volatile prefix is OK */ \
	"3", \
	"	.locals (int32)", \
	"	ldc.i4.0", \
	"	conv.i", \
	"	ldc.i4.0", \
	convi \
	"	ldc.i4.0", \
	"	volatile.", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_unaligned_prefix[] = { \
	/* OK: unaligned prefix is OK */ \
	"3", \
	"	.locals (int32)", \
	"	ldc.i4.0", \
	"	conv.i", \
	"	ldc.i4.0", \
	convi \
	"	ldc.i4.0", \
	"	unaligned. 1", \
	"	" iname, \
	"	ret", \
	0 \
}; \
static char *name##_both_prefixes[] = { \
	/* OK: both prefixes are OK */ \
	"3", \
	"	.locals (int32)", \
	"	ldc.i4.0", \
	"	conv.i", \
	"	ldc.i4.0", \
	convi \
	"	ldc.i4.0", \
	"	volatile.", \
	"	unaligned. 1", \
	"	" iname, \
	"	ldc.i4.0", \
	"	conv.i", \
	"	ldc.i4.0", \
	convi \
	"	ldc.i4.0", \
	"	unaligned. 1", \
	"	volatile.", \
	"	" iname, \
	"	ret", \
	0 \
}
#define	testPrefixNature(name)	\
	do { \
		RegisterSimpleUnsafe(name##_volatile_prefix); \
		RegisterSimpleUnsafe(name##_unaligned_prefix); \
		RegisterSimpleUnsafe(name##_both_prefixes); \
	} while (0)

/*
 * Declare test blocks for "cpblk" and "initblk".
 */
declareTernaryNature(cpblk, "cpblk", "	conv.i\n");
declarePrefixNature(cpblk, "cpblk", "	conv.i\n");
declareTernaryNature(initblk, "initblk", "");
declarePrefixNature(initblk, "initblk", "");

/*
 * Register miscellaneous unit tests.
 */
void registerMiscTests(void)
{
	ILUnitRegisterSuite("Misc Instructions");

	/*
	 * Variable argument method tests.
	 */
	RegisterSimple(vararg_test_1);
	RegisterSimpleFail(vararg_test_2);
	RegisterSimple(vararg_test_3);
	RegisterSimpleFail(vararg_test_4);
	RegisterSimpleFail(vararg_test_5);
	RegisterSimpleFail(vararg_test_6);
	RegisterSimple(vararg_test_7);

	/*
	 * Tests for the "cpblk" instruction.
	 */
	testTernaryNature(cpblk);
	testPrefixNature(cpblk);

	/*
	 * Tests for the "initblk" instruction.
	 */
	testTernaryNature(initblk);
	testPrefixNature(initblk);
}

#ifdef	__cplusplus
};
#endif
