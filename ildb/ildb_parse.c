/*
 * ildb_parse.c - Support for parsing numbers, identifiers, etc.
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

long ILDbParseNumber(ILDb *db, char *value, int len,
					 const char *name, long defValue)
{
	int sign;
	long result;

	/* Check for an empty value */
	if(len < 0)
	{
		len = (value ? strlen(value) : 0);
	}
	if(!value || !len)
	{
		ILDbError(db, "unspecified %s", name);
		return defValue;
	}

	/* Parse the sign */
	if(*value == '-')
	{
		sign = -1;
		++value;
		--len;
	}
	else if(*value == '+')
	{
		sign = 1;
		++value;
		--len;
	}
	else
	{
		sign = 1;
	}

	/* Parse the main part of the number */
	result = 0;
	if(len > 2 && *value == '0' && (value[1] == 'x' || value[1] == 'X'))
	{
		/* Parse a hexadecimal number */
		while(len > 0)
		{
			if(*value >= '0' && *value <= '9')
			{
				result = result * 16 + (long)(*value++ - '0');
			}
			else if(*value >= 'A' && *value <= 'F')
			{
				result = result * 16 + (long)(*value++ - 'A' + 10);
			}
			else if(*value >= 'a' && *value <= 'f')
			{
				result = result * 16 + (long)(*value++ - 'a' + 10);
			}
			else
			{
				break;
			}
			--len;
		}
	}
	else if(*value == '0')
	{
		/* Parse an octal number */
		while(len > 0 && *value >= '0' && *value <= '7')
		{
			result = result * 8 + (long)(*value++ - '0');
			--len;
		}
	}
	else
	{
		/* Parse a decimal number */
		while(len > 0 && *value >= '0' && *value <= '9')
		{
			result = result * 10 + (long)(*value++ - '0');
			--len;
		}
	}

	/* If we aren't at the end of the string, then this is an error */
	if(len > 0)
	{
		ILDbError(db, "invalid %s", name);
		return defValue;
	}

	/* Apply the sign and return */
	return result * (long)sign;
}

#ifdef	__cplusplus
};
#endif
