/*
 * TimeMethods.cs - Implementation of the "Platform.TimeMethods" class.
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

namespace Platform
{

using System;
using System.Runtime.InteropServices;

internal class TimeMethods
{

	// Get the time in microseconds since 12:00am 1 Jan 0001.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static long GetCurrentTime();

	// Get the number of milliseconds since the system was rebooted.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int GetUpTime();

}; // class TimeMethods

}; // namespace Platform
