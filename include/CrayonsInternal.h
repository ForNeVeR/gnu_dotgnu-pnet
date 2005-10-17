/*
 * SDLibInternal.h - Internal library header.
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

#ifndef _SD_LIBRARYINTERNAL_H_
#define _SD_LIBRARYINTERNAL_H_

#include <pixman.h>
#include "Crayons.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Define function attributes. */
#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)) && defined(__ELF__)
	#ifndef SDTESTING
		#define SDINTERNAL __attribute__((__visibility__("hidden")))
	#else
		#define SDINTERNAL
	#endif
#else
	#define SDINTERNAL
#endif

#if (__GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5))
	#define SDMATH __attribute__((__const__))
#else
	#define SDMATH
#endif

/* Include basic system headers. */
#ifdef HAVE_STDLIB_H
	#include <stdlib.h>
#endif
#if (defined(HAVE_ASSERT_H) && defined(SDDEBUG))
	#include <assert.h>
	#define SDASSERT(foo) assert(foo)
#else
	#define SDASSERT(foo)
#endif

typedef unsigned int SDBitField;
typedef SDUInt32     SDFixedU;
typedef SDInt32      SDFixed;

typedef struct _tagSDPointX SDPointX;
struct _tagSDPointX
{
	SDFixed x;
	SDFixed y;
};

typedef struct _tagSDLineX SDLineX;
struct _tagSDLineX
{
	SDPointX point1;
	SDPointX point2;
};

typedef struct _tagSDEdgeX SDEdgeX;
struct _tagSDEdgeX
{
	SDLineX line;
	SDBool  clockwise;
	SDFixed currentX;
};

typedef struct _tagSDTrapezoidX SDTrapezoidX;
struct _tagSDTrapezoidX
{
	SDFixed top;
	SDFixed bottom;
	SDLineX left;
	SDLineX right;
};

typedef struct _tagSDTrapezoids SDTrapezoids;
struct _tagSDTrapezoids
{
	SDUInt32      count;
	SDUInt32      capacity;
	SDTrapezoidX *trapezoids;
};

typedef struct _tagSDAffineTransformF SDAffineTransformF;
struct _tagSDAffineTransformF
{
	SDFloat m11;
	SDFloat m12;
	SDFloat m21;
	SDFloat m22;
	SDFloat dx;
	SDFloat dy;
};

typedef struct _tagSDPattern SDPattern;
struct _tagSDPattern
{
	pixman_image_t     *image;
	SDAffineTransformF *transform;
};

typedef struct _tagSDPointArrayX SDPointArrayX;
struct _tagSDPointArrayX
{
	SDUInt32  capacity;
	SDUInt32  count;
	SDPointX *points;
};

typedef struct _tagSDPointArrayF SDPointArrayF;
struct _tagSDPointArrayF
{
	SDUInt32  capacity;
	SDUInt32  count;
	SDPointF *points;
};

typedef struct _tagSDBezierX SDBezierX;
struct _tagSDBezierX
{
	SDPointX a;
	SDPointX b;
	SDPointX c;
	SDPointX d;
};

typedef struct _tagSDBezierF SDBezierF;
struct _tagSDBezierF
{
	SDPointF a;
	SDPointF b;
	SDPointF c;
	SDPointF d;
};

typedef struct _tagSDFiller   SDFiller;
typedef struct _tagSDPolygonX SDPolygonX;
typedef struct _tagSDStroker  SDStroker;
typedef SDPointF              SDVectorF;








SDINTERNAL void *
SDMalloc(SDUInt32 size);
SDINTERNAL void *
SDRealloc(void     *ptr,
          SDUInt32  size);
SDINTERNAL void *
SDCalloc(SDUInt32 count,
         SDUInt32 size);
SDINTERNAL void
SDFree(void *ptr);
SDINTERNAL void *
SDMemSet(void     *dst,
         SDByte    value,
         SDUInt32  length);
SDINTERNAL void *
SDMemCopy(void       *dst,
          const void *src,
          SDUInt32    length);
SDINTERNAL void *
SDMemMove(void       *dst,
          const void *src,
          SDUInt32    length);
SDINTERNAL int
SDMemCmp(const void *a,
         const void *b,
         SDUInt32    length);








#define SDCLAMP(value, min, max)                                               \
	do {                                                                       \
		if((value) < (min))                                                    \
		{                                                                      \
			(value) = (min);                                                   \
		}                                                                      \
		else if((value) > (max))                                               \
		{                                                                      \
			(value) = (max);                                                   \
		}                                                                      \
	} while(0)

