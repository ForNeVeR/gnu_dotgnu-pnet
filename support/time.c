/*
 * time.c - Get the current system time.
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

#include "il_system.h"
#if TIME_WITH_SYS_TIME
	#include <sys/time.h>
    #include <time.h>
#else
    #if HAVE_SYS_TIME_H
		#include <sys/time.h>
    #else
        #include <time.h>
    #endif
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Magic number that converts a time which is relative to
 * Jan 1, 1970 into a value which is relative to Jan 1, 0001.
 */
#ifdef 	IL_NATIVE_WIN32
#define	EPOCH_ADJUST	((ILInt64)62135596800LL)
#else
#define	EPOCH_ADJUST	((ILInt64)62135596800L)
#endif

void ILGetCurrTime(ILCurrTime *timeValue)
{
#ifdef HAVE_GETTIMEOFDAY
	/* Try to get the current time, accurate to the microsecond */
	struct timeval tv;
	gettimeofday(&tv, 0);
	timeValue->secs = ((ILInt64)(tv.tv_sec)) + EPOCH_ADJUST;
	timeValue->nsecs = (ILUInt32)(tv.tv_usec * 1000);
#else
	/* Use the ANSI routine to get the time in seconds */
	timeValue->secs = (ILInt64)(time(0)) + EPOCH_ADJUST;
	timeValue->nsecs = 0;
#endif
}

int ILGetSinceRebootTime(ILCurrTime *timeValue)
{
	return 0;
}

ILInt32 ILGetTimeZoneAdjust(void)
{
	static int initialized = 0;
	static int isdst = 0;
	if(!initialized)
	{
		/* Call "localtime", which will initialise "timezone" for us */
		time_t temp = time(0);
		struct tm *tms = localtime(&temp);
		isdst = tms->tm_isdst;
		initialized = 1;
	}
	return (ILInt32)(timezone + (isdst ? 3600 : 0));
}

#ifdef	__cplusplus
};
#endif
