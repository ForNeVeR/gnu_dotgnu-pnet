/*
 * XmlAnyElementAttribute.cs - Implementation of the
 *			"System.Xml.Serialization.XmlAnyElementAttribute" class.
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

[AttributeUsage(AttributeTargets.Property |
                AttributeTargets.Field |
                AttributeTargets.Parameter |
                AttributeTargets.ReturnValue)]
public class XmlAnyElementAttribute : Attribute
{
	// Internal state.
	private String name;
	private String nspace;

	// Constructors.
	public XmlAnyElementAttribute()
			: base()
			{
				this.name = "";
				this.nspace = null;
			}
	public XmlAnyElementAttribute(String name)
			: base()
			{
				if (name == null) { name = ""; }
				this.name = name;
				this.nspace = null;
			}
	public XmlAnyElementAttribute(String name, String nspace)
			: base()
			{
				if (name == null) { name = ""; }
				this.name = name;
				this.nspace = nspace;
			}

	// Properties.
	public String Name
			{
				get { return name; }
				set
				{
					if (value == null) { value = ""; }
					name = value;
				}
			}
	public String Namespace
			{
				get { return nspace; }
				set { nspace = value; }
			}

}; // class XmlAnyElementAttribute

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
