/*
 * FileLoadException.cs - Implementation of the
 *		"System.IO.FileLoadException" class.
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
using Platform;

public class FileLoadException : IOException
{
	// Internal state.
	private String fileName;

	// Constructors.
	public FileLoadException()
			: base(Errno.ENOENT,
				   Environment.GetResourceString("Exception_FileLoad"))
			{
				fileName = null;
			}
	public FileLoadException(String msg)
			: base(Errno.ENOENT, msg)
			{
				fileName = null;
			}
	public FileLoadException(String msg, String fileName)
			: base(Errno.ENOENT, msg)
			{
				this.fileName = fileName;
			}
	public FileLoadException(String msg, Exception inner)
			: base(Errno.ENOENT, msg, inner)
			{
				fileName = null;
			}
	public FileLoadException(String msg, String fileName, Exception inner)
			: base(Errno.ENOENT, msg, inner)
			{
				this.fileName = fileName;
			}

	// Get the message for this exception.  Because of "MessageDefault",
	// we don't actually need this.  However, we include it because
	// the ECMA standard expects this to be present.
	public override String Message
			{
				get
				{
					return base.Message;
				}
			}

	// Convert this exception into a string.  Because of "MessageExtra",
	// we don't actually need this.  However, we include it because the
	// ECMA standard expects this to be present.
	public override String ToString()
			{
				return base.ToString();
			}

	// Get the filename that was associated with this exception.
	public String FileName
			{
				get
				{
					return fileName;
				}
			}

	// Get the extra data to insert into "Exception::ToString()"'s result.
	protected internal override String MessageExtra
			{
				get
				{
					if(fileName != null)
					{
						return String.Format
							   		(Environment.GetResourceString
										("IO_FileName"), fileName);
					}
					else
					{
						return null;
					}
				}
			}

	// Get the default message for this exception type.
	protected internal override String MessageDefault
			{
				get
				{
					if(fileName != null)
					{
						return String.Format
							(Environment.GetResourceString
								("IO_FileLoad"), fileName);
					}
					else
					{
						return Environment.GetResourceString
							("Exception_FileLoad");
					}
				}
			}

}; // class FileLoadException

}; // namespace System.IO
