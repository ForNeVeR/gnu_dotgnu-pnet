/*
 * XmlSchemas.cs - Implementation of the
 *			"System.Xml.Serialization.XmlSchemas" class.
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
using System.Xml.Schema;

public class XmlSchemas : CollectionBase
{
	// Internal state.
	private Hashtable table;

	// Constructor.
	public XmlSchemas()
			: base()
			{
				table = new Hashtable();
			}


	// Get or set the schema at the given index.
	public XmlSchema this[int index]
			{
				get { return (XmlSchema)List[index]; }
				set { List[index] = value; }
			}

	// Get the schema with the given namespace.
	public XmlSchema this[String ns]
			{
				get
				{
					int i = IndexOf(ns);
					return (i == -1 ? null : ((XmlSchema)List[i]));
				}
			}

	// Add a schema to this collection, returning the index of insertion.
	public int Add(XmlSchema schema)
			{
				return List.Add(schema);
			}

	// Add the schemas to this collection.
	public void Add(XmlSchemas schemas)
			{
				foreach(XmlSchema schema in schemas) { Add(schema); }
			}

	// Add a schema to the lookup table.
	private void AddLookup(XmlSchema schema, int index)
			{
				// get the namespace of the schema
				String ns = schema.TargetNamespace;
				if(ns == null) { ns = String.Empty; }

				// give an error on duplicate namespaces
				if(table[ns] != null)
				{
					throw new InvalidOperationException
						("Xml_DuplicateSchemaNamespace");
				}

				// add the namespace to the lookup table
				table.Add(ns, index);
			}

	// Determine if this collection contains the given schema.
	public bool Contains(XmlSchema schema)
			{
				return table.Contains(schema.TargetNamespace);
			}

	// Copy the schemas in this collection to the given array.
	public void CopyTo(XmlSchema[] array, int index)
			{
				List.CopyTo(array, index);
			}

	// Find the schema with the given qualified name and type.???
	[TODO]
	public Object Find(XmlQualifiedName name, Type type)
			{
				// TODO
				throw new NotImplementedException("Find");
			}

	// Get the index of the given schema.
	public int IndexOf(XmlSchema schema)
			{
				Object o = table[schema.TargetNamespace];
				return ((o == null) ? -1 : (int)o);
			}

	// Get the index of the schema for the given namespace.
	private int IndexOf(String ns)
			{
				Object o = table[(ns == null ? String.Empty : ns)];
				return ((o == null) ? -1 : (int)o);
			}

	// Insert a schema at the given index.
	public void Insert(int index, XmlSchema schema)
			{
				List.Insert(index, schema);
			}

	[TODO]
	public static bool IsDataSet(XmlSchema schema)
			{
				// TODO
				throw new NotImplementedException("IsDataSet");
			}

	// Complete clearing this collection.
	protected override void OnClear()
			{
				table.Clear();
			}

	// Complete inserting an item into this collection.
	protected override void OnInsert(int index, Object value)
			{
				AddLookup((XmlSchema)value, index);
			}

	// Complete removing an item from this collection.
	protected override void OnRemove(int index, Object value)
			{
				RemoveLookup((XmlSchema)value);
			}

	// Complete setting an item in this collection.
	protected override void OnSet(int index, Object oldValue, Object newValue)
			{
				RemoveLookup((XmlSchema)oldValue);
				AddLookup((XmlSchema)newValue, index);
			}

	// Remove the given schema from this collection.
	public void Remove(XmlSchema schema)
			{
				List.RemoveAt(IndexOf(schema));
			}

	// Remove a schema from the lookup table.
	private void RemoveLookup(XmlSchema schema)
			{
				// get the namespace of the schema
				String ns = schema.TargetNamespace;
				if(ns == null) { ns = String.Empty; }

				// remove the namespace from the lookup table
				table.Remove(ns);
			}

}; // class XmlSchemas

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
