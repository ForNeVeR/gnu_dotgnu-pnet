/*
 * SoapAttributes.cs - Implementation of the
 *			"System.Xml.Serialization.SoapAttributes" class.
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

public class SoapAttributes
{
	// Internal state.
	private bool                   soapIgnore;
	private Object                 soapDefaultValue;
	private SoapAttributeAttribute soapAttribute;
	private SoapElementAttribute   soapElement;
	private SoapEnumAttribute      soapEnum;
	private SoapTypeAttribute      soapType;


	// Constructors.
	public SoapAttributes()
			{
				soapIgnore       = false;
				soapDefaultValue = DBNull.Value;
				soapAttribute    = null;
				soapElement      = null;
				soapEnum         = null;
				soapType         = null;
			}
	public SoapAttributes(ICustomAttributeProvider provider)
			: this()
			{
				Object[] atts;

				// first check if we've got an ignore attribute
				atts = provider.GetCustomAttributes
					(typeof(SoapIgnoreAttribute), false);
				if(atts.Length > 0)
				{
					// update ignore flag
					soapIgnore = true;

					// bail out now, since the rest aren't going to be used
					return;
				}

				// no ignore, so grab the rest of the attributes
				atts = provider.GetCustomAttributes(false);
				for(int i = 0, len = atts.Length; i < len; ++i)
				{
					Object o = atts[i];
				#if CONFIG_COMPONENT_MODEL
					if(o is DefaultValueAttribute)
					{
						soapDefaultValue = ((DefaultValueAttribute)o).Value;
					}
					else
				#endif
					if(o is SoapAttributeAttribute)
					{
						soapAttribute = (SoapAttributeAttribute)o;
					}
					else if(o is SoapElementAttribute)
					{
						soapElement = (SoapElementAttribute)o;
					}
					else if(o is SoapEnumAttribute)
					{
						soapEnum = (SoapEnumAttribute)o;
					}
					else if(o is SoapTypeAttribute)
					{
						soapType = (SoapTypeAttribute)o;
					}
				}
			}


	// Get or set the soap attribute.
	public SoapAttributeAttribute SoapAttribute
			{
				get { return soapAttribute; }
				set { soapAttribute = value; }
			}

	// Get or set the soap default value.
	public Object SoapDefaultValue
			{
				get { return soapDefaultValue; }
				set { soapDefaultValue = value; }
			}

	// Get or set the soap element.
	public SoapElementAttribute SoapElement
			{
				get { return soapElement; }
				set { soapElement = value; }
			}

	// Get or set the soap enum.
	public SoapEnumAttribute SoapEnum
			{
				get { return soapEnum; }
				set { soapEnum = value; }
			}

	// Get or set the soap ignore flag.
	public bool SoapIgnore
			{
				get { return soapIgnore; }
				set { soapIgnore = value; }
			}

	// Get or set the soap type.
	public SoapTypeAttribute SoapType
			{
				get { return soapType; }
				set { soapType = value; }
			}

}; // class SoapAttributes

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
