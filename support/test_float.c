/*
 * test_float.c - Test floating point values for NaN or finiteness.
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

#include "il_values.h"
#ifdef HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif
#ifdef IL_NATIVE_WIN32
#include <float.h>
#define isnan(value)	_isnan((value))
#define HAVE_ISNAN 1
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

int ILNativeFloatIsNaN(ILNativeFloat value)
{
#ifdef HAVE_ISNAN
	return isnan(value);
#else
	return (value != value);
#endif
}

int ILNativeFloatIsFinite(ILNativeFloat value)
{
#ifdef HAVE_FINITE
	return finite(value);
#else
#if defined(HAVE_ISNAN) && defined(HAVE_ISINF)
	return (!isnan(value) && isinf(value) == 0);
#else
	#error "Don't know how to determine if floating point numbers are finite"
	return 1;
#endif
#endif
}

#ifdef	__cplusplus
};
#endif
