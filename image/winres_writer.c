/*
 * winres_writer.c - Write the data in the ".rsrc" section of an IL binary.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

#include "image.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILResourceSection *ILResourceSectionCreateWriter(ILImage *image)
{
	ILResourceSection *section;
	section = (ILResourceSection *)ILMalloc(sizeof(ILResourceSection));
	if(section)
	{
		section->image = image;
		section->data = 0;
		section->length = 0;
		section->rootDirectory = (ILResourceEntry *)
			ILMalloc(sizeof(ILResourceEntry));
		if(!(section->rootDirectory))
		{
			ILFree(section);
			return 0;
		}
		section->rootDirectory->isDirectory = 1;
		section->rootDirectory->isMallocData = 0;
		section->rootDirectory->isNumeric = 0;
		section->rootDirectory->name = 0;
		section->rootDirectory->nameLen = 0;
		section->rootDirectory->children = 0;
		section->rootDirectory->next = 0;
		section->rootDirectory->data = 0;
		section->rootDirectory->length = 0;
	}
	return section;
}

/*
 * Insert an entry into the resource tree, with a specific name.
 */
static void *InsertEntry(ILResourceEntry *parent, ILResourceEntry *prev,
						 const char *name)
{
	/* TODO */
	return 0;
}

void *ILResourceSectionAddEntry(ILResourceSection *section, const char *name)
{
	ILResourceEntry *parent;
	ILResourceEntry *entry;
	ILResourceEntry *prev;
	int posn, cmp;

	/* Search down the hierarchy for the specified entry */
	entry = section->rootDirectory;
	parent = entry;
	prev = 0;
	while(entry != 0)
	{
		if(*name == '/')
		{
			++name;
			continue;
		}
		else if(*name == '\0')
		{
			break;
		}
		posn = 0;
		while(name[posn] != '\0' && name[posn] != '/')
		{
			++posn;
		}
		parent = entry;
		entry = entry->children;
		prev = 0;
		while(entry != 0)
		{
			if(entry->nameLen == posn)
			{
				cmp = ILStrNICmp(entry->name, name, posn);
			}
			else if(entry->nameLen < posn)
			{
				cmp = ILStrNICmp(entry->name, name, entry->nameLen);
				if(!cmp)
				{
					cmp = -1;
				}
			}
			else
			{
				cmp = ILStrNICmp(entry->name, name, posn);
				if(!cmp)
				{
					cmp = 1;
				}
			}
			if(!cmp)
			{
				/* Go down a level: we already have this name */
				break;
			}
			else if(cmp < 0)
			{
				/* This is where we need to insert the new entry */
				return InsertEntry(parent, prev, name);
			}
			prev = entry;
			entry = entry->next;
		}
		if(!entry)
		{
			/* Insert the new entry at the end of the current level */
			return InsertEntry(parent, prev, name);
		}
		name += posn;
	}

	/* We already have a leaf entry with this name */
	return entry;
}

int ILResourceSectionAddBytes(void *entry, const void *buffer, int len)
{
	/* TODO */
	return 1;
}

void ILResourceSectionFlush(ILResourceSection *section, ILWriter *writer)
{
	/* TODO */
}

#ifdef	__cplusplus
};
#endif
