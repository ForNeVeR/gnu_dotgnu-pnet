/*
 * test_vlocal.c - Test the bytecode verifier for local and arg variables.
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

/*

Note: there is one ECMA-specified test that we don't perform here.
And that is the one which checks that a local variable has been properly
initialized in a method without the "zeroinit" flag having been set.

The reason why we don't test for this is that our engine will always
assume that flag is set even if it wasn't.  i.e. we always clear the
local variable array to zero, even if the programmer didn't ask for it.
So, there is no security problem if we don't check for it.

*/

#include "test_verify.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Local variable test block.
 */
typedef struct
{
	int			isArgTest;
	unsigned	maxVars;
	const char *type;
	char      **insns;

} LocalVarTest;

/*
 * Build the assembly stream for a local variable test.
 */
static void buildVarStream(void *arg)
{
	LocalVarTest *test = (LocalVarTest *)arg;
	unsigned num;
	char **insns;

	/* Create the method header */
	ILUnitAsmCreate();
	ILUnitAsmWrite(".method public static void test(");
	if(test->isArgTest)
	{
		for(num = 0; num < test->maxVars; ++num)
		{
			if(num != 0)
			{
				ILUnitAsmWrite(", ");
			}
			ILUnitAsmPrintF("%s v_%u", test->type, num);
		}
	}
	ILUnitAsmWrite(") cil managed\n");
	ILUnitAsmWrite("{\n");

	/* Declare local variables */
	if(!(test->isArgTest))
	{
		for(num = 0; num < test->maxVars; ++num)
		{
			ILUnitAsmPrintF("\t.locals (%s v_%u)\n", test->type, num);
		}
	}

	/* Output the instructions within the method */
	insns = test->insns;
	if(insns[0][0] != '\t' && insns[0][0] != ' ')
	{
		if(insns[0][0] >= '0' && insns[0][0] <= '9')
		{
			ILUnitAsmWrite("\t.maxstack ");
			ILUnitAsmWrite(insns[0]);
			ILUnitAsmWrite("\n");
			++insns;
		}
	}
	while(*insns != 0)
	{
		ILUnitAsmWrite(*insns);
		ILUnitAsmWrite("\n");
		++insns;
	}

	/* Terminate the method and then load the final image */
	ILUnitAsmEndMethod();
	ILUnitAsmClose();
}

/*
 * Perform a local variable test.
 */
