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
using System.Threading;

internal class SocketMethods
{

	// Get the invalid socket handle.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static IntPtr GetInvalidHandle();

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
	// timeout is in microseconds.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int Select(IntPtr[] readarray, IntPtr[] writearray, IntPtr[] errorarray, long timeout);

	// Change the blocking mode on a socket.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool SetBlocking(IntPtr handle, bool blocking);

	// Get the number of available bytes on a socket.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int GetAvailable(IntPtr handle);

	// Get the name of the local end-point on a socket.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool GetSockName(IntPtr handle, out long address,
										  out int port);

	// Set a numeric or boolean socket option.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool SetSocketOption
			(IntPtr handle, int level, int name, int value);

	// Get a numeric or boolean socket option.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool GetSocketOption
			(IntPtr handle, int level, int name, out int value);

	// Set the linger socket option.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool SetLingerOption
			(IntPtr handle, bool enabled, int seconds);

	// Get the linger socket option.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool GetLingerOption
			(IntPtr handle, out bool enabled, out int seconds);

	// Set a multicast socket option.  TODO: ipv6
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool SetMulticastOption
			(IntPtr handle, int name, long group, long mcint);

	// Get a multicast socket option.  TODO: ipv6
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool GetMulticastOption
			(IntPtr handle, int name, out long group, out long mcint);

	// Get the last-occurring system error code for the current thread.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Errno GetErrno();

	// Get a descriptive message for an error from the underlying platform.
	// Returns null if the platform doesn't have an appropriate message.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static String GetErrnoMessage(Errno errno);

	// Determine if we can start threads on this system.  Needed because
	// "Thread.CanStartThreads" is not accessible from this assembly.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool CanStartThreads();

	// Make a backdoor call to "ThreadPool.QueueCompletionItem", to get
	// around the problem that it is not accessible from this assembly
	// via a direct method invocation.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool QueueCompletionItem
			(AsyncCallback callback, IAsyncResult state);

	// Create a "ManualResetEvent" instance.  Backdoor access.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static WaitHandle CreateManualResetEvent();

	// Perform a "Set" on a "WaitHandle" instance that is assumed to
	// be a "ManualResetEvent".  Backdoor access needed in ECMA mode.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void WaitHandleSet(WaitHandle waitHandle);

}; // class SocketMethods

}; // namespace Platform