#define SDMath_PI                           (3.14159265358979323846)
#define SDMath_ToRadians(angle)             (((angle) * SDMath_PI) / 180)
#define SDMath_DotProduct(x1, y1, x2, y2)   (((x1) * (x2)) + ((y1) * (y2)))
#define SDMath_CrossProduct(x1, y1, x2, y2) (((x2) * (y1)) - ((x1) * (y2)))
#define SDMath_Abs(x) ((x) < 0 ? -(x) : (x))

/*\
|*| When appromixating arcs less-than-or-equal-to 90 degrees, the control
|*| points can be calculated thusly:
|*|
|*|  i - the intersetion of the tangents of the end points of the arc
|*|  r - the radius of the arc
|*|  d - the distance from an end point to 'i'
|*|  f - the distance from an end point to 'i', along its tangent,
|*|      where the control point for that end point lies
|*|
|*|  m = (d / r)^2
|*|  n = sqrt(1 + m)
|*|  f = 4 / (3 + 3n)
|*|
|*|
|*| For 90 degree arcs, 'd' equals 'r', therefore:
|*|
|*|  m = 1
|*|  n = sqrt(2)
|*|  f = 4 / (3 + 3*sqrt(2)) ~= 0.552284749830793
|*|
\*/
#define SDMath_Arc90Fraction 0.552284749830793

#define SDDouble_ToFixed(f) ((SDFixed) ((f) * 65536))
#define SDFloat_ToFixed(f)  ((SDFixed) ((f) * 65536))
#define SDFixed_ToDouble(f) (((SDDouble) (f)) / 65536)
#define SDFixed_ToFloat(f)  (((SDFloat) (f)) / 65536)
#define SDFixed_Zero        (0)
#define SDFixed_One         (65536)
#define SDFixed_MinusOne    (-65536)

#define SDColor_FromARGB(a, r, g, b)                                           \
	((((a) << 24) & 0xFF000000) |                                              \
	 (((r) << 16) & 0x00FF0000) |                                              \
     (((g) <<  8) & 0x0000FF00) |                                              \
     (((b) <<  0) & 0x000000FF))
#define SDColor_IntensityR 0.30
#define SDColor_IntensityG 0.59
#define SDColor_IntensityB 0.11
#define SDColor_IntensityRGB(r, g, b)                                          \
	(((r) * SDColor_IntensityR) +                                              \
	 ((g) * SDColor_IntensityG) +                                              \
	 ((b) * SDColor_IntensityB))
#define SDColor_A(color) ((SDByte)((color) >> 24))
#define SDColor_R(color) ((SDByte)((color) >> 16))
#define SDColor_G(color) ((SDByte)((color) >>  8))
#define SDColor_B(color) ((SDByte)((color) >>  0))
#define SDColor_Black    (0xFF000000)
#define SDColor_White    (0xFFFFFFFF)
#define SDColor_Empty    (0x00000000)


/* TODO: use configure-time tests to generate this properly */
#define SDPixmanPixel_FromARGB(a, r, g, b)                                     \
	(((SDByte)((a) << 24)) |                                                   \
	 ((SDByte)((r) << 16)) |                                                   \
	 ((SDByte)((g) <<  8)) |                                                   \
	 ((SDByte)((b) <<  0)))

/* TODO: use configure-time tests to generate this properly */
#define SDPixmanPixel_ToARGB(pixel, a, r, g, b)                                \
	do {                                                                       \
		(a) = ((SDByte)((pixel) >> 24));                                       \
		(r) = ((SDByte)((pixel) >> 16));                                       \
		(g) = ((SDByte)((pixel) >>  8));                                       \
		(b) = ((SDByte)((pixel) >>  0));                                       \
	} while(0)

#define SDCombineMode_Default(combineMode)                                     \
	do {                                                                       \
		if((combineMode) < SDCombineMode_Replace ||                            \
		   (combineMode) > SDCombineMode_Complement)                           \
		{                                                                      \
			(combineMode) = SDCombineMode_Replace;                             \
		}                                                                      \
	} while(0)

#define SDPixelFormat_IsGdi(pixelFormat) \
	(((pixelFormat) & SDPixelFormat_Gdi) != 0)
#define SDPixelFormat_IsExtended(pixelFormat) \
	(((pixelFormat) & SDPixelFormat_Extended) != 0)
#define SDPixelFormat_IsCanonical(pixelFormat) \
	(((pixelFormat) & SDPixelFormat_Canonical) != 0)
#define SDPixelFormat_IsIndexed(pixelFormat) \
	(((pixelFormat) & SDPixelFormat_Indexed) != 0)
#define SDPixelFormat_HasAlpha(pixelFormat) \
	(((pixelFormat) & SDPixelFormat_Alpha) != 0)
