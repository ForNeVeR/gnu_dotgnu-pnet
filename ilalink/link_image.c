/*
 * link_image.c - Process object images within a linker context.
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

#include "linker.h"

#ifdef	__cplusplus
extern	"C" {
#endif

int ILLinkerAddImage(ILLinker *linker, ILImage *image, const char *filename)
{
	ILModule *thisModule;
	ILAssembly *thisAssem;
	ILModule *module;
	ILAssembly *assem;
	const ILUInt16 *version;
	const char *locale;
	const void *blob;
	unsigned long blobLen;
	ILUInt32 thisCompat;
	ILUInt32 compat;
	void *dataSection;
	unsigned long dataLen;

	/* Get the module and assembly for the final output image */
	thisModule = (ILModule *)ILImageTokenInfo(linker->image,
											  (IL_META_TOKEN_MODULE | 1));
	thisAssem = (ILAssembly *)ILImageTokenInfo(linker->image,
											   (IL_META_TOKEN_ASSEMBLY | 1));

	/* Copy module information from the image if not a C object file */
	if(ILLinkerCMemoryModel(image) == 0)
	{
		module = 0;
		while((module = (ILModule *)ILImageNextToken
					(image, IL_META_TOKEN_MODULE, module)) != 0)
		{
			if(!_ILLinkerConvertAttrs(linker, (ILProgramItem *)module,
									  (ILProgramItem *)thisModule))
			{
				return 0;
			}
		}
	}

	/* Copy assembly information from the image */
	assem = 0;
	while((assem = (ILAssembly *)ILImageNextToken
				(image, IL_META_TOKEN_ASSEMBLY, assem)) != 0)
	{
		/* Convert the assembly attributes */
		if(!_ILLinkerConvertAttrs(linker, (ILProgramItem *)assem,
								  (ILProgramItem *)thisAssem))
		{
			return 0;
		}

		/* Convert the assembly security declarations */
		if(!_ILLinkerConvertSecurity(linker, (ILProgramItem *)assem,
								     (ILProgramItem *)thisAssem))
		{
			return 0;
		}

		/* Copy the version information */
		version = ILAssemblyGetVersion(thisAssem);
		if(version[0] == 0 && version[1] == 0 &&
		   version[2] == 0 && version[3] == 0)
		{
			ILAssemblySetVersion(thisAssem, ILAssemblyGetVersion(assem));
		}

		/* Copy the locale information */
		if(ILAssembly_Locale(thisAssem) == 0)
		{
			locale = ILAssembly_Locale(assem);
			if(locale != 0)
			{
				if(!ILAssemblySetLocale(thisAssem, locale))
				{
					_ILLinkerOutOfMemory(linker);
					return 0;
				}
			}
		}

		/* Copy the originator information */
		if(!ILAssembly_HasPublicKey(thisAssem) &&
		   ILAssembly_HasPublicKey(assem))
		{
			blob = ILAssemblyGetOriginator(assem, &blobLen);
			if(blob)
			{
				if(!ILAssemblySetOriginator(thisAssem, blob, blobLen))
				{
					_ILLinkerOutOfMemory(linker);
					return 0;
				}
				ILAssemblySetAttrs(thisAssem, IL_META_ASSEM_PUBLIC_KEY,
								   IL_META_ASSEM_PUBLIC_KEY);
			}
		}

		/* Copy the compatibility mask if the new image is
		   more restrictive than what we have so far */
		thisCompat = (ILAssemblyGetAttrs(thisAssem) &
						IL_META_ASSEM_COMPATIBILITY_MASK);
		compat = (ILAssemblyGetAttrs(assem) &
						IL_META_ASSEM_COMPATIBILITY_MASK);
		if(compat > thisCompat)
		{
			ILAssemblySetAttrs(thisAssem, IL_META_ASSEM_COMPATIBILITY_MASK,
							   compat);
		}

		/* Copy the JIT flags */
		if(ILAssembly_EnableJITTracking(assem))
		{
			ILAssemblySetAttrs(thisAssem, IL_META_ASSEM_ENABLE_JIT_TRACKING,
							   IL_META_ASSEM_ENABLE_JIT_TRACKING);
		}
		if(ILAssembly_DisableJITOptimizer(assem))
		{
			ILAssemblySetAttrs(thisAssem, IL_META_ASSEM_DISABLE_JIT_OPTIMIZER,
							   IL_META_ASSEM_DISABLE_JIT_OPTIMIZER);
		}
	}

	/* Convert the classes in the image */
	if(!_ILLinkerConvertClasses(linker, image))
	{
		return 0;
	}

	/* Copy the contents of the ".sdata" and ".tls" sections */
	if(ILImageGetSection(image, IL_SECTION_DATA, &dataSection, &dataLen))
	{
		ILWriterOtherWrite(linker->writer, ".sdata", IL_IMAGESECT_SDATA,
						   dataSection, dataLen);
		linker->dataLength += dataLen;
	}
	if(ILImageGetSection(image, IL_SECTION_TLS, &dataSection, &dataLen))
	{
		ILWriterOtherWrite(linker->writer, ".tls", IL_IMAGESECT_TLS,
						   dataSection, dataLen);
		linker->tlsLength += dataLen;
	}

	/* Done */
	return 1;
}

int ILLinkerAddCObject(ILLinker *linker, ILImage *image,
					   const char *filename, int memoryModel)
{
	ILProgramItem *oldItem;
	ILProgramItem *newItem;

	/* Check that the memory model is consistent with the
	   values from previous object files that we linked */
	if(linker->memoryModel != 0)
	{
		if(linker->memoryModel != memoryModel)
		{
			fprintf(stderr,
					"%s: memory model is inconsistent with other objects\n",
					filename);
			linker->error = 1;
		}
	}
	else
	{
		/* This is the first C object, so set the memory model
		   locally and in the final image */
		linker->memoryModel = memoryModel;
		oldItem = ILProgramItem_FromToken(image, IL_META_TOKEN_MODULE | 1);
		newItem = ILProgramItem_FromToken
			(linker->image, IL_META_TOKEN_MODULE | 1);
		if(oldItem && newItem)
		{
			_ILLinkerConvertAttrs(linker, oldItem, newItem);
		}
	}

	/* Add the image to the linker */
	return ILLinkerAddImage(linker, image, filename);
}

#ifdef	__cplusplus
};
#endif
