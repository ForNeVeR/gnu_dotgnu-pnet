/*
 * StdWriter.cs - Implementation of the "System.IO.StdWriter" class.
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
using System.Globalization;
using System.Text;
using Platform;

internal sealed class StdWriter : TextWriter
{

	// Local state.
	private int fd;

	// Constructors.
	public StdWriter(int fd) : base()
			{
				this.fd = fd;
			}

	// Destructor.
	~StdWriter()
			{
				if(fd != -1)
				{
					Stdio.StdClose(fd);
					fd = -1;
				}
			}

	// Dispose this text writer.
	public override void Dispose(bool disposing)
			{
				if(fd != -1)
				{
					Stdio.StdClose(fd);
					fd = -1;
				}
			}

	// Flush the contents of the text writer.
	public override void Flush()
			{
				if(fd != -1)
				{
					Stdio.StdFlush(fd);
				}
			}

	// Write primitive values to this text writer.
	public override void Write(char value)
			{
				if(fd != -1)
				{
					Stdio.StdWrite(fd, value);
				}
			}
	public override void Write(char[] value, int index, int count)
			{
				if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				if(index < 0 || index > value.Length)
				{
					throw new ArgumentOutOfRangeException
						("index", _("ArgRange_Array"));
				}
				else if(count < 0 || (value.Length - index) < count)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_Array"));
				}
				if(fd != -1)
				{
					Stdio.StdWrite(fd, value, index, count);
				}
			}
	public override void Write(String value)
			{
				if(fd != -1 && value != null)
				{
					Stdio.StdWrite(fd, value);
				}
			}

	// Get the encoding in use by this text writer.
	public override System.Text.Encoding Encoding
			{
				get
				{
					return System.Text.Encoding.Default;
				}
			}

}; // class StdWriter

}; // namespace System.IO
