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
#include <codegen/cg_nodemap.h>

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

/*
 * Get the "field" target for a program item.
 */
static ILProgramItem *GetFieldTarget(ILGenInfo *info, ILProgramItem *item)
{
	ILEvent *event;
	ILNode_EventDeclarator *eventNode;

	/* If the item is already a field, then it is the requested target */
	if(ILProgramItemToField(item) != 0)
	{
		return item;
	}

	/* Determine if the item is a non-abstract event */
	event = ILProgramItemToEvent(item);
	if(event == 0)
	{
		return 0;
	}
	eventNode = (ILNode_EventDeclarator *)
		ILProgramItemToNode(info, ILToProgramItem(event));
	if(!eventNode)
	{
		return 0;
	}

	/* Return the backing field for the event, if there is one */
	return ILToProgramItem(eventNode->backingField);
}

/*
 * Get a particular "param" target for a method.
 */
static ILProgramItem *GetParamTarget(ILGenInfo *info, ILMethod *method,
									 unsigned long paramNum)
{
	ILParameter *param;

	/* If we are looking for the return value, then the
	   return type must not be "void" */
	if(paramNum == 0 &&
	   ILTypeGetReturn(ILMethod_Signature(method)) == ILType_Void)
	{
		return 0;
	}

	/* Look for a pre-existing parameter record */
	param = 0;
	while((param = ILMethodNextParam(method, param)) != 0)
	{
		if(ILParameter_Num(param) == paramNum)
		{
			return ILToProgramItem(param);
		}
	}

	/* Create a new parameter record */
	param = ILParameterCreate
		(method, 0, 0,
		 ((paramNum == 0) ? IL_META_PARAMDEF_RETVAL : 0),
	     (ILUInt32)paramNum);
	if(!param)
	{
		CCOutOfMemory();
	}
	return ILToProgramItem(param);
}

/*
 * Process a single attribute in a section.
 */
