/*
 * meta_build.c - Build metadata structures from a metadata index.
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
#include <stdio.h>

#ifdef	__cplusplus
extern	"C" {
#endif

#define	EXIT_IF_ERROR(call)	\
			do { \
				error = (call); \
				if(error != 0) \
				{ \
					return error; \
				} \
			} while (0)

typedef int (*TokenLoadFunc)(ILImage *image, ILUInt32 *values,
			  			     ILUInt32 *valuesNext, ILToken token,
							 void *userData);

/*
 * Load all tokens in a particular table.
 */
static int LoadTokens(ILImage *image, ILToken tokenType,
					  TokenLoadFunc func, void *userData)
{
	ILToken maxToken;
	ILToken token;
	ILUInt32 values[IL_IMAGE_TOKEN_COLUMNS];
	ILUInt32 valuesNext[IL_IMAGE_TOKEN_COLUMNS];
	ILUInt32 *currValues;
	ILUInt32 *nextValues;
	ILUInt32 *tempValues;
	int error;

	/* How many tokens of this type are there? */
	maxToken = (ILToken)(ILImageNumTokens(image, tokenType));
	if(!maxToken)
	{
		/* There are no tokens, so nothing needs to be done */
		return 0;
	}

	/* Parse and load the tokens */
	maxToken |= tokenType;
	if(!_ILImageRawTokenData(image, tokenType + 1, values))
	{
		return IL_LOADERR_BAD_META;
	}
	currValues = values;
	nextValues = valuesNext;
	for(token = tokenType + 1; token <= maxToken; ++token)
	{
		if((token + 1) <= maxToken)
		{
			if(!_ILImageRawTokenData(image, token + 1, nextValues))
			{
				return IL_LOADERR_BAD_META;
			}
			error = (*func)(image, currValues, nextValues, token, userData);
			if(error != 0)
			{
				return error;
			}
			tempValues = currValues;
			currValues = nextValues;
			nextValues = tempValues;
		}
		else
		{
			error = (*func)(image, currValues, 0, token, userData);
			if(error != 0)
			{
				return error;
			}
		}
	}

	/* Done */
	return 0;
}

/*
 * Load all tokens in a particular range within a table.
 */
static int LoadTokenRange(ILImage *image, unsigned long tokenType,
					      unsigned long firstToken, unsigned long num,
						  TokenLoadFunc func, void *userData)
{
	unsigned long maxToken;
	unsigned long tableMaxToken;
	unsigned long token;
	ILUInt32 values[IL_IMAGE_TOKEN_COLUMNS];
	ILUInt32 valuesNext[IL_IMAGE_TOKEN_COLUMNS];
	ILUInt32 *currValues;
	ILUInt32 *nextValues;
	ILUInt32 *tempValues;
	int error;

	/* Bail out now if nothing to load */
	if(!num)
	{
		return 0;
	}

	/* Parse and load the tokens */
	maxToken = firstToken + num - 1;
	tableMaxToken = image->tokenCount[tokenType >> 24] | tokenType;
	if(!_ILImageRawTokenData(image, firstToken, values))
	{
		return IL_LOADERR_BAD_META;
	}
	currValues = values;
	nextValues = valuesNext;
	for(token = firstToken; token <= maxToken; ++token)
	{
		if((token + 1) <= tableMaxToken)
		{
			if(!_ILImageRawTokenData(image, token + 1, nextValues))
			{
				return IL_LOADERR_BAD_META;
			}
			error = (*func)(image, currValues, nextValues, token, userData);
			if(error != 0)
			{
				return error;
			}
			tempValues = currValues;
			currValues = nextValues;
			nextValues = tempValues;
		}
		else
		{
			error = (*func)(image, currValues, 0, token, userData);
			if(error != 0)
			{
				return error;
			}
		}
	}

	/* Done */
	return 0;
}

#if IL_DEBUG_META

/*
 * Report an error that occurred while attempting to resolve a type.
 */
static void ReportResolveError(ILImage *image, ILToken token,
							   const char *name, const char *namespace)
{
	const char *importName;
	const char *importName2;

	/* Try to print scope information when reporting the error */
	switch(token & IL_META_TOKEN_MASK)
	{
		case IL_META_TOKEN_MODULE_REF:
		{
			/* The type was imported from a module scope */
			ILModule *module = ILModule_FromToken(image, token);
			if(module)
			{
				importName = ILModuleGetName(module);
				fprintf(stderr, "unresolved type: [.module %s]%s%s%s\n",
						importName, (namespace ? namespace : ""),
						(namespace ? "." : ""), name);
				return;
			}
		}
		break;

		case IL_META_TOKEN_ASSEMBLY_REF:
		{
			/* The type was imported from an assembly scope */
			ILAssembly *assem = ILAssembly_FromToken(image, token);
			if(assem)
			{
				importName = ILAssemblyGetName(assem);
				fprintf(stderr, "unresolved type: [%s]%s%s%s\n",
						importName, (namespace ? namespace : ""),
						(namespace ? "." : ""), name);
				return;
			}
		}
		break;

		case IL_META_TOKEN_TYPE_REF:
		{
			/* The type was nested within another type */
			ILClass *classInfo = ILClass_FromToken(image, token);
			if(classInfo)
			{
				importName = ILClass_Name(classInfo);
				importName2 = ILClass_Namespace(classInfo);
				if(importName2 && importName2 == '\0')
				{
					importName2 = 0;
				}
				fprintf(stderr, "unresolved type: %s%s%s/%s%s%s\n",
						(importName2 ? importName2 : ""),
						(importName2 ? "." : ""), importName,
						(namespace ? namespace : ""),
						(namespace ? "." : ""), name);
				return;
			}
		}
		break;
	}

	/* Leave off the scope */
	fprintf(stderr, "unresolved type: %s%s%s\n",
			(namespace ? namespace : ""), (namespace ? "." : ""), name);
}

#endif

/*
 * Determine if a scope refers to a module, or a parent
 * class in a nesting relationship that is also in a module.
 */
static int ScopeIsModule(ILProgramItem *scope)
{
	ILToken token;
	for(;;)
	{
		token = ILProgramItem_Token(scope);
		if((token & IL_META_TOKEN_MASK) == IL_META_TOKEN_MODULE)
		{
			return 1;
		}
		else if((token & IL_META_TOKEN_MASK) != IL_META_TOKEN_TYPE_REF)
		{
			break;
		}
		scope = ILClassGetScope((ILClass *)scope);
	}
	return 0;
}

/*
 * Resolve the TypeRef's.  Returns zero if OK, -1 if a
 * second-phase scan is needed for TypeRef's to this module.
 * Returns a load error otherwise.
 */
static int ResolveTypeRefs(ILImage *image, int loadFlags)
{
	ILUInt32 values[IL_IMAGE_TOKEN_COLUMNS];
	unsigned long maxToken;
	unsigned long token;
	const char *name;
	const char *namespace;
	void **table;
	int error;
	int needPhase2;
	ILProgramItem *scope;
	ILClass *info;
	ILClass *importInfo;
	ILImage *importImage;
	ILProgramItem *importScope;

	/* Create the TypeRef table for this image */
	maxToken = ILImageNumTokens(image, IL_META_TOKEN_TYPE_REF);
	if(!maxToken)
	{
		/* There are no TypeRef's, so nothing needs to be done */
		return 0;
	}
	if((table = (void **)ILCalloc(maxToken, sizeof(void *))) == 0)
	{
		/* Not enough memory to create the TypeRef table */
		return IL_LOADERR_MEMORY;
	}
	image->tokenData[IL_META_TOKEN_TYPE_REF >> 24] = table;

	/* Walk the TypeRef table to resolve external references */
	error = 0;
	needPhase2 = 0;
	maxToken |= IL_META_TOKEN_TYPE_REF;
	for(token = (IL_META_TOKEN_TYPE_REF | 1); token <= maxToken; ++token)
	{
		/* Parse the token information */
		if(!_ILImageRawTokenData(image, token, values))
		{
			error = IL_LOADERR_BAD_META;
			continue;
		}

		/* Get the TypeRef name and namespace */
		name = ILImageGetString(image, values[IL_OFFSET_TYPEREF_NAME]);
		namespace = ILImageGetString
						(image, values[IL_OFFSET_TYPEREF_NAMESPACE]);
		if(namespace && *namespace == '\0')
		{
			namespace = 0;
		}

		/* Determine which scope to use to resolve the TypeRef */
		if(values[IL_OFFSET_TYPEREF_SCOPE] != 0)
		{
			scope = ILProgramItem_FromToken
				(image, values[IL_OFFSET_TYPEREF_SCOPE]);
		}
		else
		{
			scope = ILClassGlobalScope(image);
		}
		if(!scope)
		{
			META_VAL_ERROR("invalid scope for type reference");
			error = IL_LOADERR_BAD_META;
			continue;
		}

		/* If we already have a reference, then reuse it */
		info = ILClassLookup(scope, name, namespace);
		if(info)
		{
			table[(token & ~IL_META_TOKEN_MASK) - 1] = (void *)info;
			continue;
		}

		/* Create a new type reference */
		info = ILClassCreateRef(scope, token, name, namespace);
		if(!info)
		{
			return IL_LOADERR_MEMORY;
		}
		table[(token & ~IL_META_TOKEN_MASK) - 1] = (void *)info;

		/* If the scope is the current module, then we need to
		   perform the rest of the resolution during phase 2 */
		if(ScopeIsModule(scope))
		{
			needPhase2 = 1;
			continue;
		}

		/* If the "no resolve" flag is set, then there is nothing more to do */
		if((loadFlags & IL_LOADFLAG_NO_RESOLVE) != 0)
		{
			continue;
		}

		/* Attempt to resolve references to other images */
		importInfo = 0;
		switch(ILProgramItem_Token(scope) & IL_META_TOKEN_MASK)
		{
			case IL_META_TOKEN_TYPE_REF:
			{
				/* Nested type within a foreign assembly */
				importScope = _ILProgramItemLinkedTo(scope);
				if(importScope)
				{
					importInfo = ILClassLookup
						(importScope, name, namespace);
				}
			}
			break;

			case IL_META_TOKEN_MODULE_REF:
			{
				/* Module reference imports are not currently
				   supported.  Types should be imported from
				   assemblies, not modules.  Module references
				   should only be used for PInvoke imports */
			}
			break;

			case IL_META_TOKEN_ASSEMBLY_REF:
			{
				/* Type is imported from a foreign assembly */
				importImage = ILAssemblyToImage((ILAssembly *)scope);
				if(importImage)
				{
					importScope = ILClassGlobalScope(importImage);
					if(importScope)
					{
						importInfo = ILClassLookup
							(importScope, name, namespace);
					}
				}
			}
			break;
		}
		if(importInfo)
		{
			/* Link "info" to "importInfo" */
			if(!_ILProgramItemLink(&(info->programItem),
								   &(importInfo->programItem)))
			{
				return IL_LOADERR_MEMORY;
			}
		}
		else
		{
			/* Could not resolve the type */
		#if IL_DEBUG_META
			ReportResolveError(image, values[IL_OFFSET_TYPEREF_SCOPE],
							   name, namespace);
		#endif
			error = IL_LOADERR_UNRESOLVED;
		}
	}

	/* Done */
	return (error ? error : (needPhase2 ? -1 : 0));
}

