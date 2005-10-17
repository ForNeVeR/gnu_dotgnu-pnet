/*
 * SDAffineTransform.c - Affine transformation implementation.
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

#include "CAffineTransform.h"
#include "CMath.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Determine if the given transformations are equal. */
SDINTERNAL void
SDAffineTransformF_Equals(const SDAffineTransformF *_this,
                          const SDAffineTransformF *other,
                          SDBool                   *eq)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((other != 0));
	SDASSERT((eq    != 0));

	/* determine equality */
	*eq = !SDMemCmp(_this, other, sizeof(SDAffineTransformF));
}

/* Determine if the given transformations are not equal. */
SDINTERNAL void
SDAffineTransformF_NotEquals(const SDAffineTransformF *_this,
                             const SDAffineTransformF *other,
                             SDBool                   *ne)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((other != 0));
	SDASSERT((ne    != 0));

	/* determine inequality */
	*ne = !(!SDMemCmp(_this, other, sizeof(SDAffineTransformF)));
}

/* Set this transformation to the identity transformation. */
SDINTERNAL void
SDAffineTransformF_SetIdentity(SDAffineTransformF *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* set the transformation to the identity transformation */
	(*_this) = SDAffineTransformF_Identity;
}

/* Set this transformation to warp to a parallelogram. */
SDINTERNAL SDStatus
SDAffineTransformF_SetParallelogram(SDAffineTransformF *_this,
                                    SDRectangleF        rect,
                                    SDPointF            tl,
                                    SDPointF            tr,
                                    SDPointF            bl)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* ensure we have a non-singular rectangle matrix */
	SDStatus_Require
		(((SDRectangle_Width(rect)  != 0.0f) &&
		  (SDRectangle_Height(rect) != 0.0f)),
		 SDStatus_InvalidOperation_SingularMatrix);

	/* set the transformation to the parallelogram warp transformation */
	{
		/* calculate values used multiple times */
		const SDDouble scaleX = (1.0 / SDRectangle_Width(rect));
		const SDDouble scaleY = (1.0 / SDRectangle_Height(rect));
		const SDDouble transX = -SDRectangle_X(rect);
		const SDDouble transY = -SDRectangle_Y(rect);

		/* set the transformation elements */
		_this->m11 = ((SDPoint_X(tr) - SDPoint_X(tl)) * scaleX);
		_this->m12 = ((SDPoint_Y(tr) - SDPoint_Y(tl)) * scaleX);
		_this->m21 = ((SDPoint_X(bl) - SDPoint_X(tl)) * scaleY);
		_this->m22 = ((SDPoint_Y(bl) - SDPoint_Y(tl)) * scaleY);
		_this->dx  =
			(SDPoint_X(tl) + (_this->m11 * transX) + (_this->m21 * transY));
		_this->dy  =
			(SDPoint_Y(tl) + (_this->m12 * transX) + (_this->m22 * transY));
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Set the elements of this transformation. */
SDINTERNAL void
SDAffineTransformF_SetElements(SDAffineTransformF *_this,
                               SDFloat             m11,
                               SDFloat             m12,
                               SDFloat             m21,
                               SDFloat             m22,
                               SDFloat             dx,
                               SDFloat             dy)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* set the elements of the transformation */
	_this->m11 = m11;
	_this->m12 = m12;
	_this->m21 = m21;
	_this->m22 = m22;
	_this->dx  = dx;
	_this->dy  = dy;
}

/* Get the determinant of this transformation. */
SDINTERNAL void
SDAffineTransformF_GetDeterminant(const SDAffineTransformF *_this,
                                  SDFloat                  *determinant)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* calculate the determinant */
	*determinant = ((_this->m11 * _this->m22) - (_this->m12 * _this->m21));
}

