/*
 * SocketMethods.cs - Implementation of the "Platform.SocketMethods" class.
 *
 * Copyright (C) 2002 Free Software Foundation
 * Author : Sidney Richards <sidney.richards@xs4all.nl>
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

public class SocketMethods
{
	// Create a socket and obtain a socket descriptor (return true on success).
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Create(int af, int st, int pt, out IntPtr handle);

	// Bind a socket to an IP address
	//TODO: add support for something other than IPv4
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Bind(IntPtr handle, int af, long address, int port);

	// Shutdown a socket
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Shutdown(IntPtr handle, int how);

	// Start listening
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Listen(IntPtr handle, int backlog);

	// Accept an incoming connection
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Accept(IntPtr handle, out long address,
									 out int port, out IntPtr newHandle);

	// Connect to specified address
	// TODO: add support for something other than IPv4
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Connect(IntPtr handle, int af, long address, int port);

	// Receive bytes from connected socket
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int Receive(IntPtr handle, byte[] buffer, int offset, int size, int flags);

	// Receive bytes from specified EndPoint (noted in address and port)
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int ReceiveFrom(IntPtr handle, byte[] buffer, int offset, int size, int flags, out long address, out int port);

	// Send bytes to connected socket
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int Send(IntPtr handle, byte[] buffer, int offset, int size, int flags);

	// Receive bytes from specified EndPoint (noted in address and port)
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int SendTo(IntPtr handle, byte[] buffer, int offset, int size, int flags, long address, int port);

	// Close a socket (regardless of pending in/output)
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Close(IntPtr handle);

	// Determines the read, write and error status of a set of Sockets
	// The arrays are adjusted to reflect which sockets had events.
	// Unused entries are replaced with IntPtr.Zero.  Returns the number
	// of descriptors that fired, 0 on timeout, or -1 on error.  The
	// timeout is in ticks (100 nanosecond intervals).
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int Select(IntPtr[] readarray, IntPtr[] writearray, IntPtr[] errorarray, long timeout);

	// Get the last-occurring system error code for the current thread.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Errno GetErrno();

	// Get a descriptive message for an error from the underlying platform.
	// Returns null if the platform doesn't have an appropriate message.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static String GetErrnoMessage(Errno errno);

}; // class SocketMethods

}; // namespace Platform
