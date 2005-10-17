/*
 * SDLib.h - Main library header.
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

#ifndef _SD_LIBRARY_H_
#define _SD_LIBRARY_H_

#include "CrayonsConfig.h"

#ifdef HAVE_PIXMAN_H
	#include <pixman.h>
#else
	#error "The pixman library could not be found."
#endif

#ifdef HAVE_X11_XLIB_H
	#include <X11/Xlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*\
|*| NOTE: The SDLIBRARY_* defines/ifdefs here are a hack to get something akin
|*|       to C# region blocks; they serve a purely aesthetic purpose.
\*/

/******************************************************************************/
#define SDLIBRARY_BASIC_TYPES
#ifdef SDLIBRARY_BASIC_TYPES
/* Define the 8-bit numeric types. */
typedef signed char    SDInt8;
typedef unsigned char  SDUInt8;
typedef SDInt8         SDSByte;
typedef SDUInt8        SDChar8;
typedef SDUInt8        SDByte;
typedef SDUInt8        SDBool;

/* Define the 16-bit numeric types. */
typedef short          SDInt16;
typedef unsigned short SDUInt16;
typedef SDUInt16       SDChar16;

/* Define the 32-bit numeric types. */
#if defined(_MSC_VER)
	typedef __int32          SDInt32;
	typedef unsigned __int32 SDUInt32;
#elif defined(SD_SIZEOF_INT) && (SD_SIZEOF_INT == 4)
	typedef int              SDInt32;
	typedef unsigned int     SDUInt32;
#elif defined(SD_SIZEOF_LONG) && (SD_SIZEOF_LONG == 4)
	typedef long             SDInt32;
	typedef unsigned long    SDUInt32;
#else
	#error "The 32-bit integer type for this compiler could not be determined."
#endif
typedef SDUInt32 SDChar32;
typedef SDUInt32 SDColor;
typedef SDUInt32 SDLanguageID;
typedef SDUInt32 SDPropertyID;
typedef SDUInt32 SDGraphicsContainer;

/* Define the 64-bit numeric types. */
#if defined(_MSC_VER)
	typedef __int64            SDInt64;
	typedef unsigned __int64   SDUInt64;
#elif defined(SD_SIZEOF_LONG) && (SD_SIZEOF_LONG == 8)
	typedef long               SDInt64;
	typedef unsigned long      SDUInt64;
#elif defined(SD_SIZEOF_LONG_LONG) && (SD_SIZEOF_LONG_LONG == 8)
	typedef long long          SDInt64;
	typedef unsigned long long SDUInt64;
#else
	#error "The 64-bit integer type for this compiler could not be determined."
#endif

/* Define the floating point types */
typedef float  SDFloat;
typedef double SDDouble;
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_OPAQUE_TYPES
#ifdef SDLIBRARY_OPAQUE_TYPES
/* Define opaque types. */
typedef struct _tagSDAdjustableArrowCap SDAdjustableArrowCap;
typedef struct _tagSDImage              SDBitmap;
typedef struct _tagSDBitmapSurface      SDBitmapSurface;
typedef struct _tagSDBrush              SDBrush;
typedef struct _tagSDCustomLineCap      SDCustomLineCap;
typedef struct _tagSDFont               SDFont;
typedef struct _tagSDFontFamily         SDFontFamily;
typedef struct _tagSDFontCollection     SDFontCollection;
typedef struct _tagSDGraphics           SDGraphics;
typedef struct _tagSDHatchBrush         SDHatchBrush;
typedef struct _tagSDImage              SDImage;
typedef struct _tagSDImageAttributes    SDImageAttributes;
typedef struct _tagSDLineBrush          SDLineBrush;
typedef struct _tagSDMatrix             SDMatrix;
typedef struct _tagSDPath               SDPath;
typedef struct _tagSDPathBrush          SDPathBrush;
typedef struct _tagSDPathIterator       SDPathIterator;
typedef struct _tagSDPen                SDPen;
typedef struct _tagSDRegion             SDRegion;
typedef struct _tagSDSolidBrush         SDSolidBrush;
typedef struct _tagSDStringFormat       SDStringFormat;
typedef struct _tagSDSurface            SDSurface;
typedef struct _tagSDTextureBrush       SDTextureBrush;
typedef struct _tagSDX11Surface         SDX11Surface;
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_ENUMERATIONS
#ifdef SDLIBRARY_ENUMERATIONS
/* Define enumeration types. */
typedef SDUInt32 SDBrushType;
#define SDBrushType_SolidFill      0
#define SDBrushType_HatchFill      1
#define SDBrushType_TextureFill    2
#define SDBrushType_PathGradient   3
#define SDBrushType_LinearGradient 4

typedef SDUInt32 SDColorAdjustType;
#define SDColorAdjustType_Default   0
#define SDColorAdjustType_Bitmap    1
#define SDColorAdjustType_Brush     2
#define SDColorAdjustType_Pen       3
#define SDColorAdjustType_Text      4
#define SDColorAdjustType_Count     5
#define SDColorAdjustType_Any       6

typedef SDUInt32 SDColorChannelFlag;
#define SDColorChannelFlag_C    0
#define SDColorChannelFlag_M    1
#define SDColorChannelFlag_Y    2
#define SDColorChannelFlag_K    3
#define SDColorChannelFlag_Last 4

typedef SDUInt32 SDCombineMode;
#define SDCombineMode_Replace    0
#define SDCombineMode_Intersect  1
#define SDCombineMode_Union      2
#define SDCombineMode_Xor        3
#define SDCombineMode_Exclude    4
#define SDCombineMode_Complement 5

typedef SDUInt32 SDColorMatrixFlag;
#define SDColorMatrixFlag_Default   0
#define SDColorMatrixFlag_SkipGrays 1
#define SDColorMatrixFlag_AltGray   2

typedef SDUInt32 SDCompositingMode;
#define SDCompositingMode_SourceOver 0
#define SDCompositingMode_SourceCopy 1
#define SDCompositingMode_Xor        2

typedef SDUInt32 SDCompositingQuality;
#define SDCompositingQuality_Default        0
#define SDCompositingQuality_HighSpeed      1
#define SDCompositingQuality_HighQuality    2
#define SDCompositingQuality_GammaCorrected 3
#define SDCompositingQuality_AssumeLinear   4

typedef SDUInt32 SDCoordinateSpace;
#define SDCoordinateSpace_World  0
#define SDCoordinateSpace_Page   1
#define SDCoordinateSpace_Device 2

typedef SDUInt32 SDDashCap;
#define SDDashCap_Flat     0
#define SDDashCap_Round    2
#define SDDashCap_Triangle 3

typedef SDUInt32 SDDashStyle;
#define SDDashStyle_Solid       0
#define SDDashStyle_Dot         1
#define SDDashStyle_Dash        2
#define SDDashStyle_DashDot     3
#define SDDashStyle_DashDashDot 4
#define SDDashStyle_Custom      5

typedef SDUInt32 SDFillMode;
#define SDFillMode_Alternate 0
#define SDFillMode_Winding   1

typedef SDUInt32 SDFlushIntention;
#define SDFlushIntention_Flush 0
#define SDFlushIntention_Sync  1

typedef SDUInt32 SDFontStyle;
#define SDFontStyle_Regular   0
#define SDFontStyle_Bold      1
#define SDFontStyle_Italic    2
#define SDFontStyle_Underline 4
#define SDFontStyle_Strikeout 8

typedef SDUInt32 SDGraphicsUnit;
#define SDGraphicsUnit_World      0
#define SDGraphicsUnit_Display    1
#define SDGraphicsUnit_Pixel      2
#define SDGraphicsUnit_Point      3
#define SDGraphicsUnit_Inch       4
#define SDGraphicsUnit_Document   5
#define SDGraphicsUnit_Millimeter 6

