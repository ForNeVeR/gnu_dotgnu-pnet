/*
 * socket-glue.cs - Glue between libc/socket and the C# system library.
 *
 * This file is part of the Portable.NET C library.
 * Copyright (C) 2004  Southern Storm Software, Pty Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace OpenSystem.C
{

using System;
using System.IO;
using System.Net.Sockets;
using System.Runtime.InteropServices;

[GlobalScope]
public class LibCSocket
{
	// The stream object that wraps up a socket.
	private class SocketStream : Stream, IFDOperations
	{
		// Internal state.
		private Socket socket;

		// Constructor.
		public SocketStream(Socket socket)
				{
					this.socket = socket;
				}

		// Get the socket that underlies this stream.
		public Socket Socket
				{
					get
					{
						return socket;
					}
				}

		// Implement the LibCFDOperations interface.
		public bool NonBlocking
				{
					get
					{
						return socket.Blocking;
					}
					set
					{
						socket.Blocking = value;
					}
				}
		public int NativeFd
				{
					get
					{
						return (int)(socket.Handle);
					}
				}
		public int SelectFd
				{
					get
					{
						return (int)(socket.Handle);
					}
				}

		// Implement pass throughs for "Stream" methods.
		public override void Close()
				{
					socket.Close();
				}
		public override void Flush()
				{
					// Nothing to do here.
				}
		public override int Read(byte[] buffer, int offset, int count)
				{
					return socket.Receive(buffer, offset, count,
										  SocketFlags.None);
				}
		public override int ReadByte()
				{
					byte[] buf = new byte [1];
					int len = socket.Receive(buf, 0, 1, SocketFlags.None);
					if(len > 0)
					{
						return (int)(buf[0]);
					}
					else
					{
						return -1;
					}
				}
		public override long Seek(long offset, SeekOrigin origin)
				{
					throw new NotSupportedException();
				}
		public override void SetLength(long value)
				{
					throw new NotSupportedException();
				}
		public override void Write(byte[] buffer, int offset, int count)
				{
					socket.Send(buffer, offset, count, SocketFlags.None);
				}
		public override void WriteByte(byte value)
				{
					byte[] buf = new byte [1];
					buf[0] = value;
					socket.Send(buf, 0, 1, SocketFlags.None);
				}
		public override bool CanRead
				{
					get
					{
						return true;
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
						return true;
					}
				}
		public override long Length
				{
					get
					{
						throw new NotSupportedException();
					}
				}
		public override long Position
				{
					get
					{
						throw new NotSupportedException();
					}
					set
					{
						throw new NotSupportedException();
					}
				}
	
	} // class SocketStream

	// Create a new socket.
	public static int __syscall_socket(int domain, int type)
			{
				AddressFamily family;
				SocketType socketType;
				ProtocolType protocol;

				// Convert the parametrs into their C# equivalents.
				if(domain == 2 /*AF_INET*/)
				{
					family = AddressFamily.InterNetwork;
				}
				else if(domain == 10 /*AF_INET6*/)
				{
					family = AddressFamily.InterNetworkV6;
				}
				else
				{
					return -22;		/* EINVAL */
				}
				if(type == 1 /*SOCK_STREAM*/)
				{
					socketType = SocketType.Stream;
					protocol = ProtocolType.Tcp;
				}
				else if(type == 2 /*SOCK_DGRAM*/)
				{
					socketType = SocketType.Dgram;
					protocol = ProtocolType.Udp;
				}
				else
				{
					return -22;		/* EINVAL */
				}

				// Create the socket.
				Socket socket;
				try
				{
					socket = new Socket(family, socketType, protocol);
				}
				catch(SocketException)
				{
					return -22;		/* EINVAL */
				}

				// Wrap the socket within a stream.
				Stream stream = new SocketStream(socket);

				// Associate the stream with a file descriptor.
				int fd = FileTable.AllocFD();
				if(fd == -1)
				{
					stream.Close();
				}
				FileTable.SetFileDescriptor(fd, stream);
				return fd;
			}

	// Get the socket object for a file descriptor.  Returns null
	// if an error occurred (which is written to "errno").
	public unsafe static Socket __syscall_get_socket(int fd, int *errno)
			{
				Stream stream = FileTable.GetStream(fd);
				if(stream == null)
				{
					*errno = 9;		/* EBADF */
					return null;
				}
				SocketStream sstream = (stream as SocketStream);
				if(sstream != null)
				{
					return sstream.Socket;
				}
				else
				{
					*errno = 88;	/* ENOTSOCK */
					return null;
				}
			}

} // class LibCSocket

} // namespace OpenSystem.C
