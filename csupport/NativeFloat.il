/*
 * NativeFloat.il - Wrapper class for the "native float" type.
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

/*

The standard C# base class library lacks a value type class
that can be used to wrap up a "native float" value, and there
is no way to define such a class using C#.  So, we have to
write the necessary code by hand in CIL assembly code.

*/

.assembly '<NativeFloat>' {}
.assembly extern .library {}
.module '<NativeFloat>'

.namespace OpenSystem.C
{

.class public sequential sealed ansi NativeFloat extends System.ValueType
{
	.field private specialname rtspecialname native float value__

	.method public hidebysig specialname rtspecialname instance
				void .ctor(native float 'value') cil managed 
	{
		.maxstack 2
		ldarg.0
		ldarg.1
		stfld native float OpenSystem.C.NativeFloat::value__
		ret
	}

	.method public hidebysig instance native float Unpack() cil managed 
	{
		.maxstack 1
		ldarg.0
		ldfld native float OpenSystem.C.NativeFloat::value__
		ret
	}

} // class NativeFloat

} // namespace OpenSystem.C