typedef SDUInt32 SDHatchStyle;
#define SDHatchStyle_Horizontal             0
#define SDHatchStyle_Vertical               1
#define SDHatchStyle_ForwardDiagonal        2
#define SDHatchStyle_BackwardDiagonal       3
#define SDHatchStyle_Cross                  4
#define SDHatchStyle_LargeGrid              SDHatchStyle_Cross
#define SDHatchStyle_DiagonalCross          5
#define SDHatchStyle_Percent05              6
#define SDHatchStyle_Percent10              7
#define SDHatchStyle_Percent20              8
#define SDHatchStyle_Percent25              9
#define SDHatchStyle_Percent30              10
#define SDHatchStyle_Percent40              11
#define SDHatchStyle_Percent50              12
#define SDHatchStyle_Percent60              13
#define SDHatchStyle_Percent70              14
#define SDHatchStyle_Percent75              15
#define SDHatchStyle_Percent80              16
#define SDHatchStyle_Percent90              17
#define SDHatchStyle_LightDownwardDiagonal  18
#define SDHatchStyle_LightUpwardDiagonal    19
#define SDHatchStyle_DarkDownwardDiagonal   20
#define SDHatchStyle_DarkUpwardDiagonal     21
#define SDHatchStyle_WideDownwardDiagonal   22
#define SDHatchStyle_WideUpwardDiagonal     23
#define SDHatchStyle_LightVertical          24
#define SDHatchStyle_LightHorizontal        25
#define SDHatchStyle_NarrowVertical         26
#define SDHatchStyle_NarrowHorizontal       27
#define SDHatchStyle_DarkVertical           28
#define SDHatchStyle_DarkHorizontal         29
#define SDHatchStyle_DashedDownwardDiagonal 30
#define SDHatchStyle_DashedUpwardDiagonal   31
#define SDHatchStyle_DashedHorizontal       32
#define SDHatchStyle_DashedVertical         33
#define SDHatchStyle_SmallConfetti          34
#define SDHatchStyle_LargeConfetti          35
#define SDHatchStyle_ZigZag                 36
#define SDHatchStyle_Wave                   37
#define SDHatchStyle_DiagonalBrick          38
#define SDHatchStyle_HorizontalBrick        39
#define SDHatchStyle_Weave                  40
#define SDHatchStyle_Plaid                  41
#define SDHatchStyle_Divot                  42
#define SDHatchStyle_DottedGrid             43
#define SDHatchStyle_DottedDiamond          44
#define SDHatchStyle_Shingle                45
#define SDHatchStyle_Trellis                46
#define SDHatchStyle_Sphere                 47
#define SDHatchStyle_SmallGrid              48
#define SDHatchStyle_SmallCheckerBoard      49
#define SDHatchStyle_LargeCheckerBoard      50
#define SDHatchStyle_OutlinedDiamond        51
#define SDHatchStyle_SolidDiamond           52
#define SDHatchStyle_Min                    SDHatchStyle_Horizontal
#define SDHatchStyle_Max                    SDHatchStyle_SolidDiamond

typedef SDUInt32 SDHotkeyPrefix;
#define SDHotkeyPrefix_None 0
#define SDHotkeyPrefix_Show 1
#define SDHotkeyPrefix_Hide 2

typedef SDUInt32 SDImageFlag;
#define SDImageFlag_None              0x00000000
#define SDImageFlag_Scalable          0x00000001
#define SDImageFlag_HasAlpha          0x00000002
#define SDImageFlag_HasTranslucent    0x00000004
#define SDImageFlag_PartiallyScalable 0x00000008
#define SDImageFlag_ColorSpaceRGB     0x00000010
#define SDImageFlag_ColorSpaceCMYK    0x00000020
#define SDImageFlag_ColorSpaceGRAY    0x00000040
#define SDImageFlag_ColorSpaceYCBCR   0x00000080
#define SDImageFlag_ColorSpaceYCCK    0x00000100
#define SDImageFlag_HasRealDPI        0x00001000
#define SDImageFlag_HasRealPixelSize  0x00002000
#define SDImageFlag_ReadOnly          0x00010000
#define SDImageFlag_Caching           0x00020000

typedef SDUInt32 SDImageLockMode;
#define SDImageLockMode_None      0
#define SDImageLockMode_ReadOnly  1
#define SDImageLockMode_WriteOnly 2
#define SDImageLockMode_ReadWrite 3

typedef SDUInt32 SDImageType;
#define SDImageType_Unknown  0
#define SDImageType_Bitmap   1
#define SDImageType_Metafile 2

typedef SDUInt32 SDInterpolationMode;
#define SDInterpolationMode_Default             0
#define SDInterpolationMode_LowQuality          1
#define SDInterpolationMode_HighQuality         2
#define SDInterpolationMode_Bilinear            3
#define SDInterpolationMode_Bicubic             4
#define SDInterpolationMode_NearestNeighbor     5
#define SDInterpolationMode_HighQualityBilinear 6
#define SDInterpolationMode_HighQualityBicubic  7

typedef SDUInt32 SDLineCap;
#define SDLineCap_Flat          0x00
#define SDLineCap_Square        0x01
#define SDLineCap_Round         0x02
#define SDLineCap_Triangle      0x03
#define SDLineCap_NoAnchor      0x10
#define SDLineCap_SquareAnchor  0x11
#define SDLineCap_RoundAnchor   0x12
#define SDLineCap_DiamondAnchor 0x13
#define SDLineCap_ArrowAnchor   0x14
#define SDLineCap_AnchorMask    0xF0
#define SDLineCap_Custom        0xFF

typedef SDUInt32 SDLineJoin;
#define SDLineJoin_Miter        0
#define SDLineJoin_Bevel        1
#define SDLineJoin_Round        2
#define SDLineJoin_MiterClipped 3

typedef SDUInt32 SDMatrixOrder;
#define SDMatrixOrder_Prepend 0
#define SDMatrixOrder_Append  1

typedef SDUInt32 SDPaletteFlag;
#define SDPaletteFlag_HasAlpha  1
#define SDPaletteFlag_GrayScale 2
#define SDPaletteFlag_Halftone  4

typedef SDUInt32 SDPathType;
#define SDPathType_Start        0x00
#define SDPathType_Line         0x01
#define SDPathType_Bezier       0x03
#define SDPathType_TypeMask     0x07
#define SDPathType_DashMode     0x10
#define SDPathType_PathMarker   0x20
#define SDPathType_CloseSubpath 0x80

typedef SDUInt32 SDPenAlignment;
#define SDPenAlignment_Center 0
#define SDPenAlignment_Inset  1
#define SDPenAlignment_Outset 2
#define SDPenAlignment_Left   3
#define SDPenAlignment_Right  4

typedef SDUInt32 SDPenType;
#define SDPenType_SolidFill      SDBrushType_SolidFill
#define SDPenType_HatchFill      SDBrushType_HatchFill
#define SDPenType_TextureFill    SDBrushType_TextureFill
#define SDPenType_PathGradient   SDBrushType_PathGradient
#define SDPenType_LinearGradient SDBrushType_LinearGradient

/*\
|*| SDPixelFormat format:
|*|
|*|   byte 0 = index
|*|   byte 1 = bits per pixel
|*|   byte 2 = flags
|*|   byte 3 = reserved
\*/
typedef SDUInt32 SDPixelFormat;
#define SDPixelFormat_Undefined      0x00000000
#define SDPixelFormat_DontCare       0x00000000
#define SDPixelFormat_Indexed        0x00010000
#define SDPixelFormat_Gdi            0x00020000
#define SDPixelFormat_Alpha          0x00040000
#define SDPixelFormat_PAlpha         0x00080000
#define SDPixelFormat_Extended       0x00100000
#define SDPixelFormat_Canonical      0x00200000
#define SDPixelFormat_1bppIndexed    0x00030101
#define SDPixelFormat_4bppIndexed    0x00030402
#define SDPixelFormat_8bppIndexed    0x00030803
#define SDPixelFormat_16bppGrayScale 0x00101004
#define SDPixelFormat_16bppRgb555    0x00021005
#define SDPixelFormat_16bppRgb565    0x00021006
#define SDPixelFormat_16bppArgb1555  0x00061007
#define SDPixelFormat_24bppRgb       0x00021808
#define SDPixelFormat_32bppRgb       0x00022009
#define SDPixelFormat_32bppArgb      0x0026200A
#define SDPixelFormat_32bppPArgb     0x000E200B
#define SDPixelFormat_48bppRgb       0x0010300C
#define SDPixelFormat_64bppArgb      0x0034400D
#define SDPixelFormat_64bppPArgb     0x001C400E
#define SDPixelFormat_Max            0x0000000F

typedef SDUInt32 SDPixelOffsetMode;
#define SDPixelOffsetMode_Default     0
#define SDPixelOffsetMode_HighSpeed   1
#define SDPixelOffsetMode_HighQuality 2
#define SDPixelOffsetMode_None        3
#define SDPixelOffsetMode_Half        4

typedef SDUInt32 SDRotateFlipType;
#define SDRotateFlipType_RotateNoneFlipNone 0
#define SDRotateFlipType_Rotate90FlipNone   1
#define SDRotateFlipType_Rotate180FlipNone  2
#define SDRotateFlipType_Rotate270FlipNone  3
#define SDRotateFlipType_RotateNoneFlipX    4
#define SDRotateFlipType_Rotate90FlipX      5
#define SDRotateFlipType_Rotate180FlipX     6
#define SDRotateFlipType_Rotate270FlipX     7
#define SDRotateFlipType_Rotate180FlipXY    SDRotateFlipType_RotateNoneFlipNone
#define SDRotateFlipType_Rotate270FlipXY    SDRotateFlipType_Rotate90FlipNone
#define SDRotateFlipType_RotateNoneFlipXY   SDRotateFlipType_Rotate180FlipNone
#define SDRotateFlipType_Rotate90FlipXY     SDRotateFlipType_Rotate270FlipNone
#define SDRotateFlipType_Rotate180FlipY     SDRotateFlipType_RotateNoneFlipX
#define SDRotateFlipType_Rotate270FlipY     SDRotateFlipType_Rotate90FlipX
#define SDRotateFlipType_RotateNoneFlipY    SDRotateFlipType_Rotate180FlipX
#define SDRotateFlipType_Rotate90FlipY      SDRotateFlipType_Rotate270FlipX

