/*
 * cs_modifiers.c - Verify modifier combinations.
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

#include "cs_internal.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Report errors for each modifier in a mask.
 */
static void ModifierError(ILUInt32 modifiers, const char *msg)
{
	if((modifiers & CS_MODIFIER_PUBLIC) != 0)
	{
		CSError(msg, "public");
	}
	if((modifiers & CS_MODIFIER_PRIVATE) != 0)
	{
		CSError(msg, "private");
	}
	if((modifiers & CS_MODIFIER_PROTECTED) != 0)
	{
		CSError(msg, "protected");
	}
	if((modifiers & CS_MODIFIER_INTERNAL) != 0)
	{
		CSError(msg, "internal");
	}
	if((modifiers & CS_MODIFIER_NEW) != 0)
	{
		CSError(msg, "new");
	}
	if((modifiers & CS_MODIFIER_ABSTRACT) != 0)
	{
		CSError(msg, "abstract");
	}
	if((modifiers & CS_MODIFIER_SEALED) != 0)
	{
		CSError(msg, "sealed");
	}
	if((modifiers & CS_MODIFIER_STATIC) != 0)
	{
		CSError(msg, "static");
	}
	if((modifiers & CS_MODIFIER_READONLY) != 0)
	{
		CSError(msg, "readonly");
	}
	if((modifiers & CS_MODIFIER_VIRTUAL) != 0)
	{
		CSError(msg, "virtual");
	}
	if((modifiers & CS_MODIFIER_OVERRIDE) != 0)
	{
		CSError(msg, "override");
	}
	if((modifiers & CS_MODIFIER_EXTERN) != 0)
	{
		CSError(msg, "extern");
	}
	if((modifiers & CS_MODIFIER_UNSAFE) != 0)
	{
		CSError(msg, "unsafe");
	}
}

/*
 * Report errors for modifiers that cannot be used in a particular context.
 */
static void BadModifiers(ILUInt32 modifiers)
{
	ModifierError(modifiers, "`%s' cannot be used in this context");
}

void CSModifiersUsedTwice(ILUInt32 modifiers)
{
	ModifierError(modifiers, "`%s' specified multiple times");
}

