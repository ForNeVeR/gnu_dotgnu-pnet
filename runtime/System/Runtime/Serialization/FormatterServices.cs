/*
 * FormatterServices.cs - Implementation of the
 *			"System.Runtime.Serialization.FormatterServices" class.
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

public sealed class FormatterServices
{
	// This class cannot be instantiated.
	private FormatterServices() {}

	// Extract data from a specified object.
	[TODO]
	public static Object[] GetObjectData(Object obj, MemberInfo[] members)
			{
				// TODO
				return null;
			}

	// Get all of the serializable members for a specified type.
	[TODO]
	public static MemberInfo[] GetSerializableMembers(Type type)
			{
				// TODO
				return null;
			}
	[TODO]
	public static MemberInfo[] GetSerializableMembers
				(Type type, StreamingContext context)
			{
				// TODO
				return null;
			}

	// Get a named type from a specific assembly
	public static Type GetTypeFromAssembly(Assembly assem, String name)
			{
				if(assem == null)
				{
					throw new ArgumentNullException("assem");
				}
				return assem.GetType(name);
			}

	// Create a new unitialized instance of a specific object type.
	[TODO]
	public static Object GetUnitializedObject(Type type)
			{
				// TODO
				return null;
			}

	// Populate the members of a specific object with given data values.
	[TODO]
	public static Object PopulateObjectMembers
				(Object obj, MemberInfo[] members, Object[] data)
			{
				// TODO
				return null;
			}

}; // class FormatterServices

#endif // !ECMA_COMPAT

}; // namespace System.Runtime.Serialization
