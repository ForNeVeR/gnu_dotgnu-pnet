/*
 * XmlTypeAttribute.cs - Implementation of the
 *			"System.Xml.Serialization.XmlTypeAttribute" class.
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

[TODO]
[AttributeUsage(AttributeTargets.Class |
                AttributeTargets.Struct |
                AttributeTargets.Enum |
                AttributeTargets.Interface)]
public class XmlTypeAttribute : Attribute
{
	[TODO]
	public XmlTypeAttribute()
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlTypeAttribute(String typeName)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public bool IncludeInSchema
			{
				get
				{
					// TODO
					throw new NotImplementedException("IncludeInSchema");
				}
				set
				{
					// TODO
					throw new NotImplementedException("IncludeInSchema");
				}
			}

	[TODO]
	public String Namespace
			{
				get
				{
					// TODO
					throw new NotImplementedException("Namespace");
				}
				set
				{
					// TODO
					throw new NotImplementedException("Namespace");
				}
			}

	[TODO]
	public String TypeName
			{
				get
				{
					// TODO
					throw new NotImplementedException("TypeName");
				}
				set
				{
					// TODO
					throw new NotImplementedException("TypeName");
				}
			}

}; // class XmlTypeAttribute

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