/*
 * Resolve the TypeRef's using the phase 2 algorithm.
 * This is used to fix up references to the current module
 * after the TypeDef table has been loaded.
 */
static int ResolveTypeRefsPhase2(ILImage *image, int loadFlags)
{
	unsigned long maxToken;
	unsigned long token;
	void **table;
	int error;
	ILClass *info;
	ILClass *importInfo;
	ILProgramItem *scope;

	/* Get the TypeRef table for this image */
	table = image->tokenData[IL_META_TOKEN_TYPE_REF >> 24];

	/* Walk the TypeRef table to resolve same-module references */
	error = 0;
	maxToken = ILImageNumTokens(image, IL_META_TOKEN_TYPE_REF) |
			   IL_META_TOKEN_TYPE_REF;
	for(token = (IL_META_TOKEN_TYPE_REF | 1); token <= maxToken; ++token)
	{
		/* Skip this token if it does not have module scope */
		info = (ILClass *)(table[(token & ~IL_META_TOKEN_MASK) - 1]);
		scope = (info ? info->scope : 0);
		if(!info || !ScopeIsModule(scope))
		{
			continue;
		}

		/* If the type is no longer a reference, it has been fixed up */
		if(!ILClassIsRef(info))
		{
			continue;
		}

		/* If we get here, then we have a reference to the current
		   module for a type that doesn't exist in the current module.
		   The assembler uses this in object files to mark classes
		   that are imported from other object files, but not from
		   the system library.  The linker normally fixes these up,
		   but we may see them if we are loading an object file */

		/* If we aren't resolving references, then assume that this
		   is an object file and that the dangling reference is OK */
		if((loadFlags & IL_LOADFLAG_NO_RESOLVE) != 0)
		{
			continue;
		}

		/* If the class is linked, then we already resolved it,
		   but it is present twice in the file for some reason */
		if(_ILProgramItemLinkedTo(&(info->programItem)) != 0)
		{
			continue;
		}

		/* Is this a global or a nested class? */
		if(!ILClassIsNestingScope(scope))
		{
			/* Global class: look in any image for the type */
			importInfo = ILClassLookupGlobal(image->context,
											 info->name, info->namespace);
		}
		else
		{
			/* Nested class: look within the parent scope */
			scope = _ILProgramItemLinkedTo(scope);
			if(scope)
			{
				importInfo = ILClassLookup(scope, info->name, info->namespace);
			}
			else
			{
				importInfo = 0;
			}
		}

		/* Link "info" to "importInfo" */
		if(importInfo)
		{
			if(!_ILProgramItemLink(&(info->programItem),
								   &(importInfo->programItem)))
			{
				return IL_LOADERR_MEMORY;
			}
		}
		else
		{
		#if IL_DEBUG_META
			ReportResolveError(image, 0, info->name, info->namespace);
		#endif
			error = IL_LOADERR_UNRESOLVED;
		}
	}

	/* Done */
	return error;
}

/*
 * Load a module token.
 */
static int Load_Module(ILImage *image, ILUInt32 *values,
					   ILUInt32 *valuesNext, ILToken token,
					   void *userData)
{
	const char *name;
	unsigned char *guidBase;
	unsigned long guidSize;
	ILModule *module;

	/* Unpack the token values */
	name = ILImageGetString(image, values[IL_OFFSET_MODULE_NAME]);
	guidBase = (unsigned char *)ILImageGetMetaEntry(image, "#GUID", &guidSize);

	/* Create the module structure */
	if(values[IL_OFFSET_MODULE_MVID] != IL_MAX_UINT32)
	{
		if((module = ILModuleCreate
				(image, token, name,
				 guidBase + values[IL_OFFSET_MODULE_MVID])) == 0)
		{
			return IL_LOADERR_MEMORY;
		}
	}
	else
	{
		if((module = ILModuleCreate(image, token, name, 0)) == 0)
		{
			return IL_LOADERR_MEMORY;
		}
	}

	/* If we have Edit & Continue information, then add it */
	if(values[IL_OFFSET_MODULE_GENERATION] != 0)
	{
		if(!ILModuleSetGeneration
					(module, values[IL_OFFSET_MODULE_GENERATION]))
		{
			return IL_LOADERR_MEMORY;
		}
	}
	if(values[IL_OFFSET_MODULE_ENCID] != IL_MAX_UINT32)
	{
		if(!ILModuleSetEncId
					(module, guidBase + values[IL_OFFSET_MODULE_ENCID]))
		{
			return IL_LOADERR_MEMORY;
		}
	}
	if(values[IL_OFFSET_MODULE_ENCBASEID] != IL_MAX_UINT32)
	{
		if(!ILModuleSetEncBaseId
					(module, guidBase + values[IL_OFFSET_MODULE_ENCBASEID]))
		{
			return IL_LOADERR_MEMORY;
		}
	}

	/* Done */
	return 0;
}

/*
 * Load a module reference token.
 */
static int Load_ModuleRef(ILImage *image, ILUInt32 *values,
						  ILUInt32 *valuesNext, ILToken token,
						  void *userData)
{
	const char *name;

	/* Unpack the token values */
	name = ILImageGetString(image, values[IL_OFFSET_MODULEREF_NAME]);

	/* Create the module reference structure */
	if(!ILModuleRefCreate(image, token, name))
	{
		return IL_LOADERR_MEMORY;
	}
	return 0;
}

/*
 * Load an assembly token.
 */
static int Load_Assembly(ILImage *image, ILUInt32 *values,
						 ILUInt32 *valuesNext, ILToken token,
						 void *userData)
{
	const char *name;
	ILAssembly *assem;

	/* Create the assembly structure */
	name = ILImageGetString(image, values[IL_OFFSET_ASSEMBLY_NAME]);
	assem = ILAssemblyCreate(image, token, name, 0);
	if(!assem)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Set the other assembly fields */
	ILAssemblySetHashAlgorithm(assem, values[IL_OFFSET_ASSEMBLY_HASHALG]);
	ILAssemblySetVersionSplit(assem,
							  values[IL_OFFSET_ASSEMBLY_VER_1],
							  values[IL_OFFSET_ASSEMBLY_VER_2],
							  values[IL_OFFSET_ASSEMBLY_VER_3],
							  values[IL_OFFSET_ASSEMBLY_VER_4]);
	ILAssemblySetAttrs(assem, ~0, values[IL_OFFSET_ASSEMBLY_ATTRS]);
	_ILAssemblySetOrigIndex(assem, values[IL_OFFSET_ASSEMBLY_KEY_RAW]);
	if(values[IL_OFFSET_ASSEMBLY_LOCALE])
	{
		name = ILImageGetString(image, values[IL_OFFSET_ASSEMBLY_LOCALE]);
		if(!ILAssemblySetLocale(assem, name))
		{
			return IL_LOADERR_MEMORY;
		}
	}

	/* Done */
	return 0;
}

/*
 * Load an assembly reference token.
 */
