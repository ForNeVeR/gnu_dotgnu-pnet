/*
 * generic.c - Process generic information from an image file.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

#include "program.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILGenericPar *ILGenericParCreate(ILImage *image, ILToken token,
								 ILProgramItem *owner, ILUInt32 number)
{
	ILGenericPar *genPar;
	ILClass *classInfo;

	/* Allocate space for the GenericPar block from the memory stack */
	genPar = ILMemStackAlloc(&(image->memStack), ILGenericPar);
	if(!genPar)
	{
		return 0;
	}

	/* Set the GenericPar information fields */
	genPar->ownedItem.programItem.image = image;
	genPar->ownedItem.owner = owner;
	genPar->number = (ILUInt16)number;
	genPar->flags = 0;
	genPar->name = 0;
	genPar->kind = 0;
	genPar->constraint = 0;

	/* Assign a token code to the GenericPar information block */
	if(!_ILImageSetToken(image, &(genPar->ownedItem.programItem), token,
						 IL_META_TOKEN_GENERIC_PAR))
	{
		return 0;
	}

	/* Mark the class as having generic parameters */
	classInfo = ILProgramItemToClass(owner);
	if(classInfo)
	{
		classInfo->attributes |= IL_META_TYPEDEF_GENERIC_PARS;
	}

	/* Return the GenericPar information block to the caller */
	return genPar;
}

ILUInt32 ILGenericParGetNumber(ILGenericPar *genPar)
{
	return genPar->number;
}

ILUInt32 ILGenericParGetFlags(ILGenericPar *genPar)
{
	return genPar->flags;
}

void ILGenericParSetFlags(ILGenericPar *genPar, ILUInt32 mask, ILUInt32 value)
{
	genPar->flags = (ILUInt16)((genPar->flags & ~mask) | value);
}

ILProgramItem *ILGenericParGetOwner(ILGenericPar *genPar)
{
	return genPar->ownedItem.owner;
}

const char *ILGenericParGetName(ILGenericPar *genPar)
{
	return genPar->name;
}

int ILGenericParSetName(ILGenericPar *genPar, const char *name)
{
	genPar->name = _ILContextPersistString
			(genPar->ownedItem.programItem.image, name);
	return (genPar->name != 0);
}

ILProgramItem *ILGenericParGetKind(ILGenericPar *genPar)
{
	return genPar->kind;
}

void ILGenericParSetKind(ILGenericPar *genPar, ILProgramItem *type)
{
	genPar->kind = type;
}

ILProgramItem *ILGenericParGetConstraint(ILGenericPar *genPar)
{
	return genPar->constraint;
}

void ILGenericParSetConstraint(ILGenericPar *genPar, ILProgramItem *type)
{
	genPar->constraint = type;
}

/*
 * Search key for "GenericParCompare".
 */
typedef struct
{
	ILProgramItem *owner;
	ILUInt32	   number;

} ILGenericParSearchKey;

/*
 * Compare GenericPar tokens looking for a match.
 */
