/*
 * ByteType.cs - Implementation of the
 *			"Microsoft.VisualBasic.ByteType" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace Microsoft.VisualBasic.CompilerServices
{

using System;
using System.ComponentModel;

[StandardModule]
[EditorBrowsable(EditorBrowsableState.Never)]
public sealed class ByteType
{
	// This class cannot be instantiated.
	private ByteType() {}

	// Convert a decimal value into a byte.
	public static byte DecimalToByte(IConvertible ValueInterface)
			{
				return Convert.ToByte(ValueInterface.ToDecimal(null));
			}

	// Convert an object into a byte value.
	[TODO]
	public static byte FromObject(Object Value)
			{
				// TODO
				return 0;
			}

	// Convert a string into a byte value.
	[TODO]
	public static byte FromString(String Value)
			{
				// TODO
				return 0;
			}

}; // class ByteType

}; // namespace Microsoft.VisualBasic.CompilerServices
