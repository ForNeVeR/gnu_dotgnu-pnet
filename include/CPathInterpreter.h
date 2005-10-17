/*
 * SDPathInterpreter.h - Path interpreter header.
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

#ifndef _SD_PATHINTERPRETER_H_
#define _SD_PATHINTERPRETER_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagSDPathInterpreter      SDPathInterpreter;
typedef struct _tagSDPathInterpreterClass SDPathInterpreterClass;


struct _tagSDPathInterpreter
{
	const SDPathInterpreterClass *_class;
};

struct _tagSDPathInterpreterClass
{
	/*\
	|*| Move to the given coordinates.
	|*|
	|*|      x - the x coordinate
	|*|      y - the y coordinate
	|*|   type - the type of the point
	|*|
	|*|  Returns status code.
	\*/
	SDStatus (*Move)(SDPathInterpreter *_this,
	                 SDFloat            x,
	                 SDFloat            y,
                     SDPathType         type);

	/*\
	|*| Line to the given coordinates.
	|*|
	|*|      x - the x coordinate
	|*|      y - the y coordinate
	|*|   type - the type of the point
	|*|
	|*|  Returns status code.
	\*/
	SDStatus (*Line)(SDPathInterpreter *_this,
	                 SDFloat            x,
	                 SDFloat            y,
                     SDPathType         type);

	/*\
	|*| Curve to the given coordinates.
	|*|
	|*|     x1 - the first x coordinate
	|*|     y1 - the first y coordinate
	|*|     x2 - the second x coordinate
	|*|     y2 - the second y coordinate
	|*|     x3 - the third x coordinate
	|*|     y3 - the third y coordinate
	|*|   type - the type of the third point
	|*|
	|*|  Returns status code.
	\*/
	SDStatus (*Curve)(SDPathInterpreter *_this,
	                  SDFloat            x1,
	                  SDFloat            y1,
	                  SDFloat            x2,
	                  SDFloat            y2,
	                  SDFloat            x3,
	                  SDFloat            y3,
                      SDPathType         type);

	/*\
	|*| Close the path.
	|*|
	|*|  Returns status code.
	\*/
	SDStatus (*Close)(SDPathInterpreter *_this);

	/*\
	|*| Sentinel string used to catch missing methods in class tables.
	\*/
	const char *sentinel;
};

SDINTERNAL SDStatus
SDPathInterpreter_Interpret(SDPathInterpreter *_this,
                            const SDPointF    *points,
                            const SDByte      *types,
                            SDUInt32           count);

#define SDPathInterpreter_Move(_this, x, y, t) \
	((_this)->_class->Move((_this), (x), (y), (t)))
#define SDPathInterpreter_Line(_this, x, y, t) \
	((_this)->_class->Line((_this), (x), (y), (t)))
#define SDPathInterpreter_Curve(_this, x1, y1, x2, y2, x3, y3, t) \
	((_this)->_class->Curve((_this), (x1), (y1), (x2), (y2), (x3), (y3), (t)))
#define SDPathInterpreter_Close(_this) \
	((_this)->_class->Close((_this)))

#ifdef __cplusplus
};
#endif

#endif /* _SD_PATHINTERPRETER_H_ */
