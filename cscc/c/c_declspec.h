/*
 * c_declspec.h - Declaration specifiers for the C programming language.
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

#ifndef	_C_DECLSPEC_H
#define	_C_DECLSPEC_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Storage class and type specifiers.
 */
#define	C_SPEC_TYPEDEF			(1<<0)
#define	C_SPEC_EXTERN			(1<<1)
#define	C_SPEC_STATIC			(1<<2)
#define	C_SPEC_AUTO				(1<<3)
#define	C_SPEC_REGISTER			(1<<4)
#define	C_SPEC_INLINE			(1<<5)
#define	C_SPEC_SHORT			(1<<6)
#define	C_SPEC_LONG				(1<<7)
#define	C_SPEC_LONG_LONG		(1<<8)
#define	C_SPEC_SIGNED			(1<<9)
#define	C_SPEC_UNSIGNED			(1<<10)
#define	C_SPEC_CONST			(1<<11)
#define	C_SPEC_VOLATILE			(1<<12)
#define	C_SPEC_NATIVE			(1<<13)
#define	C_SPEC_MULTIPLE_BASES	(1<<14)
#define	C_SPEC_LONG_AND_SHORT	(1<<15)
#define	C_SPEC_SIGN_AND_UNSIGN	(1<<16)
#define	C_SPEC_INVALID_COMBO	(1<<17)

/*
 * Useful specifier combinations.
 */
#define	C_SPEC_STORAGE		\
			(C_SPEC_TYPEDEF | \
			 C_SPEC_EXTERN | \
			 C_SPEC_STATIC | \
			 C_SPEC_AUTO | \
			 C_SPEC_REGISTER | \
			 C_SPEC_INLINE)
#define	C_SPEC_TYPE_COMMON		\
			(C_SPEC_CONST | \
			 C_SPEC_VOLATILE | \
			 C_SPEC_NATIVE)
#define	C_SPEC_TYPE_CHANGE		\
			(C_SPEC_SHORT | \
			 C_SPEC_LONG | \
			 C_SPEC_LONG_LONG | \
			 C_SPEC_SIGNED | \
			 C_SPEC_UNSIGNED)

/*
 * Kinds of declarations, for checking the applicability of specifiers.
 */
#define	C_KIND_GLOBAL_NAME		(1<<0)
#define	C_KIND_LOCAL_NAME		(1<<1)
#define	C_KIND_FUNCTION			(1<<2)

/*
 * Structure of a declaration specifier.
 */
typedef struct
{
	int		specifiers;
	int		dupSpecifiers;
	ILType *baseType;

} CDeclSpec;

/*
 * Set a declaration specifier to a particular flag.
 */
#define	CDeclSpecSet(spec,flag)	\
			do { \
				(spec).specifiers = (flag); \
				(spec).dupSpecifiers = 0; \
				(spec).baseType = ILType_Invalid; \
			} while (0)

/*
 * Set a declaration specifier to a particular base type.
 */
#define	CDeclSpecSetType(spec,type)	\
			do { \
				(spec).specifiers = 0; \
				(spec).dupSpecifiers = 0; \
				(spec).baseType = (type); \
			} while (0)

/*
 * Convert a storage class specifier into a name.
 * Returns NULL for an empty storage class.
 */
const char *CStorageClassToName(int specifier);

/*
 * Create an empty declaration specifier.
 */
CDeclSpec CDeclSpecEmpty(void);

/*
 * Combine two declaration specifiers.
 */
CDeclSpec CDeclSpecCombine(CDeclSpec spec1, CDeclSpec spec2);

/*
 * Finalize a declaration specifier, and output any relevant
 * errors and warnings.  The final "specifiers" value is the
 * storage class, with everything else in "baseType".
 */
CDeclSpec CDeclSpecFinalize(CDeclSpec spec, ILNode *node,
							const char *name, int kind);

#ifdef	__cplusplus
};
#endif

#endif	/* _C_DECLSPEC_H */