typedef SDUInt32 SDSmoothingMode;
#define SDSmoothingMode_Default     0
#define SDSmoothingMode_HighSpeed   1
#define SDSmoothingMode_HighQuality 2
#define SDSmoothingMode_None        3
#define SDSmoothingMode_AntiAlias   4

/*\
|*| SDStatus format:
|*|
|*|    lower half = exception
|*|   higher half = message (optional)
\*/
typedef SDUInt32 SDStatus;
#define SDStatus_OK                              0x00000000
#define SDStatus_OutOfMemory                     0x00000001
#define SDStatus_Argument                        0x00000002
#define SDStatus_ArgumentNull                    0x00000003
#define SDStatus_ArgumentOutOfRange              0x00000004
#define SDStatus_InvalidOperation                0x00000005
#define SDStatus_NotImplemented                  0x00000006
#define SDStatus_NotSupported                    0x00000007
#define SDStatus_Error                           0x00000008
#define SDStatus_Argument_FontFamilyNotFound     0x00010002
#define SDStatus_Argument_InvalidPointCount      0x00020002
#define SDStatus_Argument_NeedAtLeast2Points     0x00030002
#define SDStatus_Argument_NeedAtLeast3Points     0x00040002
#define SDStatus_Argument_NeedAtLeast4Points     0x00050002
#define SDStatus_InvalidOperation_ImageLocked    0x00010005
#define SDStatus_InvalidOperation_SingularMatrix 0x00020005

typedef SDUInt32 SDStringAlignment;
#define SDStringAlignment_Near   0
#define SDStringAlignment_Center 1
#define SDStringAlignment_Far    2

typedef SDUInt32 SDDigitSubstitute;
#define SDDigitSubstitute_User        0
#define SDDigitSubstitute_None        1
#define SDDigitSubstitute_National    2
#define SDDigitSubstitute_Traditional 3

typedef SDUInt32 SDStringFormatFlag;
#define SDStringFormatFlag_DirectionRightToLeft  0x0001
#define SDStringFormatFlag_DirectionVertical     0x0002
#define SDStringFormatFlag_NoFitBlackBox         0x0004
#define SDStringFormatFlag_DisplayFormatControl  0x0020
#define SDStringFormatFlag_NoFontFallback        0x0400
#define SDStringFormatFlag_MeasureTrailingSpaces 0x0800
#define SDStringFormatFlag_NoWrap                0x1000
#define SDStringFormatFlag_LineLimit             0x2000
#define SDStringFormatFlag_NoClip                0x4000

typedef SDUInt32 SDStringTrimming;
#define SDStringTrimming_None              0
#define SDStringTrimming_Character         1
#define SDStringTrimming_Word              2
#define SDStringTrimming_EllipsisCharacter 3
#define SDStringTrimming_EllipsisWord      4
#define SDStringTrimming_EllipsisPath      5

typedef SDUInt32 SDTextRenderingHint;
#define SDTextRenderingHint_SystemDefault            0
#define SDTextRenderingHint_SingleBitPerPixelGridFit 1
#define SDTextRenderingHint_SingleBitPerPixel        2
#define SDTextRenderingHint_AntiAliasGridFit         3
#define SDTextRenderingHint_AntiAlias                4
#define SDTextRenderingHint_ClearTypeGridFit         5

typedef SDUInt32 SDWarpMode;
#define SDWarpMode_Perspective 0
#define SDWarpMode_Bilinear    1

typedef SDUInt32 SDWrapMode;
#define SDWrapMode_Tile       0
#define SDWrapMode_TileFlipX  1
#define SDWrapMode_TileFlipY  2
#define SDWrapMode_TileFlipXY 3
#define SDWrapMode_Clamp      4
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_TRANSPARENT_TYPES
#ifdef SDLIBRARY_TRANSPARENT_TYPES
/* Define transparent types. */
typedef struct _tagSDBitmapData SDBitmapData;
struct _tagSDBitmapData
{
	SDUInt32       width;
	SDUInt32       height;
	SDUInt32       stride;
	SDPixelFormat  pixelFormat;
	SDByte        *scan0;
	SDUInt32       reserved;
};

typedef struct _tagSDBlend SDBlend;
struct _tagSDBlend
{
	SDFloat  *factors;
	SDFloat  *positions;
	SDUInt32  count;
};

typedef struct _tagSDCharacterRange SDCharacterRange;
struct _tagSDCharacterRange
{
	SDUInt32 first;
	SDUInt32 length;
};

typedef struct _tagSDColorBlend SDColorBlend;
struct _tagSDColorBlend
{
	SDColor  *colors;
	SDFloat  *positions;
	SDUInt32  count;
};

typedef struct _tagSDColorMap SDColorMap;
struct _tagSDColorMap
{
	SDColor oldColor;
	SDColor newColor;
};

typedef struct _tagSDColorMatrix SDColorMatrix;
struct _tagSDColorMatrix
{
	SDFloat m[5][5];
};

typedef struct _tagSDColorPalette SDColorPalette;
struct _tagSDColorPalette
{
	SDPaletteFlag  flags;
	SDUInt32       count;
	SDColor       *colors;
};

typedef struct _tagSDFontMetricsI SDFontMetricsI;
struct _tagSDFontMetricsI
{
	SDInt32 cellAscent;
	SDInt32 cellDescent;
	SDInt32 lineSpacing;
	SDInt32 emHeight;
};

typedef struct _tagSDFontMetricsF SDFontMetricsF;
struct _tagSDFontMetricsF
{
	SDFloat cellAscent;
	SDFloat cellDescent;
	SDFloat lineSpacing;
	SDFloat emHeight;
};

typedef struct _tagSDGuid SDGuid;
struct _tagSDGuid
{
	SDUInt32 a;
	SDUInt16 b;
	SDUInt16 c;
	SDByte   d;
	SDByte   e;
	SDByte   f;
	SDByte   g;
	SDByte   h;
	SDByte   i;
	SDByte   j;
	SDByte   k;
};

typedef SDGuid SDImageFormat;

typedef struct _tagSDPointI SDPointI;
struct _tagSDPointI
{
	SDInt32 x;
	SDInt32 y;
};

typedef struct _tagSDPointF SDPointF;
struct _tagSDPointF
{
	SDFloat x;
	SDFloat y;
};

typedef struct _tagSDPropertyItem SDPropertyItem;
struct _tagSDPropertyItem
{
	SDPropertyID  id;
	SDUInt32      length;
	SDUInt16      type;
	SDByte       *value;
};

typedef struct _tagSDRectangleF SDRectangleF;
struct _tagSDRectangleF
{
	SDFloat x;
	SDFloat y;
	SDFloat width;
	SDFloat height;
};

typedef struct _tagSDSizeF SDSizeF;
struct _tagSDSizeF
{
	SDFloat width;
	SDFloat height;
};

#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_CONSTANTS
#ifdef SDLIBRARY_CONSTANTS
/* Declare various constants. */
extern const SDGuid SDFrameDimension_Page;
extern const SDGuid SDFrameDimension_Resolution;
extern const SDGuid SDFrameDimension_Time;
extern const SDGuid SDImageFormat_MemoryBMP;
extern const SDGuid SDImageFormat_BMP;
extern const SDGuid SDImageFormat_EMF;
extern const SDGuid SDImageFormat_WMF;
extern const SDGuid SDImageFormat_JPG;
extern const SDGuid SDImageFormat_PNG;
extern const SDGuid SDImageFormat_GIF;
extern const SDGuid SDImageFormat_TIFF;
extern const SDGuid SDImageFormat_EXIF;
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_BITMAP_METHODS
#ifdef SDLIBRARY_BITMAP_METHODS
/* Declare public bitmap methods. */
SDStatus
SDBitmap_Create(SDBitmap      **_this,
                SDUInt32        width,
                SDUInt32        height,
                SDPixelFormat   format);
SDStatus
SDBitmap_CreateData(SDBitmap      **_this,
                    SDByte         *data,
                    SDUInt32        width,
                    SDUInt32        height,
                    SDUInt32        stride,
                    SDPixelFormat   format);
SDStatus
SDBitmap_Destroy(SDBitmap **_this);
SDStatus
SDBitmap_Clone(SDBitmap       *_this,
               SDBitmap      **clone,
               SDUInt32        x,
               SDUInt32        y,
               SDUInt32        width,
               SDUInt32        height,
               SDPixelFormat   format);
SDStatus
SDBitmap_GetPixel(SDBitmap *_this,
                  SDUInt32  x,
                  SDUInt32  y,
                  SDColor  *color);
SDStatus
SDBitmap_SetPixel(SDBitmap *_this,
                  SDUInt32  x,
                  SDUInt32  y,
                  SDColor   color);
SDStatus
SDBitmap_LockBits(SDBitmap        *_this,
                  SDUInt32         x,
                  SDUInt32         y,
                  SDUInt32         width,
                  SDUInt32         height,
                  SDImageLockMode  lockMode,
                  SDPixelFormat    format,
                  SDBitmapData    *bitmapData);
SDStatus
SDBitmap_SetResolution(SDBitmap *_this,
                       SDFloat   dpiX,
                       SDFloat   dpiY);
SDStatus
SDBitmap_UnlockBits(SDBitmap     *_this,
                    SDBitmapData *bitmapData);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_BRUSH_METHODS
