/*
 * context.c - Utility routines for manipulting IL contexts.
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

#include "program.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Hash a string, while ignoring case.
 */
static unsigned long HashIgnoreCase(unsigned long start,
									const char *str, int len)
{
	unsigned long hash = start;
	int ch;
	while(len > 0)
	{
		ch = (*str++ & 0xFF);
		if(ch >= 'A' && ch <= 'Z')
		{
			hash = (hash << 5) + hash + (unsigned long)(ch - 'A' + 'a');
		}
		else
		{
			hash = (hash << 5) + hash + (unsigned long)ch;
		}
		--len;
	}
	return hash;
}

/*
 * Compute the hash value for a class.
 */
static unsigned long ClassHash_Compute(const ILClass *classInfo)
{
	unsigned long hash;
	if(classInfo->namespace)
	{
		hash = HashIgnoreCase(0, classInfo->namespace,
							  strlen(classInfo->namespace));
		hash = HashIgnoreCase(hash, ".", 1);
	}
	else
	{
		hash = 0;
	}
	return HashIgnoreCase(hash, classInfo->name, strlen(classInfo->name));
}

/*
 * Compute the hash value for a class key.
 */
static unsigned long ClassHash_KeyCompute(const ILClassKeyInfo *key)
{
	unsigned long hash;
	if(key->namespace)
	{
		hash = HashIgnoreCase(0, key->namespace, key->namespaceLen);
		hash = HashIgnoreCase(hash, ".", 1);
	}
	else
	{
		hash = 0;
	}
	return HashIgnoreCase(hash, key->name, key->nameLen);
}

/*
 * Match a hash table element against a supplied key.
 */
static int ClassHash_Match(const ILClass *classInfo, const ILClassKeyInfo *key)
{
	/* Match the namespace */
	if(classInfo->namespace)
	{
		if(!(key->namespace))
		{
			return 0;
		}
		if(strncmp(classInfo->namespace, key->namespace,
				   key->namespaceLen) != 0 ||
		   classInfo->namespace[key->namespaceLen] != '\0')
		{
			return 0;
		}
	}

	/* Match the name */
	if(strncmp(classInfo->name, key->name, key->nameLen) != 0 ||
	   classInfo->name[key->nameLen] != '\0')
	{
		return 0;
	}

	/* Match the scope */
	if(key->scope && key->scope != classInfo->scope)
	{
		return 0;
	}

	/* Match the image */
	if(key->image && key->image != classInfo->programItem.image)
	{
		return 0;
	}

	/* Do we only want types at the global level? */
	if(key->wantGlobal)
	{
		if((classInfo->scope->token & IL_META_TOKEN_MASK) !=
					IL_META_TOKEN_MODULE)
		{
			return 0;
		}
	}

	/* We have a match */
	return 1;
}

/* Defined in "synthetic.c" */
int _ILContextSyntheticInit(ILContext *context);

ILContext *ILContextCreate(void)
{
	ILContext *context = (ILContext *)ILCalloc(1, sizeof(ILContext));
	if(context)
	{
		if((context->classHash = ILHashCreate
					(IL_CONTEXT_HASH_SIZE,
				     (ILHashComputeFunc)ClassHash_Compute,
					 (ILHashKeyComputeFunc)ClassHash_KeyCompute,
					 (ILHashMatchFunc)ClassHash_Match,
					 (ILHashFreeFunc)0)) == 0)
		{
			ILFree(context);
			return 0;
		}
		ILMemPoolInitType(&(context->typePool), ILType, 0);
	}
	if(!_ILContextSyntheticInit(context))
	{
		ILContextDestroy(context);
		return 0;
	}
	return context;
}

void ILContextDestroy(ILContext *context)
{
	/* Destroy the images */
	while(context->firstImage != 0)
	{
		ILImageDestroy(context->firstImage);
	}

	/* Destroy the class hash */
	ILHashDestroy(context->classHash);

	/* Destroy the synthetic types hash */
	ILHashDestroy(context->syntheticHash);

	/* Destroy the type pool */
	ILMemPoolDestroy(&(context->typePool));

	/* Destroy the context itself */
	ILFree(context);
}

