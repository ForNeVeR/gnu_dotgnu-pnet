/*
 * SDBlend.c - Gradient blending implementation.
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

#include "CBlend.h"
#include "CMath.h"

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL SDStatus
SDBlend_Initialize(SDBlend  *_this,
                   SDUInt32  count)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((count != 0));

	/* allocate the factors */
	if(!(_this->factors = (SDFloat *)SDMalloc(count * sizeof(SDFloat))))
	{
		return SDStatus_OutOfMemory;
	}

	/* allocate the positions */
	if(!(_this->positions = (SDFloat *)SDMalloc(count * sizeof(SDFloat))))
	{
		SDFree(_this->factors);
		return SDStatus_OutOfMemory;
	}

	/* set the count */
	_this->count = count;

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDBlend_Copy(SDBlend *_this,
             SDBlend *copy)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((copy  != 0));

	/* copy the blend */
	{
		/* get the count */
		const SDUInt32 count = _this->count;

		/* get the size */
		const SDUInt32 size = (count * sizeof(SDFloat));

		/* handle trivial case */
		if(count == 0)
		{
			*copy = SDBlend_Zero;
		}

		/* allocate the factors */
		if(!(copy->factors = (SDFloat *)SDMalloc(size)))
		{
			return SDStatus_OutOfMemory;
		}

		/* copy the factors */
		SDMemCopy(copy->factors, _this->factors, size);

		/* allocate the positions */
		if(!(copy->positions = (SDFloat *)SDMalloc(size)))
		{
			SDFree(copy->factors);
			return SDStatus_OutOfMemory;
		}

		/* copy the positions */
		SDMemCopy(copy->positions, _this->positions, size);

		/* copy the count */
		copy->count = count;
	}

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL void
SDBlend_SetTriangularShape(SDBlend *_this,
                           SDFloat  focus,
                           SDFloat  scale)
{
	/* declarations */
	SDFloat *currF;
	SDFloat *currP;

	/* assertions */
	SDASSERT((_this            != 0));
	SDASSERT((_this->factors   != 0));
	SDASSERT((_this->positions != 0));

	/* get the output pointers */
	currF = _this->factors;
	currP = _this->positions;

	/* set the blend factors and positions */
	if(focus == 0.0f)
	{
		/* assertions */
		SDASSERT((_this->count == SDBlend_TriangularHalfCount));

		/* set the starting factor and position */
		*currF++ = scale;
		*currP++ = 0.0f;

		/* set the ending factor and position */
		*currF = 0.0f;
		*currP = 1.0f;
	}
	else if(focus == 1.0f)
	{
		/* assertions */
		SDASSERT((_this->count == SDBlend_TriangularHalfCount));

		/* set the starting factor and position */
		*currF++ = 0.0f;
		*currP++ = 0.0f;

		/* set the ending factor and position */
		*currF = scale;
		*currP = 1.0f;
	}
	else
	{
		/* assertions */
		SDASSERT((_this->count == SDBlend_TriangularFullCount));

		/* set the starting factor and position */
		*currF++ = 0.0f;
		*currP++ = 0.0f;

		/* set the intermediate factor and position */
		*currF++ = scale;
		*currP++ = focus;

		/* set the ending factor and position */
		*currF = 0.0f;
		*currP = 1.0f;
	}
}

