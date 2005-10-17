/*
 * SDMath.c - Math implementation.
 *
 * Copyright (C) 2005  Free Software Foundation, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "CMath.h"

#ifdef HAVE_MATH_H
	#include <math.h>
#else
	#error "Basic arithmetic functions (e.g. cos, sqrt) could not be found."
#endif

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL SDMATH SDDouble
SDMath_Cos(SDDouble radians)
{
	return cos(radians);
}

SDINTERNAL SDMATH SDDouble
SDMath_Sin(SDDouble radians)
{
	return sin(radians);
}

SDINTERNAL SDMATH SDDouble
SDMath_Sqrt(SDDouble value)
{
	return sqrt(value);
}

SDINTERNAL SDMATH SDDouble
SDMath_Erf(SDDouble value)
{
	return erf(value);
}


#ifdef __cplusplus
};
#endif
