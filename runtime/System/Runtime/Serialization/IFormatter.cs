/*
 * IFormatter.cs - Implementation of the
 *			"System.Runtime.Serialization.IFormatter" interface.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.Runtime.Serialization
{

#if !ECMA_COMPAT

using System.IO;

public interface IFormatter
{

	// Get or set the binder that performs type lookups.
	SerializationBinder Binder { get; set; }

	// Get or set the streaming context to use for serialization.
	StreamingContext Context { get; set; }

	// Get or set the surrogate selector for the current formatter.
	ISurrogateSelector SurrogateSelector { get; set; }

	// Deserialize an object from a stream.
	Object Deserialize(Stream serializationStream);

	// Serialize an object graph to a stream.
	void Serialize(Stream seralizationStream, Object graph);

}; // interface IFormatter

#endif // !ECMA_COMPAT

}; // namespace System.Runtime.Serialization