/*
 * MemoryStream.cs - Implementation of the "System.IO.MemoryStream" class.
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
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

public class MemoryStream : Stream
{

	// state

	private bool streamclosed = false;
	private byte[] impl_buffer;
	private bool readable, seekable, writable, visibleBuffer;

	// from StringBuilder
	private const int defaultCapacity = 16;

	// constructors.

	[TODO]
	public MemoryStream()
	{
		impl_buffer = new char[defaultCapacity];
		readable = seekable = writable = visibleBuffer = true;
		// TODO: set up position vars
	}

	[TODO]
	public MemoryStream(int capacity)
	{
	}

	[TODO]
	public MemoryStream(byte[] buffer)
	{
	}

	[TODO]
	public MemoryStream(byte[] buffer, bool writable)
	{
	}

	[TODO]
	public MemoryStream(byte[] buffer, int index, int count)
	{
	}

	[TODO]
	public MemoryStream(byte[] buffer, int index, int count, bool writable)
	{
	}

	[TODO]
	public MemoryStream(byte[] buffer, int index, int count, bool writable, bool publiclyVisible)
	{
	}

	// methods.

	[TODO]
	public override void Close()
	{
	}

	public override void Flush()
	{
		// do nothing
	}

	[TODO]
	public virtual byte[] GetBuffer()
	{
		return null;
	}

	[TODO]
	public override int Read(byte[] buffer, int offset, int count)
	{
		return -1;
	}

	[TODO]
	public override int ReadByte()
	{
		return -1;
	}

	[TODO]
	public override long Seek(long offset, SeekOrigin loc)
	{
		return -1;
	}

	[TODO]
	public override void SetLength(long value)
	{
	}

	[TODO]
	public virtual byte[] ToArray()
	{
		return null;
	}

	[TODO]
	public override void Write(byte[] buffer, int offset, int count)
	{
	}

	[TODO]
	public override void WriteByte(byte value)
	{
	}

	[TODO]
	public virtual void WriteTo(Stream stream)
	{
	}

	// properties. beware, all can be overridden, so use whenever possible

	public override bool CanRead
	{
		get
		{
			return readable;
		}
	}

	public override bool CanSeek
	{
		get
		{
			return seekable;
		}
	}

	public override bool CanWrite
	{
		get
		{
			return writable;
		}
	}

	[TODO]
	public virtual int Capacity
	{
		get
		{
			return impl_buffer.Length;
		}
		set
		{
			if (value > impl_buffer.Length)
				;// TODO: resize
			// else, ignore
		}
	}

	[TODO]
	public override long Length
	{
		get
		{
			return -1;
		}
	}

	[TODO]
	public override long Position
	{
		get
		{
			return -1;
		}
		set
		{
		}
	}


} // class MemoryStream

} // namespace System.IO