/*
 * cvm_call.c - Opcodes for performing calls to other methods.
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

#if defined(IL_CVM_GLOBALS)

/*
 * A little bit of assembler magic to stop gcc doing tail-end combination
 * on some of the switch cases related to method calls.
 */
#if defined(CVM_X86) && defined(__GNUC__) && !defined(IL_NO_ASM)
#define	CVM_OPTIMIZE_BLOCK()		__asm__ ("")
#else
#define	CVM_OPTIMIZE_BLOCK()
#endif

/*
 * Allocate a new call frame.
 */
ILCallFrame *_ILAllocCallFrame(ILExecThread *thread)
{
	ILUInt32 newsize;
	ILCallFrame *newframe;

	/* Calculate target frame size */
	newsize = thread->maxFrames * 2;

	/* Allocate the new frame and copy the old contents into it */
	if((newframe = (ILCallFrame *)ILGCAllocPersistent
				(sizeof(ILCallFrame) * newsize)) == 0)
	{
	    thread->thrownException = thread->process->outOfMemoryObject;
		return 0;
	}
	ILMemCpy(newframe, thread->frameStack,
			 sizeof(ILCallFrame) * thread->maxFrames);

	/* Free the old frame stack and copy the new one into place */
	ILGCFreePersistent(thread->frameStack);
	thread->frameStack = newframe;
	thread->maxFrames = newsize;

	/* Return the new frame to the caller */
	return &(thread->frameStack[(thread->numFrames)++]);
}

#ifdef IL_DUMP_CVM
#define	DUMP_STACK()	\
			do { \
				int posn; \
				fprintf(IL_DUMP_CVM_STREAM, "Stack:"); \
				for(posn = 1; posn <= 16; ++posn) \
				{ \
					if(posn <= (int)(stacktop - thread->stackBase)) \
					{ \
						fprintf(IL_DUMP_CVM_STREAM, " 0x%lX", \
							    (unsigned long)(stacktop[-posn].uintValue)); \
					} \
				} \
				putc('\n', IL_DUMP_CVM_STREAM); \
				fflush(IL_DUMP_CVM_STREAM); \
			} while (0)
#define	REPORT_METHOD_CALL()	\
			do { \
				fprintf(IL_DUMP_CVM_STREAM, "Entering %s::%s (%ld)\n", \
					    methodToCall->member.owner->name, \
					    methodToCall->member.name, \
					    (long)(stacktop - thread->stackBase)); \
				DUMP_STACK(); \
			} while (0)
#else
#define	REPORT_METHOD_CALL()
#endif
#define	ALLOC_CALL_FRAME()	\
			do { \
				REPORT_METHOD_CALL(); \
				if(thread->numFrames < thread->maxFrames) \
				{ \
					callFrame = &(thread->frameStack[(thread->numFrames)++]); \
				} \
				else \
				{ \
					callFrame = _ILAllocCallFrame(thread); \
					if(!callFrame) \
					{ \
						STACK_OVERFLOW_EXCEPTION(); \
					} \
				} \
			} while (0)

/*
 * Restore state information from the thread, except the pc.
 */
#define	RESTORE_STATE_FROM_THREAD()	\
			do { \
				stacktop = thread->stackTop; \
				frame = thread->frame; \
				stackmax = thread->stackLimit; \
				if(thread->thrownException != 0) \
				{ \
					/* An exception occurred, which we now must handle */ \
					pc = thread->pc; \
					stacktop[0].ptrValue = thread->thrownException; \
					thread->thrownException = 0; \
					stacktop += 1; \
					goto throwException; \
				} \
			} while (0)

#elif defined(IL_CVM_LOCALS)

ILMethod *methodToCall;
ILType *methodSignature;
ILCallFrame *callFrame;
ILInt32 numParams;

#elif defined(IL_CVM_MAIN)

/**
 * <opcode name="call" group="Call management instructions">
 *   <operation>Call a method which is internal to the CVM
 *              method cache</operation>
 *
 *   <format>call<fsep/>mptr</format>
 *
 *   <form name="call" code="COP_CALL"/>
 *
 *   <description>The <i>call</i> instruction effects a method
 *   call to <i>mptr</i>.</description>
 *
 *   <notes>The <i>mptr</i> value is a 32-bit or 64-bit method
 *   pointer reference.<p/>
 *
 *   The <i>call</i> instruction is typically used when the CVM
 *   bytecode translation process discovers that the method has
 *   already been translated.  Since it is not necessary to check
 *   for translation at run time, then engine can take a short cut.<p/>
 *
 *   See the description of the <i>call_extern</i> instruction for
 *   a full account of frame handling, argument handling, etc.</notes>
 * </opcode>
 */
