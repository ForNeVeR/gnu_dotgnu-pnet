/*
 * SDRegionInterpreter.c - Region interpreter implementation.
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

#include "CRegionInterpreter.h"

#ifdef __cplusplus
extern "C" {
#endif

static const SDRegionInterpreter SDRegionInterpreter_Zero;

SDINTERNAL void
SDRegionInterpreter_Initialize(SDRegionInterpreter          *_this,
                               const SDRegionInterpreterClass *_class)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the stack */
	SDRegionStack_Initialize(&(_this->stack));

	/* initialize the class */
	_this->_class = _class;
}

SDINTERNAL void
SDRegionInterpreter_Finalize(SDRegionInterpreter *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the stack */
	SDRegionStack_Finalize(&(_this->stack));

	/* finalize the remaining members */
	_this->_class = 0;
}

#define SDRegionInterpreter_Op(_this, op, left, right, data) \
	((_this)->_class->Op((_this), (op), (left), (right), (data)))
#define SDRegionInterpreter_Data(_this, node, data) \
	((_this)->_class->Data((_this), (node), (data)))
#define SDRegionInterpreter_FreeData(_this, data) \
	((_this)->_class->FreeData((data)))

SDINTERNAL SDStatus
SDRegionInterpreter_Interpret(SDRegionInterpreter  *_this,
                              SDRegionNode         *head,
                              void                **data)
{
	/* declarations */
	SDRegionStack *stack;
	void          *tmp;
	SDStatus       status;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((head  != 0));
	SDASSERT((data  != 0));

	/* get the stack pointer */
	stack = &(_this->stack);

	/* reset the stack */
	SDRegionStack_Reset(&stack);

	/* handle the trivial case */
	if(SDRegionNode_IsData(head))
	{
		return SDRegionInterpreter_Data(_this, head, data);
	}
	else
	{
		/* set the data to the default */
		*data = 0;
		tmp   = 0;

		/* push the head */
		SDStatus_Check
			(SDRegionStack_Push
				(&stack, (SDRegionOp *)head));
	}

	/* interpret the region */
	while(_this->stack.count > 0)
	{
		/* get the top of the stack */
		SDRegionStackNode *top = &SDRegionStack_Top(*stack);

		/* handle the current node */
		if(top->visited == 0)
		{
			/* update the visited flag */
			++(top->visited);

			/* handle the left operand node */
			if(top->op->left == 0)
			{
				tmp = 0;
			}
			else if(SDRegionNode_IsData(top->op->left))
			{
				/* process the data */
				status =
					SDRegionInterpreter_Data
						(_this, top->op->left, &tmp);

				/* handle data failures */
				if(status != SDStatus_OK) { goto GOTO_FreeData; }
			}
			else
			{
				/* push the operation */
				status =
					SDRegionStack_Push
						(&stack, ((SDRegionOp *)top->op->left));

				/* handle push failures */
				if(status != SDStatus_OK) { goto GOTO_FreeData; }
			}
		}
		else if(top->visited == 1)
		{
			/* update the visited flag */
			++(top->visited);

			/* set the left operand data */
			top->left = tmp;

			/* handle the right operand node */
			if(top->op->right == 0)
			{
				tmp = 0;
			}
			else if(SDRegionNode_IsData(top->op->right))
			{
				/* process the data */
				status =
					SDRegionInterpreter_Data
						(_this, top->op->right, &tmp);

				/* handle data failures */
				if(status != SDStatus_OK) { goto GOTO_FreeData; }
			}
			else
			{
				/* push the operation */
				status =
					SDRegionStack_Push
						(&stack, ((SDRegionOp *)top->op->right));

				/* handle push failures */
				if(status != SDStatus_OK) { goto GOTO_FreeData; }
			}
		}
		else
		{
			/* set the right operand data */
			top->right = tmp;

			/* perform the operation */
			status =
				SDRegionInterpreter_Op
					(_this, top->op, top->left, top->right, &tmp);

			/* pop the operation */
			SDRegionStack_Pop(&stack);

			/* handle operation failures */
			if(status != SDStatus_OK) { goto GOTO_FreeData; }
		}
	}

	/* set the data */
	*data = tmp;

	/* return successfully */
	return SDStatus_OK;

GOTO_FreeData:

	/* free the data */
	if(_this->_class->FreeData != 0)
	{
		/* free the data */
		while(_this->stack.count > 0)
		{
			/* get the top of the stack */
			SDRegionStackNode *top = &SDRegionStack_Top(*stack);

			/* free the left data, as needed */
			if(top->left != 0)
			{
				SDRegionInterpreter_FreeData(_this, top->left);
				top->left = 0;
			}

			/* free the right data, as needed */
			if(top->right != 0)
			{
				SDRegionInterpreter_FreeData(_this, top->right);
				top->right = 0;
			}

			/* pop the operation */
			SDRegionStack_Pop(&stack);
		}
	}

	/* return status */
	return status;
}


#ifdef __cplusplus
};
#endif
