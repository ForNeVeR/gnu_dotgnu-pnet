/*
 * Stdio.cs - Implementation of the "Platform.Stdio" class.
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
using System.Runtime.CompilerServices;

internal class Stdio
{
	// Close a standard file descriptor.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void StdClose(int fd);

	// Flush a standard file descriptor.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void StdFlush(int fd);

	// Write to a standard file descriptor.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void StdWrite(int fd, char value);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void StdWrite(int fd, char[] value,
									   int index, int count);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void StdWrite(int fd, byte[] value,
									   int index, int count);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void StdWrite(int fd, String value);

	// Read from a standard file descriptor.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int StdRead(int fd);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int StdRead(int fd, char[] value,
								     int index, int count);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int StdRead(int fd, byte[] value,
								     int index, int count);

	// Peek from a standard file descriptor.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int StdPeek(int fd);

	// Set the console mode.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void SetConsoleMode(int mode);

	// Supported console modes.
	public const int MODE_NORMAL     = 0;
	public const int MODE_CBREAK     = 1;
	public const int MODE_RAW        = 2;
	public const int MODE_CBREAK_ALT = 3;
	public const int MODE_RAW_ALT    = 4;

	// Get the buffer dimensions for the console.  If the console
	// does not support scroll-back buffering, or cannot query the
	// value, then this returns the same value as the window size.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void GetBufferSize(out int width, out int height);

	// Get the window size and position information.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void GetWindowSize
			(out int left, out int top, out int width, out int height);

	// Get the cursor position.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void GetCursorPosition(out int x, out int y);

	// Set the cursor position.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void SetCursorPosition(int x, int y);

	// Set the title on the console window.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void SetConsoleTitle(String title);

	// Issue a beep on the console.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void Beep();

	// Clear the console to the current foreground and backgroun colors.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void Clear();

	// Set the foreground and background text attributes.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void SetTextAttributes(int attrs);

	// Determine if a key is available for reading at the console.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool KeyAvailable();

	// Read a key from the console.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void ReadKey
			(out char ch, out int key, out int modifiers);

}; // class Stdio

}; // namespace Platform