static int Load_AssemblyRef(ILImage *image, ILUInt32 *values,
							ILUInt32 *valuesNext, ILToken token,
							void *userData)
{
	const char *name;
	ILAssembly *assem;

	/* Create the assembly reference structure */
	name = ILImageGetString(image, values[IL_OFFSET_ASSEMBLYREF_NAME]);
	assem = ILAssemblyCreate(image, token, name, 1);
	if(!assem)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Set the other assembly reference fields */
	ILAssemblySetVersionSplit(assem,
							  values[IL_OFFSET_ASSEMBLYREF_VER_1],
							  values[IL_OFFSET_ASSEMBLYREF_VER_2],
							  values[IL_OFFSET_ASSEMBLYREF_VER_3],
							  values[IL_OFFSET_ASSEMBLYREF_VER_4]);
	ILAssemblySetAttrs(assem, ~0, values[IL_OFFSET_ASSEMBLYREF_ATTRS]);
	_ILAssemblySetOrigIndex(assem, values[IL_OFFSET_ASSEMBLYREF_KEY_RAW]);
	if(values[IL_OFFSET_ASSEMBLYREF_LOCALE])
	{
		name = ILImageGetString(image, values[IL_OFFSET_ASSEMBLYREF_LOCALE]);
		if(!ILAssemblySetLocale(assem, name))
		{
			return IL_LOADERR_MEMORY;
		}
	}
	_ILAssemblySetHashIndex(assem, values[IL_OFFSET_ASSEMBLYREF_HASH_RAW]);

	/* Done */
	return 0;
}

/*
 * Load an OS definition.
 */
static int Load_OSDef(ILImage *image, ILUInt32 *values,
					  ILUInt32 *valuesNext, ILToken token,
					  void *userData)
{
	if(!ILOSInfoCreate(image, token,
					   values[IL_OFFSET_OSDEF_IDENTIFIER],
					   values[IL_OFFSET_OSDEF_MAJOR],
					   values[IL_OFFSET_OSDEF_MINOR],
					   ILAssembly_FromToken
					   		(image, (IL_META_TOKEN_ASSEMBLY | 1))))
	{
		return IL_LOADERR_MEMORY;
	}
	else
	{
		return 0;
	}
}

/*
 * Load a processor definition.
 */
static int Load_ProcessorDef(ILImage *image, ILUInt32 *values,
							 ILUInt32 *valuesNext, ILToken token,
					  		 void *userData)
{
	if(!ILProcessorInfoCreate(image, token,
					  		  values[IL_OFFSET_PROCESSORDEF_NUM],
					   		  ILAssembly_FromToken
					   				(image, (IL_META_TOKEN_ASSEMBLY | 1))))
	{
		return IL_LOADERR_MEMORY;
	}
	else
	{
		return 0;
	}
}

/*
 * Load an OS reference.
 */
static int Load_OSRef(ILImage *image, ILUInt32 *values,
					  ILUInt32 *valuesNext, ILToken token,
					  void *userData)
{
	ILAssembly *ref;
	ref = ILAssembly_FromToken(image, values[IL_OFFSET_OSREF_ASSEMBLY]);
	if(!ILOSInfoCreate(image, token,
					   values[IL_OFFSET_OSREF_IDENTIFIER],
					   values[IL_OFFSET_OSREF_MAJOR],
					   values[IL_OFFSET_OSREF_MINOR], ref))
	{
		return IL_LOADERR_MEMORY;
	}
	else
	{
		return 0;
	}
}

/*
 * Load a processor reference.
 */
static int Load_ProcessorRef(ILImage *image, ILUInt32 *values,
							 ILUInt32 *valuesNext, ILToken token,
					  		 void *userData)
{
	ILAssembly *ref;
	ref = ILAssembly_FromToken(image, values[IL_OFFSET_OSREF_ASSEMBLY]);
	if(!ILProcessorInfoCreate(image, token,
					  		  values[IL_OFFSET_PROCESSORREF_NUM], ref))
	{
		return IL_LOADERR_MEMORY;
	}
	else
	{
		return 0;
	}
}

/*
 * Forward definition.
 */
static int Load_TypeDef(ILImage *image, ILUInt32 *values,
						ILUInt32 *valuesNext, ILToken token,
						void *userData);

/*
 * Load a reference to a type definition token that is
 * further along in the file from where we currently are.
 */
static int LoadForwardTypeDef(ILImage *image, ILToken token)
{
	ILUInt32 values[IL_IMAGE_TOKEN_COLUMNS];
	ILUInt32 valuesNext[IL_IMAGE_TOKEN_COLUMNS];
	if(!_ILImageRawTokenData(image, token, values))
	{
		return IL_LOADERR_BAD_META;
	}
	if((token & ~IL_META_TOKEN_MASK) <
			image->tokenCount[IL_META_TOKEN_TYPE_DEF >> 24])
	{
		if(!_ILImageRawTokenData(image, token + 1, valuesNext))
		{
			return IL_LOADERR_BAD_META;
		}
		return Load_TypeDef(image, values, valuesNext, token, 0);
	}
	else
	{
		return Load_TypeDef(image, values, 0, token, 0);
	}
}

/*
 * Find the parent of a nested class by searching the NestedClass table.
 */
static ILProgramItem *FindNestedParent(ILImage *image, ILToken child)
{
	ILToken left, right, middle;
	ILToken count, parent;
	ILUInt32 values[IL_IMAGE_TOKEN_COLUMNS];
	ILProgramItem *item;

	count = (ILToken)(image->tokenCount[IL_META_TOKEN_NESTED_CLASS >> 24]);
	if(!count)
	{
		return 0;
	}
	parent = 0;
	if((image->sorted &
			(((ILUInt64)1) << (IL_META_TOKEN_NESTED_CLASS >> 24))) != 0)
	{
		/* The NestedClass table is sorted, so use a binary search */
		left = 0;
		right = count - 1;
		while(left <= right)
		{
			middle = (left + right) / 2;
			if(!_ILImageRawTokenData(image, IL_META_TOKEN_NESTED_CLASS +
											middle + 1, values))
			{
				break;
			}
			if(values[IL_OFFSET_NESTEDCLASS_CHILD] == child)
			{
				parent = values[IL_OFFSET_NESTEDCLASS_PARENT];
				break;
			}
			else if(values[IL_OFFSET_NESTEDCLASS_CHILD] < child)
			{
				left = middle + 1;
			}
			else
			{
				right = middle - 1;
			}
		}
	}
	else
	{
		/* The NestedClass table is not sorted, so use a linear search */
		for(left = 0; left < count; ++left)
		{
			if(!_ILImageRawTokenData(image, IL_META_TOKEN_NESTED_CLASS +
											left + 1, values))
			{
				break;
			}
			if(values[IL_OFFSET_NESTEDCLASS_CHILD] == child)
			{
				parent = values[IL_OFFSET_NESTEDCLASS_PARENT];
				break;
			}
		}
	}
	if(parent)
	{
		item = ILProgramItem_FromToken(image, parent);
		if(item)
		{
			return item;
		}
	}
	return 0;
}

/*
 * Load a type definition token.
 */
static int Load_TypeDef(ILImage *image, ILUInt32 *values,
						ILUInt32 *valuesNext, ILToken token,
						void *userData)
{
	ILClass *info;
	ILClass *parent;
	const char *name;
	const char *namespace;
	int error;
	ILProgramItem *scope;

	/* If we have already loaded this type, then bail out */
	if(_ILImageTokenAlreadyLoaded(image, token))
	{
		return 0;
	}

	/* Get the name and namespace for the type */
	name = ILImageGetString(image, values[IL_OFFSET_TYPEDEF_NAME]);
	namespace = ILImageGetString(image, values[IL_OFFSET_TYPEDEF_NAMESPACE]);
	if(namespace && *namespace == '\0')
	{
		namespace = 0;
	}

	/* Determine the scope to use to define the type */
	if((values[IL_OFFSET_TYPEDEF_ATTRS] & IL_META_TYPEDEF_VISIBILITY_MASK)
			< IL_META_TYPEDEF_NESTED_PUBLIC)
	{
		/* Global scope */
		scope = ILClassGlobalScope(image);
	}
	else
	{
		/* Nested scope */
		scope = FindNestedParent(image, token);
	}

	/* If we don't have a scope, then exit with an error */
	if(!scope)
	{
		META_VAL_ERROR("unknown type definition scope");
		return IL_LOADERR_BAD_META;
	}

	/* Locate the parent class */
	if(values[IL_OFFSET_TYPEDEF_PARENT])
	{
		parent = ILClass_FromToken(image, values[IL_OFFSET_TYPEDEF_PARENT]);
		if(parent)
		{
			/* Resolve TypeSpec's into ILClass structures */
			parent = ILProgramItemToClass((ILProgramItem *)parent);
		}
		else if((values[IL_OFFSET_TYPEDEF_PARENT] & IL_META_TOKEN_MASK)
					== IL_META_TOKEN_TYPE_DEF)
		{
			/* The class inherits from a TypeDef we haven't seen yet */
			error = LoadForwardTypeDef(image, values[IL_OFFSET_TYPEDEF_PARENT]);
			if(error != 0)
			{
				return error;
			}
			parent = ILClass_FromToken(image, values[IL_OFFSET_TYPEDEF_PARENT]);
		}
		if(!parent)
		{
			META_VAL_ERROR("cannot locate parent type");
			return IL_LOADERR_BAD_META;
		}
	}
	else
	{
		/* No parent, so this is probably the "System.Object" class */
		parent = 0;
	}

	/* See if we already have a definition using this name */
	info = ILClassLookup(scope, name, namespace);
	if(info)
	{
		if(!ILClassIsRef(info))
		{
			META_VAL_ERROR("type defined multiple times");
			return IL_LOADERR_BAD_META;
		}
	}

	/* Create the class, which will convert the reference if necessary */
	info = ILClassCreate(scope, token, name, namespace, parent);
	if(!info)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Set the attributes for the class */
	ILClassSetAttrs(info, ~0, values[IL_OFFSET_TYPEDEF_ATTRS] &
								~(IL_META_TYPEDEF_HAS_SECURITY));

	/* Done: we'll get the members later */
	return 0;
}

