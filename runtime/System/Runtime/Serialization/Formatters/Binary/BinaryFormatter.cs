/*
 * BinaryFormatter.cs - Implementation of the
 *			"System.Runtime.Serialization.BinaryFormatter" class.
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

namespace System.Runtime.Serialization.Formatters.Binary
{

#if CONFIG_REMOTING

using System.IO;
using System.Reflection;
using System.Runtime.Serialization;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;

public sealed class BinaryFormatter : IRemotingFormatter, IFormatter
{
	// Internal state.
	private SerializationBinder binder;
	private StreamingContext context;
	private ISurrogateSelector surrogateSelector;
	private FormatterAssemblyStyle assemblyFormat;
	private FormatterTypeStyle typeFormat;
	private TypeFilterLevel filterLevel;
	private FormatterConverter converter;

	// Constructor.
	public BinaryFormatter()
			{
				this.context = new StreamingContext(StreamingContextStates.All);
				this.assemblyFormat = FormatterAssemblyStyle.Full;
				this.typeFormat = FormatterTypeStyle.TypesAlways;
				this.filterLevel = TypeFilterLevel.Full;
				this.converter = new FormatterConverter();
			}
	public BinaryFormatter(ISurrogateSelector selector,
						   StreamingContext context)
			{
				this.surrogateSelector = selector;
				this.context = context;
				this.assemblyFormat = FormatterAssemblyStyle.Full;
				this.typeFormat = FormatterTypeStyle.TypesAlways;
				this.filterLevel = TypeFilterLevel.Full;
				this.converter = new FormatterConverter();
			}

	// Deserialize an object from a stream.
	[TODO]
	public Object Deserialize(Stream serializationStream,
							  HeaderHandler handler)
			{
				// TODO
				return null;
			}

	// Write a serialization header to a stream.
	private static void WriteHeader(BinaryWriter writer, bool headersPresent)
			{
				writer.Write((byte)(BinaryElementType.Header));
				writer.Write(1);
				writer.Write((headersPresent ? 2 : -1));
				writer.Write(1);
				writer.Write(0);
			}

	// Write a serialization footer to a stream.
	private static void WriteFooter(BinaryWriter writer)
			{
				writer.Write((byte)(BinaryElementType.End));
			}

	// Write an assembly reference to a stream and return its object ID.
	private long WriteAssembly(BinaryWriter writer, ObjectIDGenerator gen,
							   Assembly assembly, String name)
			{
				bool firstTime;
				long objectID = gen.GetId(assembly, out firstTime);
				if(firstTime)
				{
					writer.Write((byte)(BinaryElementType.Assembly));
					writer.Write(objectID);
					if(assemblyFormat == FormatterAssemblyStyle.Full)
					{
						writer.Write(name);
					}
					else
					{
						int index = name.IndexOf(',');
						if(index != -1)
						{
							writer.Write(name.Substring(0, index));
						}
						else
						{
							writer.Write(name);
						}
					}
				}
				return objectID;
			}

	// Get the serialization data for an object.
	private SerializationInfo GetObjectData(Object obj, Type type)
			{
				SerializationInfo info;
				info = new SerializationInfo(type, converter);
				if(obj is ISerializable)
				{
					// Use the type-supplied object data method.
					((ISerializable)obj).GetObjectData(info, context);
					return info;
				}
				if(surrogateSelector != null)
				{
					// Look for a serialization surrogate.
					ISerializationSurrogate surrogate;
					ISurrogateSelector selector;
					surrogate = surrogateSelector.GetSurrogate
						(type, context, out selector);
					if(surrogate != null)
					{
						surrogate.GetObjectData(obj, info, context);
						return info;
					}
				}
				if(type.IsSerializable)
				{
					// Collect up fields using the default algorithm.
					// TODO: serialize the properties also
					FieldInfo[] fields = type.GetFields
						(BindingFlags.Public |
						 BindingFlags.NonPublic |
						 BindingFlags.Instance);
					foreach(FieldInfo field in fields)
					{
						if(!(field.IsNotSerialized))
						{
							info.AddValue(field.Name, field.GetValue(obj),
									 	  field.FieldType);
						}
					}
					return info;
				}
				throw new SerializationException
					(String.Format(_("Serialize_NotSerializable"), type));
			}

	// Write a type tag.
	private static void WriteTypeTag(BinaryWriter writer, Type type)
			{
				// TODO
			}

	// Write a type specification.
	private static void WriteTypeSpec(BinaryWriter writer, Type type)
			{
				// TODO
			}

	// Write the type information to a stream.
	private static void WriteTypeInfo(BinaryWriter writer,
									  BinaryElementType elementType,
									  SerializationInfo info,
									  long assemID, int objectID, long typeID)
			{
				if(typeID != -1)
				{
					// Reference a previous object's type information.
					writer.Write((byte)(BinaryElementType.RefTypeObject));
					writer.Write(objectID);
					writer.Write((int)typeID);
				}
				else
				{
					// Dump type information for the first time.
					writer.Write((byte)elementType);
					writer.Write(objectID);
					writer.Write(info.FullTypeName);
					writer.Write(info.MemberCount);
					SerializationInfoEnumerator e = info.GetEnumerator();
					while(e.MoveNext())
					{
						writer.Write(e.Name);
					}
					e.Reset();
					while(e.MoveNext())
					{
						WriteTypeTag(writer, e.ObjectType);
					}
					e.Reset();
					while(e.MoveNext())
					{
						WriteTypeSpec(writer, e.ObjectType);
					}
					if(assemID != -1)
					{
						writer.Write((int)assemID);
					}
				}
			}

	// Write the fields of an object.
	private void WriteObjectInfo(BinaryWriter writer, ObjectIDGenerator gen,
								 Object value, Type type,
								 SerializationInfo info)
			{
				SerializationInfoEnumerator e = info.GetEnumerator();
				while(e.MoveNext())
				{
					// TODO: write inline field values
				}
			}

	// Write an object to a stream.
	private void WriteObject(BinaryWriter writer, ObjectIDGenerator gen,
							 Object value)
			{
				// Handle the null case first.
				if(value == null)
				{
					writer.Write((byte)(BinaryElementType.NullValue));
					return;
				}

				// Get the type of the object and see if we've
				// processed the type before.
				Type type = value.GetType();
				long typeID = gen.GetIDForType(type);

				// Get an object identifier and check to see if we
				// already saw this object previously.
				bool firstTime;
				int objectID = (int)(gen.GetId(value, out firstTime));
				if(!firstTime)
				{
					writer.Write((byte)(BinaryElementType.ObjectReference));
					writer.Write(objectID);
					return;
				}

				// Handle strings, which are relatively easy.
				if(value is String)
				{
					writer.Write((byte)(BinaryElementType.String));
					writer.Write(objectID);
					writer.Write((String)value);
					return;
				}

				// Determine how to serialize the object.
				SerializationInfo info;
				if(type.IsArray)
				{
					// This is an array object.
					// TODO
				}
				else if(type.Assembly == Assembly.GetExecutingAssembly())
				{
					// This is a runtime object.
					info = GetObjectData(value, type);
					WriteTypeInfo(writer, BinaryElementType.RuntimeObject,
								  info, -1, objectID, typeID);
					WriteObjectInfo(writer, gen, value, type, info);
				}
				else
				{
					// This is an object within another assembly.
					info = GetObjectData(value, type);
					long assemID = WriteAssembly
						(writer, gen, type.Assembly, info.AssemblyName);
					WriteTypeInfo(writer, BinaryElementType.ExternalObject,
								  info, assemID, objectID, typeID);
					WriteObjectInfo(writer, gen, value, type, info);
				}
			}

	// Serialize an object to a stream.
	[TODO]
	public void Serialize(Stream serializationStream,
						  Object graph, Header[] headers)
			{
				// Validate the parameters.
				if(serializationStream == null)
				{
					throw new ArgumentNullException("serializationStream");
				}

				// We need an object ID generator for serialization.
				ObjectIDGenerator gen = new ObjectIDGenerator();

				// Wrap the stream in a binary writer.
				using(BinaryWriter writer =
						new BinaryWriter(serializationStream))
				{
					WriteHeader(writer, (headers != null));
					if(graph is IMethodCallMessage)
					{
						// TODO
					}
					else if(graph is IMethodReturnMessage)
					{
						// TODO
					}
					else
					{
						WriteObject(writer, gen, graph);
					}
					WriteFooter(writer);
				}
			}

	// Implement the IFormatter interface.
	public SerializationBinder Binder
			{
				get
				{
					return binder;
				}
				set
				{
					binder = value;
				}
			}
	public StreamingContext Context
			{
				get
				{
					return context;
				}
				set
				{
					context = value;
				}
			}
	public ISurrogateSelector SurrogateSelector
			{
				get
				{
					return surrogateSelector;
				}
				set
				{
					surrogateSelector = value;
				}
			}
	public Object Deserialize(Stream serializationStream)
			{
				return Deserialize(serializationStream, null);
			}
	public void Serialize(Stream serializationStream, Object graph)
			{
				Serialize(serializationStream, graph, null);
			}

	// Formatter properties.
	public FormatterAssemblyStyle AssemblyFormat
			{
				get
				{
					return assemblyFormat;
				}
				set
				{
					assemblyFormat = value;
				}
			}
	public FormatterTypeStyle TypeFormat
			{
				get
				{
					return typeFormat;
				}
				set
				{
					typeFormat = value;
				}
			}
	[ComVisible(false)]
	public TypeFilterLevel FilterLevel
			{
				get
				{
					return filterLevel;
				}
				set
				{
					filterLevel = value;
				}
			}

	// Deserialize the response to a method call.
	[TODO]
	public Object DeserializeMethodResponse
				(Stream serializationStream, HeaderHandler handler,
				 IMethodCallMessage methodCallMessage)
			{
				// TODO
				return null;
			}

	// Unsafe version of "Deserialize".
	[ComVisible(false)]
	public Object UnsafeDeserialize(Stream serializationStream,
							        HeaderHandler handler)
			{
				// We always do things safely.
				return Deserialize(serializationStream, handler);
			}

	// Unsafe version of "DeserializeMethodResponse".
	[ComVisible(false)]
	public Object UnsafeDeserializeMethodResponse
				(Stream serializationStream, HeaderHandler handler,
				 IMethodCallMessage methodCallMessage)
			{
				return DeserializeMethodResponse
					(serializationStream, handler, methodCallMessage);
			}

}; // class BinaryFormatter

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Serialization.Formatters.Binary
