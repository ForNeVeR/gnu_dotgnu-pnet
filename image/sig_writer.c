/*
 * sig_writer.c - Signature writing for IL image output.
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

#include "image.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Buffer handling for writing signatures.
 */
#define	IL_SIG_BUFFER_SIZE		128
typedef struct
{
	int 			posn, len;
	unsigned char  *buf;
	unsigned char	data[IL_SIG_BUFFER_SIZE];

} SigBuffer;

/*
 * Initialize a signature buffer.
 */
#define	SIG_INIT()	\
			do { \
				buffer.posn = 0; \
				buffer.len = IL_SIG_BUFFER_SIZE; \
				buffer.buf = buffer.data; \
			} while (0)

/*
 * Destroy a signature buffer.
 */
#define	SIG_DESTROY()	\
			do { \
				if(buffer.buf != buffer.data) \
				{ \
					ILFree(buffer.buf); \
				} \
			} while (0)

/*
 * Expand a signature buffer.
 */
static int ExpandBuffer(SigBuffer *buffer)
{
	unsigned char *newbuf;
	if(buffer->buf == buffer->data)
	{
		newbuf = (unsigned char *)ILMalloc(IL_SIG_BUFFER_SIZE * 2);
		if(!newbuf)
		{
			return 0;
		}
		ILMemCpy(newbuf, buffer->buf, buffer->posn);
	}
	else
	{
		newbuf = (unsigned char *)ILRealloc(buffer->buf,
											buffer->len + IL_SIG_BUFFER_SIZE);
		if(!newbuf)
		{
			return 0;
		}
	}
	buffer->buf = newbuf;
	buffer->len += IL_SIG_BUFFER_SIZE;
	return 1;
}

/*
 * Write a single byte to a signature buffer.
 */
#define	SIG_WRITE(byte)	\
			do { \
				if(buffer->posn < buffer->len) \
				{ \
					buffer->buf[(buffer->posn)++] = (unsigned char)(byte); \
				} \
				else \
				{ \
					if(ExpandBuffer(buffer)) \
					{ \
						buffer->buf[(buffer->posn)++] = (unsigned char)(byte); \
					} \
					else \
					{ \
						return 0; \
					} \
				} \
			} while (0)

/*
 * Write the token code for a class into a signature.
 */
static int WriteClassToken(SigBuffer *buffer, ILClass *info)
{
	if((buffer->posn + IL_META_COMPRESS_MAX_SIZE) > buffer->len)
	{
		if(!ExpandBuffer(buffer))
		{
			return 0;
		}
	}
	buffer->posn += ILMetaCompressToken(buffer->buf + buffer->posn,
										ILClass_Token(info));
	return 1;
}

/*
 * Write a 32-bit unsigned value into a signature.
 */
static int WriteValue(SigBuffer *buffer, unsigned long value)
{
	if((buffer->posn + IL_META_COMPRESS_MAX_SIZE) > buffer->len)
	{
		if(!ExpandBuffer(buffer))
		{
			return 0;
		}
	}
	buffer->posn += ILMetaCompressData(buffer->buf + buffer->posn, value);
	return 1;
}

/*
 * Write a 32-bit signed value into a signature.
 */
static int WriteIntValue(SigBuffer *buffer, long value)
{
	if((buffer->posn + IL_META_COMPRESS_MAX_SIZE) > buffer->len)
	{
		if(!ExpandBuffer(buffer))
		{
			return 0;
		}
	}
	buffer->posn += ILMetaCompressInt(buffer->buf + buffer->posn, value);
	return 1;
}

/*
 * Forward declaration.
 */
static int WriteType(SigBuffer *buffer, ILType *type, int methodPtr);

/*
 * Write the parameters for a method or property.
 */
static int WriteMethodParams(SigBuffer *buffer, ILType *method)
{
	long num = (long)(unsigned long)(method->num);
	ILType *params;
	if(num > 0)
	{
		if(!WriteType(buffer, method->un.method.param[0], 1))
		{
			return 0;
		}
	}
	if(num > 1)
	{
		if(!WriteType(buffer, method->un.method.param[1], 1))
		{
			return 0;
		}
	}
	if(num > 2)
	{
		if(!WriteType(buffer, method->un.method.param[2], 1))
		{
			return 0;
		}
	}
	if(num > 3)
	{
		params = method->un.method.next;
		num -= 3;
		while(num > 0)
		{
			if(!WriteType(buffer, params->un.params.param[0], 1))
			{
				return 1;
			}
			if(num > 1)
			{
				if(!WriteType(buffer, params->un.params.param[1], 1))
				{
					return 1;
				}
			}
			if(num > 2)
			{
				if(!WriteType(buffer, params->un.params.param[2], 1))
				{
					return 1;
				}
			}
			if(num > 3)
			{
				if(!WriteType(buffer, params->un.params.param[3], 1))
				{
					return 1;
				}
			}
			params = params->un.params.next;
			num -= 4;
		}
	}
	return 1;
}

