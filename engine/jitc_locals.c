/*
 * jitc_locals.c - Handle function arguments and locals during code generation.
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
 * Allocate enough space for "n" slots.
 */
#define	_ILJitLocalsAlloc(s, n)	\
			do { \
				ILUInt32 temp = (ILUInt32)(((n) + 7) & ~7); \
				if(temp > (s).maxSlots) \
				{ \
					ILJitLocalSlot *newSlots = \
						(ILJitLocalSlot *)ILRealloc((s).slots, \
											  temp * sizeof(ILJitLocalSlot)); \
					if(!newSlots) \
					{ \
						return 0; \
					} \
					(s).slots = newSlots; \
					(s).maxSlots = temp; \
				} \
			} while (0)

/*
 * Get the slot for a local value.
 */
#define _ILJitLocalGet(coder, n) ((coder)->jitLocals.slots[(n)])

/*
 * Access the flags member of a local slot.
 */
#define _ILJitLocalFlags(coder, n) _ILJitLocalGet((coder), (n)).flags

/*
 * Access the ptrToValue member of a locals slot.
 */
#define _ILJitLocalPointer(coder, n) _ILJitLocalGet((coder), (n)).ptrToValue

/*
 * Access the value member of a locals slot.
 */
#define _ILJitLocalValue(coder, n) _ILJitLocalGet((coder), (n)).value

/*
 * Get the slot for a param value.
 */
#define _ILJitParamGet(coder, n) ((coder)->jitParams.slots[(n)])

/*
 * Access the flags member of a local slot.
 */
#define _ILJitParamFlags(coder, n) _ILJitParamGet((coder), (n)).flags

/*
 * Access the ptrToValue member of a locals slot.
 */
#define _ILJitParamPointer(coder, n) _ILJitParamGet((coder), (n)).ptrToValue

/*
 * Access the value member of a locals slot.
 */
#define _ILJitParamValue(coder, n) _ILJitParamGet((coder), (n)).value


/*
 * Get the pointer to a local.
 * The pointer is created on the first time.
 */
static ILJitValue _ILJitLocalGetPointerTo(ILJITCoder *coder,
										  ILUInt32 localNum)
{
	ILJitLocalSlot *slot = &_ILJitLocalGet(coder, localNum);

	return jit_insn_address_of(coder->jitFunction, slot->value);
}

/*
 * Get the pointer to a parameter.
 * The pointer is created on the first time.
 */
static ILJitValue _ILJitParamGetPointerTo(ILJITCoder *coder,
										  ILUInt32 paramNum)
{
	ILJitLocalSlot *slot = &_ILJitParamGet(coder, paramNum);

	return jit_insn_address_of(coder->jitFunction, slot->value);
}

/*
 * Create the slots for the declared local variables.
 * Returns zero if out of memory.
 */
static int _ILJitLocalsCreate(ILJITCoder *coder, ILStandAloneSig *localVarSig)
{
	ILType *signature;
	ILType *type;
	ILJitType jitType;
	ILUInt32 num;
	ILUInt32 current;

	if(localVarSig)
	{
		/* Determine the number of locals to allocate */
		signature = ILStandAloneSigGetType(localVarSig);
		num = ILTypeNumLocals(signature);

		/* Allocate the "jitLocals" array for the variables */
		_ILJitLocalsAlloc(coder->jitLocals, num);

		/* Set the offsets for each of the local variables */
		for(current = 0; current < num; ++current)
		{
			ILJitLocalSlot *local = &_ILJitLocalGet(coder, current);

			type = ILTypeGetLocal(signature, current);
			if(!(jitType = _ILJitGetLocalsType(type, coder->process)))
			{
				return 0;
			}
			if(!(local->value = jit_value_create(coder->jitFunction, jitType)))
			{
				return 0;
			}
			local->flags = 0;
		}
		/* Record the number of used locals in the coder. */
		coder->jitLocals.numSlots = num;
	}
	else
	{
		/* Set the number of used locals to 0. */
		coder->jitLocals.numSlots = 0;
	}

	return 1;
}

/*
 * Create the slots for the fixed parameters of the current function.
 * Returns 0 on error else 1.
 */
static int _ILJitParamsCreate(ILJITCoder *coder)
{
	ILJitType signature = jit_function_get_signature(coder->jitFunction);

	if(signature)
	{
		ILJitLocalSlot *param = 0;
		ILUInt32 numParams = jit_type_num_params(signature);

		/* We don't include the ILExecThread in the params. */
		if(numParams > 1)
		{
			ILInt32 current = 0;

			/* Allocate the locals for the parameters */
			_ILJitLocalsAlloc(coder->jitParams, numParams - 1);

			for(current = 1; current < numParams; ++current)
			{
				param = &_ILJitParamGet(coder, current - 1);

				param->value = jit_value_get_param(coder->jitFunction, current);
				param->flags = 0;
			}
			coder->jitParams.numSlots = numParams - 1;
		}
		else
		{
			coder->jitParams.numSlots = 0;
		}
		return 1;
	}
	return 0;
}

