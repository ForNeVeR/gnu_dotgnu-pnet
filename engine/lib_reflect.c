/*
 * lib_reflect.c - Internalcall methods for the reflection classes.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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
System_Array *_IL_ClrHelpers_GetCustomAttributes
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
ILBool _IL_ClrHelpers_IsDefined
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
ILNativeInt _IL_ClrHelpers_GetDeclaringType(ILExecThread *thread,
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
ILString *_IL_ClrHelpers_GetName(ILExecThread *thread,
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
ILNativeInt _IL_ClrHelpers_GetParameter(ILExecThread *thread,
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
ILObject *_IL_ClrHelpers_GetParameterType(ILExecThread *thread,
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
ILInt32 _IL_ClrHelpers_GetNumParameters(ILExecThread *thread,
										ILNativeInt itemPrivate)
{
	ILMethod *method = ILProgramItemToMethod((ILProgramItem *)itemPrivate);
	if(method)
	{
		return (ILInt32)(ILTypeNumParams(ILMethod_Signature(method)));
	}
	return 0;
}

/*
 * public static int GetMemberAttrs(IntPtr itemPrivate);
 */
ILInt32 _IL_ClrHelpers_GetMemberAttrs(ILExecThread *thread,
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
ILInt32 _IL_ClrHelpers_GetCallConv(ILExecThread *thread,
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
ILInt32 _IL_ClrHelpers_GetImplAttrs(ILExecThread *thread,
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
ILObject *_IL_ClrHelpers_GetSemantics(ILExecThread *thread,
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
ILBool _IL_ClrHelpers_HasSemantics(ILExecThread *thread,
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
ILBool _IL_ClrHelpers_CanAccess(ILExecThread *thread,
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
ILObject *_IL_Assembly_GetCallingAssembly(ILExecThread *thread)
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
ILObject *_IL_Assembly_GetEntryAssembly(ILExecThread *thread)
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
ILObject *_IL_Assembly_GetExecutingAssembly(ILExecThread *thread)
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
 * public virtual Type[] GetExportedTypes();
 */
System_Array *_IL_Assembly_GetExportedTypes(ILExecThread *_thread,
											ILObject *_this)
{
	/* TODO */
	return 0;
}

/*
 * Construct a "FileStream" object for a particular
 * filename in the same directory as an assembly.
 */
static ILObject *CreateFileStream(ILExecThread *thread, const char *assemFile,
								  const char *filename, int throw)
{
	int len;
	char *newPath;
	ILString *name;

	/* Bail out if the parameters are incorrect in some way */
	if(!assemFile || !filename)
	{
		if(throw)
		{
			ILExecThreadThrowSystem
				(thread, "System.IO.FileNotFoundException", 0);
		}
		return 0;
	}

	/* Strip path information from "filename" */
	len = strlen(filename);
	while(len > 0 && filename[len - 1] != '/' && filename[len - 1] != '\\' &&
	      filename[len - 1] != ':')
	{
		--len;
	}
	filename += len;

	/* Create a new pathname for the file in the assembly's directory */
	len = strlen(assemFile);
	while(len > 0 && assemFile[len - 1] != '/' && assemFile[len - 1] != '\\')
	{
		--len;
	}
	if(!len)
	{
		if(throw)
		{
			ILExecThreadThrowSystem
				(thread, "System.IO.FileNotFoundException", 0);
		}
		return 0;
	}
	--len;
	newPath = (char *)ILMalloc(len + strlen(filename) + 2);
	if(!newPath)
	{
		ILExecThreadThrowOutOfMemory(thread);
		return 0;
	}
	ILMemCpy(newPath, assemFile, len);
	newPath[len++] = '/';
	strcpy(newPath + len, filename);

	/* Bail out if the file does not exist */
	if(!ILFileExists(newPath, (char **)0))
	{
		if(throw)
		{
			ILExecThreadThrowSystem
				(thread, "System.IO.FileNotFoundException", 0);
		}
		return 0;
	}

	/* Convert the pathname into a string object */
	name = ILStringCreate(thread, newPath);
	ILFree(newPath);
	if(!name)
	{
		return 0;
	}

	/* Create the FileStream object */
	return ILExecThreadNew(thread, "System.IO.FileStream",
		   "(ToSystem.String;vSystem.IO.FileMode;vSystem.IO.FileAccess;)V",
		   name, (ILVaInt)3 /* Open */, (ILVaInt)1 /* Read */);
}

/*
 * public virtual FileStream GetFile(String name);
 *
 * Note: the semantics of this implementation are different than
 * the .NET Framework SDK.  This can provide access to any file
 * in the same directory as the assembly, as long as the caller
 * has the appropriate security level.
 *
 * The modified semantics make this method very useful for locating
 * configuration tables in the same directory as the assembly.  These
 * tables may have been modified since the assembly was compiled.
 * The usual .NET Framework SDK semantics cannot support files that
 * are modified after linking.
 *
 * These altered semantics are still ECMA-compatible from a certain
 * point of view because ECMA doesn't specify the behaviour of
 * "GetFile" at all.
 */
ILObject *_IL_Assembly_GetFile(ILExecThread *thread, ILObject *_this,
							   ILString *name)
{
	ILProgramItem *item = (ILProgramItem *)_ILClrFromObject(thread, _this);
	ILImage *image = ((item != 0) ? ILProgramItem_Image(item) : 0);
	char *str = ILStringToUTF8(thread, name);
	if(image && str)
	{
		/* Check the caller's access permissions */
		if(!ILImageIsSecure(_ILClrCallerImage(thread)))
		{
			ILExecThreadThrowSystem
				(thread, "System.IO.FileNotFoundException", 0);
			return 0;
		}

		/* Attempt to load the file from the same directory as the assembly */
		return CreateFileStream(thread, image->filename, str, 1);
	}
	else
	{
		return 0;
	}
}

/*
 * public virtual FileStream[] GetFiles(bool getResourceModules);
 */
System_Array *_IL_Assembly_GetFiles(ILExecThread *thread,
									ILObject *_this,
									ILBool getResourceModules)
{
	/* We don't support manifest files yet */
	return 0;
}

/*
 * public virtual ManifestResourceInfo GetManifestResourceInfo
 *				(String resourceName);
 */
ILObject *_IL_Assembly_GetManifestResourceInfo(ILExecThread *thread,
											   ILObject *_this,
											   ILString *resourceName)
{
	/* TODO */
	return 0;
}

/*
 * public virtual String[] GetManifestResourceNames();
 */
System_Array *_IL_Assembly_GetManifestResourceNames(ILExecThread *thread,
													ILObject *_this)
{
	/* TODO */
	return 0;
}

/*
 * Construct a "ClrResourceStream" object for a particular
 * manifest resource.  Returns NULL if "posn" is invalid
 * or the system is out of memory.
 */
static ILObject *CreateResourceStream(ILExecThread *thread, ILImage *image,
									  ILUInt32 posn)
{
	unsigned char *section;
	unsigned long sectionLen;
	unsigned long start;
	ILUInt32 length;

	/* Find the resource section within the image */
	if(!ILImageGetSection(image, IL_SECTION_RESOURCES,
						  (void **)&section, &sectionLen))
	{
		return 0;
	}

	/* Scan through the section until we find the resource we want */
	start = 0;
	while(posn > 0)
	{
		if(sectionLen < 4)
		{
			return 0;
		}
		length = IL_READ_UINT32(section);
		if(((unsigned long)length) > (sectionLen - 4))
		{
			return 0;
		}
		start += length + 4;
		section += length + 4;
		sectionLen -= length + 4;
		--posn;
	}

	/* Extract the starting point and length of this resource */
	start += 4;
	if(sectionLen < 4)
	{
		return 0;
	}
	length = IL_READ_UINT32(section);
	if(((unsigned long)length) > (sectionLen - 4))
	{
		return 0;
	}

	/* Create the "ClrResourceStream" object and return it */
	return ILExecThreadNew(thread, "System.Reflection.ClrResourceStream",
						   "(Tjll)V", (ILNativeInt)image,
						   (ILInt64)start, (ILInt64)length);
}

/*
 * public virtual Stream GetManifestResourceStream(String name);
 */
ILObject *_IL_Assembly_GetManifestResourceStream(ILExecThread *thread,
												 ILObject *_this,
												 ILString *name)
{
	ILProgramItem *item = (ILProgramItem *)_ILClrFromObject(thread, _this);
	ILImage *image = ((item != 0) ? ILProgramItem_Image(item) : 0);
	char *str = ILStringToUTF8(thread, name);
	if(image && str)
	{
		/* Look for the manifest resource within the image */
		ILManifestRes *res = 0;
		ILFileDecl *file;
		ILUInt32 posn = 0;
		while((res = (ILManifestRes *)ILImageNextToken
				(image, IL_META_TOKEN_MANIFEST_RESOURCE, (void *)res)) != 0)
		{
			if(!strcmp(ILManifestRes_Name(res), str))
			{
				/* TODO: handle resources in external assemblies */
				if(ILManifestRes_OwnerAssembly(res))
				{
					continue;
				}

				/* If the resource is private, and the caller is not
				   the same image, then disallow the request */
				if(!ILManifestRes_IsPublic(res) &&
				   _ILClrCallerImage(thread) != image)
				{
					return 0;
				}

				/* Handle resources in external files */
				if((file = ILManifestRes_OwnerFile(res)) != 0)
				{
					/* If the calling image is not secure, then disallow.
					   Otherwise, applications may be able to force
					   the engine to read arbitrary files from the
					   same directory as a library assembly */
					if(!ILImageIsSecure(_ILClrCallerImage(thread)))
					{
						return 0;
					}

					/* The file must not have metadata and it must
					   start at offset 0 to be a valid file resource */
					if(ILFileDecl_HasMetaData(file) ||
					   ILManifestRes_Offset(res) != 0)
					{
						return 0;
					}

					/* Create a "FileStream" for the file resource */
					return CreateFileStream(thread, image->filename,
											ILFileDecl_Name(file), 0);
				}

				/* Find the manifest resource at "posn" within the image */
				return CreateResourceStream(thread, image, posn);
			}
			if(!ILManifestRes_OwnerFile(res) &&
			   !ILManifestRes_OwnerAssembly(res))
			{
				/* Increase the position within the resource section */
				++posn;
			}
		}
	}
	return 0;
}

/*
 * public virtual Type[] GetTypes();
 */
System_Array *_IL_Assembly_GetTypes(ILExecThread *_thread, ILObject *_this)
{
	ILProgramItem *item = (ILProgramItem *)_ILClrFromObject(_thread, _this);
	ILImage *image = ((item != 0) ? ILProgramItem_Image(item) : 0);
	System_Array *array=NULL; 
	ILObject **buffer=NULL;   
	ILUInt32 num=0; 
	ILClass *classInfo=NULL;

	if(item && _ILClrCheckItemAccess(_thread, item)) 
	{
		num = ILImageNumTokens (image, IL_META_TOKEN_TYPE_DEF);
		array = (System_Array *)ILExecThreadNew(_thread, "[oSystem.Object;",
												"(Ti)V", (ILVaInt)num);
		if(!array)
		{
			return 0;
		}
		buffer = (ILObject **)(ArrayToBuffer(array));
  		while ((classInfo = (ILClass *) ILImageNextToken 
							(image, IL_META_TOKEN_TYPE_DEF,classInfo)) != 0)
		{
			if (classInfo)
			{
				*buffer = _ILGetClrType(_thread,classInfo);
				if(!(*buffer)) //error getting type
				{
					return 0;
				}
				++buffer;
			}
		}
		return array;
	}
	/* Invalid item, or insufficient access: return a zero-element array */
	return (System_Array *)ILExecThreadNew
				(_thread, "[oSystem.Object;", "(Ti)V", (ILVaInt)0);
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
 * private static Assembly LoadFromName(String name, out int error,
 *										Assembly parent);
 */
ILObject *_IL_Assembly_LoadFromName(ILExecThread *thread,
									ILString *name,
									ILInt32 *error,
									ILObject *parent)
{
	ILProgramItem *item = (ILProgramItem *)_ILClrFromObject(thread, parent);
	ILImage *image = ((item != 0) ? ILProgramItem_Image(item) : 0);
	char *str = ILStringToUTF8(thread, name);
	if(image && str)
	{
		int loadError;
		ILImage *newImage;
		loadError = ILImageLoadAssembly(str, thread->process->context,
										image, &newImage);
		if(loadError == 0)
		{
			return ImageToAssembly(thread, newImage);
		}
		else if(loadError == -1)
		{
			*error = LoadError_FileNotFound;
			return 0;
		}
		else if(loadError == IL_LOADERR_MEMORY)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
		else
		{
			*error = LoadError_BadImage;
			return 0;
		}
	}
	else
	{
		*error = LoadError_FileNotFound;
		return 0;
	}
}

/*
 * private static Assembly LoadFromFile(String name, out int error,
 *										Assembly parent);
 */
ILObject *_IL_Assembly_LoadFromFile(ILExecThread *thread,
									ILString *name,
									ILInt32 *error,
									ILObject *parent)
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
ILObject *_IL_Assembly_GetType(ILExecThread *thread, ILObject *_this,
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
 * private RuntimeMethodHandle GetEntryPoint();
 */
void _IL_Assembly_GetEntryPoint(ILExecThread *thread,
							    void *result, ILObject *_this)
{
	ILProgramItem *item = (ILProgramItem *)_ILClrFromObject(thread, _this);
	ILImage *image = ((item != 0) ? ILProgramItem_Image(item) : 0);
	if(image)
	{
		ILToken token = ILImageGetEntryPoint(image);
		if(token != 0)
		{
			*((void **)result) = ILMethod_FromToken(image, token);
		}
		else
		{
			*((void **)result) = 0;
		}
	}
	else
	{
		*((void **)result) = 0;
	}
}

/*
 * private static ParameterAttributes GetParamAttrs(IntPtr itemPrivate);
 */
ILInt32 _IL_ClrParameter_GetParamAttrs(ILExecThread *thread,
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
ILString *_IL_ClrParameter_GetParamName(ILExecThread *thread,
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
ILObject *_IL_ClrParameter_GetDefault(ILExecThread *thread,
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
 * private static Type GetPropertyType(IntPtr itemPrivate);
 */
ILObject *_IL_ClrProperty_GetPropertyType(ILExecThread *thread,
									      ILNativeInt itemPrivate)
{
	ILProperty *property;
	property = ILProgramItemToProperty((ILProgramItem *)itemPrivate);
	if(property)
	{
		return _ILGetClrTypeForILType
			(thread, ILTypeGetReturn(ILProperty_Signature(property)));
	}
	else
	{
		return 0;
	}
}

/*
 * public static FieldInfo GetFieldFromHandle(RuntimeFieldHandle handle);
 */
ILObject *_IL_FieldInfo_GetFieldFromHandle(ILExecThread *thread,
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
 * public override Object GetValue(Object obj);
 */
ILObject *_IL_ClrField_GetValue(ILExecThread *thread, ILObject *_this,
								ILObject *obj)
{
	/* TODO */
	return 0;
}

/*
 * public override void SetValue(Object obj, Object value,
 *                               BindingFlags invokeAttr,
 *                               Binder binder, CultureInfo culture);
 */
void _IL_ClrField_SetValue(ILExecThread *thread, ILObject *_this,
						   ILObject *obj, ILObject *value,
						   ILInt32 invokeAttr, ILObject *binder,
						   ILObject *culture)
{
	/* TODO */
}

/*
 * private static Type GetFieldType(IntPtr item);
 */
ILObject *_IL_ClrField_GetFieldType(ILExecThread *thread, ILNativeInt item)
{
	if(item)
	{
		return _ILGetClrTypeForILType(thread, ILField_Type((ILField *)item));
	}
	else
	{
		return 0;
	}
}

/*
 * public override Object GetValueDirect(TypedReference obj);
 */
ILObject *_IL_ClrField_GetValueDirect(ILExecThread *thread,
									  ILObject *_this, ILTypedRef obj)
{
	/* TODO */
	return 0;
}

/*
 * public override void SetValueDirect(TypedReference obj, Object value);
 */
void _IL_ClrField_SetValueDirect(ILExecThread *thread, ILObject *_this,
								 ILTypedRef obj, ILObject *value)
{
	/* TODO */
}

/*
 * public static MethodBase GetMethodFromHandle(RuntimeMethodHandle handle);
 */
ILObject *_IL_MethodBase_GetMethodFromHandle(ILExecThread *thread,
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
 * public static MethodBase GetCurrentMethod();
 */
ILObject *_IL_MethodBase_GetCurrentMethod(ILExecThread *thread)
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
 * public IntPtr GetFunctionPointer();
 */
ILNativeInt _IL_RuntimeMethodHandle_GetFunctionPointer
				(ILExecThread *thread, void *_this)
{
	/* Function pointers don't make any sense for us, so always return NULL */
	return 0;
}

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
	numParams = (ILInt32)ILTypeNumParams(signature);
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
				if(ILType_Kind(paramType) == IL_TYPE_COMPLEX_ARRAY ||
				   ILType_Kind(paramType) == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
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
	paramType = ILTypeGetEnumType(ILTypeGetReturn(signature));
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
		return ILExecThreadBox(thread, ILTypeGetReturn(signature), &result);
	}
	else if(ILType_IsClass(paramType))
	{
		/* Return object values directly */
		return result.objValue;
	}
	else if(paramType != 0 && ILType_IsComplex(paramType) &&
	        (ILType_Kind(paramType) == IL_TYPE_COMPLEX_ARRAY ||
			 ILType_Kind(paramType) == IL_TYPE_COMPLEX_ARRAY_CONTINUE))
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
ILObject *_IL_ClrConstructor_Invoke(ILExecThread *thread,
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
ILObject *_IL_ClrMethod_Invoke(ILExecThread *thread,
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
 * public override MethodInfo GetBaseDefinition();
 */
ILObject *_IL_ClrMethod_GetBaseDefinition(ILExecThread *thread,
										  ILObject *_this)
{
	/* TODO */
	return 0;
}

/*
 * public virtual Type GetType(String name, bool throwOnError,
 *							   bool ignoreCase);
 */
ILObject *_IL_Module_GetType(ILExecThread *_thread, ILObject *_this,
							 ILString *name, ILBool throwOnError,
							 ILBool ignoreCase)
{
	/* TODO */
	return 0;
}

/*
 * public virtual Type[] GetTypes();
 */
System_Array *_IL_Module_GetTypes(ILExecThread *_thread, ILObject *_this)
{
	/* TODO */
	return 0;
}

/*
 * public bool IsResource();
 */
ILBool _IL_Module_IsResource(ILExecThread *_thread, ILObject *_this)
{
	/* TODO */
	return 0;
}

/*
 * internal virtual Type GetModuleType();
 */
ILObject *_IL_Module_GetModuleType(ILExecThread *_thread, ILObject *_this)
{
	/* TODO */
	return 0;
}

/*
 * private Assembly GetAssembly();
 */
ILObject *_IL_Module_GetAssembly(ILExecThread *_thread, ILObject *_this)
{
	/* TODO */
	return 0;
}

/*
 * private String GetFullName();
 */
ILString *_IL_Module_GetFullName(ILExecThread *_thread, ILObject *_this)
{
	/* TODO */
	return 0;
}

/*
 * private static int ResourceRead(IntPtr handle, long position,
 *								   byte[] buffer, int offset, int count);
 */
ILInt32 _IL_ClrResourceStream_ResourceRead(ILExecThread *_thread,
										   ILNativeInt handle,
										   ILInt64 position,
										   System_Array *buffer,
										   ILInt32 offset,
										   ILInt32 count)
{
	ILImage *image = (ILImage *)handle;
	unsigned char *section;
	unsigned long sectionLen;
	if(image && ILImageGetSection(image, IL_SECTION_RESOURCES,
								  (void **)&section, &sectionLen))
	{
		ILMemCpy(((unsigned char *)(ArrayToBuffer(buffer))) + offset,
				 section + (ILNativeInt)position, count);
		return count;
	}
	else
	{
		return 0;
	}
}

/*
 * private static int ResourceReadByte(IntPtr handle, long position);
 */
ILInt32 _IL_ClrResourceStream_ResourceReadByte(ILExecThread *_thread,
											   ILNativeInt handle,
											   ILInt64 position)
{
	ILImage *image = (ILImage *)handle;
	unsigned char *section;
	unsigned long sectionLen;
	if(image && ILImageGetSection(image, IL_SECTION_RESOURCES,
								  (void **)&section, &sectionLen))
	{
		return (ILInt32)(section[(ILNativeInt)position]);
	}
	else
	{
		return -1;
	}
}

/*
 * private static byte *ResourceGetAddress(IntPtr handle, long position);
 */
ILUInt8 *_IL_ClrResourceStream_ResourceGetAddress(ILExecThread *_thread,
												  ILNativeInt handle,
												  ILInt64 position)
{
	ILImage *image = (ILImage *)handle;
	unsigned char *section;
	unsigned long sectionLen;
	if(image && ILImageGetSection(image, IL_SECTION_RESOURCES,
								  (void **)&section, &sectionLen))
	{
		return (ILUInt8 *)(section + (ILNativeInt)position);
	}
	else
	{
		return (ILUInt8 *)0;
	}
}

/*
 * private static byte *I18N.CJK.CodeTable.GetAddress(Stream stream,
 *													  long position);
 *
 * This provides back door access to "ResourceGetAddress" from "I18N.CJK".
 */
ILUInt8 *_IL_CodeTable_GetAddress(ILExecThread *_thread,
								  ILObject *stream,
								  ILInt64 position)
{
	ILClass *classInfo;
	ILUInt8 *result;

	/* Verify that "stream" is an instance of "ClrResourceStream" */
	if(!stream)
	{
		return 0;
	}
	classInfo = ILExecThreadLookupClass
		(_thread, "System.Reflection.ClrResourceStream");
	if(!classInfo)
	{
		return 0;
	}
	if(GetObjectClass(stream) != classInfo)
	{
		return 0;
	}

	/* Make a call to "ClrResourceStream.GetAddress", which will find
	   the resource handle and adjust "position" for the resource start.
	   It will then call back to "ResourceGetAddress" above */
	result = 0;
	ILExecThreadCallNamed(_thread, "System.Reflection.ClrResourceStream",
						  "GetAddress", "(Tl)*B", &result, stream, position);
	return result;
}

#ifdef	__cplusplus
};
#endif
