/*
 * internal.c - Lookup routines for "internalcall" methods.
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

#include "engine.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Import the method tables of all internal classes.
 */
extern ILMethodTableEntry const _ILSystemObjectMethods;
extern ILMethodTableEntry const _ILSystemStringMethods;
extern ILMethodTableEntry const _ILSystemArrayMethods;
extern ILMethodTableEntry const _ILSystemDecimalMethods;
extern ILMethodTableEntry const _ILPlatformStdioMethods;

/*
 * Table that contains all classes that have "internalcall" methods.
 */
typedef struct
{
	const char *namespace;
	const char *name;
	const ILMethodTableEntry *entry;

} InternalClassInfo;
static InternalClassInfo const internalClassTable[] = {
	{"System",		"Object",		&_ILSystemObjectMethods},
	{"System",		"String",		&_ILSystemStringMethods},
	{"System",		"Array",		&_ILSystemArrayMethods},
	{"System",		"Decimal",		&_ILSystemDecimalMethods},
	{"Platform",	"Stdio",		&_ILPlatformStdioMethods},
};
#define	numInternalClasses	(sizeof(internalClassTable) / \
							 sizeof(InternalClassInfo))

/* Import from "lib_array.c" */
void *_ILGetInternalMArray(ILMethod *method, int *isCtor);

void *_ILFindInternalCall(ILMethod *method, int ctorAlloc)
{
	ILImage *image;
	ILImage *systemImage;
	ILClass *owner;
	const char *name;
	const char *namespace;
	int index;
	const ILMethodTableEntry *entry;
	ILType *signature;
	void *func;
	int isCtor;

	/* If the method is not in the system image, then this is
	   probably an attempt to circumvent system security, which
	   we don't allow */
	image = ILProgramItem_Image(method);
	systemImage = ILContextGetSystem(ILImageToContext(image));
	if(systemImage != 0 && image != systemImage)
	{
		return 0;
	}

	/* Find the method's owner and bail out if no namespace
	   (all of the internalcall methods are under "System") */
	owner = ILMethod_Owner(method);
	namespace = ILClass_Namespace(owner);
	if(!namespace)
	{
		return 0;
	}
	name = ILClass_Name(owner);

	/* Search for the class's internalcall table */
	for(index = 0; index < numInternalClasses; ++index)
	{
		if(!strcmp(name, internalClassTable[index].name) &&
		   !strcmp(namespace, internalClassTable[index].namespace))
		{
			/* Search for the method within the class's table */
			entry = internalClassTable[index].entry;
			name = ILMethod_Name(method);
			signature = ILMethod_Signature(method);
			while(entry->methodName != 0)
			{
				if(!strcmp(entry->methodName, name) &&
				   entry->signature != 0 &&
				   _ILLookupTypeMatch(signature, entry->signature))
				{
					if(ctorAlloc && entry[1].methodName &&
					   !(entry[1].signature))
					{
						return entry[1].func;
					}
					else
					{
						return entry->func;
					}
				}
				++entry;
			}
			return 0;
		}
	}

	/* Perhaps this is a "runtime" method for a multi-dimensional array? */
	func = _ILGetInternalMArray(method, &isCtor);
	if(func)
	{
		if(isCtor)
		{
			/* Multi-dimensional arrays only have allocation constructors */
			if(ctorAlloc)
			{
				return func;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			/* This is a regular method */
			return func;
		}
	}

	/* The class does not have internalcall methods */
	return 0;
}

#ifdef	__cplusplus
};
#endif
