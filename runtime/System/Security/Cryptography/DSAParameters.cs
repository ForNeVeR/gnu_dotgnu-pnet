/*
 * DSAParameters.cs - Implementation of the
 *		"System.Security.Cryptography.DSAParameters" structure.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.Security.Cryptography
{

#if !ECMA_COMPAT

using System;

public struct DSAParameters
{
	public int Counter;
	public byte[] G;
	public byte[] J;
	public byte[] P;
	public byte[] Q;
	public byte[] Seed;
	[NonSerialized] public byte[] X;
	public byte[] Y;

}; // struct DSAParameters

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
