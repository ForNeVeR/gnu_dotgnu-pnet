/*
 * DoubleType.cs - Implementation of the
 *			"Microsoft.VisualBasic.DoubleType" class.
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
using System.Globalization;

[StandardModule]
[EditorBrowsable(EditorBrowsableState.Never)]
public sealed class DoubleType
{
	// This class cannot be instantiated.
	private DoubleType() {}

	// Convert a decimal value into a double value.
	public static double DecimalToDouble(IConvertible ValueInterface)
			{
				return Convert.ToDouble(ValueInterface.ToDecimal(null));
			}

	// Convert an object into a double value.
	public static double FromObject(Object Value)
			{
				return FromObject(Value, null);
			}
	[TODO]
	public static double FromObject
				(Object Value, NumberFormatInfo NumberFormat)
			{
				// TODO
				return 0.0;
			}

	// Convert a string into a double value.
	public static double FromString(String Value)
			{
				return FromString(Value, null);
			}
	[TODO]
	public static double FromString
				(String Value, NumberFormatInfo NumberFormat)
			{
				// TODO
				return 0.0;
			}

	// Parse a string into a double value.
	public static double Parse(String Value)
			{
				return Parse(Value, null);
			}
	[TODO]
	public static double Parse(String Value, NumberFormatInfo NumberFormat)
			{
				// TODO
				return 0.0;
			}

}; // class DoubleType

}; // namespace Microsoft.VisualBasic.CompilerServices
