/*
 * ildb_utils.c - Debugger utility functions.
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

#include "ildb_context.h"
#include "ildb_utils.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

char **ILDbDupStringArray(ILDb *db, char **array, int len)
{
	int index;
	char **newArray;
	if(!array)
	{
		return 0;
	}
	if(len < 0)
	{
		len = 0;
		while(array[len] != 0)
		{
			++len;
		}
	}
	newArray = (char **)ILMalloc((len + 1) * sizeof(char *));
	if(!newArray)
	{
		ILDbOutOfMemory(db);
	}
	for(index = 0; index < len; ++index)
	{
		if(array[index] != 0)
		{
			if((newArray[index] = ILDupString(array[index])) == 0)
			{
				ILDbOutOfMemory(db);
			}
		}
		else
		{
			newArray[index] = 0;
		}
	}
	newArray[len] = 0;
	return newArray;
}

void ILDbFreeStringArray(char **array, int len)
{
	int index;
	if(!array)
	{
		return;
	}
	if(len >= 0)
	{
		for(index = 0; index < len; ++index)
		{
			if(array[index] != 0)
			{
				ILFree(array[index]);
			}
		}
	}
	else
	{
		index = 0;
		while(array[index] != 0)
		{
			ILFree(array[index]);
			++index;
		}
	}
	ILFree(array);
}

#ifdef	__cplusplus
};
#endif