static void testVar(void *arg)
{
	/* Build the stream */
	buildVarStream(arg);

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
 * Perform a local variable test that should always fail.
 */
static void testVarFail(void *arg)
{
	/* Build the stream */
	buildVarStream(arg);

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
 * Perform a local variable test that should only succeed in unsafe mode.
 */
static void testVarUnsafe(void *arg)
{
	/* Build the stream */
	buildVarStream(arg);

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
 * Register a variable test.
 */
#define	RegisterVar(name)	   	ILUnitRegister(#name, testVar, &name)
#define	RegisterVarFail(name)   ILUnitRegister(#name, testVarFail, &name)
#define	RegisterVarUnsafe(name) ILUnitRegister(#name, testVarUnsafe, &name)

/*
 * Test the stack and numeric range nature of a variable load instruction.
 */
#define	declareVarLoadNature(name,iname,maxVars,isArgTest)	\
static char *name##_stack_insns_1[] = { \
	/* OK: instruction pushes 1 value on the stack */ \
	"8", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_stack_1 = { \
	(isArgTest), (maxVars), "int32", name##_stack_insns_1 \
}; \
static char *name##_stack_insns_2[] = { \
	/* FAIL: instruction pushes 1 value on the stack */ \
	"0", \
	"	.locals (int32)", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_stack_2 = { \
	(isArgTest), (maxVars), "int32", name##_stack_insns_2 \
}; \
static char *name##_stack_insns_3[] = { \
	/* OK: instruction pushes 1 value on the stack */ \
	"1", \
	"	.locals (int32)", \
	"	" iname, \
	"	pop", \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_stack_3 = { \
	(isArgTest), (maxVars), "int32", name##_stack_insns_3 \
}; \
static char *name##_stack_insns_4[] = { \
	/* FAIL: instruction pushes 1 value on the stack */ \
	"1", \
	"	.locals (int32)", \
	"	" iname, \
	"	pop", \
	"	pop", \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_stack_4 = { \
	(isArgTest), (maxVars), "int32", name##_stack_insns_4 \
}; \
static char *name##_index_insns_1[] = { \
	/* FAIL: variable index out of range because there are no variables */ \
	"8", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_index_1 = { \
	(isArgTest), 0, "int32", name##_index_insns_1 \
}; \
static char *name##_index_insns_2[] = { \
	/* FAIL: variable index out of range because there are not enough */ \
	"8", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_index_2 = { \
	(isArgTest), (maxVars) - 1, "int32", name##_index_insns_2 \
}
#define	testVarLoadNature(name)	\
	do { \
		RegisterVar(name##_stack_1); \
		RegisterVarFail(name##_stack_2); \
		RegisterVar(name##_stack_3); \
		RegisterVarFail(name##_stack_4); \
		RegisterVarFail(name##_index_1); \
		RegisterVarFail(name##_index_2); \
	} while (0)

/*
 * Test the stack and numeric range nature of a variable store instruction.
 */
#define	declareVarStoreNature(name,iname,maxVars,isArgTest)	\
static char *name##_stack_insns_1[] = { \
	/* FAIL: instruction pops 1 value from the stack */ \
	"8", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_stack_1 = { \
	(isArgTest), (maxVars), "int32", name##_stack_insns_1 \
}; \
static char *name##_stack_insns_2[] = { \
	/* OK: instruction pops 1 value from the stack */ \
	"8", \
	"	ldc.i4.1", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_stack_2 = { \
	(isArgTest), (maxVars), "int32", name##_stack_insns_2 \
}; \
static char *name##_index_insns_1[] = { \
	/* FAIL: variable index out of range because there are no variables */ \
	"8", \
	"	ldc.i4.1", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_index_1 = { \
	(isArgTest), 0, "int32", name##_index_insns_1 \
}; \
static char *name##_index_insns_2[] = { \
	/* FAIL: variable index out of range because there are not enough */ \
	"8", \
	"	ldc.i4.1", \
	"	" iname, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_index_2 = { \
	(isArgTest), (maxVars) - 1, "int32", name##_index_insns_2 \
}
#define	testVarStoreNature(name)	\
	do { \
		RegisterVarFail(name##_stack_1); \
		RegisterVar(name##_stack_2); \
		RegisterVarFail(name##_index_1); \
		RegisterVarFail(name##_index_2); \
	} while (0)

/*
 * Test the type of a local variable's loaded value.
 */
#define	declareVarLoadType(name,iname,isArgTest) \
static char *name##_type_insns_1[] = { \
	"8", \
	"	" iname " 0", \
	VERIFY_TYPE_BOOL, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_type_1 = { \
	(isArgTest), 1, "bool", name##_type_insns_1 \
}; \
static char *name##_type_insns_2[] = { \
	"8", \
	"	" iname " 0", \
	VERIFY_TYPE_I4, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_type_2 = { \
	(isArgTest), 1, "int8", name##_type_insns_2 \
}; \
static LocalVarTest name##_type_3 = { \
	(isArgTest), 1, "unsigned int8", name##_type_insns_2 \
}; \
static LocalVarTest name##_type_4 = { \
	(isArgTest), 1, "int16", name##_type_insns_2 \
}; \
static LocalVarTest name##_type_5 = { \
	(isArgTest), 1, "unsigned int16", name##_type_insns_2 \
}; \
static LocalVarTest name##_type_6 = { \
	(isArgTest), 1, "char", name##_type_insns_2 \
}; \
static LocalVarTest name##_type_7 = { \
	(isArgTest), 1, "int32", name##_type_insns_2 \
}; \
static LocalVarTest name##_type_8 = { \
	(isArgTest), 1, "unsigned int32", name##_type_insns_2 \
}; \
static char *name##_type_insns_9[] = { \
	"8", \
	"	" iname " 0", \
	VERIFY_TYPE_I8, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_type_9 = { \
	(isArgTest), 1, "int64", name##_type_insns_9 \
}; \
static LocalVarTest name##_type_10 = { \
	(isArgTest), 1, "unsigned int64", name##_type_insns_9 \
}; \
static char *name##_type_insns_11[] = { \
	"8", \
	"	" iname " 0", \
	VERIFY_TYPE_F, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_type_11 = { \
	(isArgTest), 1, "float32", name##_type_insns_11 \
}; \
static LocalVarTest name##_type_12 = { \
	(isArgTest), 1, "float64", name##_type_insns_11 \
}; \
static char *name##_type_insns_13[] = { \
	"8", \
	"	" iname " 0", \
	VERIFY_TYPE_I, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_type_13 = { \
	(isArgTest), 1, "native int", name##_type_insns_13 \
}; \
static LocalVarTest name##_type_14 = { \
	(isArgTest), 1, "native unsigned int", name##_type_insns_13 \
}; \
static LocalVarTest name##_type_15 = { \
	(isArgTest), 1, "void *", name##_type_insns_13 \
}; \
static char *name##_type_insns_16[] = { \
	"8", \
	"	" iname " 0", \
	VERIFY_TYPE_O, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_type_16 = { \
	(isArgTest), 1, "class [mscorlib]System.Object", name##_type_insns_16 \
}; \
static LocalVarTest name##_type_17 = { \
	(isArgTest), 1, "class [mscorlib]System.String", name##_type_insns_16 \
}; \
static char *name##_type_insns_18[] = { \
	"8", \
	"	" iname " 0", \
	VERIFY_TYPE_STR, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_type_18 = { \
	(isArgTest), 1, "class [mscorlib]System.String", name##_type_insns_18 \
}; \
static char *name##_type_insns_19[] = { \
	"8", \
	"	" iname " 0", \
	VERIFY_TYPE_MV, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_type_19 = { \
	(isArgTest), 1, "valuetype [mscorlib]System.Decimal", name##_type_insns_19 \
}; \
static char *name##_type_insns_20[] = { \
	"8", \
	"	" iname " 0", \
	VERIFY_TYPE_M, \
	"	ret", \
	0, \
}; \
static LocalVarTest name##_type_20 = { \
	(isArgTest), 1, "int32 &", name##_type_insns_20 \
}
#define	testVarLoadType(name)	\
	do { \
		RegisterVar(name##_type_1); \
		RegisterVar(name##_type_2); \
		RegisterVar(name##_type_3); \
		RegisterVar(name##_type_4); \
		RegisterVar(name##_type_5); \
		RegisterVar(name##_type_6); \
		RegisterVar(name##_type_7); \
		RegisterVar(name##_type_8); \
		RegisterVar(name##_type_9); \
		RegisterVar(name##_type_10); \
		RegisterVar(name##_type_11); \
		RegisterVar(name##_type_12); \
		RegisterVar(name##_type_13); \
		RegisterVar(name##_type_14); \
		RegisterVarUnsafe(name##_type_15); \
		RegisterVar(name##_type_16); \
		RegisterVar(name##_type_17); \
		RegisterVar(name##_type_18); \
		RegisterVar(name##_type_19); \
		RegisterVar(name##_type_20); \
	} while (0)

/*
 * Declare test blocks for local variable instructions.
 */
declareVarLoadNature(ldloc_0, "ldloc.0", 1, 0);
declareVarLoadNature(ldloc_1, "ldloc.1", 2, 0);
declareVarLoadNature(ldloc_2, "ldloc.2", 3, 0);
declareVarLoadNature(ldloc_3, "ldloc.3", 4, 0);
declareVarLoadNature(ldloc_s_4, "ldloc.s 4", 5, 0);
declareVarLoadNature(ldloc_s_255, "ldloc.s 255", 256, 0);
declareVarLoadNature(ldloc_256, "ldloc 256", 257, 0);
declareVarLoadNature(ldloc_1000, "ldloc 1000", 1001, 0);

declareVarLoadType(ldloc, "ldloc", 0);

declareVarStoreNature(stloc_0, "stloc.0", 1, 0);
declareVarStoreNature(stloc_1, "stloc.1", 2, 0);
declareVarStoreNature(stloc_2, "stloc.2", 3, 0);
declareVarStoreNature(stloc_3, "stloc.3", 4, 0);
declareVarStoreNature(stloc_s_4, "stloc.s 4", 5, 0);
declareVarStoreNature(stloc_s_255, "stloc.s 255", 256, 0);
declareVarStoreNature(stloc_256, "stloc 256", 257, 0);
declareVarStoreNature(stloc_1000, "stloc 1000", 1001, 0);

declareVarLoadNature(ldloca_s_0, "ldloca.s 0", 1, 0);
declareVarLoadNature(ldloca_s_1, "ldloca.s 1", 2, 0);
declareVarLoadNature(ldloca_s_2, "ldloca.s 2", 3, 0);
declareVarLoadNature(ldloca_s_3, "ldloca.s 3", 4, 0);
declareVarLoadNature(ldloca_s_4, "ldloca.s 4", 5, 0);
declareVarLoadNature(ldloca_s_255, "ldloca.s 255", 256, 0);
declareVarLoadNature(ldloca_256, "ldloca 256", 257, 0);
declareVarLoadNature(ldloca_1000, "ldloca 1000", 1001, 0);

/*
 * Declare test blocks for argument instructions.
 */
declareVarLoadNature(ldarg_0, "ldarg.0", 1, 1);
declareVarLoadNature(ldarg_1, "ldarg.1", 2, 1);
declareVarLoadNature(ldarg_2, "ldarg.2", 3, 1);
declareVarLoadNature(ldarg_3, "ldarg.3", 4, 1);
declareVarLoadNature(ldarg_s_4, "ldarg.s 4", 5, 1);
declareVarLoadNature(ldarg_s_255, "ldarg.s 255", 256, 1);
declareVarLoadNature(ldarg_256, "ldarg 256", 257, 1);
declareVarLoadNature(ldarg_1000, "ldarg 1000", 1001, 1);

declareVarLoadType(ldarg, "ldarg", 1);

declareVarStoreNature(starg_s_0, "starg.s 0", 1, 1);
declareVarStoreNature(starg_s_1, "starg.s 1", 2, 1);
declareVarStoreNature(starg_s_2, "starg.s 2", 3, 1);
declareVarStoreNature(starg_s_3, "starg.s 3", 4, 1);
declareVarStoreNature(starg_s_4, "starg.s 4", 5, 1);
declareVarStoreNature(starg_s_255, "starg.s 255", 256, 1);
declareVarStoreNature(starg_256, "starg 256", 257, 1);
declareVarStoreNature(starg_1000, "starg 1000", 1001, 1);

declareVarLoadNature(ldarga_s_0, "ldarga.s 0", 1, 1);
declareVarLoadNature(ldarga_s_1, "ldarga.s 1", 2, 1);
declareVarLoadNature(ldarga_s_2, "ldarga.s 2", 3, 1);
declareVarLoadNature(ldarga_s_3, "ldarga.s 3", 4, 1);
declareVarLoadNature(ldarga_s_4, "ldarga.s 4", 5, 1);
declareVarLoadNature(ldarga_s_255, "ldarga.s 255", 256, 1);
declareVarLoadNature(ldarga_256, "ldarga 256", 257, 1);
declareVarLoadNature(ldarga_1000, "ldarga 1000", 1001, 1);

/*
 * Register local variable unit tests.
 */
void registerVarTests(void)
{
	ILUnitRegisterSuite("Local Variable Instructions");

	testVarLoadNature(ldloc_0);
	testVarLoadNature(ldloc_1);
	testVarLoadNature(ldloc_2);
	testVarLoadNature(ldloc_3);
	testVarLoadNature(ldloc_s_4);
	testVarLoadNature(ldloc_s_255);
	testVarLoadNature(ldloc_256);
	testVarLoadNature(ldloc_1000);

	testVarLoadType(ldloc);

	testVarStoreNature(stloc_0);
	testVarStoreNature(stloc_1);
	testVarStoreNature(stloc_2);
	testVarStoreNature(stloc_3);
	testVarStoreNature(stloc_s_4);
	testVarStoreNature(stloc_s_255);
	testVarStoreNature(stloc_256);
	testVarStoreNature(stloc_1000);

	testVarLoadNature(ldloca_s_0);
	testVarLoadNature(ldloca_s_1);
	testVarLoadNature(ldloca_s_2);
	testVarLoadNature(ldloca_s_3);
	testVarLoadNature(ldloca_s_4);
	testVarLoadNature(ldloca_s_255);
	testVarLoadNature(ldloca_256);
	testVarLoadNature(ldloca_1000);

	testVarLoadNature(ldarg_0);
	testVarLoadNature(ldarg_1);
	testVarLoadNature(ldarg_2);
	testVarLoadNature(ldarg_3);
	testVarLoadNature(ldarg_s_4);
	testVarLoadNature(ldarg_s_255);
	testVarLoadNature(ldarg_256);
	testVarLoadNature(ldarg_1000);

	testVarLoadType(ldarg);

	testVarStoreNature(starg_s_0);
	testVarStoreNature(starg_s_1);
	testVarStoreNature(starg_s_2);
	testVarStoreNature(starg_s_3);
	testVarStoreNature(starg_s_4);
	testVarStoreNature(starg_s_255);
	testVarStoreNature(starg_256);
	testVarStoreNature(starg_1000);

	testVarLoadNature(ldarga_s_0);
	testVarLoadNature(ldarga_s_1);
	testVarLoadNature(ldarga_s_2);
	testVarLoadNature(ldarga_s_3);
	testVarLoadNature(ldarga_s_4);
	testVarLoadNature(ldarga_s_255);
	testVarLoadNature(ldarga_256);
	testVarLoadNature(ldarga_1000);
}

#ifdef	__cplusplus
};
#endif
