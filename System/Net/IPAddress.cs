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

public class IPAddress
{

	// note that the address long is backwards; that is, 1.2.3.4 becomes
	// 0x0000000004030201
	// at least it is here. However, host-byte-order is what it is supposed to be,
	// and I don't know if all hosts are little word, little endian ;)
	private long value__;

	IPAddress(long newAddress)
			{
				if ((newAddress < 0) || (newAddress > 0x00000000FFFFFFFF))
					throw new ArgumentOutOfRangeException("newAddress",_("Arg_OutOfRange"));
				this.value__ = newAddress;

				// Any, Broadcast, Loopback, and None are static
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

	public override int GetHashCode() 
			{
				return unchecked(((int)(value__ ^ (value__ >> 32)))
										& 0xFFFFFFFF);
			}
	// I think the next three will have to be InternalCalls
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
	public static bool IsLoopback(IPAddress address)
			{
				// careful here, after all, 0x80 >= 0x7F, but should return false
				// only LSByte value matters
				return ((address.value__ & 0x00000000000000FF) == 0x000000000000007F);
			}
	// I think the next three will have to be InternalCalls
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
	public static IPAddress Parse(String ipString)
			{
				IPAddress parsed;
				String[]  tokenizedString;
				ulong quadA;
				ulong quadB;
				ulong quadC;
				ulong quadD;
				bool  numbersign;

				if (ipString == null)
				{
					throw new ArgumentNullException("ipString",_("Arg_NotNull"));
				}

				char[] point = {'.'};
				// this only takes char[]. not String
				tokenizedString = String.Split(point, 4);
				
				if (tokenizedString.Length < 4)
				{
					throw new FormatException("ipString", _("Format_IP"));
				}

				// this might be easier to understand were it implemented
				// in terms of System.Byte.Parse(String)
				if ((!NumberParser.StringToNumber(tokenizedString[0], 10, quadA,
 numbersign)) ||
					(!NumberParser.StringToNumber(tokenizedString[1], 10, quadB, numbersign))
 ||
					(!NumberParser.StringToNumber(tokenizedString[2], 10, quadC, numbersign))
 ||
					(!NumberParser.StringToNumber(tokenizedString[3], 10, quadD, numbersign)))
				{
					throw new FormatException("ipString", _("Format_IP"));
				}

				parsed = (quadA + (quadB << 2) + (quadC << 4) + (quadD << 6));

				return parsed;
			}	
	public override string ToString()
			{
				string ip;

				ip = NumberFormatter.FormatFixedPoint((ulong)value__ & 0x00000000000000FF,
 0, 0, false, null, 
										 NumberFormatInfo.GetInstance(provider)) +
					"." + 
					NumberFormatter.FormatFixedPoint((ulong)value__ & 0x000000000000FF, 0, 0,
 false, null, 
										 NumberFormatInfo.GetInstance(provider)) +
					"." + 
					NumberFormatter.FormatFixedPoint((ulong)value__ & 0x0000000000FF, 0, 0,
 false, null, 
										 NumberFormatInfo.GetInstance(provider)) +
					"." + 
					NumberFormatter.FormatFixedPoint((ulong)value__ & 0x00000000FF, 0, 0,
 false, null, 
										 NumberFormatInfo.GetInstance(provider));

				return ip;
			}

	public static readonly IPAddress Any = new IPAddress(0x0000000000000000);
	public static readonly IPAddress Broadcast = new IPAddress(0x00000000FFFFFFFF);
	public static readonly IPAddress Loopback = new IPAddress(0x000000000100007F);
	public static readonly IPAddress None = Broadcast;

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