ILUInt32 CSModifiersToTypeAttrs(ILUInt32 modifiers, int isNested)
{
	ILUInt32 attrs;

	/* Determine the access level of the class */
	if(!isNested)
	{
		/* Only "public" and "internal" can be used at the outermost scope */
		if((modifiers & CS_MODIFIER_PUBLIC) != 0)
		{
			attrs = IL_META_TYPEDEF_PUBLIC;
			if((modifiers & CS_MODIFIER_PRIVATE) != 0)
			{
				CSError("cannot use both `public' and `private'");
			}
			if((modifiers & CS_MODIFIER_PROTECTED) != 0)
			{
				CSError("cannot use both `public' and `protected'");
			}
			if((modifiers & CS_MODIFIER_INTERNAL) != 0)
			{
				CSError("cannot use both `public' and `internal'");
			}
		}
		else if((modifiers & CS_MODIFIER_INTERNAL) != 0)
		{
			attrs = IL_META_TYPEDEF_NOT_PUBLIC;
			if((modifiers & CS_MODIFIER_PRIVATE) != 0)
			{
				CSError("cannot use both `internal' and `private'");
			}
			if((modifiers & CS_MODIFIER_PROTECTED) != 0)
			{
				CSError("cannot use both `internal' and `protected'");
			}
		}
		else
		{
			attrs = IL_META_TYPEDEF_NOT_PUBLIC;
			if((modifiers & CS_MODIFIER_PRIVATE) != 0)
			{
				CSError("`private' modifier is not permitted "
						"on non-nested types");
			}
			if((modifiers & CS_MODIFIER_PROTECTED) != 0)
			{
				CSError("`protected' modifier is not permitted "
						"on non-nested types");
			}
		}

		/* The "new" modifier is not allowed on top-level classes */
		if((modifiers & CS_MODIFIER_NEW) != 0)
		{
			CSError("`new' modifier is not permitted on non-nested types");
		}
	}
	else
	{
		/* Nested classes have a greater range of accessibilities */
		if((modifiers & CS_MODIFIER_PUBLIC) != 0)
		{
			attrs = IL_META_TYPEDEF_NESTED_PUBLIC;
			if((modifiers & CS_MODIFIER_PRIVATE) != 0)
			{
				CSError("cannot use both `public' and `private'");
			}
			if((modifiers & CS_MODIFIER_PROTECTED) != 0)
			{
				CSError("cannot use both `public' and `protected'");
			}
			if((modifiers & CS_MODIFIER_INTERNAL) != 0)
			{
				CSError("cannot use both `public' and `internal'");
			}
		}
		else if((modifiers & CS_MODIFIER_PRIVATE) != 0)
		{
			attrs = IL_META_TYPEDEF_NESTED_PRIVATE;
			if((modifiers & CS_MODIFIER_INTERNAL) != 0)
			{
				CSError("cannot use both `private' and `internal'");
			}
			if((modifiers & CS_MODIFIER_PROTECTED) != 0)
			{
				CSError("cannot use both `private' and `protected'");
			}
		}
		else if((modifiers & CS_MODIFIER_PROTECTED) != 0)
		{
			if((modifiers & CS_MODIFIER_INTERNAL) != 0)
			{
				attrs = IL_META_TYPEDEF_NESTED_FAM_OR_ASSEM;
			}
			else
			{
				attrs = IL_META_TYPEDEF_NESTED_FAMILY;
			}
		}
		else if((modifiers & CS_MODIFIER_INTERNAL) != 0)
		{
			attrs = IL_META_TYPEDEF_NESTED_ASSEMBLY;
		}
		else
		{
			attrs = IL_META_TYPEDEF_NESTED_PRIVATE;
		}

		/* Process the "new" modifier */
		if((modifiers & CS_MODIFIER_NEW) != 0)
		{
			attrs |= CS_SPECIALATTR_NEW;
		}
	}

	/* Process "sealed", "abstract", and "unsafe" modifiers */
	if((modifiers & CS_MODIFIER_SEALED) != 0)
	{
		attrs |= IL_META_TYPEDEF_SEALED;
	}
	if((modifiers & CS_MODIFIER_ABSTRACT) != 0)
	{
		attrs |= IL_META_TYPEDEF_ABSTRACT;
	}
	if((modifiers & CS_MODIFIER_UNSAFE) != 0)
	{
		attrs |= CS_SPECIALATTR_UNSAFE;
	}

	/* Report errors for any remaining modifiers */
	BadModifiers(modifiers & (CS_MODIFIER_STATIC | CS_MODIFIER_READONLY |
							  CS_MODIFIER_VIRTUAL | CS_MODIFIER_OVERRIDE |
							  CS_MODIFIER_EXTERN));

	/* We have the attributes we wanted now */
	return attrs;
}

/*
 * Validate access modifiers and return the access level.
 */
static ILUInt32 ValidateAccess(ILUInt32 modifiers)
{
	if((modifiers & CS_MODIFIER_PUBLIC) != 0)
	{
		if((modifiers & CS_MODIFIER_PRIVATE) != 0)
		{
			CSError("cannot use both `public' and `private'");
		}
		if((modifiers & CS_MODIFIER_PROTECTED) != 0)
		{
			CSError("cannot use both `public' and `protected'");
		}
		if((modifiers & CS_MODIFIER_INTERNAL) != 0)
		{
			CSError("cannot use both `public' and `internal'");
		}
		return IL_META_FIELDDEF_PUBLIC;
	}
	else if((modifiers & CS_MODIFIER_PRIVATE) != 0)
	{
		if((modifiers & CS_MODIFIER_INTERNAL) != 0)
		{
			CSError("cannot use both `private' and `internal'");
		}
		if((modifiers & CS_MODIFIER_PROTECTED) != 0)
		{
			CSError("cannot use both `private' and `protected'");
		}
		return IL_META_FIELDDEF_PRIVATE;
	}
	else if((modifiers & CS_MODIFIER_PROTECTED) != 0)
	{
		if((modifiers & CS_MODIFIER_INTERNAL) != 0)
		{
			return IL_META_FIELDDEF_FAM_OR_ASSEM;
		}
		else
		{
			return IL_META_FIELDDEF_FAMILY;
		}
	}
	else if((modifiers & CS_MODIFIER_INTERNAL) != 0)
	{
		return IL_META_FIELDDEF_ASSEMBLY;
	}
	else
	{
		return IL_META_FIELDDEF_PRIVATE;
	}
}

