/*
 * link_module.c - Module linking routines for C-style applications.
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

#include "linker.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Determine if we have a match between two namespaces.
 */
static int NamespaceMatch(const char *nspace1, const char *nspace2)
{
	if(!nspace1)
	{
		return (nspace2 == 0);
	}
	else if(!nspace2)
	{
		return 0;
	}
	else
	{
		return !strcmp(nspace1, nspace2);
	}
}

ILAttribute *ILLinkerFindAttribute(ILProgramItem *item,
								   const char *name,
								   const char *namespace,
								   ILType *arg1Type,
								   ILType *arg2Type)
{
	ILAttribute *attr = 0;
	ILMethod *ctor;
	ILClass *attrClass;
	ILType *signature;
	while((attr = ILProgramItemNextAttribute(item, attr)) != 0)
	{
		ctor = ILProgramItemToMethod(ILAttributeTypeAsItem(attr));
		if(ctor)
		{
			attrClass = ILMethod_Owner(ctor);
			if(!strcmp(ILClass_Name(attrClass), name))
			{
				if(NamespaceMatch(ILClass_Namespace(attrClass), namespace))
				{
					signature = ILMethod_Signature(ctor);
					if(arg1Type && arg2Type)
					{
						if(ILTypeNumParams(signature) == 2 &&
						   ILTypeIdentical(ILTypeGetParam(signature, 1),
						   				   arg1Type) &&
						   ILTypeIdentical(ILTypeGetParam(signature, 2),
						   				   arg2Type))
						{
							return attr;
						}
					}
					else if(arg1Type)
					{
						if(ILTypeNumParams(signature) == 1 &&
						   ILTypeIdentical(ILTypeGetParam(signature, 1),
						   				   arg1Type))
						{
							return attr;
						}
					}
					else
					{
						if(ILTypeNumParams(signature) == 0)
						{
							return attr;
						}
					}
				}
			}
		}
	}
	return 0;
}

ILSerializeReader *ILLinkerReadAttribute(ILAttribute *attr)
{
	const void *blob;
	unsigned long blobLen;
	blob = ILAttributeGetValue(attr, &blobLen);
	if(!blob || !blobLen)
	{
		return 0;
	}
	else
	{
		return ILSerializeReaderInit
			(ILProgramItemToMethod(ILAttributeTypeAsItem(attr)), blob, blobLen);
	}
}

int ILLinkerCMemoryModel(ILImage *image)
{
	ILModule *module;
	ILAttribute *attr;
	ILSerializeReader *reader;
	int model;

	/* Find the main module within this assembly */
	module = ILModule_FromToken(image, IL_META_TOKEN_MODULE | 1);
	if(!module)
	{
		return 0;
	}

	/* Check the module to see if it has the "MemoryModel" attribute */
	attr = ILLinkerFindAttribute
			(ILToProgramItem(module), "MemoryModelAttribute", "OpenSystem.C",
			 ILType_Int32, ILType_Invalid);
	if(!attr)
	{
		return 0;
	}

	/* Read the memory model value from the attribute */
	reader = ILLinkerReadAttribute(attr);
	if(!reader)
	{
		return 0;
	}
	model = 0;
	if(ILSerializeReaderGetParamType(reader) == IL_META_ELEMTYPE_I4)
	{
		model = (int)ILSerializeReaderGetInt32(reader, IL_META_ELEMTYPE_I4);
	}
	ILSerializeReaderDestroy(reader);
	return model;
}

void _ILLinkerTagAsC(ILLinker *linker, int model)
{
}

#ifdef	__cplusplus
};
#endif
