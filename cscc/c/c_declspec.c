/*
 * c_declspec.c - Declaration specifiers for the C programming language.
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

#include <cscc/c/c_internal.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * List of valid storage class specifier combinations and names.
 */
typedef struct
{
	int			specifiers;
	int			applicableKinds;
	const char *name;

} StorageClassInfo;
static StorageClassInfo const storageClasses[] = {
	{C_SPEC_TYPEDEF,
		C_KIND_GLOBAL_NAME | C_KIND_LOCAL_NAME,
		"typedef"},
	{C_SPEC_EXTERN,
		C_KIND_GLOBAL_NAME | C_KIND_LOCAL_NAME | C_KIND_FUNCTION,
		"extern"},
	{C_SPEC_STATIC | C_SPEC_INLINE,
		C_KIND_FUNCTION,
		"static inline"},
	{C_SPEC_STATIC,
		C_KIND_GLOBAL_NAME | C_KIND_LOCAL_NAME | C_KIND_FUNCTION,
		"static"},
	{C_SPEC_AUTO,
		C_KIND_LOCAL_NAME,
		"auto"},
	{C_SPEC_REGISTER,
		C_KIND_LOCAL_NAME,
		"register"},
	{C_SPEC_INLINE,
		C_KIND_FUNCTION,
		"inline"},
};
#define	numStorageClasses	(sizeof(storageClasses) / sizeof(StorageClassInfo))

const char *CStorageClassToName(int specifier)
{
	int index;
	for(index = 0; index < numStorageClasses; ++index)
	{
		if(specifier == storageClasses[index].specifiers)
		{
			return storageClasses[index].name;
		}
	}
	return 0;
}

CDeclSpec CDeclSpecEmpty(void)
{
	CDeclSpec result;
	result.specifiers = 0;
	result.dupSpecifiers = 0;
	result.baseType = 0;
	return result;
}

