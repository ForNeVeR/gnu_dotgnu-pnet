/*
 * cg_gen.c - Helper routines for "ILGenInfo".
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

#include "cg_nodes.h"
#include "cg_scope.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Create a basic image structure with an initial module,
 * assembly, and "<Module>" type.
 */
static ILImage *CreateBasicImage(ILContext *context, const char *assemName)
{
	ILImage *image;

	/* Create the image */
	if((image = ILImageCreate(context)) == 0)
	{
		return 0;
	}

	/* Create the module definition */
	if(!ILModuleCreate(image, 0, "<Module>", 0))
	{
		ILImageDestroy(image);
		return 0;
	}

	/* Create the assembly definition */
	if(!ILAssemblyCreate(image, 0, assemName, 0))
	{
		ILImageDestroy(image);
		return 0;
	}

	/* Create the module type */
	if(!ILClassCreate(ILClassGlobalScope(image), 0, "<Module>", 0, 0))
	{
		ILImageDestroy(image);
		return 0;
	}

	/* Done */
	return image;
}

void ILGenInfoInit(ILGenInfo *info, char *progname, FILE *asmOutput)
{
	info->progname = progname;
	info->asmOutput = asmOutput;
	if((info->context = ILContextCreate()) == 0)
	{
		ILGenOutOfMemory(info);
	}
	if((info->image = CreateBasicImage(info->context, "<Assembly>")) == 0)
	{
		ILGenOutOfMemory(info);
	}
	if((info->libImage = CreateBasicImage(info->context, "mscorlib")) == 0)
	{
		ILGenOutOfMemory(info);
	}
	ILMemPoolInitType(&(info->nodePool), ILNode, 0);
	ILScopeInit(info);
	info->nextLabel = 1;
	info->overflowInsns = 1;
	info->pedanticArith = 0;
	info->clsCompliant = 0;
	info->semAnalysis = 0;
	info->useJavaLib = 0;
	info->outputIsJava = 0;
	info->decimalRoundMode = IL_DECIMAL_ROUND_HALF_EVEN;
	info->stackHeight = 0;
	info->maxStackHeight = 0;
	info->loopStack = 0;
	info->loopStackSize = 0;
	info->loopStackMax = 0;
	info->returnType = IL_TYPE_VOID;
	info->returnVar = -1;
	info->returnLabel = ILLabel_Undefined;
	info->throwVariable = -1;
	info->gotoList = 0;
	info->scopeLevel = 0;
	info->tempVars = 0;
	info->numTempVars = 0;
	info->maxTempVars = 0;
	info->tempLocalBase = 0;
	info->currentScope = 0;
	info->javaInfo = 0;
	info->numLoops = 0;
	info->numSwitches = 0;
	info->unsafeLevel = 0;
	info->currentClass = 0;
	info->currentMethod = 0;
	ILGenMakeLibrary(info);
}

void ILGenInfoToJava(ILGenInfo *info)
{
	info->outputIsJava = 1;
	JavaGenInit(info);
}

void ILGenInfoDestroy(ILGenInfo *info)
{
	ILGotoEntry *gotoEntry, *nextGoto;

	/* Destroy the memory pools */
	ILMemPoolDestroy(&(info->nodePool));
	ILMemPoolDestroy(&(info->scopePool));
	ILMemPoolDestroy(&(info->scopeDataPool));

	/* Destroy the loop stack */
	if(info->loopStack)
	{
		ILFree(info->loopStack);
	}

	/* Destroy the goto list */
	gotoEntry = info->gotoList;
	while(gotoEntry != 0)
	{
		nextGoto = gotoEntry->next;
		ILFree(gotoEntry);
		gotoEntry = nextGoto;
	}

	/* Free the tempoary variable array */
	if(info->tempVars)
	{
		ILFree(info->tempVars);
	}

	/* Destroy Java-specific information */
	JavaGenDestroy(info);

	/* Destroy the image and context */
	ILImageDestroy(info->image);
	ILContextDestroy(info->context);
}