#ifdef SDLIBRARY_BRUSH_METHODS
/* Declare public brush methods. */
SDStatus
SDBrush_GetBrushType(SDBrush     *_this,
                     SDBrushType *type);
SDStatus
SDBrush_Clone(SDBrush  *_this,
              SDBrush **clone);
SDStatus
SDBrush_Destroy(SDBrush **_this);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_COLORPALETTE_METHODS
#ifdef SDLIBRARY_COLORPALETTE_METHODS
/* Declare public color palette methods. */
SDStatus
SDColorPalette_Create(SDColorPalette **_this,
                      SDColor         *colors,
                      SDUInt32         count,
                      SDPaletteFlag    flags);
SDStatus
SDColorPalette_Destroy(SDColorPalette **_this);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_GRAPHICS_METHODS
#ifdef SDLIBRARY_GRAPHICS_METHODS
/* Declare public graphics methods. */
SDStatus
SDGraphics_Create(SDGraphics **_this,
                  SDSurface   *surface);
SDStatus
SDGraphics_Destroy(SDGraphics **_this);
SDStatus
SDGraphics_GetTransform(SDGraphics *_this,
                        SDMatrix   *matrix);
SDStatus
SDGraphics_MultiplyTransform(SDGraphics    *_this,
                             SDMatrix      *matrix,
                             SDMatrixOrder  order);
SDStatus
SDGraphics_ResetTransform(SDGraphics *_this);
SDStatus
SDGraphics_RotateTransform(SDGraphics    *_this,
                           SDFloat        angle,
                           SDMatrixOrder  order);
SDStatus
SDGraphics_ScaleTransform(SDGraphics    *_this,
                          SDFloat        sx,
                          SDFloat        sy,
                          SDMatrixOrder  order);
SDStatus
SDGraphics_SetTransform(SDGraphics *_this,
                        SDMatrix   *matrix);
SDStatus
SDGraphics_TranslateTransform(SDGraphics    *_this,
                              SDFloat        dx,
                              SDFloat        dy,
                              SDMatrixOrder  order);
SDStatus
SDGraphics_GetClip(SDGraphics *_this,
                   SDRegion   *region);
SDStatus
SDGraphics_GetClipBounds(SDGraphics   *_this,
                         SDRectangleF *clipBounds);
SDStatus
SDGraphics_GetVisibleClipBounds(SDGraphics   *_this,
                                SDRectangleF *bounds);
SDStatus
SDGraphics_IsClipEmpty(SDGraphics *_this,
                       SDBool     *empty);
SDStatus
SDGraphics_IsVisibleClipEmpty(SDGraphics *_this,
                              SDBool     *empty);
SDStatus
SDGraphics_IsVisiblePoint(SDGraphics *_this,
                          SDPointF    point,
                          SDBool     *visible);
SDStatus
SDGraphics_IsVisibleRectangle(SDGraphics   *_this,
                              SDRectangleF  rect,
                              SDBool       *visible);
SDStatus
SDGraphics_ResetClip(SDGraphics *_this);
SDStatus
SDGraphics_SetClipGraphics(SDGraphics    *_this,
                           SDGraphics    *graphics,
                           SDCombineMode  combineMode);
SDStatus
SDGraphics_SetClipPath(SDGraphics    *_this,
                       SDPath        *path,
                       SDCombineMode  combineMode);
SDStatus
SDGraphics_SetClipRegion(SDGraphics    *_this,
                         SDRegion      *region,
                         SDCombineMode  combineMode);
SDStatus
SDGraphics_SetClipRectangle(SDGraphics    *_this,
                            SDRectangleF   rect,
                            SDCombineMode  combineMode);
SDStatus
SDGraphics_TranslateClip(SDGraphics *_this,
                         SDFloat     dx,
                         SDFloat     dy);
SDStatus
SDGraphics_GetCompositingMode(SDGraphics        *_this,
                              SDCompositingMode *compositingMode);
SDStatus
SDGraphics_SetCompositingMode(SDGraphics        *_this,
                              SDCompositingMode  compositingMode);
SDStatus
SDGraphics_GetCompositingQuality(SDGraphics           *_this,
                                 SDCompositingQuality *compositingQuality);
SDStatus
SDGraphics_SetCompositingQuality(SDGraphics           *_this,
                                 SDCompositingQuality  compositingQuality);
SDStatus
SDGraphics_GetDpiX(SDGraphics *_this,
                   SDFloat    *dpiX);
SDStatus
SDGraphics_GetDpiY(SDGraphics *_this,
                   SDFloat    *dpiY);
SDStatus
SDGraphics_GetInterpolationMode(SDGraphics          *_this,
                                SDInterpolationMode *interpolationMode);
SDStatus
SDGraphics_SetInterpolationMode(SDGraphics          *_this,
                                SDInterpolationMode  interpolationMode);
SDStatus
SDGraphics_GetPageScale(SDGraphics *_this,
                        SDFloat    *pageScale);
SDStatus
SDGraphics_SetPageScale(SDGraphics *_this,
                        SDFloat     pageScale);
SDStatus
SDGraphics_GetPageUnit(SDGraphics     *_this,
                       SDGraphicsUnit *pageUnit);
SDStatus
SDGraphics_SetPageUnit(SDGraphics     *_this,
                       SDGraphicsUnit  pageUnit);
SDStatus
SDGraphics_GetPixelOffsetMode(SDGraphics        *_this,
                              SDPixelOffsetMode *pixelOffsetMode);
SDStatus
SDGraphics_SetPixelOffsetMode(SDGraphics        *_this,
                              SDPixelOffsetMode  pixelOffsetMode);
SDStatus
SDGraphics_GetRenderingOrigin(SDGraphics *_this,
                              SDPointI   *renderingOrigin);
SDStatus
SDGraphics_SetRenderingOrigin(SDGraphics *_this,
                              SDPointI    renderingOrigin);
SDStatus
SDGraphics_GetSmoothingMode(SDGraphics      *_this,
                            SDSmoothingMode *smoothingMode);
SDStatus
SDGraphics_SetSmoothingMode(SDGraphics      *_this,
                            SDSmoothingMode  smoothingMode);
SDStatus
SDGraphics_GetTextContrast(SDGraphics *_this,
                           SDUInt32   *textContrast);
SDStatus
SDGraphics_SetTextContrast(SDGraphics *_this,
                           SDUInt32    textContrast);
SDStatus
SDGraphics_GetTextRenderingHint(SDGraphics          *_this,
                                SDTextRenderingHint *textRenderingHint);
SDStatus
SDGraphics_SetTextRenderingHint(SDGraphics          *_this,
                                SDTextRenderingHint  textRenderingHint);
SDStatus
SDGraphics_DrawXBM(SDGraphics   *_this,
                   SDByte       *bits,
                   SDUInt32      count,
                   SDRectangleF  dst,
                   SDColor       color);
SDStatus
SDGraphics_DrawImage(SDGraphics *_this,
                     SDImage    *image,
                     SDFloat     x,
                     SDFloat     y);
SDStatus
SDGraphics_DrawImageRect(SDGraphics *_this,
                         SDImage    *image,
                         SDFloat     x,
                         SDFloat     y,
                         SDFloat     width,
                         SDFloat     height);
SDStatus
SDGraphics_DrawImagePoints(SDGraphics *_this,
                           SDImage    *image,
                           SDPointF   *dst,
                           SDUInt32    count);
SDStatus
SDGraphics_DrawImageRectPoints(SDGraphics     *_this,
                               SDImage        *image,
                               SDPointF        dst,
                               SDRectangleF    src,
                               SDGraphicsUnit  srcUnit);
SDStatus
SDGraphics_DrawImageRectPointsAttributes(SDGraphics           *_this,
                                         SDImage              *image,
                                         SDPointF             *dst,
                                         SDUInt32              count,
                                         SDRectangleF          src,
                                         SDGraphicsUnit        srcUnit,
                                         SDImageAttributes    *atts);
SDStatus
SDGraphics_DrawImageRectRectAttributes(SDGraphics           *_this,
                                       SDImage              *image,
                                       SDRectangleF          dst,
                                       SDRectangleF          src,
                                       SDGraphicsUnit        srcUnit,
                                       SDImageAttributes    *atts);
SDStatus
SDGraphics_DrawArc(SDGraphics   *_this,
                   SDPen        *pen,
                   SDRectangleF  rect,
                   SDFloat       startAngle,
                   SDFloat       sweepAngle);
SDStatus
SDGraphics_DrawBezier(SDGraphics *_this,
                      SDPen      *pen,
                      SDPointF    a,
                      SDPointF    b,
                      SDPointF    c,
                      SDPointF    d);
SDStatus
SDGraphics_DrawBeziers(SDGraphics *_this,
                       SDPen      *pen,
                       SDPointF   *points,
                       SDUInt32    count);
SDStatus
SDGraphics_DrawClosedCurve(SDGraphics *_this,
                           SDPen      *pen,
                           SDPointF   *points,
                           SDUInt32    count,
                           SDFloat     tension);
