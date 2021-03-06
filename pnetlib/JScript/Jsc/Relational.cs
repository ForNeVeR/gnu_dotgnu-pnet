/*
 * Relational.cs - Relational operators.
 *
 * Copyright (C) 2003 Southern Storm Software, Pty Ltd.
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
 
namespace Microsoft.JScript
{

using System;
using System.Reflection;

// Dummy class for backwards-compatibility.

public class Relational : BinaryOp
{
	// Constructor.
	public Relational(int operatorTok)
			: base(operatorTok)
			{
				// Nothing to do here.
			}

	// Evaluate a relational operator on two values.
	public double EvaluateRelational(Object v1, Object v2)
			{
				return JScriptCompare(v1, v2);
			}

	// Compare two values.
	public static double JScriptCompare(Object v1, Object v2)
			{
				v1 = Convert.ToPrimitive(v1, DefaultValueHint.None);
				v2 = Convert.ToPrimitive(v2, DefaultValueHint.None);
				if(v1 is String && v2 is String)
				{
					return String.CompareOrdinal((String)v1, (String)v2);
				}
				else
				{
					return (Convert.ToNumber(v1) - Convert.ToNumber(v2));
				}
			}

}; // class Relational

}; // namespace Microsoft.JScript
