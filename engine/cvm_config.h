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
	#ifdef IL_CONFIG_DIRECT
		#define	IL_CVM_DIRECT_ALLOWED
	#endif
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
#if defined(__arm) || defined(__arm__)
	#define	CVM_ARM
	#define	CVM_LONGS_ALIGNED_WORD
	#define CVM_WORDS_AND_PTRS_SAME_SIZE
#endif
#if defined(__powerpc__) || defined(powerpc) || \
		defined(__powerpc) || defined(PPC)
	#define	CVM_PPC
#endif
#if defined(__x86_64__) || defined(__x86_64) 
	#define CVM_X86_64
	#define CVM_LITTLE_ENDIAN
	#define	CVM_LONGS_ALIGNED_WORD
#endif
#if defined(__ia64) || defined(__ia64__)
	#define	CVM_IA64
#endif

/*
 * Determine the style of interpreter to use, which is one
 * of "IL_CVM_SWITCH", "IL_CVM_TOKEN", or "IL_CVM_DIRECT".
 * These correspond to "simple switch loop", "token threaded
 * based on bytecode", and "direct threaded based on address".
 */
#ifdef HAVE_COMPUTED_GOTO
	#ifdef IL_CVM_DIRECT_ALLOWED
		#define	IL_CVM_DIRECT
		#define	IL_CVM_FLAVOUR "Direct Threaded"
		#if defined(PIC) && defined(HAVE_PIC_COMPUTED_GOTO)
			#define	IL_CVM_PIC_DIRECT
		#endif
	#else
		#define	IL_CVM_TOKEN
		#define	IL_CVM_FLAVOUR "Token Threaded"
		#if defined(PIC) && defined(HAVE_PIC_COMPUTED_GOTO)
			#define	IL_CVM_PIC_TOKEN
		#endif
	#endif
#else /* !HAVE_COMPUTED_GOTO */
	#define	IL_CVM_SWITCH
	#define	IL_CVM_FLAVOUR "Switch Loop"
#endif /* !HAVE_COMPUTED_GOTO */

/*
 * Declare the code necessary to export the direct threading
 * tables from "_ILCVMInterpreter", and to extract addresses
 * for specific opcodes in the CVM coder.
 */
#ifdef IL_CVM_DIRECT
	#ifdef IL_CVM_PIC_DIRECT

		/* We are building a direct interpreter with PIC labels */
		extern const int *_ILCVMMainLabelTable;
		extern const int *_ILCVMPrefixLabelTable;
		extern void *_ILCVMBaseLabel;

		#define CVM_DEFINE_TABLES()	\
					const int *_ILCVMMainLabelTable; \
					const int *_ILCVMPrefixLabelTable; \
					void *_ILCVMBaseLabel

		#define	CVM_EXPORT_TABLES()	\
					do { \
						_ILCVMMainLabelTable = main_label_table; \
						_ILCVMPrefixLabelTable = prefix_label_table; \
						_ILCVMBaseLabel = &&COP_NOP_label; \
					} while (0)

		#define	CVM_LABEL_FOR_OPCODE(opcode)	\
					(_ILCVMBaseLabel + _ILCVMMainLabelTable[(opcode)])
		#define	CVMP_LABEL_FOR_OPCODE(opcode)	\
					(_ILCVMBaseLabel + _ILCVMPrefixLabelTable[(opcode)])

	#else /* !IL_CVM_PIC_DIRECT */

		/* We are building a direct interpreter from non-PIC labels */
		extern void **_ILCVMMainLabelTable;
		extern void **_ILCVMPrefixLabelTable;

		#define CVM_DEFINE_TABLES()	\
					void **_ILCVMMainLabelTable; \
					void **_ILCVMPrefixLabelTable

		#define	CVM_EXPORT_TABLES()	\
					do { \
						_ILCVMMainLabelTable = main_label_table; \
						_ILCVMPrefixLabelTable = prefix_label_table; \
					} while (0)

		#define	CVM_LABEL_FOR_OPCODE(opcode)	\
					(_ILCVMMainLabelTable[(opcode)])
		#define	CVMP_LABEL_FOR_OPCODE(opcode)	\
					(_ILCVMPrefixLabelTable[(opcode)])

	#endif /* !IL_CVM_PIC_DIRECT */
#else /* !IL_CVM_DIRECT */

	/* We are building a non-direct interpreter */
	#define	CVM_DEFINE_TABLES()
	#define	CVM_EXPORT_TABLES()

#endif /* !IL_CVM_DIRECT */

/*
 * Determine if we can unroll the direct threaded interpreter.
 */
#if defined(IL_CVM_DIRECT) && defined(CVM_X86) && \
	defined(__GNUC__) && !defined(IL_NO_ASM) && \
	!defined(IL_CVM_PROFILE_CVM_VAR_USAGE) && \
	defined(IL_CONFIG_UNROLL)
#define	IL_CVM_DIRECT_UNROLLED
#undef	IL_CVM_FLAVOUR
#define	IL_CVM_FLAVOUR "Direct Unrolled (x86)"
#endif
#if defined(IL_CVM_DIRECT) && defined(CVM_ARM) && \
	defined(__GNUC__) && !defined(IL_NO_ASM) && \
	!defined(IL_CVM_PROFILE_CVM_VAR_USAGE) && \
	defined(IL_CONFIG_UNROLL)
#define	IL_CVM_DIRECT_UNROLLED
#undef	IL_CVM_FLAVOUR
#define	IL_CVM_FLAVOUR "Direct Unrolled (ARM)"
#endif
#if defined(IL_CVM_DIRECT) && defined(CVM_PPC) && \
	defined(__GNUC__) && !defined(IL_NO_ASM) && \
	!defined(IL_CVM_PROFILE_CVM_VAR_USAGE) && \
	defined(IL_CONFIG_UNROLL)
#define	IL_CVM_DIRECT_UNROLLED
#undef	IL_CVM_FLAVOUR
#define	IL_CVM_FLAVOUR "Direct Unrolled (PPC)"
#endif
#if 0	/* remove this once ia64 unroller is finished */
#if defined(IL_CVM_DIRECT) && defined(CVM_IA64) && \
	defined(__GNUC__) && !defined(IL_NO_ASM) && \
	!defined(IL_CVM_PROFILE_CVM_VAR_USAGE) && \
	defined(IL_CONFIG_UNROLL)
#define	IL_CVM_DIRECT_UNROLLED
#undef	IL_CVM_FLAVOUR
#define	IL_CVM_FLAVOUR "Direct Unrolled (ia64)"
#endif
#endif

/*
 * The constructor offset value.
 */
#ifdef IL_CVM_DIRECT
	#define	CVM_CTOR_OFFSET		(3 * sizeof(void *))
#else
	#define	CVM_CTOR_OFFSET		6
#endif

#ifdef	__cplusplus
};
#endif

#endif	/* _ENGINE_CVM_CONFIG_H */
