/*
 * XmlElementAttributes.cs - Implementation of the
 *			"System.Xml.Serialization.XmlElementAttributes" class.
 *
 * Copyright (C) 2003, 2004  Free Software Foundation, Inc.
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

namespace System.Xml.Serialization
{

#if !ECMA_COMPAT

using System;
using System.Xml;
using System.Collections;

public class XmlElementAttributes : CollectionBase
{
	// Constructor.
	public XmlElementAttributes()
			: base()
			{
				// nothing to do here.
			}


	// Get or set the attribute at the given index.
	public XmlElementAttribute this[int index]
			{
				get { return (XmlElementAttribute)List[index]; }
				set { List[index] = value; }
			}


	// Add an attribute to this collection.
	public int Add(XmlElementAttribute attribute)
			{
				return List.Add(attribute);
			}

	// Determine if this collection contains the given attribute.
	public bool Contains(XmlElementAttribute attribute)
			{
				return List.Contains(attribute);
			}

	// Copy the attributes in this collection to the given array.
	public void CopyTo(XmlElementAttribute[] array, int index)
			{
				List.CopyTo(array, index);
			}

	// Get the index of the given attribute.
	public int IndexOf(XmlElementAttribute attribute)
			{
				return List.IndexOf(attribute);
			}

	// Insert an attribute at the given index.
	public void Insert(int index, XmlElementAttribute attribute)
			{
				List.Insert(index, attribute);
			}

	// Remove the given attribute from this collection.
	public void Remove(XmlElementAttribute attribute)
			{
				List.Remove(attribute);
			}

}; // class XmlElementAttributes

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
