/*
 * RSAParameters.cs - Implementation of the
 *		"System.Security.Cryptography.RSAParameters" structure.
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

public struct RSAParameters
{
	public byte[] Exponent;
	public byte[] Modulus;
	[NonSerialized] public byte[] D;
	[NonSerialized] public byte[] DP;
	[NonSerialized] public byte[] DQ;
	[NonSerialized] public byte[] InverseQ;
	[NonSerialized] public byte[] P;
	[NonSerialized] public byte[] Q;

	// Clear the contents of this structure.
	internal void Clear()
			{
				if(Exponent != null) { Exponent.Initialize(); Exponent = null; }
				if(Modulus != null) { Modulus.Initialize(); Modulus = null; }
				if(D != null) { D.Initialize(); D = null; }
				if(DP != null) { DP.Initialize(); DP = null; }
				if(DQ != null) { DQ.Initialize(); DQ = null; }
				if(InverseQ != null) { InverseQ.Initialize(); InverseQ = null; }
				if(P != null) { P.Initialize(); P = null; }
				if(Q != null) { Q.Initialize(); Q = null; }
			}

	// Clone the public parameters in this structure.
	internal RSAParameters ClonePublic()
			{
				RSAParameters p;
				p.Exponent = Exponent;
				p.Modulus = Modulus;
				p.D = null;
				p.DP = null;
				p.DQ = null;
				p.InverseQ = null;
				p.P = null;
				p.Q = null;
				return p;
			}

}; // struct RSAParameters

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