case COP_CALL:
{
	/* Call a method that has already been converted into CVM code */
	methodToCall = (ILMethod *)(ReadPointer(pc + 1));

	/* Allocate a new call frame */
	ALLOC_CALL_FRAME();

	/* Fill in the call frame details */
	callFrame->method = method;
	callFrame->pc = pc + sizeof(void *) + 1;
	callFrame->frame = frame;
	callFrame->exceptHeight = thread->exceptHeight;

	/* Pass control to the new method */
	pc = (unsigned char *)(methodToCall->userData);
	method = methodToCall;
	CVM_OPTIMIZE_BLOCK();
#ifdef IL_PROFILE_CVM_METHODS
	++(method->count);
#endif
}
break;

/**
 * <opcode name="call_extern" group="Call management instructions">
 *   <operation>Call a method which may be external to the
 *              CVM method cache</operation>
 *
 *   <format>call_extern<fsep/>mptr</format>
 *
 *   <form name="call_extern" code="COP_CALL_EXTERN"/>
 *
 *   <description>The <i>call_extern</i> instruction effects a method
 *   call to <i>mptr</i>.  The call proceeds as follows:
 *
 *   <ul>
 *     <li>The method is converted into CVM bytecode.  If this is not
 *         possible, then <code>System.MissingMethodException</code>
 *         will be thrown.</li>
 *     <li>A new call frame is allocated.</li>
 *     <li>The current method, program counter, frame pointer, and
 *         exception frame height are saved into the call frame.</li>
 *     <li>The program counter is set to the first instruction in
 *         the method <i>mptr</i>.</li>
 *     <li>The exception frame height is set to zero.</li>
 *     <li>The current method is set to <i>mptr</i>.</li>
 *   </ul>
 *   </description>
 *
 *   <notes>The <i>mptr</i> value is a 32-bit or 64-bit method
 *   pointer reference.<p/>
 *
 *   The <i>call_extern</i> instruction does not set up a new frame pointer.
 *   The <i>set_num_args</i> instruction is used to set up local variable
 *   frames at the start of the new method's code.
 *
 *   The <i>return*</i> instructions are responsible for popping the
 *   method arguments from the stack at method exit.</notes>
 *
 *   <exceptions>
 *     <exception name="System.MissingMethodException">Raised if
 *     the method could not be translated into CVM bytecode.</exception>
 *   </exceptions>
 * </opcode>
 */
case COP_CALL_EXTERN:
{
	/* Call a method that we don't know if it has been converted */
	methodToCall = (ILMethod *)(ReadPointer(pc + 1));
	if(methodToCall->userData)
	{
		/* It is converted: allocate a new call frame */
		ALLOC_CALL_FRAME();

		/* Fill in the call frame details */
		callFrame->method = method;
		callFrame->pc = pc + sizeof(void *) + 1;
		callFrame->frame = frame;
		callFrame->exceptHeight = thread->exceptHeight;

		/* Pass control to the new method */
		pc = (unsigned char *)(methodToCall->userData);
		method = methodToCall;
		CVM_OPTIMIZE_BLOCK();
	}
	else
	{
		/* Copy the state back into the thread object */
		COPY_STATE_TO_THREAD();

		/* Convert the method */
		tempptr = (void *)(_ILConvertMethod(thread, methodToCall));
		if(!tempptr)
		{
			MISSING_METHOD_EXCEPTION();
		}

		/* Allocate a new call frame */
		ALLOC_CALL_FRAME();

		/* Fill in the call frame details */
		callFrame->method = method;
		callFrame->pc = thread->pc + 1 + sizeof(void *);
		callFrame->frame = thread->frame;
		callFrame->exceptHeight = thread->exceptHeight;

		/* Restore the state information and jump to the new method */
		RESTORE_STATE_FROM_THREAD();
		pc = (unsigned char *)tempptr;
		method = methodToCall;
		CVM_OPTIMIZE_BLOCK();
	}
#ifdef IL_PROFILE_CVM_METHODS
	++(method->count);
#endif
}
break;

/**
 * <opcode name="call_ctor" group="Call management instructions">
 *   <operation>Call a constructor</operation>
 *
 *   <format>call_ctor<fsep/>mptr</format>
 *
 *   <form name="call_ctor" code="COP_CALL_CTOR"/>
 *
 *   <description>The <i>call_ctor</i> instruction effects a method
 *   call to the constructor identified by <i>mptr</i>.</description>
 *
 *   <notes>Constructors in the CVM system have two entry points: one
 *   which creates a block of memory and then initializes it; and the
 *   other which initializes a pre-allocated block.  The particular
 *   entry point is chosen based on the constructor's usage in the
 *   original CIL bytecode:
 *
 *   <ul>
 *     <li>If the CIL bytecode invoked the constructor method using
 *         <i>newobj</i>, then <i>call_ctor</i> should be used.</li>
 *     <li>If the CIL bytecode invoked a parent class's constructor
 *         method directly using <i>call</i>, then <i>call_extern</i>
 *         should be used.</li>
 *   </ul>
 *
 *   See the description of the <i>call_extern</i> instruction for
 *   a full account of frame handling, argument handling, etc.</notes>
 * </opcode>
 */
