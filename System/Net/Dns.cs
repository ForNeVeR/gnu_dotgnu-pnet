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
using System.Runtime.CompilerServices;
using System.Net;
using System.Net.Sockets;

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
	public static IPHostEntry GetHostByAddress(String address)
			{
				//allow Parse to throw FormatException or ArgumentNullException
				IPAddress ip=IPAddress.Parse(address);
				return GetHostByAddress(ip);
			}

	public static IPHostEntry GetHostByAddress(IPAddress address)
			{
				if(address==null)
				{
					throw new ArgumentNullException("address");
				}
				String h_name;
				String [] h_aliases;
				long [] h_addr_list;
				
				if(!InternalGetHostByAddr(address.Address, out h_name, 
						out h_aliases, out h_addr_list))
				{
					throw new SocketException(); // Hm...	
				}
				return ToIPHostEntry(h_name,h_aliases,h_addr_list);
			}

	// Get a host by name synchronously.
	public static IPHostEntry GetHostByName(String hostName)
			{
				if(hostName==null)
				{
					throw new ArgumentNullException("hostname");
				}
				String h_name;
				String [] h_aliases;
				long [] h_addr_list;
				
				if(!InternalGetHostByName(hostName, out h_name, 
						out h_aliases, out h_addr_list))
				{
					throw new SocketException(); // Hm...	
				}
				return ToIPHostEntry(h_name,h_aliases,h_addr_list);
			}

	// Get the host name of the local machine.
	public static String GetHostName()
			{
				IPHostEntry x=GetHostByAddress(IPAddress.Loopback);
				// this is a crude hack , call gethostname() from
				// icalls later
				return x.HostName;
			}

	// Resolve a name or IP address synchronously.
	public static IPHostEntry Resolve(String hostName)
			{
				return GetHostByName(hostName);
			}
	
	private static IPHostEntry ToIPHostEntry(String h_name,
				String []h_aliases,long[] h_addr_list)
	{
		IPHostEntry entry=new IPHostEntry();
		entry.HostName=h_name;
		entry.Aliases=h_aliases;
		entry.AddressList=new IPAddress[h_addr_list.Length];
		for(int i=0;i<h_addr_list.Length;i++)
		{
			entry.AddressList[i]=new IPAddress(h_addr_list[i]);
		}
		return entry;
	}

	//helper functions
	[MethodImpl(MethodImplOptions.InternalCall)]
	private extern static bool InternalGetHostByName(String host,out String h_name,
	out String[] h_aliases, out long[] h_addr_list);

	[MethodImpl(MethodImplOptions.InternalCall)]
	private extern static bool InternalGetHostByAddr(long address,out String h_name,
	out String[] h_aliases, out long[] h_addr_list);

}; // class Dns

}; // namespace System.Net