/*
 * Load a type specification token.
 */
static int Load_TypeSpec(ILImage *image, ILUInt32 *values,
						 ILUInt32 *valuesNext, ILToken token,
						 void *userData)
{
	ILType *type;
	ILTypeSpec *spec;

	/* Parse the type specification blob */
	type = ILTypeFromTypeSpec(image->context, image,
						      values[IL_OFFSET_TYPESPEC_TYPE],
						      values[IL_OFFSET_TYPESPEC_TYPE_LEN]);
	if(!type)
	{
		META_VAL_ERROR("invalid type specification");
		return IL_LOADERR_BAD_META;
	}

	/* Create a TypeSpec structure and attach it to the image */
	spec = ILTypeSpecCreate(image, token, type);
	if(!spec)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Set the type blob index in case this image is later written */
	_ILTypeSpecSetTypeIndex(spec, values[IL_OFFSET_TYPESPEC_TYPE_RAW]);

	/* Done */
	return 0;
}

/*
 * Load a field definition token.
 */
static int Load_FieldDef(ILImage *image, ILUInt32 *values,
						 ILUInt32 *valuesNext, ILToken token,
						 void *userData)
{
	ILField *field;

	/* Create the field and attach it to the class */
	field = ILFieldCreate((ILClass *)userData, token,
				  ILImageGetString(image, values[IL_OFFSET_FIELDDEF_NAME]),
				  values[IL_OFFSET_FIELDDEF_ATTRS] &
				  	~(IL_META_FIELDDEF_HAS_FIELD_MARSHAL |
					  IL_META_FIELDDEF_HAS_SECURITY |
					  IL_META_FIELDDEF_HAS_DEFAULT |
					  IL_META_FIELDDEF_HAS_FIELD_RVA));
	if(!field)
	{
		return IL_LOADERR_MEMORY;
	}

	/* We must have a signature */
	if(!(values[IL_OFFSET_FIELDDEF_SIGNATURE]))
	{
		META_VAL_ERROR("missing signature");
		return IL_LOADERR_BAD_META;
	}

	/* Parse the signature blob */
	ILMemberSetSignature((ILMember *)field,
		ILTypeFromFieldSig(image->context, image,
						   values[IL_OFFSET_FIELDDEF_SIGNATURE],
						   values[IL_OFFSET_FIELDDEF_SIGNATURE_LEN]));
	if(!(ILMemberGetSignature((ILMember *)field)))
	{
		META_VAL_ERROR("invalid field signature");
		return IL_LOADERR_BAD_META;
	}
	_ILMemberSetSignatureIndex((ILMember *)field,
							   values[IL_OFFSET_FIELDDEF_SIGNATURE_RAW]);

	return 0;
}

/*
 * Load a parameter definition token.
 */
static int Load_ParamDef(ILImage *image, ILUInt32 *values,
						 ILUInt32 *valuesNext, ILToken token,
						 void *userData)
{
	const char *name = ILImageGetString(image, values[IL_OFFSET_PARAMDEF_NAME]);
	if(!ILParameterCreate((ILMethod *)userData, token, name,
						  values[IL_OFFSET_PARAMDEF_ATTRS] &
						  	~(IL_META_PARAMDEF_HAS_DEFAULT |
							  IL_META_PARAMDEF_HAS_FIELD_MARSHAL),
						  values[IL_OFFSET_PARAMDEF_NUMBER]))
	{
		return IL_LOADERR_MEMORY;
	}
	else
	{
		return 0;
	}
}

/*
 * Determine the number of items in a token range.
 */
static int SizeOfRange(ILImage *image, unsigned long tokenKind,
					   ILUInt32 *values, ILUInt32 *valuesNext,
					   int index, ILUInt32 *num)
{
	if(!(values[index]))
	{
		*num = 0;
		return 1;
	}
	else if(valuesNext && valuesNext[index] != 0)
	{
		if(valuesNext[index] < values[index])
		{
			return 0;
		}
		*num = valuesNext[index] - values[index];
		return 1;
	}
	else
	{
		*num = (image->tokenCount[tokenKind >> 24] + 1) -
			   (values[index] & ~IL_META_TOKEN_MASK);
		return 1;
	}
}

/*
 * Load a method definition token.
 */
static int Load_MethodDef(ILImage *image, ILUInt32 *values,
						  ILUInt32 *valuesNext, ILToken token,
						  void *userData)
{
	ILMethod *method;
	ILUInt32 num;
	int error;
	ILType *signature;

	/* Create the method and attach it to the class */
	method = ILMethodCreate((ILClass *)userData, token,
				  ILImageGetString(image, values[IL_OFFSET_METHODDEF_NAME]),
				  values[IL_OFFSET_METHODDEF_ATTRS] &
				  	~(IL_META_METHODDEF_HAS_SECURITY));
	if(!method)
	{
		return IL_LOADERR_MEMORY;
	}

	/* We must have a signature */
	if(!(values[IL_OFFSET_METHODDEF_SIGNATURE]))
	{
		META_VAL_ERROR("missing signature");
		return IL_LOADERR_BAD_META;
	}

	/* Parse the signature blob */
	signature =
		ILTypeFromMethodDefSig(image->context, image,
						       values[IL_OFFSET_METHODDEF_SIGNATURE],
						       values[IL_OFFSET_METHODDEF_SIGNATURE_LEN]);
	if(!signature)
	{
		META_VAL_ERROR("invalid method signature");
		return IL_LOADERR_BAD_META;
	}
	ILMemberSetSignature((ILMember *)method, signature);
	_ILMemberSetSignatureIndex((ILMember *)method,
							   values[IL_OFFSET_METHODDEF_SIGNATURE_RAW]);

	/* Set the other method fields */
	ILMethodSetImplAttrs(method, ~0,
		(values[IL_OFFSET_METHODDEF_IMPL_ATTRS] &
			~(IL_META_METHODIMPL_JAVA_FP_STRICT |
			  IL_META_METHODIMPL_JAVA)));
	ILMethodSetCallConv(method, ILType_CallConv(signature));
	ILMethodSetRVA(method, values[IL_OFFSET_METHODDEF_RVA]);

	/* Parse the parameter definitions */
	if(!SizeOfRange(image, IL_META_TOKEN_PARAM_DEF,
					values, valuesNext, IL_OFFSET_METHODDEF_FIRST_PARAM, &num))
	{
		META_VAL_ERROR("invalid parameter count");
		return IL_LOADERR_BAD_META;
	}
	EXIT_IF_ERROR(LoadTokenRange(image, IL_META_TOKEN_PARAM_DEF,
								 values[IL_OFFSET_METHODDEF_FIRST_PARAM], num,
								 Load_ParamDef, method));

	/* Done */
	return 0;
}

/*
 * Load the fields and methods for a type definition token.
 */
static int Load_TypeDefPhase2(ILImage *image, ILUInt32 *values,
						      ILUInt32 *valuesNext, ILToken token,
						      void *userData)
{
	ILClass *info;
	ILUInt32 num;
	int error;

	/* Get the class information block for the token */
	info = ILClass_FromToken(image, token);

	/* If the class has a nested attribute, but it is still
	   at the top-most level, then complain */
	if(ILClassGetNestedParent(info) == 0 &&
	   (ILClass_IsNestedPublic(info) ||
	    ILClass_IsNestedPrivate(info) ||
		ILClass_IsNestedFamily(info) ||
		ILClass_IsNestedAssembly(info) ||
		ILClass_IsNestedFamAndAssem(info) ||
		ILClass_IsNestedFamOrAssem(info)))
	{
		META_VAL_ERROR("nested class at outer scope");
		return IL_LOADERR_BAD_META;
	}

	/* Load the fields */
	if(!SizeOfRange(image, IL_META_TOKEN_FIELD_DEF,
					values, valuesNext, IL_OFFSET_TYPEDEF_FIRST_FIELD, &num))
	{
		META_VAL_ERROR("invalid field count");
		return IL_LOADERR_BAD_META;
	}
	EXIT_IF_ERROR(LoadTokenRange(image, IL_META_TOKEN_FIELD_DEF,
								 values[IL_OFFSET_TYPEDEF_FIRST_FIELD], num,
								 Load_FieldDef, info));

	/* Load the methods */
	if(!SizeOfRange(image, IL_META_TOKEN_METHOD_DEF,
					values, valuesNext, IL_OFFSET_TYPEDEF_FIRST_METHOD, &num))
	{
		META_VAL_ERROR("invalid method count");
		return IL_LOADERR_BAD_META;
	}
	EXIT_IF_ERROR(LoadTokenRange(image, IL_META_TOKEN_METHOD_DEF,
								 values[IL_OFFSET_TYPEDEF_FIRST_METHOD], num,
								 Load_MethodDef, info));

	/* Done: we'll get the events, properties, and interfaces later */
	return 0;
}

