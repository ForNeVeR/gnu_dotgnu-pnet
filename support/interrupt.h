/*
 * interlocked.h - Implementation of interlocked functions.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include "il_config.h"
#include "il_values.h"

#if (defined(HAVE_SETJMP) || defined(HAVE_SETJMP_H)) \
	&& defined(HAVE_LONGJMP)

	#include <setjmp.h>

	#define IL_JMP_BUFFER jmp_buf

	#define IL_SETJMP(buf) \
		setjmp(buf)

	#define IL_LONGJMP(buf, arg) \
		longjmp(buf, arg)

	#ifdef IL_WIN32_PLATFORM
		#define IL_INTERRUPT_SUPPORTS 1
		#define IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS 1
		#define IL_INTERRUPT_WIN32 1
	#endif

#endif

#endif /* _INTERRUPT_H */
