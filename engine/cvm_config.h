/*
 * cvm_config.h - Configure CVM in various ways.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#ifndef	_ENGINE_CVM_CONFIG_H
#define	_ENGINE_CVM_CONFIG_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Turn off assembly code optimisations if this is defined.
 */
/*#define IL_NO_ASM*/

/*
 * Enable or disable dumping of CVM instructions during execution.
 */
/*#define	IL_DUMP_CVM*/
#define	IL_DUMP_CVM_STREAM	stdout

/*
 * Enable or disable profiling.
 */
/*#define	IL_PROFILE_CVM_INSNS*/
/*#define	IL_PROFILE_CVM_METHODS*/
/*#define	IL_PROFILE_CVM_VAR_USAGE*/
#ifdef IL_PROFILE_CVM_INSNS
extern int _ILCVMInsnCount[];
#endif

/*
 * Determine what kind of instruction dumping to perform.
 */
#if defined(IL_DUMP_CVM)
	#define	CVM_DUMP()	\
		_ILDumpCVMInsn(IL_DUMP_CVM_STREAM, method, pc)
	#define CVM_WIDE_DUMP()
	#define CVM_PREFIX_DUMP()
#elif defined(IL_PROFILE_CVM_INSNS)
	#define	CVM_DUMP()	\
		(++(_ILCVMInsnCount[pc[0]]))
	#define CVM_WIDE_DUMP()	\
		(++(_ILCVMInsnCount[pc[1]]))
	#define CVM_PREFIX_DUMP()	\
		(++(_ILCVMInsnCount[((int)(pc[1])) + 256]))
#else
	#define	CVM_DUMP()
	#define CVM_WIDE_DUMP()
	#define CVM_PREFIX_DUMP()
	#define	IL_CVM_DIRECT_ALLOWED
#endif

/*
 * Determine what CPU we are compiling for, and any
 * additional optimizations we can use for that CPU.
 */
#if defined(__i386) || defined(__i386__)
	#define	CVM_X86
	#define CVM_LITTLE_ENDIAN
	#define	CVM_LONGS_ALIGNED_WORD
	#define	CVM_REALS_ALIGNED_WORD
	#define	CVM_DOUBLES_ALIGNED_WORD
	#define CVM_WORDS_AND_PTRS_SAME_SIZE
#endif

/*
 * Determine the style of interpreter to use, which is one
 * of "IL_CVM_SWITCH", "IL_CVM_TOKEN", or "IL_CVM_DIRECT".
 * These correspond to "simple switch loop", "token threaded
 * based on bytecode", and "direct threaded based on address".
 */
#ifdef HAVE_COMPUTED_GOTO
	#define	IL_CVM_TOKEN
#else /* !HAVE_COMPUTED_GOTO */
	#define	IL_CVM_SWITCH
#endif /* !HAVE_COMPUTED_GOTO */

#ifdef	__cplusplus
};
#endif

#endif	/* _ENGINE_CVM_CONFIG_H */
