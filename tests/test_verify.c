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

#include "test_verify.h"

#ifdef	__cplusplus
extern	"C" {
#endif

void buildSimpleStream(void *arg)
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

int verify(int allowUnsafe)
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

int verifyAllFail(int allowUnsafe)
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

void testSimple(void *arg)
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

void testSimpleFail(void *arg)
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

void testSimpleUnsafe(void *arg)
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
 * Test "break".  Note: ECMA says that "break" is verifiable.
 * However, our verifier always rejects breakpoints in loaded
 * code, since we don't want miscreants forcing an exit to
 * the debugger.  Besides, breakpoints are not set in IL code
 * in our engine.  They are set in CVM or native code using
 * other mechanisms.
 */
static char *break_1[] = {
	/* FAIL: "break" is an invalid instruction */
	"	break",
	"	ret",
	0,
};

/*
 * Test "dup".
 */
static char *dup_1[] = {
	/* FAIL: dup requires 1 or more arguments */
	"	dup",
	"	ret",
	0,
};
static char *dup_2[] = {
	/* OK: dup requires 1 or more arguments */
	"	ldc.i4.0",
	"	dup",
	"	ret",
	0,
};
static char *dup_3[] = {
	/* FAIL: dup pushes 1 value */
	"1",
	"	.locals (int32)",
	"	ldc.i4.0",
	"	dup",
	"	ret",
	0,
};
static char *dup_4[] = {
	/* OK: dup pushes 1 value */
	"2",
	"	.locals (int32)",
	"	ldc.i4.0",
	"	dup",
	"	ret",
	0,
};

/*
 * Test "pop".
 */
static char *pop_1[] = {
	/* FAIL: pop requires 1 or more arguments */
	"	pop",
	"	ret",
	0,
};
static char *pop_2[] = {
	/* OK: pop requires 1 or more arguments */
	"	ldc.i4.0",
	"	pop",
	"	ret",
	0,
};
static char *pop_3[] = {
	/* OK: pop does not push any values */
	"1",
	"	.locals (int32)",
	"	ldc.i4.0",
	"	pop",
	"	ret",
	0,
};
static char *pop_4[] = {
	/* FAIL: dup pops 1 value */
	"	ldc.i4.0",
	"	pop",
	"	pop",
	"	ret",
	0,
};

/*
 * Declare the illegal instructions.
 */
static int illegalInsns[] = {
	0x24, 0x78, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab,
	0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xbb, 0xbc, 0xbd, 0xbe,
	0xbf, 0xc0, 0xc1, 0xc4, 0xc5, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6,
	0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1,
	0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc,
	0xfd, 0xff, -1
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
	RegisterSimpleFail(break_1);
	RegisterSimpleFail(dup_1);
	RegisterSimple(dup_2);
	RegisterSimpleFail(dup_3);
	RegisterSimple(dup_4);
	RegisterSimpleFail(pop_1);
	RegisterSimple(pop_2);
	RegisterSimple(pop_3);
	RegisterSimpleFail(pop_4);

	/*
	 * Register tests of the main instruction categories.
	 */
	registerArithTests();
	registerConvTests();
	registerCompareTests();
	registerBranchTests();
	registerConstantTests();

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