void ILGenOutOfMemory(ILGenInfo *info)
{
	fprintf(stderr, "%s: virtual memory exhausted\n", info->progname);
	exit(1);
}

ILType *ILFindSystemType(ILGenInfo *info, const char *name)
{
	ILClass *classInfo;
	ILProgramItem *scope;

	/* Look in the program itself first */
	scope = ILClassGlobalScope(info->image);
	if(scope)
	{
		classInfo = ILClassLookup(scope, name, "System");
		if(classInfo)
		{
			return ILType_FromClass(classInfo);
		}
	}

	/* Look in the library image */
	scope = ILClassGlobalScope(info->libImage);
	if(scope)
	{
		classInfo = ILClassLookup(scope, name, "System");
		classInfo = ILClassImport(info->image, classInfo);
		if(classInfo)
		{
			return ILType_FromClass(classInfo);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

ILType *ILClassToType(ILClass *info)
{
	const char *name;
	const char *namespace;

	/* Check for system classes with primitive equivalents */
	name = ILClass_Name(info);
	namespace = ILClass_Name(info);
	if(namespace && !strcmp(namespace, "System") &&
	   ILClass_NestedParent(info) == 0)
	{
		if(!strcmp(name, "Boolean"))
		{
			return ILType_Boolean;
		}
		else if(!strcmp(name, "SByte"))
		{
			return ILType_Int8;
		}
		else if(!strcmp(name, "Byte"))
		{
			return ILType_UInt8;
		}
		else if(!strcmp(name, "Int16"))
		{
			return ILType_Int16;
		}
		else if(!strcmp(name, "UInt16"))
		{
			return ILType_UInt16;
		}
		else if(!strcmp(name, "Char"))
		{
			return ILType_Char;
		}
		else if(!strcmp(name, "Int32"))
		{
			return ILType_Int32;
		}
		else if(!strcmp(name, "UInt32"))
		{
			return ILType_UInt32;
		}
		else if(!strcmp(name, "Int64"))
		{
			return ILType_Int64;
		}
		else if(!strcmp(name, "UInt64"))
		{
			return ILType_UInt64;
		}
		else if(!strcmp(name, "Single"))
		{
			return ILType_Float32;
		}
		else if(!strcmp(name, "Double"))
		{
			return ILType_Float64;
		}
		else if(!strcmp(name, "IntPtr"))
		{
			return ILType_Int;
		}
		else if(!strcmp(name, "UIntPtr"))
		{
			return ILType_UInt;
		}
		else if(!strcmp(name, "Void"))
		{
			return ILType_Void;
		}
		else if(!strcmp(name, "TypedReference"))
		{
			return ILType_TypedRef;
		}
	}

	/* Convert into either a value type or a class type */
	if(ILClass_IsValueType(info) ||
	   ILClass_IsUnmanagedValueType(info))
	{
		return ILType_FromValueType(info);
	}
	else
	{
		return ILType_FromClass(info);
	}
}

ILClass *ILTypeToClass(ILGenInfo *info, ILType *type)
{
	ILType *newType;
	if(ILType_IsPrimitive(type))
	{
		newType = 0;
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:
			{
				newType = ILFindSystemType(info, "Void");
			}
			break;

			case IL_META_ELEMTYPE_BOOLEAN:
			{
				newType = ILFindSystemType(info, "Boolean");
			}
			break;

			case IL_META_ELEMTYPE_I1:
			{
				newType = ILFindSystemType(info, "SByte");
			}
			break;

			case IL_META_ELEMTYPE_U1:
			{
				newType = ILFindSystemType(info, "Byte");
			}
			break;

			case IL_META_ELEMTYPE_I2:
			{
				newType = ILFindSystemType(info, "Int16");
			}
			break;

			case IL_META_ELEMTYPE_U2:
			{
				newType = ILFindSystemType(info, "UInt16");
			}
			break;

			case IL_META_ELEMTYPE_CHAR:
			{
				newType = ILFindSystemType(info, "Char");
			}
			break;

			case IL_META_ELEMTYPE_I4:
			{
				newType = ILFindSystemType(info, "Int32");
			}
			break;

			case IL_META_ELEMTYPE_U4:
			{
				newType = ILFindSystemType(info, "UInt32");
			}
			break;

			case IL_META_ELEMTYPE_I8:
			{
				newType = ILFindSystemType(info, "Int64");
			}
			break;

			case IL_META_ELEMTYPE_U8:
			{
				newType = ILFindSystemType(info, "UInt64");
			}
			break;

			case IL_META_ELEMTYPE_R4:
			{
				newType = ILFindSystemType(info, "Single");
			}
			break;

			case IL_META_ELEMTYPE_R8:
			{
				newType = ILFindSystemType(info, "Double");
			}
			break;

			case IL_META_ELEMTYPE_I:
			{
				newType = ILFindSystemType(info, "IntPtr");
			}
			break;

			case IL_META_ELEMTYPE_U:
			{
				newType = ILFindSystemType(info, "UIntPtr");
			}
			break;

			case IL_META_ELEMTYPE_TYPEDBYREF:
			{
				newType = ILFindSystemType(info, "TypedReference");
			}
			break;

			default: break;
		}
		if(newType)
		{
			return ILType_ToClass(newType);
		}
	}
	else if(ILType_IsValueType(type))
	{
		return ILType_ToValueType(type);
	}
	else if(ILType_IsClass(type))
	{
		return ILType_ToClass(type);
	}
	return 0;
}

ILMachineType ILTypeToMachineType(ILType *type)
{
	ILClass *classInfo;
	const char *namespace;

	if(ILType_IsPrimitive(type))
	{
		/* Convert a primitive type into a machine type */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_BOOLEAN:	return ILMachineType_Boolean;
			case IL_META_ELEMTYPE_I1:		return ILMachineType_Int8;
			case IL_META_ELEMTYPE_U1:		return ILMachineType_UInt8;
			case IL_META_ELEMTYPE_I2:		return ILMachineType_Int16;
			case IL_META_ELEMTYPE_U2:		return ILMachineType_UInt16;
			case IL_META_ELEMTYPE_CHAR:		return ILMachineType_Char;
			case IL_META_ELEMTYPE_I4:		return ILMachineType_Int32;
			case IL_META_ELEMTYPE_U4:		return ILMachineType_UInt32;
			case IL_META_ELEMTYPE_I8:		return ILMachineType_Int64;
			case IL_META_ELEMTYPE_U8:		return ILMachineType_UInt64;
			case IL_META_ELEMTYPE_I:		return ILMachineType_NativeInt;
			case IL_META_ELEMTYPE_U:		return ILMachineType_NativeUInt;
			case IL_META_ELEMTYPE_R4:		return ILMachineType_Float32;
			case IL_META_ELEMTYPE_R8:		return ILMachineType_Float64;
			case IL_META_ELEMTYPE_R:		return ILMachineType_NativeFloat;
			case IL_META_ELEMTYPE_TYPEDBYREF:
					return ILMachineType_ManagedValue;
			default: break;
		}
		return ILMachineType_Void;
	}
	else if(ILType_IsValueType(type))
	{
		/* Check for "System.Decimal", which has a special machine type */
		classInfo = ILType_ToClass(type);
		namespace = ILClass_Namespace(classInfo);
		if(namespace && !strcmp(namespace, "System") &&
		   !strcmp(ILClass_Name(classInfo), "Decimal") &&
		   ILClass_NestedParent(classInfo) == 0)
		{
			return ILMachineType_Decimal;
		}

		/* Everything else is a managed value */
		return ILMachineType_ManagedValue;
	}
	else if(ILType_IsClass(type))
	{
		/* Check for "System.String", which has a special machine type */
		classInfo = ILType_ToClass(type);
		namespace = ILClass_Namespace(classInfo);
		if(namespace && !strcmp(namespace, "System") &&
		   !strcmp(ILClass_Name(classInfo), "String") &&
		   ILClass_NestedParent(classInfo) == 0)
		{
			return ILMachineType_String;
		}

		/* Everything else is an object reference */
		return ILMachineType_ObjectRef;
	}
	else if(type == ILType_Invalid)
	{
		/* Invalid types are treated as "void" */
		return ILMachineType_Void;
	}
	else
	{
		/* Everything else is treated as a managed pointer for now */
		return ILMachineType_ManagedPtr;
	}
}

ILType *ILValueTypeToType(ILGenInfo *info, ILMachineType valueType)
{
	switch(valueType)
	{
		case ILMachineType_Boolean:		return ILType_Boolean;
		case ILMachineType_Int8:		return ILType_Int8;
		case ILMachineType_UInt8:		return ILType_UInt8;
		case ILMachineType_Int16:		return ILType_Int16;
		case ILMachineType_UInt16:		return ILType_UInt16;
		case ILMachineType_Char:		return ILType_Char;
		case ILMachineType_Int32:		return ILType_Int32;
		case ILMachineType_UInt32:		return ILType_UInt32;
		case ILMachineType_NativeInt:	return ILType_Int;
		case ILMachineType_NativeUInt:	return ILType_UInt;
		case ILMachineType_Int64:		return ILType_Int64;
		case ILMachineType_UInt64:		return ILType_UInt64;
		case ILMachineType_Float32:		return ILType_Float32;
		case ILMachineType_Float64:		return ILType_Float64;
		case ILMachineType_NativeFloat:	return ILType_Float;
		case ILMachineType_ObjectRef:	return ILFindSystemType(info, "Object");
		default:						break;
	}
	return ILType_Invalid;
}

unsigned ILGenTempVar(ILGenInfo *info, ILMachineType type)
{
	return ILGenTempTypedVar(info, ILValueTypeToType(info, type));
}

unsigned ILGenTempTypedVar(ILGenInfo *info, ILType *type)
{
	unsigned varNum;

	/* See if we can re-use a free temporary variable */
	for(varNum = 0; varNum < info->numTempVars; ++varNum)
	{
		if(!(info->tempVars[varNum].allocated) &&
		   ILTypeIdentical(info->tempVars[varNum].type, type))
		{
			info->tempVars[varNum].allocated = 1;
			return varNum + info->tempLocalBase;
		}
	}

	/* Abort if too many temporary variables */
	if((info->numTempVars + info->tempLocalBase) == (unsigned)0xFFFF)
	{
		fprintf(stderr, "%s: too many local variables - aborting\n",
				info->progname);
		exit(1);
	}

	/* Add a new temporary variable to the current method */
	if(info->numTempVars >= info->maxTempVars)
	{
		ILLocalVar *newvars;
		newvars = (ILLocalVar *)ILRealloc(info->tempVars,
										  sizeof(ILLocalVar) *
										  		(info->maxTempVars + 16));
		if(!newvars)
		{
			ILGenOutOfMemory(info);
		}
		info->tempVars = newvars;
		info->maxTempVars += 16;
	}
	info->tempVars[info->numTempVars].name = 0;
	info->tempVars[info->numTempVars].scopeLevel = -1;
	info->tempVars[info->numTempVars].type = type;
	info->tempVars[info->numTempVars].allocated = 1;

	/* Generate assembly code to allocate the local */
	ILGenAllocLocal(info, type, (const char *)0);

	/* Return the new variable index to the caller */
	return (info->tempLocalBase + (info->numTempVars)++);
}

void ILGenReleaseTempVar(ILGenInfo *info, unsigned localNum)
{
	if(localNum >= info->tempLocalBase &&
	   localNum < (info->tempLocalBase + info->numTempVars) &&
	   info->tempVars[localNum - info->tempLocalBase].scopeLevel == -1)
	{
		info->tempVars[localNum - info->tempLocalBase].allocated = 0;
	}
}

#ifdef	__cplusplus
};
#endif
