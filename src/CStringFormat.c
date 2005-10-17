/*
 * SDStringFormat.c - String format implementation.
 *
 * Copyright (C) 2005  Free Software Foundation, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "CStringFormat.h"

#ifdef __cplusplus
extern "C" {
#endif

static void
SDStringFormat_Initialize(SDStringFormat     *_this,
                          SDStringFormatFlag  flags,
                          SDLanguageID        language)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the members */
	_this->alignment           = SDStringAlignment_Near;
	_this->lineAlignment       = SDStringAlignment_Near;
	_this->formatFlags         = flags;
	_this->hotkeyPrefix        = SDHotkeyPrefix_None;
	_this->trimming            = SDStringTrimming_None;
	_this->method              = SDDigitSubstitute_None;
	_this->language            = language;
	_this->firstTabOffset      = 0;
	_this->tabStops            = NULL;
	_this->tabStopCount        = 0;
	_this->characterRanges     = NULL;
	_this->characterRangeCount = 0;
}

static void
SDStringFormat_Finalize(SDStringFormat *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the members */
	{
		/* finalize the tab stop list, as needed */
		if(_this->tabStops != 0)
		{
			SDFree(_this->tabStops);
			_this->tabStopCount = 0;
			_this->tabStops     = 0;
		}

		/* finalize the character range list, as needed */
		if(_this->characterRanges != 0)
		{
			SDFree(_this->characterRanges);
			_this->characterRangeCount = 0;
			_this->characterRanges     = 0;
		}
	}
}

