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
using System.Collections;

[TODO]
public class Socket : IDisposable
{
	//TODO: The security system should check for caller permissions
	//TODO: Give the socket errors nice messages
	
	//A note on blocking: methods can block, but they don't set blocking,
	//blocking is set by the user using property Blocking to specify if
	//blocking methods are allowed	

	//Internal variables
	private int disposed = 1;	//If this instance gets disposed this is set to 0
	private AddressFamily myaddressfamily;
	private ProtocolType myprotocoltype; 
	private SocketType mysockettype;
	private EndPoint mylocalendpoint = null;
	private EndPoint myremoteendpoint = null;
	private bool listening = false;
	private bool connected = false;
	private bool blocking = true; //This class standard blocks
	//The handle of this socket
	private IntPtr myhandle;
		
	//This represents the number of pending asyncronous calls
	private int pending;
	
	public Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
			{
				//Check list for invalid combinations
				if ((addressFamily == AddressFamily.Unknown) || (protocolType == ProtocolType.Unknown))
					throw new SocketException();
				if ((socketType == SocketType.Dgram) && !((protocolType == ProtocolType.Udp) && (addressFamily == AddressFamily.InterNetwork)))
					throw new SocketException();
				if ((socketType == SocketType.Raw) && !((protocolType != ProtocolType.Tcp) && (protocolType != ProtocolType.Udp)))
					throw new SocketException();
				if ((socketType == SocketType.Stream) && !((protocolType == ProtocolType.Tcp) && (addressFamily == AddressFamily.InterNetwork)))
					throw new SocketException();
					
				myaddressfamily = addressFamily;
				myprotocoltype = protocolType;
				mysockettype = socketType;	
				
				if (!(SocketMethods.Create((int)addressFamily, (int)socketType, (int)protocolType, out myhandle)))
					throw new SocketException(SocketMethods.GetErrno(), "Socket");

			}
			
	[TODO]	
	public Socket Accept()
			{	
				if (disposed == 0)			
					throw new ObjectDisposedException(S._("Exception_Disposed"));
				if (blocking == false)
					return null;
				if (pending != 0) //This function blocks
					throw new InvalidOperationException(S._("Invalid_AsyncAndBlocking"));
						
				Socket newsock;
				try
				{
					//Now create the socket
					newsock = new Socket(myaddressfamily, mysockettype, myprotocoltype);
				}
				catch(Exception e)
				{
					throw new ArgumentException(S._("IO_SocketCreating"));
				}
	
				newsock.blocking = this.blocking;
				newsock.mylocalendpoint = this.mylocalendpoint;
				newsock.connected = true;

				//TODO: Set the remote endpoint of the new socket	

				long address;
				int port;

				if (!SocketMethods.Accept(myhandle, out address, out port,
										  out newsock.myhandle))
				{
					throw new SocketException(SocketMethods.GetErrno(), "Accept");
				}
				
				return newsock;
			}
			
	[TODO]
	public IAsyncResult BeginAccept(AsyncCallback callback, object state)
			{
				return null;
			}
	
	[TODO]
	public IAsyncResult BeginConnect(EndPoint remoteEP, AsyncCallback callback, object state)
			{
				return null;
			}
	
	[TODO]
	public IAsyncResult BeginReceive(byte[] buffer, int offset, int size, SocketFlags socketFlags, AsyncCallback callback, object state)
			{
				return null;
			}	
	
