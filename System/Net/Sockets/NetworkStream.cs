/*
 * NetworkStream.cs - Implementation of the "System.Net.NetworkStreams.NetworkStream" class.
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

using System;
using System.IO;

public class NetworkStream : Stream
{
	private Socket mysocket;
	private bool readaccess;
	private bool writeaccess;
	private bool ownsocket;
	
	public NetworkStream(Socket socket)
			: this(socket, System.IO.FileAccess.ReadWrite, false)
			{
				//Nothing to do
			}
	
	public NetworkStream(Socket socket, bool ownsSocket)
			: this(socket, System.IO.FileAccess.ReadWrite, ownsSocket)					
			{
				//Nothing to do
			}
	
	public NetworkStream(Socket socket, FileAccess access)
			: this(socket, access, false)
			{
				//Nothing to do	
			}
			
	public NetworkStream(Socket socket, FileAccess access, bool ownsSocket)
			{
				if (socket == null) 
					throw new ArgumentNullException("socket", S._("Arg_NotNull"));
				if (socket.Blocking == false)
					throw new IOException(S._("IO_Socket"));
				if (socket.Connected == false)
					throw new IOException(S._("IO_Socket"));				
				if (socket.SocketType != SocketType.Stream)
					throw new IOException(S._("IO_Socket"));
			
				mysocket = socket;
				ownsocket = ownsSocket;					
				
				if (access == FileAccess.Read)
					{
						readaccess = true;
						writeaccess = false;
					}
				else if (access == FileAccess.Write)
					{
						readaccess = false;
						writeaccess = true;
					}
				else //Access == FileAccess.ReadWrite or another value
					{
						readaccess = true;
						writeaccess = true;
					}					
			}
			
	[TODO]		
	public override IAsyncResult BeginRead(byte[] buffer, int offset, int size, AsyncCallback callback, object state)
			{
				return null;
			}
	[TODO]	
	public override IAsyncResult BeginWrite(byte[] buffer, int offset, int size, AsyncCallback callback, object state)
			{
				return null;
			}
			
	public override void Close()
			{
				if (ownsocket == true)
					mysocket.Close();
				
			#if !ECMA_COMPAT
				Dispose(true);
			#endif
			}
	
#if !ECMA_COMPAT
	[TODO]
	protected override void Dispose(bool disposing)
			{
			
			}
#endif
			
	[TODO]		
	public override int EndRead(IAsyncResult asyncResult)		
			{
				return 0;
			}
			
	[TODO]
	public override void EndWrite(IAsyncResult asyncResult)
			{
			
			}
			
	[TODO]		
	~NetworkStream()
			{
			
			}
			
	public override void Flush()
			{
				//This feature is reserved for future use
			}
			
	public override int Read(byte[] buffer, int offset, int size)
			{
				if (mysocket == null)			
					throw new ObjectDisposedException(S._("Exception_Disposed"));				

				if (buffer == null)
					throw new ArgumentNullException("buffer", S._("Arg_NotNull"));							
					
				if ((offset < 0) || (offset > buffer.Length))			
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));
				
				if ((size < 0) || (size > (buffer.Length - offset)))
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));

				try
				{
					return mysocket.Receive(buffer, size, offset, SocketFlags.None);
				}					
				catch(Exception e)
				{
					throw new IOException(S._("IO_SocketRead"));		
				}
							
			}		
	
	public override long Seek(long offset, SeekOrigin origin)
			{
				throw new NotSupportedException(S._("NotSupp_DerivedStream"));
			}
	
	public override void SetLength(long value)
			{
				throw new NotSupportedException(S._("NotSupp_DerivedStream"));		
			}
			
	public override void Write(byte[] buffer, int offset, int size)
			{
				if (mysocket == null)
					throw new ObjectDisposedException(S._("Exception_Disposed"));									

				if (buffer == null)
					throw new ArgumentNullException("buffer", S._("Arg_NotNull"));

				if ((offset < 0) || (offset > buffer.Length))			
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));
				
				if ((size < 0) || (size > (buffer.Length - offset)))
					throw new ArgumentException("offset", S._("Arg_OutOfRange"));					

				try
				{
					mysocket.Send(buffer, offset, size, SocketFlags.None);
				}
				catch(Exception e)
				{					
					throw new IOException(S._("IO_SocketWrite"));
				}
			}		
	
	public override bool CanRead 
			{ 
				get
				{
					return readaccess;
				}
			}
	
	public override bool CanSeek
			{
				get
				{
					return false;
				}
			}
	
	public override bool CanWrite
			{
				get
				{
					return writeaccess;
				}
			}
	
	public virtual bool DataAvailable
			{
				get
				{
					if (mysocket == null)
						throw new ObjectDisposedException(S._("Exception_Disposed"));				
					
					if (mysocket.Available != 0)
						return true;
					else
						return false;		
				}
			}
			
	public override long Length
			{
				get
				{
					throw new NotSupportedException(S._("NotSupp_DerivedStream"));							
				}
			}
	
	public override long Position
			{
				get
				{
					throw new NotSupportedException(S._("NotSupp_DerivedStream"));
				}
				set
				{
					throw new NotSupportedException(S._("NotSupp_DerivedStream"));
				}				
			}	
}; // class NetworkStream

}; // namespace System.Net.NetworkStreams

