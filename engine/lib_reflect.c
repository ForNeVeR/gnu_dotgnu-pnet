/*
 * lib_reflect.c - Internalcall methods for the reflection classes.
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
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Determine if we have an attribute type match.
 */
static int AttrMatch(ILExecThread *thread, ILAttribute *attr, ILClass *type)
{
	ILMethod *method = ILProgramItemToMethod(ILAttributeTypeAsItem(attr));
	if(method && ILClassInheritsFrom(ILMethod_Owner(method), type))
	{
		return _ILClrCheckAccess(thread, 0, (ILMember *)method);
	}
	else
	{
		return 0;
	}
}

/*
 * Get the number of attributes that match particular conditions.
 */
static ILInt32 NumMatchingAttrs(ILExecThread *thread, ILProgramItem *item,
								ILClass *type, ILBool inherit)
{
	ILInt32 num = 0;
	ILClass *classInfo;
	ILAttribute *attr;
	if(inherit && (classInfo = ILProgramItemToClass(item)) != 0)
	{
		while(classInfo != 0)
		{
			attr = 0;
			while((attr = ILProgramItemNextAttribute
						((ILProgramItem *)classInfo, attr)) != 0)
			{
				if(!type || AttrMatch(thread, attr, type))
				{
					++num;
				}
			}
			classInfo = ILClassGetParent(classInfo);
		}
	}
	else
	{
		attr = 0;
		while((attr = ILProgramItemNextAttribute(item, attr)) != 0)
		{
			if(!type || AttrMatch(thread, attr, type))
			{
				++num;
			}
		}
	}
	return num;
}

/*
 * De-serialize a custom attribute and construct an object for it.
 */
static ILObject *DeserializeAttribute(ILExecThread *thread,
									  ILAttribute *attr)
{
	return 0;
}

/*
 * public static Object[] GetCustomAttributes(IntPtr itemPrivate,
 *											  IntPtr clrTypePrivate,
 *											  bool inherit);
 */
static System_Array *ClrHelpers_GetCustomAttributes
						(ILExecThread *thread, ILNativeInt itemPrivate,
						 ILNativeInt clrTypePrivate, ILBool inherit)
{
	ILProgramItem *item = (ILProgramItem *)itemPrivate;
	ILClass *type = ILProgramItemToClass((ILProgramItem *)clrTypePrivate);
	ILInt32 num;
	System_Array *array;
	ILObject **buffer;
	ILClass *classInfo;
	ILAttribute *attr;

	/* Check that we have reflection access to the item */
	if(item && _ILClrCheckItemAccess(thread, item))
	{
		num = NumMatchingAttrs(thread, item, type, inherit);
		array = (System_Array *)ILExecThreadNew(thread, "[oSystem.Object;",
												"(Ti)V", (ILVaInt)num);
		if(!array)
		{
			return 0;
		}
		buffer = (ILObject **)(ArrayToBuffer(array));
		if(inherit && (classInfo = ILProgramItemToClass(item)) != 0)
		{
			while(classInfo != 0)
			{
				attr = 0;
				while((attr = ILProgramItemNextAttribute
							((ILProgramItem *)classInfo, attr)) != 0)
				{
					if(!type || AttrMatch(thread, attr, type))
					{
						*buffer = DeserializeAttribute(thread, attr);
						if(!(*buffer))
						{
							return 0;
						}
						++buffer;
					}
				}
				classInfo = ILClassGetParent(classInfo);
			}
		}
		else
		{
			attr = 0;
			while((attr = ILProgramItemNextAttribute(item, attr)) != 0)
			{
				if(!type || AttrMatch(thread, attr, type))
				{
					*buffer = DeserializeAttribute(thread, attr);
					if(!(*buffer))
					{
						return 0;
					}
					++buffer;
				}
			}
		}
		return array;
	}

	/* Invalid item, or insufficient access: return a zero-element array */
	return (System_Array *)ILExecThreadNew
				(thread, "[oSystem.Object;", "(Ti)V", (ILVaInt)0);
}

/*
 * public static bool IsDefined(IntPtr itemPrivate, IntPtr clrTypePrivate,
 *							    bool inherit);
 */
static ILBool ClrHelpers_IsDefined
						(ILExecThread *thread, ILNativeInt itemPrivate,
						 ILNativeInt clrTypePrivate, ILBool inherit)
{
	ILProgramItem *item = (ILProgramItem *)itemPrivate;
	ILClass *type = ILProgramItemToClass((ILProgramItem *)clrTypePrivate);

	/* Check that we have reflection access to the item */
	if(item && _ILClrCheckItemAccess(thread, item))
	{
		return (NumMatchingAttrs(thread, item, type, inherit) != 0);
	}
	else
	{
		return 0;
	}
}

