/*
 * lib_emit.c - Internalcall methods for the "Reflection.Emit" classes.
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
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
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

#ifdef IL_CONFIG_REFLECTION

/* TODO: all of these methods must be synchronized with the metadata
   lock to ensure that user-level apps cannot circumvent heap security */

/*
 * private static IntPtr ClrAssemblyCreate(String name, int v1, int v2,
 *										   int v3, int v4,
 *                                         AssemblyBuilderAccess access,
 *										   out IntPtr writer);
 */
ILNativeInt _IL_AssemblyBuilder_ClrAssemblyCreate
		(ILExecThread *_thread, ILString *name, ILInt32 v1,
		 ILInt32 v2, ILInt32 v3, ILInt32 v4, ILInt32 access,
		 ILNativeInt *writer)
{
	/* TODO */
	*writer = 0;
	return 0;
}

/*
 * private void ClrSetEntryPoint(IntPtr nativeMethod, PEFileKinds fileKind);
 */
void _IL_AssemblyBuilder_ClrSetEntryPoint(ILExecThread *thread,
										  ILObject *_this,
										  ILNativeInt clrMethod,
										  ILInt32 fileKind)
{
	/* TODO */
}

/*
 * internal static int ClrGetItemToken(IntPtr item);
 */
ILInt32 _IL_AssemblyBuilder_ClrGetItemToken(ILExecThread *_thread,
											ILNativeInt item)
{
	if(item)
	{
		return ILProgramItem_Token((ILProgramItem *)item);
	}
	else
	{
		return 0;
	}
}

/*
 * internal static IntPtr GetItemFromToken(IntPtr assembly, int token);
 */
ILNativeInt _IL_AssemblyBuilder_ClrGetItemFromToken(ILExecThread *_thread,
													ILNativeInt assembly,
													ILInt32 token)
{
	if(assembly)
	{
		return (ILNativeInt)ILProgramItem_FromToken
			(ILProgramItem_Image((ILAssembly *)assembly), (ILToken)token);
	}
	else
	{
		return 0;
	}
}

/*
 * private static IntPtr ClrEventCreate(IntPtr classInfo, String name,
 *									    IntPtr type, EventAttributes attrs);
 */
ILNativeInt _IL_EventBuilder_ClrEventCreate(ILExecThread *_thread,
											ILNativeInt classInfo,
											ILString *name, ILNativeInt type,
											ILInt32 attrs)
{
	/* TODO */
	return 0;
}

/*
 * private static ClrEventAddSemantics(IntPtr eventInfo,
 *									   MethodSemanticsAttributes attr,
 *									   MethodToken token);
 */
void _IL_EventBuilder_ClrEventAddSemantics(ILExecThread *_thread,
										   ILNativeInt eventInfo,
										   ILInt32 attr, void *token)
{
	/* TODO */
}

/*
 * private static IntPtr ClrFieldCreate(IntPtr classInfo, String name,
 *										IntPtr type, FieldAttributes attrs);
 */
ILNativeInt _IL_FieldBuilder_ClrFieldCreate(ILExecThread *_thread,
											ILNativeInt classInfo,
											ILString *name, ILNativeInt type,
											ILInt32 attrs)
{
	/* TODO */
	return 0;
}

/*
 * private static void ClrFieldSetConstant(IntPtr item, Object value);
 */
void _IL_FieldBuilder_ClrFieldSetConstant(ILExecThread *_thread,
										  ILNativeInt item,
										  ILObject *value)
{
	/* TODO */
}

/*
 * private static void ClrFieldSetMarshal(IntPtr item, byte[] data);
 */
void _IL_FieldBuilder_ClrFieldSetMarshal(ILExecThread *_thread,
										 ILNativeInt item,
										 System_Array *data)
{
	/* TODO */
}

/*
 * private static void ClrFieldSetOffset(IntPtr item, int offset);
 */
void _IL_FieldBuilder_ClrFieldSetOffset(ILExecThread *_thread,
										ILNativeInt item, ILInt32 offset)
{
	/* TODO */
}

/*
 * private static void ClrFieldSetRVA(IntPtr item, int rva);
 */
void _IL_FieldBuilder_ClrFieldSetRVA(ILExecThread *_thread,
									 ILNativeInt item, ILInt32 rva)
{
	/* TODO */
}

/*
 * private static IntPtr ClrModuleCreate(IntPtr assembly, String name);
 */
ILNativeInt _IL_ModuleBuilder_ClrModuleCreate(ILExecThread *_thread,
											  ILNativeInt assembly,
											  ILString *name)
{
	ILImage *image = ILProgramItem_Image(assembly);
	ILToken token = ILProgramItem_Token(assembly);
	const char *str = (const char *)ILStringToAnsi(_thread, name);
	return (ILNativeInt)ILModuleCreate(image, token, str, NULL);
}

