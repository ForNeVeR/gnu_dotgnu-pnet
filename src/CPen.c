/*
 * SDPen.c - Pen implementation.
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

#include "CPen.h"
#include "CBrush.h"
#include "CMatrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDDashStyle_IsValid(ds) \
	(((ds) >= SDDashStyle_Solid) && ((ds) <= SDDashStyle_Custom))
#define SDDashInfo_MAXCOUNT 6
typedef struct _tagSDDashInfo SDDashInfo;
struct _tagSDDashInfo
{
	const SDUInt32 count;
	const SDFloat  pattern[SDDashInfo_MAXCOUNT];
};
static const SDDashInfo SDPen_DashInfo[] =
{
	{ 0, { 0.0 }                          }, /* SDDashStyle_Solid       */
	{ 2, { 1.0, 1.0 }                     }, /* SDDashStyle_Dot         */
	{ 2, { 3.0, 1.0 }                     }, /* SDDashStyle_Dash        */
	{ 4, { 3.0, 1.0, 1.0, 1.0 }           }, /* SDDashStyle_DashDot     */
	{ 6, { 3.0, 1.0, 1.0, 1.0, 1.0, 1.0 } }, /* SDDashStyle_DashDashDot */
	{ 1, { 1.0 }                          }  /* SDDashStyle_Custom      */
};


static SDStatus
SDPen_Initialize(SDPen   *_this,
                 SDBrush *brush,
                 SDFloat  width)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((brush != 0));

	/* initialize the members */
	_this->dashOffset     = 0.0f;
	_this->miterLimit     = 10.0f;
	_this->width          = width;
	_this->compoundArray  = 0;
	_this->compoundCount  = 0;
	_this->dashPattern    = 0;
	_this->dashCount      = 0;
	_this->transform      = SDAffineTransformF_Identity;
	_this->dashCap        = SDDashCap_Flat;
	_this->dashStyle      = SDDashStyle_Solid;
	_this->customEndCap   = 0;
	_this->customStartCap = 0;
	_this->endCap         = SDLineCap_Flat;
	_this->startCap       = SDLineCap_Flat;
	_this->lineJoin       = SDLineJoin_Miter;
	_this->alignment      = SDPenAlignment_Center;

	/* initialize the brush and return */
	return SDBrush_Clone(brush, &(_this->brush));
}

static void
SDPen_Finalize(SDPen *_this)
{
	SDASSERT((_this != 0));

	/* finalize, as needed */
	if(_this->brush != 0)
	{
		/* dispose of the brush */
		SDBrush_Destroy(&(_this->brush));

		/* dispose of the compound array, as needed */
		if(_this->compoundArray != 0)
		{
			SDFree(_this->compoundArray);
			_this->compoundArray = 0;
			_this->compoundCount = 0;
		}

		/* dispose of the dash pattern, as needed */
		if(_this->dashPattern != 0)
		{
			SDFree(_this->dashPattern);
			_this->dashPattern = 0;
			_this->dashCount = 0;
		}

		/* dispose of the custom end cap, as needed */
		if(_this->customEndCap != 0)
		{
			SDFree(_this->customEndCap);
			_this->customEndCap = 0;
		}

		/* dispose of the custom start cap, as needed */
		if(_this->customStartCap != 0)
		{
			SDFree(_this->customStartCap);
			_this->customStartCap = 0;
		}
	}
}

