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
#include "il_serialize.h"

#ifdef	__cplusplus
extern	"C" {
#endif

#if 0

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

#endif

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
 * Determine if a class is "AttributeUsageAttribute".
 */
static int IsAttributeUsage(ILClass *classInfo)
{
	const char *namespace;
	if(strcmp(ILClass_Name(classInfo), "AttributeUsageAttribute") != 0)
	{
		return 0;
	}
	namespace = ILClass_Namespace(classInfo);
	if(!namespace || strcmp(namespace, "System") != 0)
	{
		return 0;
	}
	return (ILClass_NestedParent(classInfo) == 0);
}

/*
 * Modify an attribute name so that it ends in "Attribute".
 */
static void ModifyAttrName(ILNode_Identifier *ident)
{
	char *name = ident->name;
	int namelen = strlen(name);
	if(namelen < 9 || strcmp(name + namelen - 9, "Attribute") != 0)
	{
		ident->name = ILInternAppendedString
			(ILInternString(name, namelen),
			 ILInternString("Attribute", 9)).string;
	}
}

/*
 * Process a single attribute in a section.
 */
static void ProcessAttr(ILGenInfo *info, ILProgramItem *item,
						int target, ILNode_Attribute *attr)
{
	ILType *type;
	ILClass *classInfo;
	ILNode *argList;
	int numArgs, argNum;
	ILNode_ListIter iter;
	ILNode *arg;
	CSEvalArg *evalArgs;
	ILEvalValue *evalValues;
	CSSemValue value;
	int haveErrors;
	CSSemValue method;
	unsigned long itemNum;
	int candidateForm;
	ILProgramItem *itemInfo;
	ILMethod *methodInfo;
	ILType *signature;
	ILType *paramType;
	int allowMultiple;
	int inherited;
	ILSerializeWriter *writer = 0;
	ILEvalValue *argValue;
	int serialType;
	const void *blob;
	unsigned long blobLen;
	ILAttribute *attribute;

	/* Make sure that the attribute type name ends in "Attribute" */
	if(yyisa(attr->name, ILNode_Identifier))
	{
		ModifyAttrName((ILNode_Identifier *)(attr->name));
	}
	else if(yyisa(attr->name, ILNode_QualIdent))
	{
		ModifyAttrName
		  ((ILNode_Identifier *)(((ILNode_QualIdent *)(attr->name))->right));
	}

	/* Perform semantic analysis on the attribute type */
	type = CSSemType(attr->name, info, &(attr->name));

	/* The type must inherit from "System.Attribute" and not be abstract */
	if(!ILTypeAssignCompatible
			(info->image, type, ILFindSystemType(info, "Attribute")))
	{
		CCErrorOnLine(yygetfilename(attr), yygetlinenum(attr),
					  _("`%s' does not inherit from `System.Attribute'"),
					  CSTypeToName(type));
		return;
	}
	classInfo = ILTypeToClass(info, type);
	if(ILClass_IsAbstract(classInfo))
	{
		CCErrorOnLine(yygetfilename(attr), yygetlinenum(attr),
			  _("cannot use the abstract type `%s' as an attribute name"),
			  CSTypeToName(type));
		return;
	}

	/* Check that that the attribute can be applied to this kind of target.
	   We use a different algorithm for "AttributeUsageAttribute", to avoid
	   circularities in the semantic analysis network */
	if(!IsAttributeUsage(classInfo))
	{
		/* Perform semantic analysis on the attribute type */
		CSSemProgramItem(info, ILToProgramItem(classInfo));

		/* Get the usage information for the attribute */
		allowMultiple = 1;
		inherited = 1;
	}
	else
	{
		/* We can only use "AttributeUsageAttribute" on classes
		   that inherit from "System.Attribute" */
		classInfo = ILProgramItemToClass(item);
		if(!classInfo)
		{
			CCErrorOnLine(yygetfilename(attr), yygetlinenum(attr),
		     _("`System.AttributeUsageAttribute' may only be used on classes"));
			return;
		}
		if(!ILTypeAssignCompatible(info->image, ILClassToType(classInfo),
		   						   ILFindSystemType(info, "Attribute")))
		{
			CCErrorOnLine(yygetfilename(attr), yygetlinenum(attr),
						  _("`%s' does not inherit from `System.Attribute'"),
						  CSTypeToName(ILClassToType(classInfo)));
			return;
		}
		allowMultiple = 0;
		inherited = 1;
	}

	/* Check the "AllowMultiple" and "Inherited" states of the attribute */
	/* TODO */

	/* Perform semantic analysis on the positional attributes */
	argList = ((ILNode_AttrArgs *)(attr->args))->positionalArgs;
	numArgs = ILNode_List_Length(argList);
	haveErrors = 0;
	if(numArgs)
	{
		evalArgs = (CSEvalArg *)ILMalloc(sizeof(CSEvalArg) * numArgs);
		if(!evalArgs)
		{
			CCOutOfMemory();
		}
		evalValues = (ILEvalValue *)ILMalloc(sizeof(ILEvalValue) * numArgs);
		if(!evalValues)
		{
			CCOutOfMemory();
		}
		ILNode_ListIter_Init(&iter, argList);
		argNum = 0;
		while((arg = ILNode_ListIter_Next(&iter)) != 0)
		{
			/* Perform semantic analysis on the argument to get the type.
			   Because the argument is wrapped in "ToConst", we don't
			   have to worry about reporting errors here */
			if(!CSSemExpectValue(arg, info, iter.last, &value))
			{
				haveErrors = 1;
				evalArgs[argNum].type = ILType_Int32;
			}
			else
			{
				evalArgs[argNum].type = CSSemGetType(value);
			}
			evalArgs[argNum].node = *(iter.last);
			evalArgs[argNum].parent = iter.last;
			evalArgs[argNum].modifier = ILParamMod_empty;

			/* Evaluate the constant value of the argument */
			if(!haveErrors &&
			   !ILNode_EvalConst(*(iter.last), info, &(evalValues[argNum])))
			{
				haveErrors = 1;
			}

			/* Advance to the next argument */
			++argNum;
		}
	}
	else
	{
		evalArgs = 0;
		evalValues = 0;
	}

	/* Perform semantic analysis on the named arguments */
	/* TODO */

	/* Bail out if we had errors during analysis of the arguments */
	if(haveErrors)
	{
		goto cleanup;
	}

	/* Resolve the constructors in the attribute type */
	method = CSResolveConstructor(info, (ILNode *)attr, type);
	if(!CSSemIsMethodGroup(method))
	{
		CCErrorOnLine(yygetfilename(attr), yygetlinenum(attr),
					  "`%s' does not have an accessible constructor",
					  CSTypeToName(type));
		goto cleanup;
	}

	/* Find the set of candidate methods */
	itemNum = 0;
	while((itemInfo = CSGetGroupMember(CSSemGetGroup(method), itemNum)) != 0)
	{
		candidateForm = CSItemIsCandidate(info, itemInfo, evalArgs, numArgs);
		if(candidateForm)
		{
			CSSetGroupMemberForm(CSSemGetGroup(method), itemNum,
								 candidateForm);
			++itemNum;
		}
		else
		{
			CSSemModifyGroup
				(method, CSRemoveGroupMember(CSSemGetGroup(method), itemNum));
		}
	}

	/* If there are no candidates left, then bail out */
	itemNum = 0;
	itemInfo = CSGetGroupMember(CSSemGetGroup(method), itemNum);
	if(!itemInfo)
	{
		CSItemCandidateError((ILNode *)attr, 0, 1,
						     CSSemGetGroup(method), evalArgs, numArgs);
		goto cleanup;
	}

	/* There are two or more candidates, then try to find the best one */
	if(CSGetGroupMember(CSSemGetGroup(method), 1) != 0)
	{
		itemInfo = CSBestCandidate(info, CSSemGetGroup(method),
								   evalArgs, numArgs);
		if(!itemInfo)
		{
			CSItemCandidateError((ILNode *)attr, 0, 1,
							     CSSemGetGroup(method), evalArgs, numArgs);
			goto cleanup;
		}
	}

	/* Import the constructor method into this image */
	methodInfo = (ILMethod *)ILMemberImport
						(info->image, (ILMember *)itemInfo);
	if(!methodInfo)
	{
		CCOutOfMemory();
	}

	/* Coerce the positional arguments to their final types */
	signature = ILMethod_Signature(methodInfo);
	haveErrors = 0;
	for(argNum = 0; argNum < numArgs; ++argNum)
	{
		paramType = ILTypeGetParam(signature, argNum + 1);
		if(!ILGenCastConst(info, &(evalValues[argNum]),
						   evalValues[argNum].valueType,
						   ILTypeToMachineType(paramType)))
		{
			CCErrorOnLine(yygetfilename(evalArgs[argNum].node),
						  yygetlinenum(evalArgs[argNum].node),
						  _("could not coerce constant argument %d"),
						  argNum + 1);
			haveErrors = 1;
		}
		else if(ILSerializeGetType(paramType) == -1)
		{
			CCErrorOnLine(yygetfilename(evalArgs[argNum].node),
						  yygetlinenum(evalArgs[argNum].node),
						  _("attribute argument %d is not serializable"),
						  argNum + 1);
			haveErrors = 1;
		}
	}
	if(haveErrors)
	{
		goto cleanup;
	}

	/* Build the serialized attribute value */
	writer = ILSerializeWriterInit();
	if(!writer)
	{
		CCOutOfMemory();
	}
	for(argNum = 0; argNum < numArgs; ++argNum)
	{
		paramType = ILTypeGetParam(signature, argNum + 1);
		serialType = ILSerializeGetType(paramType);
		argValue = &(evalValues[argNum]);
		switch(serialType)
		{
			case IL_META_SERIALTYPE_BOOLEAN:
			case IL_META_SERIALTYPE_I1:
			case IL_META_SERIALTYPE_U1:
			case IL_META_SERIALTYPE_I2:
			case IL_META_SERIALTYPE_U2:
			case IL_META_SERIALTYPE_CHAR:
			case IL_META_SERIALTYPE_I4:
			case IL_META_SERIALTYPE_U4:
			{
				ILSerializeWriterSetInt32(writer, argValue->un.i4Value,
										  serialType);
			}
			break;

			case IL_META_SERIALTYPE_I8:
			case IL_META_SERIALTYPE_U8:
			{
				ILSerializeWriterSetInt64(writer, argValue->un.i8Value);
			}
			break;

			case IL_META_SERIALTYPE_R4:
			{
				ILSerializeWriterSetFloat32(writer, argValue->un.r4Value);
			}
			break;

			case IL_META_SERIALTYPE_R8:
			{
				ILSerializeWriterSetFloat64(writer, argValue->un.r8Value);
			}
			break;

			case IL_META_SERIALTYPE_STRING:
			{
				ILSerializeWriterSetString(writer, argValue->un.strValue.str,
										   argValue->un.strValue.len);
			}
			break;

			default:
			{
				/* TODO: types and arrays */
			}
			break;
		}
	}
	ILSerializeWriterSetNumExtra(writer, 0);
	/* TODO: serialize named arguments */
	blob = ILSerializeWriterGetBlob(writer, &blobLen);
	if(!blob)
	{
		CCOutOfMemory();
	}

	/* Add the attribute value to the program item */
	attribute = ILAttributeCreate(info->image, 0);
	if(!attribute)
	{
		CCOutOfMemory();
	}
	ILAttributeSetType(attribute, ILToProgramItem(methodInfo));
	if(!ILAttributeSetValue(attribute, blob, blobLen))
	{
		CCOutOfMemory();
	}
	ILProgramItemAddAttribute(item, attribute);

cleanup:
	if(evalArgs)
	{
		ILFree(evalArgs);
	}
	if(evalValues)
	{
		ILFree(evalValues);
	}
	if(writer)
	{
		ILSerializeWriterDestroy(writer);
	}

#if 0
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
				   ILTypeIsStringClass(CSSemGetType(value)))
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
#endif
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
