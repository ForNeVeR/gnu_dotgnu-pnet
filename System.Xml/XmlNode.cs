/*
 * XmlNode.cs - Implementation of the "System.XmlNode" class.
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

using System;

//Quadruple linked list used for storing an XML tree
internal class XmlNode
{
	public XmlNode parent; //Pointer to the parent node, NULL if no parent
	public XmlNode prev; //Pointer to the previous node, NULL if no previous
	public XmlNode next; //Pointer to the next node, NULL if no next
	public XmlNode children; //Pointer to the first child node, NULL if no children

	public XmlNodeType type; //The type of this node
	public String text; //The text of this node, 
				 	//for example the name of an element node

}; //class XmlNode

}; //namespace System.Xml

