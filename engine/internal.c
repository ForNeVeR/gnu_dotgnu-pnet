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
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Import the method tables of all internal classes.
 */
#include "int_table.c"

/* Import from "lib_array.c" */
void *_ILGetInternalArray(ILMethod *method, int *isCtor);

void *_ILFindInternalCall(ILMethod *method, int ctorAlloc)
{
	ILImage *image;
	ILClass *owner;
	const char *name;
	const char *namespace;
	int left, right, middle;
	const ILMethodTableEntry *entry;
	ILType *signature;
	void *func;
	int isCtor;
	int cmp;

	/* If the method is not in a secure image, then this is
	   probably an attempt to circumvent system security, which
	   we don't allow */
	image = ILProgramItem_Image(method);
	if(!ILImageIsSecure(image))
	{
		goto arraysOnly;
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
	left = 0;
	right = numInternalClasses - 1;
	while(left <= right)
	{
		middle = (left + right) / 2;
		cmp = strcmp(name, internalClassTable[middle].name);
		if(!cmp)
		{
			if(!strcmp(namespace, internalClassTable[middle].namespace))
			{
				/* Search for the method within the class's table */
				entry = internalClassTable[middle].entry;
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
			}
			return 0;
		}
		else if(cmp < 0)
		{
			right = middle - 1;
		}
		else
		{
			left = middle + 1;
		}
	}

	/* Perhaps this is a "runtime" method for an array? */
arraysOnly:
	func = _ILGetInternalArray(method, &isCtor);
	if(func)
	{
		if(isCtor)
		{
			/* Arrays only have allocation constructors */
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

/*
 * Find an "internalcall" method from its address, which is
 * useful when debugging the virtual machine.
 */
const ILMethodTableEntry *_ILFindInternalByAddr(void *addr,
												const char **className)
{
	int index;
	const ILMethodTableEntry *entry;
	for(index = 0; index < numInternalClasses; ++index)
	{
		entry = internalClassTable[index].entry;
		while(entry->methodName != 0)
		{
			if(entry->func == addr)
			{
				*className = internalClassTable[index].name;
				return entry;
			}
			++entry;
		}
	}
	return 0;
}

#ifdef	__cplusplus
};
#endif
