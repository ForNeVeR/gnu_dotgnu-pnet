/*
 * ObjectManager.cs - Implementation of the
 *			"System.Runtime.Serialization.ObjectManager" class.
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

using System.Reflection;

public class ObjectManager
{

	// Constructor.
	[TODO]
	public ObjectManager(ISurrogateSelector selector,
						 StreamingContext context)
			{
				// TODO
			}

	// Perform recorded fixups.
	[TODO]
	public virtual void DoFixups()
			{
				// TODO
			}

	// Return an object with a specific identifier.
	[TODO]
	public virtual Object GetObject(long objectID)
			{
				// TODO
				return null;
			}

	// Raise a deserialization event on all register objects that want it.
	[TODO]
	public virtual void RaiseDeseralizationEvent()
			{
				// TODO
			}

	// Record an array element fixup to be performed later.
	[TODO]
	public virtual void RecordArrayElementFixup
				(long arrayToBeFixed, int index, long objectRequired)
			{
				// TODO
			}
	public virtual void RecordArrayElementFixup
				(long arrayToBeFixed, int[] indices, long objectRequired)
			{
				// TODO
			}

	// Record an object member fixup to be performed later.
	[TODO]
	public virtual void RecordDelayedFixup
				(long objectToBeFixed, String memberName, long objectRequired)
			{
				// TODO
			}
	public virtual void RecordFixup
				(long objectToBeFixed, MemberInfo member, long objectRequired)
			{
				// TODO
			}

	// Register an object with the object manager.
	[TODO]
	public virtual void RegisterObject(Object obj, long objectID)
			{
				// TODO
			}
	[TODO]
	public void RegisterObject(Object obj, long objectID,
							   SerializationInfo info)
			{
				// TODO
			}
	[TODO]
	public void RegisterObject(Object obj, long objectID,
							   SerializationInfo info,
							   long idOfContainingObj,
							   MemberInfo member)
			{
				// TODO
			}
	[TODO]
	public void RegisterObject(Object obj, long objectID,
							   SerializationInfo info,
							   long idOfContainingObj,
							   MemberInfo member,
							   int[] arrayIndex)
			{
				// TODO
			}

}; // class ObjectManager

#endif // !ECMA_COMPAT

}; // namespace System.Runtime.Serialization