/*
 * Load an interface implementation token.
 */
static int Load_InterfaceImpl(ILImage *image, ILUInt32 *values,
							  ILUInt32 *valuesNext, ILToken token,
							  void *userData)
{
	ILClass *info;
	ILClass *interface;

	/* Get the type that is implementing the interface */
	info = ILClass_FromToken(image, values[IL_OFFSET_INTERFACE_TYPE]);
	if(!info)
	{
		META_VAL_ERROR("unknown type");
		return IL_LOADERR_BAD_META;
	}

	/* Get the interface type */
	interface = ILClass_FromToken(image, values[IL_OFFSET_INTERFACE_INTERFACE]);
	if(interface)
	{
		/* Resolve TypeSpec's into ILClass structures */
		interface = ILProgramItemToClass((ILProgramItem *)interface);
	}
	if(!interface)
	{
		META_VAL_ERROR("unknown interface");
		return IL_LOADERR_BAD_META;
	}

	/* The second class must be an interface */
	if(!ILClass_IsInterface(interface) && !ILClassIsRef(interface))
	{
		META_VAL_ERROR("interface type is not an interface");
		return IL_LOADERR_BAD_META;
	}
	
	/* Add the "implements" clause to the class */
	if(!ILClassAddImplements(info, interface, token))
	{
		return IL_LOADERR_MEMORY;
	}

	/* Done */
	return 0;
}

/*
 * Load a PInvoke token.
 */
static int Load_PInvoke(ILImage *image, ILUInt32 *values,
						ILUInt32 *valuesNext, ILToken token,
						void *userData)
{
	ILMethod *method;
	ILModule *module;
	ILPInvoke *pinvoke;

	/* An early version supported PInvoke for fields.  That is now obsolete */
	if((values[IL_OFFSET_IMPLMAP_METHOD] & IL_META_TOKEN_MASK)
				!= IL_META_TOKEN_METHOD_DEF)
	{
		META_VAL_ERROR("pinvoke must be applied to a method");
		return IL_LOADERR_BAD_META;
	}

	/* Validate that the method is really PInvoke */
	method = ILMethod_FromToken(image, values[IL_OFFSET_IMPLMAP_METHOD]);
	if(!method || !ILMethod_HasPInvokeImpl(method))
	{
		META_VAL_ERROR("pinvoke token applied to a non-pinvoke method");
		return IL_LOADERR_BAD_META;
	}

	/* Find the module that the function is being imported from */
	module = ILModule_FromToken(image, values[IL_OFFSET_IMPLMAP_MODULE]);
	if(!module)
	{
		META_VAL_ERROR("invalid module for pinvoke import");
		return IL_LOADERR_BAD_META;
	}

	/* Create the PInvoke record */
	pinvoke = ILPInvokeCreate(method, token,
							  values[IL_OFFSET_IMPLMAP_ATTRS], module,
							  ILImageGetString
							  		(image, values[IL_OFFSET_IMPLMAP_ALIAS]));
	if(!pinvoke)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Done */
	return 0;
}

/*
 * Load an override declaration token.
 */
static int Load_Override(ILImage *image, ILUInt32 *values,
						 ILUInt32 *valuesNext, ILToken token,
						 void *userData)
{
	ILClass *classInfo;
	ILProgramItem *decl;
	ILProgramItem *body;

	/* Locate the class that contains the method declaration */
	classInfo = ILClass_FromToken
					(image, values[IL_OFFSET_METHODIMPL_TYPE]);
	if(!classInfo)
	{
		META_VAL_ERROR("invalid class in override token");
		return IL_LOADERR_BAD_META;
	}

	/* Locate the override method declaration */
	decl = ILProgramItem_FromToken
					(image, values[IL_OFFSET_METHODIMPL_METHOD_2]);
	if(!ILProgramItemToMethod(decl))
	{
		META_VAL_ERROR("invalid method declaration in override token");
		return IL_LOADERR_BAD_META;
	}

	/* Locate the override method body */
	body = ILProgramItem_FromToken
					(image, values[IL_OFFSET_METHODIMPL_METHOD_1]);
	if(!ILProgramItemToMethod(body))
	{
		META_VAL_ERROR("invalid method body in override token");
		return IL_LOADERR_BAD_META;
	}

	/* Create the PInvoke record */
	if(!ILOverrideCreate(classInfo, token, (ILMethod *)decl, (ILMethod *)body))
	{
		return IL_LOADERR_MEMORY;
	}

	/* Done */
	return 0;
}

/*
 * Load an Event token.
 */
static int Load_Event(ILImage *image, ILUInt32 *values,
					  ILUInt32 *valuesNext, ILToken token,
					  void *userData)
{
	ILClass *type;

	/* Get the event's type */
	if(values[IL_OFFSET_EVENT_TYPE])
	{
		type = ILClass_FromToken(image, values[IL_OFFSET_EVENT_TYPE]);
		if(type)
		{
			/* Resolve TypeSpec's into ILClass structures */
			type = ILProgramItemToClass((ILProgramItem *)type);
		}
		if(!type)
		{
			META_VAL_ERROR("unknown event type");
			return IL_LOADERR_BAD_META;
		}
	}
	else
	{
		type = 0;
	}

	/* Create the event */
	if(!ILEventCreate((ILClass *)userData, token,
					  ILImageGetString
					  		(image, values[IL_OFFSET_EVENT_NAME]),
					  values[IL_OFFSET_EVENT_ATTRS], type))
	{
		return IL_LOADERR_MEMORY;
	}
	else
	{
		return 0;
	}
}

/*
 * Load an EventAssociation token, and the asssociated event.
 */
static int Load_EventAssociation(ILImage *image, ILUInt32 *values,
								 ILUInt32 *valuesNext, ILToken token,
								 void *userData)
{
	ILClass *info;
	ILUInt32 num;
	int error;

	/* Find the type that owns the event */
	info = ILClass_FromToken(image, values[IL_OFFSET_EVENTMAP_TYPE]);
	if(!info)
	{
		META_VAL_ERROR("unknown owning type");
		return IL_LOADERR_BAD_META;
	}

	/* Load the events that are covered by this association */
	if(!SizeOfRange(image, IL_META_TOKEN_EVENT,
					values, valuesNext, IL_OFFSET_EVENTMAP_EVENT, &num))
	{
		META_VAL_ERROR("invalid event count");
		return IL_LOADERR_BAD_META;
	}
	EXIT_IF_ERROR(LoadTokenRange(image, IL_META_TOKEN_EVENT,
								 values[IL_OFFSET_EVENTMAP_EVENT], num,
								 Load_Event, info));

	/* Done */
	return 0;
}

/*
 * Load a Property token.
 */
static int Load_Property(ILImage *image, ILUInt32 *values,
						 ILUInt32 *valuesNext, ILToken token,
						 void *userData)
{
	ILType *signature;
	ILProperty *property;

	/* Get the property's signature */
	signature = ILTypeFromPropertySig
						(image->context, image,
						 values[IL_OFFSET_PROPERTY_SIGNATURE],
						 values[IL_OFFSET_PROPERTY_SIGNATURE_LEN]);
	if(!signature)
	{
		META_VAL_ERROR("invalid property signature");
		return IL_LOADERR_BAD_META;
	}

	/* Create the property */
	property = ILPropertyCreate((ILClass *)userData, token,
					     ILImageGetString
					  		(image, values[IL_OFFSET_PROPERTY_NAME]),
					     values[IL_OFFSET_PROPERTY_ATTRS] &
						 	~(IL_META_PROPDEF_HAS_DEFAULT),
						 signature);
	if(!signature)
	{
		return IL_LOADERR_MEMORY;
	}
	_ILMemberSetSignatureIndex((ILMember *)property,
							   values[IL_OFFSET_PROPERTY_SIGNATURE_RAW]);

	/* Done */
	return 0;
}

/*
 * Load a PropertyAssociation token, and the asssociated properties.
 */
static int Load_PropertyAssociation(ILImage *image, ILUInt32 *values,
								    ILUInt32 *valuesNext, ILToken token,
								    void *userData)
{
	ILClass *info;
	ILUInt32 num;
	int error;

	/* Find the type that owns the property */
	info = ILClass_FromToken(image, values[IL_OFFSET_PROPMAP_TYPE]);
	if(!info)
	{
		META_VAL_ERROR("unknown owning type");
		return IL_LOADERR_BAD_META;
	}

	/* Load the properties that are covered by this association */
	if(!SizeOfRange(image, IL_META_TOKEN_PROPERTY,
					values, valuesNext, IL_OFFSET_PROPMAP_PROPERTY, &num))
	{
		META_VAL_ERROR("invalid property count");
		return IL_LOADERR_BAD_META;
	}
	EXIT_IF_ERROR(LoadTokenRange(image, IL_META_TOKEN_PROPERTY,
								 values[IL_OFFSET_PROPMAP_PROPERTY], num,
								 Load_Property, info));

	/* Done */
	return 0;
}

