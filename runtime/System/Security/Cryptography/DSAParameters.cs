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

	// Clear the contents of this structure.
	internal void Clear()
			{
				Counter = 0;
				if(G != null) { G.Initialize(); G = null; }
				if(J != null) { J.Initialize(); J = null; }
				if(P != null) { P.Initialize(); P = null; }
				if(Q != null) { Q.Initialize(); Q = null; }
				if(Seed != null) { Seed.Initialize(); Seed = null; }
				if(X != null) { X.Initialize(); X = null; }
				if(Y != null) { Y.Initialize(); Y = null; }
			}

	// Clone the public parameters in this structure.
	internal DSAParameters ClonePublic()
			{
				DSAParameters p;
				p.Counter = Counter;
				p.G = G;
				p.J = J;
				p.P = P;
				p.Q = Q;
				p.Seed = Seed;
				p.X = null;
				p.Y = Y;
				return p;
			}

}; // struct DSAParameters

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