case COP_CALL_CTOR:
{
	/* Call a constructor that we don't know if it has been converted */
	methodToCall = (ILMethod *)(ReadPointer(pc + 1));

	/* Copy the state back into the thread object */
	COPY_STATE_TO_THREAD();

	/* Convert the method */
	tempptr = (void *)(_ILConvertMethod(thread, methodToCall));
	if(!tempptr)
	{
		MISSING_METHOD_EXCEPTION();
	}

	/* Allocate a new call frame */
	ALLOC_CALL_FRAME();

	/* Fill in the call frame details */
	callFrame->method = method;
	callFrame->pc = thread->pc + 1 + sizeof(void *);
	callFrame->frame = thread->frame;
	callFrame->exceptHeight = thread->exceptHeight;

	/* Restore the state information and jump to the new method */
	RESTORE_STATE_FROM_THREAD();
	pc = ((unsigned char *)tempptr) - ILCoderCtorOffset(thread->process->coder);
	method = methodToCall;
	CVM_OPTIMIZE_BLOCK();
#ifdef IL_PROFILE_CVM_METHODS
	++(method->count);
#endif
}
break;

/**
 * <opcode name="call_native" group="Call management instructions">
 *   <operation>Call a native function that has a return value</operation>
 *
 *   <format>call_native<fsep/>function<fsep/>cif</format>
 *
 *   <form name="call_native" code="COP_CALL_NATIVE"/>
 *
 *   <before>..., address</before>
 *   <after>...</after>
 *
 *   <description>The <i>call_native</i> instruction effects a native
 *   function call to <i>function</i>, using <i>cif</i> to define the
 *   format of the function arguments and return value.  The return
 *   value is stored at <i>address</i>.  The arguments are assumed
 *   to have already been stored into the "native argument buffer"
 *   using the <i>waddr_native*</i> instructions.</description>
 *
 *   <notes>Both <i>function</i> and <i>cif</i> are native pointers,
 *   which may be either 32 or 64 bits in size, depending upon the
 *   platform.<p/>
 *
 *   Native function calls occur in CIL "InternalCall" and "PInvoke"
 *   methods.  For each such method, the CVM translation process
 *   creates a CVM stub method that transfers the arguments on
 *   the CVM stack to the native argument buffer, makes the native
 *   call, and then puts the function's return value back onto
 *   the CVM stack prior to exiting.</notes>
 * </opcode>
 */
case COP_CALL_NATIVE:
{
	/* Call a native method */
	COPY_STATE_TO_THREAD();
	ffi_call((ffi_cif *)(ReadPointer(pc + 1 + sizeof(void *))),
	         (void (*)())(ReadPointer(pc + 1)), stacktop[-1].ptrValue,
			 nativeArgs);
	RESTORE_STATE_FROM_THREAD();
	pc = thread->pc;
	MODIFY_PC_AND_STACK(1 + sizeof(void *) * 2, -1);
}
break;

/**
 * <opcode name="call_native_void" group="Call management instructions">
 *   <operation>Call a native function with no return value</operation>
 *
 *   <format>call_native_void<fsep/>function<fsep/>cif</format>
 *
 *   <form name="call_native_void" code="COP_CALL_NATIVE_VOID"/>
 *
 *   <description>The <i>call_native_void</i> instruction is identical
 *   to <i>call_native</i>, except that the native function is assumed
 *   not to have a return value.</description>
 * </opcode>
 */
case COP_CALL_NATIVE_VOID:
{
	/* Call a native method that has no return value */
	COPY_STATE_TO_THREAD();
	ffi_call((ffi_cif *)(ReadPointer(pc + 1 + sizeof(void *))),
	         (void (*)())(ReadPointer(pc + 1)), 0, nativeArgs);
	RESTORE_STATE_FROM_THREAD();
	pc = thread->pc;
	MODIFY_PC_AND_STACK(1 + sizeof(void *) * 2, 0);
}
break;

/**
 * <opcode name="call_virtual" group="Call management instructions">
 *   <operation>Call a virtual method</operation>
 *
 *   <format>call_virtual<fsep/>N[1]<fsep/>M[1]</format>
 *   <format>wide<fsep/>call_virtual<fsep/>N[4]<fsep/>M[4]</format>
 *
 *   <form name="call_virtual" code="COP_CALL_VIRTUAL"/>
 *
 *   <description>The <i>call_virtual</i> instruction effects a
 *   virtual method call.  The value <i>N</i> indicates the
 *   position of the <code>this</code> pointer on the stack:
 *   1 indicates the top of stack, 2 indicates the stack word
 *   just below the top-most stack word, etc.  The value <i>M</i>
 *   is the offset into the object's vtable for the method.</description>
 *
 *   <notes>See the description of the <i>call_extern</i> instruction for
 *   a full account of frame handling, argument handling, etc.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     the <code>this</code> pointer is <code>null</code>.</exception>
 *     <exception name="System.MissingMethodException">Raised if
 *     the method could not be translated into CVM bytecode.</exception>
 *   </exceptions>
 * </opcode>
 */
