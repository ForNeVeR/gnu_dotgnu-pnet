/*
 * XmlSerializationReader.cs - Implementation of the
 *			"System.Xml.Serialization.XmlSerializationReader" class.
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
public abstract class XmlSerializationReader
{
	[TODO]
	protected XmlSerializationReader()
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	protected XmlDocument Document
			{
				get
				{
					// TODO
					throw new NotImplementedException("Document");
				}
			}

	[TODO]
	protected bool IsReturnValue
			{
				get
				{
					// TODO
					throw new NotImplementedException("IsReturnValue");
				}
				set
				{
					// TODO
					throw new NotImplementedException("IsReturnValue");
				}
			}

	[TODO]
	protected XmlReader Reader
			{
				get
				{
					// TODO
					throw new NotImplementedException("Reader");
				}
			}

	[TODO]
	protected void AddFixup
				(CollectionFixup fixup)
			{
				// TODO
				throw new NotImplementedException("AddFixup");
			}

	[TODO]
	protected void AddFixup
				(Fixup fixup)
			{
				// TODO
				throw new NotImplementedException("AddFixup");
			}

	[TODO]
	protected void AddReadCallback
				(String name, String ns, Type type,
				 XmlSerializationReadCallback read)
			{
				// TODO
				throw new NotImplementedException("AddReadCallback");
			}

	[TODO]
	protected void AddTarget
				(String id, Object o)
			{
				// TODO
				throw new NotImplementedException("AddTarget");
			}

	[TODO]
	protected Exception CreateAbstractTypeException
				(String name, String ns)
			{
				// TODO
				throw new NotImplementedException("CreateAbstractTypeException");
			}

	[TODO]
	protected Exception CreateCtorHasSecurityException
				(String typeName)
			{
				// TODO
				throw new NotImplementedException("CreateCtorHasSecurityException");
			}

	[TODO]
	protected Exception CreateInaccessibleConstructorException
				(String typeName)
			{
				// TODO
				throw new NotImplementedException("CreateInaccessibleConstructorException");
			}

	[TODO]
	protected Exception CreateInvalidCastException
				(Type type, Object value)
			{
				// TODO
				throw new NotImplementedException("CreateInvalidCastException");
			}

	[TODO]
	protected Exception CreateReadOnlyCollectionException
				(String name)
			{
				// TODO
				throw new NotImplementedException("CreateReadOnlyCollectionException");
			}

	[TODO]
	protected Exception CreateUnknownConstantException
				(String value, Type enumType)
			{
				// TODO
				throw new NotImplementedException("CreateUnknownConstantException");
			}

	[TODO]
	protected Exception CreateUnknownNodeException()
			{
				// TODO
				throw new NotImplementedException("CreateUnknownNodeException");
			}

	[TODO]
	protected Exception CreateUnknownTypeException
				(XmlQualifiedName type)
			{
				// TODO
				throw new NotImplementedException("CreateUnknownTypeException");
			}

	[TODO]
	protected Array EnsureArrayIndex
				(Array a, int index, Type elementType)
			{
				// TODO
				throw new NotImplementedException("EnsureArrayIndex");
			}

	[TODO]
	protected void FixupArrayRefs
				(Object fixup)
			{
				// TODO
				throw new NotImplementedException("FixupArrayRefs");
			}

	[TODO]
	protected int GetArrayLength
				(String name, String ns)
			{
				// TODO
				throw new NotImplementedException("GetArrayLength");
			}

	[TODO]
	protected bool GetNullAttr()
			{
				// TODO
				throw new NotImplementedException("GetNullAttr");
			}

	[TODO]
	protected Object GetTarget
				(String id)
			{
				// TODO
				throw new NotImplementedException("GetTarget");
			}

	[TODO]
	protected XmlQualifiedName GetXsiType()
			{
				// TODO
				throw new NotImplementedException("GetXsiType");
			}

	protected abstract void InitCallbacks();
	protected abstract void InitIDs();

	[TODO]
	protected bool IsXmlnsAttribute
				(String name)
			{
				// TODO
				throw new NotImplementedException("IsXmlnsAttribute");
			}

	[TODO]
	protected void ParseWsdlArrayType
				(XmlAttribute attr)
			{
				// TODO
				throw new NotImplementedException("ParseWsdlArrayType");
			}

	[TODO]
	protected XmlQualifiedName ReadElementQualifiedName()
			{
				// TODO
				throw new NotImplementedException("ReadElementQualifiedName");
			}

	[TODO]
	protected void ReadEndElement()
			{
				// TODO
				throw new NotImplementedException("ReadEndElement");
			}

	[TODO]
	protected bool ReadNull()
			{
				// TODO
				throw new NotImplementedException("ReadNull");
			}

	[TODO]
	protected XmlQualifiedName ReadNullableQualifiedName()
			{
				// TODO
				throw new NotImplementedException("ReadNullableQualifiedName");
			}

	[TODO]
	protected String ReadNullableString()
			{
				// TODO
				throw new NotImplementedException("ReadNullableString");
			}

	[TODO]
	protected bool ReadReference
				(out String fixupReference)
			{
				// TODO
				throw new NotImplementedException("ReadReference");
			}

	[TODO]
	protected Object ReadReferencedElement()
			{
				// TODO
				throw new NotImplementedException("ReadReferencedElement");
			}

	[TODO]
	protected Object ReadReferencedElement
				(String name, String ns)
			{
				// TODO
				throw new NotImplementedException("ReadReferencedElement");
			}

	[TODO]
	protected void ReadReferencedElements()
			{
				// TODO
				throw new NotImplementedException("ReadReferencedElements");
			}

	[TODO]
	protected Object ReadReferencingElement
				(out String fixupReference)
			{
				// TODO
				throw new NotImplementedException("ReadReferencingElement");
			}

	[TODO]
	protected Object ReadReferencingElement
				(String name, String ns, out String fixupReference)
			{
				// TODO
				throw new NotImplementedException("ReadReferencingElement");
			}

	[TODO]
	protected Object ReadReferencingElement
				(String name, String ns, bool elementCanBeType,
				 out String fixupReference)
			{
				// TODO
				throw new NotImplementedException("ReadReferencingElement");
			}

	[TODO]
	protected IXmlSerializable ReadSerializable
				(IXmlSerializable serializable)
			{
				// TODO
				throw new NotImplementedException("ReadSerializable");
			}

	[TODO]
	protected String ReadString
				(String value)
			{
				// TODO
				throw new NotImplementedException("ReadString");
			}

	[TODO]
	protected Object ReadTypedPrimitive
				(XmlQualifiedName type)
			{
				// TODO
				throw new NotImplementedException("ReadTypedPrimitive");
			}

	[TODO]
	protected XmlDocument ReadXmlDocument
				(bool wrapped)
			{
				// TODO
				throw new NotImplementedException("ReadXmlDocument");
			}

	[TODO]
	protected XmlNode ReadXmlNode
				(bool wrapped)
			{
				// TODO
				throw new NotImplementedException("ReadXmlNode");
			}

	[TODO]
	protected void Referenced
				(Object o)
			{
				// TODO
				throw new NotImplementedException("Referenced");
			}

	[TODO]
	protected Array ShrinkArray
				(Array a, int length, Type elementType, bool isNullable)
			{
				// TODO
				throw new NotImplementedException("ShrinkArray");
			}

	[TODO]
	protected byte[] ToByteArrayBase64
				(bool isNull)
			{
				// TODO
				throw new NotImplementedException("ToByteArrayBase64");
			}

	[TODO]
	protected static byte[] ToByteArrayBase64
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToByteArrayBase64");
			}

	[TODO]
	protected byte[] ToByteArrayHex
				(bool isNull)
			{
				// TODO
				throw new NotImplementedException("ToByteArrayHex");
			}

	[TODO]
	protected static byte[] ToByteArrayHex
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToByteArrayHex");
			}

	[TODO]
	protected static char ToChar
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToChar");
			}

	[TODO]
	protected static DateTime ToDate
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToDate");
			}

	[TODO]
	protected static DateTime ToDateTime
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToDateTime");
			}

	[TODO]
	protected static long ToEnum
				(String value, Hashtable h, String typeName)
			{
				// TODO
				throw new NotImplementedException("ToEnum");
			}

	[TODO]
	protected static DateTime ToTime
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToTime");
			}

	[TODO]
	protected static String ToXmlName
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToXmlName");
			}

	[TODO]
	protected static String ToXmlNCName
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToXmlNCName");
			}

	[TODO]
	protected static String ToXmlNmToken
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToXmlNmToken");
			}

	[TODO]
	protected static String ToXmlNmTokens
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToXmlNmTokens");
			}

	[TODO]
	protected XmlQualifiedName ToXmlQualifiedName
				(String value)
			{
				// TODO
				throw new NotImplementedException("ToXmlQualifiedName");
			}

	[TODO]
	protected void UnknownAttribute
				(Object o, XmlAttribute attr)
			{
				// TODO
				throw new NotImplementedException("UnknownAttribute");
			}

	[TODO]
	protected void UnknownElement
				(Object o, XmlElement elem)
			{
				// TODO
				throw new NotImplementedException("UnknownElement");
			}

	[TODO]
	protected void UnknownNode
				(Object o)
			{
				// TODO
				throw new NotImplementedException("UnknownNode");
			}

	[TODO]
	protected void UnreferencedObject
				(String id, Object o)
			{
				// TODO
				throw new NotImplementedException("UnreferencedObject");
			}



































	protected class CollectionFixup
	{
		// Internal state.
		private XmlSerializationCollectionFixupCallback callback;
		private Object collection;
		private Object collectionItems;

		// Constructor.
		public CollectionFixup
					(Object collection,
					 XmlSerializationCollectionFixupCallback callback,
					 Object collectionItems)
				: base()
				{
					this.callback = callback;
					this.collection = collection;
					this.collectionItems = collectionItems;
				}

		// Properties.
		public XmlSerializationCollectionFixupCallback Callback
				{
					get { return callback; }
				}
		public Object Collection
				{
					get { return collection; }
				}
		public Object CollectionItems
				{
					get { return collectionItems; }
				}

	}; // class CollectionFixup










	[TODO]
	protected class Fixup
	{
		// Internal state.
		private XmlSerializationFixupCallback callback;
		private String[] ids;
		private Object source;

		// Constructors.
		[TODO]
		public Fixup(Object source, XmlSerializationFixupCallback callback, int count)
				: base()
				{
					this.callback = callback;
					this.ids = null; // TODO
					this.source = source;
				}
		public Fixup(Object source, XmlSerializationFixupCallback callback, String[] ids)
				: base()
				{
					this.callback = callback;
					this.ids = ids;
					this.source = source;
				}

		// Properties.
		public XmlSerializationFixupCallback Callback
				{
					get { return callback; }
				}
		public String[] Ids
				{
					get { return ids; }
				}
		[TODO]
		public Object Source
				{
					get { return source; }
					set
					{
						// TODO?
						source = value;
					}
				}

	}; // class Fixup

}; // class XmlSerializationReader

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
