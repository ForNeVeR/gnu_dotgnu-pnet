/*
 * interlocked.h - Implementation of interlocked functions.
 *
 * Copyright (C) 2004  Southern Storm Software, Pty Ltd.
 *
 * Authors: Thong Nguyen (tum@veridicus.com)
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

#include "interrupt.h"

#ifdef IL_INTERRUPT_POSIX

#include "thr_defs.h"
#include "il_thread.h"

#ifdef	__cplusplus
extern	"C" {
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
	#define __USE_GNU
#endif

#if defined(IL_INTERRUPT_HAVE_X86_CONTEXT)
	#include <sys/ucontext.h>
#endif

#if defined(IL_INTERRUPT_SUPPORTS)

#if defined(HAVE_SIGACTION)

static void __sigaction_handler(int signo, siginfo_t *info, void *ctx)
{
	ILThread *thread;
	ILInterruptContext context;
#if defined(IL_INTERRUPT_HAVE_X86_CONTEXT)
	ucontext_t *uc;
	uc = (ucontext_t *)ctx;
#endif

	thread = ILThreadSelf();

#if defined(IL_INTERRUPT_HAVE_X86_CONTEXT)
	/* Integer registers */
	context.Eax = uc->uc_mcontext.gregs[REG_EAX];
	context.Ebx = uc->uc_mcontext.gregs[REG_EBX];
	context.Ecx = uc->uc_mcontext.gregs[REG_ECX];
	context.Edx = uc->uc_mcontext.gregs[REG_EDX];
	context.Edi = uc->uc_mcontext.gregs[REG_EDI];
	context.Esi = uc->uc_mcontext.gregs[REG_ESI];

	/* Control registers */
	context.Ebp = uc->uc_mcontext.gregs[REG_EBP];
	context.Eip = uc->uc_mcontext.gregs[REG_EIP];
	context.Esp = uc->uc_mcontext.gregs[REG_ESP];

	context.instructionAddress = (void *)context.Eip;

#else
	context.instructionAddress = 0;
#endif

	switch (signo)
	{	
		#if defined(IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS)
		case SIGSEGV:
		case SIGBUS:

			thread = ILThreadSelf();

			context.memoryAddress = info->si_addr;
			context.type = IL_INTERRUPT_TYPE_ILLEGAL_MEMORY_ACCESS;

			thread->interruptHandler(&context);
			
			break;
		#endif

		#if defined(IL_INTERRUPT_SUPPORTS_ANY_ARITH)
		case SIGFPE:

			switch (info->si_code)
			{
				#if defined(IL_INTERRUPT_SUPPORTS_INT_DIVIDE_BY_ZERO)
				case FPE_INTDIV:
					context.type = IL_INTERRUPT_TYPE_INT_DIVIDE_BY_ZERO;
					context.instructionAddress = info->si_addr;
					context.memoryAddress = 0;
					thread->interruptHandler(&context);
					break;
				#endif

				#if defined(IL_INTERRUPT_SUPPORTS_INT_OVERFLOW)
				case FPE_INTOVF:
					context.type = IL_INTERRUPT_TYPE_INT_OVERFLOW;
					context.instructionAddress = info->si_addr;
					context.memoryAddress = 0;
					thread->interruptHandler(&context);
					break;
				#endif
			}

			break;
		#endif
	}
	
}

#elif defined(HAVE_SIGNAL)

static void __signal_handler(int signal)
{
	ILThread *thread;
	ILInterruptContext context;

	thread = ILThreadSelf();

	switch (signal)
	{
		#if defined(IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS)
		case SIGSEGV:
		case SIGBUS:
			context.memoryAddress = 0
			context.instructionAddress = 0
			context.type = IL_INTERRUPT_TYPE_ILLEGAL_MEMORY_ACCESS;
			thread->interruptHandler(&context);
			break;
		#endif
	}
}

#endif

#endif /* IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS */

void _ILInterruptInit()
{
#if defined(IL_INTERRUPT_SUPPORTS)	
	#if defined(HAVE_SIGACTION)

		/* Use SIGACTION */
		struct sigaction sa;

		sa.sa_sigaction = __sigaction_handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_SIGINFO;
		
		/* Registers memory violation handlers */
		#ifdef IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS
			sigaction(SIGSEGV, &sa, 0);
			sigaction(SIGBUS, &sa, 0);
		#endif

		/* Registers FPE exception handlers */
		#if defined(IL_INTERRUPT_SUPPORTS_ANY_ARITH)
			sigemptyset(&sa.sa_mask);
			sa.sa_flags = SA_SIGINFO | SA_NODEFER;
			sigaction(SIGFPE, &sa, 0);
		#endif
	#elif defined(HAVE_SIGNAL)

		/* Use SIGNAL */
		#warning sigaction() not available, using signal() which may be inaccurate

		/* Register memory violation handlers */
		signal(SIGSEGV, __signal_handler);
		signal(SIGBUS, __signal_handler);
	#endif
#endif
}

void _ILInterruptDeinit()
{
}

#ifdef	__cplusplus
};
#endif

#endif /* IL_INTERRUPT_POSIX */