/*
 * public static IntPtr GetDeclaringType(IntPtr itemPrivate);
 */
static ILNativeInt ClrHelpers_GetDeclaringType(ILExecThread *thread,
											   ILNativeInt itemPrivate)
{
	ILMember *member = ILProgramItemToMember((ILProgramItem *)itemPrivate);
	if(member)
	{
		return (ILNativeInt)(void *)(ILMember_Owner(member));
	}
	else
	{
		return 0;
	}
}

/*
 * public static String GetName(IntPtr itemPrivate)
 */
static ILString *ClrHelpers_GetName(ILExecThread *thread,
									ILNativeInt itemPrivate)
{
	ILProgramItem *item = (ILProgramItem *)itemPrivate;
	ILMember *member;
	ILAssembly *assem;
	ILModule *module;
	ILClass *classInfo;
	ILFileDecl *decl;
	ILManifestRes *res;

	/* Is this a type member? */
	member = ILProgramItemToMember(item);
	if(member)
	{
		return ILStringCreate(thread, ILMember_Name(member));
	}

	/* Is this an assembly? */
	assem = ILProgramItemToAssembly(item);
	if(assem)
	{
		return ILStringCreate(thread, ILAssembly_Name(assem));
	}

	/* Is this a module? */
	module = ILProgramItemToModule(item);
	if(module)
	{
		return ILStringCreate(thread, ILModule_Name(module));
	}

	/* Is this a class?  (Normally "ClrType.GetClrName()" is used for this) */
	classInfo = ILProgramItemToClass(item);
	if(classInfo)
	{
		return ILStringCreate(thread, ILClass_Name(classInfo));
	}

	/* Is this a file declaration? */
	decl = ILProgramItemToFileDecl(item);
	if(decl)
	{
		return ILStringCreate(thread, ILFileDecl_Name(decl));
	}

	/* Is this a manifest resource? */
	res = ILProgramItemToManifestRes(item);
	if(res)
	{
		return ILStringCreate(thread, ILManifestRes_Name(res));
	}

	/* No idea how to get the name of this item */
	return 0;
}

/*
 * public static IntPtr GetParameter(IntPtr itemPrivate, int num);
 */
static ILNativeInt ClrHelpers_GetParameter(ILExecThread *thread,
										   ILNativeInt itemPrivate,
										   ILInt32 num)
{
	ILMethod *method = ILProgramItemToMethod((ILProgramItem *)itemPrivate);
	if(method)
	{
		ILParameter *param = 0;
		while((param = ILMethodNextParam(method, param)) != 0)
		{
			if(ILParameter_Num(param) == (ILUInt32)num)
			{
				return (ILNativeInt)param;
			}
		}
	}
	return 0;
}

/*
 * public static Type GetParameterType(IntPtr itemPrivate, int num);
 */
static ILObject *ClrHelpers_GetParameterType(ILExecThread *thread,
										     ILNativeInt itemPrivate,
										     ILInt32 num)
{
	ILMethod *method = ILProgramItemToMethod((ILProgramItem *)itemPrivate);
	if(method)
	{
		ILType *type = ILTypeGetParam(ILMethod_Signature(method),
									  (unsigned long)num);
		if(type != ILType_Invalid)
		{
			return _ILGetClrTypeForILType(thread, type);
		}
	}
	return 0;
}

/*
 * public static int GetNumParameters(IntPtr itemPrivate);
 */
static ILInt32 ClrHelpers_GetNumParameters(ILExecThread *thread,
										   ILNativeInt itemPrivate)
{
	ILMethod *method = ILProgramItemToMethod((ILProgramItem *)itemPrivate);
	if(method)
	{
		return (ILInt32)(ILMethod_Signature(method)->num);
	}
	return 0;
}

/*
 * public static int GetMemberAttrs(IntPtr itemPrivate);
 */
static ILInt32 ClrHelpers_GetMemberAttrs(ILExecThread *thread,
										 ILNativeInt itemPrivate)
{
	ILMember *member = ILProgramItemToMember((ILProgramItem *)itemPrivate);
	if(member)
	{
		return (ILInt32)(ILMember_Attrs(member));
	}
	return 0;
}

