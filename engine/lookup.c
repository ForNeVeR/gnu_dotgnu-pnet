/*
 * lookup.c - Look up methods and fields by name.
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

/*

The naming scheme is loosely based on the Java signature scheme,
with a few changes to make it C#-friendly.

Types are named using their full name, with namespace separated by
periods.  Nested types must be fully qualified, with the nesting levels
separated by slashes.  e.g. "System.String" and "System.Enum/HashEntry".

Signatures are specified using the following characters:

	V			void
	Z			bool
	b			sbyte
	B			byte
	s			short
	S			ushort
	c			char
	i			int
	I			uint
	j			native int
	J			native uint
	l			long
	L			ulong
	f			float
	d			double
	D			native double
	r			typedref
	&type		byref "type"
	*type		ptr "type"
	vname;		value type called "name"
	oname;		object reference called "name"
	[type		array of "type"
	{n,type		n-dimensional array of "type"
	T			"this" parameter, indicating an instance method

An example of a signature:

	struct System.Decimal
	{
		public System.String ToString();
	}

	"System.Decimal" "ToString" "(T)OSystem.String;"
*/

#include "engine.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILClass *ILExecThreadLookupType(ILExecThread *thread, const char *typeName)
{
	int len, dot;
	const char *name;
	int nameLen;
	const char *namespace;
	int namespaceLen;
	ILClass *classInfo;
	ILImage *image;

	/* Sanity-check the arguments */
	if(!thread || !typeName)
	{
		return 0;
	}

	/* Get the name of the global type */
	len = 0;
	dot = -1;
	while(typeName[len] != '\0' && typeName[len] != '/')
	{
		if(typeName[len] == '.')
		{
			dot = len;
		}
		++len;
	}

	/* Break the global type into namespace and name components */
	if(dot != -1)
	{
		name = typeName + dot + 1;
		nameLen = len - dot - 1;
		namespace = typeName;
		namespaceLen = dot;
	}
	else
	{
		name = typeName;
		nameLen = len;
		namespace = 0;
		namespaceLen = 0;
	}

	/* Look for the global type within the process's context */
	classInfo = 0;
	if(nameLen > 7 && !strncmp(name, "System.", 7))
	{
		/* Try looking in the system image first, to prevent the
		   application from redirecting system types elsewhere */
		image = ILContextGetSystem(thread->process->context);
		if(image)
		{
			classInfo = ILClassLookupLen(ILClassGlobalScope(image),
										 name, nameLen,
										 namespace, namespaceLen);
		}
	}
	if(!classInfo)
	{
		classInfo = ILClassLookupGlobalLen(thread->process->context,
									   	   name, nameLen,
										   namespace, namespaceLen);
	}

	/* Resolve nested type names */
	while(classInfo != 0 && typeName[len] == '/')
	{
		++len;
		dot = len;
		while(typeName[len] != '\0' && typeName[len] != '/')
		{
			++len;
		}
		name = typeName + dot;
		nameLen = len - dot;
		classInfo = ILClassLookupLen(ILToProgramItem(classInfo),
									 name, nameLen, 0, 0);
	}

	/* Make sure that the class has been laid out */
	if(classInfo != 0)
	{
		if(!_ILLayoutClass(classInfo))
		{
			return 0;
		}
	}

	/* Return the final class structure to the caller */
	return classInfo;
}

/*
 * Inner version of "MatchClassName".
 */
static int MatchClassNameInner(ILClass *classInfo, const char *name)
{
	ILClass *nestedParent;
	int len, complen, dot;
	const char *temp;

	/* Match the nested parent first */
	nestedParent = ILClass_NestedParent(classInfo);
	if(nestedParent)
	{
		len = MatchClassNameInner(nestedParent, name);
		if(!len || name[len] != '/')
		{
			return 0;
		}
		++len;
	}
	else
	{
		len = 0;
	}

	/* Extract the next component from the name */
	complen = len;
	dot = -1;
	while(name[complen] != '\0' && name[complen] != ';' &&
	      name[complen] != '/')
	{
		if(name[complen] == '.')
		{
			dot = complen;
		}
		++complen;
	}
	if(complen <= len)
	{
		/* Empty components are not valid class names */
		return 0;
	}

	/* Match the component against this class */
	if(nestedParent == 0)
	{
		if(dot != -1)
		{
			/* Match both name and namespace */
			temp = ILClass_Namespace(classInfo);
			if(strlen(temp) != (dot - len) ||
			   strncmp(temp, name + len, dot - len) != 0)
			{
				return 0;
			}
			temp = ILClass_Name(classInfo);
			if(strlen(temp) != (complen - dot - 1) ||
			   strncmp(temp, name + dot + 1, complen - dot - 1) != 0)
			{
				return 0;
			}
		}
		else
		{
			/* This class should not have a namespace */
			if(ILClass_Namespace(classInfo) != 0)
			{
				return 0;
			}
			temp = ILClass_Name(classInfo);
			if(strlen(temp) != (complen - len) ||
			   strncmp(temp, name + len, complen - len) != 0)
			{
				return 0;
			}
		}
	}
	else
	{
		/* Nested classes only match the name */
		temp = ILClass_Name(classInfo);
		if(strlen(temp) != (complen - len) ||
		   strncmp(temp, name + len, complen - len) != 0)
		{
			return 0;
		}
	}

	/* The whole name has been matched */
	return complen;
}

