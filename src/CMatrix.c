/*
 * SDMatrix.c - Matrix implementation.
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

#include "CMatrix.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Create an identity matrix. */
SDStatus
SDMatrix_Create(SDMatrix **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the matrix */
	if(!(*_this = (SDMatrix *)SDMalloc(sizeof(SDMatrix))))
	{
		return SDStatus_OutOfMemory;
	}

	/* set the transformation to the identity transformation */
	(*_this)->transform = SDAffineTransformF_Identity;

	/* return successfully */
	return SDStatus_OK;
}

/* Create a parallelogram warp matrix. */
SDStatus
SDMatrix_CreateParallelogram(SDMatrix     **_this,
                             SDRectangleF   rect,
                             SDPointF       tl,
                             SDPointF       tr,
                             SDPointF       bl)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the matrix */
	if(!(*_this = (SDMatrix *)SDMalloc(sizeof(SDMatrix))))
	{
		return SDStatus_OutOfMemory;
	}

	/* set the transformation to the parallelogram warp transformation */
	status =
		SDAffineTransformF_SetParallelogram
			(&((*_this)->transform), rect, tl, tr, bl);

	/* handle status */
	if(status != SDStatus_OK)
	{
		SDFree(*_this);
		*_this = 0;
	}

	/* return status */
	return status;
}

/* Create a matrix with the given elements. */
SDStatus
SDMatrix_CreateElements(SDMatrix **_this,
                        SDFloat    m11,
                        SDFloat    m12,
                        SDFloat    m21,
                        SDFloat    m22,
                        SDFloat    dx,
                        SDFloat    dy)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the matrix */
	if(!(*_this = (SDMatrix *)SDMalloc(sizeof(SDMatrix))))
	{
		return SDStatus_OutOfMemory;
	}

	/* set the elements of the transformation */
	SDAffineTransformF_SetElements
		(&((*_this)->transform), m11, m12, m21, m22, dx, dy);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the determinant of this matrix. */
SDStatus
SDMatrix_GetDeterminant(SDMatrix *_this,
                        SDFloat  *determinant)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a determinant pointer */
	SDStatus_Require((determinant != 0), SDStatus_ArgumentNull);

	/* get the determinant */
	SDAffineTransformF_GetDeterminant
		(&(_this->transform), determinant);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the inverse of this matrix. */
SDStatus
SDMatrix_GetInverse(SDMatrix *_this,
                    SDMatrix *inverse)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an inverse pointer */
	SDStatus_Require((inverse != 0), SDStatus_ArgumentNull);

	/* get the inverse */
	return
		SDAffineTransformF_GetInverse
			(&(_this->transform), &(inverse->transform));
}

/* Get the transformation of this matrix. */
SDINTERNAL SDStatus
SDMatrix_GetTransform(SDMatrix           *_this,
                      SDAffineTransformF *transform)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a transformation pointer */
	SDStatus_Require((transform != 0), SDStatus_ArgumentNull);

	/* get the transformation */
	*transform = _this->transform;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the transformation of this matrix. */
SDINTERNAL SDStatus
SDMatrix_SetTransform(SDMatrix           *_this,
                      SDAffineTransformF *transform)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a transformation pointer */
	SDStatus_Require((transform != 0), SDStatus_ArgumentNull);

	/* set the transformation */
	_this->transform = *transform;

	/* return successfully */
	return SDStatus_OK;
}

/* Multiply this matrix with another. */
SDStatus
SDMatrix_Multiply(SDMatrix      *_this,
                  SDMatrix      *other,
                  SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an other pointer */
	SDStatus_Require((other != 0), SDStatus_ArgumentNull);

	/* perform the multiplication */
	SDAffineTransformF_Multiply
		(&(_this->transform), &(other->transform), order);

	/* return successfully */
	return SDStatus_OK;
}

/* Determine if the given matrices are equal. */
SDStatus
SDMatrix_Equals(SDMatrix *_this,
                SDMatrix *other,
                SDBool   *eq)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an other pointer */
	SDStatus_Require((other != 0), SDStatus_ArgumentNull);

	/* ensure we have an equality pointer */
	SDStatus_Require((eq != 0), SDStatus_ArgumentNull);

	/* determine equality */
	SDAffineTransformF_Equals(&(_this->transform), &(other->transform), eq);

	/* return successfully */
	return SDStatus_OK;
}

/* Determine if the given matrices are not equal. */
SDStatus
SDMatrix_NotEquals(SDMatrix *_this,
                   SDMatrix *other,
                   SDBool   *ne)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an other pointer */
	SDStatus_Require((other != 0), SDStatus_ArgumentNull);

	/* ensure we have an inequality pointer */
	SDStatus_Require((ne != 0), SDStatus_ArgumentNull);

	/* determine equality */
	SDAffineTransformF_NotEquals(&(_this->transform), &(other->transform), ne);

	/* return successfully */
	return SDStatus_OK;
}

/* Rotate this matrix. */
SDStatus
SDMatrix_Rotate(SDMatrix      *_this,
                SDFloat        angle,
                SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* perform the rotation */
	SDAffineTransformF_Rotate(&(_this->transform), angle, order);

	/* return successfully */
	return SDStatus_OK;
}

/* Scale this matrix. */
SDStatus
SDMatrix_Scale(SDMatrix      *_this,
               SDFloat        scaleX,
               SDFloat        scaleY,
               SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* perform the scale */
	SDAffineTransformF_Scale(&(_this->transform), scaleX, scaleY, order);

	/* return successfully */
	return SDStatus_OK;
}

/* Shear this matrix. */
SDStatus
SDMatrix_Shear(SDMatrix      *_this,
               SDFloat        shearX,
               SDFloat        shearY,
               SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* perform the shear */
	SDAffineTransformF_Shear(&(_this->transform), shearX, shearY, order);

	/* return successfully */
	return SDStatus_OK;
}

/* Translate this matrix. */
SDStatus
SDMatrix_Translate(SDMatrix      *_this,
                   SDFloat        offsetX,
                   SDFloat        offsetY,
                   SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* perform the translation */
	SDAffineTransformF_Translate(&(_this->transform), offsetX, offsetY, order);

	/* return successfully */
	return SDStatus_OK;
}

/* Transform a list of points. */
SDStatus
SDMatrix_TransformPoints(SDMatrix *_this,
                         SDPointF *points,
                         SDUInt32  count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a points pointer */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* perform the point transformations */
	SDAffineTransformF_TransformPoints(&(_this->transform), points, count);

	/* return successfully */
	return SDStatus_OK;
}

/* Transform a list of vectors. */
SDStatus
SDMatrix_TransformVectors(SDMatrix *_this,
                          SDPointF *points,
                          SDUInt32  count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a points pointer */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* perform the vector transformations */
	SDAffineTransformF_TransformVectors(&(_this->transform), points, count);

	/* return successfully */
	return SDStatus_OK;
}

#ifdef __cplusplus
};
#endif