/*
 * public static CallingConventions GetCallConv(IntPtr itemPrivate);
 */
static ILInt32 ClrHelpers_GetCallConv(ILExecThread *thread,
								      ILNativeInt itemPrivate)
{
	ILMethod *method = ILProgramItemToMethod((ILProgramItem *)itemPrivate);
	if(method)
	{
		return (ILInt32)(ILMethod_CallConv(method));
	}
	return 0;
}

/*
 * public static MethodImplAttributes GetImplAttrs(IntPtr itemPrivate);
 */
static ILInt32 ClrHelpers_GetImplAttrs(ILExecThread *thread,
								       ILNativeInt itemPrivate)
{
	ILMethod *method = ILProgramItemToMethod((ILProgramItem *)itemPrivate);
	if(method)
	{
		return (ILInt32)(ILMethod_ImplAttrs(method));
	}
	return 0;
}

/*
 * public static MethodInfo GetSemantics(IntPtr itemPrivate,
 *										 MethodSemanticsAttributes type,
 *										 bool nonPublic);
 */
static ILObject *ClrHelpers_GetSemantics(ILExecThread *thread,
								         ILNativeInt itemPrivate,
										 ILInt32 type, ILBool nonPublic)
{
	ILMember *member = ILProgramItemToMember((ILProgramItem *)itemPrivate);
	ILMethod *method;
	if(member)
	{
		method = ILMethodSemGetByType(ILToProgramItem(member), type);
		if(!nonPublic && method && !ILMethod_IsPublic(method))
		{
			method = 0;
		}
		if(method && _ILClrCheckAccess(thread, 0, (ILMember *)method))
		{
			return _ILClrToObject(thread, (void *)method,
								  "System.Reflection.ClrMethod");
		}
	}
	return 0;
}

/*
 * public static bool HasSemantics(IntPtr itemPrivate,
 *								   MethodSemanticsAttributes type,
 *								   bool nonPublic);
 */
static ILBool ClrHelpers_HasSemantics(ILExecThread *thread,
								      ILNativeInt itemPrivate,
									  ILInt32 type, ILBool nonPublic)
{
	ILMember *member = ILProgramItemToMember((ILProgramItem *)itemPrivate);
	ILMethod *method;
	if(member)
	{
		method = ILMethodSemGetByType(ILToProgramItem(member), type);
		if(!nonPublic && method && !ILMethod_IsPublic(method))
		{
			method = 0;
		}
		if(method && _ILClrCheckAccess(thread, 0, (ILMember *)method))
		{
			return 1;
		}
	}
	return 0;
}

/*
 * public static bool CanAccess(IntPtr itemPrivate)
 */
static ILBool ClrHelpers_CanAccess(ILExecThread *thread,
								   ILNativeInt itemPrivate)
{
	ILProgramItem *item = (ILProgramItem *)itemPrivate;
	ILClass *classInfo = ILProgramItemToClass(item);
	ILMember *member = ILProgramItemToMember(item);
	if(classInfo)
	{
		return _ILClrCheckAccess(thread, classInfo, 0);
	}
	else if(member)
	{
		return _ILClrCheckAccess(thread, 0, member);
	}
	else
	{
		return 0;
	}
}

/*
 * Method table for the "System.Reflection.ClrHelpers" class.
 */
IL_METHOD_BEGIN(_ILReflectionClrHelpersMethods)
	IL_METHOD("GetCustomAttributes",	"(jjZ)[oSystem.Object;",
					ClrHelpers_GetCustomAttributes)
	IL_METHOD("IsDefined",				"(jjZ)Z",
					ClrHelpers_IsDefined)
	IL_METHOD("GetDeclaringType",		"(j)j",
					ClrHelpers_GetDeclaringType)
	IL_METHOD("GetName",				"(j)oSystem.String;",
					ClrHelpers_GetName)
	IL_METHOD("GetParameter",			"(ji)j",
					ClrHelpers_GetParameter)
	IL_METHOD("GetParameterType",		"(ji)oSystem.Type;",
					ClrHelpers_GetParameterType)
	IL_METHOD("GetNumParameters",		"(j)i",
					ClrHelpers_GetNumParameters)
	IL_METHOD("GetMemberAttrs",			"(j)i",
					ClrHelpers_GetMemberAttrs)
	IL_METHOD("GetCallConv",
					"(j)vSystem.Reflection.CallingConventions;",
					ClrHelpers_GetCallConv)
	IL_METHOD("GetImplAttrs",
					"(j)vSystem.Reflection.MemberImplAttributes;",
					ClrHelpers_GetImplAttrs)
	IL_METHOD("GetSemantics",
					"(jvSystem.Reflection.MemberSemanticsAttributes;Z)"
							"oSystem.Reflection.MethodInfo;",
					ClrHelpers_GetSemantics)
	IL_METHOD("HasSemantics",
					"(jvSystem.Reflection.MemberSemanticsAttributes;Z)Z",
					ClrHelpers_HasSemantics)
	IL_METHOD("CanAccess",				"(j)Z",
					ClrHelpers_CanAccess)
