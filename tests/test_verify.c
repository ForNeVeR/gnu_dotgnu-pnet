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
		ILUnitAsmMethod("public static void test()", 0);
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
 * Simple tests.
 */
static char *test_nop_0[] = {
	"0",
	"	nop",
	"	ret",
	0
};
static char *test_nop[] = {
	"	nop",
	"	ret",
	0
};
static char *test_mul[] = {
	"	ldc.i4.3",
	//"	ldc.i4.4",
	"	mul",
	"	ret",
	0
};
static char *test_ldind[] = {
	"	ldc.i4.0",
	"	conv.u",
	"	ldind.i4",
	"	ret",
	0
};

/*
 * Register all unit tests.
 */
void ILUnitRegisterTests(void)
{
	ILUnitRegisterSuite("Simple Tests");
	RegisterSimple(test_nop_0);
	RegisterSimple(test_nop);
	RegisterSimpleFail(test_mul);
	RegisterSimpleUnsafe(test_ldind);
}

#ifdef	__cplusplus
};
#endif
