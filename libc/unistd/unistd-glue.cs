/*
 * unistd-glue.cs - Glue between unistd and the C# system library.
 *
 * This file is part of the Portable.NET C library.
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

using System;
using System.IO;
using System.Runtime.InteropServices;
using OpenSystem.C;

__module
{

	// Read data from a file descriptor.
	public static int __syscall_read(int fd, IntPtr buf, uint count)
			{
				// Get the stream and validate it.
				byte[] buffer;
				Stream stream = FileTable.GetStreamAndBuffer(fd, out buffer);
				if(stream == null)
				{
					return -9;		/* EBADF */
				}
				else if(count == 0)
				{
					return 0;
				}
				else if(buf == IntPtr.Zero)
				{
					return -14;		/* EFAULT */
				}

				// Read data from the stream.  We lock down the buffer
				// to prevent other threads from accessing the stream
				// until we have finished with it.
				int result = 0;
				int templen;
				try
				{
					lock(buffer)
					{
						while(count > 0)
						{
							templen = buffer.Length;
							if(((uint)templen) > count)
							{
								templen = (int)count;
							}
							templen = stream.Read(buffer, 0, templen);
							if(templen > 0)
							{
								Marshal.Copy(buffer, 0, buf, templen);
								buf = new IntPtr(buf.ToInt64() + templen);
								result += templen;
								count -= (uint)templen;
							}
							else
							{
								break;
							}
						}
					}
				}
				catch(IOException)
				{
					return -5;		/* EIO */
				}
				catch(ObjectDisposedException)
				{
					return -9;		/* EBADF */
				}
				catch(NotSupportedException)
				{
					return -22;		/* EINVAL */
				}
				return result;
			}

	// Read data from a file descriptor at a file offset.
	public static int __syscall_pread(int fd, IntPtr buf,
									  uint count, long offset)
			{
				// Get the stream and validate it.
				byte[] buffer;
				Stream stream = FileTable.GetStreamAndBuffer(fd, out buffer);
				if(stream == null)
				{
					return -9;		/* EBADF */
				}
				else if(count == 0)
				{
					return 0;
				}
				else if(buf == IntPtr.Zero)
				{
					return -14;		/* EFAULT */
				}

				// Read data from the stream.  We lock down the buffer
				// to prevent other threads from accessing the stream
				// until we have finished with it.
				int result = 0;
				int templen;
				long position;
				try
				{
					lock(buffer)
					{
						try
						{
							position = stream.Position;
							stream.Position = offset;
						}
						catch(NotSupportedException)
						{
							return -29;		/* ESPIPE */
						}
						catch(EndOfStreamException)
						{
							return -22;		/* EINVAL */
						}
						while(count > 0)
						{
							templen = buffer.Length;
							if(((uint)templen) > count)
							{
								templen = (int)count;
							}
							templen = stream.Read(buffer, 0, templen);
							if(templen > 0)
							{
								Marshal.Copy(buffer, 0, buf, templen);
								buf = new IntPtr(buf.ToInt64() + templen);
								result += templen;
								count -= (uint)templen;
							}
							else
							{
								break;
							}
						}
						stream.Position = position;
					}
				}
				catch(IOException)
				{
					return -5;		/* EIO */
				}
				catch(ObjectDisposedException)
				{
					return -9;		/* EBADF */
				}
				catch(NotSupportedException)
				{
					return -22;		/* EINVAL */
				}
				return result;
			}

	// Write data to a file descriptor.
	public static int __syscall_write(int fd, IntPtr buf, uint count)
			{
				// Get the stream and validate it.
				byte[] buffer;
				Stream stream = FileTable.GetStreamAndBuffer(fd, out buffer);
				if(stream == null)
				{
					return -9;		/* EBADF */
				}
				else if(count == 0)
				{
					return 0;
				}
				else if(buf == IntPtr.Zero)
				{
					return -14;		/* EFAULT */
				}

				// Write data to the stream.  We lock down the buffer
				// to prevent other threads from accessing the stream
				// until we have finished with it.
				int result = 0;
				int templen;
				try
				{
					lock(buffer)
					{
						while(count > 0)
						{
							templen = buffer.Length;
							if(((uint)templen) > count)
							{
								templen = (int)count;
							}
							Marshal.Copy(buf, buffer, 0, templen);
							stream.Write(buffer, 0, templen);
							buf = new IntPtr(buf.ToInt64() + templen);
							result += templen;
							count -= (uint)templen;
						}
					}
				}
				catch(IOException)
				{
					return -5;		/* EIO */
				}
				catch(ObjectDisposedException)
				{
					return -9;		/* EBADF */
				}
				catch(NotSupportedException)
				{
					return -22;		/* EINVAL */
				}
				return result;
			}

	// Write data to a file descriptor at a file offset.
	public static int __syscall_pwrite(int fd, IntPtr buf,
									   uint count, long offset)
			{
				// Get the stream and validate it.
				byte[] buffer;
				Stream stream = FileTable.GetStreamAndBuffer(fd, out buffer);
				if(stream == null)
				{
					return -9;		/* EBADF */
				}
				else if(count == 0)
				{
					return 0;
				}
				else if(buf == IntPtr.Zero)
				{
					return -14;		/* EFAULT */
				}

				// Write data to the stream.  We lock down the buffer
				// to prevent other threads from accessing the stream
				// until we have finished with it.
				int result = 0;
				int templen;
				long position;
				try
				{
					lock(buffer)
					{
						try
						{
							position = stream.Position;
							stream.Position = offset;
						}
						catch(NotSupportedException)
						{
							return -29;		/* ESPIPE */
						}
						catch(EndOfStreamException)
						{
							return -22;		/* EINVAL */
						}
						while(count > 0)
						{
							templen = buffer.Length;
							if(((uint)templen) > count)
							{
								templen = (int)count;
							}
							Marshal.Copy(buf, buffer, 0, templen);
							stream.Write(buffer, 0, templen);
							buf = new IntPtr(buf.ToInt64() + templen);
							result += templen;
							count -= (uint)templen;
						}
						stream.Position = position;
					}
				}
				catch(IOException)
				{
					return -5;		/* EIO */
				}
				catch(ObjectDisposedException)
				{
					return -9;		/* EBADF */
				}
				catch(NotSupportedException)
				{
					return -22;		/* EINVAL */
				}
				return result;
			}

	// Seek to a new position on a file descriptor.
	public static long __syscall_lseek(int fd, long offset, int whence)
			{
				// Get the stream and validate it.
				byte[] buffer;
				Stream stream = FileTable.GetStreamAndBuffer(fd, out buffer);
				if(stream == null)
				{
					return -9;		/* EBADF */
				}
				else if(!stream.CanSeek)
				{
					return -29;		/* ESPIPE */
				}

				// Validate the "whence" argument.
				if(whence < 0 || whence > 2)
				{
					return -22;		/* EINVAL */
				}

				// Seek to the new position.  We lock down the buffer
				// to synchronize descriptor accesses with pread/pwrite.
				try
				{
					lock(buffer)
					{
						return stream.Seek(offset, (SeekOrigin)whence);
					}
				}
				catch(NotSupportedException)
				{
					return -29;		/* ESPIPE */
				}
				catch(ObjectDisposedException)
				{
					return -9;		/* EBADF */
				}
				catch(EndOfStreamException)
				{
					return -22;		/* EINVAL */
				}
			}

} // __module