/*
 * Match a class name against a supplied signature name.
 * Returns the number of characters matched, or zero.
 */
static int MatchClassName(ILClass *classInfo, const char *name)
{
	int len = MatchClassNameInner(classInfo, name);
	if(!len || name[len] != ';')
	{
		return 0;
	}
	else
	{
		return len;
	}
}

/*
 * Count the number of ranks in an array type and extract
 * the inner-most element type.  Returns -1 if the array
 * ranks are not as expected.
 */
static int CountRanks(ILType *type, ILType **elemType)
{
	int rank = 1;
	while(type->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
	{
		if(type->un.array.size != 0 || type->un.array.lowBound != 0)
		{
			return -1;
		}
		type = type->un.array.elemType;
		++rank;
	}
	if(type->un.array.size != 0 || type->un.array.lowBound != 0)
	{
		return -1;
	}
	*elemType = type->un.array.elemType;
	return rank;
}

/*
 * Match a type against a supplied signature name.  Returns the
 * number of name characters matched, or zero if not matched.
 */
static int MatchTypeName(ILType *type, const char *name)
{
	int len, rank;
	ILType *elemType;

	if(ILType_IsPrimitive(type))
	{
		/* Match a primitive type */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:	    if(*name == 'V') return 1; break;
			case IL_META_ELEMTYPE_BOOLEAN:	if(*name == 'Z') return 1; break;
			case IL_META_ELEMTYPE_I1:		if(*name == 'b') return 1; break;
			case IL_META_ELEMTYPE_U1:		if(*name == 'B') return 1; break;
			case IL_META_ELEMTYPE_I2:		if(*name == 's') return 1; break;
			case IL_META_ELEMTYPE_U2:		if(*name == 'S') return 1; break;
			case IL_META_ELEMTYPE_CHAR:		if(*name == 'c') return 1; break;
			case IL_META_ELEMTYPE_I4:		if(*name == 'i') return 1; break;
			case IL_META_ELEMTYPE_U4:		if(*name == 'I') return 1; break;
			case IL_META_ELEMTYPE_I:		if(*name == 'j') return 1; break;
			case IL_META_ELEMTYPE_U:		if(*name == 'J') return 1; break;
			case IL_META_ELEMTYPE_I8:		if(*name == 'l') return 1; break;
			case IL_META_ELEMTYPE_U8:		if(*name == 'L') return 1; break;
			case IL_META_ELEMTYPE_R4:		if(*name == 'f') return 1; break;
			case IL_META_ELEMTYPE_R8:		if(*name == 'd') return 1; break;
			case IL_META_ELEMTYPE_R:		if(*name == 'D') return 1; break;
			case IL_META_ELEMTYPE_TYPEDBYREF: if(*name == 'r') return 1; break;
		}
		return 0;
	}
	else if(ILType_IsValueType(type))
	{
		/* Match a value type */
		if(*name != 'v')
		{
			return 0;
		}
		len = MatchClassName(ILType_ToValueType(type), name + 1);
		if(len != 0)
		{
			return len + 2;
		}
		else
		{
			return 0;
		}
	}
	else if(ILType_IsClass(type))
	{
		/* Match an object reference */
		if(*name != 'o')
		{
			return 0;
		}
		len = MatchClassName(ILType_ToClass(type), name + 1);
		if(len != 0)
		{
			return len + 2;
		}
		else
		{
			return 0;
		}
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		/* Match some other kind of complex type */
		if(type->kind == IL_TYPE_COMPLEX_BYREF)
		{
			/* Match a reference type */
			if(*name != '&')
			{
				return 0;
			}
			len = MatchTypeName(type->un.refType, name + 1);
			if(len != 0)
			{
				return len + 1;
			}
			else
			{
				return 0;
			}
		}
		else if(type->kind == IL_TYPE_COMPLEX_PTR)
		{
			/* Match a pointer type */
			if(*name != '*')
			{
				return 0;
			}
			len = MatchTypeName(type->un.refType, name + 1);
			if(len != 0)
			{
				return len + 1;
			}
			else
			{
				return 0;
			}
		}
		else if(type->kind == IL_TYPE_COMPLEX_ARRAY)
		{
			/* Match a single-dimensional array type */
			if(*name != '[')
			{
				return 0;
			}
			if(type->un.array.size != 0 ||
			   type->un.array.lowBound != 0)
			{
				return 0;
			}
			len = MatchTypeName(type->un.array.elemType, name + 1);
			if(len != 0)
			{
				return len + 1;
			}
			else
			{
				return 0;
			}
		}
		else if(type->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
		{
			/* Match a multi-dimensional array type */
			if(*name != '{')
			{
				return 0;
			}
			len = 1;
			rank = 0;
			while(name[len] != '\0' && name[len] != ',')
			{
				if(name[len] >= '0' && name[len] <= '9')
				{
					rank = rank * 10 + (int)(name[len++]);
				}
				else
				{
					return 0;
				}
			}
			if(name[len] != ',')
			{
				return 0;
			}
			++len;
			if(rank != CountRanks(type, &elemType))
			{
				return 0;
			}
			rank = MatchTypeName(elemType, name + len);
			if(!rank)
			{
				return 0;
			}
			else
			{
				return len + rank;
			}
		}
		else
		{
			/* Don't know how to match this type */
			return 0;
		}
	}
	else
	{
		/* Unrecognised type */
		return 0;
	}
}

/*
 * Match a method signature against a supplied signature name.
 */
static int MatchSignatureName(ILType *signature, const char *name)
{
	ILUInt32 param, numParams;
	ILType *paramType;
	int nameLen;
	int matchLen;

	/* Match the parameters */
	numParams = signature->num;
	for(param = 1; param <= numParams; ++param)
	{
		paramType = ILTypeGetParam(signature, param);
		nameLen = MatchTypeName(paramType, name);
		if(!nameLen)
		{
			return 0;
		}
		name += nameLen;
	}

	/* Match the return type */
	if(*name != ')')
	{
		return 0;
	}
	matchLen = MatchTypeName(signature->un.method.retType, name + 1);
	if(matchLen == 0 || name[matchLen + 1] != '\0')
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

ILMethod *ILExecThreadLookupMethod(ILExecThread *thread,
								   const char *typeName,
								   const char *methodName,
								   const char *signature)
{
	ILClass *classInfo;
	ILMethod *method;
	ILType *methodSignature;

	/* Sanity-check the arguments */
	if(!methodName || !signature || *signature != '(')
	{
		return 0;
	}

	/* Look up the type */
	classInfo = ILExecThreadLookupType(thread, typeName);

	/* Resolve the method within the type or any of its ancestors */
	while(classInfo != 0)
	{
		classInfo = ILClassResolve(classInfo);
		method = 0;
		while((method = (ILMethod *)ILClassNextMemberByKind
					(classInfo, (ILMember *)method,
					 IL_META_MEMBERKIND_METHOD)) != 0)
		{
			if(!strcmp(ILMethod_Name(method), methodName))
			{
				methodSignature = ILMethod_Signature(method);
				if(ILType_HasThis(methodSignature))
				{
					/* The method has a "this" parameter */
					if(signature[1] != 'T')
					{
						continue;
					}
					if(MatchSignatureName(methodSignature, signature + 2))
					{
						return method;
					}
				}
				else
				{
					/* The method does not have a "this" parameter */
					if(MatchSignatureName(methodSignature, signature + 1))
					{
						return method;
					}
				}
			}
		}
		classInfo = ILClass_ParentRef(classInfo);
	}

	/* Could not find the method */
	return 0;
}

ILField *ILExecThreadLookupField(ILExecThread *thread,
								 const char *typeName,
								 const char *fieldName,
								 const char *signature)
{
	ILClass *classInfo;
	ILField *field;
	ILType *fieldType;
	int matchCount;

	/* Sanity-check the arguments */
	if(!fieldName || !signature)
	{
		return 0;
	}

	/* Look up the type */
	classInfo = ILExecThreadLookupType(thread, typeName);

	/* Resolve the field within the type or any of its ancestors */
	while(classInfo != 0)
	{
		classInfo = ILClassResolve(classInfo);
		field = 0;
		while((field = (ILField *)ILClassNextMemberByKind
					(classInfo, (ILMember *)field,
					 IL_META_MEMBERKIND_FIELD)) != 0)
		{
			if(!strcmp(ILField_Name(field), fieldName))
			{
				fieldType = ILField_Type(field);
				matchCount = MatchTypeName(fieldType, signature);
				if(matchCount != 0 && signature[matchCount] == '\0')
				{
					return field;
				}
			}
		}
		classInfo = ILClass_ParentRef(classInfo);
	}

	/* Could not find the field */
	return 0;
}

int _ILLookupTypeMatch(ILType *type, const char *signature)
{
	int matchCount;
	if(*signature == '(')
	{
		/* Match a method signature */
		if(type == 0 || !ILType_IsComplex(type) ||
		   (type->kind & 0xFF) != IL_TYPE_COMPLEX_METHOD)
		{
			return 0;
		}
		if(ILType_HasThis(type))
		{
			/* The method has a "this" parameter */
			if(signature[1] != 'T')
			{
				return 0;
			}
			if(MatchSignatureName(type, signature + 2))
			{
				return 1;
			}
		}
		else
		{
			/* The method does not have a "this" parameter */
			if(MatchSignatureName(type, signature + 1))
			{
				return 1;
			}
		}
	}
	else
	{
		/* Match a field signature */
		matchCount = MatchTypeName(type, signature);
		if(matchCount != 0 && signature[matchCount] == '\0')
		{
			return 1;
		}
	}
	return 0;
}

#ifdef	__cplusplus
};
#endif
