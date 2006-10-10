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

#ifdef IL_JITC_DECLARATIONS

/*
 * Define the structure of a local/argument slot.
 */
typedef struct _tagILJitLocalSlot ILJitLocalSlot;
struct _tagILJitLocalSlot
{
	ILJitValue	value;			/* the ILJitValue */
	ILJitValue	refValue;		/* the address of the value */
	ILUInt32	flags;			/* State of the local/arg. */
};

#define ILJitLocalSlotValue(localSlot)	(localSlot).value

/*
 * Define the structure for managing the local slots.
 */
typedef struct _tagILJitLocalSlots ILJitLocalSlots;
struct _tagILJitLocalSlots
{
	ILJitLocalSlot *slots;		/* Pointer to the slots. */
	int				numSlots;	/* Number of used slots. */
	int				maxSlots;	/* Number of allocated slots. */
};

#define _ILJitLocalSlotsInit(s) \
	do { \
		(s).slots = 0;		\
		(s).numSlots = 0;	\
		(s).maxSlots = 0;	\
	} while (0);

#define _ILJitLocalSlotsDestroy(s) \
	do { \
		if((s).slots)			\
		{						\
			ILFree((s).slots);	\
		}						\
		(s).slots = 0;			\
		(s).numSlots = 0;		\
		(s).maxSlots = 0;		\
	} while (0);

/*
 * definitions used in the slot's flags
 */
#define _IL_JIT_VALUE_NULLCHECKED 	0x00000001
#define _IL_JIT_VALUE_INITIALIZED 	0x00000002

/*
 * additional flags used on the evaluation stack.
 */
#define _IL_JIT_VALUE_COPYOF		0x00000100	/* complete copy of the local/arg value */
#define _IL_JIT_VALUE_POINTER_TO	0x00000200	/* pointer to the local/arg value */

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
#define _ILJitLocalPointer(coder, n) _ILJitLocalGet((coder), (n)).refValue

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
#define _ILJitParamPointer(coder, n) _ILJitParamGet((coder), (n)).refValue

/*
 * Access the value member of a locals slot.
 */
#define _ILJitParamValue(coder, n) _ILJitParamGet((coder), (n)).value

#endif /* IL_JITC_DECLARATIONS */

#ifdef	IL_JITC_CODER_INSTANCE

	/* Members to manage the fixed arguments. */
	ILJitLocalSlots	jitParams;

	/* Members to manage the local variables. */
	ILJitLocalSlots jitLocals;
#ifdef _IL_JIT_OPTIMIZE_INIT_LOCALS
	int				localsInitialized;
#endif

#endif	/* IL_JITC_CODER_INSTANCE */

#ifdef	IL_JITC_CODER_INIT

	/* Initialize the parameter management. */
	_ILJitLocalSlotsInit(coder->jitParams)

	/* Initialize the locals management. */
	_ILJitLocalSlotsInit(coder->jitLocals)

#endif	/* IL_JITC_CODER_INIT */

#ifdef	IL_JITC_CODER_DESTROY

	_ILJitLocalSlotsDestroy(coder->jitLocals)

	_ILJitLocalSlotsDestroy(coder->jitParams)

#endif	/* IL_JITC_CODER_DESTROY */

#ifdef IL_JITC_FUNCTIONS

/*
 * Get the pointer to a local.
 * The pointer is created on the first time.
 */
static ILJitValue _ILJitLocalGetPointerTo(ILJITCoder *coder,
										  ILUInt32 localNum)
{
	ILJitLocalSlot *slot = &_ILJitLocalGet(coder, localNum);

	/*
	if(!slot->refValue)
	{	
		slot->refValue = jit_insn_address_of(coder->jitFunction, slot->value);
	}
	return slot->refValue;
	*/
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

	/*
	if(!slot->refValue)
	{	
		slot->refValue = jit_insn_address_of(coder->jitFunction, slot->value);
	}
	return slot->refValue;
	*/
	return jit_insn_address_of(coder->jitFunction, slot->value);
}

/*
 * Initialize the local value to 0.
 */
