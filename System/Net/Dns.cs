/*
 * Dns.cs - Implementation of the "System.Net.Dns" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.Net
{

using System;

public sealed class Dns
{

	// Begin an asynchronous "get host by name" operation.
	[TODO]
	public static IAsyncResult BeginGetHostByName
		(String hostName, AsyncCallback requestedCallback,
		 Object stateObject)
			{
				// TODO
				return null;
			}

	// Begin an asynchronous name or IP resolution operation.
	[TODO]
	public static IAsyncResult BeginResolve
		(String hostName, AsyncCallback requestedCallback,
		 Object stateObject)
			{
				// TODO
				return null;
			}

	// End an asynchronous "get host by name" operation.
	[TODO]
	public static IPHostEntry EndGetHostByName(IAsyncResult asyncResult)
			{
				// TODO
				return null;
			}

	// End an asynchronous name or IP resolution operation.
	[TODO]
	public static IPHostEntry EndResolve(IAsyncResult asyncResult)
			{
				// TODO
				return null;
			}

	// Get a host by address synchronously.
	[TODO]
	public static IPHostEntry GetHostByAddress(String address)
			{
				// TODO
				return null;
			}
	[TODO]
	public static IPHostEntry GetHostByAddress(IPAddress address)
			{
				// TODO
				return null;
			}

	// Get a host by name synchronously.
	[TODO]
	public static IPHostEntry GetHostByName(String hostName)
			{
				// TODO
				return null;
			}

	// Get the host name of the local machine.
	[TODO]
	public static String GetHostName()
			{
				// TODO
				return null;
			}

	// Resolve a name or IP address synchronously.
	[TODO]
	public static IPHostEntry Resolve(String hostName)
			{
				// TODO
				return null;
			}

}; // class Dns

}; // namespace System.Net
