/*
 * ildasm_attrs.c - Dump custom attributes.
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

#include "ildasm_internal.h"

#ifdef	__cplusplus
extern	"C" {
#endif

void ILDAsmDumpCustomAttrs(ILImage *image, FILE *outstream, int flags,
						   int indent, ILProgramItem *item)
{
	ILAttribute *attr = 0;
	ILProgramItem *type;
	const void *value;
	unsigned long valueLen;
	ILClass *classInfo;
	ILMethod *method;
	ILTypeSpec *spec;
	ILType *rawType;

	while((attr = ILProgramItemNextAttribute(item, attr)) != 0)
	{
		/* Output the ".custom" header */
		if(indent == 1)
		{
			fputs("\t", outstream);
		}
		else if(indent == 2)
		{
			fputs("\t\t", outstream);
		}
		fputs(".custom ", outstream);
		if((flags & IL_DUMP_SHOW_TOKENS) != 0)
		{
			fprintf(outstream, "/*%08lX*/ ",
					(long)(ILProgramItem_Token(attr)));
		}

		/* Output the type */
		if(ILAttributeTypeIsItem(attr))
		{
			type = ILAttributeTypeAsItem(attr);
			if((spec = ILProgramItemToTypeSpec(type)) != 0)
			{
				rawType = ILTypeSpec_Type(spec);
				ILDumpType(outstream, image, rawType, flags);
			}
			else if((classInfo = ILProgramItemToClass(type)) != 0)
			{
				ILDumpClassName(outstream, image, classInfo, flags);
			}
			else if((method = ILProgramItemToMethod(type)) != 0)
			{
				ILDumpMethodType(outstream, image,
								 ILMethod_Signature(method), flags,
					 			 ILMethod_Owner(method),
								 ILMethod_Name(method),
								 method);
			}
			else
			{
				fputs("UNKNOWNTYPE", outstream);
			}
		}
		else
		{
			fputs("STRING", outstream);
		}

		/* Output the value */
		if((value = ILAttributeGetValue(attr, &valueLen)) != 0)
		{
			fputs(" =", outstream);
			ILDAsmDumpBinaryBlob(outstream, image, value, valueLen);
		}

		/* Terminate the line */
		putc('\n', outstream);
	}
}

#ifdef	__cplusplus
};
#endif