/*
 * Load a MethodAssociation token.
 */
static int Load_MethodAssociation(ILImage *image, ILUInt32 *values,
								  ILUInt32 *valuesNext, ILToken token,
								  void *userData)
{
	ILMethod *method;
	ILProgramItem *owner;

	/* Find the method */
	method = ILMethod_FromToken(image, values[IL_OFFSET_METHODSEM_METHOD]);
	if(!method)
	{
		META_VAL_ERROR("unknown method reference");
		return IL_LOADERR_BAD_META;
	}

	/* Get the method semantics owner */
	owner = ILProgramItem_FromToken(image, values[IL_OFFSET_METHODSEM_OWNER]);
	if(!owner)
	{
		META_VAL_ERROR("unknown owner reference for method semantics");
		return IL_LOADERR_BAD_META;
	}

	/* Create the method semantics block and attach it */
	if(!ILMethodSemCreate(owner, token,
						  values[IL_OFFSET_METHODSEM_SEMANTICS], method))
	{
		return IL_LOADERR_MEMORY;
	}

	/* Done */
	return 0;
}

/*
 * Load a member reference token.
 */
static int Load_MemberRef(ILImage *image, ILUInt32 *values,
						  ILUInt32 *valuesNext, ILToken token,
						  void *userData)
{
	const char *name;
	ILType *type;
	ILClass *classInfo;
	ILClass *currentInfo;
	ILClass *resolvedClass;
	ILTypeSpec *spec;
	ILMember *member;
	ILMember *resolvedMember;
	int isMethod;
	ILMethod *method;
	int loadFlags = (int)(ILNativeInt)userData;
	ILType *origSig;

	/* Get the member's name */
	name = ILImageGetString(image, values[IL_OFFSET_MEMBERREF_NAME]);
	if(!name)
	{
		META_VAL_ERROR("invalid MemberRef name");
		return IL_LOADERR_BAD_META;
	}

	/* Parse the type signature */
	if(values[IL_OFFSET_MEMBERREF_SIGNATURE_LEN] > 0 &&
	   image->blobPool[values[IL_OFFSET_MEMBERREF_SIGNATURE]] ==
	   		(char)IL_META_CALLCONV_FIELD)
	{
		/* Refers to a field */
		type = ILTypeFromFieldSig(image->context, image,
								  values[IL_OFFSET_MEMBERREF_SIGNATURE],
								  values[IL_OFFSET_MEMBERREF_SIGNATURE_LEN]);
		isMethod = 0;
	}
	else
	{
		/* Refers to a method */
		type = ILTypeFromMethodRefSig(image->context, image,
						      values[IL_OFFSET_MEMBERREF_SIGNATURE],
						      values[IL_OFFSET_MEMBERREF_SIGNATURE_LEN]);
		isMethod = 1;
	}
	if(!type)
	{
		META_VAL_ERROR("invalid MemberRef type signature");
		return IL_LOADERR_BAD_META;
	}
	origSig = type;

	/* Determine where to look for the member */
	classInfo = 0;
	switch(values[IL_OFFSET_MEMBERREF_PARENT] & IL_META_TOKEN_MASK)
	{
		case IL_META_TOKEN_TYPE_DEF:
		{
			/* Refers to a member of a local type */
			classInfo = ILClass_FromToken
							(image, values[IL_OFFSET_MEMBERREF_PARENT]);
			member = 0;
			if(!classInfo)
			{
				break;
			}
		localType:
			currentInfo = classInfo;
			while(currentInfo != 0)
			{
				member = 0;
				while((member = ILClassNextMember(currentInfo, member)) != 0)
				{
					if(!strcmp(ILMember_Name(member), name))
					{
						if(isMethod && ILMember_IsMethod(member))
						{
							if(ILTypeIdentical
									(ILMember_Signature(member), type))
							{
								break;
							}
						}
						else if(!isMethod && ILMember_IsField(member))
						{
							if(ILTypeIdentical
									(ILMember_Signature(member), type))
							{
								break;
							}
						}
					}
				}
				if(member != 0)
				{
					break;
				}
				currentInfo = ILClass_Parent(currentInfo);
			}
			if(member == 0 && ILType_IsComplex(type) &&
			   ILType_Kind(type) == (IL_TYPE_COMPLEX_METHOD |
			   						 IL_TYPE_COMPLEX_METHOD_SENTINEL))
			{
				/* Create a local reference to a vararg call site */
				method = ILMethodCreate(classInfo, token, name, 0);
				if(!method)
				{
					return IL_LOADERR_MEMORY;
				}
				ILMethodSetCallConv(method, ILType_CallConv(type));
				ILMemberSetSignature((ILMember *)method, type);
				return 0;
			}
		}
		break;

	 	case IL_META_TOKEN_TYPE_REF:
		{
			/* Refers to a member of a foreign type */
			classInfo = ILClass_FromToken
							(image, values[IL_OFFSET_MEMBERREF_PARENT]);
			member = 0;
			if(!classInfo)
			{
				break;
			}
			if(!ILClassIsRef(classInfo))
			{
				/* The TypeRef was resolved to a local type */
				goto localType;
			}
		foreignType:
			if(isMethod)
			{
				/* Create a method reference within the TypeRef */
				method = ILMethodCreate(classInfo, token, name, 0);
				if(!method)
				{
					return IL_LOADERR_MEMORY;
				}
				ILMethodSetCallConv(method, ILType_CallConv(type));
				member = (ILMember *)method;
			}
			else
			{
				/* Create a field reference within the TypeRef */
				member = (ILMember *)ILFieldCreate(classInfo, token, name, 0);
				if(!member)
				{
					return IL_LOADERR_MEMORY;
				}
			}
			ILMemberSetSignature(member, type);

			/* Resolve the member reference to the foreign type */
			if((loadFlags & IL_LOADFLAG_NO_RESOLVE) == 0)
			{
				resolvedClass = ILClassResolve(classInfo);
				if(!resolvedClass || ILClassIsRef(resolvedClass))
				{
					/* Failed to resolve the class */
					member = 0;
					break;
				}
				resolvedMember = 0;
				while(resolvedClass != 0)
				{
					resolvedMember = 0;
					while((resolvedMember = ILClassNextMember
								(resolvedClass, resolvedMember)) != 0)
					{
						if(!strcmp(ILMember_Name(resolvedMember), name))
						{
							if(isMethod && ILMember_IsMethod(resolvedMember))
							{
								if(ILTypeIdentical
									(ILMember_Signature(resolvedMember), type))
								{
									break;
								}
							}
							else if(!isMethod &&
									ILMember_IsField(resolvedMember))
							{
								if(ILTypeIdentical
									(ILMember_Signature(resolvedMember), type))
								{
									break;
								}
							}
						}
					}
					if(resolvedMember != 0)
					{
						break;
					}
					resolvedClass = ILClass_Parent(resolvedClass);
				}
				if(!resolvedMember)
				{
					/* Failed to resolve the member */
					if(ILType_IsComplex(type) &&
					   ILType_Kind(type) == (IL_TYPE_COMPLEX_METHOD |
					   						 IL_TYPE_COMPLEX_METHOD_SENTINEL))
					{
						/* Create a local reference to a vararg call site */
						method = ILMethodCreate(classInfo, token, name, 0);
						if(!method)
						{
							return IL_LOADERR_MEMORY;
						}
						ILMethodSetCallConv(method, ILType_CallConv(type));
						ILMemberSetSignature((ILMember *)method, type);
						return 0;
					}
					member = 0;
					break;
				}
				if(!_ILProgramItemLink(&(member->programItem),
									   &(resolvedMember->programItem)))
				{
					return IL_LOADERR_MEMORY;
				}
			}
		}
		break;

	 	case IL_META_TOKEN_MODULE_REF:
		{
			/* Refers to a member of a foreign module's global type */
			member = 0;
		}
		break;

	 	case IL_META_TOKEN_METHOD_DEF:
		{
			/* The parent refers to a local method: ignore the other info */
			member = ILMember_FromToken
						(image, values[IL_OFFSET_MEMBERREF_PARENT]);
		}
		break;

	 	case IL_META_TOKEN_TYPE_SPEC:
		{
			/* Refers to a type which is named using a signature */
			spec = ILTypeSpec_FromToken
						(image, values[IL_OFFSET_MEMBERREF_PARENT]);
			classInfo = ILTypeSpecGetClassRef(spec);
			member = 0;
			if(!classInfo)
			{
				break;
			}
			if(ILClassIsRef(classInfo) && classInfo->programItem.token != 0)
			{
				goto foreignType;
			}
			else
			{
				goto localType;
			}
		}
		/* Not reached */

		default:
		{
			META_VAL_ERROR("unknown MemberRef parent");
			return IL_LOADERR_BAD_META;
		}
		/* Not reached */
	}

	/* Validate the member information */
	if(!member)
	{
	#if IL_DEBUG_META
		if(classInfo)
		{
			if(classInfo->namespace &&
			   !strcmp(classInfo->namespace, "$Synthetic"))
			{
				fprintf(stderr,
						"metadata error in token 0x%08lX: "
								"member `%s.%s' not found\n",
						(unsigned long)token, classInfo->name, name);
			}
			else
			{
				fprintf(stderr,
						"metadata error in token 0x%08lX: "
								"member `%s%s%s.%s' not found\n",
						(unsigned long)token,
						(classInfo->namespace ? classInfo->namespace : ""),
						(classInfo->namespace ? "." : ""),
						classInfo->name, name);
			}
		}
		else
		{
			fprintf(stderr,
					"metadata error in token 0x%08lX: member %s not found\n",
					(unsigned long)token, name);
		}
	#endif
		return IL_LOADERR_BAD_META;
	}
	if(!ILMember_IsMethod(member) && !ILMember_IsField(member))
	{
		META_VAL_ERROR("MemberRef does not refer to a method or field");
		return IL_LOADERR_BAD_META;
	}

	/* Create a reference in the MemberRef table to the actual member */
	if(member->programItem.token != token)
	{
		member = ILMemberCreateRef(member, token);
		if(!member)
		{
			return IL_LOADERR_MEMORY;
		}
		if(origSig != ILType_Invalid)
		{
			member->signature = origSig;
		}
		_ILMemberSetSignatureIndex
				(member, values[IL_OFFSET_MEMBERREF_SIGNATURE_RAW]);
	}

	/* Done */
	return 0;
}