/* Get the inverse of this transformation. */
SDINTERNAL SDStatus
SDAffineTransformF_GetInverse(const SDAffineTransformF *_this,
                              SDAffineTransformF       *inverse)
{
	/* declarations */
	SDFloat determinant;

	/* assertions */
	SDASSERT((_this != 0));

	/* get the determinant */
	SDAffineTransformF_GetDeterminant(_this, &determinant);

	/* ensure the transformation is invertible */
	SDStatus_Require
		((determinant != 0.0f), SDStatus_InvalidOperation_SingularMatrix);

	/* perform the division once */
	determinant = (1.0 / determinant);

	/* set the inverse transformation */
	inverse->m11 = ( _this->m22 * determinant);
	inverse->m12 = (-_this->m12 * determinant);
	inverse->m21 = (-_this->m21 * determinant);
	inverse->m22 = ( _this->m11 * determinant);
	inverse->dx  =
		(((_this->dy * _this->m21) - (_this->dx * _this->m22)) * determinant);
	inverse->dy  =
		(((_this->dx * _this->m12) - (_this->dy * _this->m11)) * determinant);

	/* return successfully */
	return SDStatus_OK;
}

/* Multiply this transformation with another. */
SDINTERNAL void
SDAffineTransformF_Multiply(SDAffineTransformF       *_this,
                            const SDAffineTransformF *other,
                            SDMatrixOrder             order)
{
	/* declarations */
	SDAffineTransformF t1;
	SDAffineTransformF t2;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((other != 0));

	/* set up transformations based on order */
	if(order == SDMatrixOrder_Prepend)
	{
		t1 = *other;
		t2 = *_this;
	}
	else
	{
		t1 = *_this;
		t2 = *other;
	}

	/* multiply transformations */
	_this->m11 = ((t1.m11 * t2.m11) + (t1.m12 * t2.m21));
	_this->m12 = ((t1.m12 * t2.m22) + (t1.m11 * t2.m12));
	_this->m21 = ((t1.m21 * t2.m11) + (t1.m22 * t2.m21));
	_this->m22 = ((t1.m22 * t2.m22) + (t1.m21 * t2.m12));
	_this->dx  = ((t1.dx  * t2.m11) + (t1.dy  * t2.m21) + (t2.dx));
	_this->dy  = ((t1.dy  * t2.m22) + (t1.dx  * t2.m12) + (t2.dy));
}

/* Inverse multiply this transformation with another. */
SDINTERNAL SDStatus
SDAffineTransformF_MultiplyInverse(SDAffineTransformF       *_this,
                                   const SDAffineTransformF *other,
                                   SDMatrixOrder             order)
{
	/* declarations */
	SDAffineTransformF t1;
	SDAffineTransformF t2;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((other != 0));

	/* set up transformations based on order */
	if(order == SDMatrixOrder_Prepend)
	{
		t1 = *_this;
		SDStatus_Check
			(SDAffineTransformF_GetInverse
				(other, &t2));
	}
	else
	{
		SDStatus_Check
			(SDAffineTransformF_GetInverse
				(other, &t1));
		t2 = *_this;
	}

	/* multiply transformations */
	_this->m11 = ((t1.m11 * t2.m11) + (t1.m12 * t2.m21));
	_this->m12 = ((t1.m12 * t2.m22) + (t1.m11 * t2.m12));
	_this->m21 = ((t1.m21 * t2.m11) + (t1.m22 * t2.m21));
	_this->m22 = ((t1.m22 * t2.m22) + (t1.m21 * t2.m12));
	_this->dx  = ((t1.dx  * t2.m11) + (t1.dy  * t2.m21) + (t2.dx));
	_this->dy  = ((t1.dy  * t2.m22) + (t1.dx  * t2.m12) + (t2.dy));

	/* return successfully */
	return SDStatus_OK;
}