static int _ILJitLocalInit(ILJITCoder *coder, ILJitLocalSlot *slot)
{
	if((slot->flags & _IL_JIT_VALUE_INITIALIZED) == 0)
	{
		ILJitType type = jit_value_get_type(slot->value);

		if(!jit_type_is_struct(type))
		{
			int typeKind = jit_type_get_kind(type);
			ILJitValue constant = 0;

			if(_JIT_TYPEKIND_IS_FLOAT(typeKind))
			{
				if(!(constant = jit_value_create_nfloat_constant(coder->jitFunction,
															type,
															(jit_nfloat)0)))
				{
					return 0;
				}
				jit_insn_store(coder->jitFunction, slot->value, constant);
			}
			else
			{
				if(_JIT_TYPEKIND_IS_LONG(typeKind))
				{
					if(!(constant = jit_value_create_long_constant(coder->jitFunction,
															  type, (jit_long)0)))
					{
						return 0;
					}
					jit_insn_store(coder->jitFunction, slot->value, constant);
				}
				else
				{
					if(!(constant = jit_value_create_nint_constant(coder->jitFunction,
															  type, (jit_nint)0)))
					{
						return 0;
					}
					jit_insn_store(coder->jitFunction, slot->value, constant);
				}
			}
		}
		slot->flags |= _IL_JIT_VALUE_INITIALIZED;
	}
	return 1;
}

/*
 * Initialize the not yet initialized local values to 0 and move the
 * initialization sequence to the start of the function.
 */
static int _ILJitLocalsInit(ILJITCoder *coder)
{
	ILUInt32 num = coder->jitLocals.numSlots;
	ILUInt32 current;

	if(num > 0)
	{
		jit_label_t startLabel = jit_label_undefined;
		jit_label_t endLabel = jit_label_undefined;
		ILJitLocalSlot *slot;

		if(!jit_insn_label(coder->jitFunction, &startLabel))
		{
			return 0;
		}

		for(current = 0; current < num; ++current)
		{
			slot = &_ILJitLocalGet(coder, current);

			if(!_ILJitLocalInit(coder, slot))
			{
				return 0;
			}
		}

		if(!jit_insn_label(coder->jitFunction, &endLabel))
		{
			return 0;
		}
		if(!jit_insn_move_blocks_to_start(coder->jitFunction, startLabel,
															  endLabel))
		{
			return 0;
		}
	}
	return 1;
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
#ifdef IL_DEBUGGER
	jit_value_t data1;
	jit_value_t data2;
#endif

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
			local->refValue = 0;

#ifdef IL_DEBUGGER
			/* Notify debugger about address of local variable */
			if(coder->markBreakpoints)
			{
				/* Make the variable accessible for debugger */
				jit_value_set_volatile(local->value);
				jit_value_set_addressable(local->value);

				/* Report address of the variable to debugger */
				data1 = jit_value_create_nint_constant(coder->jitFunction,
				 							jit_type_nint,
											JIT_DEBUGGER_DATA1_LOCAL_VAR_ADDR);

				data2 = jit_insn_address_of(coder->jitFunction,	local->value);
				jit_insn_mark_breakpoint_variable
											(coder->jitFunction, data1, data2);
			}
#endif

			local->flags = 0;
		}
		/* Record the number of used locals in the coder. */
		coder->jitLocals.numSlots = num;

#ifndef _IL_JIT_OPTIMIZE_INIT_LOCALS
		/* Initialize the locals. */
		if(!_ILJitLocalsInit(coder))
		{
			return 0;
		}
#endif
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

#ifdef IL_JIT_THREAD_IN_SIGNATURE
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
				param->refValue = 0;
			}
			coder->jitParams.numSlots = numParams - 1;
		}
#else
		if(numParams > 0)
		{
			ILInt32 current = 0;

			/* Allocate the locals for the parameters */
			_ILJitLocalsAlloc(coder->jitParams, numParams);

			for(current = 0; current < numParams; ++current)
			{
				param = &_ILJitParamGet(coder, current);

				param->value = jit_value_get_param(coder->jitFunction, current);
				param->flags = 0;
				param->refValue = 0;
			}
			coder->jitParams.numSlots = numParams;
		}
