/*
 * XmlSerializationWriter.cs - Implementation of the
 *			"System.Xml.Serialization.XmlSerializationWriter" class.
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
using System.Collections;

[TODO]
public abstract class XmlSerializationWriter
{
	[TODO]
	protected XmlSerializationWriter()
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	protected ArrayList Namespaces
			{
				get
				{
					// TODO
					throw new NotImplementedException("Namespaces");
				}
				set
				{
					// TODO
					throw new NotImplementedException("Namespaces");
				}
			}

	[TODO]
	protected XmlWriter Writer
			{
				get
				{
					// TODO
					throw new NotImplementedException("Writer");
				}
				set
				{
					// TODO
					throw new NotImplementedException("Writer");
				}
			}

	[TODO]
	protected void AddWriteCallback
				(Type type, String typeName, String typeNs,
				 XmlSerializationWriteCallback callback)
			{
				// TODO
				throw new NotImplementedException("AddWriteCallback");
			}

	[TODO]
	protected Exception CreateChoiceIdentifierValueException
				(String value, String identifier, String name, String ns)
			{
				// TODO
				throw new NotImplementedException("CreateChoiceIdentifierValueException");
			}

	[TODO]
	protected Exception CreateInvalidChoiceIdentifierValueException
				(String type, String identifier)
			{
				// TODO
				throw new NotImplementedException("CreateInvalidChoiceIdentifierValueException");
			}

	[TODO]
	protected Exception CreateMismatchChoiceException
				(String value, String elementName, String enumValue)
			{
				// TODO
				throw new NotImplementedException("CreateMismatchChoiceException");
			}

	[TODO]
	protected Exception CreateUnknownAnyElementException
				(String name, String ns)
			{
				// TODO
				throw new NotImplementedException("CreateUnknownAnyElementException");
			}

	[TODO]
	protected Exception CreateUnknownTypeException
				(Object o)
			{
				// TODO
				throw new NotImplementedException("CreateUnknownTypeException");
			}

	[TODO]
	protected Exception CreateUnknownTypeException
				(Type type)
			{
				// TODO
				throw new NotImplementedException("CreateUnknownTypeException");
			}

	[TODO]
	protected static byte[] FromByteArrayBase64
				(byte[] value)
			{
				// TODO
				throw new NotImplementedException("FromByteArrayBase64");
			}

	[TODO]
	protected static String FromByteArrayHex
				(byte[] value)
			{
				// TODO
				throw new NotImplementedException("FromByteArrayHex");
			}

	[TODO]
	protected static String FromChar
				(char value)
			{
				// TODO
				throw new NotImplementedException("FromChar");
			}

	[TODO]
	protected static String FromDate
				(DateTime value)
			{
				// TODO
				throw new NotImplementedException("FromDate");
			}

	[TODO]
	protected static String FromDateTime
				(DateTime value)
			{
				// TODO
				throw new NotImplementedException("FromDateTime");
			}

	[TODO]
	protected static String FromEnum
				(long value, String[] values, long[] ids)
			{
				// TODO
				throw new NotImplementedException("FromEnum");
			}

	[TODO]
	protected static String FromTime
				(DateTime value)
			{
				// TODO
				throw new NotImplementedException("FromTime");
			}

	[TODO]
	protected static String FromXmlName
				(String name)
			{
				// TODO
				throw new NotImplementedException("FromXmlName");
			}

	[TODO]
	protected static String FromXmlNCName
				(String ncName)
			{
				// TODO
				throw new NotImplementedException("FromXmlNCName");
			}

	[TODO]
	protected static String FromXmlNmToken
				(String nmToken)
			{
				// TODO
				throw new NotImplementedException("FromXmlNmToken");
			}

	[TODO]
	protected static String FromXmlNmTokens
				(String nmTokens)
			{
				// TODO
				throw new NotImplementedException("FromXmlNmTokens");
			}

	[TODO]
	protected String FromXmlQualifiedName
				(XmlQualifiedName xmlQualifiedName)
			{
				// TODO
				throw new NotImplementedException("FromXmlQualifiedName");
			}

	protected abstract void InitCallbacks();

	[TODO]
	protected void TopLevelElement()
			{
				// TODO
				throw new NotImplementedException("TopLevelElement");
			}

	[TODO]
	protected void WriteAttribute
				(String localName, byte[] value)
			{
				// TODO
				throw new NotImplementedException("WriteAttribute");
			}

	[TODO]
	protected void WriteAttribute
				(String localName, String value)
			{
				// TODO
				throw new NotImplementedException("WriteAttribute");
			}

	[TODO]
	protected void WriteAttribute
				(String localName, String ns, byte[] value)
			{
				// TODO
				throw new NotImplementedException("WriteAttribute");
			}

	[TODO]
	protected void WriteAttribute
				(String localName, String ns, String value)
			{
				// TODO
				throw new NotImplementedException("WriteAttribute");
			}

	[TODO]
	protected void WriteAttribute
				(String prefix, String localName, String ns, String value)
			{
				// TODO
				throw new NotImplementedException("WriteAttribute");
			}

	[TODO]
	protected void WriteElementEncoded
				(XmlNode node, String name, String ns,
				 bool isNullable, bool any)
			{
				// TODO
				throw new NotImplementedException("WriteElementEncoded");
			}

	[TODO]
	protected void WriteElementLiteral
				(XmlNode node, String name, String ns,
				 bool isNullable, bool any)
			{
				// TODO
				throw new NotImplementedException("WriteElementLiteral");
			}

	[TODO]
	protected void WriteElementQualifiedName
				(String localName, XmlQualifiedName value)
			{
				// TODO
				throw new NotImplementedException("WriteElementQualifiedName");
			}

	[TODO]
	protected void WriteElementQualifiedName
				(String localName, String ns, XmlQualifiedName value)
			{
				// TODO
				throw new NotImplementedException("WriteElementQualifiedName");
			}

	[TODO]
	protected void WriteElementQualifiedName
				(String localName, XmlQualifiedName value,
				 XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteElementQualifiedName");
			}

	[TODO]
	protected void WriteElementQualifiedName
				(String localName, String ns, XmlQualifiedName value,
				 XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteElementQualifiedName");
			}

	[TODO]
	protected void WriteElementString
				(String localName, String value)
			{
				// TODO
				throw new NotImplementedException("WriteElementString");
			}

	[TODO]
	protected void WriteElementString
				(String localName, String ns, String value)
			{
				// TODO
				throw new NotImplementedException("WriteElementString");
			}

	[TODO]
	protected void WriteElementString
				(String localName, String value, XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteElementString");
			}

	[TODO]
	protected void WriteElementString
				(String localName, String ns, String value,
				 XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteElementString");
			}

	[TODO]
	protected void WriteElementStringRaw
				(String localName, byte[] value)
			{
				// TODO
				throw new NotImplementedException("WriteElementStringRaw");
			}

	[TODO]
	protected void WriteElementStringRaw
				(String localName, String value)
			{
				// TODO
				throw new NotImplementedException("WriteElementStringRaw");
			}

	[TODO]
	protected void WriteElementStringRaw
				(String localName, byte[] value, XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteElementStringRaw");
			}

	[TODO]
	protected void WriteElementStringRaw
				(String localName, String ns, byte[] value)
			{
				// TODO
				throw new NotImplementedException("WriteElementStringRaw");
			}

	[TODO]
	protected void WriteElementStringRaw
				(String localName, String ns, String value)
			{
				// TODO
				throw new NotImplementedException("WriteElementStringRaw");
			}

	[TODO]
	protected void WriteElementStringRaw
				(String localName, String value, XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteElementStringRaw");
			}

	[TODO]
	protected void WriteElementStringRaw
				(String localName, String ns, byte[] value,
				 XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteElementStringRaw");
			}

	[TODO]
	protected void WriteElementStringRaw
				(String localName, String ns, String value,
				 XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteElementStringRaw");
			}

	[TODO]
	protected void WriteEmptyTag
				(String name)
			{
				// TODO
				throw new NotImplementedException("WriteEmptyTag");
			}

	[TODO]
	protected void WriteEmptyTag
				(String name, String ns)
			{
				// TODO
				throw new NotImplementedException("WriteEmptyTag");
			}

	[TODO]
	protected void WriteEndElement()
			{
				// TODO
				throw new NotImplementedException("WriteEndElement");
			}

	[TODO]
	protected void WriteEndElement
				(Object o)
			{
				// TODO
				throw new NotImplementedException("WriteEndElement");
			}

	[TODO]
	protected void WriteId
				(Object o)
			{
				// TODO
				throw new NotImplementedException("WriteId");
			}

	[TODO]
	protected void WriteNamespaceDeclarations
				(XmlSerializerNamespaces xmlns)
			{
				// TODO
				throw new NotImplementedException("WriteNamespaceDeclarations");
			}

	[TODO]
	protected void WriteNullableQualifiedNameEncoded
				(String name, String ns, XmlQualifiedName value,
				 XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteNullableQualifiedNameEncoded");
			}

	[TODO]
	protected void WriteNullableQualifiedNameLiteral
				(String name, String ns, XmlQualifiedName value)
			{
				// TODO
				throw new NotImplementedException("WriteNullableQualifiedNameLiteral");
			}

	[TODO]
	protected void WriteNullableStringEncoded
				(String name, String ns, String value,
				 XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteNullableStringEncodedRaw");
			}

	[TODO]
	protected void WriteNullableStringEncodedRaw
				(String name, String ns, byte[] value,
				 XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteNullableStringEncodedRaw");
			}

	[TODO]
	protected void WriteNullableStringEncodedRaw
				(String name, String ns, String value,
				 XmlQualifiedName xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteNullableStringEncodedRaw");
			}

	[TODO]
	protected void WriteNullableStringLiteral
				(String name, String ns, String value)
			{
				// TODO
				throw new NotImplementedException("WriteNullableStringLiteral");
			}

	[TODO]
	protected void WriteNullableStringLiteralRaw
				(String name, String ns, byte[] value)
			{
				// TODO
				throw new NotImplementedException("WriteNullableStringLiteralRaw");
			}

	[TODO]
	protected void WriteNullableStringLiteralRaw
				(String name, String ns, String value)
			{
				// TODO
				throw new NotImplementedException("WriteNullableStringLiteralRaw");
			}

	[TODO]
	protected void WriteNullTagEncoded
				(String name)
			{
				// TODO
				throw new NotImplementedException("WriteNullTagEncoded");
			}

	[TODO]
	protected void WriteNullTagEncoded
				(String name, String ns)
			{
				// TODO
				throw new NotImplementedException("WriteNullTagEncoded");
			}

	[TODO]
	protected void WriteNullTagLiteral
				(String name)
			{
				// TODO
				throw new NotImplementedException("WriteNullTagLiteral");
			}

	[TODO]
	protected void WriteNullTagLiteral
				(String name, String ns)
			{
				// TODO
				throw new NotImplementedException("WriteNullTagLiteral");
			}

	[TODO]
	protected void WritePotentiallyReferencingElement
				(String n, String ns, Object o)
			{
				// TODO
				throw new NotImplementedException("WritePotentiallyReferencingElement");
			}

	[TODO]
	protected void WritePotentiallyReferencingElement
				(String n, String ns, Object o, Type ambientType)
			{
				// TODO
				throw new NotImplementedException("WritePotentiallyReferencingElement");
			}

	[TODO]
	protected void WritePotentiallyReferencingElement
				(String n, String ns, Object o, Type ambientType,
				 bool suppressReference)
			{
				// TODO
				throw new NotImplementedException("WritePotentiallyReferencingElement");
			}

	[TODO]
	protected void WritePotentiallyReferencingElement
				(String n, String ns, Object o, Type ambientType,
				 bool suppressReference, bool isNullable)
			{
				// TODO
				throw new NotImplementedException("WritePotentiallyReferencingElement");
			}

	[TODO]
	protected void WriteReferencedElements()
			{
				// TODO
				throw new NotImplementedException("WriteReferencedElements");
			}

	[TODO]
	protected void WriteReferencingElement
				(String n, String ns, Object o)
			{
				// TODO
				throw new NotImplementedException("WriteReferencingElement");
			}

	[TODO]
	protected void WriteReferencingElement
				(String n, String ns, Object o, bool isNullable)
			{
				// TODO
				throw new NotImplementedException("WriteReferencingElement");
			}

	[TODO]
	protected void WriteRpcResult
				(String name, String ns)
			{
				// TODO
				throw new NotImplementedException("WriteRpcResult");
			}

	[TODO]
	protected void WriteSerializable
				(IXmlSerializable serializable,
				 String name, String ns, bool isNullable)
			{
				// TODO
				throw new NotImplementedException("WriteSerializable");
			}

	[TODO]
	protected void WriteStartDocument()
			{
				// TODO
				throw new NotImplementedException("WriteStartDocument");
			}

	[TODO]
	protected void WriteStartElement
				(String name)
			{
				// TODO
				throw new NotImplementedException("WriteStartElement");
			}

	[TODO]
	protected void WriteStartElement
				(String name, String ns)
			{
				// TODO
				throw new NotImplementedException("WriteStartElement");
			}

	[TODO]
	protected void WriteStartElement
				(String name, String ns, bool writePrefixed)
			{
				// TODO
				throw new NotImplementedException("WriteStartElement");
			}

	[TODO]
	protected void WriteStartElement
				(String name, String ns, Object o)
			{
				// TODO
				throw new NotImplementedException("WriteStartElement");
			}

	[TODO]
	protected void WriteStartElement
				(String name, String ns, Object o, bool writePrefixed)
			{
				// TODO
				throw new NotImplementedException("WriteStartElement");
			}

	[TODO]
	protected void WriteTypedPrimitive
				(String name, String ns, Object o, bool xsiType)
			{
				// TODO
				throw new NotImplementedException("WriteTypedPrimitive");
			}

	[TODO]
	protected void WriteValue
				(byte[] value)
			{
				// TODO
				throw new NotImplementedException("WriteValue");
			}

	[TODO]
	protected void WriteValue
				(String value)
			{
				// TODO
				throw new NotImplementedException("WriteValue");
			}

	[TODO]
	protected void WriteXmlAttribute
				(XmlNode node)
			{
				// TODO
				throw new NotImplementedException("WriteXmlAttribute");
			}

	[TODO]
	protected void WriteXmlAttribute
				(XmlNode node, Object container)
			{
				// TODO
				throw new NotImplementedException("WriteXmlAttribute");
			}

	[TODO]
	protected void WriteXsiType
				(String name, String ns)
			{
				// TODO
				throw new NotImplementedException("WriteXsiType");
			}

}; // class XmlSerializationWriter

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
