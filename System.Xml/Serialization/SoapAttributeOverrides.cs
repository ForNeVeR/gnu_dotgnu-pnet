/*
 * SoapAttributeOverrides.cs - Implementation of the
 *			"System.Xml.Serialization.SoapAttributeOverrides" class.
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

public class SoapAttributeOverrides
{
	// Internal state.
	private Hashtable table;
	private XmlNameTable nt;


	// Constructor.
	public SoapAttributeOverrides()
			{
				table = new Hashtable();
				nt = new NameTable();
			}


	// Get the attributes for the given type.
	public SoapAttributes this[Type type]
			{
				get { return this[type, String.Empty]; }
			}

	// Get the attributes for the given type's member.
	public SoapAttributes this[Type type, String member]
			{
				get { return (SoapAttributes)table[GetKey(type, member)]; }
			}

	// Add the given attributes for the given type.
	public void Add(Type type, SoapAttributes attributes)
			{
				Add(type, String.Empty, attributes);
			}

	// Add the given attributes for the given type and member.
	public void Add(Type type, String member, SoapAttributes attributes)
			{
				TypeMember key = GetKey(type, member);
				if(table[key] != null)
				{
					throw new InvalidOperationException
						(String.Format(S._("Xml_MultipleOverrides"), key));
				}
				table.Add(key, attributes);
			}

	// Get a key for the given type and member.
	private TypeMember GetKey(Type type, String member)
			{
				if(member == null)
				{
					throw new ArgumentNullException("member");
				}
				return new TypeMember(type, nt.Add(member));
			}

}; // class SoapAttributeOverrides

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