/*
 * Load a custom attribute token.
 */
static int Load_CustomAttr(ILImage *image, ILUInt32 *values,
						   ILUInt32 *valuesNext, ILToken token,
						   void *userData)
{
	ILAttribute *attr;
	ILProgramItem *owner;
	ILProgramItem *type;

	/* Create an attribute block */
	if((attr = ILAttributeCreate(image, token)) == 0)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Set the owner for the attribute */
	owner = ILProgramItem_FromToken(image, values[IL_OFFSET_CUSTOMATTR_OWNER]);
	if(owner)
	{
		ILProgramItemAddAttribute(owner, attr);
	}

	/* Set the type of the attribute */
	if((values[IL_OFFSET_CUSTOMATTR_NAME] & IL_META_TOKEN_MASK)
			!= IL_META_TOKEN_STRING)
	{
		type = ILProgramItem_FromToken
					(image, values[IL_OFFSET_CUSTOMATTR_NAME]);
		if(type)
		{
			ILAttributeSetType(attr, type);
		}
	}
	else
	{
		ILAttributeSetString(attr);
	}

	/* Set the attribute's value */
	_ILAttributeSetValueIndex(attr, values[IL_OFFSET_CUSTOMATTR_DATA_RAW]);

	/* Done */
	return 0;
}

/*
 * Load a stand alone signature token.
 */
static int Load_StandAloneSig(ILImage *image, ILUInt32 *values,
						      ILUInt32 *valuesNext, ILToken token,
						      void *userData)
{
	ILType *type;
	char sigType;
	ILStandAloneSig *sig;

	/* Determine if we need to parse locals or a method */
	if(values[IL_OFFSET_SIGNATURE_VALUE_LEN])
	{
		sigType = image->blobPool[values[IL_OFFSET_SIGNATURE_VALUE]];
	}
	else
	{
		sigType = 0xFF;
	}
	if(sigType == (char)IL_META_CALLCONV_LOCAL_SIG)
	{
		/* Parse locals */
		type = ILTypeFromLocalVarSig
					(image, values[IL_OFFSET_SIGNATURE_VALUE_RAW]);
	}
	else if(sigType == (char)IL_META_CALLCONV_FIELD)
	{
		/* Parse a field signature */
		type = ILTypeFromFieldSig
					(image->context, image,
					 values[IL_OFFSET_SIGNATURE_VALUE],
					 values[IL_OFFSET_SIGNATURE_VALUE_LEN]);
	}
	else
	{
		/* Parse a method */
		type = ILTypeFromStandAloneMethodSig
					(image->context, image,
					 values[IL_OFFSET_SIGNATURE_VALUE],
					 values[IL_OFFSET_SIGNATURE_VALUE_LEN]);
	}
	if(!type)
	{
		META_VAL_ERROR("invalid stand alone signature");
		return IL_LOADERR_BAD_META;
	}

	/* Create the signature block and attach it to the image */
	sig = ILStandAloneSigCreate(image, token, type);
	if(!sig)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Set the type blob index in case this image is later written */
	_ILStandAloneSigSetTypeIndex(sig, values[IL_OFFSET_SIGNATURE_VALUE_RAW]);

	/* Done */
	return 0;
}

/*
 * Load a constant token.
 */
static int Load_Constant(ILImage *image, ILUInt32 *values,
					     ILUInt32 *valuesNext, ILToken token,
					     void *userData)
{
	ILConstant *constant;

	/* Create the constant record */
	constant = ILConstantCreate(image, token,
						ILProgramItem_FromToken
							(image, values[IL_OFFSET_CONSTANT_REFERENCE]),
						values[IL_OFFSET_CONSTANT_TYPE]);
	if(!constant)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Add the value information to the constant */
	_ILConstantSetValueIndex(constant, values[IL_OFFSET_CONSTANT_DATA_RAW]);

	/* Done */
	return 0;
}

/*
 * Load a field RVA token.
 */
static int Load_FieldRVA(ILImage *image, ILUInt32 *values,
					     ILUInt32 *valuesNext, ILToken token,
					     void *userData)
{
	if(!ILFieldRVACreate(image, token,
						 ILField_FromToken
							(image, values[IL_OFFSET_FIELDRVA_FIELD]),
						 values[IL_OFFSET_FIELDRVA_RVA]))
	{
		return IL_LOADERR_MEMORY;
	}
	return 0;
}

/*
 * Load a field layout token.
 */
static int Load_FieldLayout(ILImage *image, ILUInt32 *values,
					        ILUInt32 *valuesNext, ILToken token,
					        void *userData)
{
	if(!ILFieldLayoutCreate(image, token,
						    ILField_FromToken
								(image, values[IL_OFFSET_FIELDLAYOUT_FIELD]),
						 	values[IL_OFFSET_FIELDLAYOUT_OFFSET]))
	{
		return IL_LOADERR_MEMORY;
	}
	return 0;
}

/*
 * Load a field marshal token.
 */
static int Load_FieldMarshal(ILImage *image, ILUInt32 *values,
					     	 ILUInt32 *valuesNext, ILToken token,
					     	 void *userData)
{
	ILFieldMarshal *marshal;
	ILProgramItem *owner;

	/* Get the owner */
	owner = ILProgramItem_FromToken
				(image, values[IL_OFFSET_FIELDMARSHAL_TOKEN]);

	/* If the owner is NULL, then turn off the "sorted" flag
	   within the image.  Some images seem to have invalid
	   parameter indexes, which can cause problems later
	   in ILImageSearchForToken */
	if(!owner)
	{
		image->sorted &=
			~(((ILUInt64)1) << (IL_META_TOKEN_FIELD_MARSHAL >> 24));
	}

	/* Create the field marshal record */
	marshal = ILFieldMarshalCreate(image, token, owner);
	if(!marshal)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Add the type information to the record */
	_ILFieldMarshalSetTypeIndex
			(marshal, values[IL_OFFSET_FIELDMARSHAL_TYPE_RAW]);

	/* Done */
	return 0;
}

/*
 * Load a class layout token.
 */
static int Load_ClassLayout(ILImage *image, ILUInt32 *values,
					        ILUInt32 *valuesNext, ILToken token,
					        void *userData)
{
	if(!ILClassLayoutCreate(image, token,
						    ILClass_FromToken
								(image, values[IL_OFFSET_CLASSLAYOUT_TYPE]),
						 	values[IL_OFFSET_CLASSLAYOUT_PACKING],
						 	values[IL_OFFSET_CLASSLAYOUT_SIZE]))
	{
		return IL_LOADERR_MEMORY;
	}
	return 0;
}

/*
 * Load a security token.
 */
static int Load_DeclSecurity(ILImage *image, ILUInt32 *values,
					     	 ILUInt32 *valuesNext, ILToken token,
					     	 void *userData)
{
	ILDeclSecurity *security;

	/* Create the security record */
	security = ILDeclSecurityCreate(image, token,
						ILProgramItem_FromToken
							(image, values[IL_OFFSET_DECLSECURITY_TOKEN]),
						values[IL_OFFSET_DECLSECURITY_TYPE]);
	if(!security)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Add the blob information to the record */
	_ILDeclSecuritySetBlobIndex
			(security, values[IL_OFFSET_DECLSECURITY_DATA_RAW]);

	/* Done */
	return 0;
}

/*
 * Load a file declaration token.
 */
static int Load_File(ILImage *image, ILUInt32 *values,
				   	 ILUInt32 *valuesNext, ILToken token,
				   	 void *userData)
{
	ILFileDecl *decl;

	/* Create the file declaration record */
	decl = ILFileDeclCreate(image, token,
							ILImageGetString
								(image, values[IL_OFFSET_FILE_NAME]),
							values[IL_OFFSET_FILE_ATTRS]);
	if(!decl)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Add the hash information to the record */
	_ILFileDeclSetHashIndex(decl, values[IL_OFFSET_FILE_HASH_RAW]);

	/* Done */
	return 0;
}

/*
 * Load a manifest resource token.
 */