SDStatus
SDGraphics_DrawCurve(SDGraphics *_this,
                     SDPen      *pen,
                     SDPointF   *points,
                     SDUInt32    count,
                     SDUInt32    offset,
                     SDUInt32    numberOfSegments,
                     SDFloat     tension);
SDStatus
SDGraphics_DrawEllipse(SDGraphics   *_this,
                       SDPen        *pen,
                       SDRectangleF  rect);
SDStatus
SDGraphics_DrawLine(SDGraphics *_this,
                    SDPen      *pen,
                    SDPointF    start,
                    SDPointF    end);
SDStatus
SDGraphics_DrawLines(SDGraphics *_this,
                     SDPen      *pen,
                     SDPointF   *points,
                     SDUInt32    count);
SDStatus
SDGraphics_DrawPath(SDGraphics *_this,
                    SDPen      *pen,
                    SDPath     *path);
SDStatus
SDGraphics_DrawPie(SDGraphics   *_this,
                   SDPen        *pen,
                   SDRectangleF  rect,
                   SDFloat       startAngle,
                   SDFloat       sweepAngle);
SDStatus
SDGraphics_DrawPolygon(SDGraphics *_this,
                       SDPen      *pen,
                       SDPointF   *points,
                       SDUInt32    count);
SDStatus
SDGraphics_DrawRectangle(SDGraphics   *_this,
                         SDPen        *pen,
                         SDRectangleF  rect);
SDStatus
SDGraphics_DrawRectangles(SDGraphics   *_this,
                          SDPen        *pen,
                          SDRectangleF *rects,
                          SDUInt32      count);
SDStatus
SDGraphics_Clear(SDGraphics *_this,
                 SDColor     color);
SDStatus
SDGraphics_FillClosedCurve(SDGraphics *_this,
                           SDBrush    *brush,
                           SDPointF   *points,
                           SDUInt32    count,
                           SDFloat     tension,
                           SDFillMode  fillMode);
SDStatus
SDGraphics_FillEllipse(SDGraphics   *_this,
                       SDBrush      *brush,
                       SDRectangleF  rect);
SDStatus
SDGraphics_FillPath(SDGraphics *_this,
                    SDBrush    *brush,
                    SDPath     *path);
SDStatus
SDGraphics_FillPie(SDGraphics   *_this,
                   SDBrush      *brush,
                   SDRectangleF  rect,
                   SDFloat       startAngle,
                   SDFloat       sweepAngle);
SDStatus
SDGraphics_FillPolygon(SDGraphics *_this,
                       SDBrush    *brush,
                       SDPointF   *points,
                       SDUInt32    count,
                       SDFillMode  fillMode);
SDStatus
SDGraphics_FillRectangle(SDGraphics   *_this,
                         SDBrush      *brush,
                         SDRectangleF  rect);
SDStatus
SDGraphics_FillRectangles(SDGraphics   *_this,
                          SDBrush      *brush,
                          SDRectangleF *rects,
                          SDUInt32      count);
SDStatus
SDGraphics_FillRegion(SDGraphics *_this,
                      SDBrush    *brush,
                      SDRegion   *region);
SDStatus
SDGraphics_BeginContainer(SDGraphics          *_this,
                          SDGraphicsContainer *container);
SDStatus
SDGraphics_BeginContainer2(SDGraphics          *_this,
                           SDRectangleF         dst,
                           SDRectangleF         src,
                           SDGraphicsUnit       unit,
                           SDGraphicsContainer *container);
SDStatus
SDGraphics_EndContainer(SDGraphics          *_this,
                        SDGraphicsContainer  container);
SDStatus
SDGraphics_Restore(SDGraphics *_this,
                   SDUInt32    state);
SDStatus
SDGraphics_Save(SDGraphics *_this,
                SDUInt32   *state);
SDStatus
SDGraphics_DrawString(SDGraphics     *_this,
                      SDBrush        *brush,
                      SDChar16       *s,
                      SDUInt32        length,
                      SDFont         *font,
                      SDRectangleF    layoutRect,
                      SDStringFormat *format);
SDStatus
SDGraphics_MeasureCharacterRanges(SDGraphics      *_this,
                                  SDChar16        *s,
                                  SDUInt32         length,
                                  SDFont          *font,
                                  SDRectangleF     layoutRect,
                                  SDStringFormat  *format,
                                  SDRegion       **regions,
                                  SDUInt32        *count);
SDStatus
SDGraphics_MeasureString(SDGraphics     *_this,
                         SDChar16       *s,
                         SDUInt32        length,
                         SDFont         *font,
                         SDRectangleF    layoutRect,
                         SDStringFormat *format,
                         SDUInt32       *charactersFitted,
                         SDUInt32       *linesFilled,
                         SDSizeF        *size);
SDStatus
SDGraphics_Flush(SDGraphics       *_this,
                 SDFlushIntention  intention);
SDStatus
SDGraphics_GetHdc(SDGraphics  *_this,
                  void       **hdc);
SDStatus
SDGraphics_GetNearestColor(SDGraphics *_this,
                           SDColor     color,
                           SDColor    *nearest);
SDStatus
SDGraphics_ReleaseHdc(SDGraphics *_this,
                      void       *hdc);
SDStatus
SDGraphics_TransformPoints(SDGraphics        *_this,
                           SDCoordinateSpace  dst,
                           SDCoordinateSpace  src,
                           SDPointF          *points,
                           SDUInt32           count);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_HATCHBRUSH_METHODS
#ifdef SDLIBRARY_HATCHBRUSH_METHODS
/* Declare public hatch brush methods. */
SDStatus
SDHatchBrush_Create(SDHatchBrush **_this,
                    SDHatchStyle   style,
                    SDColor        foreground,
                    SDColor        background);
SDStatus
SDHatchBrush_GetBackgroundColor(SDHatchBrush *_this,
                                SDColor      *background);
SDStatus
SDHatchBrush_GetForegroundColor(SDHatchBrush *_this,
                                SDColor      *foreground);
SDStatus
SDHatchBrush_GetHatchStyle(SDHatchBrush *_this,
                           SDHatchStyle *style);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_IMAGE_METHODS
#ifdef SDLIBRARY_IMAGE_METHODS
/* Declare public image methods. */
SDStatus
SDImage_Destroy(SDImage **_this);
SDStatus
SDImage_GetFlags(SDImage     *_this,
                 SDImageFlag *flags);
SDStatus
SDImage_GetHeight(SDImage  *_this,
                  SDUInt32 *height);
SDStatus
SDImage_GetHorizontalResolution(SDImage *_this,
                                SDFloat *dpiX);
SDStatus
SDImage_GetImageType(SDImage     *_this,
                     SDImageType *type);
SDStatus
SDImage_GetPhysicalDimension(SDImage *_this,
                             SDSizeF *size);
SDStatus
SDImage_GetPixelFormat(SDImage       *_this,
                       SDPixelFormat *pixelFormat);
SDStatus
SDImage_GetRawFormat(SDImage *_this,
                     SDGuid  *format);
SDStatus
SDImage_GetVerticalResolution(SDImage *_this,
                              SDFloat *dpiY);
SDStatus
SDImage_GetWidth(SDImage  *_this,
                 SDUInt32 *width);
SDStatus
SDImage_Clone(SDImage  *_this,
              SDImage **clone);
SDStatus
SDImage_GetBounds(SDImage        *_this,
                  SDGraphicsUnit  pageUnit,
                  SDRectangleF   *bounds);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_LINEBRUSH_METHODS
#ifdef SDLIBRARY_LINEBRUSH_METHODS
/* Declare public line brush methods. */
SDStatus
SDLineBrush_Create(SDLineBrush  **_this,
                   SDRectangleF   rectangle,
                   SDColor        startColor,
                   SDColor        endColor,
                   SDFloat        angle,
                   SDBool         isAngleScalable,
                   SDWrapMode     wrapMode);
SDStatus
SDLineBrush_GetBlend(SDLineBrush *_this,
                     SDBlend     *blend);
SDStatus
SDLineBrush_SetBlend(SDLineBrush *_this,
                     SDBlend      blend);
SDStatus
SDLineBrush_GetColors(SDLineBrush *_this,
                      SDColor     *startColor,
                      SDColor     *endColor);
SDStatus
SDLineBrush_SetColor(SDLineBrush *_this,
                     SDColor      startColor,
                     SDColor      endColor);
SDStatus
SDLineBrush_GetColorBlend(SDLineBrush  *_this,
                          SDColorBlend *colorBlend);
SDStatus
SDLineBrush_SetColorBlend(SDLineBrush  *_this,
                          SDColorBlend  colorBlend);
SDStatus
SDLineBrush_GetGammaCorrection(SDLineBrush *_this,
                               SDBool      *gammaCorrection);
SDStatus
SDLineBrush_SetGammaCorrection(SDLineBrush *_this,
                               SDBool       gammaCorrection);
SDStatus
SDLineBrush_GetRectangle(SDLineBrush  *_this,
                         SDRectangleF *rectangle);
SDStatus
SDLineBrush_GetWrapMode(SDLineBrush *_this,
                        SDWrapMode  *wrapMode);
SDStatus
SDLineBrush_SetWrapMode(SDLineBrush *_this,
                        SDWrapMode   wrapMode);
