/*
 * LongDouble.cs - Wrapper class for the "native float" type.
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
using System.Runtime.InteropServices;

[StructLayout(LayoutKind.Sequential)]
public struct LongDouble
{
	// Internal state.
	private __long_double value__;

	// Constructor.
	public LongDouble(__long_double value)
			{
				value__ = value;
			}

	// Unpack this value to recover the "native float".
	public __long_double Unpack()
			{
				return value__;
			}

} // struct LongDouble

} // namespace OpenSystem.C
