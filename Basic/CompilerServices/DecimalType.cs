/*
 * DecimalType.cs - Implementation of the
 *			"Microsoft.VisualBasic.DecimalType" class.
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
public sealed class DecimalType
{
	// This class cannot be instantiated.
	private DecimalType() {}

	// Convert a boolean value into a decimal value.
	public static Decimal FromBoolean(bool Value)
			{
				return (Value ? -1.0m : 0.0m);
			}

	// Convert an object into a decimal value.
	public static Decimal FromObject(Object Value)
			{
				return FromObject(Value, null);
			}
	public static Decimal FromObject
				(Object Value, NumberFormatInfo NumberFormat)
			{
				if(Value != null)
				{
					IConvertible ic = (Value as IConvertible);
					if(ic != null)
					{
						if(ic.GetTypeCode() != TypeCode.String)
						{
							return ic.ToDecimal(NumberFormat);
						}
						else
						{
							return FromString(ic.ToString(null), NumberFormat);
						}
					}
					else
					{
						throw new InvalidCastException
							(String.Format
								(S._("VB_InvalidCast"),
								 Value.GetType(), "System.Decimal"));
					}
				}
				else
				{
					return 0.0m;
				}
			}

	// Convert a string into a decimal value.
	public static Decimal FromString(String Value)
			{
				return FromString(Value, null);
			}
	[TODO]
	public static Decimal FromString
				(String Value, NumberFormatInfo NumberFormat)
			{
				// TODO
				return 0.0m;
			}

	// Parse a string into a decimal value.
	[TODO]
	public static Decimal Parse(String Value, NumberFormatInfo NumberFormat)
			{
				// TODO
				return 0.0m;
			}

}; // class DecimalType

}; // namespace Microsoft.VisualBasic.CompilerServices