CDeclSpec CDeclSpecCombine(CDeclSpec spec1, CDeclSpec spec2)
{
	CDeclSpec result;
	int okSpecifiers;

	/* Swap the values so that the base type is in "spec1" */
	if(!(spec1.baseType) && spec2.baseType)
	{
		result = spec1;
		spec1 = spec2;
		spec2 = result;
	}

	/* Copy any duplicates that we got on the previous call */
	result.dupSpecifiers = spec1.dupSpecifiers | spec2.dupSpecifiers;

	/* Combine the storage classes and common type specifiers */
	result.specifiers =
			((spec1.specifiers | spec2.specifiers) &
					(C_SPEC_STORAGE | C_SPEC_TYPE_COMMON));

	/* Detect duplicates in the storage classes and common type specifiers */
	result.dupSpecifiers |=
			(spec1.specifiers & spec2.specifiers &
			 (C_SPEC_STORAGE | C_SPEC_TYPE_COMMON));

	/* If both specifiers have a base type, then record an error for later */
	if(spec1.baseType && spec2.baseType)
	{
		result.dupSpecifiers |= C_SPEC_MULTIPLE_BASES;
	}

	/* If both are "long", then change one into "long long" */
	if((spec1.specifiers & C_SPEC_LONG) != 0 &&
	   (spec2.specifiers & C_SPEC_LONG) != 0)
	{
		spec2.specifiers =
			(spec2.specifiers & ~C_SPEC_LONG) | C_SPEC_LONG_LONG;
	}

	/* Apply type specifiers to the base type */
	result.baseType = spec1.baseType;
	if(spec1.baseType != 0)
	{
		okSpecifiers = 0;
		if(ILType_IsPrimitive(spec1.baseType))
		{
			switch(ILType_ToElement(spec1.baseType))
			{
				case IL_META_ELEMTYPE_I1:
				{
					/* Look for "unsigned" to change int8 into uint8 */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt8;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_I2:
				{
					/* Look for "unsigned" to change int16 into uint16 */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt16;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_I4:
				{
					/* Look for "signed", "unsigned", "short",
					   "long", or "long long" */
					switch((spec1.specifiers | spec2.specifiers)
							& (C_SPEC_SIGNED | C_SPEC_UNSIGNED |
							   C_SPEC_SHORT | C_SPEC_LONG |
							   C_SPEC_LONG_LONG))
					{
						case C_SPEC_SIGNED:
						{
							result.baseType = ILType_Int32;
							okSpecifiers = C_SPEC_SIGNED;
						}
						break;

						case C_SPEC_UNSIGNED:
						{
							result.baseType = ILType_UInt32;
							okSpecifiers = C_SPEC_UNSIGNED;
						}
						break;

						case C_SPEC_SHORT:
						case C_SPEC_SIGNED | C_SPEC_SHORT:
						{
							result.baseType = ILType_Int16;
							okSpecifiers = C_SPEC_SIGNED | C_SPEC_SHORT;
						}
						break;

						case C_SPEC_UNSIGNED | C_SPEC_SHORT:
						{
							result.baseType = ILType_UInt16;
							okSpecifiers = C_SPEC_UNSIGNED | C_SPEC_SHORT;
						}
						break;

						case C_SPEC_LONG:
						case C_SPEC_SIGNED | C_SPEC_LONG:
						{
							if(gen_32bit_only)
							{
								result.baseType = ILType_Int32;
							}
							else
							{
								result.baseType = ILType_Int64;
							}
							okSpecifiers = C_SPEC_SIGNED | C_SPEC_LONG;
						}
						break;

						case C_SPEC_UNSIGNED | C_SPEC_LONG:
						{
							if(gen_32bit_only)
							{
								result.baseType = ILType_UInt32;
							}
							else
							{
								result.baseType = ILType_UInt64;
							}
							okSpecifiers = C_SPEC_UNSIGNED | C_SPEC_LONG;
						}
						break;

						case C_SPEC_SIGNED | C_SPEC_LONG_LONG:
						case C_SPEC_SIGNED | C_SPEC_LONG | C_SPEC_LONG_LONG:
						case C_SPEC_LONG_LONG:
						case C_SPEC_LONG | C_SPEC_LONG_LONG:
						{
							result.baseType = ILType_Int64;
							okSpecifiers = C_SPEC_SIGNED | C_SPEC_LONG |
										   C_SPEC_LONG_LONG;
						}
						break;

						case C_SPEC_UNSIGNED | C_SPEC_LONG_LONG:
						case C_SPEC_UNSIGNED | C_SPEC_LONG | C_SPEC_LONG_LONG:
						{
							result.baseType = ILType_UInt64;
							okSpecifiers = C_SPEC_UNSIGNED | C_SPEC_LONG |
										   C_SPEC_LONG_LONG;
						}
						break;
					}
				}
				break;

				case IL_META_ELEMTYPE_U4:
				{
					/* Look for "short", "long", or "long long" */
					switch((spec1.specifiers | spec2.specifiers)
							& (C_SPEC_SHORT | C_SPEC_LONG |
							   C_SPEC_LONG_LONG))
					{
						case C_SPEC_SHORT:
						{
							result.baseType = ILType_UInt16;
							okSpecifiers = C_SPEC_SHORT;
						}
						break;

						case C_SPEC_LONG:
						{
							if(gen_32bit_only)
							{
								result.baseType = ILType_UInt32;
							}
							else
							{
								result.baseType = ILType_UInt64;
							}
							okSpecifiers = C_SPEC_LONG;
						}
						break;

						case C_SPEC_LONG_LONG:
						case C_SPEC_LONG | C_SPEC_LONG_LONG:
						{
							result.baseType = ILType_UInt64;
							okSpecifiers = C_SPEC_LONG | C_SPEC_LONG_LONG;
						}
						break;
					}
				}
				break;

				case IL_META_ELEMTYPE_I8:
				{
					/* Look for "signed" and "unsigned" */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt64;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
					else if(((spec1.specifiers | spec2.specifiers)
								& C_SPEC_SIGNED) != 0)
					{
						result.baseType = ILType_Int64;
						okSpecifiers = C_SPEC_SIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_U8:
				{
					/* Look for "unsigned" */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt64;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_I:
				{
					/* Look for "signed" and "unsigned" */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
					else if(((spec1.specifiers | spec2.specifiers)
								& C_SPEC_SIGNED) != 0)
					{
						result.baseType = ILType_Int;
						okSpecifiers = C_SPEC_SIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_U:
				{
					/* Look for "unsigned" */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_R4:
				{
					/* Look for "long" and "long long" */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_LONG_LONG) != 0)
					{
						/* "long long float" == "long double" */
						result.baseType = ILType_Float;
						okSpecifiers = C_SPEC_LONG | C_SPEC_LONG_LONG;
					}
					else if(((spec1.specifiers | spec2.specifiers)
									& C_SPEC_LONG) != 0)
					{
						/* "long float" == "double" */
						result.baseType = ILType_Float32;
						okSpecifiers = C_SPEC_LONG;
					}
				}
				break;

				case IL_META_ELEMTYPE_R8:
				{
					/* Look for "long" and "long long" */
					if(((spec1.specifiers | spec2.specifiers)
							& (C_SPEC_LONG | C_SPEC_LONG_LONG)) != 0)
					{
						result.baseType = ILType_Float;
						okSpecifiers = C_SPEC_LONG | C_SPEC_LONG_LONG;
					}
				}
				break;

				case IL_META_ELEMTYPE_R:
				{
					/* Look for "long" and "long long" */
					if(((spec1.specifiers | spec2.specifiers)
							& (C_SPEC_LONG | C_SPEC_LONG_LONG)) != 0)
					{
						/* "long long double" == "long double" */
						result.baseType = ILType_Float;
						okSpecifiers = C_SPEC_LONG | C_SPEC_LONG_LONG;
					}
				}
				break;
			}
		}
		result.specifiers |= okSpecifiers;
		if(((spec1.specifiers | spec2.specifiers) &
				(C_SPEC_TYPE_CHANGE & ~okSpecifiers)) != 0)
		{
			result.dupSpecifiers |= C_SPEC_INVALID_COMBO;
		}
	}
	else
	{
		/* Copy the type change specifiers, but don't inspect them yet */
		result.specifiers |= ((spec1.specifiers | spec2.specifiers) &
							  C_SPEC_TYPE_CHANGE);
	}

	/* Check for "signed" and "unsigned" or "short" and "long" together */
	if(((spec1.specifiers | spec2.specifiers) &
			(C_SPEC_SIGNED | C_SPEC_UNSIGNED)) ==
					(C_SPEC_SIGNED | C_SPEC_UNSIGNED))
	{
		result.dupSpecifiers |= C_SPEC_SIGN_AND_UNSIGN;
	}
	if(((spec1.specifiers | spec2.specifiers) &
			(C_SPEC_SHORT | C_SPEC_LONG)) == (C_SPEC_SHORT | C_SPEC_LONG))
	{
		result.dupSpecifiers |= C_SPEC_LONG_AND_SHORT;
	}

	/* Check for duplicate "signed", "unsigned", and "short",
	   but don't worry about "long" as we max out the sizes above */
	result.dupSpecifiers |= ((spec1.specifiers | spec2.specifiers) &
								(C_SPEC_SIGNED | C_SPEC_UNSIGNED |
								 C_SPEC_SHORT));

	/* Done */
	return result;
}

/*
 * Report a warning for a duplicate specifier.
 */
static void ReportDuplicate(ILNode *node, int specifiers,
						    int flag, const char *specName)
{
	if((specifiers & flag) != 0)
	{
		CCWarningOnLine(yygetfilename(node), yygetlinenum(node),
						_("duplicate `%s'"), specName);
	}
}

CDeclSpec CDeclSpecFinalize(CDeclSpec spec, ILNode *node,
							const char *name, int kind)
{
	CDeclSpec result;
	int storageClass, test;
	int index;

	/* Validate the storage class specifiers */
	storageClass = (spec.specifiers & C_SPEC_STORAGE);
	result.specifiers = 0;
	result.dupSpecifiers = 0;
	for(index = 0; index < numStorageClasses; ++index)
	{
		test = storageClasses[index].specifiers;
		if((storageClass & test) == test)
		{
			if((storageClasses[index].applicableKinds & kind) != 0)
			{
				result.specifiers = (storageClass & test);
				if((storageClass & ~test) != 0)
				{
					CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
						_("multiple storage classes in declaration of `%s'"),
						name);
				}
			}
			else
			{
				CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
					_("`%s' is not applicable to the declaration of `%s'"),
					storageClasses[index].name, name);
			}
			break;
		}
	}

	/* Report duplicate storage classes and type specifiers */
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_TYPEDEF, "typedef");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_EXTERN, "extern");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_STATIC, "static");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_AUTO, "auto");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_REGISTER, "register");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_INLINE, "inline");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_CONST, "const");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_VOLATILE, "volatile");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_NATIVE, "__native__");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_SIGNED, "signed");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_UNSIGNED, "unsigned");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_SHORT, "short");

	/* Print pending errors that were detected by "CDeclSpecCombine" */
	if((spec.dupSpecifiers & C_SPEC_MULTIPLE_BASES) != 0)
	{
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
				_("two or more data types in declaration of `%s'"), name);
	}
	if((spec.dupSpecifiers & C_SPEC_SIGN_AND_UNSIGN) != 0)
	{
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
				_("both signed and unsigned specified for `%s'"), name);
	}
	if((spec.dupSpecifiers & C_SPEC_LONG_AND_SHORT) != 0)
	{
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
				_("both long and short specified for `%s'"), name);
	}
	if((spec.dupSpecifiers & (C_SPEC_LONG_AND_SHORT |
							  C_SPEC_SIGN_AND_UNSIGN |
							  C_SPEC_INVALID_COMBO))
			== C_SPEC_INVALID_COMBO)
	{
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
			_("long, short, signed, or unsigned invalid for `%s'"), name);
	}

	/* If we don't have a base type yet, then infer the most obvious one */
	if(spec.baseType == 0)
	{
		if((spec.specifiers & C_SPEC_SHORT) != 0)
		{
			if((spec.specifiers & C_SPEC_UNSIGNED) != 0)
			{
				result.baseType = ILType_UInt16;
			}
			else
			{
				result.baseType = ILType_Int16;
			}
		}
		else if((spec.specifiers & C_SPEC_LONG) != 0 && gen_32bit_only)
		{
			if((spec.specifiers & C_SPEC_UNSIGNED) != 0)
			{
				result.baseType = ILType_UInt32;
			}
			else
			{
				result.baseType = ILType_Int32;
			}
		}
		else if((spec.specifiers & (C_SPEC_LONG | C_SPEC_LONG_LONG)) != 0)
		{
			if((spec.specifiers & C_SPEC_UNSIGNED) != 0)
			{
				result.baseType = ILType_UInt64;
			}
			else
			{
				result.baseType = ILType_Int64;
			}
		}
		else if((spec.specifiers & C_SPEC_UNSIGNED) != 0)
		{
			result.baseType = ILType_UInt32;
		}
		else
		{
			CCWarning(yygetfilename(node), yygetlinenum(node),
					  _("type defaults to `int' in declaration of `%s'"),
					  name);
			result.baseType = ILType_Int32;
		}
	}
	else
	{
		result.baseType = spec.baseType;
	}

	/* Return the result to the caller */
	return result;
}

#ifdef	__cplusplus
};
#endif
