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

//TODO: Create EndPoint class
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
	private EndPoint mylocalendpoint = 0;
	private EndPoint myremoteendpoint = 0;
	private bool connected = false;
	private bool blocking = true; //This class standard blocks
	//The handle of this socket
	private int myhandle;
		
	//This represents the number of pending asyncronous calls
	private int pending;
	
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
				
				if (!(SocketMethods.Create(addressFamily, protocolType, socketType, myhandle)))
					throw new SocketException(SocketMethods.GetErrno(), "Socket");

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
						
				try
				{
					//Now create the socket
					Socket newsock = new Socket(myaddressfamily, mysockettype, myprotocoltype);
				}
				catch(Exception e)
				{
					throw new ArgumentException(_("IO_SocketCreating"));
				}
	
				newsock.blocking = this.blocking;
				newsock.mylocalendpoint = this.mylocalendpoint;
				newsock.connected = true;

				//TODO: Set the remote endpoint of the new socket	

				newsock.Handle = SocketMethods.Accept(newsock.myhandle);
				
				if (newsock.Handle == -1)
					throw new SocketException(SocketMethods.GetErrno(), "Bind");
				
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
	
	public void Bind(EndPoint localEP)
			{
				IPEndPoint ipend;
				
				//Remove when SocketMethods supports more than IPv4
				if (typeof(localEP) != System.Net.IPEndPoint)
					throw new NotSupportedException("localEP", "Something else than IPv4 is not yet supported");				

				ipend = (IPEndPoint) localEP;		

				if (disposed == 0)
					throw new ObjectDisposedException(_("Exception_Disposed"));
				
				if (localEP == 0)
					throw new ArgumentNullException("localEP", _("Arg_NotNull"));					
			
				if (!Security.CanUseFilemyhandle(myhandle))
					throw new SecurityException("myhandle", _("Exception_SecurityNotGranted"));			

				//Change when SocketMethods supports more than IPv4			
				if (!(SocketMethods.Bind(myhandle, ipend.AddressFamily, ipend.Address, ipend.Port)))
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
				if (typeof(remoteEP) != System.Net.IPEndPoint)
					throw new NotSupportedException("remoteEP", "Something else than IPv4 is not yet supported");				

				ipend = (IPEndPoint) remoteEP;		

				if (disposed == 0)
					throw new ObjectDisposedException(_("Exception_Disposed"));
				
				if (localEP == 0)
					throw new ArgumentNullException("remoteEP", _("Arg_NotNull"));					
				
				if (pending != 0) //This function blocks
					throw new InvalidOperationException(_("Invalid_AsyncAndBlocking"));				
						
				if (!Security.CanUseFilemyhandle(myhandle))
					throw new SecurityException("myhandle", _("Exception_SecurityNotGranted"));			

				//Change when SocketMethods supports more than IPv4					
				if (!(SocketMethods.Connect(myhandle, ipend.AddressFamily, ipend.Address, ipend.Port)))
					throw new SocketException(SocketMethods.GetErrno(), "Connect");		
														
				myremoteendpoint = remoteEP;				
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
				if (disposed == 0)			
					throw new ObjectDisposedException(_("Exception_Disposed"));				
				
				if (connected)
					throw new SocketException();
				
				if(SocketMethods.Listen(backlog) == -1)
					throw new SocketException(SocketMethods.GetErrno(), "Listen");
											
			}
	
	[TODO]
	public bool Poll(int microSeconds, SelectMode mode)
			{
				if (disposed == 0)			
					throw new ObjectDisposedException(_("Exception_Disposed"));		

				if (mode != SelectRead && mode != SelectWrite && mode != SelectError)				
					throw new NotSupportedException("mode", _("Arg_InvalidEnumValue"));
				
				try
				{
					//Call poll intercall		
				}
				catch(Exception e)
				{
					throw new SocketException();
				}
				
			}
	
			
	public int Receive(byte[] buffer, int size, SocketFlags socketFlags)
			{
				Receive(buffer, 0, buffer.Length, socketFlags);
			}
			
	public int Receive(byte[] buffer)
			{
				Receive(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None);
			}
	
	public int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags)
			{
				int sizevalue;

				if (disposed == 0)			
					throw new ObjectDisposedException(_("Exception_Disposed"));				

				if (buffer == 0)
					throw new ArgumentNullException("buffer", _("Arg_NotNull"));							
					
				if ((offset < 0) || (offset > buffer.Length))			
					throw new ArgumentException("offset", _("Arg_OutOfRange"));
				
				if ((size < 0) || (size > (buffer.Length - offset)))
					throw new ArgumentException("offset", _("Arg_OutOfRange"));
				
				if (pending != 0) //This function blocks
					throw new InvalidOperationException(_("Invalid_AsyncAndBlocking"));
							
				if ((socketFlags > 0x3) || (socketFlags < 0x0))
					throw new SocketException();
					
				if (!(socketFlags && SocketFlags.OutOfBound) && (mysockettype != SocketType.Stream))
					throw new SocketException();
				
				if (socketFlags && SocketFlags.Partial) 
					throw new SocketException();
				
				if (socketFlags && SocketFlags.DontRoute)
					throw new SocketException(); 

				if (mylocalendpoint == 0)
					throw new SocketException();
				
				if (!Security.CanUseFilemyhandle(myhandle))
					throw new SecurityException("myhandle", _("Exception_SecurityNotGranted"));					

				if ((sizevalue = SocketMethods.Receive(myhandle, buffer, offset, size, flags)) == -1)
					throw new SocketException(SocketMethods.GetErrno(), "Receive");
				
				return sizevalue;	

			}
			
	[TODO] //TODO: Change intercall call
	public int ReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				int sizevalue;

				if (disposed == 0)			
					throw new ObjectDisposedException(_("Exception_Disposed"));				

				if (buffer == 0)
					throw new ArgumentNullException("buffer", _("Arg_NotNull"));
				
				if (remoteEP == 0)
					throw new ArgumentNullException("buffer", _("Arg_NotNull"));									
					
				if ((offset < 0) || (offset > buffer.Length))			
					throw new ArgumentException("offset", _("Arg_OutOfRange"));
				
				if ((size < 0) || (size > (buffer.Length - offset)))
					throw new ArgumentException("offset", _("Arg_OutOfRange"));
				
				if (pending != 0) //This function blocks
					throw new InvalidOperationException(_("Invalid_AsyncAndBlocking"));
							
				if ((socketFlags > 0x3) || (socketFlags < 0x0))
					throw new SocketException();
					
				if (!(socketFlags && SocketFlags.OutOfBound) && (mysockettype != SocketType.Stream))
					throw new SocketException();
				
				if (socketFlags && SocketFlags.Partial) 
					throw new SocketException();
				
				if (socketFlags && SocketFlags.DontRoute)
					throw new SocketException(); 

				if (mylocalendpoint == 0)
					throw new SocketException();
					
				if (!Security.CanUseFilemyhandle(myhandle))
					throw new SecurityException("myhandle", _("Exception_SecurityNotGranted"));					