IL_METHOD_END

/*
 * Convert an image into an assembly object.
 */
static ILObject *ImageToAssembly(ILExecThread *thread, ILImage *image)
{
	void *item;
	item = ILImageTokenInfo(image, (IL_META_TOKEN_ASSEMBLY | 1));
	if(item)
	{
		return _ILClrToObject(thread, item, "System.Reflection.Assembly");
	}
	/* TODO: if the image does not have an assembly manifest,
	   then look for the parent assembly */
	return 0;
}

/*
 * public static Assembly GetCallingAssembly();
 */
static ILObject *Assembly_GetCallingAssembly(ILExecThread *thread)
{
	ILCallFrame *frame = _ILGetCallFrame(thread, 1);
	if(frame && frame->method)
	{
		return ImageToAssembly(thread, ILProgramItem_Image(frame->method));
	}
	else
	{
		return 0;
	}
}

/*
 * public static Assembly GetEntryAssembly();
 */
static ILObject *Assembly_GetEntryAssembly(ILExecThread *thread)
{
	ILImage *image = thread->process->entryImage;
	if(image)
	{
		return ImageToAssembly(thread, image);
	}
	else
	{
		return 0;
	}
}

/*
 * public static Assembly GetExecutingAssembly();
 */
static ILObject *Assembly_GetExecutingAssembly(ILExecThread *thread)
{
	ILCallFrame *frame = _ILGetCallFrame(thread, 0);
	if(frame && frame->method)
	{
		return ImageToAssembly(thread, ILProgramItem_Image(frame->method));
	}
	else
	{
		return 0;
	}
}

/*
 * Load errors.  These must be kept in sync with "pnetlib".
 */
#define	LoadError_OK			0
#define	LoadError_InvalidName	1
#define	LoadError_FileNotFound	2
#define	LoadError_BadImage		3
#define	LoadError_Security		4

/*
 * private static Assembly LoadFromName(String name, out int error);
 */
static ILObject *Assembly_LoadFromName(ILExecThread *thread,
									   ILString *name,
									   ILInt32 *error)
{
	/* TODO */
	*error = LoadError_FileNotFound;
	return 0;
}

/*
 * private static Assembly LoadFromFile(String name, out int error);
 */
static ILObject *Assembly_LoadFromFile(ILExecThread *thread,
									   ILString *name,
									   ILInt32 *error)
{
	char *filename;
	ILImage *image;
	int loadError;

	/* Convert the name into a NUL-terminated filename string */
	filename = ILStringToAnsi(thread, name);
	if(!filename)
	{
		*error = LoadError_FileNotFound;
		return 0;
	}

	/* TODO: validate the pathname */
	if(*filename == '\0')
	{
		*error = LoadError_InvalidName;
		return 0;
	}

	/* TODO: check security permissions */

	/* Attempt to load the file */
	loadError = ILImageLoadFromFile(filename, thread->process->context,
									&image, IL_LOADFLAG_FORCE_32BIT, 0);
	if(loadError == 0)
	{
		*error = LoadError_OK;
		return ImageToAssembly(thread, image);
	}

	/* Convert the error code into something the C# library knows about */
	if(loadError == -1)
	{
		*error = LoadError_FileNotFound;
	}
	else if(loadError == IL_LOADERR_MEMORY)
	{
		*error = LoadError_FileNotFound;
		ILExecThreadThrowOutOfMemory(thread);
	}
	else
	{
		*error = LoadError_BadImage;
	}
	return 0;
}

/*
 * private static Type GetType(String typeName, bool throwOnError,
 *							   bool ignoreCase)
 */
