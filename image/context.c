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

#include "image.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILContext *ILContextCreate(void)
{
	ILContext *context = (ILContext *)ILCalloc(1, sizeof(ILContext));
	if(context)
	{
		ILMemPoolInitType(&(context->typePool), ILType, 0);
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

#ifdef	__cplusplus
};
#endif
