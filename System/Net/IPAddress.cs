/*
 * IPAddress.cs - Implementation of the "System.Net.IPAddress" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * Contributions by Gerard Toonstra <toonstra@ntlworld.com>
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
using System.Net.Sockets;
using System.Runtime.CompilerServices;

public class IPAddress
{

	// The IP address is stored in the low 4 bytes of "value__"
	// in network byte order.
	private long value__;

	public IPAddress(long newAddress)
			{
				if((newAddress < 0) || (newAddress > 0x00000000FFFFFFFF))
				{
					throw new ArgumentOutOfRangeException
						("newAddress", S._("Arg_OutOfRange"));
				}
				this.value__ = newAddress;
			}

	public override bool Equals(Object comparand)
			{
				if(comparand is IPAddress)
				{
					return (value__ == ((IPAddress)comparand).value__);
				}
				else
				{
					return false;
				}
			}

	public override int GetHashCode() 
			{
				return unchecked(((int)(value__ ^ (value__ >> 32)))
										& (int)0x7FFFFFFF);
			}

	// Convert from host to network order.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static long HostToNetworkOrder(long host);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int HostToNetworkOrder(int host);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static short HostToNetworkOrder(short host);

	// Determine if an address corresponds to the loopback interface.
	public static bool IsLoopback(IPAddress address)
			{
				long mask = (long)(uint)HostToNetworkOrder(0x7F000000);
				return ((address.value__ & mask) == mask);
			}

	// Convert from network to host order.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static long NetworkToHostOrder(long network);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int NetworkToHostOrder(int network);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static short NetworkToHostOrder(short network);

	public static IPAddress Parse(String ipString)
			{
				int parsed;
				String[]  tokenizedString;
				int quadA;
				int quadB;
				int quadC;
				int quadD;
				bool  numbersign;

				if (ipString == null)
				{
					throw new ArgumentNullException
						("ipString", S._("Arg_NotNull"));
				}

				// this only takes char[]. not String
				tokenizedString = ipString.Split(new char[]{'.'}, 4);

				if (tokenizedString.Length < 4)
				{
					throw new FormatException(S._("Format_IP"));
				}

				try
				{
					quadA = Byte.Parse(tokenizedString[0]);
					quadB = Byte.Parse(tokenizedString[1]);
					quadC = Byte.Parse(tokenizedString[2]);
					quadD = Byte.Parse(tokenizedString[3]);
				}
				catch(OverflowException)
				{
					throw new FormatException(S._("Format_IP"));
				}

				parsed = (quadD + (quadC << 8) + (quadB << 16) + (quadA << 24));

				return new IPAddress((long)(uint)HostToNetworkOrder(parsed));
			}	
	public override string ToString()
			{
				int host = NetworkToHostOrder((int)value__);
				return ((host >> 24) & 0xFF).ToString() + "." +
					   ((host >> 16) & 0xFF).ToString() + "." +
					   ((host >> 8) & 0xFF).ToString() + "." +
					   (host & 0xFF).ToString();
			}

	public static readonly IPAddress Any = new IPAddress(0x0000000000000000);
	public static readonly IPAddress Broadcast =
			new IPAddress((long)(uint)HostToNetworkOrder
							(unchecked((int)0xFFFFFFFF)));
	public static readonly IPAddress Loopback =
			new IPAddress((long)(uint)HostToNetworkOrder(0x7F000001));
	public static readonly IPAddress None = Broadcast;

	public long Address
			{
				get
				{
					return value__;
				}
				set
				{
					if((value < 0) || (value > 0x00000000FFFFFFFF))
					{
						throw new ArgumentOutOfRangeException
							("newAddress", S._("Arg_OutOfRange"));
					}
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