const char *_ILContextPersistString(ILImage *image, const char *str)
{
	if(str)
	{
		if(image->type == IL_IMAGETYPE_BUILDING)
		{
			/* We need to create a persistent version in the "#Strings" blob */
			unsigned long offset;
			offset = ILImageAddString(image, str);
			if(offset)
			{
				return ILImageGetString(image, offset);
			}
		}
		else
		{
			/* The string is assumed to already be persistent */
			return str;
		}
	}
	return 0;
}

const char *_ILContextPersistMalloc(ILImage *image, char *str)
{
	if(str)
	{
		ILUInt32 len = sizeof(ILStringBlock) + strlen(str) + 1;
		ILStringBlock *block = (ILStringBlock *)ILMalloc(len);
		if(!block)
		{
			ILFree(str);
			return 0;
		}
		block->next = image->extraStrings;
		image->extraStrings = block;
		block->used = len;
		block->len = len;
		strcpy((char *)(block + 1), str);
		ILFree(str);
		return (const char *)(block + 1);
	}
	return 0;
}

static ILImage *GetImageByName(ILContext *context, const char *name,
							   unsigned long tokenType, int offset)
{
	ILImage *image;
	ILUInt32 values[IL_IMAGE_TOKEN_COLUMNS];
	unsigned long numTokens;
	unsigned long token;
	const char *imageName;

	image = context->firstImage;
	while(image != 0)
	{
		numTokens = ILImageNumTokens(image, tokenType);
		for(token = 1; token <= numTokens; ++token)
		{
			if(_ILImageRawTokenData(image, tokenType | token, values))
			{
				imageName = ILImageGetString(image, values[offset]);
				if(imageName && !ILStrICmp(imageName, name))
				{
					return image;
				}
			}
		}
		image = image->nextImage;
	}

	return 0;
}

ILImage *ILContextGetModule(ILContext *context, const char *name)
{
	return GetImageByName(context, name, IL_META_TOKEN_MODULE,
						  IL_OFFSET_MODULE_NAME);
}

ILImage *ILContextGetAssembly(ILContext *context, const char *name)
{
	return GetImageByName(context, name, IL_META_TOKEN_ASSEMBLY,
						  IL_OFFSET_ASSEMBLY_NAME);
}

ILImage *ILContextNextImage(ILContext *context, ILImage *image)
{
	if(image)
	{
		return image->nextImage;
	}
	else
	{
		return context->firstImage;
	}
}

ILImage *ILContextGetSynthetic(ILContext *context)
{
	ILImage *image;

	/* If we already have a synthetic types image, then return it */
	if(context->syntheticImage)
	{
		return context->syntheticImage;
	}

	/* Create a new image */
	image = ILImageCreate(context);
	if(!image)
	{
		return 0;
	}

	/* Create the "Module" record for the image */
	if(!ILModuleCreate(image, 0, "$Synthetic", 0))
	{
		ILImageDestroy(image);
		return 0;
	}

	/* Create the "Assembly" record for the image */
	if(!ILAssemblyCreate(image, 0, "$Synthetic", 0))
	{
		ILImageDestroy(image);
		return 0;
	}

	/* Create the main "<Module>" type for the image */
	if(!ILClassCreate(ILClassGlobalScope(image), 0, "<Module>", 0, 0))
	{
		ILImageDestroy(image);
		return 0;
	}

	/* Attach the synthetic image to the context and return */
	context->syntheticImage = image;
	return image;
}

void ILContextSetSystem(ILContext *context, ILImage *image)
{
	context->systemImage = image;
}

ILImage *ILContextGetSystem(ILContext *context)
{
	return context->systemImage;
}

void ILContextSetLibraryDirs(ILContext *context,
							 char **libraryDirs,
							 int numLibraryDirs)
{
	context->libraryDirs = libraryDirs;
	context->numLibraryDirs = numLibraryDirs;
}

#ifdef	__cplusplus
};
#endif