SDStatus
SDLineBrush_GetTransform(SDLineBrush *_this,
                         SDMatrix    *matrix);
SDStatus
SDLineBrush_MultiplyTransform(SDLineBrush   *_this,
                              SDMatrix      *matrix,
                              SDMatrixOrder  order);
SDStatus
SDLineBrush_ResetTransform(SDLineBrush *_this);
SDStatus
SDLineBrush_RotateTransform(SDLineBrush   *_this,
                            SDFloat        angle,
                            SDMatrixOrder  order);
SDStatus
SDLineBrush_ScaleTransform(SDLineBrush   *_this,
                           SDFloat        sx,
                           SDFloat        sy,
                           SDMatrixOrder  order);
SDStatus
SDLineBrush_SetTriangularShape(SDLineBrush *_this,
                               SDFloat      focus,
                               SDFloat      scale);
SDStatus
SDLineBrush_SetSigmaBellShape(SDLineBrush *_this,
                              SDFloat      focus,
                              SDFloat      scale);
SDStatus
SDLineBrush_SetTransform(SDLineBrush *_this,
                         SDMatrix    *matrix);
SDStatus
SDLineBrush_TranslateTransform(SDLineBrush   *_this,
                               SDFloat        dx,
                               SDFloat        dy,
                               SDMatrixOrder  order);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_MATRIX_METHODS
#ifdef SDLIBRARY_MATRIX_METHODS
/* Declare public matrix methods. */
SDStatus
SDMatrix_Create(SDMatrix **_this);
SDStatus
SDMatrix_CreateParallelogram(SDMatrix     **_this,
                             SDRectangleF   rect,
                             SDPointF       tl,
                             SDPointF       tr,
                             SDPointF       bl);
SDStatus
SDMatrix_CreateElements(SDMatrix **_this,
                        SDFloat    m11,
                        SDFloat    m12,
                        SDFloat    m21,
                        SDFloat    m22,
                        SDFloat    dx,
                        SDFloat    dy);
SDStatus
SDMatrix_GetDeterminant(SDMatrix *_this,
                        SDFloat  *determinant);
SDStatus
SDMatrix_GetInverse(SDMatrix *_this,
                    SDMatrix *inverse);
SDStatus
SDMatrix_Multiply(SDMatrix      *_this,
                  SDMatrix      *other,
                  SDMatrixOrder  order);
SDStatus
SDMatrix_Equals(SDMatrix *_this,
                SDMatrix *other,
                SDBool   *eq);
SDStatus
SDMatrix_NotEquals(SDMatrix *_this,
                   SDMatrix *other,
                   SDBool   *ne);
SDStatus
SDMatrix_Rotate(SDMatrix      *_this,
                SDFloat        angle,
                SDMatrixOrder  order);
SDStatus
SDMatrix_Scale(SDMatrix      *_this,
               SDFloat        scaleX,
               SDFloat        scaleY,
               SDMatrixOrder  order);
SDStatus
SDMatrix_Shear(SDMatrix      *_this,
               SDFloat        shearX,
               SDFloat        shearY,
               SDMatrixOrder  order);
SDStatus
SDMatrix_Translate(SDMatrix      *_this,
                   SDFloat        offsetX,
                   SDFloat        offsetY,
                   SDMatrixOrder  order);
SDStatus
SDMatrix_TransformPoints(SDMatrix *_this,
                         SDPointF *points,
                         SDUInt32  count);
SDStatus
SDMatrix_TransformVectors(SDMatrix *_this,
                          SDPointF *points,
                          SDUInt32  count);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_PATH_METHODS
#ifdef SDLIBRARY_PATH_METHODS
/* Declare public path methods. */
SDStatus
SDPath_Create(SDPath **_this);
SDStatus
SDPath_Destroy(SDPath **_this);
SDStatus
SDPath_GetFillMode(SDPath     *_this,
                   SDFillMode *fillMode);
SDStatus
SDPath_SetFillMode(SDPath     *_this,
                   SDFillMode  fillMode);
SDStatus
SDPath_GetPoints(SDPath    *_this,
                 SDPointF **points,
                 SDUInt32  *count);
SDStatus
SDPath_GetTypes(SDPath    *_this,
                SDByte   **types,
                SDUInt32  *count);
SDStatus
SDPath_GetPathData(SDPath    *_this,
                   SDPointF **points,
                   SDByte   **types,
                   SDUInt32  *count);
SDStatus
SDPath_SetPathData(SDPath   *_this,
                   SDPointF *points,
                   SDByte   *types,
                   SDUInt32  count);
SDStatus
SDPath_AddArc(SDPath  *_this,
              SDFloat  x,
              SDFloat  y,
              SDFloat  width,
              SDFloat  height,
              SDFloat  startAngle,
              SDFloat  sweepAngle);
SDStatus
SDPath_AddBezier(SDPath  *_this,
                 SDFloat  x1,
                 SDFloat  y1,
                 SDFloat  x2,
                 SDFloat  y2,
                 SDFloat  x3,
                 SDFloat  y3,
                 SDFloat  x4,
                 SDFloat  y4);
SDStatus
SDPath_AddBeziers(SDPath   *_this,
                  SDPointF *points,
                  SDUInt32  count);
SDStatus
SDPath_AddClosedCardinalCurve(SDPath   *_this,
                              SDPointF *points,
                              SDUInt32  count,
                              SDFloat   tension);
SDStatus
SDPath_AddCardinalCurve(SDPath   *_this,
                        SDPointF *points,
                        SDUInt32  count,
                        SDUInt32  offset,
                        SDUInt32  numberOfSegments,
                        SDFloat   tension);
SDStatus
SDPath_AddEllipse(SDPath  *_this,
                  SDFloat  x,
                  SDFloat  y,
                  SDFloat  width,
                  SDFloat  height);
SDStatus
SDPath_AddLine(SDPath  *_this,
               SDFloat  x1,
               SDFloat  y1,
               SDFloat  x2,
               SDFloat  y2);
SDStatus
SDPath_AddLines(SDPath   *_this,
                SDPointF *points,
                SDUInt32  count);
SDStatus
SDPath_AddPath(SDPath *_this,
               SDPath *path,
               SDBool  connect);
SDStatus
SDPath_AddPie(SDPath  *_this,
              SDFloat  x,
              SDFloat  y,
              SDFloat  width,
              SDFloat  height,
              SDFloat  startAngle,
              SDFloat  sweepAngle);
SDStatus
SDPath_AddPolygon(SDPath   *_this,
                  SDPointF *points,
                  SDUInt32  count);
SDStatus
SDPath_AddRectangle(SDPath  *_this,
                    SDFloat  x,
                    SDFloat  y,
                    SDFloat  width,
                    SDFloat  height);
SDStatus
SDPath_AddRectangles(SDPath       *_this,
                     SDRectangleF *rects,
                     SDUInt32      count);
SDStatus
SDPath_AddString(SDPath         *_this,
                 SDChar16       *s,
                 SDUInt32        length,
                 SDFontFamily   *family,
                 SDFontStyle     style,
                 SDFloat         emSize,
                 SDRectangleF    layoutRect,
                 SDStringFormat *format);
SDStatus
SDPath_ClearMarkers(SDPath *_this);
SDStatus
SDPath_Clone(SDPath  *_this,
             SDPath **clone);
SDStatus
SDPath_CloseAllFigures(SDPath *_this);
SDStatus
SDPath_CloseFigure(SDPath *_this);
SDStatus
SDPath_Flatten(SDPath   *_this,
               SDMatrix *matrix,
               SDFloat   flatness);
SDStatus
SDPath_GetCount(SDPath   *_this,
                SDUInt32 *count);
SDStatus
SDPath_GetBounds(SDPath       *_this,
                 SDMatrix     *matrix,
                 SDPen        *pen,
                 SDRectangleF *bounds);
SDStatus
SDPath_GetLastPoint(SDPath   *_this,
                    SDPointF *point);
SDStatus
SDPath_IsOutlineVisible(SDPath     *_this,
                        SDFloat     x,
                        SDFloat     y,
                        SDPen      *pen,
                        SDGraphics *graphics,
                        SDBool     *visible);
SDStatus
SDPath_IsVisible(SDPath     *_this,
                 SDFloat     x,
                 SDFloat     y,
                 SDGraphics *graphics,
                 SDBool     *visible);
SDStatus
SDPath_Reset(SDPath *_this);
SDStatus
SDPath_Reverse(SDPath *_this);
SDStatus
SDPath_SetMarker(SDPath *_this);
SDStatus
SDPath_StartFigure(SDPath *_this);
SDStatus
SDPath_Transform(SDPath   *_this,
                 SDMatrix *matrix);
SDStatus
SDPath_Warp(SDPath     *_this,
            SDMatrix   *matrix,
            SDPointF   *dstPoints,
            SDUInt32    dstLength,
            SDFloat     srcX,
            SDFloat     srcY,
            SDFloat     srcWidth,
            SDFloat     srcHeight,
            SDWarpMode  warpMode,
            SDFloat     flatness);
SDStatus
SDPath_Widen(SDPath   *_this,
             SDPen    *pen,
             SDMatrix *matrix,
             SDFloat   flatness);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_PATHBRUSH_METHODS
