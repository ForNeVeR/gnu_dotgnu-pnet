/*
 * IPv6Address.cs - Implementation of the "System.Net.IPv6Address" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * Contributions by Gopal V <gopalv82@symonds.net>
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

using System;
using System.Net;
using System.Text;
using System.Net.Sockets;
using System.Globalization;
using System.Collections;

namespace System.Net
{
internal class IPv6Address
{

	/* TODO: implement checks for compatibility and conversion to IPv4
	addressing scheme ie ::127.0.0.1 should work in a non-IPv6 environment
	as well */

	private  ushort[] value_;	
	public static readonly IPv6Address LoopBack = IPv6Address.Parse("::1");
	public static readonly IPv6Address Any = IPv6Address.Parse("::");
	/* Note: there are no broad cast IP addresses in IPv6 , only multi-cast
	addresses */
	public IPv6Address(ushort[] values)
	{
		value_ = values;
	}
	public static ushort[] ParseHex(String input)
	{
		if(input==String.Empty)
		{
			return new ushort[0];
		}
		String [] toks = input.Split(':');
		ushort [] retval = new ushort[toks.Length];
		for(int i=0; i< toks.Length; i++)
		{
			retval[i]=UInt16.Parse("0x"+toks[i],NumberStyles.HexNumber);
		}
		return retval;
	}
	private static ushort[] ParseNoCompress(String input)
	{
		if(input==String.Empty)
		{
			return new ushort[0];
		}
		int ipv4Start=input.IndexOf(".");
		int lastPart = input.LastIndexOf(":");
		String ip4 = null;
		ushort [] retval;
		if(ipv4Start!=-1 && lastPart!=-1)
		{
			// F00F::0:13.2.3.4
			if(lastPart > ipv4Start)
			{
				throw new FormatException("Invalid '.' in IPv6 Address"); 
			}
			ip4 = input.Substring(input.LastIndexOf(":")+1);
			input = input.Substring(0, input.LastIndexOf(":"));
		}
		else if(ipv4Start!=-1 && lastPart == -1)
		{
			// ::13.2.2.3
			ip4 = input.Substring(input.LastIndexOf(":")+1);
			input = String.Empty;
		}
		ushort[] hex4 = ParseHex(input);
		
		retval = new ushort[hex4.Length + ((ip4!=null) ? 2 : 0)];
		
		Array.Copy(hex4,retval,hex4.Length);
		if(ip4!=null)
		{
			long ipValue = IPAddress.Parse(ip4).Address; 
			retval [hex4.Length] = (ushort) 
										(((int) (ipValue & 0xff) << 8) + 
										((int) ((ipValue >> 8) & 0xff)));
										
			retval [hex4.Length + 1] = (ushort) 
										(((int) ((ipValue >> 16) & 0xff) << 8)
										+ ((int) ((ipValue >> 24) & 0xff)));
		}
		return retval;
	}	
	public static IPv6Address Parse(String input)
	{
		if(input==null)
		{
			throw new ArgumentNullException("input");
		}
		if(input.IndexOf(":")==-1) 
		{
			throw new FormatException("Not an IPv6 address");
		}
		if(input[0] == '[' && input[input.Length - 1] == ']')
		{
			input = input.Substring(1,input.Length-2);
		}
		bool zeroCompress = (input.IndexOf("::") != -1);
		bool multipleCompress = 
					(input.IndexOf("::") != input.LastIndexOf("::"));
		if(!zeroCompress)
		{
			ushort[] retval=ParseNoCompress(input);
			if(retval.Length != 8)
			{
				throw new FormatException("Not enough address fields");
			}
			return new IPv6Address(retval);
		}
		else if(!multipleCompress)
		{
			String part1=input.Substring(0,input.IndexOf("::"));
			String part2=input.Substring(input.IndexOf("::")+2);
			ushort[] retval1=ParseNoCompress(part1);
			ushort[] retval2=ParseNoCompress(part2);
			if((retval1.Length + retval2.Length) >= 8)
				throw new FormatException("Invalid zero compress");
			ushort[] retval = new ushort[8]{0,0,0,0,0,0,0,0};
			Array.Copy(retval1, retval, retval1.Length);
			Array.Copy(retval2, 0, retval, 8-retval2.Length, retval2.Length);
			return new IPv6Address(retval);
		}
		throw new FormatException("Multiple Zero compress");
	}

	public static bool IsLoopback (IPv6Address ip)
	{
		if(ip == null)
		{
			throw new ArgumentNullException("ip");
		}
		/* Note: this is slightly different from normal IPAddresses
		because ::127.0.0.1 is a loopback as well due to the IPv4
		mapping which are there for either IPv4 mapped or compatible
		ie 5th uint16 , and the higher ones are zero'd , 6 th ushort
		is 0x7fXX (or 127.*.*.*). In the end we have the full IPv6
		loopback as well which is ::1 */
		if((ip.value_[5] !=0) && (ip.value_[5] != 0xffff))
		{
			return false;
		}
		for(int i = 0 ; i < 4 ;i++)
		{
			if(ip.value_[i] !=0)
			{
				return false;
			}
		}
		if((ip.value_[6] >> 8) == 0x7f)
		{
			return true;
		}
		return ((ip.value_[6] == 0) && (ip.value_[7] == 1));
	} 

	public override String ToString()
	{
		StringBuilder sb=new StringBuilder();
		for(int i=0;i<7;i++)
		{
			sb.Append(String.Format("{0:X}:",value_[i]));
		}
		sb.Append(String.Format("{0:X}",value_[7]));
		return sb.ToString();
	}

	public ushort[] Address
	{
		get
		{
			return value_;
		}
	}

	public AddressFamily AddressFamily
	{
		get
		{
			return AddressFamily.InterNetworkV6;
		}
	}

	/* TODO: add IPv4 mapping/compatible based parameters as well 
		introduce checks for multi-case prefixes for IPv6 */
}
}
