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

	// Write an object to a stream.
	private void WriteObject
				(BinaryValueWriter.BinaryValueContext context, Object value)
			{
				// Handle the null case first.
				if(value == null)
				{
					context.writer.Write((byte)(BinaryElementType.NullValue));
					return;
				}

				// Get the type of the object and see if we've
				// processed the type before.
				Type type = value.GetType();
				long typeID = context.gen.GetIDForType(type);

				// Allocate an object identifier.
				bool firstTime;
				long objectID = context.gen.GetId(value, out firstTime);

				// Get a value writer for the type.
				BinaryValueWriter writer;
				writer = BinaryValueWriter.GetWriter(context, type);

				// Write the object header.
				writer.WriteObjectHeader(context, type, objectID, typeID);

				// Write the object internals.
				writer.WriteObject(context, value, type);
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

				// Wrap the stream in a binary writer.
				using(BinaryWriter writer =
						new BinaryWriter(serializationStream))
				{
					// Create a binary value writing context.
					BinaryValueWriter.BinaryValueContext context =
						new BinaryValueWriter.BinaryValueContext(this, writer);

					// Write the header information.
					WriteHeader(writer, (headers != null));

					// Write the main object for the message.
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
						WriteObject(context, graph);
					}

					// Write the footer information.
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