case COP_CALL_VIRTUAL:
{
	/* Call a virtual method */
	tempptr = stacktop[-((ILInt32)(pc[1]))].ptrValue;
	if(tempptr)
	{
		/* Locate the method to be called */
		methodToCall = ((ILClassPrivate *)(GetObjectClass(tempptr)->userData))
							->vtable[(ILUInt32)(pc[2])];

		/* Copy the state back into the thread object */
		COPY_STATE_TO_THREAD();

		/* Convert the method */
		tempptr = (void *)(_ILConvertMethod(thread, methodToCall));
		if(!tempptr)
		{
			MISSING_METHOD_EXCEPTION();
		}

		/* Allocate a new call frame */
		ALLOC_CALL_FRAME();

		/* Fill in the call frame details */
		callFrame->method = method;
		callFrame->pc = thread->pc + 3;
		callFrame->frame = thread->frame;
		callFrame->exceptHeight = thread->exceptHeight;

		/* Restore the state information and jump to the new method */
		RESTORE_STATE_FROM_THREAD();
		pc = (unsigned char *)tempptr;
		method = methodToCall;
		CVM_OPTIMIZE_BLOCK();
	#ifdef IL_PROFILE_CVM_METHODS
		++(method->count);
	#endif
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

/**
 * <opcode name="call_interface" group="Call management instructions">
 *   <operation>Call an interface method</operation>
 *
 *   <format>call_interface<fsep/>N[1]<fsep/>M[1]<fsep/>cptr</format>
 *   <format>wide<fsep/>call_interface<fsep/>N[4]<fsep/>M[4]<fsep/>cptr</format>
 *
 *   <form name="call_interface" code="COP_CALL_INTERFACE"/>
 *
 *   <description>The <i>call_interface</i> instruction effects an
 *   interface method call.  The value <i>N</i> indicates the
 *   position of the <code>this</code> pointer on the stack:
 *   1 indicates the top of stack, 2 indicates the stack word
 *   just below the top-most stack word, etc.  The value <i>M</i>
 *   is the offset into the interface's vtable for the method.  The value
 *   <i>cptr</i> indicates the interface class pointer.</description>
 *
 *   <notes>See the description of the <i>call_extern</i> instruction for
 *   a full account of frame handling, argument handling, etc.<p/>
 *
 *   The <i>cptr</i> value is a native pointer that may be either 32 or
 *   64 bits in size, depending upon the platform.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     the <code>this</code> pointer is <code>null</code>.</exception>
 *     <exception name="System.MissingMethodException">Raised if
 *     the method could not be translated into CVM bytecode.</exception>
 *   </exceptions>
 * </opcode>
 */
case COP_CALL_INTERFACE:
{
	/* Call an interface method */
	tempptr = stacktop[-((ILInt32)(pc[1]))].ptrValue;
	if(tempptr)
	{
		/* Locate the method to be called */
		methodToCall = _ILLookupInterfaceMethod(GetObjectClass(tempptr),
												(ILClass *)ReadPointer(pc + 3),
												(ILUInt32)(pc[2]));
		if(!methodToCall)
		{
			MISSING_METHOD_EXCEPTION();
		}

		/* Copy the state back into the thread object */
		COPY_STATE_TO_THREAD();

		/* Convert the method */
		tempptr = (void *)(_ILConvertMethod(thread, methodToCall));
		if(!tempptr)
		{
			MISSING_METHOD_EXCEPTION();
		}

		/* Allocate a new call frame */
		ALLOC_CALL_FRAME();

		/* Fill in the call frame details */
		callFrame->method = method;
		callFrame->pc = thread->pc + 3 + sizeof(void *);
		callFrame->frame = thread->frame;
		callFrame->exceptHeight = thread->exceptHeight;

		/* Restore the state information and jump to the new method */
		RESTORE_STATE_FROM_THREAD();
		pc = (unsigned char *)tempptr;
		method = methodToCall;
		CVM_OPTIMIZE_BLOCK();
	#ifdef IL_PROFILE_CVM_METHODS
		++(method->count);
	#endif
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

/**
 * <opcode name="cctor_once" group="Call management instructions">
 *   <operation>Block the current method from being executed
 *              more than once</operation>
 *
 *   <format>cctor_once</format>
 *
 *   <form name="cctor_once" code="COP_CCTOR_ONCE"/>
 *
 *   <description>The <i>cctor_once</i> instruction is typically
 *   the first instruction in a static constructor.  If this is the
 *   first time that the static constructor has been called, then
 *   the method will continue.  Otherwise, the effect will be the
 *   same as for <i>return</i>.</description>
 *
 *   <notes>This instruction permits CVM bytecode to call static
 *   constructors from anywhere in the system, without having to
 *   worry about whether it has already been called.  This instruction
 *   will also prevent recursive static constructor calls from
 *   looping indefinitely.</notes>
 * </opcode>
 */
case COP_CCTOR_ONCE:
{
	/* Execute the rest of this static constructor method once only */
	if((method->member.owner->attributes & IL_META_TYPEDEF_CCTOR_ONCE) == 0)
	{
		/* We haven't executed this method yet, so mark and continue */
		method->member.owner->attributes |= IL_META_TYPEDEF_CCTOR_ONCE;
		MODIFY_PC_AND_STACK(1, 0);
		break;
	}
}
/* Fall through to the next case */

/**
 * <opcode name="return" group="Call management instructions">
 *   <operation>Return from the current method with no return value</operation>
 *
 *   <format>return</format>
 *
 *   <form name="return" code="COP_RETURN"/>
 *
 *   <description>Return control to the method that called the current
 *   method, as follows:
 *
 *   <ul>
 *     <li>Set the top of stack pointer to the frame pointer.</li>
 *     <li>Pop the top-most call frame from the call frame stack.</li>
 *     <li>Retrieve the method pointer, progrm counter, exception frame
 *         height, and the frame pointer from the call frame.</li>
 *   </ul>
 *   </description>
 *
 *   <notes>The <i>set_num_args</i> instruction has previously set the
 *   frame pointer to the address of the first argument.  When <i>return</i>
 *   is executed, the first step above will pop all of the arguments.</notes>
 * </opcode>
 */
case COP_RETURN:
{
	/* Return from a method with no return value */
	stacktop = frame;
popFrame:
	callFrame = &(thread->frameStack[--(thread->numFrames)]);
	methodToCall = callFrame->method;
	pc = callFrame->pc;
	thread->exceptHeight = callFrame->exceptHeight;
	frame = callFrame->frame;
	method = methodToCall;

	/* Should we return to an external method? */
	if(pc == IL_INVALID_PC)
	{
		COPY_STATE_TO_THREAD();
		return 0;
	}

#ifdef IL_DUMP_CVM
	/* Dump the name of the method we are returning to */
	fprintf(IL_DUMP_CVM_STREAM, "Returning to %s::%s (%ld)\n",
			method->member.owner->name,
		    method->member.name, (long)(stacktop - thread->stackBase));
	DUMP_STACK();
#endif
}
break;

/**
 * <opcode name="return_1" group="Call management instructions">
 *   <operation>Return from the current method with a single stack
 *              word as a return value</operation>
 *
 *   <format>return_1</format>
 *
 *   <form name="return_1" code="COP_RETURN_1"/>
 *
 *   <description>Return control to the method that called the current
 *   method, as follows:
 *
 *   <ul>
 *     <li>Copy the top-most word on the stack to the position
 *         indicated by the frame pointer, and then set the top
 *         of stack pointer to point just after the copy.</li>
 *     <li>Pop the top-most call frame from the call frame stack.</li>
 *     <li>Retrieve the method pointer, progrm counter, exception frame
 *         height, and the frame pointer from the call frame.</li>
 *   </ul>
 *   </description>
 *
 *   <notes>The <i>set_num_args</i> instruction has previously set the
 *   frame pointer to the address of the first argument.  When <i>return_1</i>
 *   is executed, the first step above will pop all of the arguments,
 *   with the single-word return value left in their place.</notes>
 * </opcode>
 */
case COP_RETURN_1:
{
	/* Return from a method with a single-word return value */
	frame[0] = stacktop[-1];
	stacktop = frame + 1;
	goto popFrame;
}
/* Not reached */

/**
 * <opcode name="return_2" group="Call management instructions">
 *   <operation>Return from the current method with two stack
 *              words as the return value</operation>
 *
 *   <format>return_2</format>
 *
 *   <form name="return_2" code="COP_RETURN_2"/>
 *
 *   <description>Return control to the method that called the current
 *   method, as follows:
 *
 *   <ul>
 *     <li>Copy the two top-most words on the stack to the position
 *         indicated by the frame pointer, and then set the top
 *         of stack pointer to point just after the two copied words.</li>
 *     <li>Pop the top-most call frame from the call frame stack.</li>
 *     <li>Retrieve the method pointer, progrm counter, exception frame
 *         height, and the frame pointer from the call frame.</li>
 *   </ul>
 *   </description>
 *
 *   <notes>The <i>set_num_args</i> instruction has previously set the
 *   frame pointer to the address of the first argument.  When <i>return_2</i>
 *   is executed, the first step above will pop all of the arguments,
 *   with the double-word return value left in their place.</notes>
 * </opcode>
 */
case COP_RETURN_2:
{
	/* Return from a method with a double-word return value */
	frame[0] = stacktop[-2];
	frame[1] = stacktop[-1];
	stacktop = frame + 2;
	goto popFrame;
}
/* Not reached */

/**
 * <opcode name="return_n" group="Call management instructions">
 *   <operation>Return from the current method with <i>n</i> stack
 *              words as the return value</operation>
 *
 *   <format>return_n</format>
 *
 *   <form name="return_2" code="COP_RETURN_N"/>
 *
 *   <description>Return control to the method that called the current
 *   method, as follows:
 *
 *   <ul>
 *     <li>Copy the <i>n</i> top-most words on the stack to the position
 *         indicated by the frame pointer, and then set the top of
 *         stack pointer to point just after the <i>n</i> copied words.</li>
 *     <li>Pop the top-most call frame from the call frame stack.</li>
 *     <li>Retrieve the method pointer, progrm counter, exception frame
 *         height, and the frame pointer from the call frame.</li>
 *   </ul>
 *   </description>
 *
 *   <notes>The <i>set_num_args</i> instruction has previously set the
 *   frame pointer to the address of the first argument.  When <i>return_n</i>
 *   is executed, the first step above will pop all of the arguments,
 *   with the <i>n</i>-word return value left in their place.</notes>
 * </opcode>
 */
case COP_RETURN_N:
{
	/* Return from a method with an N-word return value */
	tempNum = IL_READ_UINT32(pc + 1);
	IL_MEMMOVE(frame, stacktop - tempNum, sizeof(CVMWord) * tempNum);
	stacktop = frame + tempNum;
	goto popFrame;
}
/* Not reached */

/**
 * <opcode name="push_thread" group="Call management instructions">
 *   <operation>Push the thread identifier onto the native
 *              argument stack</operation>
 *
 *   <format>push_thread</format>
 *
 *   <form name="push_thread" code="COP_PUSH_THREAD"/>
 *
 *   <description>Pushes an identifier for the current thread onto
 *   the native argument stack.  This is only used for "InternalCall"
 *   methods.  "PInvoke" methods should use <i>waddr_native_m1</i> instead.
 *   </description>
 * </opcode>
 */
case COP_PUSH_THREAD:
{
	/* Push a pointer to the thread value onto the native argument stack */
	nativeArgs[0] = (void *)&thread;
	MODIFY_PC_AND_STACK(1, 0);
}
break;

/**
 * <opcode name="pushdown" group="Call management instructions">
 *   <operation>Push the <code>ptr</code> value at the top of
 *              stack down and duplicate it twice</operation>
 *
 *   <format>pushdown<fsep/>N[4]</format>
 *
 *   <form name="pushdown" code="COP_PUSHDOWN"/>
 *
 *   <before>..., val1, ..., valN, value</before>
 *   <after>..., value, value, val1, ..., valN</after>
 *
 *   <description>The <i>value</i> at the top of the stack is popped,
 *   pushed down <i>N</i> stack words, and duplicated twice.</description>
 *
 *   <notes>This instruction is used in combination with <i>new</i>
 *   to construct a block of memory for a new object.  The block
 *   is allocated, and then pushed down.  The lowest duplicated
 *   <i>value</i> becomes the return value for the constructor method.
 *   The other duplicated <i>value</i> becomes the <code>this</code>
 *   argument for the constructor method.</notes>
 * </opcode>
 */
case COP_PUSHDOWN:
{
	/* Push a value on the stack top down and duplicate it twice */
	if(((ILUInt32)(stackmax - stacktop)) >= 1)
	{
		tempptr = stacktop[-1].ptrValue;
		tempNum = IL_READ_UINT32(pc + 1);
		if(tempNum != 0)
		{
			IL_MEMMOVE(stacktop + 1 - tempNum, stacktop - 1 - tempNum,
					   sizeof(CVMWord) * tempNum);
		}
		(stacktop - tempNum - 1)->ptrValue = tempptr;
		(stacktop - tempNum)->ptrValue = tempptr;
		MODIFY_PC_AND_STACK(5, 1);
	}
	else
	{
		STACK_OVERFLOW_EXCEPTION();
	}
}
break;

#define COP_WADDR_NATIVE(name,value)	\
case COP_WADDR_NATIVE_##name: \
{ \
	/* Set a value within the native argument stack */ \
	nativeArgs[(value) + 1] = (void *)(&(frame[pc[1]])); \
	MODIFY_PC_AND_STACK(2, 0); \
} \
break

/**
 * <opcode name="waddr_native_&lt;n&gt;" group="Call management instructions">
 *   <operation>Set position <i>n</i> of the native argument buffer
 *              to the address of a local variable</operation>
 *
 *   <format>waddr_native_&lt;n&gt;<fsep/>V[1]</format>
 *   <format>wide<fsep/>waddr_native_&lt;n&gt;<fsep/>V[4]</format>
 *
 *   <form name="waddr_native_m1" code="COP_WADDR_NATIVE_M1"/>
 *   <form name="waddr_native_0" code="COP_WADDR_NATIVE_0"/>
 *   <form name="waddr_native_1" code="COP_WADDR_NATIVE_1"/>
 *   <form name="waddr_native_2" code="COP_WADDR_NATIVE_2"/>
 *   <form name="waddr_native_3" code="COP_WADDR_NATIVE_3"/>
 *   <form name="waddr_native_4" code="COP_WADDR_NATIVE_4"/>
 *   <form name="waddr_native_5" code="COP_WADDR_NATIVE_5"/>
 *   <form name="waddr_native_6" code="COP_WADDR_NATIVE_6"/>
 *   <form name="waddr_native_7" code="COP_WADDR_NATIVE_7"/>
 *
 *   <description>Set position <i>n</i> of the native argument buffer
 *   to the address of local variable <i>V</i>.  For an "InternalCall"
 *   method, 0 is the first argument.  For a "PInvoke" method,
 *   -1 (<i>m1</i>) is the first argument.</description>
 * </opcode>
 */
COP_WADDR_NATIVE(M1, -1);
COP_WADDR_NATIVE(0, 0);
COP_WADDR_NATIVE(1, 1);
COP_WADDR_NATIVE(2, 2);
COP_WADDR_NATIVE(3, 3);
COP_WADDR_NATIVE(4, 4);
COP_WADDR_NATIVE(5, 5);
COP_WADDR_NATIVE(6, 6);
COP_WADDR_NATIVE(7, 7);

#elif defined(IL_CVM_WIDE)

case COP_CALL_VIRTUAL:
{
	/* Wide version of "call_virtual" */
	tempptr = stacktop[-(IL_READ_INT32(pc + 2))].ptrValue;
	if(tempptr)
	{
		/* Locate the method to be called */
		methodToCall = ((ILClassPrivate *)(GetObjectClass(tempptr)->userData))
							->vtable[IL_READ_UINT32(pc + 6)];

		/* Copy the state back into the thread object */
		COPY_STATE_TO_THREAD();

		/* Convert the method */
		tempptr = (void *)(_ILConvertMethod(thread, methodToCall));
		if(!tempptr)
		{
			MISSING_METHOD_EXCEPTION();
		}

		/* Allocate a new call frame */
		ALLOC_CALL_FRAME();

		/* Fill in the call frame details */
		callFrame->method = method;
		callFrame->pc = thread->pc + 10;
		callFrame->frame = thread->frame;
		callFrame->exceptHeight = thread->exceptHeight;

		/* Restore the state information and jump to the new method */
		RESTORE_STATE_FROM_THREAD();
		pc = (unsigned char *)tempptr;
		method = methodToCall;
		CVM_OPTIMIZE_BLOCK();
	#ifdef IL_PROFILE_CVM_METHODS
		++(method->count);
	#endif
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_CALL_INTERFACE:
{
	/* Wide version of "call_interface" */
	tempptr = stacktop[-(IL_READ_INT32(pc + 2))].ptrValue;
	if(tempptr)
	{
		/* Locate the method to be called */
		methodToCall = _ILLookupInterfaceMethod(GetObjectClass(tempptr),
												(ILClass *)ReadPointer(pc + 10),
												IL_READ_UINT32(pc + 6));
		if(!methodToCall)
		{
			MISSING_METHOD_EXCEPTION();
		}

		/* Copy the state back into the thread object */
		COPY_STATE_TO_THREAD();

		/* Convert the method */
		tempptr = (void *)(_ILConvertMethod(thread, methodToCall));
		if(!tempptr)
		{
			MISSING_METHOD_EXCEPTION();
		}

		/* Allocate a new call frame */
		ALLOC_CALL_FRAME();

		/* Fill in the call frame details */
		callFrame->method = method;
		callFrame->pc = thread->pc + 10 + sizeof(void *);
		callFrame->frame = thread->frame;
		callFrame->exceptHeight = thread->exceptHeight;

		/* Restore the state information and jump to the new method */
		RESTORE_STATE_FROM_THREAD();
		pc = (unsigned char *)tempptr;
		method = methodToCall;
		CVM_OPTIMIZE_BLOCK();
	#ifdef IL_PROFILE_CVM_METHODS
		++(method->count);
	#endif
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_CALLI:
{
	/* Call a method by pointer */
	/* TODO */
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_JMPI:
{
	/* Jump to a method by pointer */
	/* TODO */
	MODIFY_PC_AND_STACK(1, 0);
}
break;

#define COP_WADDR_NATIVE_WIDE(name,value)	\
case COP_WADDR_NATIVE_##name: \
{ \
	/* Wide version of "waddr_native_*" */ \
	nativeArgs[(value) + 1] = (void *)(&(frame[IL_READ_UINT32(pc + 2)])); \
	MODIFY_PC_AND_STACK(6, 0); \
} \
break

COP_WADDR_NATIVE_WIDE(M1, -1);
COP_WADDR_NATIVE_WIDE(0, 0);
COP_WADDR_NATIVE_WIDE(1, 1);
COP_WADDR_NATIVE_WIDE(2, 2);
COP_WADDR_NATIVE_WIDE(3, 3);
COP_WADDR_NATIVE_WIDE(4, 4);
COP_WADDR_NATIVE_WIDE(5, 5);
COP_WADDR_NATIVE_WIDE(6, 6);
COP_WADDR_NATIVE_WIDE(7, 7);

#elif defined(IL_CVM_PREFIX)

/**
 * <opcode name="tail" group="Call management instructions">
 *   <operation>Modify the next call instruction to make
 *				it a tail call</operation>
 *
 *   <format>prefix<fsep/>tail</format>
 *
 *   <form name="tail" code="COP_PREFIX_TAIL"/>
 *
 *   <description>Modify the next <i>call</i>, <i>call_extern</i>,
 *   <i>call_virtual</i> or <i>call_interface</i> instruction to
 *   use tail call semantics.</description>
 * </opcode>
 */
case COP_PREFIX_TAIL:
{
	/*
	 * It will be necessary to adjust the stack based upon the
	 * number of parameters in the called function.
	 */
	switch (pc[2]) 
	{
	case COP_CALL:
	case COP_CALL_EXTERN:

		/* Retreive the target method */
		methodToCall = (ILMethod *)(ReadPointer(pc + 3));
		methodSignature = ILMethod_Signature(methodToCall);
		numParams = ILTypeNumParams(methodSignature);

		if (pc[2] == COP_CALL) 
		{
			tempptr = (void *)(methodToCall->userData);
		}
		else
		{
			COPY_STATE_TO_THREAD();
			tempptr = (void *)(_ILConvertMethod(thread, methodToCall));
			RESTORE_STATE_FROM_THREAD();

			if (!tempptr)
			{
				MISSING_METHOD_EXCEPTION();
			}
		}

		/* Purge the stack (except for call arguments) if necessary */
		if (numParams < stacktop - frame) 
		{
			int i;
			for (i=0; i<numParams; i++)
			{
				frame[i] = stacktop[i-numParams];
			}
			stacktop = frame + numParams;
		}

		/*  Transfer control  */
		REPORT_METHOD_CALL();
		pc = (unsigned char *)tempptr;
		thread->exceptHeight = 0;
		method = methodToCall;
#ifdef IL_PROFILE_CVM_METHODS
		++(method->count);
#endif

	default:
		MODIFY_PC_AND_STACK(2, 0);
		/* TODO - Implement other call styles */
		break;
	}
}
break;

/**
 * <opcode name="ldftn" group="Call management instructions">
 *   <operation>Load the address of a function method onto the stack</operation>
 *
 *   <format>prefix<fsep/>ldftn<fsep/>method</format>
 *
 *   <form name="ldftn" code="COP_PREFIX_LDFTN"/>
 *
 *   <before>...</before>
 *   <after>..., method</after>
 *
 *   <description>Push <i>method</i> onto the stack as a <code>ptr</code>
 *   value.</description>
 *
 *   <notes>The <i>method</i> value may be either 32 or 64 bits in size,
 *   depending upon the platform.</notes>
 * </opcode>
 */
case COP_PREFIX_LDFTN:
{
	/* Load the address of a function onto the stack */
	stacktop[0].ptrValue = ReadPointer(pc + 2);
	MODIFY_PC_AND_STACK(2 + sizeof(void *), 1);
}
break;

/**
 * <opcode name="ldvirtftn" group="Call management instructions">
 *   <operation>Load the address of a virtual function method
 *				onto the stack</operation>
 *
 *   <format>prefix<fsep/>ldvirtftn<fsep/>index[4]</format>
 *
 *   <form name="ldvirtftn" code="COP_PREFIX_LDVIRTFTN"/>
 *
 *   <before>..., object</before>
 *   <after>..., address</after>
 *
 *   <description>Pop <i>object</i> from the stack as type <code>ptr</code>
 *   and locate the virtual method at <i>index</i> within the object's
 *   vtable.  The address of this method is pushed onto the stack
 *   as type <code>ptr</code>.</description>
 * </opcode>
 */
case COP_PREFIX_LDVIRTFTN:
{
	/* Load the address of a virtual function onto the stack */
	tempptr = stacktop[-1].ptrValue;
	if(tempptr)
	{
		stacktop[-1].ptrValue =
			((ILClassPrivate *)(GetObjectClass(tempptr)->userData))
					->vtable[IL_READ_UINT32(pc + 2)];
		MODIFY_PC_AND_STACK(6, 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

/**
 * <opcode name="ldinterfftn" group="Call management instructions">
 *   <operation>Load the address of an interface function method
 *				onto the stack</operation>
 *
 *   <format>prefix<fsep/>ldinterfftn<fsep/>index[4]<fsep/>class</format>
 *
 *   <form name="ldinterfftn" code="COP_PREFIX_LDINTERFFTN"/>
 *
 *   <before>..., object</before>
 *   <after>..., address</after>
 *
 *   <description>Pop <i>object</i> from the stack as type <code>ptr</code>
 *   and locate the virtual method at <i>index</i> within the object's
 *   interface vtable for the interface <i>class</i>.  The address of
 *   this method is pushed onto the stack as type
 *   <code>ptr</code>.</description>
 *
 *   <notes>The <i>class</i> value may be either 32 or 64 bits in size,
 *   depending upon the platform.</notes>
 * </opcode>
 */
case COP_PREFIX_LDINTERFFTN:
{
	/* Load the address of an interface function onto the stack */
	tempptr = stacktop[-1].ptrValue;
	if(tempptr)
	{
		stacktop[-1].ptrValue =
			_ILLookupInterfaceMethod(GetObjectClass(tempptr),
									 (ILClass *)ReadPointer(pc + 6),
									 IL_READ_UINT32(pc + 2));
		MODIFY_PC_AND_STACK(6 + sizeof(void *), 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

#endif /* IL_CVM_PREFIX */
