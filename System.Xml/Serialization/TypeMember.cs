/*
 * TypeMember.cs - Implementation of the
 *			"System.Xml.Serialization.TypeMember" class.
 *
 * Copyright (C) 2004  Free Software Foundation, Inc.
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

internal sealed class TypeMember
{
	// Publicly-accessible state.
	public Type type;
	public String member;

	// Constructor.
	public TypeMember(Type type, String member)
			{
				this.type = type;
				this.member = member;
			}


	// Get the hash code for this key.
	public override int GetHashCode()
			{
				return ((type.GetHashCode() << 16) | member.GetHashCode());
			}

	// Determine if this key is equal to the given object.
	public override bool Equals(Object o)
			{
				if(o is TypeMember) { return Equals(this, (TypeMember)o); }
				return false;
			}

	// Determine if two keys are equal.
	public static bool Equals(TypeMember first, TypeMember second)
			{
				if(first == second) { return true; }
				if(first == null || second == null) { return false; }
				if(first.type != second.type) { return false; }
				if(first.member != second.member) { return false; }
				return true;
			}

	// Get the string form of this key.
	public override String ToString()
			{
				return String.Format("{0}::{1}", type.FullName, member);
			}

}; // class TypeMember

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