ILUInt32 CSModifiersToConstAttrs(ILUInt32 modifiers)
{
	ILUInt32 attrs;

	/* Process the common attributes */
	attrs = ValidateAccess(modifiers);

	/* Process the "new" modifier */
	if((modifiers & CS_MODIFIER_NEW) != 0)
	{
		attrs |= CS_SPECIALATTR_NEW;
	}

	/* Add the "literal" attribute */
	attrs |= IL_META_FIELDDEF_LITERAL;

	/* Report errors for the remaining modifiers */
	BadModifiers(modifiers & (CS_MODIFIER_ABSTRACT | CS_MODIFIER_SEALED |
							  CS_MODIFIER_STATIC | CS_MODIFIER_READONLY |
							  CS_MODIFIER_VIRTUAL | CS_MODIFIER_OVERRIDE |
							  CS_MODIFIER_EXTERN | CS_MODIFIER_UNSAFE));

	/* Done */
	return attrs;
}

ILUInt32 CSModifiersToFieldAttrs(ILUInt32 modifiers)
{
	ILUInt32 attrs;

	/* Process the common attributes */
	attrs = ValidateAccess(modifiers);

	/* Process the "static", "readonly", and "new" modifiers */
	if((modifiers & CS_MODIFIER_STATIC) != 0)
	{
		attrs |= IL_META_FIELDDEF_STATIC;
	}
	if((modifiers & CS_MODIFIER_READONLY) != 0)
	{
		attrs |= IL_META_FIELDDEF_INIT_ONLY;
	}
	if((modifiers & CS_MODIFIER_NEW) != 0)
	{
		attrs |= CS_SPECIALATTR_NEW;
	}
	if((modifiers & CS_MODIFIER_UNSAFE) != 0)
	{
		CSTypedWarning("unsafe", "unsafe field declaration");
		attrs |= CS_SPECIALATTR_UNSAFE;
	}

	/* Report errors for the remaining modifiers */
	BadModifiers(modifiers & (CS_MODIFIER_ABSTRACT | CS_MODIFIER_SEALED |
							  CS_MODIFIER_VIRTUAL | CS_MODIFIER_OVERRIDE |
							  CS_MODIFIER_EXTERN));

	/* Done */
	return attrs;
}