static int GenericParCompare(void *item, void *userData)
{
	ILGenericPar *genPar = (ILGenericPar *)item;
	ILGenericParSearchKey *key = (ILGenericParSearchKey *)userData;
	ILToken token1 =
		(genPar->ownedItem.owner ? genPar->ownedItem.owner->token : 0);
	ILToken token1Stripped = (token1 & ~IL_META_TOKEN_MASK);
	ILToken token2 = key->owner->token;
	ILToken token2Stripped = (token2 & ~IL_META_TOKEN_MASK);
	if(token1Stripped < token2Stripped)
	{
		return -1;
	}
	else if(token1Stripped > token2Stripped)
	{
		return 1;
	}
	else if(token1 < token2)
	{
		return -1;
	}
	else if(token1 > token2)
	{
		return 1;
	}
	else if(genPar->number < key->number)
	{
		return -1;
	}
	else if(genPar->number > key->number)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

ILGenericPar *ILGenericParGetFromOwner(ILProgramItem *owner, ILUInt32 number)
{
	ILGenericParSearchKey key;

	/* Take a short-cut if we know that the item is not generic */
	if((owner->token & IL_META_TOKEN_MASK) == IL_META_TOKEN_TYPE_DEF &&
	   (((ILClass *)owner)->attributes & IL_META_TYPEDEF_GENERIC_PARS) == 0)
	{
		return 0;
	}
	if((owner->token & IL_META_TOKEN_MASK) == IL_META_TOKEN_METHOD_DEF &&
	   (ILMethod_CallConv((ILMethod *)owner) & IL_META_CALLCONV_GENERIC) == 0)
	{
		return 0;
	}

	/* Search for the generic parameter information */
	key.owner = owner;
	key.number = number;
	return (ILGenericPar *)ILImageSearchForToken
				(owner->image, IL_META_TOKEN_GENERIC_PAR,
				 GenericParCompare, (void *)&key);
}

ILUInt32 ILGenericParGetNumParams(ILProgramItem *owner)
{
	ILUInt32 number = 0;
	while(ILGenericParGetFromOwner(owner, number) != 0)
	{
		++number;
	}
	return number;
}

ILMethodSpec *ILMethodSpecCreate(ILImage *image, ILToken token,
								 ILMember *method, ILType *type)
{
	ILMethodSpec *spec;

	/* Allocate space for the MethodSpec block from the memory stack */
	spec = ILMemStackAlloc(&(image->memStack), ILMethodSpec);
	if(!spec)
	{
		return 0;
	}

	/* Set the MethodSpec information fields */
	spec->programItem.image = image;
	spec->method = method;
	spec->type = type;
	spec->typeBlob = 0;

	/* Assign a token code to the MethodSpec information block */
	if(!_ILImageSetToken(image, &(spec->programItem), token,
						 IL_META_TOKEN_METHOD_SPEC))
	{
		return 0;
	}

	/* Return the MethodSpec information block to the caller */
	return spec;
}

ILMember *ILMethodSpecGetMethod(ILMethodSpec *spec)
{
	return spec->method;
}

ILType *ILMethodSpecGetType(ILMethodSpec *spec)
{
	return spec->type;
}

void _ILMethodSpecSetTypeIndex(ILMethodSpec *spec, ILUInt32 index)
{
	spec->typeBlob = index;
}

int ILTypeNeedsInstantiation(ILType *type)
{
	unsigned long num;
	unsigned long posn;

	if(type != 0 && ILType_IsComplex(type))
	{
		switch(ILType_Kind(type))
		{
			case IL_TYPE_COMPLEX_BYREF:
			case IL_TYPE_COMPLEX_PTR:
			case IL_TYPE_COMPLEX_PINNED:
			{
				return ILTypeNeedsInstantiation(ILType_Ref(type));
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_ARRAY:
			case IL_TYPE_COMPLEX_ARRAY_CONTINUE:
			{
				return ILTypeNeedsInstantiation(ILTypeGetElemType(type));
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_CMOD_REQD:
			case IL_TYPE_COMPLEX_CMOD_OPT:
			{
				return ILTypeNeedsInstantiation(type->un.modifier__.type__);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_LOCALS:
			{
				num = ILTypeNumLocals(type);
				for(posn = 0; posn < num; ++posn)
				{
					if(ILTypeNeedsInstantiation(ILTypeGetLocal(type, posn)))
					{
						return 1;
					}
				}
			}
			break;

			case IL_TYPE_COMPLEX_MVAR:
			case IL_TYPE_COMPLEX_VAR:
			{
				return 1;
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_WITH:
			case IL_TYPE_COMPLEX_PROPERTY:
			case IL_TYPE_COMPLEX_METHOD:
			case IL_TYPE_COMPLEX_METHOD | IL_TYPE_COMPLEX_SENTINEL:
			{
				if(ILTypeNeedsInstantiation(ILTypeGetReturn(type)))
				{
					return 1;
				}
				num = ILTypeNumParams(type);
				for(posn = 1; posn <= num; ++posn)
				{
					if(ILTypeNeedsInstantiation(ILTypeGetParam(type, posn)))
					{
						return 1;
					}
				}
			}
			break;
		}
	}
	return 0;
}

ILType *ILTypeInstantiate(ILContext *context, ILType *type,
						  ILType *classParams, ILType *methodParams)
{
	ILType *inner;
	ILType *newType;
	unsigned long num;
	unsigned long posn;

	/* Bail out immediately if the type does not need to be instantiated */
	if(!ILTypeNeedsInstantiation(type))
	{
		return type;
	}

	/* Re-construct the type with the instantiations in place */
	switch(ILType_Kind(type))
	{
		case IL_TYPE_COMPLEX_BYREF:
		case IL_TYPE_COMPLEX_PTR:
		case IL_TYPE_COMPLEX_PINNED:
		{
			/* Instantiate a simple reference type */
			inner = ILTypeInstantiate(context, ILType_Ref(type),
									  classParams, methodParams);
			if(inner)
			{
				type = ILTypeCreateRef(context, ILType_Kind(type), inner);
			}
			else
			{
				type = ILType_Invalid;
			}
		}
		break;

		case IL_TYPE_COMPLEX_ARRAY:
		case IL_TYPE_COMPLEX_ARRAY_CONTINUE:
		{
			/* Instantiate an array type */
			inner = ILTypeInstantiate(context, ILTypeGetElemType(type),
									  classParams, methodParams);
			if(inner)
			{
				type = ILTypeCreateArray(context, ILTypeGetRank(type), inner);
			}
			else
			{
				type = ILType_Invalid;
			}
		}
		break;

		case IL_TYPE_COMPLEX_CMOD_REQD:
		case IL_TYPE_COMPLEX_CMOD_OPT:
		{
			/* Instantiate a custom modifier reference */
			inner = ILTypeInstantiate(context, type->un.modifier__.type__,
									  classParams, methodParams);
			type = ILTypeCreateModifier
				(context, 0, ILType_Kind(type), type->un.modifier__.info__);
			if(type)
			{
				type = ILTypeAddModifiers(context, type, inner);
			}
		}
		break;

		case IL_TYPE_COMPLEX_LOCALS:
		{
			/* Instantiate a local variable signature */
			newType = ILTypeCreateLocalList(context);
			if(!newType)
			{
				return ILType_Invalid;
			}
			num = ILTypeNumLocals(type);
			for(posn = 0; posn < num; ++posn)
			{
				inner = ILTypeInstantiate(context, ILTypeGetLocalWithPrefixes
														(type, posn),
										  classParams, methodParams);
				if(!inner || !ILTypeAddLocal(context, newType, inner))
				{
					return ILType_Invalid;
				}
			}
			return newType;
		}
		/* Not reached */

		case IL_TYPE_COMPLEX_MVAR:
		{
			/* Instantiate a generic method variable reference */
			if(methodParams)
			{
				return ILTypeGetParamWithPrefixes
					(methodParams, ILType_VarNum(type) + 1);
			}
			else
			{
				return 0;
			}
		}
		/* Not reached */

		case IL_TYPE_COMPLEX_VAR:
		{
			/* Instantiate a generic class variable reference */
			if(classParams)
			{
				return ILTypeGetWithParamWithPrefixes
					(classParams, ILType_VarNum(type) + 1);
			}
			else
			{
				return 0;
			}
		}
		/* Not reached */

		case IL_TYPE_COMPLEX_WITH:
		case IL_TYPE_COMPLEX_PROPERTY:
		case IL_TYPE_COMPLEX_METHOD:
		case IL_TYPE_COMPLEX_METHOD | IL_TYPE_COMPLEX_SENTINEL:
		{
			/* Instantiate a method signature */
			inner = ILTypeGetReturnWithPrefixes(type);
			if(inner)
			{
				inner = ILTypeInstantiate
					(context, type, classParams, methodParams);
				if(!inner)
				{
					return ILType_Invalid;
				}
			}
			newType = ILTypeCreateMethod(context, inner);
			if(!newType)
			{
				return ILType_Invalid;
			}
			newType->kind__ = type->kind__;
			num = ILTypeNumParams(type);
			for(posn = 0; posn < num; ++posn)
			{
				inner = ILTypeInstantiate(context, ILTypeGetParamWithPrefixes
														(type, posn),
										  classParams, methodParams);
				if(!inner || !ILTypeAddParam(context, newType, inner))
				{
					return ILType_Invalid;
				}
			}
			return newType;
		}
		/* Not reached */
	}
	return type;
}

/*
 * Expand a class reference.
 */
static ILClass *ExpandClass(ILImage *image, ILClass *classInfo,
							ILType *classParams)
{
	ILType *type = ILClassToType(classInfo);
	return ILClassInstantiate(image, type, classParams);
}

/*
 * Expand the instantiations in a class.  Returns zero if out of memory.
 */
static int ExpandInstantiations(ILImage *image, ILClass *classInfo,
								ILType *classType, ILType *classParams)
{
	ILClass *origClass;
	ILMember *member;
	ILMethod *newMethod;
	ILField *newField;
	ILType *signature;
	ILImplements *impl;
	ILClass *tempInfo;

	/* Mark this class as being expanded, to deal with circularities */
	classInfo->attributes |= IL_META_TYPEDEF_CLASS_EXPANDED;

	/* Bail out if not a "with" type, since the instantiation would
	   have already been taken care of by "ILClassFromType" */
	if(!ILType_IsWith(classType))
	{
		return 1;
	}

	/* Find the original class underlying the type */
	origClass = ILClassFromType(image, 0, ILTypeGetWithMain(classType), 0);
	if(!origClass)
	{
		return 0;
	}
	origClass = ILClassResolve(origClass);

	/* Remember the original class so we can find it again later */
	classInfo->ext = (ILClassExt *)origClass;

	/* Copy across the class attributes */
	ILClassSetAttrs(classInfo, ~((ILUInt32)0), ILClass_Attrs(origClass));

	/* Expand the parent class and interfaces */
	if(origClass->parent)
	{
		classInfo->parent = ExpandClass
			(image, ILClass_Parent(origClass), classParams);
		if(!(classInfo->parent))
		{
			return 0;
		}
	}
	impl = 0;
	while((impl = ILClassNextImplements(origClass, impl)) != 0)
	{
		tempInfo = ILImplementsGetInterface(impl);
		tempInfo = ExpandClass(image, tempInfo, classParams);
		if(!tempInfo)
		{
			return 0;
		}
		if(!ILClassAddImplements(classInfo, tempInfo, 0))
		{
			return 0;
		}
	}

	/* Expand the methods and fields */
	member = 0;
	while((member = ILClassNextMember(origClass, member)) != 0)
	{
		switch(ILMemberGetKind(member))
		{
			case IL_META_MEMBERKIND_METHOD:
			{
				/* Skip static methods, which are shared */
				if(ILMethod_IsStatic((ILMethod *)member))
				{
					break;
				}

				/* Create a new method */
				newMethod = ILMethodCreate(classInfo, 0,
										   ILMember_Name(member),
										   ILMember_Attrs(member));
				if(!newMethod)
				{
					return 0;
				}

				/* Copy the original method's properties */
				signature = ILTypeInstantiate
					(image->context, ILMember_Signature(member),
					 classParams, 0);
				if(!signature)
				{
					return 0;
				}
				ILMethodSetImplAttrs
					(newMethod, ~((ILUInt32)0),
					 ILMethod_ImplAttrs((ILMethod *)member));
				ILMethodSetCallConv
					(newMethod, ILMethod_CallConv((ILMethod *)member));
				ILMethodSetRVA(newMethod, ILMethod_RVA((ILMethod *)member));

				/* Remember the mapping, so we can resolve method references */
				newMethod->userData = (void *)member;

				/* Copy the original method's parameter blocks */
				/* TODO */
			}
			break;

			case IL_META_MEMBERKIND_FIELD:
			{
				/* Skip static fields, which are shared */
				if(ILField_IsStatic((ILField *)member))
				{
					break;
				}

				/* Create a new field */
				newField = ILFieldCreate(classInfo, 0,
										 ILMember_Name(member),
										 ILMember_Attrs(member));
				if(!newField)
				{
					return 0;
				}

				/* Copy the original field's properties */
				signature = ILTypeInstantiate
					(image->context, ILMember_Signature(member),
					 classParams, 0);
				if(!signature)
				{
					return 0;
				}
			}
			break;
		}
	}

	/* Expand the properties, events, overrides, and pinvokes */
	member = 0;
	while((member = ILClassNextMember(origClass, member)) != 0)
	{
		switch(ILMemberGetKind(member))
		{
			case IL_META_MEMBERKIND_PROPERTY:
			{
				/* TODO */
			}
			break;

			case IL_META_MEMBERKIND_EVENT:
			{
				/* TODO */
			}
			break;

			case IL_META_MEMBERKIND_OVERRIDE:
			{
				/* TODO */
			}
			break;

			case IL_META_MEMBERKIND_PINVOKE:
			{
				/* TODO */
			}
			break;
		}
	}

	/* Clear the "userData" fields on the new methods, because
	   we don't need them any more */
	member = 0;
	while((member = ILClassNextMemberByKind
				(classInfo, member, IL_META_MEMBERKIND_METHOD)) != 0)
	{
		((ILMethod *)member)->userData = 0;
	}

	/* Done */
	return 1;
}

ILClass *ILClassInstantiate(ILImage *image, ILType *classType,
							ILType *classParams)
{
	ILClass *classInfo;
	ILType *type;

	/* Bail out early if the type does not need instantiation */
	if(!ILTypeNeedsInstantiation(classType))
	{
		return ILClassFromType(image, 0, classType, 0);
	}

	/* Search for a synthetic type that matches the expanded
	   form of the class type, in case we already instantiated
	   this class previously.  We do this in such a way that we
	   won't need to call "ILTypeInstantiate" unless necessary */
	classInfo = _ILTypeToSyntheticInstantiation(image, classType, classParams);
	if(classInfo)
	{
		if((classInfo->attributes & IL_META_TYPEDEF_CLASS_EXPANDED) == 0)
		{
			if(!ExpandInstantiations(image, classInfo, classType, classParams))
			{
				return 0;
			}
		}
		return classInfo;
	}

	/* Instantiate the class type */
	type = ILTypeInstantiate(image->context, classType, classParams, 0);
	if(!type)
	{
		return 0;
	}

	/* Create a synthetic type for the expanded form */
	classInfo = ILClassFromType(image, 0, type, 0);
	if(!classInfo)
	{
		return 0;
	}
	if(!ExpandInstantiations(image, classInfo, type, classParams))
	{
		return 0;
	}
	return classInfo;
}

ILClass *ILClassGetUnderlying(ILClass *info)
{
	ILType *synType = info->synthetic;
	if(ILType_IsWith(synType))
	{
		synType = ILTypeGetWithMain(synType);
		return ILClassFromType(info->programItem.image, 0, synType, 0);
	}
	else
	{
		return info;
	}
}

#ifdef	__cplusplus
};
#endif