/*
 * Write the signature encoding for a list of local variables.
 */
static int WriteLocalVars(SigBuffer *buffer, ILType *locals)
{
	long num = (long)(unsigned long)(locals->num);
	ILType *temp = locals;
	while(num > 0)
	{
		if(!WriteType(buffer, temp->un.locals.local[0], 1))
		{
			return 1;
		}
		if(num > 1)
		{
			if(!WriteType(buffer, temp->un.locals.local[1], 1))
			{
				return 1;
			}
		}
		if(num > 2)
		{
			if(!WriteType(buffer, temp->un.locals.local[2], 1))
			{
				return 1;
			}
		}
		if(num > 3)
		{
			if(!WriteType(buffer, temp->un.locals.local[3], 1))
			{
				return 1;
			}
		}
		temp = temp->un.locals.next;
		num -= 4;
	}
	return 1;
}

/*
 * Write the signature encoding for a type.
 */
static int WriteType(SigBuffer *buffer, ILType *type, int methodPtr)
{
	unsigned long value;
	ILClass *info;
	const char *name;
	const char *namespace;

	if(ILType_IsPrimitive(type))
	{
		/* Write a primitive type */
		value = ILType_ToElement(type);
		SIG_WRITE(value);
	}
	else if(ILType_IsClass(type))
	{
		/* Encode a reference to a class */
		info = ILType_ToClass(type);
		name = ILClass_Name(info);
		namespace = ILClass_Namespace(info);
		if(namespace && !strcmp(namespace, "System") &&
		   !ILClass_NestedParent(info))
		{
			if(!strcmp(name, "Object"))
			{
				SIG_WRITE(IL_META_ELEMTYPE_OBJECT);
				return 1;
			}
			else if(!strcmp(name, "String"))
			{
				SIG_WRITE(IL_META_ELEMTYPE_STRING);
				return 1;
			}
		}
		SIG_WRITE(IL_META_ELEMTYPE_CLASS);
		return WriteClassToken(buffer, ILType_ToClass(type));
	}
	else if(ILType_IsValueType(type))
	{
		/* Encode a reference to a value type */
		info = ILType_ToClass(type);
		name = ILClass_Name(info);
		namespace = ILClass_Namespace(info);
		if(namespace && !strcmp(namespace, "System") &&
		   !ILClass_NestedParent(info))
		{
			if(!strcmp(name, "TypedReference"))
			{
				SIG_WRITE(IL_META_ELEMTYPE_TYPEDBYREF);
				return 1;
			}
		}
		SIG_WRITE(IL_META_ELEMTYPE_VALUETYPE);
		return WriteClassToken(buffer, ILType_ToValueType(type));
	}
	else if(type != ILType_Invalid)
	{
		/* Encode a complex type */
		switch(type->kind & 0xFF)
		{
			case IL_TYPE_COMPLEX_BYREF:
			{
				SIG_WRITE(IL_META_ELEMTYPE_BYREF);
				return WriteType(buffer, type->un.refType, 1);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_PTR:
			{
				SIG_WRITE(IL_META_ELEMTYPE_PTR);
				return WriteType(buffer, type->un.refType, 1);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_ARRAY:
			{
				if(type->un.array.size == 0 &&
				   type->un.array.lowBound == 0 &&
				   (!ILType_IsComplex(type->un.array.elemType) ||
				    type->un.array.elemType->kind != IL_TYPE_COMPLEX_ARRAY))
				{
					/* Single dimensional array with no specified bounds */
					SIG_WRITE(IL_META_ELEMTYPE_SZARRAY);
					return WriteType(buffer, type->un.array.elemType, 1);
				}
				else
				{
					/* Other types of arrays */
					unsigned long rank;
					unsigned long sizes;
					int needSizes;
					int needLowBounds;
					ILType *elemType;

					/* Determine the array rank, number of explicit sizes,
					   and the type for the inner elements */
					rank = 0;
					needSizes = 0;
					needLowBounds = 0;
					elemType = type;
					while(ILType_IsComplex(elemType) &&
					      elemType->kind == IL_TYPE_COMPLEX_ARRAY)
					{
						++rank;
						if(elemType->un.array.lowBound != 0 ||
						   elemType->un.array.size != 0)
						{
							needSizes = 1;
						}
						if(elemType->un.array.lowBound != 0)
						{
							needLowBounds = 1;
						}
						elemType = elemType->un.array.elemType;
					}
					if(needSizes)
					{
						sizes = rank;
					}
					else
					{
						sizes = 0;
					}

					/* Encode the array */
					SIG_WRITE(IL_META_ELEMTYPE_ARRAY);
					if(!WriteType(buffer, elemType, 1))
					{
						return 0;
					}
					if(!WriteValue(buffer, rank))
					{
						return 0;
					}
					if(!WriteValue(buffer, sizes))
					{
						return 0;
					}
					if(needSizes)
					{
						elemType = type;
						while(ILType_IsComplex(elemType) &&
						      elemType->kind == IL_TYPE_COMPLEX_ARRAY)
						{
							if(!WriteIntValue(buffer, elemType->un.array.size))
							{
								return 0;
							}
							elemType = elemType->un.array.elemType;
						}
					}
					if(needLowBounds)
					{
						if(!WriteValue(buffer, sizes))
						{
							return 0;
						}
						elemType = type;
						while(ILType_IsComplex(elemType) &&
						      elemType->kind == IL_TYPE_COMPLEX_ARRAY)
						{
							if(!WriteIntValue(buffer,
											  elemType->un.array.lowBound))
							{
								return 0;
							}
							elemType = elemType->un.array.elemType;
						}
					}
				}
			}
			break;

			case IL_TYPE_COMPLEX_CMOD_REQD:
			{
				SIG_WRITE(IL_META_ELEMTYPE_CMOD_REQD);
				if(!WriteClassToken(buffer, type->un.modifier.info))
				{
					return 0;
				}
				return WriteType(buffer, type->un.modifier.type, 1);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_CMOD_OPT:
			{
				SIG_WRITE(IL_META_ELEMTYPE_CMOD_OPT);
				if(!WriteClassToken(buffer, type->un.modifier.info))
				{
					return 0;
				}
				return WriteType(buffer, type->un.modifier.type, 1);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_PROPERTY:
			{
				SIG_WRITE(IL_META_CALLCONV_PROPERTY);
				if(!WriteValue(buffer, (unsigned long)(type->num)))
				{
					return 0;
				}
				if(!WriteType(buffer, type->un.method.retType, 1))
				{
					return 0;
				}
				return WriteMethodParams(buffer, type);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_SENTINEL:
			{
				SIG_WRITE(IL_META_ELEMTYPE_SENTINEL);
			}
			break;

			case IL_TYPE_COMPLEX_PINNED:
			{
				SIG_WRITE(IL_META_ELEMTYPE_PINNED);
				return WriteType(buffer, type->un.refType, 1);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_LOCALS:
			{
				SIG_WRITE(IL_META_CALLCONV_LOCAL_SIG);
				if(!WriteValue(buffer, (unsigned long)(type->num)))
				{
					return 0;
				}
				return WriteLocalVars(buffer, type);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_METHOD:
			case (IL_TYPE_COMPLEX_METHOD | IL_TYPE_COMPLEX_METHOD_SENTINEL):
			{
				/* Write out a method signature */
				if(methodPtr)
				{
					SIG_WRITE(IL_META_ELEMTYPE_FNPTR);
				}

				/* Write the calling conventions */
				SIG_WRITE(type->kind >> 8);

				/* Write the number of parameters */
				if((type->kind & IL_TYPE_COMPLEX_METHOD_SENTINEL) != 0)
				{
					/* Subtract one from the count if a sentinel is present */
					if(!WriteValue(buffer, ((unsigned long)(type->num)) - 1))
					{
						return 0;
					}
				}
				else
				{
					if(!WriteValue(buffer, ((unsigned long)(type->num))))
					{
						return 0;
					}
				}

				/* Write the return type */
				if(!WriteType(buffer, type->un.method.retType, 1))
				{
					return 0;
				}

				/* Write the parameters */
				return WriteMethodParams(buffer, type);
			}
			/* Not reached */
		}
	}
	return 1;
}

unsigned long ILTypeToMethodSig(ILImage *image, ILType *type)
{
	SigBuffer buffer;
	unsigned long offset;
	SIG_INIT();
	if(!WriteType(&buffer, type, 0))
	{
		SIG_DESTROY();
		return 0;
	}
	offset = ILImageAddBlob(image, buffer.buf, (unsigned long)(buffer.posn));
	SIG_DESTROY();
	return offset;
}

unsigned long ILTypeToFieldSig(ILImage *image, ILType *type)
{
	SigBuffer buffer;
	unsigned long offset;
	SIG_INIT();
	buffer.buf[(buffer.posn)++] = (unsigned char)IL_META_CALLCONV_FIELD;
	if(!WriteType(&buffer, type, 1))
	{
		SIG_DESTROY();
		return 0;
	}
	offset = ILImageAddBlob(image, buffer.buf, (unsigned long)(buffer.posn));
	SIG_DESTROY();
	return offset;
}

unsigned long ILTypeToOtherSig(ILImage *image, ILType *type)
{
	SigBuffer buffer;
	unsigned long offset;
	SIG_INIT();
	if(!WriteType(&buffer, type, 1))
	{
		SIG_DESTROY();
		return 0;
	}
	offset = ILImageAddBlob(image, buffer.buf, (unsigned long)(buffer.posn));
	SIG_DESTROY();
	return offset;
}

#ifdef	__cplusplus
};
#endif