static ILObject *Assembly_GetType(ILExecThread *thread, ILObject *_this,
								  ILString *name, ILBool throwOnError,
								  ILBool ignoreCase)
{
	ILProgramItem *item = (ILProgramItem *)_ILClrFromObject(thread, _this);
	ILImage *image = ((item != 0) ? ILProgramItem_Image(item) : 0);
	if(!image)
	{
		if(throwOnError)
		{
			ILExecThreadThrowSystem(thread, "System.TypeLoadException",
									(const char *)0);
		}
		return 0;
	}
	return _ILGetTypeFromImage(thread, image, name, throwOnError, ignoreCase);
}

/*
 * Method table for the "System.Reflection.Assembly" class.
 */
IL_METHOD_BEGIN(_ILReflectionAssemblyMethods)
	IL_METHOD("GetCallingAssembly",		"()oSystem.Reflection.Assembly;",
					Assembly_GetCallingAssembly)
	IL_METHOD("GetEntryAssembly",		"()oSystem.Reflection.Assembly;",
					Assembly_GetEntryAssembly)
	IL_METHOD("GetExecutingAssembly",	"()oSystem.Reflection.Assembly;",
					Assembly_GetExecutingAssembly)
	IL_METHOD("LoadFromName",
					"(oSystem.String;&i)oSystem.Reflection.Assembly;",
					Assembly_LoadFromName)
	IL_METHOD("LoadFromFile",
					"(oSystem.String;&i)oSystem.Reflection.Assembly;",
					Assembly_LoadFromFile)
	IL_METHOD("GetType", "(ToSystem.String;ZZ)oSystem.Type;",
					Assembly_GetType)
IL_METHOD_END

/*
 * private static ParameterAttributes GetParamAttrs(IntPtr itemPrivate);
 */
static ILInt32 ClrParameter_GetParamAttrs(ILExecThread *thread,
									      ILNativeInt itemPrivate)
{
	ILParameter *param;
	param = ILProgramItemToParameter((ILProgramItem *)itemPrivate);
	if(param)
	{
		return ILParameter_Attrs(param);
	}
	else
	{
		return 0;
	}
}

/*
 * private static String GetParamName(IntPtr itemPrivate);
 */
static ILString *ClrParameter_GetParamName(ILExecThread *thread,
									       ILNativeInt itemPrivate)
{
	ILParameter *param;
	param = ILProgramItemToParameter((ILProgramItem *)itemPrivate);
	if(param)
	{
		return ILStringCreate(thread, ILParameter_Name(param));
	}
	else
	{
		return 0;
	}
}

/*
 * private static Object GetDefault(IntPtr itemPrivate);
 */
static ILObject *ClrParameter_GetDefault(ILExecThread *thread,
									     ILNativeInt itemPrivate)
{
	ILParameter *param;
	param = ILProgramItemToParameter((ILProgramItem *)itemPrivate);
	if(param)
	{
		/* TODO */
		return 0;
	}
	else
	{
		return 0;
	}
}

/*
 * Method table for the "System.Reflection.ClrParameter" class.
 */
IL_METHOD_BEGIN(_ILReflectionClrParameterMethods)
	IL_METHOD("GetParamAttrs",
					"(j)vSystem.Reflection.ParameterAttributes;",
					ClrParameter_GetParamAttrs)
	IL_METHOD("GetParamName",		"(j)oSystem.String;",
					ClrParameter_GetParamName)
	IL_METHOD("GetDefault",			"(j)oSystem.Object;",
					ClrParameter_GetDefault)
IL_METHOD_END

/*
 * private static Type GetPropertyType(IntPtr itemPrivate);
 */
static ILObject *ClrProperty_GetPropertyType(ILExecThread *thread,
									         ILNativeInt itemPrivate)
{
	ILProperty *property;
	property = ILProgramItemToProperty((ILProgramItem *)itemPrivate);
	if(property)
	{
		return _ILGetClrTypeForILType
			(thread, ILProperty_Signature(property)->un.method.retType);
	}
	else
	{
		return 0;
	}
}

/*
 * Method table for the "System.Reflection.ClrProperty" class.
 */
IL_METHOD_BEGIN(_ILReflectionClrPropertyMethods)
	IL_METHOD("GetPropertyType",	"(j)oSystem.Type;",
					ClrProperty_GetPropertyType)
IL_METHOD_END

/*
 * public static FieldInfo GetFieldFromHandle(RuntimeFieldHandle handle);
 */
static ILObject *FieldInfo_GetFieldFromHandle(ILExecThread *thread,
									          void *handle)
{
	ILField *field;
	field = ILProgramItemToField((ILProgramItem *)(*((void **)handle)));
	if(field)
	{
		return _ILClrToObject(thread, field, "System.Reflection.ClrField");
	}
	else
	{
		return 0;
	}
}

