/*
 * lib_defs.h - Definitions for the builtin class library.
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

#ifndef	_ENGINE_LIB_DEFS_H
#define	_ENGINE_LIB_DEFS_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Get the class that is associated with a non-null object.
 */
#define	GetObjectClass(obj)	\
			(*((ILClass **)(((unsigned char *)(obj)) - IL_BEST_ALIGNMENT)))

/*
 * Internal structure of a string object header.
 */
typedef struct
{
	ILInt32		capacity;
	ILInt32		length;

} System_String;

/*
 * Convert a string object pointer into a pointer to the first character.
 */
#define	StringToBuffer(str)		((ILUInt16 *)(((System_String *)(str)) + 1))

/*
 * Internal structure of an array header.
 */
typedef struct
{
	ILInt32		length;

} System_Array;

/*
 * Convert an array object pointer into a pointer to the first item.
 */
#define	ArrayToBuffer(array)	((void *)(((System_Array *)(array)) + 1))

/*
 * Internal structure of a multi-dimensional array header.
 */
typedef struct
{
	ILInt32		lower;
	ILInt32		size;
	ILInt32		multiplier;

} MArrayBounds;
typedef struct
{
	ILInt32			rank;
	ILInt32			elemSize;
	void	       *data;
	MArrayBounds	bounds[1];

} System_MArray;

/*
 * Determine if an array inherits from "$Synthetic.SArray".
 */
int _ILIsSArray(System_Array *array);

/*
 * Determine if an array inherits from "$Synthetic.MArray".
 */
int _ILIsMArray(System_Array *array);

/*
 * Internal structure of a reflection object.  Types, methods, fields, etc.
 */
typedef struct
{
	void	   *privateData;

} System_Reflection;

/*
 * Check that the caller has permission to access a specific class
 * or member via reflection.  Returns non-zero if access has been granted.
 */
int _ILClrCheckAccess(ILExecThread *thread, ILClass *classInfo,
					  ILMember *member);

/*
 * Check that the caller has permission to access a specific program item.
 */
int _ILClrCheckItemAccess(ILExecThread *thread, ILProgramItem *item);

/*
 * Convert an "ILClass" into a "ClrType" instance.
 */
ILObject *_ILGetClrType(ILExecThread *thread, ILClass *classInfo);

/*
 * Convert an "ILType" into a "ClrType" instance.
 */
ILObject *_ILGetClrTypeForILType(ILExecThread *thread, ILType *type);

/*
 * Get the "ILClass" value associated with a "ClrType" object.
 */
ILClass *_ILGetClrClass(ILExecThread *thread, ILObject *type);

/*
 * Convert a non-type program item pointer into a reflection object.
 * Do not use this for types.  Use "_ILGetClrType" instead.
 */
ILObject *_ILClrToObject(ILExecThread *thread, void *item, const char *name);

/*
 * Convert a reflection object into a non-type program item pointer.
 * Do not use this for types.  Use "_ILGetClrClass" instead.
 */
void *_ILClrFromObject(ILExecThread *thread, ILObject *object);

/*
 * Look for a type, starting at a particular image.  If "assemblyImage"
 * is NULL, then use the name to determine the image to start at.
 */
ILObject *_ILGetTypeFromImage(ILExecThread *thread,
							  ILImage *assemblyImage,
							  ILString *name,
							  ILBool throwOnError,
							  ILBool ignoreCase);

/*
 * Internal structure of "System.Threading.Thread".
 */
typedef struct
{
	void	   *privateData;
	ILObject   *stateInfo;
	ILObject   *start;
	ILString   *name;

} System_Thread;

#ifdef	__cplusplus
};
#endif

#endif	/* _ENGINE_LIB_DEFS_H */