#endif
		else
		{
			coder->jitParams.numSlots = 0;
		}
		return 1;
	}
	return 0;
}

/*
 * Get the value of a local slot.
 * The value in the local slot is initialized to 0 if it is not yet initialized.
 */
static ILJitValue _ILJitLocalGetValue(ILJITCoder *coder, ILUInt32 localNum)
{
	ILJitLocalSlot *slot = &_ILJitLocalGet(coder, localNum);

	if((slot->flags & _IL_JIT_VALUE_INITIALIZED) == 0)
	{
		if(!_ILJitLocalInit(coder, slot))
		{
			return 0;
		}
	}
	return slot->value;
}

/*
 * Store a value in a local variable.
 */
static void _ILJitLocalStoreValue(ILJITCoder *coder, ILUInt32 localNum,
													 ILJitValue value)
{
	ILJitLocalSlot *slot = &_ILJitLocalGet(coder, localNum);

	jit_insn_store(coder->jitFunction, slot->value, 
				   _ILJitValueConvertImplicit(coder->jitFunction,
											  value,
											  jit_value_get_type(slot->value)));

	slot->flags |= _IL_JIT_VALUE_INITIALIZED;
	slot->flags &= ~_IL_JIT_VALUE_NULLCHECKED;
}

#ifdef _IL_JIT_OPTIMIZE_LOCALS

/*
 * Store a value known to be not null in a local variable.
 */
static void _ILJitLocalStoreNotNullValue(ILJITCoder *coder,
										 ILUInt32 paramNum,
										 ILJitValue value)
{
	ILJitLocalSlot *slot = &_ILJitLocalGet(coder, paramNum);

	jit_insn_store(coder->jitFunction, slot->value,
				   _ILJitValueConvertImplicit(coder->jitFunction,
											  value,
											  jit_value_get_type(slot->value)));

	slot->flags |= (_IL_JIT_VALUE_INITIALIZED | _IL_JIT_VALUE_NULLCHECKED);
}

#endif

/*
 * Get the value of a parameter slot.
 */
static ILJitValue _ILJitParamGetValue(ILJITCoder *coder, ILUInt32 paramNum)
{
	ILJitLocalSlot *slot = &_ILJitParamGet(coder, paramNum);

	return slot->value;
}

/*
 * Store a value in a parameter.
 */
static void _ILJitParamStoreValue(ILJITCoder *coder, ILUInt32 paramNum,
													 ILJitValue value)
{
	ILJitLocalSlot *slot = &_ILJitParamGet(coder, paramNum);

	jit_insn_store(coder->jitFunction, slot->value,
				   _ILJitValueConvertImplicit(coder->jitFunction,
											  value,
											  jit_value_get_type(slot->value)));

	slot->flags &= ~_IL_JIT_VALUE_NULLCHECKED;
}

#ifdef _IL_JIT_OPTIMIZE_LOCALS

/*
 * Store a value known to be not null in a parameter.
 */
static void _ILJitParamStoreNotNullValue(ILJITCoder *coder,
										 ILUInt32 paramNum,
										 ILJitValue value)
{
	ILJitLocalSlot *slot = &_ILJitParamGet(coder, paramNum);

	jit_insn_store(coder->jitFunction, slot->value,
				   _ILJitValueConvertImplicit(coder->jitFunction,
											  value,
											  jit_value_get_type(slot->value)));

	slot->flags |= _IL_JIT_VALUE_NULLCHECKED;
}

#endif

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

#ifdef _IL_JIT_OPTIMIZE_NULLCHECKS

#define _ILJitValuesResetNullChecked(coder) \
		_ILJitValuesSetFlags((coder), 0, _IL_JIT_VALUE_NULLCHECKED)
#endif

/*
 * Find the slot for the given value.
 * Returns the slot with the value when found otherwise 0.
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

#endif /* IL_JITC_FUNCTIONS */

