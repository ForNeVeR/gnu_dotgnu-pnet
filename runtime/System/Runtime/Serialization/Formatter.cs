/*
 * Formatter.cs - Implementation of the
 *			"System.Runtime.Serialization.Formatter" class.
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
using System.Collections;

[CLSCompliant(false)]
public abstract class Formatter : IFormatter
{
	// Inherited state.
	protected ObjectIDGenerator m_idGenerator;
	protected Queue m_objectQueue;

	// Constructor.
	protected Formatter()
			{
				m_idGenerator = new ObjectIDGenerator();
				m_objectQueue = new Queue();
			}

	// Get or set the binder that performs type lookups.
	public abstract SerializationBinder Binder { get; set; }

	// Get or set the streaming context to use for serialization.
	public abstract StreamingContext Context { get; set; }

	// Get or set the surrogate selector for the current formatter.
	public abstract ISurrogateSelector SurrogateSelector { get; set; }

	// Deserialize an object from a stream.
	public abstract Object Deserialize(Stream serializationStream);

	// Serialize an object graph to a stream.
	public abstract void Serialize(Stream seralizationStream, Object graph);

	// Get the next object to be serialized.
	protected virtual Object GetNext(out long objID)
			{
				Object next = m_objectQueue.Dequeue();
				bool firstTime;
				objID = m_idGenerator.HasId(next, out firstTime);
				if(!firstTime)
				{
					throw new SerializationException
						(_("Serialize_NoObjectID"));
				}
				return next;
			}

	// Schedule an object for later serialization.
	protected virtual long Schedule(Object obj)
			{
				bool firstTime;
				long objID = m_idGenerator.GetId(obj, out firstTime);
				m_objectQueue.Enqueue(obj);
				return objID;
			}

	// Write various kinds of types to the serialization stream.
	protected abstract void WriteArray(Object obj, String name,
									   Type memberType);
	protected abstract void WriteBoolean(bool val, String name);
	protected abstract void WriteByte(byte val, String name);
	protected abstract void WriteSByte(sbyte val, String name);
	protected abstract void WriteInt16(short val, String name);
	protected abstract void WriteUInt16(ushort val, String name);
	protected abstract void WriteChar(char val, String name);
	protected abstract void WriteInt32(int val, String name);
	protected abstract void WriteUInt32(uint val, String name);
	protected abstract void WriteInt64(long val, String name);
	protected abstract void WriteUInt64(ulong val, String name);
	protected abstract void WriteSingle(float val, String name);
	protected abstract void WriteDouble(double val, String name);
	protected abstract void WriteDateTime(DateTime val, String name);
	protected abstract void WriteDecimal(Decimal val, String name);
	protected abstract void WriteMember(String memberName, Object data);
	protected abstract void WriteObjectRef(Object obj, String name,
										   Type memberType);
	protected abstract void WriteTimeSpan(TimeSpan val, String name);
	protected abstract void WriteValueType(Object obj, String name,
										   Type memberType);

}; // class Formatter

#endif // !ECMA_COMPAT

}; // namespace System.Runtime.Serialization