#ifdef SDLIBRARY_PATHBRUSH_METHODS
/* Declare public path brush methods. */
SDStatus
SDPathBrush_Create(SDPathBrush **_this,
                   SDPath       *path);
SDStatus
SDPathBrush_GetBlend(SDPathBrush *_this,
                     SDBlend     *blend);
SDStatus
SDPathBrush_SetBlend(SDPathBrush *_this,
                     SDBlend      blend);
SDStatus
SDPathBrush_GetCenterColor(SDPathBrush *_this,
                           SDColor     *centerColor);
SDStatus
SDPathBrush_SetCenterColor(SDPathBrush *_this,
                           SDColor      centerColor);
SDStatus
SDPathBrush_GetCenterPoint(SDPathBrush *_this,
                           SDPointF    *centerPoint);
SDStatus
SDPathBrush_SetCenterPoint(SDPathBrush *_this,
                           SDPointF     centerPoint);
SDStatus
SDPathBrush_GetColorBlend(SDPathBrush  *_this,
                          SDColorBlend *colorBlend);
SDStatus
SDPathBrush_SetColorBlend(SDPathBrush  *_this,
                          SDColorBlend  colorBlend);
SDStatus
SDPathBrush_GetFocusPoint(SDPathBrush *_this,
                          SDPointF    *focusPoint);
SDStatus
SDPathBrush_SetFocusPoint(SDPathBrush *_this,
                          SDPointF     focusPoint);
SDStatus
SDPathBrush_GetRectangle(SDPathBrush  *_this,
                         SDRectangleF *rectangle);
SDStatus
SDPathBrush_GetSurroundColors(SDPathBrush  *_this,
                              SDColor     **surroundColors,
                              SDUInt32     *count);
SDStatus
SDPathBrush_SetSurroundColors(SDPathBrush *_this,
                              SDColor     *surroundColors,
                              SDUInt32     count);
SDStatus
SDPathBrush_GetWrapMode(SDPathBrush *_this,
                        SDWrapMode  *wrapMode);
SDStatus
SDPathBrush_SetWrapMode(SDPathBrush *_this,
                        SDWrapMode   wrapMode);
SDStatus
SDPathBrush_GetTransform(SDPathBrush *_this,
                         SDMatrix    *matrix);
SDStatus
SDPathBrush_MultiplyTransform(SDPathBrush   *_this,
                              SDMatrix      *matrix,
                              SDMatrixOrder  order);
SDStatus
SDPathBrush_ResetTransform(SDPathBrush *_this);
SDStatus
SDPathBrush_RotateTransform(SDPathBrush   *_this,
                            SDFloat        angle,
                            SDMatrixOrder  order);
SDStatus
SDPathBrush_ScaleTransform(SDPathBrush   *_this,
                           SDFloat        sx,
                           SDFloat        sy,
                           SDMatrixOrder  order);
SDStatus
SDPathBrush_SetTriangularShape(SDPathBrush *_this,
                               SDFloat      focus,
                               SDFloat      scale);
SDStatus
SDPathBrush_SetSigmaBellShape(SDPathBrush *_this,
                              SDFloat      focus,
                              SDFloat      scale);
SDStatus
SDPathBrush_SetTransform(SDPathBrush *_this,
                         SDMatrix    *matrix);
SDStatus
SDPathBrush_TranslateTransform(SDPathBrush   *_this,
                               SDFloat        dx,
                               SDFloat        dy,
                               SDMatrixOrder  order);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_PEN_METHODS
#ifdef SDLIBRARY_PEN_METHODS
/* Declare public pen methods. */
SDStatus
SDPen_Create(SDPen   **_this,
             SDBrush  *brush,
             SDFloat   width);
SDStatus
SDPen_Destroy(SDPen **_this);
SDStatus
SDPen_GetAlignment(SDPen          *_this,
                   SDPenAlignment *alignment);
SDStatus
SDPen_SetAlignment(SDPen          *_this,
                   SDPenAlignment  alignment);
SDStatus
SDPen_GetBrush(SDPen    *_this,
               SDBrush **brush);
SDStatus
SDPen_SetBrush(SDPen   *_this,
               SDBrush *brush);
SDStatus
SDPen_SetCaps(SDPen     *_this,
              SDLineCap  startCap,
              SDLineCap  endCap,
              SDDashCap  dashCap);
SDStatus
SDPen_GetColor(SDPen   *_this,
               SDColor *color);
SDStatus
SDPen_GetCompoundArray(SDPen     *_this,
                       SDFloat  **compoundArray,
                       SDUInt32  *count);
SDStatus
SDPen_SetCompoundArray(SDPen         *_this,
                       const SDFloat *compoundArray,
                       SDUInt32       count);
SDStatus
SDPen_GetCustomEndCap(SDPen            *_this,
                      SDCustomLineCap **customEndCap);
SDStatus
SDPen_SetCustomEndCap(SDPen           *_this,
                      SDCustomLineCap *customEndCap);
SDStatus
SDPen_GetCustomStartCap(SDPen            *_this,
                        SDCustomLineCap **customStartCap);
SDStatus
SDPen_SetCustomStartCap(SDPen           *_this,
                        SDCustomLineCap *customStartCap);
SDStatus
SDPen_GetDashCap(SDPen     *_this,
                 SDDashCap *dashCap);
SDStatus
SDPen_SetDashCap(SDPen     *_this,
                 SDDashCap  dashCap);
SDStatus
SDPen_GetDashOffset(SDPen   *_this,
                    SDFloat *dashOffset);
SDStatus
SDPen_SetDashOffset(SDPen   *_this,
                    SDFloat  dashOffset);
SDStatus
SDPen_GetDashPattern(SDPen     *_this,
                     SDFloat  **dashPattern,
                     SDUInt32  *count);
SDStatus
SDPen_SetDashPattern(SDPen         *_this,
                     const SDFloat *dashPattern,
                     SDUInt32       count);
SDStatus
SDPen_GetDashStyle(SDPen       *_this,
                   SDDashStyle *dashStyle);
SDStatus
SDPen_SetDashStyle(SDPen       *_this,
                   SDDashStyle  dashStyle);
SDStatus
SDPen_GetEndCap(SDPen     *_this,
                SDLineCap *endCap);
SDStatus
SDPen_SetEndCap(SDPen     *_this,
                SDLineCap  endCap);
SDStatus
SDPen_GetLineJoin(SDPen      *_this,
                  SDLineJoin *lineJoin);
SDStatus
SDPen_SetLineJoin(SDPen      *_this,
                  SDLineJoin  lineJoin);
SDStatus
SDPen_GetMiterLimit(SDPen   *_this,
                    SDFloat *miterLimit);
SDStatus
SDPen_SetMiterLimit(SDPen   *_this,
                    SDFloat  miterLimit);
SDStatus
SDPen_GetPenType(SDPen     *_this,
                 SDPenType *type);
SDStatus
SDPen_GetStartCap(SDPen     *_this,
                  SDLineCap *startCap);
SDStatus
SDPen_SetStartCap(SDPen     *_this,
                  SDLineCap  startCap);
SDStatus
SDPen_GetWidth(SDPen   *_this,
               SDFloat *width);
SDStatus
SDPen_SetWidth(SDPen   *_this,
               SDFloat  width);
SDStatus
SDPen_Clone(SDPen  *_this,
            SDPen **clone);
SDStatus
SDPen_GetTransform(SDPen    *_this,
                   SDMatrix *matrix);
SDStatus
SDPen_MultiplyTransform(SDPen         *_this,
                        SDMatrix      *matrix,
                        SDMatrixOrder  order);
SDStatus
SDPen_ResetTransform(SDPen *_this);
SDStatus
SDPen_RotateTransform(SDPen         *_this,
                      SDFloat        angle,
                      SDMatrixOrder  order);
SDStatus
SDPen_ScaleTransform(SDPen         *_this,
                     SDFloat        sx,
                     SDFloat        sy,
                     SDMatrixOrder  order);
SDStatus
SDPen_SetTransform(SDPen    *_this,
                   SDMatrix *matrix);
SDStatus
SDPen_TranslateTransform(SDPen         *_this,
                         SDFloat        dx,
                         SDFloat        dy,
                         SDMatrixOrder  order);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_REGION_METHODS
#ifdef SDLIBRARY_REGION_METHODS
/* Declare public region methods. */
SDStatus
SDRegion_Create(SDRegion **_this);
SDStatus
SDRegion_CreatePath(SDRegion **_this,
                    SDPath    *path);
SDStatus
SDRegion_CreateRectangle(SDRegion     **_this,
                         SDRectangleF   rectangle);
SDStatus
SDRegion_Destroy(SDRegion **_this);
SDStatus
SDRegion_Clone(SDRegion  *_this,
               SDRegion **clone);
SDStatus
SDRegion_CombinePath(SDRegion      *_this,
                     SDPath        *path,
                     SDCombineMode  combineMode);
SDStatus
SDRegion_CombineRectangle(SDRegion      *_this,
                          SDRectangleF   rectangle,
                          SDCombineMode  combineMode);
