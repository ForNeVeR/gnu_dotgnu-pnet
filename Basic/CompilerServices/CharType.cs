/*
 * CharType.cs - Implementation of the
 *			"Microsoft.VisualBasic.CharType" class.
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
#if CONFIG_COMPONENT_MODEL
[EditorBrowsable(EditorBrowsableState.Never)]
#endif
public sealed class CharType
{
	// This class cannot be instantiated.
	private CharType() {}

	// Convert an object into a char value.
	public static char FromObject(Object Value)
			{
				if(Value != null)
				{
					IConvertible ic = (Value as IConvertible);
					if(ic != null)
					{
						if(ic.GetTypeCode() != TypeCode.String)
						{
							return ic.ToChar(null);
						}
						else
						{
							return FromString(ic.ToString(null));
						}
					}
					else
					{
						throw new InvalidCastException
							(String.Format
								(S._("VB_InvalidCast"),
								 Value.GetType(), "System.Char"));
					}
				}
				else
				{
					return '\0';
				}
			}

	// Convert a string into a char array value.
	public static char FromString(String Value)
			{
				if(Value != null && Value.Length != 0)
				{
					return Value[0];
				}
				else
				{
					return '\0';
				}
			}

}; // class CharType

}; // namespace Microsoft.VisualBasic.CompilerServices
