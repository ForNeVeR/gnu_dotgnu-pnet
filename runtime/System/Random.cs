// Random.cs
// A reimplementation of the orginal .NET System.Random class
// Copyright (C) 2001 Mike Krueger
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

namespace System {

	public class Random
	{
		int seed;
	
		const int multiplier = 99991;
		const int addend     = 0xFE;
		
		public Random()
		{
		}
		
		public Random(int seed)
		{
			this.seed = seed;
		}
		
		public virtual int Next()
		{
			seed = seed * multiplier + addend;
			return seed & 0x7FFFFFFF;
		}
		
		public virtual int Next(int maxvalue)
		{
			if (maxvalue < 0)
				throw new ArgumentOutOfRangeException("maxvalue can't be negative");
			return Next() % maxvalue;
		}
		
		public virtual int Next(int minvalue, int maxvalue)
		{
			if (maxvalue < minvalue)
				throw new ArgumentOutOfRangeException("maxvalue is lower than minvalue");
			return minvalue + Next() % (maxvalue - minvalue);
		}
		
		public virtual void NextBytes(byte[] buffer)
		{
			if (buffer == null)
				throw new ArgumentNullException("buffer can't be a null reference");
			
			for (int i = 0; i < buffer.Length; ++i)
				buffer[i] = (byte)Next();
		}
		
		public virtual double NextDouble()
		{
			return Sample();
		}
		
		protected virtual double Sample()
		{
			return (double)((uint)Next()) / (double)uint.MaxValue;
		}
	}
}
