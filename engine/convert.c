/*
 * convert.c - Convert methods using a coder.
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

#include "engine_private.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Locate or load an external module that is being referenced via "PInvoke".
 * Returns the system module pointer, or NULL if it could not be loaded.
 */
static void *LocateExternalModule(ILExecProcess *process, const char *name,
								  ILPInvoke *pinvoke)
{
	ILLoadedModule *loaded;
	char *pathname;

	/* Search for an already-loaded module with the same name */
	loaded = process->loadedModules;
	while(loaded != 0)
	{
		if(!ILStrICmp(loaded->name, name))
		{
			return loaded->handle;
		}
		loaded = loaded->next;
	}

	/* Create a new module structure.  We keep this structure even
	   if we cannot load the actual module.  This ensures that
	   future requests for the same module will be rejected without
	   re-trying the open */
	loaded = (ILLoadedModule *)ILMalloc(sizeof(ILLoadedModule) + strlen(name));
	if(!loaded)
	{
		return 0;
	}
	loaded->next = process->loadedModules;
	loaded->handle = 0;
	strcpy(loaded->name, name);
	process->loadedModules = loaded;

	/* Resolve the module name to a library name */
	pathname = ILPInvokeResolveModule(pinvoke);
	if(!pathname)
	{
		return 0;
	}

	/* Attempt to open the module */
	loaded->handle = ILDynLibraryOpen(pathname);
	ILFree(pathname);
	return loaded->handle;
}

