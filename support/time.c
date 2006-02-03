/*
 * time.c - Get the current system time.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2004  Free Software Foundation
 *
 * Contributions from Thong Nguyen (tum@veridicus.com)
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
#include "il_thread.h"
#if defined(__palmos__)
	#include <PalmTypes.h>
	#include <TimeMgr.h>
#elif TIME_WITH_SYS_TIME
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
#ifdef IL_WIN32_PLATFORM
#include <windows.h>
#define timezone _timezone
#endif

#if defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO) \
	&& (defined(linux) \
	|| defined(__linux) || defined(__linux__))
	
	#include <sys/sysinfo.h>
	#define USE_BOOTTIME 1
#endif

#if defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL) \
	&& defined(__FreeBSD__)
	
	#include <sys/sysctl.h>
	#define USE_BOOTTIME 1
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Magic number that converts a time which is relative to
 * Jan 1, 1970 into a value which is relative to Jan 1, 0001.
 */
#define	EPOCH_ADJUST	((ILInt64)62135596800LL)

/*
 * Magic number that converts a time which is relative to
 * Jan 1, 1601 into a value which is relative to Jan 1, 0001.
 */
#define	WIN32_EPOCH_ADJUST	((ILInt64)50491123200LL)

/*
 * Magic number that converts a time which is relative to
 * Jan 1, 1904 into a value which is relative to Jan 1, 0001.
 */
#define	PALM_EPOCH_ADJUST	((ILInt64)60052752000LL)

void ILGetCurrTime(ILCurrTime *timeValue)
{
#ifdef HAVE_GETTIMEOFDAY
	/* Try to get the current time, accurate to the microsecond */
	struct timeval tv;
	gettimeofday(&tv, 0);
	timeValue->secs = ((ILInt64)(tv.tv_sec)) + EPOCH_ADJUST;
	timeValue->nsecs = (ILUInt32)(tv.tv_usec * 1000);
#else
#ifdef IL_WIN32_PLATFORM
	/* Get the time using a Win32-specific API */
	FILETIME filetime;
	ILInt64 value;
	GetSystemTimeAsFileTime(&filetime);
	value = (((ILInt64)(filetime.dwHighDateTime)) << 32) +
			 ((ILInt64)(filetime.dwLowDateTime));
	timeValue->secs = (value / (ILInt64)10000000) + WIN32_EPOCH_ADJUST;
	timeValue->nsecs = (ILUInt32)((value % (ILInt64)10000000) * (ILInt64)100);
#elif defined(__palmos__)
	/* Use the PalmOS routine to get the time in seconds */
	timeValue->secs = ((ILInt64)(TimGetSeconds())) + PALM_EPOCH_ADJUST;
	timeValue->nsecs = 0;
#else
	/* Use the ANSI routine to get the time in seconds */
	timeValue->secs = (ILInt64)(time(0)) + EPOCH_ADJUST;
	timeValue->nsecs = 0;
#endif
#endif
}

#if defined(USE_BOOTTIME)
static ILCurrTime bootTime;
#endif

int ILGetSinceRebootTime(ILCurrTime *timeValue)
{
#ifdef IL_WIN32_PLATFORM
	DWORD tick;
	
	tick = GetTickCount();

	timeValue->secs = tick / 1000;
	timeValue->nsecs = (tick % 1000) * 1000000;

	return 1;
#elif defined(USE_BOOTTIME) && defined(__FreeBSD__) 
	int len, mib[2];	
	struct timeval tv;

	ILGetCurrTime(timeValue);

	if (bootTime.secs == 0 && bootTime.nsecs == 0)
	{
		mib[0] = CTL_KERN;
		mib[1] = KERN_BOOTTIME;

		len = sizeof(struct timeval);

		if (sysctl(mib, 2, &tv, &len, 0, 0) != 0)
		{
			return 0;
		}

		ILThreadAtomicStart();

		bootTime.secs = ((ILInt64)(tv.tv_sec)) + EPOCH_ADJUST;
		bootTime.nsecs = (ILUInt32)(tv.tv_usec * 1000);

		ILThreadAtomicEnd();
	}
	
#elif defined(USE_BOOTTIME) && (defined(linux) \
	|| defined(__linux) || defined(__linux__))

	struct sysinfo si;
	
	ILGetCurrTime(timeValue);

	if (bootTime.secs == 0 && bootTime.nsecs == 0)
	{
		if (sysinfo(&si) != 0)
		{
			return 0;
		}
		
		ILThreadAtomicStart();		
		bootTime.secs = timeValue->secs - si.uptime;

		/* sysinfo() is only accurate to the second so
		   use the nsec value from the curren time.
		   This allows subsequent calls to this function
		   to get nsec time-differential precision  */

		bootTime.nsecs = timeValue->nsecs;
		ILThreadAtomicEnd();
	}
#endif

#if defined(USE_BOOTTIME)
	/* Subtract the current time from the time since the system
	   was started */

	if(timeValue->nsecs < bootTime.nsecs)
	{
		timeValue->nsecs = 
			timeValue->nsecs - bootTime.nsecs + 1000000000;
		timeValue->secs =
			timeValue->secs - bootTime.secs - 1;
	}
	else
	{
		timeValue->nsecs =
			timeValue->nsecs - bootTime.nsecs;			
		timeValue->secs =
			timeValue->secs - bootTime.secs;
	}	

	return 1;
	
#else
	return 0;
#endif
}

ILInt32 ILGetTimeZoneAdjust(void)
{
#if !defined(__palmos__)
	static int initialized = 0;
	static int isdst = 0;
#ifdef HAVE_TM_GMTOFF
    static long timezone = 0;
#endif
	if(!initialized)
	{
#ifdef IL_WIN32_PLATFORM
		TIME_ZONE_INFORMATION temp;
		DWORD tmz = GetTimeZoneInformation(&temp);
		isdst = (tmz == TIME_ZONE_ID_DAYLIGHT) ? 1 : 0;
		/* we expect the adjustment to be in seconds, not minutes */
		if(isdst)
		{
			timezone = (temp.Bias + temp.DaylightBias) * 60;
		}
		else
		{
			timezone = temp.Bias * 60;
		}
#else
		/* Call "localtime", which will set the global "timezone" for us */
		time_t temp = time(0);
		struct tm *tms = localtime(&temp);
		isdst = tms->tm_isdst;
#ifdef HAVE_TM_GMTOFF
		timezone = -(tms->tm_gmtoff);
#endif
#endif
		//initialized = 1;
	}
	return (ILInt32)timezone;
#else
	/* TODO */
	return 0;
#endif
}

ILInt64 ILCLIToUnixTime(ILInt64 timeValue)
{
	return (timeValue / (ILInt64)10000000) - EPOCH_ADJUST;
}

ILInt64 ILUnixToCLITime(ILInt64 timeValue)
{
	return (timeValue + EPOCH_ADJUST) * (ILInt64)10000000;
}

#ifdef	__cplusplus
};
#endif
