/*
 * XmlNodeType.cs - Implementation of the "System.XmlNodeType" class.
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

public enum XmlNodeType
{
	Attribute = 2,
	CDATA = 4,
	Comment = 8,
	Document = 9,
	DocumentFragment = 11,
	DocumentType = 10,
	Element = 1,
	EndElement = 15,
	EndEntity = 16,
	Entity = 6,
	EntityReference = 5,
	None = 0,
	Notation = 12,
	ProcessingInstruction = 7,
	SignificantWhitespace = 14,
	Text = 3,
	Whitespace = 13,
	XmlDeclaration = 17

}; //enum XmlNodeType

}; //namespace System.Xml