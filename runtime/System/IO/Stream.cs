/*
 * Stream.cs - Implementation of the "System.IO.Stream" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System.IO
{

using System;
using System.Threading;

public abstract class Stream : MarshalByRefObject, IDisposable
{
	// Standard null stream.
	public static readonly Stream Null = new NullStream();

	// Constructor.
	protected Stream() {}

	// Destructor.
	~Stream()
			{
			#if ECMA_COMPAT
				Close();
			#else
				Dispose(false);
			#endif
			}

	// Begin an asynchronous read operation.
	public virtual IAsyncResult BeginRead
				(byte[] buffer, int offset, int count,
				 AsyncCallback callback, Object state)
			{
				ValidateBuffer(buffer, offset, count);
				// TODO
				return null;
			}

	// Wait for an asynchronous read operation to end.
	public virtual int EndRead(IAsyncResult asyncResult)
			{
				// TODO
				return 0;
			}

	// Begin an asychronous write operation.
	public virtual IAsyncResult BeginWrite
				(byte[] buffer, int offset, int count,
				 AsyncCallback callback, Object state)
			{
				ValidateBuffer(buffer, offset, count);
				// TODO
				return null;
			}

	// Wait for an asynchronous write operation to end.
	public virtual int EndWrite(IAsyncResult asyncResult)
			{
				// TODO
				return 0;
			}

	// Close the stream.
	public virtual void Close()
			{
			#if !ECMA_COMPAT
				Dispose(true);
			#endif
			}

	// Create a wait handle for asynchronous operations.
	protected virtual WaitHandle CreateWaitHandle()
			{
				// TODO
				return null;
			}

#if !ECMA_COMPAT
	// Dispose of this stream.
	protected virtual void Dispose(bool disposing)
			{
				// Nothing to do here: normally overridden.
			}
#endif

	// Flush the pending contents in this stream.
	public abstract void Flush();

	// Read data from this stream.
	public abstract int Read(byte[] buffer, int offset, int count);

	// Read a single byte from this stream.
	public virtual int ReadByte()
			{
				byte[] bytes = new byte[1];
				if(Read(bytes, 0, 1) == 1)
				{
					return (int)(bytes[0]);
				}
				else
				{
					return -1;
				}
			}

	// Seek to a new position within this stream.
	public abstract long Seek(long offset, SeekOrigin origin);

	// Set the length of this stream.
	public abstract void SetLength(long value);

	// Implement the IDisposable interface.
	void IDisposable.Dispose()
			{
			#if ECMA_COMPAT
				Close();
			#else
				Dispose(true);
			#endif
			}

	// Write a buffer of bytes to this stream.
	public abstract void Write(byte[] buffer, int offset, int count);

	// Write a single byte to this stream.
	public virtual void WriteByte(byte value)
			{
				byte[] bytes = new byte[1];
				bytes[0] = value;
				Write(bytes, 0, 1);
			}

	// Determine if it is possible to read from this stream.
	public abstract bool CanRead { get; }

	// Determine if it is possible to seek within this stream.
	public abstract bool CanSeek { get; }

	// Determine if it is possible to write to this stream.
	public abstract bool CanWrite { get; }

	// Get the length of this stream.
	public abstract long Length { get; }

	// Get the current position within the stream.
	public abstract long Position { get; set; }

	// Helper function for validating buffer arguments.
	protected internal void ValidateBuffer
				(byte[] buffer, int offset, int count)
			{
				if(buffer == null)
				{
					throw new ArgumentNullException("null");
				}
				else if(offset < 0 || offset > buffer.Length)
				{
					throw new ArgumentOutOfRangeException
						("offset", _("ArgRange_Array"));
				}
				else if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_Array"));
				}
				else if((buffer.Length - offset) < count)
				{
					throw new ArgumentException(_("Arg_InvalidArrayRange"));
				}
			}

	// Private class that implements null streams.
	private sealed class NullStream : Stream
	{
		// Constructor.
		public NullStream() {}

		// Stub out all stream functionality.
		public override void Flush() {}
		public override int Read(byte[] buffer, int offset, int count)
				{
					ValidateBuffer(buffer, offset, count);
					return 0;
				}
		public override int ReadByte() { return -1; }
		public override long Seek(long offset, SeekOrigin origin)
				{
					throw new NotSupportedException(_("IO_NotSupp_Seek"));
				}
		public override void SetLength(long value)
				{
					throw new NotSupportedException(_("IO_NotSupp_SetLength"));
				}
		public override void Write(byte[] buffer, int offset, int count)
				{
					ValidateBuffer(buffer, offset, count);
				}
		public override void WriteByte(byte value) {}
		public override bool CanRead { get { return true; } }
		public override bool CanSeek { get { return false; } }
		public override bool CanWrite { get { return true; } }
		public override long Length
				{
					get
					{
						throw new NotSupportedException(_("IO_NotSupp_Seek"));
					}
				}
		public override long Position
				{
					get
					{
						throw new NotSupportedException(_("IO_NotSupp_Seek"));
					}
					set
					{
						throw new NotSupportedException(_("IO_NotSupp_Seek"));
					}
				}

	}; // class NullStream

}; // class Stream

}; // namespace System.IO
