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
		: base(Environment.GetResourceString("Exception_BadImage")) {}
	public BadImageFormatException(String msg)
		: base(msg) {}
	public BadImageFormatException(String msg, Exception inner)
		: base(msg, inner) {}
	public BadImageFormatException(String msg, String fileName, Exception inner)
		: base(msg, inner) { this.fileName = fileName; }
	public BadImageFormatException(String msg, String fileName)
		: base(msg) { this.fileName = fileName; }

	// Convert this exception object into a string.
	public override String ToString()
			{
				String className = GetType().ToString();
				String result;
				result = className + ": " + Message;
				if(fileName != null && fileName.Length > 0)
				{
					result = result + Environment.NewLine +
						String.Format(Environment.GetResourceString
											("Exception_Filename"),
									  fileName);
				}
				if(InnerException != null)
				{
					result = result + " ---> " + InnerException.ToString();
				}
				return result;
			}

	// Properties.
	public String FileName
			{
				get
				{
					return fileName;
				}
			}
	public override String Message
			{
				get
				{
					return base.Message;
				}
			}

}; // class BadImageFormatException

}; // namespace System