/* Create a pen. */
SDStatus
SDPen_Create(SDPen   **_this,
             SDBrush  *brush,
             SDFloat   width)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* allocate the pen */
	if(!(*_this = (SDPen *)SDMalloc(sizeof(SDPen))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the pen */
	status = SDPen_Initialize(*_this, brush, width);

	/* handle initialization failures */
	if(status != SDStatus_OK) { *_this = 0; }

	/* return status */
	return status;
}

/* Destroy a pen. */
SDStatus
SDPen_Destroy(SDPen **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require(((*_this) != 0), SDStatus_ArgumentNull);

	/* finalize the pen */
	SDPen_Finalize(*_this);

	/* null the this pointer */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the alignment of this pen. */
SDStatus
SDPen_GetAlignment(SDPen          *_this,
                   SDPenAlignment *alignment)
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

/* Set the alignment of this pen. */
SDStatus
SDPen_SetAlignment(SDPen          *_this,
                   SDPenAlignment  alignment)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the alignment */
	_this->alignment = alignment;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the brush of this pen. */
SDStatus
SDPen_GetBrush(SDPen    *_this,
               SDBrush **brush)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush pointer pointer */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* get the brush */
	return SDBrush_Clone(_this->brush, brush);
}

/* Set the brush of this pen. */
SDStatus
SDPen_SetBrush(SDPen   *_this,
               SDBrush *brush)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush pointer */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* set the brush */
	{
		/* declarations */
		SDBrush *b;

		/* clone the given brush */
		SDStatus_Check
			(SDBrush_Clone
				(brush, &b));

		/* dispose of the current brush */
		SDBrush_Destroy(&(_this->brush));

		/* set the brush */
		_this->brush = b;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Set the line and dash caps of this pen. */
SDStatus
SDPen_SetCaps(SDPen     *_this,
              SDLineCap  startCap,
              SDLineCap  endCap,
              SDDashCap  dashCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the line and dash caps */
	_this->startCap = startCap;
	_this->endCap   = endCap;
	_this->dashCap  = dashCap;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the color of this pen. */
SDStatus
SDPen_GetColor(SDPen   *_this,
               SDColor *color)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a color pointer */
	SDStatus_Require((color != 0), SDStatus_ArgumentNull);

	/* get the color */
	{
		/* declarations */
		SDBrushType bt;

		/* get the brush type */
		SDStatus_Check
			(SDBrush_GetBrushType
				(_this->brush, &bt));

		/* get the color, if available */
		if(bt == SDBrushType_SolidFill)
		{
			return SDSolidBrush_GetColor(((SDSolidBrush *)_this->brush), color);
		}
		else
		{
			*color = SDColor_Empty;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the compound array of this pen. */
SDStatus
SDPen_GetCompoundArray(SDPen     *_this,
                       SDFloat  **compoundArray,
                       SDUInt32  *count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a compound array pointer pointer */
	SDStatus_Require((compoundArray != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get the compound array */
	{
		/* get the compound array, if available */
		if(_this->compoundCount == 0)
		{
			/* get the defaults */
			*compoundArray = 0;
			*count         = 0;
		}
		else
		{
			/* declarations */
			SDUInt32 size;

			/* get the count */
			*count = _this->compoundCount;

			/* get the size */
			size = (*count * sizeof(SDFloat));

			/* allocate the compound array */
			*compoundArray = (SDFloat *)SDMalloc(size);

			/* ensure we have a compound array */
			SDStatus_Require
				((*compoundArray != 0), SDStatus_OutOfMemory);

			/* get the compound array */
			SDMemCopy(*compoundArray, _this->compoundArray, size);
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Set the compound array of this pen. */
SDStatus
SDPen_SetCompoundArray(SDPen         *_this,
                       const SDFloat *compoundArray,
                       SDUInt32       count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the compound array */
	{
		/* set the compound array, if available */
		if(compoundArray == 0 || count == 0)
		{
			/* dispose of the current compound array, as needed */
			if(_this->compoundCount != 0)
			{
				/* dispose of the current compound array */
				SDFree(_this->compoundArray);
			}

			/* null the compound array */
			_this->compoundArray = 0;

			/* set the compound array count */
			_this->compoundCount = 0;
		}
		else
		{
			/* get the size */
			const SDUInt32 size = (count * sizeof(SDFloat));

			/* validate the compound array */
			{
				/* declarations */
				const SDFloat *curr;
				const SDFloat *last;
				SDFloat        prev;

				/* ensure there are at least two entries */
				SDStatus_Require((count >= 2), SDStatus_Argument);

				/* get the current pointer */
				curr = compoundArray;

				/* get the last pointer */
				last = (curr + (count - 1));

				/* get the first entry */
				prev = *curr++;

				/* ensure the first entry is zero */
				SDStatus_Require((prev == 0.0f), SDStatus_Argument);

				/* ensure the last entry is one */
				SDStatus_Require((*last == 1.0f), SDStatus_Argument);

				/* ensure the entries increase in value */
				while(curr != last)
				{
					/* ensure the current entry is greater than the previous */
					SDStatus_Require((*curr > prev), SDStatus_Argument);

					/* move to the next position */
					prev = *curr++;
				}

				/* ensure the penultimate entry is less than one */
				SDStatus_Require((prev < 1.0f), SDStatus_Argument);
			}

			/* allocate the compound array, as needed */
			if(_this->compoundCount != count)
			{
				/* declarations */
				SDFloat *tmp;

				/* allocate the compound array */
				tmp = (SDFloat *)SDMalloc(size);

				/* ensure we have a compound array */
				SDStatus_Require((tmp != 0), SDStatus_OutOfMemory);

				/* dispose of the current compound array, as needed */
				if(_this->compoundCount != 0)
				{
					/* dispose of the current compound array */
					SDFree(_this->compoundArray);
				}

				/* set the compound array */
				_this->compoundArray = tmp;

				/* set the compound array count */
				_this->compoundCount = count;
			}

			/* copy the compound array data */
			SDMemCopy(_this->compoundArray, compoundArray, size);
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the custom end cap of this pen. */
SDStatus
SDPen_GetCustomEndCap(SDPen            *_this,
                      SDCustomLineCap **customEndCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a custom end cap pointer pointer */
	SDStatus_Require((customEndCap != 0), SDStatus_ArgumentNull);

	/* get the custom end cap */
#if 0
	if(_this->customEndCap == 0)
	{
		*customEndCap = 0;
	}
	else
	{
		return SDCustomLineCap_Clone(_this->customEndCap, customEndCap);
	}
#else
	*customEndCap = 0;
#endif

	/* return successfully */
	return SDStatus_OK;
}

/* Set the custom end cap of this pen. */
SDStatus
SDPen_SetCustomEndCap(SDPen           *_this,
                      SDCustomLineCap *customEndCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the custom end cap */
#if 0
	{
		/* dispose of the current custom end cap, as needed */
		if(_this->customEndCap != 0)
		{
			/* dispose of the current custom end cap */
			SDCustomLineCap_Destroy(&(_this->customEndCap));
		}

		/* set the custom end cap */
		if(customEndCap == 0)
		{
			_this->customEndCap = 0;
		}
		else
		{
			return
				SDCustomLineCap_Clone
					(customEndCap, &(_this->customEndCap));
		}
	}
#endif

	/* return successfully */
	return SDStatus_OK;
}

/* Get the custom start cap of this pen. */
SDStatus
SDPen_GetCustomStartCap(SDPen            *_this,
                        SDCustomLineCap **customStartCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a custom start cap pointer pointer */
	SDStatus_Require((customStartCap != 0), SDStatus_ArgumentNull);

	/* get the custom start cap */
#if 0
	if(_this->customStartCap == 0)
	{
		*customStartCap = 0;
	}
	else
	{
		return SDCustomLineCap_Clone(_this->customStartCap, customStartCap);
	}
#else
	*customStartCap = 0;
#endif

	/* return successfully */
	return SDStatus_OK;
}

/* Set the custom start cap of this pen. */
SDStatus
SDPen_SetCustomStartCap(SDPen           *_this,
                        SDCustomLineCap *customStartCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the custom start cap */
#if 0
	{
		/* dispose of the current custom start cap, as needed */
		if(_this->customStartCap != 0)
		{
			/* dispose of the current custom start cap */
			SDCustomLineCap_Destroy(&(_this->customStartCap));
		}

		/* set the custom start cap */
		if(customStartCap == 0)
		{
			_this->customStartCap = 0;
		}
		else
		{
			return
				SDCustomLineCap_Clone
					(customStartCap, &(_this->customStartCap)));
		}
	}
#endif

	/* return successfully */
	return SDStatus_OK;
}

/* Get the dash cap of this pen. */
SDStatus
SDPen_GetDashCap(SDPen     *_this,
                 SDDashCap *dashCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a dash cap pointer */
	SDStatus_Require((dashCap != 0), SDStatus_ArgumentNull);

	/* get the dash cap */
	*dashCap = _this->dashCap;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the dash cap of this pen. */
SDStatus
SDPen_SetDashCap(SDPen     *_this,
                 SDDashCap  dashCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the dash cap */
	_this->dashCap = dashCap;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the dash offset of this pen. */
SDStatus
SDPen_GetDashOffset(SDPen   *_this,
                    SDFloat *dashOffset)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a dash offset pointer */
	SDStatus_Require((dashOffset != 0), SDStatus_ArgumentNull);

	/* get the dash offset */
	*dashOffset = _this->dashOffset;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the dash offset of this pen. */
SDStatus
SDPen_SetDashOffset(SDPen   *_this,
                    SDFloat  dashOffset)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the dash offset */
	_this->dashOffset = dashOffset;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the dash pattern of this pen. */
SDStatus
SDPen_GetDashPattern(SDPen     *_this,
                     SDFloat  **dashPattern,
                     SDUInt32  *count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a dash pattern pointer pointer */
	SDStatus_Require((dashPattern != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get the dash pattern */
	{
		/* get the dash pattern, if available */
		if(_this->dashCount == 0)
		{
			/* get the defaults */
			*dashPattern = 0;
			*count       = 0;
		}
		else
		{
			/* declarations */
			SDUInt32 size;

			/* get the count */
			*count = _this->dashCount;

			/* get the size */
			size = (*count * sizeof(SDFloat));

			/* allocate the dash pattern */
			*dashPattern = (SDFloat *)SDMalloc(size);

			/* ensure we have a dash pattern */
			SDStatus_Require((*dashPattern != 0), SDStatus_OutOfMemory);

			/* get the dash pattern */
			SDMemCopy(*dashPattern, _this->dashPattern, size);
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Set the dash pattern of this pen. */
SDStatus
SDPen_SetDashPattern(SDPen         *_this,
                     const SDFloat *dashPattern,
                     SDUInt32       count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the dash pattern */
	{
		/* set the dash pattern, if available */
		if(dashPattern == 0 || count == 0)
		{
			/* dispose of the current dash pattern, as needed */
			if(_this->dashCount != 0)
			{
				/* dispose of the current dash pattern */
				SDFree(_this->dashPattern);
			}

			/* null the dash pattern */
			_this->dashPattern = 0;

			/* set the dash pattern count */
			_this->dashCount = 0;

			/* set the dash style */
			_this->dashStyle = SDDashStyle_Solid;
		}
		else
		{
			/* declarations */
			SDFloat *tmp;

			/* allocate the dash pattern */
			tmp = (SDFloat *)SDMalloc(count * sizeof(SDFloat));

			/* ensure we have a dash pattern */
			SDStatus_Require((tmp != 0), SDStatus_OutOfMemory);

			/* dispose of the current dash pattern, as needed */
			if(_this->dashCount != 0)
			{
				/* dispose of the current dash pattern */
				SDFree(_this->dashPattern);
			}

			/* copy the dash pattern data */
			SDMemCopy(tmp, dashPattern, (count * sizeof(SDFloat)));

			/* set the dash pattern */
			_this->dashPattern = tmp;

			/* set the dash pattern count */
			_this->dashCount = count;

			/* set the dash style */
			_this->dashStyle = SDDashStyle_Custom;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the dash style of this pen. */
SDStatus
SDPen_GetDashStyle(SDPen       *_this,
                   SDDashStyle *dashStyle)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a dash style pointer */
	SDStatus_Require((dashStyle != 0), SDStatus_ArgumentNull);

	/* get the dash style */
	*dashStyle = _this->dashStyle;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the dash style of this pen. */
SDStatus
SDPen_SetDashStyle(SDPen       *_this,
                   SDDashStyle  dashStyle)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure the dash style is in range */
	SDStatus_Require(SDDashStyle_IsValid(dashStyle), SDStatus_Argument);

	/* set the dash style */
	{
		/* get the dash information for the given style */
		const SDDashInfo *info = &(SDPen_DashInfo[dashStyle]);

		/* set the dash pattern */
		SDStatus_Check
			(SDPen_SetDashPattern
				(_this, info->pattern, info->count));

		/* set the dash style */
		_this->dashStyle = dashStyle;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the end cap of this pen. */
SDStatus
SDPen_GetEndCap(SDPen     *_this,
                SDLineCap *endCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a end cap pointer */
	SDStatus_Require((endCap != 0), SDStatus_ArgumentNull);

	/* get the end cap */
	*endCap = _this->endCap;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the end cap of this pen. */
SDStatus
SDPen_SetEndCap(SDPen     *_this,
                SDLineCap  endCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the end cap */
	_this->endCap = endCap;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the line join of this pen. */
SDStatus
SDPen_GetLineJoin(SDPen      *_this,
                  SDLineJoin *lineJoin)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a line join pointer */
	SDStatus_Require((lineJoin != 0), SDStatus_ArgumentNull);

	/* get the line join */
	*lineJoin = _this->lineJoin;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the line join of this pen. */
SDStatus
SDPen_SetLineJoin(SDPen      *_this,
                  SDLineJoin  lineJoin)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the line join */
	_this->lineJoin = lineJoin;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the miter limit of this pen. */
SDStatus
SDPen_GetMiterLimit(SDPen   *_this,
                    SDFloat *miterLimit)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a miter limit pointer */
	SDStatus_Require((miterLimit != 0), SDStatus_ArgumentNull);

	/* get the miter limit */
	*miterLimit = _this->miterLimit;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the miter limit of this pen. */
SDStatus
SDPen_SetMiterLimit(SDPen   *_this,
                    SDFloat  miterLimit)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the miter limit */
	_this->miterLimit = miterLimit;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the type of this pen. */
SDStatus
SDPen_GetPenType(SDPen     *_this,
                 SDPenType *type)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a type pointer */
	SDStatus_Require((type != 0), SDStatus_ArgumentNull);

	/* get the type */
	{
		/* declarations */
		SDBrushType bt;

		/* get the type */
		SDStatus_Check
			(SDBrush_GetBrushType
				(_this->brush, &bt));

		/* set the type pointer to the type */
		*type = (SDPenType)bt;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the start cap of this pen. */
SDStatus
SDPen_GetStartCap(SDPen     *_this,
                  SDLineCap *startCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a start cap pointer */
	SDStatus_Require((startCap != 0), SDStatus_ArgumentNull);

	/* get the start cap */
	*startCap = _this->startCap;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the start cap of this pen. */
SDStatus
SDPen_SetStartCap(SDPen     *_this,
                  SDLineCap  startCap)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the start cap */
	_this->startCap = startCap;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the width of this pen. */
SDStatus
SDPen_GetWidth(SDPen   *_this,
               SDFloat *width)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a width pointer */
	SDStatus_Require((width != 0), SDStatus_ArgumentNull);

	/* get the width */
	*width = _this->width;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the width of this pen. */
SDStatus
SDPen_SetWidth(SDPen   *_this,
               SDFloat  width)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the width */
	_this->width = width;

	/* return successfully */
	return SDStatus_OK;
}

/* Clone this pen. */
SDStatus
SDPen_Clone(SDPen  *_this,
            SDPen **clone)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a clone pointer pointer */
	SDStatus_Require((clone != 0), SDStatus_ArgumentNull);

	/* create the clone */
	{
		/* get the counts */
		const SDUInt32 cc = _this->compoundCount;
		const SDUInt32 dc = _this->dashCount;

		/* create the clone pen */
		SDStatus_Check
			(SDPen_Create
				(clone, _this->brush, _this->width));

		/* deep copy the compound array, as needed */
		if(cc != 0)
		{
			/* get the size */
			const SDUInt32 size = (cc * sizeof(SDFloat));

			/* allocate the compound array */
			(*clone)->compoundArray = (SDFloat *)SDMalloc(size);

			/* ensure we have a compound array */
			if((*clone)->compoundArray == 0)
			{
				SDPen_Destroy(clone);
				return SDStatus_OutOfMemory;
			}

			/* deep copy the compound array */
			SDMemCopy((*clone)->compoundArray, _this->compoundArray, size);

			/* set the compound array count */
			(*clone)->compoundCount = cc;
		}

		/* deep copy the dash pattern, as needed */
		if(dc != 0)
		{
			/* get the size */
			const SDUInt32 size = (dc * sizeof(SDFloat));

			/* allocate the dash pattern */
			(*clone)->dashPattern = (SDFloat *)SDMalloc(size);

			/* ensure we have a dash pattern */
			if((*clone)->dashPattern == 0)
			{
				SDPen_Destroy(clone);
				return SDStatus_OutOfMemory;
			}

			/* deep copy the dash pattern */
			SDMemCopy((*clone)->dashPattern, _this->dashPattern, size);

			/* set the dash pattern count */
			(*clone)->dashCount = dc;
		}

		/* deep copy the custom end cap, as needed */
#if 0
		if(_this->customEndCap != 0)
		{
			/* declarations */
			Status status;

			/* deep copy the custom end cap */
			status = SDCustomLineCap_Clone
				(_this->customEndCap, &((*clone)->customEndCap));

			/* ensure we have a custom end cap */
			if(status != SDStatus_OK)
			{
				SDPen_Destroy(clone);
				return status;
			}
		}
#endif

		/* deep copy the custom start cap, as needed */
#if 0
		if(_this->customStartCap != 0)
		{
			/* declarations */
			Status status;

			/* deep copy the custom start cap */
			status = SDCustomLineCap_Clone
				(_this->customStartCap, &((*clone)->customStartCap));

			/* ensure we have a custom start cap */
			if(status != SDStatus_OK)
			{
				SDPen_Destroy(clone);
				return status;
			}
		}
#endif

		/* set the remaining clone members */
		(*clone)->dashOffset     = _this->dashOffset;
		(*clone)->miterLimit     = _this->miterLimit;
		(*clone)->transform      = _this->transform;
		(*clone)->dashCap        = _this->dashCap;
		(*clone)->dashStyle      = _this->dashStyle;
		(*clone)->endCap         = _this->endCap;
		(*clone)->startCap       = _this->startCap;
		(*clone)->lineJoin       = _this->lineJoin;
		(*clone)->alignment      = _this->alignment;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the transformation matrix of this pen. */
SDStatus
SDPen_GetTransform(SDPen    *_this,
                   SDMatrix *matrix)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* get the transformation */
	return SDMatrix_SetTransform(matrix, &(_this->transform));
}

/* Multiply the transformation matrix of this pen by another matrix. */
SDStatus
SDPen_MultiplyTransform(SDPen         *_this,
                        SDMatrix      *matrix,
                        SDMatrixOrder  order)
{
	/* declarations */
	SDAffineTransformF t;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* get the matrix transformation */
	SDStatus_Check
		(SDMatrix_GetTransform
			(matrix, &t));

	/* multiply the transformation */
	SDAffineTransformF_Multiply(&(_this->transform), &t, order);

	/* return successfully */
	return SDStatus_OK;
}

/* Reset the transformation matrix of this pen. */
SDStatus
SDPen_ResetTransform(SDPen *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* reset the transformation */
	_this->transform = SDAffineTransformF_Identity;

	/* return successfully */
	return SDStatus_OK;
}

/* Rotate the transformation matrix of this pen. */
SDStatus
SDPen_RotateTransform(SDPen         *_this,
                      SDFloat        angle,
                      SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* rotate the transformation */
	SDAffineTransformF_Rotate(&(_this->transform), angle, order);

	/* return successfully */
	return SDStatus_OK;
}

/* Scale the transformation matrix of this pen. */
SDStatus
SDPen_ScaleTransform(SDPen         *_this,
                     SDFloat        sx,
                     SDFloat        sy,
                     SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* scale the transformation */
	SDAffineTransformF_Scale(&(_this->transform), sx, sy, order);

	/* return successfully */
	return SDStatus_OK;
}

/* Set the transformation matrix of this pen. */
SDStatus
SDPen_SetTransform(SDPen    *_this,
                   SDMatrix *matrix)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the transformation */
	SDStatus_Check
		(SDMatrix_GetTransform
			(matrix, &(_this->transform)));

	/* return successfully */
	return SDStatus_OK;
}

/* Translate the transformation matrix of this pen. */
SDStatus
SDPen_TranslateTransform(SDPen         *_this,
                         SDFloat        dx,
                         SDFloat        dy,
                         SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* translate the transformation */
	SDAffineTransformF_Translate(&(_this->transform), dx, dy, order);

	/* return successfully */
	return SDStatus_OK;
}

/* Get a pattern for this pen. */
SDINTERNAL SDStatus
SDPen_GetPattern(SDPen     *_this,
                 SDPattern *pattern)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((pattern != 0));

	/* get the pattern */
	SDStatus_Check
		(SDBrush_GetPattern
			(_this->brush, pattern));

	/* return successfully */
	return SDStatus_OK;
}

#ifdef __cplusplus
};
#endif