/* Rotate a transformation. */
SDINTERNAL void
SDAffineTransformF_Rotate(SDAffineTransformF *_this,
                          SDFloat             angle,
                          SDMatrixOrder       order)
{
	/* declarations */
	SDAffineTransformF rotate;
	SDDouble           radians;
	SDFloat            sin;
	SDFloat            cos;

	/* assertions */
	SDASSERT((_this != 0));

	/* calculate the radians */
	radians = SDMath_ToRadians(angle);

	/* calculate the sine */
	sin = (SDFloat)SDMath_Sin(radians);

	/* calculate the cosine */
	cos = (SDFloat)SDMath_Cos(radians);

	/* create the rotation transformation */
	SDAffineTransformF_SetElements(&rotate, cos, sin, -sin, cos, 0, 0);

	/* rotate the transformation */
	SDAffineTransformF_Multiply(_this, &rotate, order);
}

/* Inverse rotate a transformation. */
SDINTERNAL void
SDAffineTransformF_RotateInverse(SDAffineTransformF *_this,
                                 SDFloat             angle,
                                 SDMatrixOrder       order)
{
	/* declarations */
	SDAffineTransformF rotate;
	SDDouble           radians;
	SDFloat            sin;
	SDFloat            cos;

	/* assertions */
	SDASSERT((_this != 0));

	/* calculate the radians */
	radians = SDMath_ToRadians(-angle);

	/* calculate the sine */
	sin = (SDFloat)SDMath_Sin(radians);

	/* calculate the cosine */
	cos = (SDFloat)SDMath_Cos(radians);

	/* create the rotation transformation */
	SDAffineTransformF_SetElements(&rotate, cos, sin, -sin, cos, 0, 0);

	/* invert the order */
	if(order == SDMatrixOrder_Prepend)
	{
		order = SDMatrixOrder_Append;
	}
	else
	{
		order = SDMatrixOrder_Prepend;
	}

	/* rotate the transformation */
	SDAffineTransformF_Multiply(_this, &rotate, order);
}

/* Scale a transformation. */
SDINTERNAL void
SDAffineTransformF_Scale(SDAffineTransformF *_this,
                         SDFloat             scaleX,
                         SDFloat             scaleY,
                         SDMatrixOrder       order)
{
	/*\
	|*| NOTE: technically we could just multiply with an
	|*|       SDAffineTransformF(scaleX, 0, 0, scaleY, 0, 0),
	|*|       but this is more efficient
	\*/

	/* assertions */
	SDASSERT((_this != 0));

	/* scale the transformation */
	if(order == SDMatrixOrder_Prepend)
	{
		_this->m11 *= scaleX;
		_this->m12 *= scaleX;
		_this->m21 *= scaleY;
		_this->m22 *= scaleY;
	}
	else
	{
		_this->m11 *= scaleX;
		_this->m12 *= scaleY;
		_this->m21 *= scaleX;
		_this->m22 *= scaleY;
		_this->dx  *= scaleX;
		_this->dy  *= scaleY;
	}
}

/* Inverse scale a transformation. */
SDINTERNAL void
SDAffineTransformF_ScaleInverse(SDAffineTransformF *_this,
                                SDFloat             scaleX,
                                SDFloat             scaleY,
                                SDMatrixOrder       order)
{
	/* declarations */
	SDDouble sx;
	SDDouble sy;

	/* assertions */
	SDASSERT((_this != 0));

	/* calculate the inverse scale factors */
	sx = (1.0f / (SDDouble)scaleX);
	sy = (1.0f / (SDDouble)scaleY);

	/* scale the transformation */
	if(order == SDMatrixOrder_Prepend)
	{
		_this->m11 *= sx;
		_this->m12 *= sy;
		_this->m21 *= sx;
		_this->m22 *= sy;
		_this->dx  *= sx;
		_this->dy  *= sy;
	}
	else
	{
		_this->m11 *= sx;
		_this->m12 *= sx;
		_this->m21 *= sy;
		_this->m22 *= sy;
	}
}

