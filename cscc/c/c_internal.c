/*
 * c_internal.c - Internal definitions for the C compiler front end.
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

#include <cscc/c/c_internal.h>

#ifdef	__cplusplus
extern	"C" {
#endif

int gen_32bit_only;
ILScope *CCurrentScope;
CSemValue CSemValueDefault = {C_SEMKIND_VOID, ILType_Void, 0};
CSemValue CSemValueBool = {C_SEMKIND_RVALUE | C_SEMKIND_BOOLEAN,
						   ILType_Boolean, 0};
CSemValue CSemValueError = {C_SEMKIND_ERROR, ILType_Void, 0};


void *CSemDupExtra(const void *buf, unsigned int len)
{
	void *temp = yynodealloc(len);
	ILMemCpy(temp, buf, len);
	return temp;
}

#ifdef	__cplusplus
};
#endif