ILUInt32 CSModifiersToMethodAttrs(ILUInt32 modifiers)
{
	ILUInt32 attrs;

	/* Process the common attributes */
	attrs = ValidateAccess(modifiers);

	/* Process the other method modifiers */
	if((modifiers & CS_MODIFIER_STATIC) != 0)
	{
		attrs |= IL_META_METHODDEF_STATIC;
	}
	if((modifiers & CS_MODIFIER_ABSTRACT) != 0)
	{
		attrs |= IL_META_METHODDEF_ABSTRACT;
	}
	if((modifiers & CS_MODIFIER_SEALED) != 0)
	{
		attrs |= IL_META_METHODDEF_FINAL;
	}
	if((modifiers & CS_MODIFIER_VIRTUAL) != 0)
	{
		attrs |= IL_META_METHODDEF_VIRTUAL;
		if((modifiers & CS_MODIFIER_NEW) != 0)
		{
			attrs |= IL_META_METHODDEF_NEW_SLOT;
		}
		if((modifiers & CS_MODIFIER_OVERRIDE) != 0)
		{
			CSError("cannot use both `virtual' and `override'");
		}
	}
	else if((modifiers & CS_MODIFIER_OVERRIDE) != 0)
	{
		attrs |= IL_META_METHODDEF_VIRTUAL | CS_SPECIALATTR_OVERRIDE;
		if((modifiers & CS_MODIFIER_NEW) != 0)
		{
			CSError("cannot use both `override' and `new'");
		}
	}
	else if((modifiers & CS_MODIFIER_NEW) != 0)
	{
		attrs |= CS_SPECIALATTR_NEW;
	}
	if((modifiers & CS_MODIFIER_EXTERN) != 0)
	{
		attrs |= CS_SPECIALATTR_EXTERN;
	}
	if((modifiers & CS_MODIFIER_UNSAFE) != 0)
	{
		CSTypedWarning("unsafe", "unsafe method declaration");
		attrs |= CS_SPECIALATTR_UNSAFE;
	}

	/* Add the "hidebysig" attribute always */
	attrs |= IL_META_METHODDEF_HIDE_BY_SIG;

	/* Done */
	return attrs;
}

ILUInt32 CSModifiersToEventAttrs(ILUInt32 modifiers)
{
	ILUInt32 attrs;

	/* Process the common attributes */
	attrs = ValidateAccess(modifiers);

	/* Process the other property modifiers */
	if((modifiers & CS_MODIFIER_STATIC) != 0)
	{
		attrs |= IL_META_METHODDEF_STATIC;
	}
	if((modifiers & CS_MODIFIER_ABSTRACT) != 0)
	{
		attrs |= IL_META_METHODDEF_ABSTRACT;
	}
	if((modifiers & CS_MODIFIER_SEALED) != 0)
	{
		attrs |= IL_META_METHODDEF_FINAL;
	}
	if((modifiers & CS_MODIFIER_VIRTUAL) != 0)
	{
		attrs |= IL_META_METHODDEF_VIRTUAL;
		if((modifiers & CS_MODIFIER_NEW) != 0)
		{
			attrs |= IL_META_METHODDEF_NEW_SLOT;
		}
		if((modifiers & CS_MODIFIER_OVERRIDE) != 0)
		{
			CSError("cannot use both `virtual' and `override'");
		}
	}
	else if((modifiers & CS_MODIFIER_OVERRIDE) != 0)
	{
		attrs |= IL_META_METHODDEF_VIRTUAL | CS_SPECIALATTR_OVERRIDE;
		if((modifiers & CS_MODIFIER_NEW) != 0)
		{
			CSError("cannot use both `override' and `new'");
		}
	}
	else if((modifiers & CS_MODIFIER_NEW) != 0)
	{
		attrs |= CS_SPECIALATTR_NEW;
	}
	if((modifiers & CS_MODIFIER_UNSAFE) != 0)
	{
		CSTypedWarning("unsafe", "unsafe event declaration");
		attrs |= CS_SPECIALATTR_UNSAFE;
	}

	/* Report errors for the remaining modifiers */
	BadModifiers(modifiers & CS_MODIFIER_EXTERN);

	/* Done */
	return attrs;
}

