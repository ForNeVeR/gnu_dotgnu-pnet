/*
 * c_typesize.h - Determine the sizes of standard C types.
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

#ifndef	_C_TYPESIZE_H
#define	_C_TYPESIZE_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Alignment modifier flags for object file memory model designators.
 * These are typically used to specify native alignment rules that
 * modify the standard "Model 64" and "Model 32" rules.  If a flag
 * isn't set for a particular type, then the standard model rules
 * are used for that type.
 */
#define	C_ALIGNMOD_SHORT_1		(1<<0)		/* Short is byte-aligned */
#define	C_ALIGNMOD_INT_1		(1<<1)		/* Int is byte-aligned */
#define	C_ALIGNMOD_INT_2		(1<<2)		/* Int is short-aligned */
#define	C_ALIGNMOD_LONG_1		(1<<3)		/* Long is byte-aligned */
#define	C_ALIGNMOD_LONG_2		(1<<4)		/* Long is short-aligned */
#define	C_ALIGNMOD_LONG_4		(1<<5)		/* Long is int-aligned */
#define	C_ALIGNMOD_FLOAT_1		(1<<6)		/* Float is byte-aligned */
#define	C_ALIGNMOD_FLOAT_2		(1<<7)		/* Float is int-aligned */
#define	C_ALIGNMOD_DOUBLE_1		(1<<8)		/* Double is byte-aligned */
#define	C_ALIGNMOD_DOUBLE_2		(1<<9)		/* Double is short-aligned */
#define	C_ALIGNMOD_DOUBLE_4		(1<<10)		/* Double is int-aligned */
#define	C_ALIGNMOD_LDOUBLE_1	(1<<11)		/* Long double is byte-aligned */
#define	C_ALIGNMOD_LDOUBLE_2	(1<<12)		/* Long double is short-aligned */
#define	C_ALIGNMOD_LDOUBLE_4	(1<<13)		/* Long double is int-aligned */
#define	C_ALIGNMOD_LDOUBLE_8	(1<<14)		/* Long double is long-aligned */
#define	C_ALIGNMOD_LDOUBLE_16	(1<<15)		/* Long double is hex-aligned */
#define	C_ALIGNMOD_PTR_1		(1<<16)		/* Pointer is byte-aligned */
#define	C_ALIGNMOD_PTR_2		(1<<17)		/* Pointer is short-aligned */
#define	C_ALIGNMOD_PTR_4		(1<<18)		/* Pointer is int-aligned */
#define	C_ALIGNMOD_PTR_8		(1<<19)		/* Pointer is long-aligned */

/*
 * Size and alignment values for the standard C types.
 */
extern ILUInt32 CTypeShortSize;
extern ILUInt32 CTypeShortAlign;
extern ILUInt32 CTypeIntSize;
extern ILUInt32 CTypeIntAlign;
extern ILUInt32 CTypeLongSize;
extern ILUInt32 CTypeLongAlign;
extern ILUInt32 CTypeLongLongSize;
extern ILUInt32 CTypeLongLongAlign;
extern ILUInt32 CTypeFloatSize;
extern ILUInt32 CTypeFloatAlign;
extern ILUInt32 CTypeDoubleSize;
extern ILUInt32 CTypeDoubleAlign;
extern ILUInt32 CTypeLongDoubleSize;
extern ILUInt32 CTypeLongDoubleAlign;
extern ILUInt32 CTypeNativeIntSize;
extern ILUInt32 CTypeNativeIntAlign;
extern ILUInt32 CTypePtrSize;
extern ILUInt32 CTypePtrAlign;

/*
 * Detected alignment modifiers.
 */
extern ILUInt32 CTypeAlignModifiers;

/*
 * Detect the size and alignment of the standard C types.
 */
void CTypeSizeDetect(void);

/*
 * Push into a new type context that uses native type layout.
 */
void CTypeSizePushNative(void);

/*
 * Pop from a native type layout context.
 */
void CTypeSizePopNative(void);

#ifdef	__cplusplus
};
#endif

#endif	/* _C_TYPESIZE_H */
