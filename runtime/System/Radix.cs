// Radix.cs
// A reimplementation of the orginal .NET System.Radix class
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
	
	public class Radix
	{
		const string DIGITTABLE = "0123456789abcdefghijklmnopqrstuvwxyz";
		
		public const int MinRadix = 2;
		public const int MaxRadix = 36;
		
		public static char ForDigit(int val, int radix)
		{
			if (radix < MinRadix || radix > MaxRadix)
				throw new ArgumentOutOfRangeException("radix must be between MinRadix and MaxRadix");
			if (val   < 0        || val   > radix)
				throw new ArgumentOutOfRangeException("val must be between 0 and radix");
			return DIGITTABLE[val];
		}
		
		public static bool IsDigit(char c, int radix)
		{
			for (int i = 0; i < radix; ++i)
				if (DIGITTABLE[i] == c)
					return true;
			return false;
		}
		
		public static string ToString(long val, int radix)
		{
			if (radix < MinRadix || radix > MaxRadix)
				throw new ArgumentOutOfRangeException("radix must be between MinRadix and MaxRadix");
			
			string prefix = "";
			string digit  = "";
			switch (radix) {
				case 8:
					prefix += "0";
					break;
				case 10:
					break;
				case 16:
					prefix += "0x";
					break;
				default:
					prefix += radix.ToString() + "#";
					if (prefix.Length == 2)
						prefix = "0" + prefix;
					break;
			}
			
			if (val == 0)
				return prefix + "0";
			
			if (radix == 8 || radix == 16) {
				ulong uval   = (ulong)val;
				uint uradix = (uint)radix;
				while (uval > 0) {
					digit = DIGITTABLE[(int)(uval % uradix)] + digit;
					uval /= uradix;
				}
			} else {
				if (val < 0) {
					prefix = "-" + prefix;
					val *= -1;
				}
				while (val > 0) {
					digit = DIGITTABLE[(int)(val % radix)] + digit;
					val /= radix;
				}
			}
			
			return prefix + digit;
		}
	}
}
