/*
 * XmlArrayItemAttributes.cs - Implementation of the
 *			"System.Xml.Serialization.XmlArrayItemAttributes" class.
 *
 * Copyright (C) 2003  Free Software Foundation, Inc.
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

[TODO]
public class XmlArrayItemAttributes : CollectionBase
{
	[TODO]
	public XmlArrayItemAttributes()
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlArrayItemAttribute this[int index]
			{
				get
				{
					// TODO
					throw new NotImplementedException("Item");
				}
				set
				{
					// TODO
					throw new NotImplementedException("Item");
				}
			}

	[TODO]
	public int Add(XmlArrayItemAttribute attribute)
			{
				// TODO
				throw new NotImplementedException("Add");
			}

	[TODO]
	public bool Contains(XmlArrayItemAttribute attribute)
			{
				// TODO
				throw new NotImplementedException("Contains");
			}

	[TODO]
	public void CopyTo(XmlArrayItemAttribute[] array, int index)
			{
				// TODO
				throw new NotImplementedException("CopyTo");
			}

	[TODO]
	public int IndexOf(XmlArrayItemAttribute attribute)
			{
				// TODO
				throw new NotImplementedException("IndexOf");
			}

	[TODO]
	public void Insert(int index, XmlArrayItemAttribute attribute)
			{
				// TODO
				throw new NotImplementedException("Insert");
			}

	[TODO]
	public void Remove(XmlArrayItemAttribute attribute)
			{
				// TODO
				throw new NotImplementedException("Remove");
			}

}; // class XmlArrayItemAttributes

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
