/*
 * SoapTypeAttribute.cs - Implementation of the
 *			"System.Runtime.Remoting.Metadata.SoapTypeAttribute" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace System.Runtime.Remoting.Metadata
{

#if CONFIG_REMOTING

[AttributeUsage(AttributeTargets.Class |
				AttributeTargets.Struct |
				AttributeTargets.Enum |
				AttributeTargets.Interface)]
public sealed class SoapTypeAttribute : SoapAttribute
{
	// Internal state.
	private SoapOption soapOptions;
	private String xmlElementName;
	private String xmlTypeName;
	private String xmlTypeNamespace;

	// Constructor.
	public SoapTypeAttribute() {}

	// Get or set the attribute's properties.
	public SoapOption SoapOptions
			{
				get
				{
					return soapOptions;
				}
				set
				{
					soapOptions = value;
				}
			}
	public override bool UseAttribute
			{
				get
				{
					return false;
				}
				set
				{
					throw new RemotingException
						(_("NotSupp_SetRemotingValue"));
				}
			}
	public String XmlElementName
			{
				get
				{
					// TODO: default element name handling
					return xmlElementName;
				}
				set
				{
					xmlElementName = value;
				}
			}
	public override String XmlNamespace
			{
				get
				{
					// TODO: default namespace handling
					return ProtXmlNamespace;
				}
				set
				{
					ProtXmlNamespace = value;
				}
			}
	public String XmlTypeName
			{
				get
				{
					// TODO: default type name handling
					return xmlTypeName;
				}
				set
				{
					xmlTypeName = value;
				}
			}
	public String XmlTypeNamespace
			{
				get
				{
					// TODO: default type namespace handling
					return xmlTypeNamespace;
				}
				set
				{
					xmlTypeNamespace = value;
				}
			}

}; // class SoapTypeAttribute

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Metadata