/*
 * Method table for the "System.Reflection.FieldInfo" class.
 */
IL_METHOD_BEGIN(_ILReflectionFieldInfoMethods)
	IL_METHOD("GetFieldFromHandle",
				"(vSystem.RuntimeFieldHandle;)oSystem.Reflection.FieldInfo;",
				FieldInfo_GetFieldFromHandle)
IL_METHOD_END

/*
 * public static MethodBase GetMethodFromHandle(RuntimeMethodHandle handle);
 */
static ILObject *MethodBase_GetMethodFromHandle(ILExecThread *thread,
									            void *handle)
{
	ILMethod *method;
	method = ILProgramItemToMethod((ILProgramItem *)(*((void **)handle)));
	if(method)
	{
		if(ILMethod_IsConstructor(method) ||
		   ILMethod_IsStaticConstructor(method))
		{
			return _ILClrToObject
				(thread, method, "System.Reflection.ClrConstructor");
		}
		else
		{
			return _ILClrToObject
				(thread, method, "System.Reflection.ClrMethod");
		}
	}
	else
	{
		return 0;
	}
}

/*
 * public static MethodBase CurrentMethod();
 */
static ILObject *MethodBase_CurrentMethod(ILExecThread *thread)
{
	ILCallFrame *frame = _ILGetCallFrame(thread, 0);
	ILMethod *method = (frame ? frame->method : 0);
	if(method)
	{
		if(ILMethod_IsConstructor(method) ||
		   ILMethod_IsStaticConstructor(method))
		{
			return _ILClrToObject
				(thread, method, "System.Reflection.ClrConstructor");
		}
		else
		{
			return _ILClrToObject
				(thread, method, "System.Reflection.ClrMethod");
		}
	}
	else
	{
		return 0;
	}
}

/*
 * Method table for the "System.Reflection.MethodBase" class.
 */
IL_METHOD_BEGIN(_ILReflectionMethodBaseMethods)
	IL_METHOD("GetMethodFromHandle",
				"(vSystem.RuntimeMethodHandle;)oSystem.Reflection.MethodBase;",
				MethodBase_GetMethodFromHandle)
	IL_METHOD("CurrentMethod",
				"()oSystem.Reflection.MethodBase;",
				MethodBase_CurrentMethod)
IL_METHOD_END

/*
 * Invoke a method via reflection.
 */
