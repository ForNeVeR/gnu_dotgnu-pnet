/*
 * c_internal.c - Internal definitions for the C compiler front end.
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

#include <cscc/c/c_internal.h>

#ifdef	__cplusplus
extern	"C" {
#endif

int gen_32bit_only;
CSemValue CSemValueDefault = {C_SEMKIND_VOID, ILType_Void, 0};
CSemValue CSemValueBool = {C_SEMKIND_RVALUE | C_SEMKIND_BOOLEAN,
						   ILType_Boolean, 0};
CSemValue CSemValueError = {C_SEMKIND_ERROR, ILType_Void, 0};


void *CSemDupExtra(const void *buf, unsigned int len)
{
	void *temp = yynodealloc(len);
	ILMemCpy(temp, buf, len);
	return temp;
}

CSemValue CSemInlineAnalysis(ILGenInfo *info, ILNode *node, ILScope *scope)
{
	ILScope *currentScope = info->currentScope;
	CSemValue result;
	info->currentScope = scope;
	result = ILNode_CSemAnalysis(node, info, &node, 1);
	info->currentScope = currentScope;
	return result;
}

void CGenBeginCode(ILGenInfo *info)
{
	/* Register the builtin library */
	CGenRegisterLibrary(info);

	/* Tag the module with the memory model, which tells the linker
	   that this is a C module requiring special treatment */
	if(info->asmOutput)
	{
		if(gen_32bit_only)
		{
			fputs(".custom instance void "
					"OpenSystem.Languages.C.MemoryModelAttribute"
					"::.ctor(int32) = (01 00 20 00 00 00 00 00)\n",
				  info->asmOutput);
		}
		else
		{
			fputs(".custom instance void "
					"OpenSystem.Languages.C.MemoryModelAttribute"
					"::.ctor(int32) = (01 00 40 00 00 00 00 00)\n",
				  info->asmOutput);
		}
	}

	/* Initialize the global definition scope */
	CScopeGlobalInit(info);

	/* We don't use CCParseTree, but we need to initialize it
	   to make sure that "common/cc_main.c" has something */
	CCParseTree = ILNode_Empty_create();

	/* Mark the current treecc node pool location */
	yynodepush();

	/* Pre-declare builtin definitions */
	CFunctionPredeclare(info);
}

void CGenEndCode(ILGenInfo *info)
{
	FILE *stream = info->asmOutput;

	/* Output pending class definitions */
	if(stream != 0)
	{
		CTypeOutputPending(info, stream);
	}

	/* Output the string constant pool */
	CGenStringPool(info);

	/* Output the "crt0" code if this module has a "main" function */
	if(stream != 0)
	{
		CGenCrt0(info, stream);
	}
}

#ifdef	__cplusplus
};
#endif
