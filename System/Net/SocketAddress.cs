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
	
	//An ipaddress is stored as a long in the same way as in IPAddress
	//the 2 bytes after the IPAddress are the port
		
	[TODO]
	public SocketAddress(AddressFamily family)
			{
	
			}
	
	[TODO]
	public SocketAddress(AddressFamily family, int size)
			{
	
			}
	
	[TODO]
	public override bool Equals(object comparand)
			{
		
			}		
	
	[TODO]
	public override int GetHashCode()
			{
	
			}
	
	[TODO]
	public override String ToString()
			{
				return 0;
			}
	
	[TODO]
	public AddressFamily Family 
			{ 
				get
				{
					return 0;
				}			
			}

	[TODO]
	public byte this[int offset]
			{
				get
				{
					return myarray[offset];				
				}
				set
				{
					if (offset < 0 || offset > this.Size())
						throw new ArgumentOutOfRangeException("item");
					myarray[offset] = value;
				}
			}
	
	[TODO]
	public int Size
			{
				get
				{
					return myarray.Count;
				}
			}
		
}; //class SocketAddress

}; //namespace System.Net