/*
 * private static int ClrModuleCreateString(IntPtr module, String str);
 */
ILInt32 _IL_ModuleBuilder_ClrModuleCreateString(ILExecThread *_thread,
											    ILNativeInt module,
												ILString *str)
{
	/* TODO */
	return 0;
}

/*
 * internal static int ClrModuleWriteData(IntPtr module, byte[] data);
 */
ILInt32 _IL_ModuleBuilder_ClrModuleWriteData(ILExecThread *_thread,
											 ILNativeInt module,
											 System_Array *data)
{
	/* TODO */
	return 0;
}

/*
 * internal static int ClrModuleWriteGap(IntPtr module, int size);
 */
ILInt32 _IL_ModuleBuilder_ClrModuleWriteGap(ILExecThread *_thread,
											ILNativeInt module, ILInt32 size)
{
	/* TODO */
	return 0;
}

/*
 * private static IntPtr ClrPropertyCreate(IntPtr classInfo, String name,
 *										   PropertyAttributes attrs,
 *										   IntPtr signature);
 */
ILNativeInt _IL_PropertyBuilder_ClrPropertyCreate(ILExecThread *_thread,
												  ILNativeInt classInfo,
												  ILString *name,
												  ILInt32 attrs,
												  ILNativeInt signature)
{
	/* TODO */
	return 0;
}

/*
 * private static void ClrPropertyAddSemantics(IntPtr item,
 *											   MethodSemanticsAttributes attr,
 *											   MethodToken token);
 */
void _IL_PropertyBuilder_ClrPropertyAddSemantics(ILExecThread *_thread,
												 ILNativeInt item,
												 ILInt32 attr,
												 void *token)
{
	/* TODO */
}

/*
 * private static void ClrPropertySetConstant(IntPtr item, Object value);
 */
void _IL_PropertyBuilder_ClrPropertySetConstant(ILExecThread *_thread,
												ILNativeInt item,
												ILObject *value)
{
	/* TODO */
}

/*
 * private static IntPtr ClrTypeCreate(IntPtr module, String name,
 *									   String nspace, TypeAttributes attr,
 *									   TypeToken parent);
 */
ILNativeInt _IL_TypeBuilder_ClrTypeCreate(ILExecThread *_thread,
										  ILNativeInt module,
										  ILNativeInt nestedParent,
										  ILString *name,
										  ILString *nspace,
										  ILInt32 attr,
										  void *parent)
{
	/* TODO */
	return 0;
}

/*
 * private static void ClrTypeSetPackingSize(IntPtr classInfo, int packingSize);
 */
void _IL_TypeBuilder_ClrTypeSetPackingSize(ILExecThread *_thread,
										   ILNativeInt classInfo,
										   ILInt32 packingSize)
{
	/* TODO */
}

/*
 * private static void ClrTypeSetClassSize(IntPtr classInfo, int classSize);
 */
void _IL_TypeBuilder_ClrTypeSetClassSize(ILExecThread *_thread,
										 ILNativeInt classInfo,
										 ILInt32 classSize)
{
	/* TODO */
}

/*
 * private static void ClrTypeAddInterface(IntPtr classInfo, TypeToken iface);
 */
void _IL_TypeBuilder_ClrTypeAddInterface(ILExecThread *_thread,
										 ILNativeInt classInfo,
										 void *iface)
{
	/* TODO */
}

/*
 * private static int ClrTypeGetPackingSize(IntPtr classInfo);
 */
ILInt32 _IL_TypeBuilder_ClrTypeGetPackingSize(ILExecThread *_thread,
											  ILNativeInt classInfo)
{
	/* TODO */
	return 0;
}

/*
 * private static int ClrTypeGetClassSize(IntPtr classInfo);
 */
ILInt32 _IL_TypeBuilder_ClrTypeGetClassSize(ILExecThread *_thread,
											ILNativeInt classInfo)
{
	/* TODO */
	return 0;
}

/*
 * private static void ClrTypeSetParent(IntPtr classInfo, TypeToken parent);
 */
void _IL_TypeBuilder_ClrTypeSetParent(ILExecThread *_thread,
									  ILNativeInt classInfo,
									  void *parent)
{
	/* TODO */
}

/*
 * internal static int ClrTypeImport(IntPtr module, IntPtr classInfo);
 */
ILInt32 _IL_TypeBuilder_ClrTypeImport(ILExecThread *_thread,
									  ILNativeInt module,
									  ILNativeInt classInfo)
{
	/* TODO */
	return 0;
}

/*
 * internal static int ClrTypeImportMember(IntPtr module, IntPtr memberInfo);
 */
