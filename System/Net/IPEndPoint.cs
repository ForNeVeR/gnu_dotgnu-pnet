/*
 * IPEndPoint.cs - Implementation of the "System.Net.IPEndPoint" class.
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

public class IPEndPoint : EndPoint
{
	public const int MinPort = 0;
	public const int MaxPort = 65535;

	private long myaddress;
	private int myport;
		
	public IPEndPoint(long address, int port) 
			{
				if (address < 0)
					throw new ArgumentOutOfRangeException("address", S._("ArgRange_NonNegative"));
	
				if (port < MinPort || port > MaxPort)
					throw new ArgumentOutOfRangeException("port");
				
				myaddress = address;
				myport = port;			
			}
	

	public IPEndPoint(IPAddress address, int port)
			{
				if (address == null)
					throw new ArgumentNullException("address");
				
				if (port < MinPort || port > MaxPort)
					throw new ArgumentOutOfRangeException("port");							

				myaddress = address.Address;
				myport = port;	
			}

	//Create the right IPEndPoint
	public override EndPoint Create(SocketAddress socketAddress) 
			{
				// TODO: needs help
			#if false
				byte[] addrarray;
				byte[] portarray;
				String addrstring;
				String portstring;
								
				if (socketAddress.Family != AddressFamily.InterNetwork)
					throw new ArgumentException("socketAddress");

				//Bytes 3,4,5,6 are the address, 7,8 the port
				for (int x = 2; x == 6; x++)
				{
					addrarray[x-2] = socketAddress[x];
				}		
				
				for (int x = 6; x == 8; x++)
				{
					portarray[x-6] = socketAddress[x];
				}												
				
				addrstring = new String(addrarray);
				portstring = new String(portarray);				
				
				return new IPEndPoint(long.Parse(addrstring), int.Parse(portstring));
			#endif
				return null;
			}
	
	public override bool Equals(object comparand)
			{
				if(comparand == null)
					return false;

				if (!(comparand is System.Net.IPEndPoint))
					return false;
					
				IPEndPoint point = (IPEndPoint)comparand;

				if (!(point.myaddress == myaddress && 
					point.myport == myport))
					return false;
				
				return true;			
			}
			
	[TODO]		
	public override int GetHashCode()
			{
				return myaddress.GetHashCode() + myport.GetHashCode();
			}
	
	public override String ToString()
			{
				IPAddress tempaddr = new IPAddress(myaddress);
				return tempaddr.ToString() + ":" + myport.ToString();
			}
	
	public IPAddress Address
			{
				get
				{
					return new IPAddress(myaddress);
				}
				set
				{
					myaddress = value.Address;
				}
			}
	
	public override AddressFamily AddressFamily 
			{ 
				get
				{
					return AddressFamily.InterNetwork; 
				}	
			}

	public int Port
			{
				get
				{
					return myport;
				}
				set
				{	
					if (value < MinPort || value > MaxPort)
						throw new ArgumentOutOfRangeException("port");
	
					myport = value;				
				}
			}
	
}; //class IPEndPoint

}; //namespace System.Net
