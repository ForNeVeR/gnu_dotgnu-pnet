/*
 * cs_attrs.c - Attribute handling.
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

#include "cs_internal.h"

#ifdef	__cplusplus
extern	"C" {
#endif

static void AddDllImport(ILProgramItem *item, const char *dllName)
{
	ILModule *module;
	module = ILModuleRefCreateUnique(ILProgramItem_Image(item), dllName);
	if(!module)
	{
		CCOutOfMemory();
	}
	if(ILPInvokeCreate((ILMethod *)item, 0,
					   IL_META_PINVOKE_CALL_CONV_CDECL, module, 0) == 0)
	{
		CCOutOfMemory();
	}
	ILMemberSetAttrs((ILMember *)item,
					 IL_META_METHODDEF_PINVOKE_IMPL,
					 IL_META_METHODDEF_PINVOKE_IMPL);
}

void CSProcessAttrs(ILGenInfo *info, ILProgramItem *item,
					ILNode *attributes, int target)
{
	/* TODO */

	/* Quick and dirty hack: recognise the "DllImport" attribute */
	if(target == CS_ATTR_METHOD && attributes != 0)
	{
		ILNode_ListIter iter1;
		ILNode_ListIter iter2;
		ILNode_ListIter iter3;
		ILNode *section, *attr;
		ILNode *args, *arg;
		CSSemValue value;
		ILEvalValue evalValue;
		char *name;
		ILNode_ListIter_Init(&iter1, ((ILNode_AttributeTree *)attributes)
											->sections);
		while((section = ILNode_ListIter_Next(&iter1)) != 0)
		{
			ILNode_ListIter_Init(&iter2, ((ILNode_AttributeSection *)section)
												->attrs);
			while((attr = ILNode_ListIter_Next(&iter2)) != 0)
			{
				name = ILQualIdentName(((ILNode_Attribute *)attr)->name, 0);
				if(!strcmp(name, "DllImport") ||
				   !strcmp(name, "DllImportAttribute"))
				{
					args = ((ILNode_AttrArgs *)
								(((ILNode_Attribute *)attr)->args))
							->positionalArgs;
					ILNode_ListIter_Init(&iter3, args);
					while((arg = ILNode_ListIter_Next(&iter3)) != 0)
					{
						if(CSSemExpectValue(arg, info, &arg, &value) &&
						   ILTypeIsStringClass(value.type))
						{
							if(ILNode_EvalConst(arg, info, &evalValue) &&
							   evalValue.valueType == ILMachineType_String)
							{
								AddDllImport(item,
									ILInternString(evalValue.un.strValue.str,
												   evalValue.un.strValue.len)
										.string);
							}
						}
					}
				}
			}
		}
	}
}

void CSProcessAttrsForParam(ILGenInfo *info, ILMethod *method,
							unsigned long paramNum,
							ILNode *attributes)
{
	/* TODO */
}

#ifdef	__cplusplus
};
#endif