ILUInt32 CSModifiersToPropertyAttrs(ILUInt32 modifiers)
{
	ILUInt32 attrs;

	/* Process the common attributes */
	attrs = ValidateAccess(modifiers);

	/* Process the other property modifiers */
	if((modifiers & CS_MODIFIER_STATIC) != 0)
	{
		attrs |= IL_META_METHODDEF_STATIC;
	}
	if((modifiers & CS_MODIFIER_ABSTRACT) != 0)
	{
		attrs |= IL_META_METHODDEF_ABSTRACT;
	}
	if((modifiers & CS_MODIFIER_SEALED) != 0)
	{
		attrs |= IL_META_METHODDEF_FINAL;
	}
	if((modifiers & CS_MODIFIER_VIRTUAL) != 0)
	{
		attrs |= IL_META_METHODDEF_VIRTUAL;
		if((modifiers & CS_MODIFIER_NEW) != 0)
		{
			attrs |= IL_META_METHODDEF_NEW_SLOT;
		}
		if((modifiers & CS_MODIFIER_OVERRIDE) != 0)
		{
			CSError("cannot use both `virtual' and `override'");
		}
	}
	else if((modifiers & CS_MODIFIER_OVERRIDE) != 0)
	{
		attrs |= IL_META_METHODDEF_VIRTUAL | CS_SPECIALATTR_OVERRIDE;
		if((modifiers & CS_MODIFIER_NEW) != 0)
		{
			CSError("cannot use both `override' and `new'");
		}
	}
	else if((modifiers & CS_MODIFIER_NEW) != 0)
	{
		attrs |= CS_SPECIALATTR_NEW;
	}
	if((modifiers & CS_MODIFIER_UNSAFE) != 0)
	{
		CSTypedWarning("unsafe", "unsafe property declaration");
		attrs |= CS_SPECIALATTR_UNSAFE;
	}

	/* Report errors for the remaining modifiers */
	BadModifiers(modifiers & CS_MODIFIER_EXTERN);

	/* Done */
	return attrs;
}

ILUInt32 CSModifiersToOperatorAttrs(ILUInt32 modifiers)
{
	ILUInt32 attrs = 0;
	if((modifiers & CS_MODIFIER_PUBLIC) == 0)
	{
		CSError("operators must have `public' access");
	}
	if((modifiers & CS_MODIFIER_STATIC) == 0)
	{
		CSError("operators must have `static' access");
	}
	if((modifiers & CS_MODIFIER_UNSAFE) != 0)
	{
		CSTypedWarning("unsafe", "unsafe operator declaration");
		attrs |= CS_SPECIALATTR_UNSAFE;
	}
	BadModifiers(modifiers & ~(CS_MODIFIER_PUBLIC | CS_MODIFIER_STATIC |
							   CS_MODIFIER_UNSAFE));
	return (attrs | IL_META_METHODDEF_PUBLIC | IL_META_METHODDEF_STATIC |
			IL_META_METHODDEF_SPECIAL_NAME | IL_META_METHODDEF_HIDE_BY_SIG);
}

ILUInt32 CSModifiersToConstructorAttrs(ILUInt32 modifiers)
{
	ILUInt32 attrs;

	/* Different flags are used for instance and static constructors */
	if((modifiers & CS_MODIFIER_STATIC) == 0)
	{
		attrs = ValidateAccess(modifiers);
		if((modifiers & CS_MODIFIER_EXTERN) != 0)
		{
			attrs |= CS_SPECIALATTR_EXTERN;
		}
		BadModifiers(modifiers & ~(CS_MODIFIER_PUBLIC |
								   CS_MODIFIER_PRIVATE |
								   CS_MODIFIER_PROTECTED |
								   CS_MODIFIER_INTERNAL |
								   CS_MODIFIER_EXTERN |
								   CS_MODIFIER_UNSAFE));
	}
	else
	{
		BadModifiers(modifiers & ~(CS_MODIFIER_STATIC | CS_MODIFIER_UNSAFE));
		attrs = IL_META_METHODDEF_PUBLIC | IL_META_METHODDEF_STATIC;
	}

	/* Process the "unsafe" modifier */
	if((modifiers & CS_MODIFIER_UNSAFE) != 0)
	{
		CSTypedWarning("unsafe", "unsafe constructor declaration");
		attrs |= CS_SPECIALATTR_UNSAFE;
	}

	/* Add the "hidebysig" and "*specialname" attributes always */
	attrs |= IL_META_METHODDEF_HIDE_BY_SIG |
			 IL_META_METHODDEF_SPECIAL_NAME |
			 IL_META_METHODDEF_RT_SPECIAL_NAME;

	/* Done */
	return attrs;
}

#ifdef	__cplusplus
};
#endif
