/*
 * BadImageFormatException.cs - Implementation of the
 *		"System.BadImageFormatException" class.
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

namespace System
{

public class BadImageFormatException : SystemException
{

	// Internal state.
	private String fileName;

	// Constructors.
	public BadImageFormatException()
		: base(_("Exception_BadImage")) {}
	public BadImageFormatException(String msg)
		: base(msg) {}
	public BadImageFormatException(String msg, Exception inner)
		: base(msg, inner) {}
	public BadImageFormatException(String msg, String fileName, Exception inner)
		: base(msg, inner) { this.fileName = fileName; }
	public BadImageFormatException(String msg, String fileName)
		: base(msg) { this.fileName = fileName; }

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

	// Properties.
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
							   		(_("Exception_Filename"), fileName);
					}
					else
					{
						return null;
					}
				}
			}

	// Get the default message to use for this exception type.
	protected internal override String MessageDefault
			{
				get
				{
					return _("Exception_BadImage");
				}
			}

}; // class BadImageFormatException

}; // namespace System