/* Extract the scaling factors from, then unscale, this transformation. */
SDINTERNAL void
SDAffineTransformF_ExtractScale(SDAffineTransformF *_this,
                                SDFloat            *scaleX,
                                SDFloat            *scaleY,
                                SDMatrixOrder       order)
{
	/* declarations */
	SDFloat m11;
	SDFloat m12;
	SDFloat m21;
	SDFloat m22;
	SDFloat determinant;
	SDFloat shear;

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((scaleX != 0));
	SDASSERT((scaleY != 0));

	/* get the transformation components */
	m11 = _this->m11;
	m12 = _this->m12;
	m21 = _this->m21;
	m22 = _this->m22;

	/* get the determinant */
	SDAffineTransformF_GetDeterminant(_this, &determinant);

	/* calculate the horizontal scaling factor */
	*scaleX = SDMath_Sqrt((m11 * m11) + (m12 * m12));

	/* unscale the first row */
	m11 *= (1.0f / *scaleX);
	m12 *= (1.0f / *scaleX);

	/* calculate the shear adjustment ((shearX / shearY) * scaleY) */
	shear = ((m11 * m21) + (m12 * m22));

	/* compensate for shear */
	m21 -= (shear * m11);
	m22 -= (shear * m12);

	/* calculate the vertical scaling factor */
	*scaleY = SDMath_Sqrt((m21 * m21) + (m22 * m22));

	/* handle reflection, as needed */
	if(determinant < 0.0f)
	{
		*scaleX *= -1;
		*scaleY *= -1;
	}

	/* inverse scale the transformation */
	if(order == SDMatrixOrder_Prepend)
	{
		SDAffineTransformF_ScaleInverse
			(_this, *scaleX, *scaleY, SDMatrixOrder_Append);
	}
	else
	{
		SDAffineTransformF_ScaleInverse
			(_this, *scaleX, *scaleY, SDMatrixOrder_Prepend);
	}
}

/* Shear a transformation. */
SDINTERNAL void
SDAffineTransformF_Shear(SDAffineTransformF *_this,
                         SDFloat             shearX,
                         SDFloat             shearY,
                         SDMatrixOrder       order)
{
	/*\
	|*| NOTE: technically we could just multiply with an
	|*|       SDAffineTransformF(1, shearX, shearY, 1, 0, 0),
	|*|       but this is more efficient
	\*/

	/* assertions */
	SDASSERT((_this != 0));

	/* shear the transformation */
	if(order == SDMatrixOrder_Prepend)
	{
		/* get the first row of the transformation */
		const SDFloat m11 = _this->m11;
		const SDFloat m12 = _this->m12;

		/* perform the shear */
		_this->m11 += (_this->m21 * shearY);
		_this->m12 += (_this->m22 * shearY);
		_this->m21 += (       m11 * shearX);
		_this->m22 += (       m12 * shearX);
	}
	else
	{
		/* get the first column of the transformation */
		const SDFloat m11 = _this->m11;
		const SDFloat m21 = _this->m21;
		const SDFloat dx  = _this->dx;

		/* perform the shear */
		_this->m11 += (_this->m12 * shearY);
		_this->m12 += (       m11 * shearX);
		_this->m21 += (_this->m22 * shearY);
		_this->m22 += (       m21 * shearX);
		_this->dx  += (_this->dy  * shearY);
		_this->dy  += (       dx  * shearX);
	}
}

/* Translate a transformation. */
SDINTERNAL void
SDAffineTransformF_Translate(SDAffineTransformF *_this,
                             SDFloat             offsetX,
                             SDFloat             offsetY,
                             SDMatrixOrder       order)
{
	/*\
	|*| NOTE: technically we could just multiply with an
	|*|       SDAffineTransformF(1, 0, 0, 1, offsetX, offsetY),
	|*|       but this is more efficient
	\*/

	/* assertions */
	SDASSERT((_this != 0));

	/* translate the matrix */
	if(order == SDMatrixOrder_Prepend)
	{
		_this->dx += (_this->m11 * offsetX) + (_this->m21 * offsetY);
		_this->dy += (_this->m12 * offsetX) + (_this->m22 * offsetY);
	}
	else
	{
		_this->dx += offsetX;
		_this->dy += offsetY;
	}
}

