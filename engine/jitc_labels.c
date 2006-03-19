/*
 * jitc_labels.c - Jit coder label management routines.
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

/*
 * Save the current jitStack status to the label.
 * This is done when the label is referenced the first time.
 */
static int _ILJitLabelSaveStack(ILJITCoder *coder, ILJITLabel *label)
{
	if(((label->labelType & (_IL_JIT_LABEL_NORMAL |
							 _IL_JIT_LABEL_STARTCATCH)) != 0) &&
		(coder->stackTop > 0))
	{
		int current = 0;
		ILJitValue *stack = ILMemStackAllocItem(&(coder->stackStates),
									coder->stackTop * sizeof(ILJitValue));
		if(!stack)
		{
			return 0;
		}
		/* Now save the current stack state. */
		for(current = 0; current < coder->stackTop; current++)
		{
			ILJitValue value = coder->jitStack[current];

			if(jit_value_is_constant(value))
			{
				/* We have to handle this case different. */
				/* Create a local value of the type of the constant. */
				ILJitValue temp = jit_value_create(coder->jitFunction,
												   jit_value_get_type(value));
				/* and store the value o the constant in the new temporary. */
				jit_insn_store(coder->jitFunction, temp, value);
				/* Now replace the constant with the new temporary. */
				coder->jitStack[current] = temp;
			}
			else
			{
				if(_ILJitValueIsArgOrLocal(coder, coder->jitStack[current]))
				{
					coder->jitStack[current] = jit_insn_dup(coder->jitFunction,
													coder->jitStack[current]);
				}
			}
			stack[current] = coder->jitStack[current];
		}
		label->jitStack = stack;
		label->stackSize = coder->stackTop;
	}
	return 1;
}

/*
 * Merge the current jitStack status with the one saved in the label.
 */
static int _ILJitLabelMergeStack(ILJITCoder *coder, ILJITLabel *label)
{
	if(label->labelType & (_IL_JIT_LABEL_NORMAL | _IL_JIT_LABEL_STARTCATCH))
	{
		/* Verify that the stack sizes match. */
		if(coder->stackTop != label->stackSize)
		{
			fprintf(stdout, "Stack sizes don't match!\n");
			/* return 0; */
		}
		if(coder->stackTop > 0)
		{
			int numItems = coder->stackTop > label->stackSize ?
								label->stackSize : coder->stackTop;
			int current = 0;

			/* Now save the current stack state. */
			for(current = 0; current < numItems; current++)
			{
				ILJitValue value = coder->jitStack[current];

				if(value != label->jitStack[current])
				{
					/* store the changed value to the saved stack. */
					jit_insn_store(coder->jitFunction, label->jitStack[current],
								   value);
				}
			}
		}
	}
	return 1;
}

/*
 * Restore the stack from the label to the coder.
 */
static void _ILJitLabelRestoreStack(ILJITCoder *coder, ILJITLabel *label)
{
	if(label->stackSize > 0)
	{
		int current = 0;

		/* Now restore the stack state. */
		for(current = 0; current < label->stackSize; current++)
		{
			coder->jitStack[current] = label->jitStack[current];
		}
	}
	coder->stackTop = label->stackSize;
}

/*
 * Look for an existing label for the specified IL address.
 * Returns 0 if there is no label for this address.
 */
static ILJITLabel *_ILJitLabelFind(ILJITCoder *coder, ILUInt32 address)
{
	ILJITLabel *label = coder->labelList;

	while(label != 0)
	{
		if(label->address == address)
		{
			return label;
		}
		label = label->next;
	}
	return 0;
}

/*
 * Look for a label for a specific IL address.  Create
 * a new label if necessary.
 * This function handles the jit stack changes on label invocation too.
 */
static ILJITLabel *_ILJitLabelGet(ILJITCoder *coder, ILUInt32 address,
													 int labelType)
{
	ILJITLabel *label = _ILJitLabelFind(coder, address);
;
	if(!label)
	{
		label = ILMemPoolAlloc(&(coder->labelPool), ILJITLabel);
		if(label)
		{
			label->stackSize = 0;
			label->jitStack = 0;
			label->address = address;
			label->label = jit_label_undefined;
			label->labelType = labelType;
			if(!_ILJitLabelSaveStack(coder, label))
			{
				coder->labelOutOfMemory = 1;
				return 0;
			}
			label->next = coder->labelList;
			coder->labelList = label;
		}
		else
		{
			coder->labelOutOfMemory = 1;
		}
	}
	else
	{
		if(!_ILJitLabelMergeStack(coder, label))
		{
			/* We have a stack size mismatch!!! */
			coder->labelOutOfMemory = 1;
			return 0;
		}
	}
	return label;
}


