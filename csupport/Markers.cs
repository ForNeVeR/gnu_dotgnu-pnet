/*
 * Markers.cs - Classes that mark program items for special C behaviours.
 *
 * This file is part of the Portable.NET "C language support" library.
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace OpenSystem.C
{

using System;

// A modifier class that is used to mark "const" types.
public sealed class IsConst {}

// An attribute class that is used to mark bit fields.
[AttributeUsage(AttributeTargets.Field)]
public sealed class BitFieldAttribute : Attribute
{
	public BitFieldAttribute(int start, int numBits) {}
}

// An attribute class that is used to mark weak alias definitions.
[AttributeUsage(AttributeTargets.Field | AttributeTargets.Method)]
public sealed class WeakAliasForAttribute : Attribute
{
	public WeakAliasForAttribute(String name) {}
}

// An attribute class that is used to mark initializers.
[AttributeUsage(AttributeTargets.Method)]
public sealed class InitializerAttribute : Attribute
{
	public InitializerAttribute() {}
}

// An attribute class that is used to specify initialization order.
[AttributeUsage(AttributeTargets.Method)]
public sealed class InitializerOrderAttribute : Attribute
{
	public InitializerOrderAttribute(int order) {}
}

// An attribute class that is used to mark finalizers.
[AttributeUsage(AttributeTargets.Method)]
public sealed class FinalizerAttribute : Attribute
{
	public FinalizerAttribute() {}
}

// An attribute class that is used to specify finalization order.
[AttributeUsage(AttributeTargets.Method)]
public sealed class FinalizerOrderAttribute : Attribute
{
	public FinalizerOrderAttribute(int order) {}
}

// An attribute class that is used to mark the memory model.
[AttributeUsage(AttributeTargets.Module)]
public sealed class MemoryModelAttribute : Attribute
{
	public MemoryModelAttribute(int ptrBits) {}
}

} // namespace OpenSystem.C