/* Inverse translate a transformation. */
SDINTERNAL void
SDAffineTransformF_TranslateInverse(SDAffineTransformF *_this,
                                    SDFloat             offsetX,
                                    SDFloat             offsetY,
                                    SDMatrixOrder       order)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* inverse translate the matrix */
	if(order == SDMatrixOrder_Prepend)
	{
		_this->dx -= offsetX;
		_this->dy -= offsetY;
	}
	else
	{
		_this->dx -= (_this->m11 * offsetX) + (_this->m21 * offsetY);
		_this->dy -= (_this->m12 * offsetX) + (_this->m22 * offsetY);
	}
}

/* Transform a list of points. */
SDINTERNAL void
SDAffineTransformF_TransformPoints(const SDAffineTransformF *_this,
                                   SDPointF                 *points,
                                   SDUInt32                  count)
{
	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((points != 0));

	/* transform the points */
	{
		/* get the transformation */
		const SDAffineTransformF t = *_this;

		/* get the end pointer */
		const SDPointF *const end = (points + count);

		/* transform the points */
		while(points != end)
		{
			/* get the x coordinate */
			const SDFloat x = SDPoint_X(*points);

			/* get the y coordinate */
			const SDFloat y = SDPoint_Y(*points);

			/* transform the point */
			SDPoint_X(*points) = ((x * t.m11) + (y * t.m21) + t.dx);
			SDPoint_Y(*points) = ((x * t.m12) + (y * t.m22) + t.dy);

			/* update the points pointer */
			++points;
		}
	}
}

/* Transform a list of vectors. */
SDINTERNAL void
SDAffineTransformF_TransformVectors(const SDAffineTransformF *_this,
                                    SDPointF                 *points,
                                    SDUInt32                  count)
{
	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((points != 0));

	/* transform the vectors */
	{
		/* get the transformation */
		const SDAffineTransformF t = *_this;

		/* get the end pointer */
		const SDPointF *const end = (points + count);

		/* transform the vectors */
		while(points != end)
		{
			/* get the horizontal weight */
			const SDFloat x = SDPoint_X(*points);

			/* get the vertical weight */
			const SDFloat y = SDPoint_Y(*points);

			/* transform the vector */
			SDPoint_X(*points) = ((x * t.m11) + (y * t.m21));
			SDPoint_Y(*points) = ((x * t.m12) + (y * t.m22));

			/* update the vector pointer */
			++points;
		}
	}
}

SDINTERNAL void
SDVectorF_ScalePoints(const SDVectorF *_this,
                      SDPointF        *points,
                      SDUInt32         count)
{
	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((points != 0));

	/* scale the points */
	{
		/* get the scaling factors */
		const SDFloat scaleX = SDVector_X(*_this);
		const SDFloat scaleY = SDVector_Y(*_this);

		/* get the end pointer */
		const SDPointF *const end = (points + count);

		/* scale the points */
		while(points != end)
		{
			/* scale the point */
			SDPoint_X(*points) *= scaleX;
			SDPoint_Y(*points) *= scaleY;

			/* update the points pointer */
			++points;
		}
	}
}

SDINTERNAL void
SDVectorF_TranslatePoints(const SDVectorF *_this,
                          SDPointF        *points,
                          SDUInt32         count)
{
	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((points != 0));

	/* translate the points */
	{
		/* get the translation factors */
		const SDFloat transX = SDVector_X(*_this);
		const SDFloat transY = SDVector_Y(*_this);

		/* get the end pointer */
		const SDPointF *const end = (points + count);

		/* translate the points */
		while(points != end)
		{
			/* translate the point */
			SDPoint_X(*points) += transX;
			SDPoint_Y(*points) += transY;

			/* update the points pointer */
			++points;
		}
	}
}


#ifdef __cplusplus
};
#endif
