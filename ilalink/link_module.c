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

void _ILLinkerCreateAttribute(ILLinker *linker, ILProgramItem *item,
							  const char *name, const char *namespace,
							  ILType *arg1Type, ILType *arg2Type,
							  void *data, int len)
{
	ILClass *attrClass;
	ILType *signature;
	ILMethod *method;
	ILAttribute *attr;

	/* Import the attribute class from the library that it lives in */
	attrClass = _ILLinkerFindByName(linker, name, namespace);
	if(!attrClass)
	{
		return;
	}

	/* Create the constructor signature that we are interested in */
	signature = ILTypeCreateMethod(linker->context, ILType_Void);
	if(!signature)
	{
		_ILLinkerOutOfMemory(linker);
		return;
	}
	if(arg1Type)
	{
		if(!ILTypeAddParam(linker->context, signature, arg1Type))
		{
			_ILLinkerOutOfMemory(linker);
			return;
		}
	}
	if(arg2Type)
	{
		if(!ILTypeAddParam(linker->context, signature, arg2Type))
		{
			_ILLinkerOutOfMemory(linker);
			return;
		}
	}
	ILTypeSetCallConv(signature, IL_META_CALLCONV_HASTHIS);

	/* Search for the constructor or create a new reference */
	if((method = (ILMethod *)ILClassNextMemberMatch
			(attrClass, (ILMember *)0,
			 IL_META_MEMBERKIND_METHOD, ".ctor", signature)) == 0)
	{
		method = ILMethodCreate(attrClass, (ILToken)IL_MAX_UINT32, ".ctor", 0);
		if(!method)
		{
			_ILLinkerOutOfMemory(linker);
			return;
		}
		ILMemberSetSignature((ILMember *)method, signature);
		ILMethodSetCallConv(method, IL_META_CALLCONV_HASTHIS);
	}

	/* Create and add the attribute */
	attr = ILAttributeCreate(linker->image, 0);
	if(!attr)
	{
		_ILLinkerOutOfMemory(linker);
		return;
	}
	ILProgramItemAddAttribute(item, attr);
	ILAttributeSetType(attr, (ILProgramItem *)method);
	if(!ILAttributeSetValue(attr, data, (unsigned)len))
	{
		_ILLinkerOutOfMemory(linker);
		return;
	}
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

int ILLinkerCMemoryModel(ILImage *image, int *alignFlags)
{
	ILModule *module;
	ILAttribute *attr;
	ILSerializeReader *reader;
	int model, hasAlign;

	/* Find the main module within this assembly */
	module = ILModule_FromToken(image, IL_META_TOKEN_MODULE | 1);
	if(!module)
	{
		return 0;
	}

	/* Check the module to see if it has the "MemoryModel" attribute */
	hasAlign = 0;
	attr = ILLinkerFindAttribute
			(ILToProgramItem(module), "MemoryModelAttribute", "OpenSystem.C",
			 ILType_Int32, ILType_Invalid);
	if(!attr)
	{
		attr = ILLinkerFindAttribute
			(ILToProgramItem(module), "MemoryModelAttribute", "OpenSystem.C",
			 ILType_Int32, ILType_Int32);
		if(!attr)
		{
			return 0;
		}
		hasAlign = 1;
	}

	/* Read the memory model value from the attribute */
	reader = ILLinkerReadAttribute(attr);
	if(!reader)
	{
		return 0;
	}
	model = 0;
	*alignFlags = 0;
	if(ILSerializeReaderGetParamType(reader) == IL_META_ELEMTYPE_I4)
	{
		model = (int)ILSerializeReaderGetInt32(reader, IL_META_ELEMTYPE_I4);
		if(hasAlign)
		{
			if(ILSerializeReaderGetParamType(reader) == IL_META_ELEMTYPE_I4)
			{
				*alignFlags = (int)ILSerializeReaderGetInt32
						(reader, IL_META_ELEMTYPE_I4);
			}
		}
	}
	ILSerializeReaderDestroy(reader);
	return model;
}

const char *_ILLinkerModuleName(ILLinker *linker)
{
	if(linker->moduleClass)
	{
		return linker->moduleName;
	}
	else
	{
		return "<Module>";
	}
}

ILClass *_ILLinkerModuleClass(ILLinker *linker)
{
	if(linker->moduleClass)
	{
		return linker->moduleClass;
	}
	else
	{
		return ILClassLookup(ILClassGlobalScope(linker->image), "<Module>", 0);
	}
}

void ILLinkerModuleCreate(ILLinker *linker)
{
	ILClass *parent;
	if(!(linker->moduleClass))
	{
		if(!strcmp(linker->moduleName, IL_LINKER_EXE_MODULE_NAME))
		{
			/* Creating a module class for an executable */
			linker->moduleClass = ILClassLookup
				(ILClassGlobalScope(linker->image), "<Module>", 0);
			ILClassSetAttrs(linker->moduleClass, ~((ILUInt32)0),
							IL_META_TYPEDEF_PUBLIC |
							IL_META_TYPEDEF_ABSTRACT);
		}
		else
		{
			/* Creating a module class for a library */
			parent = _ILLinkerFindByName(linker, "Object", "System");
			if(!parent)
			{
				return;
			}
			linker->moduleClass = ILClassCreate
				(ILClassGlobalScope(linker->image), 0,
				 linker->moduleName, 0, parent);
			if(!(linker->moduleClass))
			{
				_ILLinkerOutOfMemory(linker);
				return;
			}
			ILClassSetAttrs(linker->moduleClass, ~((ILUInt32)0),
							IL_META_TYPEDEF_PUBLIC |
							IL_META_TYPEDEF_SEALED);

			/* Add the "OpenSystem.C.ModuleScope" attribute to the
			   class to indicate that it is a C module */
			_ILLinkerCreateAttribute
				(linker, ILToProgramItem(linker->moduleClass),
				 "ModuleScopeAttribute", "OpenSystem.C",
				 0, 0, "\1\0\0\0", 4);
		}
	}
}

int _ILLinkerIsModule(ILClass *classInfo)
{
	const char *name = ILClass_Name(classInfo);
	const char *namespace = ILClass_Namespace(classInfo);
	if(!strcmp(name, IL_LINKER_EXE_MODULE_NAME) ||
	   !strcmp(name, IL_LINKER_DLL_MODULE_NAME))
	{
		if(!namespace)
		{
			return 1;
		}
	}
	return (ILLinkerFindAttribute(ILToProgramItem(classInfo),
							 	  "ModuleScopeAttribute",
							 	  "OpenSystem.C", 0, 0) != 0);
}

int _ILLinkerIsGlobalScope(ILClass *classInfo)
{
	return (ILLinkerFindAttribute(ILToProgramItem(classInfo),
							 	  "GlobalScopeAttribute",
							 	  "OpenSystem.C", 0, 0) != 0);
}

#ifdef	__cplusplus
};
#endif