/*
 * Check if the given value is a parameter or local.
 * Returns 1 if the value is either a param or local value or pointer to
 * a param or local otherwise 0.
 */
static ILInt32 _ILJitValueIsArgOrLocal(ILJITCoder *coder, ILJitValue value)
{
	ILInt32 current;
	ILJitLocalSlot *slot = 0;

	for(current = 0; current < coder->jitLocals.numSlots; ++current)
	{
		slot = &_ILJitLocalGet(coder, current);

		if(slot->value == value)
		{
			return 1;
		}
	}

	for(current = 0; current < coder->jitParams.numSlots; ++current)
	{
		slot = &_ILJitParamGet(coder, current);

		if(slot->value == value)
		{
			return 1;
		}
	}

	return 0;
}

#ifdef _IL_JIT_OPTIMIZE_LOCALS

/*
 * Find the given value on the stack and replace it by a duplicate.
 */
static void _ILJitValueFindAndDup(ILJITCoder *coder, ILJitValue value)
{
	ILJitValue dupValue = 0;
	ILInt32 stackPos = 0;

	for(stackPos = 0; stackPos < coder->stackTop; ++stackPos)
	{
		if(coder->jitStack[stackPos] == value)
		{
			if(!dupValue)
			{
				dupValue = jit_insn_dup(coder->jitFunction, value);
			}
			coder->jitStack[stackPos] = dupValue;
		}
	}
}
#endif

#ifdef _IL_JIT_OPTIMIZE_NULLCHECKS

#define _IL_JIT_VALUE_NULLCHECKED 1

#define _ILJitValuesResetNullChecked(coder) \
		_ILJitValuesSetFlags((coder), 0, _IL_JIT_VALUE_NULLCHECKED)
#endif

/*
 * Find the slot for the given value.
 * Returns the slot with the valuewhen found otherwise 0.
 */
static ILJitLocalSlot *_ILJitValueFindSlot(ILJITCoder *coder, ILJitValue value)
{
	ILInt32 current;
	ILJitLocalSlot *slot = 0;

	for(current = 0; current < coder->jitLocals.numSlots; ++current)
	{
		slot = &_ILJitLocalGet(coder, current);

		if(slot->value == value)
		{
			return slot;
		}
	}

	for(current = 0; current < coder->jitParams.numSlots; ++current)
	{
		slot = &_ILJitParamGet(coder, current);

		if(slot->value == value)
		{
			return slot;
		}
	}
	return 0;
}

/*
 * Set or reset flags for the given value.
 */
static void _ILJitValueSetFlags(ILJITCoder *coder, ILJitValue value,
								ILUInt32 setFlags, ILUInt32 resetFlags)
{
	ILJitLocalSlot *slot = _ILJitValueFindSlot(coder, value);

	if(slot)
	{
		slot->flags |= setFlags;
		slot->flags &= ~resetFlags;
	}
}

/*
 * Set or reset flags for all locals / parameters.
 */
static void _ILJitValuesSetFlags(ILJITCoder *coder,
								 ILUInt32 setFlags, ILUInt32 resetFlags)
{

	ILInt32 current;
	ILJitLocalSlot *slot = 0;

	for(current = 0; current < coder->jitLocals.numSlots; ++current)
	{
		slot = &_ILJitLocalGet(coder, current);

		slot->flags |= setFlags;
		slot->flags &= ~resetFlags;
	}

	for(current = 0; current < coder->jitParams.numSlots; ++current)
	{
		slot = &_ILJitParamGet(coder, current);

		slot->flags |= setFlags;
		slot->flags &= ~resetFlags;
	}
}

/*
 * Throw a NullReferenceException if address.is 0.
 */
static void _ILJitCheckNull(ILJITCoder *coder, ILJitValue address)
{
#ifdef _IL_JIT_OPTIMIZE_NULLCHECKS
	ILJitLocalSlot *slot = 0;
#endif

	if(jit_value_is_constant(address))
	{
		/* We assume that the type of the constant is a pointer type. */
		if(jit_value_get_nint_constant(address) != 0)
		{
			/* We don't have to do a null check for a non null constant. */
			return;
		}
	}
#ifdef _IL_JIT_OPTIMIZE_NULLCHECKS
	if((slot = _ILJitValueFindSlot(coder, address)))
	{
		if((slot->flags & _IL_JIT_VALUE_NULLCHECKED) == 0)
		{
			jit_insn_check_null(coder->jitFunction, address);
			slot->flags |= _IL_JIT_VALUE_NULLCHECKED;
		}
		return;
	}
#endif
	jit_insn_check_null(coder->jitFunction, address);
}