#define SDPixelFormat_HasPAlpha(pixelFormat) \
	(((pixelFormat) & SDPixelFormat_PAlpha) != 0)
#define SDPixelFormat_BitsPerPixel(pixelFormat) \
	(((pixelFormat) >> 8) & 0xFF)
#define SDPixelFormat_PaletteSize(pixelFormat) \
	(2 << (SDPixelFormat_BitsPerPixel((pixelFormat))))

#define SDRectangle_ContainsPoint(r, p) \
	((SDPoint_X(p) >= (SDRectangle_X(r))) && \
	 (SDPoint_X(p) <= (SDRectangle_X(r) + SDRectangle_Width(r))) && \
	 (SDPoint_Y(p) >= (SDRectangle_Y(r))) && \
	 (SDPoint_Y(p) <= (SDRectangle_Y(v) + SDRectangle_Height(r))))

#define SDAffineTransform_XX(transform) ((transform).m11)
#define SDAffineTransform_XY(transform) ((transform).m12)
#define SDAffineTransform_YX(transform) ((transform).m21)
#define SDAffineTransform_YY(transform) ((transform).m22)
#define SDAffineTransform_DX(transform) ((transform).dx)
#define SDAffineTransform_DY(transform) ((transform).dy)


#define SDPoint_X(point)              ((point).x)
#define SDPoint_Y(point)              ((point).y)
#define SDLine_Point1(line)           ((line).point1)
#define SDLine_Point2(line)           ((line).point2)
#define SDLine_X1(line)               (SDPoint_X(SDLine_Point1(line)))
#define SDLine_Y1(line)               (SDPoint_Y(SDLine_Point1(line)))
#define SDLine_X2(line)               (SDPoint_X(SDLine_Point2(line)))
#define SDLine_Y2(line)               (SDPoint_Y(SDLine_Point2(line)))
#define SDEdge_Line(edge)             ((edge).line)
#define SDEdge_Clockwise(edge)        ((edge).clockwise)
#define SDEdge_CurrentX(edge)         ((edge).currentX)
#define SDEdge_Point1(edge)           (SDLine_Point1(SDEdge_Line(edge)))
#define SDEdge_Point2(edge)           (SDLine_Point2(SDEdge_Line(edge)))
#define SDEdge_X1(edge)               (SDLine_X1(SDEdge_Line(edge)))
#define SDEdge_Y1(edge)               (SDLine_Y1(SDEdge_Line(edge)))
#define SDEdge_X2(edge)               (SDLine_X2(SDEdge_Line(edge)))
#define SDEdge_Y2(edge)               (SDLine_Y2(SDEdge_Line(edge)))
#define SDVector_X(vector)            (SDPoint_X(vector))
#define SDVector_Y(vector)            (SDPoint_Y(vector))
#define SDRectangle_X(rectangle)      ((rectangle).x)
#define SDRectangle_Y(rectangle)      ((rectangle).y)
#define SDRectangle_Width(rectangle)  ((rectangle).width)
#define SDRectangle_Height(rectangle) ((rectangle).height)
#define SDSize_Width(size)            ((size).width)
#define SDSize_Height(size)           ((size).height)
#define SDTrapezoid_Top(trapezoid)    ((trapezoid).top)
#define SDTrapezoid_Bottom(trapezoid) ((trapezoid).bottom)
#define SDTrapezoid_Left(trapezoid)   ((trapezoid).left)
#define SDTrapezoid_Right(trapezoid)  ((trapezoid).right)

#define SDPointArray_Count(array)    ((array).count)
#define SDPointArray_Points(array)   ((array).points)
#define SDPointArray_Point(array, i) ((array).points[(i)])

#define SDTrapezoids_Count(t)      ((t).count)
#define SDTrapezoids_Trapezoids(t) ((t).trapezoids)

#define SDGraphics_DefaultDpiX 96.0f
#define SDGraphics_DefaultDpiY 96.0f

#define SDFiller_TOLERANCE 0.1f

#define SDStatus_Check(status)                                                 \
	do {                                                                       \
		const SDStatus _status_ = (status);                                    \
		if(_status_ != SDStatus_OK)                                            \
		{                                                                      \
			return _status_;                                                   \
		}                                                                      \
	} while(0)
#define SDStatus_Require(cond, status)                                         \
	do {                                                                       \
		if(!(cond))                                                            \
		{                                                                      \
			return (status);                                                   \
		}                                                                      \
	} while(0)

#ifdef __cplusplus
};
#endif

#endif /* _SD_LIBRARYINTERNAL_H_ */
