/*
 * IPAddress.cs - Implementation of the "System.Net.IPAddress" class.
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

public class IPAddress
{
	private long value__;
	
	public IPAddress(long newAddress) 
			{
				if ((newAddress < 0) || (newAddress > 0x00000000FFFFFFFF))
					throw new ArgumentOutOfRangeException("newAddress",_("Arg_OutOfRange")); 
				value__ = newAddress;
				
				Any = new IPAddress(0x0000000000000000);
				Broadcast = new IPAddress(0x00000000FFFFFFFF);
				Loopback = new IPAddress(0x000000000100007F);
				None = new IPAddress(0x00000000FFFFFFFF);
			}
	
	public override bool Equals(Object comparand)
			{
				if (comparand is IPAddress)
				{
					return (value__ == ((IPAddress)comparand).value__);
				}
				else
				{
					return false;
				}				
			}

	[TODO]
	public override int GetHashCode() 
			{
			
			}
	
	[TODO]
	public static long HostToNetworkOrder(long host)
			{
				return 0;
			}
	[TODO]
	public static int HostToNetworkOrder(int host)
			{
				return 0;
			}
	[TODO]
	public static short HostToNetworkOrder(short host)
			{
				return 0;
			}
	[TODO]
	public static bool IsLoopback(IPAddress address)
			{
				
			}
	
	[TODO]
	public static long NetworkToHostOrder(long network)
			{
				return 0;
			}
	[TODO]
	public static int NetworkToHostOrder(int network)
			{
				return 0;
			}	
	[TODO]	
	public static short NetworkToHostOrder(short network)
			{
				return 0;
			}
	[TODO]
	public static IPAddress Parse(string ipString)
			{
				
			}	
	[TODO]		
	public override string ToString()
			{
				return "";
			}
	
	public static readonly IPAddress Any;
	public static readonly IPAddress Broadcast;
	public static readonly IPAddress Loopback;
	public static readonly IPAddress None;
	
	public long Address
			{
				get
				{
					return value__;
				}
				set
				{
					if ((value < 0) || (value > 0x00000000FFFFFFFF))
						throw new ArgumentOutOfRangeException("newAddress",_("Arg_OutOfRange"));
					value__ = value;
				}
			}
			
	public AddressFamily AddressFamily
			{
				get
				{
					return AddressFamily.InterNetwork;
				}
			}
	
}; // class IPAddress

}; // namespace System.Net

