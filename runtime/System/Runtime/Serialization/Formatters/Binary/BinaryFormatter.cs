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
using System.Runtime.Serialization;
using System.Runtime.Remoting.Messaging;

public sealed class BinaryFormatter : IRemotingFormatter, IFormatter
{
	// Internal state.
	private SerializationBinder binder;
	private StreamingContext context;
	private ISurrogateSelector surrogateSelector;
	private FormatterAssemblyStyle assemblyFormat;
	private FormatterTypeStyle typeFormat;

	// Constructor.
	public BinaryFormatter()
			{
				context = new StreamingContext(StreamingContextStates.All);
			}
	public BinaryFormatter(ISurrogateSelector selector,
						   StreamingContext context)
			{
				this.surrogateSelector = selector;
				this.context = context;
			}

	// Implement the IRemotingFormatter interface.
	[TODO]
	public Object Deserialize(Stream serializationStream,
							  HeaderHandler handler)
			{
				// TODO
				return null;
			}
	[TODO]
	public void Serialize(Stream serializationStream,
						  Object graph, Header[] headers)
			{
				// TODO
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
				Serialize(serializationStream, graph);
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

	// Deserialize the response to a method call.
	public Object DeserializeMethodResponse
				(Stream serializationStream, HeaderHandler handler,
				 IMethodCallMessage methodCallMessage)
			{
				// TODO
				return null;
			}

}; // class BinaryFormatter

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Serialization.Formatters.Binary
