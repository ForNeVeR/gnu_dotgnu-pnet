/*
 * Object.cs - Implementation of the "System.Object" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System
{

using System.Reflection;
using System.Runtime.Remoting;
using System.Runtime.CompilerServices;

public class Object
{

	// Default constructor for the Object class.
	public Object() {}

	// Default definition of the object finalizer.
	protected virtual void Finalize() {}

	// Default implementation of the ToString method.
	public virtual String ToString()
			{ return GetType().ToString(); }

	// Internal methods that are provided by the runtime engine.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public Type GetType();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual int GetHashCode();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual bool Equals(Object obj);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern protected Object MemberwiseClone();

	// Determine if two object references are equal.
	public static bool ReferenceEquals(Object objA, Object objB)
			{
				return (objA == objB);
			}

	// Helper methods that are used in runtime engine callbacks.
	private void FieldSetter(FieldInfo field, Object obj)
	{
		Message.CoerceArg(obj, field.FieldType);
		field.SetValue(this, obj);
	}
	private void FieldGetter(FieldInfo field, ref Object obj)
	{
		obj = field.GetValue(this);
	}

}; // class Object

}; // namespace System
