/*
 * FormatterServices.cs - Implementation of the
 *			"System.Runtime.Serialization.FormatterServices" class.
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
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
using System.Security.Permissions;
using System.Runtime.CompilerServices;
using System.Runtime.Serialization.Formatters;

public sealed class FormatterServices
{
	// This class cannot be instantiated.
	private FormatterServices() {}

	// Check whether a type can be deserialized.
	[TODO]
	public static void CheckTypeSecurity(Type t, TypeFilterLevel securityLevel)
			{
				// TODO
			}

	// Extract data from a specified object.
	[SecurityPermission(SecurityAction.LinkDemand,
						Flags=SecurityPermissionFlag.SerializationFormatter)]
	public static Object[] GetObjectData(Object obj, MemberInfo[] members)
			{
				if(obj == null)
				{
					throw new ArgumentNullException("obj");
				}
				if(members == null)
				{
					throw new ArgumentNullException("members");
				}
				Object[] data = new Object [members.Length];
				int index;
				for(index = 0; index < members.Length; ++index)
				{
					if(members[index] == null)
					{
						throw new ArgumentNullException
							("members[" + index.ToString() + "]");
					}
					if(!(members[index] is FieldInfo))
					{
						throw new SerializationException
							(_("Serialize_NotField"));
					}
					data[index] = ((FieldInfo)(members[index])).GetValue(obj);
				}
				return data;
			}

	// Get all of the serializable members for a specified type.
	[SecurityPermission(SecurityAction.LinkDemand,
						Flags=SecurityPermissionFlag.SerializationFormatter)]
	public static MemberInfo[] GetSerializableMembers(Type type)
			{
				return GetSerializableMembers
					(type, new StreamingContext(StreamingContextStates.All));
			}
	[SecurityPermission(SecurityAction.LinkDemand,
						Flags=SecurityPermissionFlag.SerializationFormatter)]
	public static MemberInfo[] GetSerializableMembers
				(Type type, StreamingContext context)
			{
				if(type == null)
				{
					throw new ArgumentNullException("type");
				}

				// Fetch the property and field lists.
				FieldInfo[] fields = type.GetFields
					(BindingFlags.Public | BindingFlags.NonPublic |
					 BindingFlags.Instance);
				PropertyInfo[] properties = type.GetProperties
					(BindingFlags.Public | BindingFlags.NonPublic |
					 BindingFlags.Instance);

				// Determine the size of the final array.
				int size = 0;
				int posn;
				for(posn = 0; posn < fields.Length; ++posn)
				{
					if((fields[posn].Attributes &
							FieldAttributes.NotSerialized) == 0)
					{
						++size;
					}
				}
				for(posn = 0; posn < properties.Length; ++posn)
				{
					if(properties[posn].CanRead &&
					   properties[posn].CanWrite)
					{
						++size;
					}
				}

				// Create the member array and populate it.
				MemberInfo[] members = new MemberInfo [size];
				size = 0;
				for(posn = 0; posn < fields.Length; ++posn)
				{
					if((fields[posn].Attributes &
							FieldAttributes.NotSerialized) == 0)
					{
						members[size++] = fields[posn];
					}
				}
				for(posn = 0; posn < properties.Length; ++posn)
				{
					if(properties[posn].CanRead &&
					   properties[posn].CanWrite)
					{
						members[size++] = properties[posn];
					}
				}
				return members;
			}

	// Get a named type from a specific assembly
	[SecurityPermission(SecurityAction.LinkDemand,
						Flags=SecurityPermissionFlag.SerializationFormatter)]
	[ReflectionPermission(SecurityAction.LinkDemand, TypeInformation=true)]
	public static Type GetTypeFromAssembly(Assembly assem, String name)
			{
				if(assem == null)
				{
					throw new ArgumentNullException("assem");
				}
				return assem.GetType(name);
			}

	// Get an uninitialized instance from the runtime engine.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static Object InternalGetUninitializedObject(Type type);

	// Create a new unitialized instance of a specific object type.
	[SecurityPermission(SecurityAction.LinkDemand,
						Flags=SecurityPermissionFlag.SerializationFormatter)]
	public static Object GetUninitializedObject(Type type)
			{
				if(type == null)
				{
					throw new ArgumentNullException("type");
				}
				else if(!(type is ClrType))
				{
					throw new SerializationException
						(_("Serialize_NotClrType"));
				}
				return InternalGetUninitializedObject(type);
			}
	[SecurityPermission(SecurityAction.LinkDemand,
						Flags=SecurityPermissionFlag.SerializationFormatter)]
	public static Object GetSafeUninitializedObject(Type type)
			{
				return GetUninitializedObject(type);
			}

	// Populate the members of a specific object with given data values.
	[SecurityPermission(SecurityAction.LinkDemand,
						Flags=SecurityPermissionFlag.SerializationFormatter)]
	public static Object PopulateObjectMembers
				(Object obj, MemberInfo[] members, Object[] data)
			{
				// Validate the parameters.
				if(obj == null)
				{
					throw new ArgumentNullException("obj");
				}
				else if(members == null)
				{
					throw new ArgumentNullException("members");
				}
				else if(data == null)
				{
					throw new ArgumentNullException("data");
				}
				else if(members.Length != data.Length)
				{
					throw new ArgumentException
						(_("Serialize_MemberDataMismatch"));
				}

				// Set the member values.
				int index;
				for(index = 0; index < members.Length; ++index)
				{
					if(members[index] == null)
					{
						throw new ArgumentNullException
							("members[" + index.ToString() + "]");
					}
					if(data[index] == null)
					{
						// Skip entries with null values.
						continue;
					}
					if(!(members[index] is FieldInfo))
					{
						throw new SerializationException
							(_("Serialize_NotField"));
					}
					((FieldInfo)(members[index])).SetValue(obj, data[index]);
				}
				return obj;
			}

}; // class FormatterServices

#endif // !ECMA_COMPAT

}; // namespace System.Runtime.Serialization
