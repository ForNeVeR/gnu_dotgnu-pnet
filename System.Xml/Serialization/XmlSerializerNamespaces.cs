/*
 * XmlSerializerNamespaces.cs - Implementation of the
 *			"System.Xml.Serialization.XmlSerializerNamespaces" class.
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

public class XmlSerializerNamespaces
{
	// Internal state.
	private Hashtable table;

	// Constructors.
	public XmlSerializerNamespaces()
			{
				table = new Hashtable();
			}
	public XmlSerializerNamespaces(XmlQualifiedName[] namespaces)
			: this()
			{
				for(int i = 0, len = namespaces.Length; i < len; ++i)
				{
					XmlQualifiedName qname = namespaces[i];
					table[qname.Name] = qname;
				}
			}
	public XmlSerializerNamespaces(XmlSerializerNamespaces namespaces)
			: this(namespaces.ToArray())
			{
				// nothing to do here
			}


	// Get a count of the prefix/namespace pairs.
	public int Count
			{
				get { return table.Count; }
			}


	// Add a new prefix/namespace pair.
	public void Add(String prefix, String ns)
			{
				XmlQualifiedName qname = new XmlQualifiedName(prefix, ns);
				table[qname.Name] = qname;
			}

	// Create an array that contains the prefix/namespace pairs.
	public XmlQualifiedName[] ToArray()
			{
				XmlQualifiedName[] retval = new XmlQualifiedName[table.Count];
				table.Values.CopyTo(retval, 0);
				return retval;
			}

}; // class XmlSerializerNamespaces

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
