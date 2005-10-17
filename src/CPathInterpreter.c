/*
 * SDPathInterpreter.c - Path interpreter implementation.
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

#include "CPathInterpreter.h"

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL SDStatus
SDPathInterpreter_Interpret(SDPathInterpreter *_this,
                            const SDPointF    *points,
                            const SDByte      *types,
                            SDUInt32           count)
{
	/* declarations */
	const SDPointF *currP;
	const SDPointF *end;
	const SDByte   *currT;

	/* assertions */
	SDASSERT((_this                != 0));
	SDASSERT((_this->_class        != 0));
	SDASSERT((_this->_class->Move  != 0));
	SDASSERT((_this->_class->Line  != 0));
	SDASSERT((_this->_class->Curve != 0));
	SDASSERT((_this->_class->Close != 0));
	SDASSERT((points               != 0));
	SDASSERT((types                != 0));

	/* get the type input pointer */
	currT = types;

	/* get the point input pointer */
	currP = points;

	/* get the end of input pointer */
	end  = (currP + count);

	/* interpret the path */
	while(currP != end)
	{
		/* declarations */
		SDByte type;

		/* get the current type */
		type = (*currT & SDPathType_TypeMask);

		/* process point based on type */
		if(type == SDPathType_Line)
		{
			/* line to point */
			SDStatus_Check
				(SDPathInterpreter_Line
					(_this, SDPoint_X(*currP), SDPoint_Y(*currP), *currT));
		}
		else if(type == SDPathType_Bezier)
		{
			/* declarations */
			SDFloat x1, y1, x2, y2, x3, y3;

			/* assertions */
			SDASSERT(((currP + 2) != end));

			/* get the first point coordinates */
			x1 = SDPoint_X(*currP);
			y1 = SDPoint_Y(*currP);

			/* advance to the second point */
			++currP; ++currT;

			/* assertions */
			SDASSERT(((*currT & SDPathType_TypeMask) == SDPathType_Bezier));

			/* get the second point coordinates */
			x2 = SDPoint_X(*currP);
			y2 = SDPoint_Y(*currP);

			/* advance to the third point */
			++currP; ++currT;

			/* assertions */
			SDASSERT(((*currT & SDPathType_TypeMask) == SDPathType_Bezier));

			/* get the third point coordinates */
			x3 = SDPoint_X(*currP);
			y3 = SDPoint_Y(*currP);

			/* curve to point */
			SDStatus_Check
				(SDPathInterpreter_Curve
					(_this, x1, y1, x2, y2, x3, y3, *currT));
		}
		else
		{
			/* assertions */
			SDASSERT((type == SDPathType_Start));

			/* move to point */
			SDStatus_Check
				(SDPathInterpreter_Move
					(_this, SDPoint_X(*currP), SDPoint_Y(*currP), *currT));
		}

		/* close the subpath, as needed */
		if((*currT & SDPathType_CloseSubpath) != 0)
		{
			SDStatus_Check
				(SDPathInterpreter_Close
					(_this));
        }

		/* move to the next input position */
		++currP; ++currT;
	}

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
