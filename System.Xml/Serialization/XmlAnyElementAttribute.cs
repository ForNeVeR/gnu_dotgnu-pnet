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

[TODO]
[AttributeUsage(AttributeTargets.Property |
                AttributeTargets.Field |
                AttributeTargets.Parameter |
                AttributeTargets.ReturnValue)]
public class XmlAnyElementAttribute : Attribute
{
	[TODO]
	public XmlAnyElementAttribute()
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlAnyElementAttribute(String name)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlAnyElementAttribute(String name, String ns)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public String Name
			{
				get
				{
					// TODO
					throw new NotImplementedException("Name");
				}
				set
				{
					// TODO
					throw new NotImplementedException("Name");
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

}; // class XmlAnyElementAttribute

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
