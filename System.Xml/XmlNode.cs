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
using System.Collections;

//Quadruple linked list used for storing an XML tree
internal class XmlNode
{
	public XmlNode Parent = null; //Pointer to the parent node, NULL if no parent
	public XmlNode Prev = null; //Pointer to the previous node, NULL if no previous
	public XmlNode Next = null; //Pointer to the next node, NULL if no next
	public ArrayList Children = null; //Pointer to the child nodes, NULL if no children

	public XmlNodeType Type; //The type of this node
	public String Text; //The text of this node, 
						//for example the name of an element node
	public int Depth = -1; //The Depth of the current node, 0 is the top layer,
						   //1 the layer under top, etc. -1 Is error.
	
	
	public XmlNode(XmlNode prev, XmlNode up, XmlNodeType type, String text, int depth)
	{
		if (prev != null)
		{
			prev.Next = this;
			this.Prev = prev;
		}
		
		if (up != null)
		{
			this.Parent = up;
			up.Children = up.Children.Add(this);
		}
		
		this.Type = type;
		this.Text = text;
		this.Depth = depth;
	}
	
}; //class XmlNode

}; //namespace System.Xml

