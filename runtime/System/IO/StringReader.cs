/*
 * StringReader.cs - Implementation of the "System.IO.StringReader" class.
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
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

public class StringReader : TextReader
{

	// state.

	// the string to be read from
	private String readfrom;

	// index of the next character to be read
	private int position = 0;

	private bool streamclosed = false;

	// constructor

	public StringReader(String s)
	{
		if (s == null)
			throw new ArgumentNullException("s");
		else
			this.readfrom = s;
	}

	// methods

	public override void Close()
	{
		this.Dispose(true);
	}

	protected override void Dispose(bool disposing)
	{
		if (disposing)
		{
			this.readfrom = null;
			this.streamclosed = true;
		}
		base.Dispose(disposing);
	}

	public override int Peek()
	{
		if (this.streamclosed) // no String
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));

		else if (this.position < this.readfrom.Length) // there are chars left
			return readfrom[position];

		else // out of chars to read
			return -1;
	}

	public override int Read(char[] buffer, int index, int count)
	{
		if (this.streamclosed)
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));

		// CopyTo will check if buffer is null

		if (this.position == readfrom.Length) // no more chars, not necessary
			return 0;

		if (count > readfrom.Length - position) // count exceeds remaining chars in stream
			count = readfrom.Length - position;

		// CopyTo will do nothing if count-index == 0

		try
		{
			readfrom.CopyTo(position, buffer, index, count);
		}
		catch (ArgumentOutOfRangeException badRange)
		{
			// using CopyTo to avoid checking if buffer is null
			if (count > buffer.Length - index) // dest not big enough
				throw new ArgumentException(_("Arg_InvalidArrayRange"));
			else
				throw badRange;
		}
		return count;
	}

	public override int Read()
	{
		if (this.streamclosed) // no String
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));

		else if (this.position < this.readfrom.Length) // there are chars left
			return readfrom[position++];

		else // out of chars to read
			return -1;
	}

	public override String ReadLine()
	{
		if (this.streamclosed) // no String
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));
		if (this.readfrom.Length == this.position)
			return null;

		int newPosition;
		int eolDexWindows = this.readfrom.IndexOf("\r\n", position);
		int eolDex = this.readfrom.IndexOfAny(new char[]{'\n', '\r'}, position);
		if (eolDexWindows == eolDex && eolDexWindows != -1) // CRLF found, CR also found
			newPosition = eolDex + 2;
		else if (eolDex == -1) // end of line is end of string
			eolDex = newPosition = readfrom.Length;
		else // CR or LF found
			newPosition = eolDex + 1;

		String retval = readfrom.Substring(position, eolDex - position);
		position = newPosition;
		return retval;
	}

	public override String ReadToEnd()
	{
		if (this.streamclosed) // no String
			throw new ObjectDisposedException(null, _("IO_StreamClosed"));

		String retval = readfrom.Substring(position);
		position = readfrom.Length;
		return retval;
	}


} // class StringReader

} // namespace System.IO
