/*
 * numformat.c - Number formatting support
 *
 * Copyright (C) 2002  Free Software Foundation, Inc,
 *
 * Contributed by Rich Baumann
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

#include <stdlib.h>
#include <stdio.h>
#include "il_config.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

char *ILFormatReal(double value, char *format, int buflen)
{
	char *retval = malloc(sizeof(char)*buflen);
#ifdef HAVE_SNPRINTF
	snprintf(retval,buflen,format,value);
#else
	sprintf(retval,format,value);
#endif
	return retval;
}

#ifdef	__cplusplus
};
#endif
