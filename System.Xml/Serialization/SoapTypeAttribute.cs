/*
 * SoapTypeAttribute.cs - Implementation of the
 *			"System.Xml.Serialization.SoapTypeAttribute" class.
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

[AttributeUsage(AttributeTargets.Class |
                AttributeTargets.Struct |
                AttributeTargets.Enum |
                AttributeTargets.Interface)]
public class SoapTypeAttribute : Attribute
{
	// Internal state.
	private bool includeInSchema;
	private String nspace;
	private String typeName;

	// Constructors.
	public SoapTypeAttribute()
			: base()
			{
				this.includeInSchema = true;
				this.nspace = null;
				this.typeName = "";
			}
	public SoapTypeAttribute(String typeName)
			: base()
			{
				if (typeName == null) { typeName = ""; }
				this.includeInSchema = true;
				this.nspace = null;
				this.typeName = typeName;
			}
	public SoapTypeAttribute(String typeName, String ns)
			: base()
			{
				if (typeName == null) { typeName = ""; }
				this.includeInSchema = true;
				this.nspace = ns;
				this.typeName = typeName;
			}

	// Properties.
	public bool IncludeInSchema
			{
				get { return includeInSchema; }
				set { includeInSchema = value; }
			}
	public String Namespace
			{
				get { return nspace; }
				set { nspace = value; }
			}
	public String TypeName
			{
				get { return typeName; }
				set
				{
					if (value == null) { value = ""; }
					typeName = value;
				}
			}

}; // class SoapTypeAttribute

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