ILInt32 _IL_TypeBuilder_ClrTypeImportMember(ILExecThread *_thread,
											ILNativeInt module,
											ILNativeInt memberInfo)
{
	/* TODO */
	return 0;
}

/*
 * private static void ClrTypeAddOverride(IntPtr module, int bodyToken,
 *										  int declToken);
 */
void _IL_TypeBuilder_ClrTypeAddOverride(ILExecThread *_thread,
										ILNativeInt module,
										ILInt32 bodyToken,
										ILInt32 declToken)
{
	/* TODO */
}

/*
 * internal static IntPtr ClrMethodCreate(IntPtr classInfo, String name,
 *										  MethodAttributes attributes,
 *										  IntPtr signature);
 */
ILNativeInt _IL_MethodBuilder_ClrMethodCreate(ILExecThread *_thread,
											  ILNativeInt classInfo,
											  ILString *name,
											  ILInt32 attributes,
											  ILNativeInt signature)
{
	/* TODO */
	return 0;
}

/*
 * internal static void ClrMethodSetImplAttrs(IntPtr method,
 *											  MethodImplAttributes attributes);
 */
void _IL_MethodBuilder_ClrMethodSetImplAttrs(ILExecThread *_thread,
											 ILNativeInt item,
											 ILInt32 attributes)
{
	if(item)
	{
		ILMethodSetImplAttrs((ILMethod *)item, ~((unsigned long)0),
							 (unsigned long)(long)attributes);
	}
}

/*
 * internal static int ClrMethodCreateVarArgRef(IntPtr module,
 *												int methodToken,
 *												IntPtr signature);
 */
ILInt32 _IL_MethodBuilder_ClrMethodCreateVarArgRef(ILExecThread *_thread,
												   ILNativeInt module,
												   ILInt32 methodToken,
												   ILNativeInt signature)
{
	/* TODO */
	return 0;
}

/*
 * internal static void ClrMethodSetRVA(IntPtr method, int rva);
 */
void _IL_MethodBuilder_ClrMethodSetRVA(ILExecThread *_thread,
									   ILNativeInt method, ILInt32 rva)
{
	if(method)
	{
		ILMethodSetRVA((ILMethod *)method, (ILUInt32)rva);
	}
}

/*
 * internal static void ClrMethodAddPInvoke(IntPtr method, int pinvAttrs,
 *											String dllName, String entryName);
 */
void _IL_MethodBuilder_ClrMethodAddPInvoke(ILExecThread *_thread,
										   ILNativeInt method,
										   ILInt32 pinvAttrs,
										   ILString *dllName,
										   ILString *entryName)
{
	/* TODO */
}

/*
 * private static IntPtr ClrSigCreateMethod(IntPtr context, int callConv,
 *											IntPtr returnType);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreateMethod(ILExecThread *_thread,
												   ILNativeInt context,
												   ILInt32 callConv,
												   ILNativeInt returnType)
{
	/* TODO */
	return 0;
}

/*
 * private static IntPtr ClrSigCreateMethod(IntPtr context, IntPtr returnType);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreateProperty(ILExecThread *_thread,
													 ILNativeInt context,
													 ILNativeInt returnType)
{
	/* TODO */
	return 0;
}

/*
 * private static IntPtr ClrSigModuleToContext(IntPtr module);
 */
ILNativeInt _IL_SignatureHelper_ClrSigModuleToContext(ILExecThread *_thread,
													  ILNativeInt module)
{
	if(module)
	{
		return (ILNativeInt)ILImageToContext
					(ILProgramItem_Image((ILModule *)module));
	}
	else
	{
		return 0;
	}
}

/*
 * private static IntPtr ClrSigCreatePrimitive(IntPtrContext, Type type);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreatePrimitive(ILExecThread *_thread,
													  ILNativeInt context,
													  ILObject *type)
{
	/* TODO */
	return 0;
}

/*
 * private static IntPtr ClrSigCreateArray(IntPtr context, int rank,
 *										   IntPtr elemType);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreateArray(ILExecThread *_thread,
												  ILNativeInt context,
												  ILInt32 rank,
												  ILNativeInt elemType)
{
	/* TODO */
	return 0;
}

/*
 * private static IntPtr ClrSigCreatePointer(IntPtr context, IntPtr elemType);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreatePointer(ILExecThread *_thread,
													ILNativeInt context,
													ILNativeInt elemType)
{
	/* TODO */
	return 0;
}

/*
 * private static IntPtr ClrSigCreateByRef(IntPtr context, IntPtr elemType);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreateByRef(ILExecThread *_thread,
												  ILNativeInt context,
												  ILNativeInt elemType)
{
	/* TODO */
	return 0;
}

