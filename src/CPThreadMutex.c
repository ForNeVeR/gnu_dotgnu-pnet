/*
 * SDPThreadMutex.c - Posix thread mutex implementation.
 *
 * Copyright (C) 2005  Free Software Foundation, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "CPThreadMutex.h"

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL void
SDMutex_Lock(SDMutex *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* lock the mutex */
	pthread_mutex_lock(&(_this->mutex));
}

SDINTERNAL void
SDMutex_Unlock(SDMutex *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* unlock the mutex */
	pthread_mutex_unlock(&(_this->mutex));
}

SDINTERNAL SDStatus
SDMutex_Create(SDMutex **_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* allocate the mutex */
	if(!(*_this = (SDMutex *)SDMalloc(sizeof(SDMutex))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the mutex */
	pthread_mutex_init(&((*_this)->mutex), 0);

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL void
SDMutex_Destroy(SDMutex **_this)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((*_this != 0));

	/* finalize the mutex */
	pthread_mutex_destroy(&((*_this)->mutex));
}


#ifdef __cplusplus
};
#endif
