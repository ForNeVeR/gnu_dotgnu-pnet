/*
 * XmlException.cs - Implementation of the "System.Xml.XmlException" class.
 *
 * Copyright (C) 2002 Southern Storm Software, Pty Ltd.
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
 
namespace System.Xml
{

public class XmlException : SystemException
{
	// Internal state.
	private int lineNumber, linePosition;

	// Constructors.
	public XmlException()
		: base(S._("Xml_Default")) {}
	public XmlException(String message)
		: base(message) {}
	public XmlException(String message, Exception innerException)
		: base(message, innerException) {}
	public XmlException(String message, Exception innerException,
						int lineNumber, int linePosition)
		: base(message, innerException)
		{
			this.lineNumber = lineNumber;
			this.linePosition = linePosition;
		}
	
	// Get the default message to use for this exception type.
	public override String Message
			{
				get
				{
					String parentMsg = base.Message;
					if(parentMsg != null)
					{
						return parentMsg;
					}
					else
					{
						return S._("Xml_Default");
					}
				}
			}

	// Get the line number.
	public int LineNumber
			{
				get
				{
					return lineNumber;
				}
			}

	// Get the line position.
	public int LinePosition
			{
				get
				{
					return linePosition;
				}
			}

}; // class XmlException

}; // namespace System.Xml
