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

internal class SocketMethods
{
	// Create a socket and return the socket descriptor (true on success).
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Create(AddressFamily af, SocketType st, ProtocolType pt, out IntPtr handle);

	// Bind a socket to an EndPoint (true on success).
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Bind(IntPtr handle, EndPoint EP);

	// Shutdown a socket
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Shutdown(IntPtr handle, SocketShutdown how);

	// Accept an incoming connection
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static  bool Accept(IntPtr handle);

	// Connect to specified End Point
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int Connect(IntPtr handle, EndPoint EP);

	// Receive bytes from connected socket
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int Receive(IntPtr handle, byte[] buffer, int offset, int size, int flags);

	// Receive bytes from specified EndPoint
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int ReceiveFrom(IntPtr handle, byte[] buffer, int offset, int size, int flags, ref EndPoint EP);

	// Send bytes to connected socket
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int Send(IntPtr handle, byte[] buffer, int offset, int size, int flags);

	// Receive bytes from specified EndPoint
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int SendTo(IntPtr handle, byte[] buffer, int offset, int size, int flags, EndPoint EP);

	// Close a socket (regardless of pending in/output)
	extern public static bool Close(IntPtr handle);

	// Get the last-occurring system error code for the current thread.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Errno GetErrno();

	// Get a descriptive message for an error from the underlying platform.
	// Returns null if the platform doesn't have an appropriate message.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static String GetErrnoMessage(Errno errno);

}; // class SocketMethods

}; // namespace Platform