//				Change when SocketMethods intercall def is changed
//				if ((sizevalue = SocketMethods.ReceiveFrom(myhandle, buffer, offset, size, flags, remoteEP)) == -1)
//					throw new SocketException(SocketMethods.GetErrno(), "ReceiveFrom"));
				
				return sizevalue;																						
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
				if (checkRead == 0 && checkWrite == 0 && checkError == 0)
					throw new ArgumentNullException("checkRead, checkWrite and checkError", _("Arg_NotNull"));
				
				if (checkRead.Count == 0 && checkWrite.Count == 0 && checkError.Count == 0)
					throw new ArgumentNullException("checkRead, checkWrite and checkError", _("Arg_NotNull"));	
					
				try
				{
					//Call select intercall
				}	
				catch(Exception e)
				{
					throw new SocketException();
				}
			}

	public int Send(byte[] buffer, int size, SocketFlags socketFlags)		
			{
				Send(buffer, 0, size, socketFlags);									
			}
	
	public int Send(byte[] buffer, SocketFlags socketFlags)
			{
				Send(buffer, 0, buffer.Length, socketFlags);
			}
	
	public int Send(byte[] buffer)
			{
				Send(buffer, 0, buffer.Length, System.Net.Sockets.SocketFlags.None);
			}
	
	public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags)
			{
				int sizevalue;

				if (disposed == 0)			
					throw new ObjectDisposedException(_("Exception_Disposed"));									

				if (buffer == 0)
					throw new ArgumentNullException("buffer", _("Arg_NotNull"));
				
				if ((offset < 0) || (offset > buffer.Length))			
					throw new ArgumentException("offset", _("Arg_OutOfRange"));
				
				if ((size < 0) || (size > (buffer.Length - offset)))
					throw new ArgumentException("offset", _("Arg_OutOfRange"));											

				if (pending != 0) //This function blocks
					throw new InvalidOperationException(_("Invalid_AsyncAndBlocking"));
					
				if ( !(socketFlags || SocketFlags.Peek) || !(socketFlags || SocketFlags.Partial))
					throw new SocketException();				

				if ( !(SocketMethods.Send(buffer, offset, size, socketFlags)))
					throw new SocketException();

				if ((sizevalue = SocketMethods.Send(mthandle, buffer, offset, size, flags)) == -1)
					throw new SocketException(SocketMethods.GetErrno(), "Send");
				
				return sizevalue;							
				
			}
			
	public int SendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP)
			{
				if (disposed == 0)			
					throw new ObjectDisposedException(_("Exception_Disposed"));									

				if (buffer == 0)
					throw new ArgumentNullException("buffer", _("Arg_NotNull"));

				if (remoteEP == 0)
					throw new ArgumentNullException("remoteEP", _("Arg_NotNull"));					

				if ((offset < 0) || (offset > buffer.Length))			
					throw new ArgumentException("offset", _("Arg_OutOfRange"));
				
				if ((size < 0) || (size > (buffer.Length - offset)))
					throw new ArgumentException("offset", _("Arg_OutOfRange"));											

				if (pending != 0) //This function blocks
					throw new InvalidOperationException(_("Invalid_AsyncAndBlocking"));
					
				if ( !(socketFlags || SocketFlags.Peek) || !(socketFlags || SocketFlags.Partial))
					throw new SocketException();				

				if ( !(SocketMethods.SendTo(buffer, offset, size, socketFlags, remoteEP)))
					throw new SocketException();

//				Change when SocketMethods intercall def is changed
//				if ((sizevalue = SocketMethods.SendTo(myhandle, buffer, offset, size, flags, remoteEP)) == -1)
//					throw new SocketException(SocketMethods.GetErrno(), "SendTo");
				
				return sizevalue;		

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
				if (disposed == 0)			
					throw new ObjectDisposedException(_("Exception_Disposed"));	

				if ( !(SocketMethods.Shutdown(myhandle, how)))
					throw new SocketException();

			}
/*  //The compiler seems to have trouble with this inheritance, ignore it for now
	[TODO] 
	void IDisposable.Dispose()
			{
			
			} */
	
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
						throw new ObjectDisposedException(_("Exception_Disposed"));									
					try
					{
						return ReceiveFrom(new byte[](), 0, int.MaxValue, SocketFlags.Peek, new EndPoint());
					}
					catch(Exception e)
					{
						throw new SocketException("Available", _("IO_SocketRead")); 
					}
			
				}
			}

	public bool Blocking 
			{ 
				get
				{
					if (disposed == 0)			
						throw new ObjectDisposedException(_("Exception_Disposed"));				
					return blocking;
				}
				set
				{
					if (disposed == 0)			
						throw new ObjectDisposedException(_("Exception_Disposed"));					
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
						throw new ObjectDisposedException(_("Exception_Disposed"));									
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
						throw new ObjectDisposedException(_("Exception_Disposed"));									
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



