/*
 * jitc_profile.c - Profiling functions for the JIT Coder.
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
 * Emit the code to increase the call count of a method.
 */
static void _ILJitProfileIncreaseMethodCallCount(ILJITCoder *jitCoder, ILMethod *method)
{
	ILJitValue callCounter = jit_value_create_nint_constant(jitCoder->jitFunction,
															_IL_JIT_TYPE_VPTR,
															(jit_nint)(&(method->count)));
#ifdef ENHANCED_PROFILER
	/* If the enhanced profiler is selected then don't count when
	 * profiling is disabled
	 * (e.g. via DotGNU.Misc.Profiling.StopProfiling())
	 */
	jit_label_t label = jit_label_undefined;
	ILJitValue thread = _ILJitCoderGetThread(jitCoder);
	ILJitValue profilingEnabled = jit_insn_load_relative(jitCoder->jitFunction,
														 thread,
														 offsetof(ILExecThread, profilingEnabled),
														 jit_type_sys_int);
	jit_insn_branch_if_not(jitCoder->jitFunction, profilingEnabled, &label);
#endif

	jit_insn_call_native(jitCoder->jitFunction,
						 "ILInterlockedIncrement",
						 ILInterlockedIncrement,
						 _ILJitSignature_ILInterlockedIncrement,
						 &callCounter, 1, JIT_CALL_NOTHROW);
#ifdef ENHANCED_PROFILER
	jit_insn_label(jitCoder->jitFunction, &label);
#endif
}


