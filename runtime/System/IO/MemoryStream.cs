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

// TODO:
// some longs returned by posn, length, etc. will have to be cast

namespace System.IO
{

using System;

public class MemoryStream : Stream
{

	// state

	private bool streamclosed = false;
	private byte[] impl_buffer;

	// these are used because if indices and a publicly visible buffer are
	// provided, it must not move outside these limits
	private int bottomLimit, topLimit;
	// readable and seekable are always true
	private bool writable, resizable, visibleBuffer;

	private int position = 0;
	// a capacity variable is unnecessary

	// from StringBuilder
	private const int defaultCapacity = 16;

	// constructors.
	// ECMA doesn't specify what the starting position should be, so I
	// assume it's 0
	// TODO: check this assumption against another MemoryStream

	public MemoryStream() : this(defaultCapacity)
	{
	}

	public MemoryStream(int capacity)
	{
		if (capacity < defaultCapacity)
			capacity = defaultCapacity;

		impl_buffer = new byte[capacity];
		writable = resizable = visibleBuffer = true;
	}

	public MemoryStream(byte[] buffer) : this(buffer, true)
	{
	}

	public MemoryStream(byte[] buffer, bool writable)
	{
		if (buffer == null)
			throw new ArgumentNullException("buffer");

		impl_buffer = (byte[]) buffer.Clone(); // protect from intruders
		writable = true;
		resizable = visibleBuffer = false;
		bottomLimit = 0;
		topLimit = buffer.Length;
	}

	public MemoryStream(byte[] buffer, int index, int count)
		: this(buffer, index, count, true, false)
	{
	}

	public MemoryStream(byte[] buffer, int index, int count, bool writable)
		: this(buffer, index, count, writable, false)
	{
	}

	public MemoryStream(byte[] buffer, int index, int count, bool writable, bool publiclyVisible)
	{
		if (buffer == null)
			throw new ArgumentNullException("buffer");
		if (index < 0)
			throw new ArgumentOutOfRangeException("index", _("Arg_InvalidArrayIndex"));
		if (count < 0)
			throw new ArgumentOutOfRangeException("count", _("Arg_InvalidArrayIndex"));
		if (count > buffer.Length - index)
			throw new ArgumentException(_("Arg_InvalidArrayRange"));

		if (publiclyVisible) // can use buffer passed as internal buffer
		{
			impl_buffer = buffer;
			bottomLimit = position = index;
			topLimit = index + count;
		}
		else // must hide
		{
			impl_buffer = new byte[count];
			Array.Copy(buffer, index, impl_buffer, 0, count);
			bottomLimit = 0;
			topLimit = count;
		}
		visibleBuffer = publiclyVisible;
		this.writable = writable; // scoping
		resizable = false;
	}

	// methods.

	public override void Close()
	{
		// effectively call Flush()
		this.Flush();
		impl_buffer = null; // release resources per ECMA Stream
		streamclosed = true;
		base.Close();
	}

	public override void Flush()
	{
		// do nothing
	}

	public virtual byte[] GetBuffer()
	{
		if (!visibleBuffer)
			throw new UnauthorizedAccessException();
		return impl_buffer;
	}

	public override int Read(byte[] buffer, int offset, int count)
	{
		if (streamclosed)
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));
		// all other exceptions handled by Copy

		// MemoryStream doesn't say what to do when can't read,
		// but Stream says use NotSupportedException.
		// however, it should say throw InvalidOperationException
		// so I am just returning 0, because 0 chars were read :)
		// TODO: is this behavior correct?
		if (!CanRead)
			return 0;

		byte[] src_buffer = forceGetBuffer();

		if (count > topLimit - Position) // Copy will throw
			count = (int)(topLimit - Position); // fixed

		Array.Copy(impl_buffer, (int)Position, buffer, offset, count);
		forcePositionChange(count);
		return count;
	}

	public override int ReadByte()
	{
		if (streamclosed)
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));

		if (!CanRead || topLimit == Position)
			return -1;
		else
		{
			int retval = forceGetBuffer()[Position];
			forcePositionChange(1);
			return retval;
		}
	}

	[TODO]
	public override long Seek(long offset, SeekOrigin loc)
	{
		switch (loc)
		{
		case SeekOrigin.Begin:
			break;
		case SeekOrigin.Current:
			break;
		case SeekOrigin.End:
			break;
		}
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
	// assuming that closed stream means all Can* are false, but no throw

	public override bool CanRead
	{
		get
		{
			// never changes
			return !streamclosed;
		}
	}

	public override bool CanSeek
	{
		get
		{
			// never changes
			return !streamclosed;
		}
	}

	public override bool CanWrite
	{
		get
		{
			return writable && !streamclosed;
		}
	}

	public virtual int Capacity
	{
		get
		{
			if (streamclosed)
				return 0;
			else
				return topLimit - bottomLimit;
		}
		set
		{
			if (streamclosed)
				throw new NotSupportedException(_("IO_StreamClosed"));

			// just ignore if smaller
			if (resizable && value > topLimit) // bottom is always 0
			{
				byte[] new_impl_buffer = new byte[value];
				Array.Copy(impl_buffer, new_impl_buffer, topLimit);
				impl_buffer = new_impl_buffer;
				topLimit = value;
			}
		}
	}

	public override long Length
	{
		get
		{
			if (streamclosed)
				throw new ObjectDisposedException(null, _("IO_StreamClosed"));
			return topLimit - bottomLimit;
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
			// ECMA behavior when CanSeek==false is undefined,
			// but Stream says it should do NotSupportedException.
			// If you change it, fix
			// forcePositionChange(int) to comply
			if (streamclosed)
				throw new ObjectDisposedException(null, _("IO_StreamClosed"));
			if (!CanSeek)
				return;
			if (value > topLimit || value < bottomLimit)
				throw new ArgumentOutOfRangeException("value", _("Arg_InvalidArrayIndex"));

			position = (int)value;
		}
	}

	// private methods

	// force position to change, either by virtual property or internally
	// specify increment to posn
	private void forcePositionChange(int increment)
	{
		int posn = (int)(Position + increment);
		Position = posn;
		if (Position != posn) // weird but quite possible
			position = posn;
	}

	// use this in case a child has its own buffer
	private byte[] forceGetBuffer()
	{
		try // for derived classes with their own buffers
		{
			return GetBuffer();
		}
		catch (UnauthorizedAccessException) // they can override themselves
		{
			return this.impl_buffer;
		}
	}

} // class MemoryStream

} // namespace System.IO