/* Create a string format. */
SDStatus
SDStringFormat_Create(SDStringFormat     **_this,
                      SDStringFormatFlag   flags,
                      SDLanguageID         language)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the string format */
	if(!(*_this = (SDStringFormat *)SDMalloc(sizeof(SDStringFormat))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the string format */
	SDStringFormat_Initialize(*_this, flags, language);

	/* return successfully */
	return SDStatus_OK;
}

/* Destroy a string format. */
SDStatus
SDStringFormat_Destroy(SDStringFormat **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require(((*_this) != 0), SDStatus_ArgumentNull);

	/* finalize the string format */
	SDStringFormat_Finalize(*_this);

	/* free the string format */
	SDFree(*_this);

	/* null the this pointer */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the generic default string format. */
SDStatus
SDStringFormat_CreateGenericDefault(SDStringFormat **_this)
{
	/* create the format */
	SDStatus_Check
		(SDStringFormat_Create
			(_this, 0, 0));

	/* initialize the members */
	(*_this)->trimming = SDStringTrimming_Character;
	(*_this)->method   = SDDigitSubstitute_User;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the generic typographic string format. */
SDStatus
SDStringFormat_CreateGenericTypographic(SDStringFormat **_this)
{
	/* get the flags */
	const SDStringFormatFlag flags =
		(SDStringFormatFlag_NoFitBlackBox |
		 SDStringFormatFlag_LineLimit     |
		 SDStringFormatFlag_NoClip);

	/* create the format */
	SDStatus_Check
		(SDStringFormat_Create
			(_this, flags, 0));

	/* initialize the members */
	(*_this)->method = SDDigitSubstitute_User;

	/* return successfully */
	return SDStatus_OK;
}

/* Clone this string format. */
SDStatus
SDStringFormat_Clone(SDStringFormat  *_this,
                     SDStringFormat **clone)
{
	/* declarations */
	SDStatus status;

	/* create the format */
	SDStatus_Check
		(SDStringFormat_Create
			(clone, _this->formatFlags, _this->language));

	/* set the clone tab stops */
	status =
		SDStringFormat_SetTabStops
			(*clone, _this->firstTabOffset, _this->tabStops,
			 _this->tabStopCount);

	/* handle tab stop cloning failures */
	if(status != SDStatus_OK)
	{
		SDStringFormat_Destroy(clone);
		return status;
	}

	/* set the clone character ranges */
	status =
		SDStringFormat_SetCharacterRanges
			(*clone, _this->characterRanges, _this->characterRangeCount);

	/* handle character range cloning failures */
	if(status != SDStatus_OK)
	{
		SDStringFormat_Destroy(clone);
		return status;
	}

	/* clone remaining members */
	(*clone)->alignment     = _this->alignment;
	(*clone)->lineAlignment = _this->lineAlignment;
	(*clone)->hotkeyPrefix  = _this->hotkeyPrefix;
	(*clone)->trimming      = _this->trimming;
	(*clone)->method        = _this->method;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the alignment of this string format. */
SDStatus
SDStringFormat_GetAlignment(SDStringFormat    *_this,
                            SDStringAlignment *alignment)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an alignment pointer */
	SDStatus_Require((alignment != 0), SDStatus_ArgumentNull);

	/* get the alignment */
	*alignment = _this->alignment;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the alignment of this string format. */
SDStatus
SDStringFormat_SetAlignment(SDStringFormat    *_this,
                            SDStringAlignment  alignment)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the alignment */
	_this->alignment = alignment;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the character ranges of this string format. */
SDStatus
SDStringFormat_GetCharacterRanges(SDStringFormat    *_this,
                                  SDCharacterRange **characterRanges,
                                  SDUInt32          *count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a character range list pointer */
	SDStatus_Require((characterRanges != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get the character ranges */
	{
		/* get the count and size */
		const SDUInt32 cnt  = _this->characterRangeCount;
		const SDUInt32 size = (cnt * sizeof(SDCharacterRange));

		/* allocate the character range list */
		if(!(*characterRanges = (SDCharacterRange *)SDMalloc(size)))
		{
			*count = 0;
			return SDStatus_OutOfMemory;
		}

		/* get the character ranges */
		SDMemCopy(*characterRanges, _this->characterRanges, size);

		/* get the count */
		*count = cnt;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Set the character ranges of this string format. */
SDStatus
SDStringFormat_SetCharacterRanges(SDStringFormat   *_this,
                                  SDCharacterRange *characterRanges,
                                  SDUInt32          count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a character range list */
	SDStatus_Require((characterRanges != 0), SDStatus_ArgumentNull);

	/* set the character ranges */
	{
		/* get the size */
		const SDUInt32 size = (count * sizeof(SDCharacterRange));

		/* clear the character range list, as needed */
		if(count == 0)
		{
			/* free the existing character range list, as needed */
			if(_this->characterRanges != 0)
			{
				SDFree(_this->characterRanges);
			}

			/* reset the count */
			_this->characterRangeCount = 0;

			/* return successfully */
			return SDStatus_OK;
		}

		/* reallocate the character range list, as needed */
		if(count != _this->characterRangeCount)
		{
			/* declarations */
			SDCharacterRange *tmp;

			/* allocate the character range list */
			if(!(tmp = (SDCharacterRange *)SDMalloc(size)))
			{
				return SDStatus_OutOfMemory;
			}

			/* free the existing character range list, as needed */
			if(_this->characterRanges != 0)
			{
				SDFree(_this->characterRanges);
			}

			/* set the character range list */
			_this->characterRanges = tmp;

			/* set the count */
			_this->characterRangeCount = count;
		}

		/* copy the character ranges */
		SDMemCopy(_this->characterRanges, characterRanges, size);
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the digit substitution settings of this string format. */
SDStatus
SDStringFormat_GetDigitSubstitution(SDStringFormat    *_this,
                                    SDDigitSubstitute *method,
                                    SDLanguageID      *language)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a method pointer */
	SDStatus_Require((method != 0), SDStatus_ArgumentNull);

	/* ensure we have a language pointer */
	SDStatus_Require((language != 0), SDStatus_ArgumentNull);

	/* get the method */
	*method = _this->method;

	/* get the language */
	*language = _this->language;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the digit substitution settings of this string format. */
SDStatus
SDStringFormat_SetDigitSubstitution(SDStringFormat    *_this,
                                    SDDigitSubstitute  method,
                                    SDLanguageID       language)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the method */
	_this->method = method;

	/* set the language */
	_this->language = language;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the format flags of this string format. */
SDStatus
SDStringFormat_GetFormatFlags(SDStringFormat     *_this,
                              SDStringFormatFlag *formatFlags)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a format flags pointer */
	SDStatus_Require((formatFlags != 0), SDStatus_ArgumentNull);

	/* get the format flags */
	*formatFlags = _this->formatFlags;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the format flags of this string format. */
SDStatus
SDStringFormat_SetFormatFlags(SDStringFormat     *_this,
                              SDStringFormatFlag  formatFlags)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the format flags */
	_this->formatFlags = formatFlags;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the hotkey prefix mode of this string format. */
SDStatus
SDStringFormat_GetHotkeyPrefix(SDStringFormat *_this,
                               SDHotkeyPrefix *hotkeyPrefix)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a hotkey prefix mode pointer */
	SDStatus_Require((hotkeyPrefix != 0), SDStatus_ArgumentNull);

	/* get the hotkey prefix mode */
	*hotkeyPrefix = _this->hotkeyPrefix;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the hotkey prefix mode of this string format. */
SDStatus
SDStringFormat_SetHotkeyPrefix(SDStringFormat *_this,
                               SDHotkeyPrefix  hotkeyPrefix)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the hotkey prefix mode */
	_this->hotkeyPrefix = hotkeyPrefix;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the line alignment of this string format. */
SDStatus
SDStringFormat_GetLineAlignment(SDStringFormat    *_this,
                                SDStringAlignment *lineAlignment)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a line alignment pointer */
	SDStatus_Require((lineAlignment != 0), SDStatus_ArgumentNull);

	/* get the line alignment */
	*lineAlignment = _this->lineAlignment;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the line alignment of this string format. */
SDStatus
SDStringFormat_SetLineAlignment(SDStringFormat    *_this,
                                SDStringAlignment  lineAlignment)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the line alignment */
	_this->lineAlignment = lineAlignment;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the tab stop settings of this string format. */
SDStatus
SDStringFormat_GetTabStops(SDStringFormat  *_this,
                           SDFloat         *firstTabOffset,
                           SDFloat        **tabStops,
                           SDUInt32        *count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a first tab offset pointer */
	SDStatus_Require((firstTabOffset != 0), SDStatus_ArgumentNull);

	/* ensure we have a tab stop list pointer */
	SDStatus_Require((tabStops != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get the tab stops */
	{
		/* get the count and size */
		const SDUInt32 cnt  = _this->tabStopCount;
		const SDUInt32 size = (cnt * sizeof(SDFloat));

		/* allocate the tab stop list */
		if(!(*tabStops = (SDFloat *)SDMalloc(size)))
		{
			*count = 0;
			return SDStatus_OutOfMemory;
		}

		/* get the tab stops */
		SDMemCopy(*tabStops, _this->tabStops, size);

		/* get the count */
		*count = cnt;
	}

	/* get the first tab offset */
	*firstTabOffset = _this->firstTabOffset;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the tab stop settings of this string format. */
SDStatus
SDStringFormat_SetTabStops(SDStringFormat *_this,
                           SDFloat         firstTabOffset,
                           SDFloat        *tabStops,
                           SDUInt32        count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a tab stop list */
	SDStatus_Require((tabStops != 0), SDStatus_ArgumentNull);

	/* set the tab stops */
	{
		/* get the size */
		const SDUInt32 size = (count * sizeof(SDFloat));

		/* clear the tab stop list, as needed */
		if(count == 0)
		{
			/* free the existing tab stop list, as needed */
			if(_this->tabStops != 0)
			{
				SDFree(_this->tabStops);
			}

			/* reset the count */
			_this->tabStopCount = 0;

			/* return successfully */
			return SDStatus_OK;
		}

		/* reallocate the tab stop list, as needed */
		if(count != _this->tabStopCount)
		{
			/* declarations */
			SDFloat *tmp;

			/* allocate the tab stop list */
			if(!(tmp = (SDFloat *)SDMalloc(size)))
			{
				return SDStatus_OutOfMemory;
			}

			/* free the existing tab stop list, as needed */
			if(_this->tabStops != 0)
			{
				SDFree(_this->tabStops);
			}

			/* set the tab stop list */
			_this->tabStops = tmp;

			/* set the count */
			_this->tabStopCount = count;
		}

		/* copy the tab stops */
		SDMemCopy(_this->tabStops, tabStops, size);
	}

	/* set the first tab offset */
	_this->firstTabOffset = firstTabOffset;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the trimming mode of this string format. */
SDStatus
SDStringFormat_GetTrimming(SDStringFormat   *_this,
                           SDStringTrimming *trimming)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a trimming mode pointer */
	SDStatus_Require((trimming != 0), SDStatus_ArgumentNull);

	/* get the trimming mode */
	*trimming = _this->trimming;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the trimming mode of this string format. */
SDStatus
SDStringFormat_SetTrimming(SDStringFormat   *_this,
                           SDStringTrimming  trimming)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the trimming mode */
	_this->trimming = trimming;

	/* return successfully */
	return SDStatus_OK;
}

#ifdef __cplusplus
};
#endif