	[TODO]
	public IAsyncResult BeginReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP, AsyncCallback callback, object state)		
			{
				return null;
			}
	
	[TODO]
	public IAsyncResult BeginSend(byte[] buffer, int offset, int size, SocketFlags socketFlags, AsyncCallback callback, object state)
			{
				return null;
			}
			
	[TODO]		
	public IAsyncResult BeginSendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP, AsyncCallback callback, object state)
			{
				return null;
			}
	
	public void Bind(EndPoint localEP)
			{
				IPEndPoint ipend;
				
				//Remove when SocketMethods supports more than IPv4
				//TODO: translate the string resource
				if (!(localEP is System.Net.IPEndPoint))
					throw new NotSupportedException("Something else than IPv4 is not yet supported");				

				ipend = (IPEndPoint) localEP;		

				if (disposed == 0)
					throw new ObjectDisposedException(S._("Exception_Disposed"));
				
				if (localEP == null)
					throw new ArgumentNullException("localEP", S._("Arg_NotNull"));					

				/* TODO: fix security code - this is wrong
				if (!Security.CanUseFilemyhandle(myhandle))
					throw new SecurityException("myhandle", S._("Exception_SecurityNotGranted"));			
				*/

				//Change when SocketMethods supports more than IPv4			
				if (!(SocketMethods.Bind(myhandle, (int)(ipend.AddressFamily), ipend.Address.Address, ipend.Port)))
					throw new SocketException(SocketMethods.GetErrno(), "Bind");			

				mylocalendpoint = localEP;				
			}
	
	[TODO] 
	public void Close()
			{
				SocketMethods.Close(myhandle);
				Dispose(true);
				//Do other things that are needed, if needed
			}	
			
	public void Connect(EndPoint remoteEP)
			{		
				IPEndPoint ipend;
				
				//Remove when SocketMethods supports more than IPv4
				// TODO: translate the string
				if (!(remoteEP is System.Net.IPEndPoint))
					throw new NotSupportedException("Something else than IPv4 is not yet supported");				

				ipend = (IPEndPoint) remoteEP;		

				if (disposed == 0)
					throw new ObjectDisposedException(S._("Exception_Disposed"));
				
				if (remoteEP == null)
					throw new ArgumentNullException("remoteEP", S._("Arg_NotNull"));					
				
				if (pending != 0) //This function blocks
					throw new InvalidOperationException(S._("Invalid_AsyncAndBlocking"));				
						
				/* TODO: fix security code - this is wrong
				if (!Security.CanUseFilemyhandle(myhandle))
					throw new SecurityException("myhandle", S._("Exception_SecurityNotGranted"));			
				*/

				//Change when SocketMethods supports more than IPv4					
				if (!(SocketMethods.Connect(myhandle, (int)(ipend.AddressFamily), ipend.Address.Address, ipend.Port)))
					throw new SocketException(SocketMethods.GetErrno(), "Connect");		

				//The specs for property LocalEndPoint say Connect sets
				//LocalEndPoint if not previously set
				//Let's set it to the 'any' setting
				mylocalendpoint = new IPEndPoint(IPAddress.Any, 0);				
																					
				myremoteendpoint = remoteEP;				
			}
	
	[TODO] 
	protected virtual void Dispose(bool disposing)
			{
			
			}
	
	[TODO]
	public Socket EndAccept(IAsyncResult asyncResult)
			{
				return null;
			}
	
	[TODO]
	public void EndConnect(IAsyncResult asyncResult)
			{
			
			}
	
	[TODO]
	public int EndReceive(IAsyncResult asyncResult)
			{
				return 0;
			}
			
	[TODO]
	public int EndReceiveFrom(IAsyncResult asyncResult, ref EndPoint endPoint)
			{
				return 0;
			}
	
	[TODO]
	public int EndSend(IAsyncResult asyncResult)	
			{
				return 0;
			}
	
	[TODO]
	public int EndSendTo(IAsyncResult asyncResult)
			{
				return 0;
			}
	
	[TODO]
	~Socket()
			{
		
			}
		
	public override int GetHashCode()
			{
				return myhandle.GetHashCode();
			}
		
	[TODO]
	public object GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName)
			{
				return null;
			}
	
	[TODO]
	public void GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue)
			{
			
			}
	
	[TODO]
	public byte[] GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionLength)
			{
				return null;
			}
			
	[TODO]
	public int IOControl(int ioControlCode, byte[] optionInValue, byte[] optionOutValue)
			{
				return 0;
			}
	
	public void Listen(int backlog)
			{	
				if (listening)
					return;
					
				listening = true;
				
				if (disposed == 0)			
					throw new ObjectDisposedException(S._("Exception_Disposed"));				
				
				if (connected)
					throw new SocketException();
				
				if(!SocketMethods.Listen(myhandle, backlog))
					throw new SocketException(SocketMethods.GetErrno(), "Listen");
											
			}
	
	[TODO]
	public bool Poll(int microSeconds, SelectMode mode)
			{
				if (disposed == 0)			
					throw new ObjectDisposedException(S._("Exception_Disposed"));		

				if (mode != SelectMode.SelectRead && mode != SelectMode.SelectWrite && mode != SelectMode.SelectError)				
					throw new NotSupportedException(S._("Arg_InvalidEnumValue"));
				
				try
				{
					//Call poll intercall		
					// TODO
				}
				catch(Exception e)
				{
					throw new SocketException();
				}
				
				return false;
			}
	
			
	public int Receive(byte[] buffer, int size, SocketFlags socketFlags)
			{
				return Receive(buffer, 0, buffer.Length, socketFlags);
			}
			
	public int Receive(byte[] buffer)
			{
				return Receive(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None);
			}
	
	public int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags)
			{
				int sizevalue;

				if (disposed == 0)			
					throw new ObjectDisposedException(S._("Exception_Disposed"));				

				if (buffer == null)
					throw new ArgumentNullException("buffer", S._("Arg_NotNull"));							
					
				if ((offset < 0) || (offset > buffer.Length))			
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));
				
				if ((size < 0) || (size > (buffer.Length - offset)))
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));
				
				if (pending != 0) //This function blocks
					throw new InvalidOperationException(S._("Invalid_AsyncAndBlocking"));
							
				if (((int)socketFlags > 0x3) || ((int)socketFlags < 0x0))
					throw new SocketException();
					
				if ((socketFlags && SocketFlags.OutOfBand) != 0 &&
					(mysockettype != SocketType.Stream))
				{
					throw new SocketException();
				}
				
				if ((socketFlags && SocketFlags.Partial) != 0)
					throw new SocketException();
				
				if ((socketFlags && SocketFlags.DontRoute) != 0)
					throw new SocketException(); 

				if (mylocalendpoint == null)
					throw new SocketException();
				
				/* TODO: change security - this is wrong
				if (!Security.CanUseFilemyhandle(myhandle))
					throw new SecurityException("myhandle", S._("Exception_SecurityNotGranted"));					
				*/

				if ((sizevalue = SocketMethods.Receive(myhandle, buffer, offset, size, (int)socketFlags)) == -1)
					throw new SocketException(SocketMethods.GetErrno(), "Receive");
				
				return sizevalue;	

			}
			
	//TODO: Improve intercall
	public int ReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				int sizevalue;

				if (disposed == 0)			
					throw new ObjectDisposedException(S._("Exception_Disposed"));				

				if (buffer == null)
					throw new ArgumentNullException("buffer", S._("Arg_NotNull"));
				
				if (remoteEP == null)
					throw new ArgumentNullException("buffer", S._("Arg_NotNull"));									
					
				if ((offset < 0) || (offset > buffer.Length))			
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));
				
				if ((size < 0) || (size > (buffer.Length - offset)))
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));
				
				if (pending != 0) //This function blocks
					throw new InvalidOperationException(S._("Invalid_AsyncAndBlocking"));
							
				if (((int)socketFlags > 0x3) || ((int)socketFlags < 0x0))
					throw new SocketException();
					
				if ((socketFlags && SocketFlags.OutOfBand) != 0 &&
				    (mysockettype != SocketType.Stream))
					throw new SocketException();
				
				if ((socketFlags && SocketFlags.Partial) != 0)
					throw new SocketException();
				
				if ((socketFlags && SocketFlags.DontRoute) != 0)
					throw new SocketException(); 

				if (mylocalendpoint == null)
					throw new SocketException();
					
				/* TODO: change security - this is wrong, use socketpermission
				if (!Security.CanUseFilemyhandle(myhandle))
					throw new SecurityException("myhandle", S._("Exception_SecurityNotGranted"));					
				*/

				// TODO: allow for more than IPv4
				IPEndPoint ipend;
				remoteEP = ipend;
				
				if ((sizevalue = SocketMethods.ReceiveFrom(myhandle, buffer, offset, size, (int)socketFlags, out ipend.Address, out ipend.Port)) == -1)
					throw new SocketException(SocketMethods.GetErrno(), "ReceiveFrom");
				
				return sizevalue;																						
			}
			
	public int ReceiveFrom(byte[] buffer, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				return ReceiveFrom(buffer, 0, size , socketFlags, ref remoteEP);		
			}
	
	public int ReceiveFrom(byte[] buffer, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				return ReceiveFrom(buffer, 0, buffer.Length, socketFlags, ref remoteEP);
			}
	
	public int ReceiveFrom(byte[] buffer, ref EndPoint remoteEP)
			{
				return ReceiveFrom(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None, ref remoteEP);			
			}
	
	
	public static void Select(IList checkRead, IList checkWrite, IList checkError, int microSeconds)
			{
				if (checkRead == null && checkWrite == null && checkError == null)
					throw new ArgumentNullException("checkRead, checkWrite and checkError", S._("Arg_NotNull"));
				
				if (checkRead.Count == 0 && checkWrite.Count == 0 && checkError.Count == 0)
					throw new ArgumentNullException("checkRead, checkWrite and checkError", S._("Arg_NotNull"));	
					
				// Get Socket Handles from
				// checkRead, checkWrite, etc.
				IntPtr[] chkRead = new IntPtr [checkRead.Count];
				IntPtr[] chkWrite = new IntPtr [checkWrite.Count];
				IntPtr[] chkError = new IntPtr [checkError.Count];
				
				try
				{	
				for(int i = 0; i < checkRead.Count; ++i)
				{
					chkRead[i] = ((Socket)(checkRead[i])).Handle;    
				}
				
				for(int i = 0; i < checkWrite.Count; ++i)
				{
					chkWrite[i] = ((Socket)(checkWrite[i])).Handle;    
				}
				
				for(int i = 0; i < checkError.Count; ++i)
				{
					chkError[i] = ((Socket)(checkError[i])).Handle;    
				}
				}
				catch(Exception e)
				{
					throw new SocketException();
				}
										
			       	int r = SocketMethods.Select(chkRead, chkWrite,
	                        chkError, (long)microSeconds);
				if (r == -1)
				{
					switch(SocketMethods.GetErrno())
					{
						// Didn't want to use
						// FileNotFoundException
						// for invalid FileHandle
						case Errno.EBADF:
							throw new SocketException();
						case Errno.EINTR:
							throw new SocketException();
						case Errno.ENOMEM:
							throw new OutOfMemoryException();
					}
				}

	}

	public int Send(byte[] buffer, int size, SocketFlags socketFlags)		
			{
				return Send(buffer, 0, size, socketFlags);									
			}
	
	public int Send(byte[] buffer, SocketFlags socketFlags)
			{
				return Send(buffer, 0, buffer.Length, socketFlags);
			}
	
	public int Send(byte[] buffer)
			{
				return Send(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None);
			}
	
	public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags)
			{
				int sizevalue;

				if (disposed == 0)			
					throw new ObjectDisposedException(S._("Exception_Disposed"));									

				if (buffer == null)
					throw new ArgumentNullException("buffer", S._("Arg_NotNull"));
				
				if ((offset < 0) || (offset > buffer.Length))			
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));
				
				if ((size < 0) || (size > (buffer.Length - offset)))
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));											

				if (pending != 0) //This function blocks
					throw new InvalidOperationException(S._("Invalid_AsyncAndBlocking"));
					
				if ( (socketFlags && SocketFlags.Peek) != 0 ||
				     (socketFlags && SocketFlags.Partial) != 0)
				{
					throw new SocketException();				
				}

				if ((sizevalue = SocketMethods.Send(myhandle, buffer, offset, size, (int)socketFlags)) == -1)
					throw new SocketException(SocketMethods.GetErrno(), "Send");
				
				return sizevalue;							
				
			}
	
	public int SendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP)
			{
				if (disposed == 0)			
					throw new ObjectDisposedException(S._("Exception_Disposed"));									

				if (buffer == null)
					throw new ArgumentNullException("buffer", S._("Arg_NotNull"));

				if (remoteEP == null)
					throw new ArgumentNullException("remoteEP", S._("Arg_NotNull"));					

				if ((offset < 0) || (offset > buffer.Length))			
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));
				
				if ((size < 0) || (size > (buffer.Length - offset)))
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));											

				if (pending != 0) //This function blocks
					throw new InvalidOperationException(S._("Invalid_AsyncAndBlocking"));
					
				if ( (socketFlags && SocketFlags.Peek) != 0 ||
				     (socketFlags && SocketFlags.Partial) != 0)
				{
					throw new SocketException();				
				}

				//TODO: Change this into something better when more than
				//IPv4 gets supported
				if (remoteEP is IPEndPoint)
				{
					address = remoteEP.Address;
					port = remoteEP.Port;
				}				
				else
				{
					//TODO: translate the string
					throw new NotSupportedException("Something else than IPv4 is not yet supported");				
				}

				int sizevalue;
				if ((sizevalue = SocketMethods.SendTo(myhandle, buffer, offset, size, (int)socketFlags, address, port)) == -1)
					throw new SocketException(SocketMethods.GetErrno(), "SendTo");

				//The specs for property LocalEndPoint say SendTo sets
				//LocalEndPoint if not previously set
				//Let's set it to the 'any' setting
				mylocalendpoint = new IPEndPoint(IPAddress.Any, 0);	
				
				return sizevalue;		

			}
	
	
	public int SendTo(byte[] buffer, int size, SocketFlags socketFlags, EndPoint remoteEP)
			{
				return SendTo(buffer, 0, size, socketFlags, remoteEP);
			}
	
	public int SendTo(byte[] buffer, SocketFlags socketFlags, EndPoint remoteEP)
			{
				return SendTo(buffer, 0, buffer.Length, socketFlags, remoteEP);
			}	
	
	public int SendTo(byte[] buffer, EndPoint remoteEP)
			{
				return SendTo(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None, remoteEP);
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
				if (disposed == 0)			
					throw new ObjectDisposedException(S._("Exception_Disposed"));	

				if ( !(SocketMethods.Shutdown(myhandle, (int)how)))
					throw new SocketException();

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
	public int Available 
			{
				get //TODO: check errors
				{
					if (disposed == 0)			
						throw new ObjectDisposedException(S._("Exception_Disposed"));									
					try
					{
						// TODO: this is wrong
						//return ReceiveFrom(new byte[](), 0, int.MaxValue, SocketFlags.Peek, new EndPoint());
						return 0;
					}
					catch(Exception e)
					{
						throw new SocketException(S._("IO_SocketRead")); 
					}
			
				}
			}

	public bool Blocking 
			{ 
				get
				{
					if (disposed == 0)			
						throw new ObjectDisposedException(S._("Exception_Disposed"));				
					return blocking;
				}
				set
				{
					if (disposed == 0)			
						throw new ObjectDisposedException(S._("Exception_Disposed"));					
					blocking = value;
				}
			}

	public bool Connected 
			{ 
				get
				{
					return connected;
				}
			}
	
	public IntPtr Handle 
			{ 
				get
				{
					return myhandle; 
				}
			}
	
	public EndPoint LocalEndPoint 
			{ 
				get
				{	
					if (disposed == 0)			
						throw new ObjectDisposedException(S._("Exception_Disposed"));									
					try //ECMA specs say I have to check for an SocketException,
					{	//but it isn't really needed here, since we don't need to access the socket for that
						return mylocalendpoint;
					}
					catch(Exception e)
					{
						throw new SocketException();
					}				
				}
			}	
	
	public ProtocolType ProtocolType
			{
				get
				{
					return myprotocoltype;
				}
			}
	
	public EndPoint RemoteEndPoint
			{
				get
				{	
					if (disposed == 0)			
						throw new ObjectDisposedException(S._("Exception_Disposed"));									
					try //ECMA specs say I have to check for an SocketException,
					{	//but it isn't really needed here, since we don't need to access the socket for that  
						return mylocalendpoint; 
					}
					catch(Exception e)
					{
						throw new SocketException();
					}				
				}
			}
	public SocketType SocketType	
			{
				get
				{
					return mysockettype;
				}
			}

}; // class Socket

}; // namespace System.Net.Sockets