SDStatus
SDRegion_CombineRegion(SDRegion      *_this,
                       SDRegion      *other,
                       SDCombineMode  combineMode);
SDStatus
SDRegion_Equals(SDRegion   *_this,
                SDRegion   *other,
                SDGraphics *graphics,
                SDBool     *eq);
SDStatus
SDRegion_GetBounds(SDRegion     *_this,
                   SDGraphics   *graphics,
                   SDRectangleF *bounds);
SDStatus
SDRegion_GetData(SDRegion  *_this,
                 SDByte   **data,
                 SDUInt32  *count);
SDStatus
SDRegion_GetRegionScans(SDRegion      *_this,
                        SDMatrix      *matrix,
                        SDRectangleF **scans,
                        SDUInt32      *count);
SDStatus
SDRegion_IsEmpty(SDRegion   *_this,
                 SDGraphics *graphics,
                 SDBool     *empty);
SDStatus
SDRegion_IsInfinite(SDRegion   *_this,
                    SDGraphics *graphics,
                    SDBool     *infinite);
SDStatus
SDRegion_IsVisiblePoint(SDRegion   *_this,
                        SDGraphics *graphics,
                        SDPointF    point,
                        SDBool     *visible);
SDStatus
SDRegion_IsVisibleRectangle(SDRegion     *_this,
                            SDGraphics   *graphics,
                            SDRectangleF  rectangle,
                            SDBool       *visible);
SDStatus
SDRegion_MakeEmpty(SDRegion *_this);
SDStatus
SDRegion_MakeInfinite(SDRegion *_this);
SDStatus
SDRegion_Transform(SDRegion *_this,
                   SDMatrix *matrix);
SDStatus
SDRegion_Translate(SDRegion *_this,
                   SDFloat   dx,
                   SDFloat   dy);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_SOLIDBRUSH_METHODS
#ifdef SDLIBRARY_SOLIDBRUSH_METHODS
/* Declare public solid brush methods. */
SDStatus
SDSolidBrush_Create(SDSolidBrush **_this,
                    SDColor        color);
SDStatus
SDSolidBrush_GetColor(SDSolidBrush *_this,
                      SDColor      *color);
SDStatus
SDSolidBrush_SetColor(SDSolidBrush *_this,
                      SDColor       color);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_STRINGFORMAT_METHODS
#ifdef SDLIBRARY_STRINGFORMAT_METHODS
/* Declare public string format methods. */
SDStatus
SDStringFormat_Create(SDStringFormat      **_this,
                      SDStringFormatFlag    flags,
                      SDLanguageID          language);
SDStatus
SDStringFormat_Destroy(SDStringFormat **_this);
SDStatus
SDStringFormat_CreateGenericDefault(SDStringFormat **_this);
SDStatus
SDStringFormat_CreateGenericTypographic(SDStringFormat **_this);
SDStatus
SDStringFormat_Clone(SDStringFormat  *_this,
                     SDStringFormat **clone);
SDStatus
SDStringFormat_GetAlignment(SDStringFormat    *_this,
                            SDStringAlignment *alignment);
SDStatus
SDStringFormat_SetAlignment(SDStringFormat    *_this,
                            SDStringAlignment  alignment);
SDStatus
SDStringFormat_GetCharacterRanges(SDStringFormat    *_this,
                                  SDCharacterRange **characterRanges,
                                  SDUInt32          *count);
SDStatus
SDStringFormat_SetCharacterRanges(SDStringFormat   *_this,
                                  SDCharacterRange *characterRanges,
                                  SDUInt32          count);
SDStatus
SDStringFormat_GetDigitSubstitution(SDStringFormat          *_this,
                                    SDDigitSubstitute *method,
                                    SDLanguageID            *language);
SDStatus
SDStringFormat_SetDigitSubstitution(SDStringFormat          *_this,
                                    SDDigitSubstitute  method,
                                    SDLanguageID             language);
SDStatus
SDStringFormat_GetFormatFlags(SDStringFormat     *_this,
                              SDStringFormatFlag *formatFlags);
SDStatus
SDStringFormat_SetFormatFlags(SDStringFormat     *_this,
                              SDStringFormatFlag  formatFlags);
SDStatus
SDStringFormat_GetHotkeyPrefix(SDStringFormat *_this,
                               SDHotkeyPrefix *hotkeyPrefix);
SDStatus
SDStringFormat_SetHotkeyPrefix(SDStringFormat *_this,
                               SDHotkeyPrefix  hotkeyPrefix);
SDStatus
SDStringFormat_GetLineAlignment(SDStringFormat    *_this,
                                SDStringAlignment *lineAlignment);
SDStatus
SDStringFormat_SetLineAlignment(SDStringFormat    *_this,
                                SDStringAlignment  lineAlignment);
SDStatus
SDStringFormat_GetTabStops(SDStringFormat  *_this,
                           SDFloat         *firstTabOffset,
                           SDFloat        **tabStops,
                           SDUInt32        *count);
SDStatus
SDStringFormat_SetTabStops(SDStringFormat *_this,
                           SDFloat         firstTabOffset,
                           SDFloat        *tabStops,
                           SDUInt32        count);
SDStatus
SDStringFormat_GetTrimming(SDStringFormat   *_this,
                           SDStringTrimming *trimming);
SDStatus
SDStringFormat_SetTrimming(SDStringFormat   *_this,
                           SDStringTrimming  trimming);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_SURFACE_METHODS
#ifdef SDLIBRARY_SURFACE_METHODS
/* Declare public surface methods. */
SDStatus
SDSurface_Lock(SDSurface *_this);
SDStatus
SDSurface_Unlock(SDSurface *_this);
SDStatus
SDSurface_GetBoundsF(SDSurface    *_this,
                     SDRectangleF *bounds);
SDStatus
SDSurface_GetBounds(SDSurface *_this,
                    SDUInt32  *x,
                    SDUInt32  *y,
                    SDUInt32  *width,
                    SDUInt32  *height);
SDStatus
SDSurface_SetBounds(SDSurface *_this,
                    SDUInt32   x,
                    SDUInt32   y,
                    SDUInt32   width,
                    SDUInt32   height);
SDStatus
SDSurface_Reference(SDSurface *_this);
SDStatus
SDSurface_Destroy(SDSurface **_this);
SDStatus
SDSurface_Composite(SDSurface           *_this,
                    SDUInt32             x,
                    SDUInt32             y,
                    SDUInt32             width,
                    SDUInt32             height,
                    pixman_image_t      *src,
                    pixman_image_t      *mask,
                    SDInterpolationMode  interpolationMode,
                    SDCompositingMode    compositingMode);
SDStatus
SDSurface_Clear(SDSurface *_this,
                SDColor    color);
SDStatus
SDSurface_Flush(SDSurface        *_this,
                SDFlushIntention  intention);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_TEXTUREBRUSH_METHODS
#ifdef SDLIBRARY_TEXTUREBRUSH_METHODS
/* Declare public texture brush methods. */
SDStatus
SDTextureBrush_Create(SDTextureBrush **_this,
                      SDImage         *image,
                      SDRectangleF     rectangle,
                      SDWrapMode       wrapMode);
SDStatus
SDTextureBrush_GetImage(SDTextureBrush  *_this,
                        SDImage        **image);
SDStatus
SDTextureBrush_GetWrapMode(SDTextureBrush *_this,
                           SDWrapMode     *wrapMode);
SDStatus
SDTextureBrush_SetWrapMode(SDTextureBrush *_this,
                           SDWrapMode      wrapMode);
SDStatus
SDTextureBrush_GetTransform(SDTextureBrush *_this,
                            SDMatrix       *matrix);
SDStatus
SDTextureBrush_MultiplyTransform(SDTextureBrush *_this,
                                 SDMatrix       *matrix,
                                 SDMatrixOrder   order);
SDStatus
SDTextureBrush_ResetTransform(SDTextureBrush *_this);
SDStatus
SDTextureBrush_RotateTransform(SDTextureBrush *_this,
                               SDFloat         angle,
                               SDMatrixOrder   order);
SDStatus
SDTextureBrush_ScaleTransform(SDTextureBrush *_this,
                              SDFloat         sx,
                              SDFloat         sy,
                              SDMatrixOrder   order);
SDStatus
SDTextureBrush_SetTransform(SDTextureBrush *_this,
                            SDMatrix       *matrix);
SDStatus
SDTextureBrush_TranslateTransform(SDTextureBrush *_this,
                                  SDFloat         dx,
                                  SDFloat         dy,
                                  SDMatrixOrder   order);
#endif
/******************************************************************************/



/******************************************************************************/
#define SDLIBRARY_X11SURFACE_METHODS
#ifdef SDLIBRARY_X11SURFACE_METHODS
/* Declare public x surface methods. */
#ifdef HAVE_X11_XLIB_H
SDStatus
SDX11Surface_Create(SDX11Surface **_this,
                    Display       *dpy,
                    Drawable       drawable,
                    Screen        *screen,
                    Visual        *visual,
                    SDUInt32       width,
                    SDUInt32       height);
#endif
#endif
/******************************************************************************/

#ifdef __cplusplus
};
#endif

#endif /* _SD_LIBRARY_H_ */
