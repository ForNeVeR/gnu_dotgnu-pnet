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
 * public static IntPtr GetReflectingType(IntPtr itemPrivate);
 */
static ILNativeInt ClrHelpers_GetReflectedType(ILExecThread *thread,
											   ILNativeInt itemPrivate)
{
	/* Not sure what the different between "declaring" and "reflected" is */
	return ClrHelpers_GetDeclaringType(thread, itemPrivate);
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
 * Method table for the "System.Reflection.ClrHelpers" class.
 */
IL_METHOD_BEGIN(_ILReflectionClrHelpersMethods)
	IL_METHOD("GetCustomAttributes",	"(jjZ)[oSystem.Object;",
					ClrHelpers_GetCustomAttributes)
	IL_METHOD("IsDefined",				"(jjZ)Z",
					ClrHelpers_IsDefined)
	IL_METHOD("GetDeclaringType",		"(j)j",
					ClrHelpers_GetDeclaringType)
	IL_METHOD("GetReflectedType",		"(j)j",
					ClrHelpers_GetReflectedType)
	IL_METHOD("GetName",				"(j)oSystem.String;",
					ClrHelpers_GetName)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