static ILObject *InvokeMethod(ILExecThread *thread, ILMethod *method,
							  ILType *signature, ILObject *_this,
							  System_Array *parameters, int isCtor)
{
	ILExecValue *args;
	ILExecValue result;
	ILInt32 numParams;
	ILInt32 paramNum;
	ILInt32 argNum;
	ILType *paramType;
	ILObject *paramObject;
	ILType *objectType;
	ILImage *image = ILProgramItem_Image(method);

	/* Check that the number of parameters is correct */
	numParams = (ILInt32)(signature->num);
	if(numParams == 0)
	{
		if(parameters && parameters->length != 0)
		{
			ILExecThreadThrowSystem(thread, "System.ArgumentException", 0);
			return 0;
		}
	}
	else
	{
		if(!parameters || parameters->length != numParams)
		{
			ILExecThreadThrowSystem(thread, "System.ArgumentException", 0);
			return 0;
		}
	}

	/* Allocate an argument array for the invocation */
	if(numParams != 0 || _this != 0)
	{
		args = (ILExecValue *)ILGCAlloc(sizeof(ILExecValue) *
									    (numParams + (_this ? 1 : 0)));
		if(!args)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
	}
	else
	{
		args = 0;
	}

	/* Copy the parameter values into the array, and check their types */
	if(_this != 0)
	{
		args[0].objValue = _this;
		argNum = 1;
	}
	else
	{
		argNum = 0;
	}
	for(paramNum = 0; paramNum < numParams; ++paramNum)
	{
		paramType = ILTypeGetParam(signature, paramNum + 1);
		paramObject = ((ILObject **)ArrayToBuffer(parameters))[paramNum];
		if(paramObject)
		{
			/* Non-null object supplied */
			objectType = ILClassToType(GetObjectClass(paramObject));
			if(!ILTypeAssignCompatible(image, objectType, paramType))
			{
				ILExecThreadThrowSystem(thread, "System.ArgumentException", 0);
				return 0;
			}

			/* Unbox the object into the argument structure */
			paramType = ILTypeGetEnumType(paramType);
			if(ILType_IsPrimitive(paramType))
			{
				/* Unbox primitive and enumerated types into the argument */
				if(!ILExecThreadUnbox(thread, objectType, paramObject,
									  &(args[argNum])))
				{
					ILExecThreadThrowSystem
						(thread, "System.ArgumentException", 0);
					return 0;
				}
			}
			else if(ILType_IsValueType(paramType))
			{
				/* Pass non-enumerated value types as a pointer
				   into the boxed object.  The "CallMethodV"
				   function will copy the data onto the stack */
				args[argNum].ptrValue = (void *)paramObject;
			}
			else if(ILType_IsClass(paramType))
			{
				/* Pass class types by value */
				args[argNum].objValue = paramObject;
			}
			else if(paramType != 0 && ILType_IsComplex(paramType))
			{
				if(paramType->kind == IL_TYPE_COMPLEX_ARRAY ||
				   paramType->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
				{
					/* Array object references are passed directly */
					args[argNum].objValue = paramObject;
				}
				else
				{
					/* Don't know how to pass this kind of value yet */
					ILExecThreadThrowSystem
						(thread, "System.ArgumentException", 0);
					return 0;
				}
			}
			else
			{
				/* Don't know what this is, so raise an error */
				ILExecThreadThrowSystem(thread, "System.ArgumentException", 0);
				return 0;
			}
		}
		else
		{
			/* Null object supplied: the parameter must be an object ref */
			if(!ILTypeAssignCompatible(image, 0, paramType))
			{
				ILExecThreadThrowSystem(thread, "System.ArgumentException", 0);
				return 0;
			}
			args[argNum].objValue = 0;
		}
		++argNum;
	}

	/* Allocate a boxing object for the result if it is a value type */
	ILMemZero(&result, sizeof(result));
	paramType = ILTypeGetEnumType(signature->un.method.retType);
	if(ILType_IsValueType(paramType))
	{
		paramObject = (ILObject *)_ILEngineAllocObject
			(thread, ILType_ToValueType(paramType));
		if(!paramObject)
		{
			return 0;
		}
		result.ptrValue = (void *)paramObject;
	}
	else
	{
		paramObject = 0;
	}

	/* Invoke the method */
	if(isCtor)
	{
		return ILExecThreadCallCtorV(thread, method, args);
	}
	else if(ILExecThreadCallV(thread, method, &result, args))
	{
		/* An exception was thrown by the method */
		return 0;
	}

	/* Box the return value and exit */
	if(paramObject)
	{
		/* The value is already boxed */
		return paramObject;
	}
	else if(ILType_IsPrimitive(paramType))
	{
		/* Box a primitive value */
		if(paramType == ILType_Void)
		{
			return 0;
		}
		return ILExecThreadBox(thread, signature->un.method.retType, &result);
	}
	else if(ILType_IsClass(paramType))
	{
		/* Return object values directly */
		return result.objValue;
	}
	else if(paramType != 0 && ILType_IsComplex(paramType) &&
	        (paramType->kind == IL_TYPE_COMPLEX_ARRAY ||
			 paramType->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE))
	{
		/* Return array values directly */
		return result.objValue;
	}
	else
	{
		/* Don't know how to box this type of value */
		return 0;
	}
}

/*
 * public override Object Invoke(BindingFlags invokeAttr, Binder binder,
 *								 Object[] parameters, CultureInfo culture);
 */
static ILObject *ClrConstructor_Invoke(ILExecThread *thread,
									   ILObject *_this,
									   ILInt32 invokeAttr,
									   ILObject *binder,
									   System_Array *parameters,
									   ILObject *culture)
{
	ILMethod *method;
	ILType *signature;

	/* Extract the method item from the "this" object */
	method = ILProgramItemToMethod(_ILClrFromObject(thread, _this));
	if(!method)
	{
		/* Something is wrong with the object */
		ILExecThreadThrowSystem(thread, "System.MissingMethodException", 0);
		return 0;
	}

	/* Check that we have sufficient access credentials for the method */
	if(!_ILClrCheckAccess(thread, 0, (ILMember *)method))
	{
		ILExecThreadThrowSystem
			(thread, "System.Security.SecurityException", 0);
		return 0;
	}

	/* We cannot use this on static constructors */
	if(ILMethod_IsStaticConstructor(method))
	{
		ILExecThreadThrowSystem(thread, "System.MemberAccessException", 0);
		return 0;
	}

	/* Get the constructor's signature */
	signature = ILMethod_Signature(method);

	/* Invoke the constructor method */
	return InvokeMethod(thread, method, signature, 0, parameters, 1);
}

/*
 * Method table for the "System.Reflection.ClrConstructor" class.
 */
IL_METHOD_BEGIN(_ILReflectionClrConstructorMethods)
	IL_METHOD("Invoke",
				"(TvSystem.Reflection.BindingFlags;oSystem.Reflection.Binder;"
					"[oSystem.Object;oSystem.Globalization.CultureInfo;)"
						"oSystem.Object;",
				ClrConstructor_Invoke)
IL_METHOD_END

/*
 * Throw a target exception.
 */
static void ThrowTargetException(ILExecThread *thread)
{
	ILExecThreadThrowSystem(thread, "System.Reflection.TargetException", 0);
}

/*
 * public override Object Invoke(Object obj, BindingFlags invokeAttr,
 *								 Binder binder, Object[] parameters,
 *								 CultureInfo culture);
 */
static ILObject *ClrMethod_Invoke(ILExecThread *thread,
							      ILObject *_this,
								  ILObject *obj,
							      ILInt32 invokeAttr,
							      ILObject *binder,
							      System_Array *parameters,
							      ILObject *culture)
{
	ILMethod *method;
	ILType *signature;
	ILClass *classInfo;
	ILClass *targetClass;

	/* Extract the method item from the "this" object */
	method = ILProgramItemToMethod(_ILClrFromObject(thread, _this));
	if(!method)
	{
		/* Something is wrong with the object */
		ILExecThreadThrowSystem(thread, "System.MissingMethodException", 0);
		return 0;
	}

	/* Check that we have sufficient access credentials for the method */
	if(!_ILClrCheckAccess(thread, 0, (ILMember *)method))
	{
		ILExecThreadThrowSystem
			(thread, "System.Security.SecurityException", 0);
		return 0;
	}

	/* Resolve the method relative to the target */
	signature = ILMethod_Signature(method);
	classInfo = ILMethod_Owner(method);
	if(ILMethod_IsVirtual(method))
	{
		/* We must have a target object */
		if(!obj)
		{
			ThrowTargetException(thread);
			return 0;
		}

		/* Resolve interface and virtual references to the actual method */
		targetClass = GetObjectClass(obj);
		if(ILClass_IsInterface(classInfo))
		{
			if(!ILClassImplements(targetClass, classInfo))
			{
				ThrowTargetException(thread);
				return 0;
			}
			method = _ILLookupInterfaceMethod(targetClass, classInfo,
											  method->index);
		}
		else
		{
			if(!ILClassInheritsFrom(targetClass, classInfo))
			{
				ThrowTargetException(thread);
				return 0;
			}
			method = ((ILClassPrivate *)(targetClass->userData))->
							vtable[method->index];
		}

		/* If we don't have a resolved method, then we cannot invoke */
		if(!method)
		{
			ILExecThreadThrowSystem(thread, "System.MissingMethodException", 0);
			return 0;
		}
	}
	else if(ILType_HasThis(signature))
	{
		/* We must have a target object */
		if(!obj)
		{
			ThrowTargetException(thread);
			return 0;
		}

		/* The target class must inherit from the owner class */
		targetClass = GetObjectClass(obj);
		if(ILClass_IsInterface(classInfo))
		{
			/* This will only happen if an instance method is defined
			   in an interface.  Since C# disallows this, it will be rare.
			   However, CLI does not disallow it, so we must handle it */
			if(!ILClassImplements(targetClass, classInfo))
			{
				ThrowTargetException(thread);
				return 0;
			}
		}
		else
		{
			if(!ILClassInheritsFrom(targetClass, classInfo))
			{
				ThrowTargetException(thread);
				return 0;
			}
		}
	}
	else
	{
		/* Static method: we must not have a target object */
		if(obj)
		{
			ThrowTargetException(thread);
			return 0;
		}
	}

	/* Invoke the method */
	return InvokeMethod(thread, method, signature, obj, parameters, 0);
}

/*
 * Method table for the "System.Reflection.ClrMethod" class.
 */
IL_METHOD_BEGIN(_ILReflectionClrMethodMethods)
	IL_METHOD("Invoke",
				"(ToSystem.Object;vSystem.Reflection.BindingFlags;"
					"oSystem.Reflection.Binder;[oSystem.Object;"
					"oSystem.Globalization.CultureInfo;)oSystem.Object;",
				ClrMethod_Invoke)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
