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

#ifndef _INTERLOCKED_H_
#define _INTERLOCKED_H_

#ifndef IL_INLINE
	#ifdef __GNUC__
		#define IL_INLINE       __inline__
	#elif defined(_MSC_VER)
		#define IL_INLINE	__forceinline
	#else
		#define IL_INLINE
	#endif
#endif

/* TODO: implement native interlocked functions for other processors */

#include "interlocked_x86.h"
#include "interlocked_any.h"

#endif /* _INTERLOCKED_H_ */
