/*
 * XmlAttributes.cs - Implementation of the
 *			"System.Xml.Serialization.XmlAttributes" class.
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
using System.Reflection;
using System.ComponentModel;

public class XmlAttributes
{
	// Internal state.
	private bool                         xmlIgnore;
	private bool                         xmlns;
	private Object                       xmlDefaultValue;
	private XmlAnyAttributeAttribute     xmlAnyAttribute;
	private XmlAnyElementAttributes      xmlAnyElements;
	private XmlArrayAttribute            xmlArray;
	private XmlArrayItemAttributes       xmlArrayItems;
	private XmlAttributeAttribute        xmlAttribute;
	private XmlChoiceIdentifierAttribute xmlChoiceIdentifier;
	private XmlElementAttributes         xmlElements;
	private XmlEnumAttribute             xmlEnum;
	private XmlRootAttribute             xmlRoot;
	private XmlTextAttribute             xmlText;
	private XmlTypeAttribute             xmlType;


	// Constructors.
	public XmlAttributes()
			{
				xmlIgnore           = false;
				xmlns               = false;
				xmlDefaultValue     = DBNull.Value;
				xmlAnyAttribute     = null;
				xmlAnyElements      = new XmlAnyElementAttributes();
				xmlArray            = null;
				xmlArrayItems       = new XmlArrayItemAttributes();
				xmlAttribute        = null;
				xmlChoiceIdentifier = null;
				xmlElements         = new XmlElementAttributes();
				xmlEnum             = null;
				xmlRoot             = null;
				xmlText             = null;
				xmlType             = null;
			}
	public XmlAttributes(ICustomAttributeProvider provider)
			: this()
			{
				Object[] atts;

				// first check if we've got an ignore attribute
				atts = provider.GetCustomAttributes
					(typeof(XmlIgnoreAttribute), false);
				if(atts.Length > 0)
				{
					// update ignore flag
					xmlIgnore = true;

					// bail out now, since the rest aren't going to be used
					return;
				}

				// no ignore, so grab the rest of the attributes
				atts = provider.GetCustomAttributes(false);
				for(int i = 0, len = atts.Length; i < len; ++i)
				{
					Object o = atts[i];
					if(o is XmlNamespaceDeclarationsAttribute)
					{
						xmlns = true;
					}
				#if CONFIG_COMPONENT_MODEL
					else if(o is DefaultValueAttribute)
					{
						xmlDefaultValue = ((DefaultValueAttribute)o).Value;
					}
				#endif
					else if(o is XmlAnyAttributeAttribute)
					{
						xmlAnyAttribute = (XmlAnyAttributeAttribute)o;
					}
					else if(o is XmlAnyElementAttribute)
					{
						xmlAnyElements.Add((XmlAnyElementAttribute)o);
					}
					else if(o is XmlArrayAttribute)
					{
						xmlArray = (XmlArrayAttribute)o;
					}
					else if(o is XmlArrayItemAttribute)
					{
						xmlArrayItems.Add((XmlArrayItemAttribute)o);
					}
					else if(o is XmlAttributeAttribute)
					{
						xmlAttribute = (XmlAttributeAttribute)o;
					}
					else if(o is XmlChoiceIdentifierAttribute)
					{
						xmlChoiceIdentifier = (XmlChoiceIdentifierAttribute)o;
					}
					else if(o is XmlElementAttribute)
					{
						xmlElements.Add((XmlElementAttribute)o);
					}
					else if(o is XmlEnumAttribute)
					{
						xmlEnum = (XmlEnumAttribute)o;
					}
					else if(o is XmlRootAttribute)
					{
						xmlRoot = (XmlRootAttribute)o;
					}
					else if(o is XmlTextAttribute)
					{
						xmlText = (XmlTextAttribute)o;
					}
					else if(o is XmlTypeAttribute)
					{
						xmlType = (XmlTypeAttribute)o;
					}
				}
			}


	// Get or set the xml any attribute.
	public XmlAnyAttributeAttribute XmlAnyAttribute
			{
				get { return xmlAnyAttribute; }
				set { xmlAnyAttribute = value; }
			}

	// Get the xml any elements.
	public XmlAnyElementAttributes XmlAnyElements
			{
				get { return xmlAnyElements; }
			}

	// Get or set the xml array.
	public XmlArrayAttribute XmlArray
			{
				get { return xmlArray; }
				set { xmlArray = value; }
			}

	// Get the xml array items.
	public XmlArrayItemAttributes XmlArrayItems
			{
				get { return xmlArrayItems; }
			}

	// Get or set the xml attribute.
	public XmlAttributeAttribute XmlAttribute
			{
				get { return xmlAttribute; }
				set { xmlAttribute = value; }
			}

	// Get the xml choice identifier.
	public XmlChoiceIdentifierAttribute XmlChoiceIdentifier
			{
				get { return xmlChoiceIdentifier; }
			}

	// Get or set the xml default value.
	public Object XmlDefaultValue
			{
				get { return xmlDefaultValue; }
				set { xmlDefaultValue = value; }
			}

	// Get the xml elements.
	public XmlElementAttributes XmlElements
			{
				get { return xmlElements; }
			}

	// Get or set the xml enum.
	public XmlEnumAttribute XmlEnum
			{
				get { return xmlEnum; }
				set { xmlEnum = value; }
			}

	// Get or set the xml ignore flag.
	public bool XmlIgnore
			{
				get { return xmlIgnore; }
				set { xmlIgnore = value; }
			}

	// Get or set the xml namespace flag.
	public bool Xmlns
			{
				get { return xmlns; }
				set { xmlns = value; }
			}

	// Get or set the xml root.
	public XmlRootAttribute XmlRoot
			{
				get { return xmlRoot; }
				set { xmlRoot = value; }
			}

	// Get or set the xml text.
	public XmlTextAttribute XmlText
			{
				get { return xmlText; }
				set { xmlText = value; }
			}

	// Get or set the xml type.
	public XmlTypeAttribute XmlType
			{
				get { return xmlType; }
				set { xmlType = value; }
			}

}; // class XmlAttributes

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
