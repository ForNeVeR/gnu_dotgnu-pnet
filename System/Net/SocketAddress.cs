/*
 * SocketAddress.cs - Implementation of the "System.Net.SocketAddress" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * This program is free software, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace System.Net
{
	
using System;
using System.Net.Sockets;

public class SocketAddress
{
	private byte[] myarray;
	private AddressFamily family;	
	
	public SocketAddress(AddressFamily family) 
		: SocketAddress(family, 32)
	{
	}
		
	public SocketAddress(AddressFamily family, int size)
	{
		this.family = family;
		if (size < 2) 
		{
				throw new ArgumentOutOfRangeException();
		}
		this.myarray = new byte[size];
	}
		
	public override Equals(object comparand) 
	{
		if (comparand is SocketAddress)
		{
			return ((SocketAddress)comparand).ToString() ==
			ToString();
		}
		else
		{
			return false;
		}

	}

	[TODO]
	public override int GetHashCode()
	{
			return 123;
	}
		
	public override string ToString()
	{
		string str;
		switch(this.family) 
		{	
			case AddressFamily.AppleTalk:
				str += "AppleTalk:";
				break;
			case AddressFamily.Atm:
				str += "Atm:";
				break;
			case AddressFamily.Banyan:
				str += "Banyan:";
				break;
			case AddressFamily.Ccitt:
				str += "Ccit:";
				break;
			case AddressFamily.Chaos:
				str += "Chaos:";
				break;
			case AddressFamily.Cluster:
				str += "Cluster:";
				break;
			case AddressFamily.DataKit:
				str += "DataKit:";
				break;
			case AddressFamily.DataLink:
				str += "DataLink:";
				break;
			case AddressFamily.DecNet:
				str += "DecNet:";
				break;
			case AddressFamily.Ecma:
				str += "Ecma:";
				break;
			case AddressFamily.FireFox:
				str += "FireFox:";
				break;
			case AddressFamily.HyperChannel:
				str += "HyperChannel:";
				break;
			case AddressFamily.Ieee1284:
				str += "Ieee1284:";
				break;
			case AddressFamily.ImpLink:
				str += "ImpLink:";
				break;		
			case AddressFamily.InterNetwork:
				str += "InterNetwork:";
				break;
			case AddressFamily.InterNetworkV6:
				str += "InterNetworkV6:";
				break;
			case AddressFamily.Ipx:
				str += "Ipx:";
				break;
			case AddressFamily.Irda:
				str += "Irda:";
				break;
			case AddressFamily.Iso:
				str += "Iso:";
				break;
			case AddressFamily.NS:
				str += "NS:";
				break;
			case AddressFamily.NetBios:
				str += "NetBios:";
				break;
			case AddressFamily.NetworkDesigners:
				str += "NetworkDesigners:";
				break;
			case AddressFamily.Osi:
				str += "Osi:";
				break;	
			case AddressFamily.Pup:
				str += "Pup:";
				break;
			case AddressFamily.Sna:
				str += "Sna:";
				break;
			case AddressFamily.Unix:
				str += "Unix:";
				break;
			case AddressFamily.Unknown:
				str += "Unknown:";
				break;
			case AddressFamiy.Unspecified:
				str += "Unspecified:";
				break;
			case AddressFamily.VoiceView:
				str += "VoiceView:";
				break;
			default:
				return null;
		}
		str += myarray.Count.ToString();
		str += ':';
		str += '{';
		string tmp;
		foreach(byte x in myarray) 
		{
			tmp += x.ToString();
			tmp += ',';
		}
		// Got to get rid of trailing ','	
		tmp = tmp.Substring(0, tmp.Length-1);
		str += tmp;
		str += '}';
		return str;
	}
		
	public AddressFamily Family
	{
		get
		{
			return family;
		}
	}
	
	public byte this[int offset]
	{
	 	get
		{
			return myarray[offset];				
		}
		set
		{
			if (offset < 0 || offset > this.Size)
				throw new ArgumentOutOfRangeException("item");
				myarray[offset] = value;
		}
	}
	
	
	public int Size
	{
		get
		{
			return myarray.Count;
		}
	}	


   }
} // class SocketAddress