static void ProcessAttr(ILGenInfo *info, ILProgramItem *item,
						int target, ILNode_Attribute *attr)
{
	/* Quick and dirty hack: recognise the "DllImport" attribute */
	if(target == CS_ATTR_METHOD)
	{
		ILNode_ListIter iter3;
		ILNode *args, *arg;
		CSSemValue value;
		ILEvalValue evalValue;
		char *name;

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

void CSProcessAttrs(ILGenInfo *info, ILProgramItem *mainItem,
					ILNode *attributes, int mainTarget)
{
	ILProgramItem *item;
	int target;
	ILNode_ListIter iter;
	ILNode_ListIter iter2;
	ILNode_AttributeSection *section;
	ILNode_Attribute *attr;
	char *targetName;
	ILClass *classInfo;
	ILMethod *method;
	unsigned long numParams;

	/* Bail out if we don't have any attributes */
	if(!attributes)
	{
		return;
	}

	/* Scan through the attribute sections */
	ILNode_ListIter_Init(&iter, ((ILNode_AttributeTree *)attributes)
										->sections);
	while((section = (ILNode_AttributeSection *)
				ILNode_ListIter_Next(&iter)) != 0)
	{
		/* Resolve the target item */
		item = mainItem;
		target = mainTarget;
		switch(section->type)
		{
			case ILAttrTargetType_None:		break;

			case ILAttrTargetType_Named:
			{
				targetName = ILQualIdentName(section->target, 0);
				if(!strcmp(targetName, "assembly"))
				{
					/* Assembly targets can be applied anywhere */
					item = (ILProgramItem *)ILAssembly_FromToken
						(ILProgramItem_Image(item), IL_META_TOKEN_ASSEMBLY | 1);
					target = CS_ATTR_ASSEMBLY;
				}
				else if(!strcmp(targetName, "module"))
				{
					/* Module targets can be applied anywhere */
					item = (ILProgramItem *)ILModule_FromToken
						(ILProgramItem_Image(item), IL_META_TOKEN_MODULE | 1);
					target = CS_ATTR_MODULE;
				}
				else if(!strcmp(targetName, "field"))
				{
					/* Field targets can apply to fields or events */
					item = GetFieldTarget(info, item);
					target = CS_ATTR_FIELD;
				}
				else if(!strcmp(targetName, "method"))
				{
					/* Method targets can apply to methods,
					   constructors, or operators */
					if(ILProgramItemToMethod(item) == 0)
					{
						item = 0;
					}
				}
				else if(!strcmp(targetName, "param"))
				{
					/* Parameter targets can apply to parameter records,
					   or to the first parameter of a method, event accessor,
					   or property set accessor */
					method = ILProgramItemToMethod(item);
					if(method != 0)
					{
						numParams = ILTypeNumParams(ILMethod_Signature(method));
						if(numParams == 1)
						{
							item = GetParamTarget(info, method, 1);
							target = CS_ATTR_PARAMETER;
						}
						else
						{
							item = 0;
						}
					}
					else if(ILProgramItemToParameter(item) == 0)
					{
						item = 0;
					}
				}
				else if(!strcmp(targetName, "property"))
				{
					/* Property targets can only apply to properties */
					if(ILProgramItemToProperty(item) == 0)
					{
						item = 0;
					}
				}
				else if(!strcmp(targetName, "type"))
				{
					/* Type targets can apply to classes, structures,
					   enumerated types, and delegates */
					if(ILProgramItemToClass(item) == 0)
					{
						item = 0;
					}
				}
				else
				{
					CCErrorOnLine(yygetfilename(section),
								  yygetlinenum(section),
								  _("invalid attribute target type `%s'"),
								  targetName);
					continue;
				}
				if(!item)
				{
					CCErrorOnLine
						(yygetfilename(section),
						 yygetlinenum(section),
						 _("attribute target type `%s' is not appropriate "
						   "in this context"), targetName);
					continue;
				}
			}
			break;

			case ILAttrTargetType_Event:
			{
				/* Event targets can only apply to events */
				if(ILProgramItemToEvent(item) == 0)
				{
					CCErrorOnLine
						(yygetfilename(section),
						 yygetlinenum(section),
						 _("attribute target type `event' is not appropriate "
						   "in this context"));
					continue;
				}
			}
			break;

			case ILAttrTargetType_Return:
			{
				/* Return targets can apply to methods, operators,
				   and delegates that return non-void */
				if(ILProgramItemToMethod(item) != 0)
				{
					item = GetParamTarget
						(info, ILProgramItemToMethod(item), 0);
				}
				else if((classInfo = ILProgramItemToClass(item)) != 0 &&
				        ILTypeIsDelegate(ILType_FromClass(classInfo)))
				{
					item = GetParamTarget
						(info, ILTypeGetDelegateMethod
									(ILType_FromClass(classInfo)), 0);
				}
				else
				{
					item = 0;
				}
				if(!item)
				{
					CCErrorOnLine
						(yygetfilename(section),
						 yygetlinenum(section),
						 _("attribute target type `return' is not appropriate "
						   "in this context"));
					continue;
				}
				target = CS_ATTR_RETURNVALUE;
			}
			break;
		}

		/* Process the attributes in this section */
		ILNode_ListIter_Init(&iter2, section->attrs);
		while((attr = (ILNode_Attribute *)ILNode_ListIter_Next(&iter2)) != 0)
		{
			ProcessAttr(info, item, target, attr);
		}

		/* Convert system library attributes into metadata structures */
		if(!ILProgramItemConvertAttrs(item))
		{
			CCOutOfMemory();
		}
	}
}

void CSProcessAttrsForParam(ILGenInfo *info, ILMethod *method,
							unsigned long paramNum,
							ILNode *attributes)
{
	ILProgramItem *item;

	/* Bail out if there are no parameter attributes */
	if(!attributes)
	{
		return;
	}

	/* Locate the parameter record on the method */
	item = GetParamTarget(info, method, paramNum);
	if(!item)
	{
		/* Shouldn't happen, but do something sane anyway just in case */
		return;
	}

	/* Process the attributes for the parameter */
	return CSProcessAttrs(info, item, attributes, CS_ATTR_PARAMETER);
}

#ifdef	__cplusplus
};
#endif