static int Load_ManifestRes(ILImage *image, ILUInt32 *values,
				   	 		ILUInt32 *valuesNext, ILToken token,
				   	 		void *userData)
{
	ILManifestRes *res;
	ILProgramItem *owner;
	ILFileDecl *decl;
	ILAssembly *assem;

	/* Create the manifest resource record */
	res = ILManifestResCreate(image, token,
							  ILImageGetString
								(image, values[IL_OFFSET_MANIFESTRES_NAME]),
							  values[IL_OFFSET_MANIFESTRES_ATTRS]);
	if(!res)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Add the owner information to the record */
	owner = ILProgramItem_FromToken(image, values[IL_OFFSET_MANIFESTRES_IMPL]);
	if((decl = ILProgramItemToFileDecl(owner)) != 0)
	{
		ILManifestResSetOwnerFile(res, decl,
								  values[IL_OFFSET_MANIFESTRES_OFFSET]);
	}
	else if((assem = ILProgramItemToAssembly(owner)) != 0)
	{
		ILManifestResSetOwnerAssembly(res, assem);
	}

	/* Done */
	return 0;
}

/*
 * Load an exported type token.
 */
static int Load_ExportedType(ILImage *image, ILUInt32 *values,
				   	 		 ILUInt32 *valuesNext, ILToken token,
				   	 		 void *userData)
{
	ILExportedType *type;
	ILProgramItem *scope;
	ILFileDecl *decl;
	ILAssembly *assem;
	ILExportedType *expType;

	/* Create the exported type record */
	type = ILExportedTypeCreate(image, token,
				  	values[IL_OFFSET_EXPTYPE_ATTRS],
				    ILImageGetString
						(image, values[IL_OFFSET_EXPTYPE_NAME]),
				    ILImageGetString
						(image, values[IL_OFFSET_EXPTYPE_NAMESPACE]));
	if(!type)
	{
		return IL_LOADERR_MEMORY;
	}

	/* Set the foreign class identifier */
	ILExportedTypeSetId(type, values[IL_OFFSET_EXPTYPE_CLASS]);

	/* Set the scope */
	scope = ILProgramItem_FromToken(image, values[IL_OFFSET_EXPTYPE_FILE]);
	if((decl = ILProgramItemToFileDecl(scope)) != 0)
	{
		ILExportedTypeSetScopeFile(type, decl);
	}
	else if((assem = ILProgramItemToAssembly(scope)) != 0)
	{
		ILExportedTypeSetScopeAssembly(type, assem);
	}
	else if((expType = ILProgramItemToExportedType(scope)) != 0)
	{
		ILExportedTypeSetScopeType(type, expType);
	}

	/* Done */
	return 0;
}

int _ILImageBuildMetaStructures(ILImage *image, const char *filename,
								int loadFlags)
{
	int error;
	int needPhase2;

	/* Load information about modules and assemblies */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_MODULE,
							 Load_Module, 0));
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_MODULE_REF,
							 Load_ModuleRef, 0));
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_ASSEMBLY,
							 Load_Assembly, 0));
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_ASSEMBLY_REF,
							 Load_AssemblyRef, 0));
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_OS_DEF,
							 Load_OSDef, 0));
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_PROCESSOR_DEF,
							 Load_ProcessorDef, 0));
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_OS_REF,
							 Load_OSRef, 0));
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_PROCESSOR_REF,
							 Load_ProcessorRef, 0));

	/* Load the assemblies that this image depends upon */
	if((loadFlags & IL_LOADFLAG_NO_RESOLVE) == 0)
	{
		error = _ILImageDynamicLink(image, filename, loadFlags);
		if(error != 0)
		{
			return error;
		}
	}

	/* Perform phase 1 type resolution on the TypeRef table */
	error = ResolveTypeRefs(image, loadFlags);
	if(error > 0)
	{
		return error;
	}
	needPhase2 = (error < 0);

	/* Load the TypeDef table - phase 1 */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_TYPE_DEF,
							 Load_TypeDef, 0));

	/* Load the TypeSpec table */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_TYPE_SPEC,
							 Load_TypeSpec, 0));

	/* Perform phase 2 type resolution on the TypeRef table */
	if(needPhase2)
	{
		EXIT_IF_ERROR(ResolveTypeRefsPhase2(image, loadFlags));
	}

	/* Load the TypeDef table - phase 2 (fields and methods) */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_TYPE_DEF,
							 Load_TypeDefPhase2, 0));

	/* Load the InterfaceImpl table */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_INTERFACE_IMPL,
							 Load_InterfaceImpl, 0));

	/* Load the PInvoke definitions */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_IMPL_MAP,
							 Load_PInvoke, 0));

	/* Load events and properties for all of the types */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_EVENT_MAP,
							 Load_EventAssociation, 0));
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_PROPERTY_MAP,
							 Load_PropertyAssociation, 0));
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_METHOD_SEMANTICS,
							 Load_MethodAssociation, 0));

	/* Load member references to other images */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_MEMBER_REF,
							 Load_MemberRef, (void *)(ILNativeInt)loadFlags));

	/* Load the override declarations */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_METHOD_IMPL,
							 Load_Override, 0));

	/* Load the custom attributes for all of the above */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_CUSTOM_ATTRIBUTE,
							 Load_CustomAttr, 0));

	/* Load the stand-alone signatures */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_STAND_ALONE_SIG,
							 Load_StandAloneSig, 0));

	/* Load constants */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_CONSTANT,
							 Load_Constant, 0));

	/* Load field RVA declarations */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_FIELD_RVA,
							 Load_FieldRVA, 0));

	/* Load field layout declarations */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_FIELD_LAYOUT,
							 Load_FieldLayout, 0));

	/* Load field marshal declarations */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_FIELD_MARSHAL,
							 Load_FieldMarshal, 0));

	/* Load class layout declarations */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_CLASS_LAYOUT,
							 Load_ClassLayout, 0));

	/* Load security declarations */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_DECL_SECURITY,
							 Load_DeclSecurity, 0));

	/* Load file declarations */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_FILE,
							 Load_File, 0));

	/* Load manifest resource declarations */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_MANIFEST_RESOURCE,
							 Load_ManifestRes, 0));

	/* Load exported type declarations */
	EXIT_IF_ERROR(LoadTokens(image, IL_META_TOKEN_EXPORTED_TYPE,
							 Load_ExportedType, 0));

	/* Done */
	return 0;
}

#if 0

/*
 * Table of all token loading functions.
 */
static TokenLoadFunc const TokenLoadFunctions[] = {
	Load_Module,				/* 00 */
	0, /*Load_TypeRef,*/
	Load_TypeDef,
	0,
	Load_FieldDef,
	0,
	Load_MethodDef,
	0,
	Load_ParamDef,				/* 08 */
	Load_InterfaceImpl,
	Load_MemberRef,
	Load_Constant,
	Load_CustomAttr,
	Load_FieldMarshal,
	Load_DeclSecurity,
	Load_ClassLayout,
	Load_FieldLayout,			/* 10 */
	Load_StandAloneSig,
	0, /*Load_EventMap,*/
	0,
	Load_Event,
	0, /*Load_PropertyMap,*/
	0,
	Load_Property,
	0, /*Load_MethodSemantics,*/		/* 18 */
	0, /*Load_MethodImpl,*/
	Load_ModuleRef,
	Load_TypeSpec,
	0, /*Load_ImplMap,*/
	Load_FieldRVA,
	0,
	0,
	Load_Assembly,				/* 20 */
	Load_ProcessorDef,
	Load_OSDef,
	Load_AssemblyRef,
	Load_ProcessorRef,
	Load_OSRef,
	Load_File,
	Load_ExportedType,
	0, /*Load_ManifestResource,*/		/* 28 */
	0, /*Load_NestedClass,*/
	0,
	0,
	0,
	0,
	0,
	0,
	0,							/* 30 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,							/* 38 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

void *_ILImageLoadOnDemand(ILImage *image, ILToken token)
{
	ILUInt32 values[IL_IMAGE_TOKEN_COLUMNS];
	ILUInt32 valuesNext[IL_IMAGE_TOKEN_COLUMNS];
	ILUInt32 *pvaluesNext;
	TokenLoadFunc func;
	void **data;

	/* Load the values for the token */
	if(!_ILImageRawTokenData(image, token, values))
	{
		return 0;
	}

	/* Load the values for the following token, in case it is a range */
	if((token + 1) <= ((token & IL_META_TOKEN_MASK) |
					   image->tokenCount[token >> 24]))
	{
		if(!_ILImageRawTokenData(image, token + 1, valuesNext))
		{
			return 0;
		}
		pvaluesNext = valuesNext;
	}
	else
	{
		pvaluesNext = 0;
	}

	/* Find the loading function for this token type */
	func = TokenLoadFunctions[token >> 24];
	if(!func)
	{
		return 0;
	}

	/* Load the token information */
	if((*func)(image, values, pvaluesNext, token, 0) != 0)
	{
		/* A metadata error was detected */
		return 0;
	}

	/* Retrieve the program item from the token table */
	data = image->tokenData[token >> 24];
	if(data)
	{
		return data[(token & ~IL_META_TOKEN_MASK) - 1];
	}
	return 0;
}

#else

void *_ILImageLoadOnDemand(ILImage *image, ILToken token)
{
	return 0;
}

#endif

#ifdef	__cplusplus
};
#endif
