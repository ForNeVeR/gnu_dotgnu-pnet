/*
 * ParameterToken.cs - Implementation of the
 *			"System.Reflection.Emit.ParameterToken" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System.Reflection.Emit
{

#if !ECMA_COMPAT

public struct ParameterToken
{
	// The empty token.
	public static readonly ParameterToken Empty;

	// Internal state.
	private int token;

	// Get the token code.
	public int Token
			{
				get
				{
					return token;
				}
			}

	// Determine if this token is identical to another.
	public override bool Equals(Object obj)
			{
				if(obj is ParameterToken)
				{
					return (token == ((ParameterToken)obj).token);
				}
				else
				{
					return false;
				}
			}

	// Get a hash code for this token.
	public override int GetHashCode()
			{
				return token;
			}

}; // struct ParameterToken

#endif // !ECMA_COMPAT

}; // namespace System.Reflection.Emit