/*
 * XmlException.cs - Implementation of the "System.XmlException" class.
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

[TODO]
public class XmlException : SystemException
{
	//TODO: The line number stuff

	// Constructors.
	public XmlException()
		: base(_("Xml_Default")) {}
	public XmlException(String msg)
		: base(msg) {}
	public XmlException(String msg, Exception inner)
		: base(msg, inner) {}
	
	// Get the default message to use for this exception type.
	protected internal override String MessageDefault
			{
				get
				{
					return _("Xml_Default");
				}
			}

}; //class XmlException

}; //namespace System.Xml