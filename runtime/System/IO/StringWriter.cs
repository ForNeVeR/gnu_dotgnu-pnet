/*
 * StringWriter.cs - Implementation of the "System.IO.StringWriter" class.
 *
 * Copyright (C) 2002 Free Software Foundation, Inc.
 *
 * Contributed by Stephen Compall <rushing@earthling.net>
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
 *
 */

namespace System.IO
{

using System;
using System.Globalization;
using System.Text;

public class StringWriter : TextWriter, IDisposable
{

	// state

	// the System.Text.StringBuilder to work with (like java.lang.StringBuffer).
 	private StringBuilder buffer;
	// closing is very simple...just look at this!
	private bool streamclosed = false;

	// Constructors.
	public StringWriter()
	{
		// TODO: consider using the StringBuilder(int) constructor
		// instead, because the default size for that is 16 :(
		this.buffer = new StringBuilder();
	}

	public StringWriter(IFormatProvider formatProvider) : base(formatProvider)
	{
		// hmmmm...gonna let TextWriter handle this one
		this.buffer = new StringBuilder();
	}

	public StringWriter(StringBuilder sb)
	{
		if (sb == null) throw new ArgumentNullException("sb");
		this.buffer = sb;
	}

	public StringWriter(StringBuilder sb, IFormatProvider formatProvider) :
		base(formatProvider)
	{
		if (sb == null) throw new ArgumentNullException("sb");
		this.buffer = sb;
	}

	// methods

	public override void Close()
	{
		this.streamclosed = true;
		// still keep the this.buffer, though...norm for streams to save :)
		// although TextWriter.Close(bool) wants you to get rid of it
	}

	protected override void Dispose(bool disposing)
	{
		// however, if you don't need the object anymore (see
		// System.IDisposable), then it can gc the StringBuilder.
		if (disposing)
		{
			this.buffer = null;
			this.streamclosed = true;
		}
		// safe for call multiple times

		// I don't know if it automatically chains these, but just in
		// case base.Dispose actually does something...
		base.Dispose(disposing);
	}

	public virtual StringBuilder GetStringBuilder()
	{
		return this.buffer;
	}

	public override String ToString()
	{
		return this.buffer.ToString();
	}

	public override void Write(String value)
	{
		if (this.streamclosed) throw new ObjectDisposedException(null, _("IO_StreamClosed"));
		this.buffer.Append(value);
	}

	public override void Write(char value)
	{
		if (this.streamclosed) throw new ObjectDisposedException(null, _("IO_StreamClosed"));
		this.buffer.Append(value);
	}

	public override void Write(char[] buffer, int index, int count)
	{
		if (this.streamclosed) throw new ObjectDisposedException(null, _("IO_StreamClosed"));

		// StringBuilder puts another requirement on this one, but not us!
		if (buffer == null) throw new ArgumentNullException("buffer");

		try
		{
			this.buffer.Append(buffer, index, count);
		}
		catch (ArgumentOutOfRangeException aoore)
		// the definition combines the cases for StringBuilder, and
		// differentiates for StringWriter. Oh well.
		{
			if (index < 0 || count < 0)
				// kind of improper, but faster
				throw aoore;
			else
				throw new ArgumentException(_("ArgRange_Array"));
		}

	}

	// Properties.

	public override Encoding Encoding
	{
		get 
		{
			// Since presumably, StringBuilder is implemented
			// internally as a char[], this being the most efficient
			// method, and programs relying on performance would
			// certainly see this Stream as very efficient,
			return new UnicodeEncoding();
		}
	}

} // StringWriter

} // namespace
