/*
 * MemoryStream.cs - Implementation of the "System.IO.MemoryStream" class.
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
 *
 * Contributed by Stephen Compall <rushing@sigecom.net>.
 * Contributions by Haran Shivanan <ch99057@che.iitm.ac.in>
 * Contributions by Thong Nguyen <tum@veridicus.com>
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

// Stephen says:
// I had a great deal of trouble with the problem of Position indexing,
// and had to rewrite portions of the class twice because of this
// particular ECMA standard, which is in some cases contradictory to the
// documentation for System.IO.Stream and NotSupportedException

// Also please note that, at various places, I have written some weird
// code to get state, instead of accessing the private variables
// directly. This is because most of this stuff is virtual, and
// unfortunately, the Properties are virtual too. So if someone finds it
// a quick solution to just override a property and expect the real
// methods to work with them, I want the class to be able to do so, to
// it's fullest potential. For example, if GetBuffer doesn't work,
// thanks to the child class, then that screws it up, but there's no way
// we can do anything about that in this file.

// The point of this diatribe is that you should also follow this standard.

namespace System.IO
{

using System;
using System.IO;

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

	// position is always indexed to bottomLimit; that is, when
	// bottomLimit is 5, "Position = 0" will access impl_buffer[5]
	private int position = 0;
	// a capacity variable is unnecessary

	// from StringBuilder
	// Thong says:  Made default to 0 to match MS & Mono.
	private const int defaultCapacity = 0;

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
		this.writable = writable; // scoping
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
			bottomLimit = index;
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
		// effectively call Flush(); this is the kind of silliness
		// I referred to in the note about child classes
		this.Flush();
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
			throw new UnauthorizedAccessException(_("Exception_HiddenBuffer"));
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
		// TODO: is this behavior correct?
		if (!CanRead)
			throw new NotSupportedException(_("IO_NotSupp_Read"));

		byte[] src_buffer = forceGetBuffer();

		if (count > Length - Position) // Copy will throw
			count = (int)(Length - Position); // fixed

		Array.Copy(src_buffer, (int)(Position + bottomLimit), buffer, offset, count);
		forcePositionChange(count);
		return count;
	}

	public override int ReadByte()
	{
		if (streamclosed)
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));

		if (!CanRead)
			throw new NotSupportedException(_("IO_NotSupp_Read"));

		if (Capacity == Position)
			return -1;
		else
		{
			int retval = forceGetBuffer()[Position + bottomLimit];
			forcePositionChange(1);
			return retval;
		}
	}


	// The stream should only reveal through the regular Stream methods
	// the range given to it by the constructor index&count, that is,
	// bottomLimit and topLimit. Haran, you are correct.
	public override long Seek(long offset, SeekOrigin loc)
	{
		long l = Length; // for `virtual' portability, see note at top
		if (streamclosed)
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));

		// ECMA says, throw ArgumentOutOfRange if offset > maximum stream length
		if (offset > l)
			throw new ArgumentOutOfRangeException("value",_("Arg_InvalidArrayIndex"));

		switch (loc)
		{
		case SeekOrigin.Begin:
			// We have already tested for (offset>stream length) above
			if (offset < 0) // attempt to seek before begin
				throw new IOException(_("Arg_InvalidArrayIndex"));

			forcePositionSet(offset);
			break;
		
		case SeekOrigin.Current:
			long oldpos = Position;
			if ( offset+oldpos > l || offset+oldpos < 0)
				throw new IOException(_("Arg_InvalidArrayIndex"));

			forcePositionChange(offset);
			break;

		case SeekOrigin.End:
			if (offset > 0 || offset < -l) // out of range
				throw new IOException(_("Arg_InvalidArrayIndex"));

			forcePositionSet(l+offset);
			break;

		default:
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		return Position;
	}

	// remember that all resizable MemoryStreams have bottomLimit=0
	public override void SetLength(long value)
	{
		if (!resizable)
			throw new NotSupportedException(_("IO_NotSupp_SetLength"));
		if (!CanWrite)
			throw new NotSupportedException(_("IO_NotSupp_Write"));
		if (value < 1 || value > Int32.MaxValue)
			throw new ArgumentOutOfRangeException("value");

		int newLength = (int)value;
		int oldLength = topLimit;
		growStream(newLength);
		if (newLength < position)
			position = newLength;
		if (newLength > oldLength)
		{
			for (int i = oldLength; i < newLength; ++i)
				impl_buffer[i] = 0;
		}
	}

	// ignore the normal limits this time, because it works when the stream
	// is closed, but there's no way to get the capacity/length publicly
	// in that case
	public virtual byte[] ToArray()
	{
		if (topLimit-bottomLimit == impl_buffer.Length) // can just clone
			return (byte[])impl_buffer.Clone();
		else
		{
			byte[] retarray = new byte[topLimit-bottomLimit];
			Array.Copy(impl_buffer, bottomLimit, retarray,
				0, topLimit-bottomLimit);
			return retarray;
		}
	}

	public override void Write(byte[] buffer, int offset, int count)
	{
		// Get all the exceptions out of the way
		if (buffer == null)
			throw new ArgumentNullException("buffer");
		if (!CanWrite)
			throw new NotSupportedException(_("IO_NotSupp_Write"));
		if ( offset<0 || count<0)
				throw new ArgumentOutOfRangeException("value", _("Arg_InvalidArrayIndex"));
		if (offset+count > buffer.Length)
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		if (streamclosed)
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));


		// Check if we need to expand the stream
		if (count > Length-Position)
		{
			// Is the stream resizable?
			if (!resizable)
				throw new NotSupportedException(_("IO_NotSupp_Write"));
			else
				growStream(Position+count);
		}

		// Copy buffer to impl_buffer using forceGetBuffer() and update
		// the position,
		Array.Copy(buffer,offset,forceGetBuffer(),(int)(Position+bottomLimit),count);
		forcePositionChange(count);
	}

	public override void WriteByte(byte value)
	{
		if (streamclosed)
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));
		if (!CanWrite)
			throw new NotSupportedException(_("IO_NotSupp_Write"));
		if (Length-Position < 1)
		{
			if (!resizable)
				throw new NotSupportedException(_("IO_NotSupp_Wrte"));
			else
				growStream(Position+1);
		}
		forceGetBuffer()[Position+bottomLimit] = value;
		forcePositionChange(1);
	}

	public virtual void WriteTo(Stream stream)
	{
		if (stream == null)
			throw new ArgumentNullException("buffer");
		if (streamclosed)
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));

		stream.Write(impl_buffer, bottomLimit, topLimit-bottomLimit);
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
			else if (resizable)
				return impl_buffer.Length;
			else
				return topLimit - bottomLimit;
		}
		set
		{
			if (streamclosed)
				throw new NotSupportedException(_("IO_StreamClosed"));

			// just ignore if smaller
			int old_capacity = Capacity;
			if (value > old_capacity) // bottom is always 0
			{
				if (!resizable)
					throw new NotSupportedException(_("IO_NotSupp_SetLength"));
				growStream(value);
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

	public override long Position
	{
		get
		{
			return position;
		}
		set
		{
			// ECMA behavior when CanSeek==false is undefined,
			// but Stream says it should do NotSupportedException.
			// If you change it, fix
			// forcePositionChange(int) to comply
			if (streamclosed)
				throw new ObjectDisposedException(null, _("IO_StreamClosed"));
			// little neat technicality: the check for CanSeek
			// must come before the range check, < ECMA
			if (!CanSeek)
				throw new NotSupportedException(_("IO_NotSupp_Seek"));
			if (value > Capacity || value < 0)
				throw new ArgumentOutOfRangeException("value", _("Arg_InvalidArrayIndex"));

			position = (int)value;
		}
	}

	// private methods

	// force position to change, either by virtual property or internally
	// specify increment to posn
	private void forcePositionChange(long increment)
	{
		forcePositionSet(Position + increment);
	}

	private void forcePositionSet(long newpos)
	{
		try
		{
			Position = newpos;
		}
		catch (NotSupportedException)
		{
			position = (int)newpos;
		}
		catch (ArgumentOutOfRangeException)
		{
			Position = Length;
		}
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

	// grow the stream efficiently, setting topLimit
	private void growStream(long minNewLength)
	{
		if (minNewLength > impl_buffer.Length)
		{
			int realNewLength = Math.Max(impl_buffer.Length, 1024);
			while (minNewLength > realNewLength)
				realNewLength = realNewLength * 2; // double the buffer size.
			byte[] new_buffer = new byte[(int)realNewLength];
			Array.Copy(impl_buffer, new_buffer, topLimit);
			impl_buffer = new_buffer;
		}
		topLimit = (int)minNewLength;
	}

} // class MemoryStream

} // namespace System.IO
