/*
 * Socket.cs - Implementation of the "System.Net.Sockets.Socket" class.
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

namespace System.Net.Sockets
{

using Platform;
using System;
using System.Net;

public class Socket : IDisposable
{
	//Internal variables
	private int disposed = 1;	//If this instance gets disposed this is set to 0
	private AddressFamily myaddressfamily;
	private ProtocolType myprotocoltype; 
	private SocketType mysockettype;
	private EndPoint mylocalendpoint;
	private EndPoint myremotendpoint;
	private bool connected = false;
	private bool blocking = false; 
	
	//This represents the number of pending asyncronous calls
	private int pending;
	
	
	[TODO]
	public Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
			{
				//A couple of convenience variables for the enums
				AddressFamily af;
				SocketType st;
				ProtocolType pt;
				
				//Check list for invalid combinations
				if ((addressFamily == af.Unknown) || (protocolType == pt.Unknown))
					throw new SocketException();
				if ((socketType == st.Dgram) && !((protocolType == Udp) && (addressFamily == af.InterNetwork)))
					throw new SocketException();
				if ((socketType == st.Raw) && !((protocolType != pt.Tcp) && (protocolType != pt.Udp)))
					throw new SocketException();
				if ((socketType == st.Stream) && !((protocolType == Tcp) && (addressFamily == af.InterNetwork)))
					throw new SocketException();
					
				myaddressfamily = addressFamily;
				myprotocoltype = protocolType;
				mysockettype = socketType;	
				
				//Now create the socket
				//TODO: call the intercall
			}
			
	[TODO]	
	public Socket Accept()
			{	
				if (disposed == 0)			
					throw new ObjectDisposedException(_("Exception_Disposed"));
				if (blocking == false)
					return null;
				if (pending != 0) //This function blocks
					throw new InvalidOperationException(_("Invalid_AsyncAndBlocking"));
				
				blocking = true;
				
				try
				{
					//Now create the socket
					Socket newsock = 0; //TODO: exchange the 0 for the intercall
				}
				catch(Exception e)
				{
					throw new ArgumentException(_("Arg_SocketCreating"));
				}
	
				try
				{
					newsock.myaddressfamily = this.myaddressaamily;
					newsock.blocking = this.blocking;
					newsock.mylocalendpoint = this.mylocalendpoint;
					newsock.myprotocoltype = this.myprotocoltype;
					newsock.mysockettype = this.mysockettype;
					newsock.connected = true;
					//TODO: Set the remote endpoint of the new socket	
				}
				catch(Exception e)
				{
					throw new SocketException();
				}
			}
			
	[TODO]
	public IAsyncResult BeginAccept(AsyncCallback callback, object state)
			{
			
			}
	
	[TODO]
	public IAsyncResult BeginConnect(EndPoint remoteEP, AsyncCallback callback, object state)
			{
			
			}
	
	[TODO]
	public IAsyncResult BeginReceive(byte[] buffer, int offset, int size, SocketFlags socketFlags, AsyncCallback callback, object state)
			{
			
			}	
	
	[TODO]
	public IAsyncResult BeginReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP, AsyncCallback callback, object state)		
			{
			
			}
	
	[TODO]
	public IAsyncResult BeginSend(byte[] buffer, int offset, int size, SocketFlags socketFlags, AsyncCallback callback, object state)
			{
			
			}
			
	[TODO]		
	public IAsyncResult BeginSendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP, AsyncCallback callback, object state)
			{
			
			}
	
	[TODO]
	public void Bind(EndPoint localEP)
			{
				if (disposed == 0)
					throw new ObjectDisposedException(_("Exception_Disposed"));
				
				if (localEP == 0)
					throw new ArgumentNullException("localEP", _("Arg_NotNull"));					
				
				//TODO: check for security problems and eventually throw
				//SecurityException

				//TODO: Bind the socket and eventually throw SocketException
								
			}
	
	[TODO]
	public void Close()
			{
				
				Dispose(true);
			}	
			
	[TODO]
	public void Connect(EndPoint remoteEP)
			{
				if (disposed == 0)
					throw new ObjectDisposedException(_("Exception_Disposed"));
				
				if (localEP == 0)
					throw new ArgumentNullException("remoteEP", _("Arg_NotNull"));					
				
				//Connect is blocking I believe
				if (pending != 0) //This function blocks
					throw new InvalidOperationException(_("Invalid_AsyncAndBlocking"));				
				
				blocking = true;
				
				//TODO: Call the intercall and check for security and socket errors				
							
			}
	
	[TODO]
	protected virtual void Dispose(bool disposing)
			{
			
			}
	
	[TODO]
	public Socket EndAccept(IAsyncResult asyncResult)
			{
			
			}
	
	[TODO]
	public void EndConnect(IAsyncResult asyncResult)
			{
			
			}
	
	[TODO]
	public int EndReceive(IAsyncResult asyncResult)
			{
			
			}
			
	[TODO]
	public int EndReceiveFrom(IAsyncResult asyncResult, ref EndPoint endPoint)
			{
			
			}
	
	[TODO]
	public int EndSend(IAsyncResult asyncResult)	
			{
			
			}
	
	[TODO]
	public int EndSendTo(IAsyncResult asyncResult)
			{
			
			}
	
	[TODO]
	~Socket()
			{
		
			}
		
	[TODO]
	public override int GetHashCode()
			{
		
			}
		
	[TODO]
	public object GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName)
			{
			
			}
	
	[TODO]
	public void GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue)
			{
			
			}
	
	[TODO]
	public byte[] GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionLength)
			{
			
			}
			
	[TODO]
	public int IOControl(int ioControlCode, byte[] optionInValue, byte[] optionOutValue)
			{
			
			}
	
	[TODO]
	public void Listen(int backlog)
			{
			
			}
	
	[TODO]
	public bool Poll(int microSeconds, SelectMode mode)
			{
			
			}
			
	[TODO]
	public int Receive(byte[] buffer, int size, SocketFlags socketFlags)
			{
				Receive(buffer, 0, buffer.Length, socketFlags);
			}
			
	public int Receive(byte[] buffer)
			{
				Receive(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None);
			}
	
	[TODO]
	public int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags)
			{
			
			}
			
	[TODO]
	public int ReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				
			}
			
	public int ReceiveFrom(byte[] buffer, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				ReceiveFrom(buffer, 0, size , socketFlags, remoteEP);		
			}
	
	public int ReceiveFrom(byte[] buffer, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				ReceiveFrom(buffer, 0, buffer.Length, socketFlags, remoteEP);
			}
	
	public int ReceiveFrom(byte[] buffer, ref EndPoint remoteEP)
			{
				ReceiveFrom(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None, remoteEP);			
			}
	
	[TODO]
	public static void Select(IList checkRead, IList checkWrite, IList checkError, int microSeconds)
			{
		
			}
	[TODO]
	public int Send(byte[] buffer, int size, SocketFlags socketFlags)		
			{
			
			}
	
	public int Send(byte[] buffer, SocketFlags socketFlags)
			{
				Send(buffer, 0, buffer.Length, socketFlags);
			}
	
	public int Send(byte[] buffer)
			{
				Send(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None);
			}
	
	[TODO]
	public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags)
			{
			
			}
			
	[TODO]
	public int SendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP)
			{
			
			}
	
	
	public int SendTo(byte[] buffer, int size, SocketFlags socketFlags, EndPoint remoteEP)
			{
				SendTo(buffer, 0, size, socketFlags, remoteEP);
			}
	
	public int SendTo(byte[] buffer, SocketFlags socketFlags, EndPoint remoteEP)
			{
				SendTo(buffer, 0, buffer.Length, socketFlags, remoteEP);
			}	
	
	public int SendTo(byte[] buffer, EndPoint remoteEP)
			{
				SendTo(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None, remoteEP);
			}
	
	[TODO]
	public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionValue)	
			{
			
			}
	
	[TODO]
	public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue)
			{
			
			}
	
	[TODO]
	public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, object optionValue)
			{
			
			}
	
	[TODO]
	public void Shutdown(SocketShutdown how)
			{
			
			}
	[TODO]
	void IDisposable.Dispose()
			{
			
			}
	
	public AddressFamily AddressFamily
			{
				get
				{
					return myaddressfamily;
				}
			}
	
	[TODO]
	public int Available { get; }

	[TODO]
	public bool Blocking { get; set; }

	[TODO]
	public bool Connected 
			{ 
				get
				{
					return connected;
				}
			}
	
	[TODO]
	public IntPtr Handle { get; }
	
	[TODO]
	public EndPoint LocalEndPoint { get; }	
	
	public ProtocolType ProtocolType
			{
				get
				{
					return myprotocoltype;
				}
			}
	
	[TODO]
	public EndPoint RemoteEndPoint { get; }

	public SocketType SocketType	
			{
				get
				{
					return mysockettype;
				}
			}
}; // class Socket

}; // namespace System.Net.Sockets


