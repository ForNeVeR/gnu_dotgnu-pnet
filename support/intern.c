/*
 * intern.c - Internalise strings within a global hash table.
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

#include <stdio.h>
#include "il_system.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * A hash table that maps strings to their intern'ed forms.
 */
typedef struct _InternEntry
{
	struct _InternEntry *next;
	int len;
	char str[1];

} InternEntry;
#define	INTERN_HASH_SIZE		2039
static InternEntry *hashTable[INTERN_HASH_SIZE];
static char emptyString[] = "";

ILIntString ILInternString(char *str, int len)
{
	unsigned long hash;
	InternEntry *entry;
	ILIntString result;

	/* Get the length of the string if not specified */
	if(len < 0)
	{
		len = strlen(str);
	}

	/* The empty string always hashes to the same value */
	if(!str || !len)
	{
		result.string = emptyString;
		result.len = 0;
		return result;
	}

	/* Hash the string */
	hash = (ILHashString(0, str, len) % INTERN_HASH_SIZE);

	/* Look in the hash table to see if we already have the string */
	entry = hashTable[hash];
	while(entry != 0)
	{
		if(len == entry->len && !ILMemCmp(entry->str, str, len))
		{
			result.string = entry->str;
			result.len = len;
			return result;
		}
		entry = entry->next;
	}

	/* Add a new item to the hash table */
	entry = (InternEntry *)ILMalloc(sizeof(InternEntry) + len);
	if(!entry)
	{
	#ifndef REDUCED_STDIO
		fprintf(stderr, "virtual memory exhausted - cannot intern \"");
		fwrite(str, 1, len, stderr);
		fprintf(stderr, "\"\n");
		exit(1);
	#endif
		result.string = emptyString;
		result.len = 0;
		return result;
	}
	entry->next = hashTable[hash];
	entry->len = len;
	ILMemCpy(entry->str, str, len);
	entry->str[len] = '\0';
	hashTable[hash] = entry;
	result.string = entry->str;
	result.len = len;
	return result;
}

ILIntString ILInternAppendedString(ILIntString str1, ILIntString str2)
{
	unsigned long hash;
	InternEntry *entry;
	ILIntString result;

	/* Bail out early for the easy cases */
	if(str1.len == 0)
	{
		return str2;
	}
	else if(str2.len == 0)
	{
		return str1;
	}

	/* Hash the combined string */
	hash = ILHashString(0, str1.string, str1.len);
	hash = ILHashString(hash, str2.string, str2.len);
	hash %= INTERN_HASH_SIZE;

	/* Look in the hash table to see if we already have the combined string */
	entry = hashTable[hash];
	while(entry != 0)
	{
		if((str1.len + str2.len) == entry->len &&
		   !ILMemCmp(entry->str, str1.string, str1.len) &&
		   !ILMemCmp(entry->str + str1.len, str2.string, str2.len))
		{
			result.string = entry->str;
			result.len = entry->len;
			return result;
		}
		entry = entry->next;
	}

	/* Add a new item to the hash table */
	entry = (InternEntry *)ILMalloc(sizeof(InternEntry) + str1.len + str2.len);
	if(!entry)
	{
	#ifndef REDUCED_STDIO
		fprintf(stderr, "virtual memory exhausted - cannot intern \"");
		fwrite(str1.string, 1, str1.len, stderr);
		fwrite(str2.string, 1, str2.len, stderr);
		fprintf(stderr, "\"\n");
		exit(1);
	#endif
		result.string = emptyString;
		result.len = 0;
		return result;
	}
	entry->next = hashTable[hash];
	entry->len = str1.len + str2.len;
	ILMemCpy(entry->str, str1.string, str1.len);
	ILMemCpy(entry->str + str1.len, str2.string, str2.len);
	entry->str[entry->len] = '\0';
	hashTable[hash] = entry;
	result.string = entry->str;
	result.len = entry->len;
	return result;
}

#ifdef	__cplusplus
};
#endif