unsigned char *_ILConvertMethod(ILExecThread *thread, ILMethod *method)
{
	ILMethodCode code;
	ILPInvoke *pinv;
	ILCoder *coder = thread->process->coder;
	unsigned char *start;
	void *fn;
	void *ctorfn;
	void *cif;
	void *ctorcif;
	int isConstructor;
	ILModule *module;
	const char *name;
	void *moduleHandle;
	int result;

	/* Is the method already converted and in the current generation? */
	if(method->userData1 != 0 &&
	   method->userData2 == (void *)(thread->process->coderGeneration))
	{
		return (unsigned char *)(method->userData1);
	}

	/* Get the method code */
	if(!ILMethodGetCode(method, &code))
	{
		code.code = 0;
	}

	/* The conversion is different depending upon whether the
	   method is written in IL or not */
	if(code.code)
	{
		/* Use the bytecode verifier and coder to convert the method */
		if(!_ILVerify(coder, &start, method,
					  &code, 1 /* TODO: safe vs unsafe */))
		{
			return 0;
		}
		thread->process->coderGeneration = ILCoderGeneration(coder);
	}
	else
	{
		/* This is a "PInvoke", "internalcall", or "runtime" method */
		pinv = ILPInvokeFind(method);
		fn = 0;
		ctorfn = 0;
		isConstructor = ILMethod_IsConstructor(method);
		switch(method->implementAttrs &
					(IL_META_METHODIMPL_CODE_TYPE_MASK |
					 IL_META_METHODIMPL_INTERNAL_CALL |
					 IL_META_METHODIMPL_JAVA))
		{
			case IL_META_METHODIMPL_IL:
			case IL_META_METHODIMPL_OPTIL:
			{
				/* If we don't have a PInvoke record, then we don't
				   know what to map this method call to */
				if(!pinv)
				{
					return 0;
				}

				/* Find the module for the PInvoke record */
				module = ILPInvoke_Module(pinv);
				if(!module)
				{
					return 0;
				}
				name = ILModule_Name(module);
				if(!name || *name == '\0')
				{
					return 0;
				}
				moduleHandle = LocateExternalModule
									(thread->process, name, pinv);
				if(!moduleHandle)
				{
					return 0;
				}

				/* Get the name of the function within the module */
				name = ILPInvoke_Alias(pinv);
				if(!name || *name == '\0')
				{
					name = ILMethod_Name(method);
				}

				/* Look up the method within the module */
				fn = ILDynLibraryGetSymbol(moduleHandle, name);
				if(!fn)
				{
					return 0;
				}
			}
			break;

			case IL_META_METHODIMPL_RUNTIME:
			case IL_META_METHODIMPL_IL | IL_META_METHODIMPL_INTERNAL_CALL:
			{
				/* "internalcall" and "runtime" methods must not
				   have PInvoke records associated with them */
				if(pinv)
				{
					return 0;
				}
				fn = _ILFindInternalCall(method, 0);
				if(isConstructor)
				{
					ctorfn = _ILFindInternalCall(method, 1);
				}
			}
			break;

			default:
			{
				/* No idea how to invoke this method */
				return 0;
			}
			/* Not reached */
		}

		/* Bail out if we did not find the underlying native method */
		if(!fn && !ctorfn)
		{
			return 0;
		}

		/* Generate a "cif" structure to handle the native call details */
		if(fn)
		{
			/* Make the "cif" structure for the normal method entry */
			cif = _ILMakeCifForMethod(coder, method, (pinv == 0));
			if(!cif)
			{
			#if 0
				/* TODO */
				/* The coder ran out of memory, so flush it and retry */
				ILCoderFlush(coder);
				thread->process->coderGeneration = ILCoderGeneration(coder);
				cif = _ILMakeCifForMethod(coder, method, (pinv == 0));
				if(!cif)
				{
					return 0;
				}
			#endif
				return 0;
			}
		}
		else
		{
			cif = 0;
		}
		if(ctorfn)
		{
			/* Make the "cif" structure for the allocating constructor */
			ctorcif = _ILMakeCifForConstructor(coder, method, (pinv == 0));
			if(!ctorcif)
			{
			#if 0
				/* TODO */
				/* The coder ran out of memory, so flush it and retry */
				ILCoderFlush(coder);
				thread->process->coderGeneration = ILCoderGeneration(coder);
				if(fn)
				{
					cif = _ILMakeCifForMethod(coder, method, (pinv == 0));
				}
				ctorcif = _ILMakeCifForConstructor(coder, method, (pinv == 0));
				if(!cif || !ctorcif)
				{
					return 0;
				}
			#endif
				return 0;
			}
		}
		else
		{
			ctorcif = 0;
		}

		/* Generate the coder stub for marshalling the call */
		if(!isConstructor)
		{
			/* We only need the method entry point */
			if(!ILCoderSetupExtern(coder, &start, method,
								   fn, cif, (pinv == 0)))
			{
				return 0;
			}
			while((result = ILCoderFinish(coder)) != IL_CODER_END_OK)
			{
				/* Do we need a coder restart due to cache overflow? */
				if(result != IL_CODER_END_RESTART)
				{
					return 0;
				}
				thread->process->coderGeneration = ILCoderGeneration(coder);
				cif = _ILMakeCifForMethod(coder, method, (pinv == 0));
				if(!ILCoderSetupExtern(coder, &start, method,
									   fn, cif, (pinv == 0)))
				{
					return 0;
				}
			}
		}
		else
		{
			/* We need both the method and constructor entry points */
			if(!ILCoderSetupExternCtor(coder, &start, method,
								       fn, cif, ctorfn, ctorcif,
									   (pinv == 0)))
			{
				return 0;
			}
			while((result = ILCoderFinish(coder)) != IL_CODER_END_OK)
			{
				/* Do we need a coder restart due to cache overflow? */
				if(result != IL_CODER_END_RESTART)
				{
					return 0;
				}
				thread->process->coderGeneration = ILCoderGeneration(coder);
				if(fn)
				{
					cif = _ILMakeCifForMethod(coder, method, (pinv == 0));
				}
				if(ctorfn)
				{
					ctorcif = _ILMakeCifForConstructor
									(coder, method, (pinv == 0));
				}
				if(!ILCoderSetupExternCtor(coder, &start, method,
									       fn, cif, ctorfn, ctorcif,
										   (pinv == 0)))
				{
					return 0;
				}
			}
		}
	}

	/* The method is in the current generation now */
	method->userData1 = (void *)start;
	method->userData2 = (void *)(thread->process->coderGeneration);
	return (unsigned char *)(method->userData1);
}

#ifdef	__cplusplus
};
#endif
