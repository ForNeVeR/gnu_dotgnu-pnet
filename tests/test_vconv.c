/*
 * test_vconv.c - Test the bytecode verifier on conversions instructions.
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
 * Test the result types of performing conversions.
 */
#define	declareConvResultType(name, iname, verify)	\
static char *name##_type_1[] = { \
	/* Convert from i4 */ \
	"	ldc.i4.5", \
	"	" iname, \
	verify, \
	"	ret", \
	0, \
}; \
static char *name##_type_2[] = { \
	/* Convert from i8 */ \
	"	ldc.i8	5", \
	"	" iname, \
	verify, \
	"	ret", \
	0, \
}; \
static char *name##_type_3[] = { \
	/* Convert from i */ \
	"	ldc.i4.5", \
	"	conv.i", \
	"	" iname, \
	verify, \
	"	ret", \
	0, \
}; \
static char *name##_type_4[] = { \
	/* Convert from f */ \
	"	ldc.r4	5.0", \
	"	" iname, \
	verify, \
	"	ret", \
	0, \
}
#define	testConvResultType(name) \
	do { \
		RegisterSimple(name##_type_1); \
		RegisterSimple(name##_type_2); \
		RegisterSimple(name##_type_3); \
		RegisterSimple(name##_type_4); \
	} while (0)

/*
 * Declare test blocks for the conversion instructions.
 */
declareUnaryNature(conv_i1, "conv.i1");
declareUnaryOperands(conv_i1, "conv.i1");
declareConvResultType(conv_i1, "conv.i1", VERIFY_TYPE_I4);

declareUnaryNature(conv_i2, "conv.i2");
declareUnaryOperands(conv_i2, "conv.i2");
declareConvResultType(conv_i2, "conv.i2", VERIFY_TYPE_I4);

declareUnaryNature(conv_i4, "conv.i4");
declareUnaryOperands(conv_i4, "conv.i4");
declareConvResultType(conv_i4, "conv.i4", VERIFY_TYPE_I4);

declareUnaryNature(conv_i8, "conv.i8");
declareUnaryOperands(conv_i8, "conv.i8");
declareConvResultType(conv_i8, "conv.i8", VERIFY_TYPE_I8);

declareUnaryNature(conv_r4, "conv.r4");
declareUnaryOperands(conv_r4, "conv.r4");
declareConvResultType(conv_r4, "conv.r4", VERIFY_TYPE_F);

declareUnaryNature(conv_r8, "conv.r8");
declareUnaryOperands(conv_r8, "conv.r8");
declareConvResultType(conv_r8, "conv.r8", VERIFY_TYPE_F);

declareUnaryNature(conv_u4, "conv.u4");
declareUnaryOperands(conv_u4, "conv.u4");
declareConvResultType(conv_u4, "conv.u4", VERIFY_TYPE_I4);

declareUnaryNature(conv_u8, "conv.u8");
declareUnaryOperands(conv_u8, "conv.u8");
declareConvResultType(conv_u8, "conv.u8", VERIFY_TYPE_I8);

declareUnaryNature(conv_r_un, "conv.r.un");
declareUnaryOperands(conv_r_un, "conv.r.un");
declareConvResultType(conv_r_un, "conv.r.un", VERIFY_TYPE_F);

declareUnaryNature(conv_ovf_i1_un, "conv.ovf.i1.un");
declareUnaryOperands(conv_ovf_i1_un, "conv.ovf.i1.un");
declareConvResultType(conv_ovf_i1_un, "conv.ovf.i1.un", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_i2_un, "conv.ovf.i2.un");
declareUnaryOperands(conv_ovf_i2_un, "conv.ovf.i2.un");
declareConvResultType(conv_ovf_i2_un, "conv.ovf.i2.un", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_i4_un, "conv.ovf.i4.un");
declareUnaryOperands(conv_ovf_i4_un, "conv.ovf.i4.un");
declareConvResultType(conv_ovf_i4_un, "conv.ovf.i4.un", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_i8_un, "conv.ovf.i8.un");
declareUnaryOperands(conv_ovf_i8_un, "conv.ovf.i8.un");
declareConvResultType(conv_ovf_i8_un, "conv.ovf.i8.un", VERIFY_TYPE_I8);

declareUnaryNature(conv_ovf_u1_un, "conv.ovf.u1.un");
declareUnaryOperands(conv_ovf_u1_un, "conv.ovf.u1.un");
declareConvResultType(conv_ovf_u1_un, "conv.ovf.u1.un", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_u2_un, "conv.ovf.u2.un");
declareUnaryOperands(conv_ovf_u2_un, "conv.ovf.u2.un");
declareConvResultType(conv_ovf_u2_un, "conv.ovf.u2.un", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_u4_un, "conv.ovf.u4.un");
declareUnaryOperands(conv_ovf_u4_un, "conv.ovf.u4.un");
declareConvResultType(conv_ovf_u4_un, "conv.ovf.u4.un", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_u8_un, "conv.ovf.u8.un");
declareUnaryOperands(conv_ovf_u8_un, "conv.ovf.u8.un");
declareConvResultType(conv_ovf_u8_un, "conv.ovf.u8.un", VERIFY_TYPE_I8);

declareUnaryNature(conv_ovf_i_un, "conv.ovf.i.un");
declareUnaryOperands(conv_ovf_i_un, "conv.ovf.i.un");
declareConvResultType(conv_ovf_i_un, "conv.ovf.i.un", VERIFY_TYPE_I);

declareUnaryNature(conv_ovf_u_un, "conv.ovf.u.un");
declareUnaryOperands(conv_ovf_u_un, "conv.ovf.u.un");
declareConvResultType(conv_ovf_u_un, "conv.ovf.u.un", VERIFY_TYPE_I);

declareUnaryNature(conv_ovf_i1, "conv.ovf.i1");
declareUnaryOperands(conv_ovf_i1, "conv.ovf.i1");
declareConvResultType(conv_ovf_i1, "conv.ovf.i1", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_u1, "conv.ovf.u1");
declareUnaryOperands(conv_ovf_u1, "conv.ovf.u1");
declareConvResultType(conv_ovf_u1, "conv.ovf.u1", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_i2, "conv.ovf.i2");
declareUnaryOperands(conv_ovf_i2, "conv.ovf.i2");
declareConvResultType(conv_ovf_i2, "conv.ovf.i2", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_u2, "conv.ovf.u2");
declareUnaryOperands(conv_ovf_u2, "conv.ovf.u2");
declareConvResultType(conv_ovf_u2, "conv.ovf.u2", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_i4, "conv.ovf.i4");
declareUnaryOperands(conv_ovf_i4, "conv.ovf.i4");
declareConvResultType(conv_ovf_i4, "conv.ovf.i4", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_u4, "conv.ovf.u4");
declareUnaryOperands(conv_ovf_u4, "conv.ovf.u4");
declareConvResultType(conv_ovf_u4, "conv.ovf.u4", VERIFY_TYPE_I4);

declareUnaryNature(conv_ovf_i8, "conv.ovf.i8");
declareUnaryOperands(conv_ovf_i8, "conv.ovf.i8");
declareConvResultType(conv_ovf_i8, "conv.ovf.i8", VERIFY_TYPE_I8);

declareUnaryNature(conv_ovf_u8, "conv.ovf.u8");
declareUnaryOperands(conv_ovf_u8, "conv.ovf.u8");
declareConvResultType(conv_ovf_u8, "conv.ovf.u8", VERIFY_TYPE_I8);

declareUnaryNature(conv_u2, "conv.u2");
declareUnaryOperands(conv_u2, "conv.u2");
declareConvResultType(conv_u2, "conv.u2", VERIFY_TYPE_I4);

declareUnaryNature(conv_u1, "conv.u1");
declareUnaryOperands(conv_u1, "conv.u1");
declareConvResultType(conv_u1, "conv.u1", VERIFY_TYPE_I4);

declareUnaryNature(conv_i, "conv.i");
declareUnaryOperands(conv_i, "conv.i");
declareConvResultType(conv_i, "conv.i", VERIFY_TYPE_I);

declareUnaryNature(conv_ovf_i, "conv.ovf.i");
declareUnaryOperands(conv_ovf_i, "conv.ovf.i");
declareConvResultType(conv_ovf_i, "conv.ovf.i", VERIFY_TYPE_I);

declareUnaryNature(conv_ovf_u, "conv.ovf.u");
declareUnaryOperands(conv_ovf_u, "conv.ovf.u");
declareConvResultType(conv_ovf_u, "conv.ovf.u", VERIFY_TYPE_I);

declareUnaryNature(conv_u, "conv.u");
declareUnaryOperands(conv_u, "conv.u");
declareConvResultType(conv_u, "conv.u", VERIFY_TYPE_I);

/*
 * Register conversion unit tests.
 */
void registerConvTests(void)
{
	ILUnitRegisterSuite("Conversion Instructions");

	testUnaryNature(conv_i1);
	testUnaryNumericOperands(conv_i1);
	testConvResultType(conv_i1);

	testUnaryNature(conv_i2);
	testUnaryNumericOperands(conv_i2);
	testConvResultType(conv_i2);

	testUnaryNature(conv_i4);
	testUnaryNumericOperands(conv_i4);
	testConvResultType(conv_i4);

	testUnaryNature(conv_i8);
	testUnaryNumericOperands(conv_i8);
	testConvResultType(conv_i8);

	testUnaryNature(conv_r4);
	testUnaryNumericOperands(conv_r4);
	testConvResultType(conv_r4);

	testUnaryNature(conv_r8);
	testUnaryNumericOperands(conv_r8);
	testConvResultType(conv_r8);

	testUnaryNature(conv_u4);
	testUnaryNumericOperands(conv_u4);
	testConvResultType(conv_u4);

	testUnaryNature(conv_u8);
	testUnaryNumericOperands(conv_u8);
	testConvResultType(conv_u8);

	testUnaryNature(conv_r_un);
	testUnaryNumericOperands(conv_r_un);
	testConvResultType(conv_r_un);

	testUnaryNature(conv_ovf_i1_un);
	testUnaryNumericOperands(conv_ovf_i1_un);
	testConvResultType(conv_ovf_i1_un);

	testUnaryNature(conv_ovf_i2_un);
	testUnaryNumericOperands(conv_ovf_i2_un);
	testConvResultType(conv_ovf_i2_un);

	testUnaryNature(conv_ovf_i4_un);
	testUnaryNumericOperands(conv_ovf_i4_un);
	testConvResultType(conv_ovf_i4_un);

	testUnaryNature(conv_ovf_i8_un);
	testUnaryNumericOperands(conv_ovf_i8_un);
	testConvResultType(conv_ovf_i8_un);

	testUnaryNature(conv_ovf_u1_un);
	testUnaryNumericOperands(conv_ovf_u1_un);
	testConvResultType(conv_ovf_u1_un);

	testUnaryNature(conv_ovf_u2_un);
	testUnaryNumericOperands(conv_ovf_u2_un);
	testConvResultType(conv_ovf_u2_un);

	testUnaryNature(conv_ovf_u4_un);
	testUnaryNumericOperands(conv_ovf_u4_un);
	testConvResultType(conv_ovf_u4_un);

	testUnaryNature(conv_ovf_u8_un);
	testUnaryNumericOperands(conv_ovf_u8_un);
	testConvResultType(conv_ovf_u8_un);

	testUnaryNature(conv_ovf_i_un);
	testUnaryNumericOperands(conv_ovf_i_un);
	testConvResultType(conv_ovf_i_un);

	testUnaryNature(conv_ovf_u_un);
	testUnaryNumericOperands(conv_ovf_u_un);
	testConvResultType(conv_ovf_u_un);

	testUnaryNature(conv_ovf_i1);
	testUnaryNumericOperands(conv_ovf_i1);
	testConvResultType(conv_ovf_i1);

	testUnaryNature(conv_ovf_u1);
	testUnaryNumericOperands(conv_ovf_u1);
	testConvResultType(conv_ovf_u1);

	testUnaryNature(conv_ovf_i2);
	testUnaryNumericOperands(conv_ovf_i2);
	testConvResultType(conv_ovf_i2);

	testUnaryNature(conv_ovf_u2);
	testUnaryNumericOperands(conv_ovf_u2);
	testConvResultType(conv_ovf_u2);

	testUnaryNature(conv_ovf_i4);
	testUnaryNumericOperands(conv_ovf_i4);
	testConvResultType(conv_ovf_i4);

	testUnaryNature(conv_ovf_u4);
	testUnaryNumericOperands(conv_ovf_u4);
	testConvResultType(conv_ovf_u4);

	testUnaryNature(conv_ovf_i8);
	testUnaryNumericOperands(conv_ovf_i8);
	testConvResultType(conv_ovf_i8);

	testUnaryNature(conv_ovf_u8);
	testUnaryNumericOperands(conv_ovf_u8);
	testConvResultType(conv_ovf_u8);

	testUnaryNature(conv_u2);
	testUnaryNumericOperands(conv_u2);
	testConvResultType(conv_u2);

	testUnaryNature(conv_u1);
	testUnaryNumericOperands(conv_u1);
	testConvResultType(conv_u1);

	testUnaryNature(conv_i);
	testUnaryNumericOperands(conv_i);
	testConvResultType(conv_i);

	testUnaryNature(conv_ovf_i);
	testUnaryNumericOperands(conv_ovf_i);
	testConvResultType(conv_ovf_i);

	testUnaryNature(conv_ovf_u);
	testUnaryNumericOperands(conv_ovf_u);
	testConvResultType(conv_ovf_u);

	testUnaryNature(conv_u);
	testUnaryNumericOperands(conv_u);
	testConvResultType(conv_u);
}

#ifdef	__cplusplus
};
#endif