/*
 * private static IntPtr ClrSigCreateValueType(IntPtr module, int token);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreateValueType(ILExecThread *_thread,
													  ILNativeInt module,
													  ILInt32 token)
{
	if(module && token)
	{
		return (ILNativeInt)(ILType_FromValueType
					(ILClass_FromToken(ILProgramItem_Image(module),
									   (ILToken)(long)token)));
	}
	else
	{
		return (ILNativeInt)ILType_Invalid;
	}
}

/*
 * private static IntPtr ClrSigCreateClass(IntPtr module, int token);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreateClass(ILExecThread *_thread,
												  ILNativeInt module,
												  ILInt32 token)
{
	if(module && token)
	{
		return (ILNativeInt)(ILType_FromClass
					(ILClass_FromToken(ILProgramItem_Image(module),
									   (ILToken)(long)token)));
	}
	else
	{
		return (ILNativeInt)ILType_Invalid;
	}
}

/*
 * private static IntPtr ClrSigCreateField(IntPtr context);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreateField(ILExecThread *_thread,
												  ILNativeInt context)
{
	/* TODO */
	return 0;
}

/*
 * private static IntPtr ClrSigCreateLocal(IntPtr context);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreateLocal(ILExecThread *_thread,
												  ILNativeInt context)
{
	/* TODO */
	return 0;
}

/*
 * private static bool ClrSigAddArgument(IntPtr context, IntPtr sig,
 *										 IntPtr arg);
 */
ILBool _IL_SignatureHelper_ClrSigAddArgument(ILExecThread *_thread,
											 ILNativeInt context,
											 ILNativeInt sig,
											 ILNativeInt arg)
{
	/* TODO */
	return 1;
}

/*
 * private static bool ClrSigAddSentinel(IntPtr context, IntPtr sig);
 */
ILBool _IL_SignatureHelper_ClrSigAddSentinel(ILExecThread *_thread,
											 ILNativeInt context,
											 ILNativeInt sig)
{
	/* TODO */
	return 1;
}

/*
 * private static IntPtr ClrSigCreateMethodCopy(IntPtr context, IntPtr module,
 *												int methodToken);
 */
ILNativeInt _IL_SignatureHelper_ClrSigCreateMethodCopy(ILExecThread *_thread,
													   ILNativeInt context,
													   ILNativeInt module,
													   ILInt32 methodToken)
{
	/* TODO */
	return 0;
}

/*
 * private static bool ClrSigIdentical(IntPtr sig1, IntPtr sig2);
 */
ILBool _IL_SignatureHelper_ClrSigIdentical(ILExecThread *_thread,
										   ILNativeInt sig1,
										   ILNativeInt sig2)
{
	return (ILBool)(ILTypeIdentical((ILType *)sig1, (ILType *)sig2));
}

/*
 * private static int ClrSigGetHashCode(IntPtr sig);
 */
ILInt32 _IL_SignatureHelper_ClrSigGetHashCode(ILExecThread *_thread,
											  ILNativeInt sig)
{
	/* TODO */
	return 0;
}

System_Array *_IL_SignatureHelper_ClrSigGetBytes(ILExecThread *_thread,
												 ILNativeInt module,
												 ILNativeInt sig)
{
	/* TODO */
	return 0;
}

/*
 * internal static IntPtr ClrParameterCreate(IntPtr method, int position,
 *											 ParameterAttributes attributes,
 *											 String name);
 */
ILNativeInt _IL_ParameterBuilder_ClrParameterCreate(ILExecThread *_thread,
													ILNativeInt method,
													ILInt32 position,
													ILInt32 attributes,
													ILString *name)
{
	/* TODO */
	return 0;
}

/*
 * internal static int ClrParameterGetPosition(IntPtr parameter);
 */
ILInt32 _IL_ParameterBuilder_ClrParameterGetPosition(ILExecThread *_thread,
													 ILNativeInt parameter)
{
	if(parameter)
	{
		return ILParameter_Num((ILParameter *)parameter);
	}
	else
	{
		return 0;
	}
}

/*
 * internal static ParameterAttributes ClrParameterGetPosition
 *						(IntPtr parameter);
 */
ILInt32 _IL_ParameterBuilder_ClrParameterGetAttrs(ILExecThread *_thread,
												  ILNativeInt parameter)
{
	if(parameter)
	{
		return ILParameter_Attrs((ILParameter *)parameter);
	}
	else
	{
		return 0;
	}
}

/*
 * internal static String ClrParameterGetName(IntPtr parameter);
 */
ILString *_IL_ParameterBuilder_ClrParameterGetName(ILExecThread *_thread,
												   ILNativeInt parameter)
{
	if(parameter)
	{
		return ILStringCreate
			(_thread, ILParameter_Name((ILParameter *)parameter));
	}
	else
	{
		return 0;
	}
}

#endif /* IL_CONFIG_REFLECTION */

#ifdef	__cplusplus
};
#endif