SDINTERNAL void
SDBlend_SetSigmaBellShape(SDBlend *_this,
                          SDFloat  focus,
                          SDFloat  scale)
{
	/*\
	|*| NOTE: The cumulative distribution function of the standard normal
	|*|       distribution (i.e. the bell curve) is used here.
	|*|
	|*|   cfd(x) = (1 / 2) * (1 + erf(z(x)))
	|*|     z(x) = (x - mu) / (sigma * sqrt(2))
	|*|
	|*|    sigma = standard deviation
	|*|       mu = mean
	|*|      cfd = cumulative distribution function
	|*|      erf = error function
	|*|     sqrt = square root function
	|*|
	|*|
	|*| see: http://en.wikipedia.org/wiki/Normal_distribution
	|*| see: http://mathworld.wolfram.com/NormalDistribution.html
	|*|
	\*/

	/* declarations */
	SDFloat *currF;
	SDFloat *currP;
	SDFloat *last;

	/* assertions */
	SDASSERT((_this            != 0));
	SDASSERT((_this->factors   != 0));
	SDASSERT((_this->positions != 0));

	/* get the output pointers */
	currF = _this->factors;
	currP = _this->positions;

	/* get the end of output pointer */
	last = (currF + (_this->count - 1));

	/* define the error function input calculation */
	#define _CALC_Z(pos) (((pos) * zmul) + zoff)

	/* define the factor calculation */
	#define _CALC_F(pos) ((SDMath_Erf(_CALC_Z(pos)) * fmul) + foff)

	/* set the blend factors and positions */
	if(focus == 0.0f)
	{
		/* declarations */
		SDFloat pos;

		/* calculate distance between samples */
		const SDDouble delta = (1.0f / 255.0f);

		/* calculate error function input multiplier (sigma = 1/2) */
		const SDDouble zmul = (1.0f / (0.5f * SDMath_Sqrt(2.0f)));

		/* calculate error function input offset (mu = 1/2) */
		const SDDouble zoff = -(0.5f * zmul);

		/* calculate the curve extrema */
		const SDDouble cmax  = (0.5f * (1.0f - SDMath_Erf(_CALC_Z(0.0f))));
		const SDDouble cmin  = (0.5f * (1.0f - SDMath_Erf(_CALC_Z(1.0f))));
		const SDDouble cdiff = (cmax - cmin);

		/* precalculate factor calculation values */
		const SDDouble foff =  ((scale * (0.5f - cmin)) / cdiff);
		const SDDouble fmul = -((scale * (0.5f       )) / cdiff);

		/* assertions */
		SDASSERT((_this->count == SDBlend_SigmaBellHalfCount));

		/* set the starting factor and position */
		*currF++ = scale;
		*currP++ = 0.0f;

		/* set the position to the first sample */
		pos = delta;

		/* set the intermediate factors and positions */
		while(currF != last)
		{
			/* set the current factor and position */
			*currF++ = _CALC_F(pos);
			*currP++ = pos;

			/* update the position */
			pos += delta;
		}

		/* set the ending factor and position */
		*currF = 0.0f;
		*currP = 1.0f;
	}
	else if(focus == 1.0f)
	{
		/* declarations */
		SDFloat pos;

		/* calculate distance between samples */
		const SDDouble delta = (1.0f / 255.0f);

		/* calculate error function input multiplier (sigma = 1/2) */
		const SDDouble zmul = (1.0f / (0.5f * SDMath_Sqrt(2.0f)));

		/* calculate error function input offset (mu = 1/2) */
		const SDDouble zoff = -(0.5f * zmul);

		/* calculate the curve extrema */
		const SDDouble cmax  = (0.5f * (1.0f + SDMath_Erf(_CALC_Z(1.0f))));
		const SDDouble cmin  = (0.5f * (1.0f + SDMath_Erf(_CALC_Z(0.0f))));
		const SDDouble cdiff = (cmax - cmin);

		/* precalculate factor calculation values */
		const SDDouble foff = ((scale * (0.5f - cmin)) / cdiff);
		const SDDouble fmul = ((scale * (0.5f       )) / cdiff);

		/* assertions */
		SDASSERT((_this->count == SDBlend_SigmaBellHalfCount));

		/* set the starting factor and position */
		*currF++ = 0.0f;
		*currP++ = 0.0f;

		/* set the position to the first sample */
		pos = delta;

		/* set the intermediate factors and positions */
		while(currF != last)
		{
			/* set the current factor and position */
			*currF++ = _CALC_F(pos);
			*currP++ = pos;

			/* update the position */
			pos += delta;
		}

		/* set the ending factor and position */
		*currF = scale;
		*currP = 1.0f;
	}
	else
	{
		/* declarations */
		SDDouble pos, delta, zmul, zoff, cmax, cmin, cdiff, foff, fmul;

		/* get the middle of output pointer */
		SDFloat *const middle = (currF + 256);

		/* assertions */
		SDASSERT((_this->count == SDBlend_SigmaBellFullCount));

		/* calculate distance between samples */
		delta = (focus / 255.0f);

		/* calculate error function input multiplier (sigma = focus/4) */
		zmul = (1.0f / (focus * 0.25f * SDMath_Sqrt(2.0f)));

		/* calculate error function input offset (mu = focus/2) */
		zoff = -(focus * 0.5f * zmul);

		/* calculate the curve extrema */
		cmax  = (0.5f * (1.0f + SDMath_Erf(_CALC_Z(focus))));
		cmin  = (0.5f * (1.0f + SDMath_Erf(_CALC_Z(0.0f))));
		cdiff = (cmax - cmin);

		/* precalculate factor calculation values */
		foff = ((scale * (0.5f - cmin)) / cdiff);
		fmul = ((scale * (0.5f       )) / cdiff);

		/* set the starting factor and position */
		*currF++ = 0.0f;
		*currP++ = 0.0f;

		/* set the position to the first pre-peak sample */
		pos = delta;

		/* set the intermediate pre-peak factors and positions */
		while(currF != middle)
		{
			/* set the current factor and position */
			*currF++ = _CALC_F(pos);
			*currP++ = pos;

			/* update the position */
			pos += delta;
		}

		/* set the peak factor and position */
		*currF++ = scale;
		*currP++ = focus;

		/* calculate distance between samples */
		delta = ((1.0f - focus) / 255.0f);

		/* calculate error function input multiplier (sigma = (1-focus)/4) */
		zmul = (1.0f / ((1.0f - focus) * 0.25f * SDMath_Sqrt(2.0f)));

		/* calculate error function input offset (mu = (1+focus)/2) */
		zoff = -((1.0f + focus) * 0.5f * zmul);

		/* calculate the curve extrema */
		cmax  = (0.5f * (1.0f - SDMath_Erf(_CALC_Z(focus))));
		cmin  = (0.5f * (1.0f - SDMath_Erf(_CALC_Z(1.0f))));
		cdiff = (cmax - cmin);

		/* precalculate factor calculation values */
		foff =  ((scale * (0.5f - cmin)) / cdiff);
		fmul = -((scale * (0.5f       )) / cdiff);

		/* set the position to the first post-peak sample */
		pos = focus + delta;

		/* set the intermediate post-peak factors and positions */
		while(currF != last)
		{
			/* set the current factor and position */
			*currF++ = _CALC_F(pos);
			*currP++ = pos;

			/* update the position */
			pos += delta;
		}

		/* set the ending factor and position */
		*currF = 0.0f;
		*currP = 1.0f;
	}

	/* undefine macros */
	#undef _CALC_Z
	#undef _CALC_F
}

