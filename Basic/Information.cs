/*
 * Information.cs - Implementation of the
 *			"Microsoft.VisualBasic.Information" class.
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

namespace Microsoft.VisualBasic
{

using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using Microsoft.VisualBasic.CompilerServices;

[StandardModule]
public sealed class Information
{
	// Internal state.
	private static ErrObject err;

	// This class cannot be instantiated.
	private Information() {}

	// Get the ERL code for the current error object.
	public static int Erl()
			{
				return Err().Erl;
			}

	// Get the current error object.
	public static ErrObject Err()
			{
				if(err == null)
				{
					err = new ErrObject();
				}
				return err;
			}

	// Value type testing methods.
	public static bool IsArray(Object VarName)
			{
				return (VarName is Array);
			}
	public static bool IsDBNull(Object VarName)
			{
				return (VarName is DBNull);
			}
	public static bool IsDate(Object VarName)
			{
				if(VarName is DateTime)
				{
					return true;
				}
				else if(VarName is String)
				{
					try
					{
						DateType.FromString((String)VarName);
						return true;
					}
					catch(Exception)
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
	public static bool IsError(Object VarName)
			{
				return (VarName is Exception);
			}
	public static bool IsNothing(Object VarName)
			{
				return (VarName == null);
			}
	[TODO]
	public static bool IsNumeric(Object VarName)
			{
				// TODO
				return false;
			}
	public static bool IsReference(Object VarName)
			{
				return !(VarName is ValueType);
			}

	// Get the lower bound of an array.
	[TODO]
	public static int LBound
				(Array Array, [Optional] [DefaultValue(1)] int Rank)
			{
				// TODO
				return 1;
			}

	// Convert a color value into a QB color value.
	[TODO]
	public static int QBColor(int Color)
			{
				// TODO
				return Color;
			}

	// Build a color value from RGB values.
	[TODO]
	public static int RGB(int Red, int Green, int Blue)
			{
				// TODO
				return 0;
			}

	// Get the system type name from a VB type name.
	[TODO]
	public static String SystemTypeName(String VbName)
			{
				// TODO
				return VbName;
			}

	// Get the type name for an object.
	[TODO]
	public static String TypeName(Object VarName)
			{
				if(VarName == null)
				{
					return "Nothing";
				}
				else
				{
					// TODO
					return null;
				}
			}

	// Get the upper bound of an array.
	[TODO]
	public static int UBound
				(Array Array, [Optional] [DefaultValue(1)] int Rank)
			{
				// TODO
				return 1;
			}

	// Get the variant type for an object.
	[TODO]
	public static VariantType VarType(Object VarName)
			{
				// TODO
				return VariantType.Empty;
			}

	// Convert a system type name into a VB type name.
	[TODO]
	public static String VbTypeName(String UrtName)
			{
				// TODO
				return UrtName;
			}

}; // class Information

}; // namespace Microsoft.VisualBasic
