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

#if defined(IL_INTERRUPT_HAVE_X86_CONTEXT)
#include "ucontext.h"
#endif

#if defined(HAVE_SIGACTION)

static void __sigaction_handler(int signo, siginfo_t *info, void *context);
{
	ILThread *thread;
	ILInterruptContext context;
#if defined(IL_INTERRUPT_HAVE_X86_CONTEXT)
	ucontext_t *uc;
	uc = (ucontext_t *)context;
#endif

	thread = ILThreadSelf();

	context.address = 0;

#if defined(IL_INTERRUPT_HAVE_X86_CONTEXT)
	/* Integer registers */
	context.Edi = uc->uc_mcontext.gregs[REG_EDI];
	context.Esi = uc->uc_mcontext.gregs[REG_ESI];
	context.Ebx = uc->uc_mcontext.gregs[REG_EBX];
	context.Edx = uc->uc_mcontext.gregs[REG_EDX];
	context.Ecx = uc->uc_mcontext.gregs[REG_ECX];
	context.Eax = uc->uc_mcontext.gregs[REG_EAX];

	/* Control registers */
	context.Ebp = uc->uc_mcontext.gregs[REG_EBP];
	context.Eip = uc->uc_mcontext.gregs[REG_EIP];
	context.Esp = uc->uc_mcontext.gregs[REG_ESP];

	context.address = context.Ebx;
#endif

	thread->illegalMemoryAccessHandler(&context);
}

#elif defined(HAVE_SIGNAL)

static void __signal_handler(int signal)
{
	ILThread *thread;
	ILInterruptContext context;

	thread = ILThreadSelf();

	context.address = 0

	thread->illegalMemoryAccessHandler(&context);
}

#endif

void _ILInterruptInit()
{
#ifdef IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS	
	#if defined(HAVE_SIGACTION)
		struct sigaction sa;

		sa.sa_handler = __sigaction_handler;
		sa.sa_mask = 0;
		sa.sa_flags = SA_SIGINFO;
		
		sigaction(SIGSEGV, &sa, 0);
		sigaction(SIGBUS, &sa, 0);
	#elif defined(HAVE_SIGNAL)
		signal(SIG_SEGV, __signal_handler);
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