SDINTERNAL void
SDBlend_Finalize(SDBlend *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the blend, as needed */
	if(_this->count != 0)
	{
		SDFree(_this->factors);
		SDFree(_this->positions);
		_this->count = 0;
	}
}

SDINTERNAL SDStatus
SDColorBlend_Copy(SDColorBlend *_this,
                  SDColorBlend *copy)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((copy  != 0));

	/* copy the color blend */
	{
		/* get the count */
		const SDUInt32 count = _this->count;

		/* get the color size */
		const SDUInt32 sizeC = (count * sizeof(SDColor));

		/* get the position size */
		const SDUInt32 sizeP = (count * sizeof(SDFloat));

		/* handle trivial case */
		if(count == 0)
		{
			*copy = SDColorBlend_Zero;
		}

		/* allocate the colors */
		if(!(copy->colors = (SDColor *)SDMalloc(sizeC)))
		{
			return SDStatus_OutOfMemory;
		}

		/* copy the colors */
		SDMemCopy(copy->colors, _this->colors, sizeC);

		/* allocate the positions */
		if(!(copy->positions = (SDFloat *)SDMalloc(sizeP)))
		{
			SDFree(copy->colors);
			return SDStatus_OutOfMemory;
		}

		/* copy the positions */
		SDMemCopy(copy->positions, _this->positions, sizeP);

		/* copy the count */
		copy->count = count;
	}

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL void
SDColorBlend_Finalize(SDColorBlend *_this)
{
	/* assertions */
	SDASSERT((_this        != 0));

	/* finalize the blend, as needed */
	if(_this->count != 0)
	{
		SDFree(_this->colors);
		SDFree(_this->positions);
		_this->count = 0;
	}
}


#ifdef __cplusplus
};
#endif
