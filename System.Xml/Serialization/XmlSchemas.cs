/*
 * XmlSchemas.cs - Implementation of the
 *			"System.Xml.Serialization.XmlSchemas" class.
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
using System.Xml.Schema;

[TODO]
public class XmlSchemas : CollectionBase
{
	[TODO]
	public XmlSchemas()
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlSchema this[int index]
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
	public XmlSchema this[String ns]
			{
				get
				{
					// TODO
					throw new NotImplementedException("Item");
				}
			}

	[TODO]
	public int Add(XmlSchema schema)
			{
				// TODO
				throw new NotImplementedException("Add");
			}

	[TODO]
	public void Add(XmlSchemas schemas)
			{
				// TODO
				throw new NotImplementedException("Add");
			}

	[TODO]
	public bool Contains(XmlSchema schema)
			{
				// TODO
				throw new NotImplementedException("Contains");
			}

	[TODO]
	public void CopyTo(XmlSchema[] array, int index)
			{
				// TODO
				throw new NotImplementedException("CopyTo");
			}

	[TODO]
	public Object Find(XmlQualifiedName name, Type type)
			{
				// TODO
				throw new NotImplementedException("Find");
			}

	[TODO]
	public int IndexOf(XmlSchema schema)
			{
				// TODO
				throw new NotImplementedException("IndexOf");
			}

	[TODO]
	public void Insert(int index, XmlSchema schema)
			{
				// TODO
				throw new NotImplementedException("Insert");
			}

	[TODO]
	public static bool IsDataSet(XmlSchema schema)
			{
				// TODO
				throw new NotImplementedException("IsDataSet");
			}

	[TODO]
	protected override void OnClear()
			{
				// TODO
				throw new NotImplementedException("OnClear");
			}

	[TODO]
	protected override void OnInsert(int index, Object value)
			{
				// TODO
				throw new NotImplementedException("OnInsert");
			}

	[TODO]
	protected override void OnRemove(int index, Object value)
			{
				// TODO
				throw new NotImplementedException("OnRemove");
			}

	[TODO]
	protected override void OnSet(int index, Object oldValue, Object newValue)
			{
				// TODO
				throw new NotImplementedException("OnSet");
			}

	[TODO]
	public void Remove(XmlSchema schema)
			{
				// TODO
				throw new NotImplementedException("Remove");
			}

}; // class XmlSchemas

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
