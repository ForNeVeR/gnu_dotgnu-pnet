// Math.cs
// A reimplementation of the orginal .NET System.Math class
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

	public sealed class Math
	{
		public const double E  = 2.7182818284590452354;
		
		public const double PI = 3.14159265358979323846;
		
		public static long Abs(long x)
		{
			return (x < 0) ? -x : x;
		}
		
		public static int Abs(int x)
		{
			return (x < 0) ? -x : x;
		}

		public static short Abs(short x)
		{
			return (short)((x < 0) ? -x : x);
		}

		public static sbyte Abs(sbyte x)
		{
			return (sbyte)((x < 0) ? -x : x);
		}

		public static decimal Abs(decimal x)
		{
			return (x < 0) ? -x : x;
		}

		public static double Abs(double x)
		{
			return (x < 0) ? -x : x;
		}

		public static float Abs(float x)
		{
			return (x < 0) ? -x : x;
		}

		public static double Acos(double x)
		{
			if (x <= -1 || x >= 1) {
				if (x == 1)
					return 0.0;
				if (x == -1)
					return PI;
				return double.NaN;
			}
			return PI / 2 - Atan(x / Sqrt(1.0 - (x * x)));
		}

		public static double Asin(double x)
		{
			if (x <= -1 || x >= 1) {
				if (x == 1)
					return PI / 2;
				if (x == -1)
					return - PI / 2;
				return double.NaN;
			}
			return Atan(x / (Sqrt(1.0 - (x * x))));
		}
		
		public static double Atan(double x)
		{
			return 0.0; // TODO
		}
		
		public static double Atan2(double y, double x)
		{	
			if (x == 0.0) {
				if (y == 0.0)
					return 0.0;
				if (y > 0.0)
					return PI / 2;
				return - PI / 2;
			}
			
			double theta = Atan(y / x);
			if (x < 0.0) {
				if (y >= 0.0)
					theta += PI; // PI/2..PI -> quadrant 2
				else
					theta -= PI; // -PI..-PI/2 -> quadrant 3
			}
			return theta;
		}
		
		public static double Ceiling(double x)
		{
			return 0.0; // TODO
		}
		
		public static double Cos(double x)
		{
			double expx = Exp(x);
			return 0.5 * (expx + 1.0 / expx);
		}
		
		public static double Cosh(double x)
		{
			return 0.5 * (Math.Exp(x) * Math.Exp(-x));
		}
		
		public static double Exp(double x)
		{
			return 0.0; // TODO
		}
		
		public static double Floor(double x)
		{
			return 0.0; // TODO
		}
		
		public static double IEEERemainder(double x, double y)
		{
			if (y == 0)
				return double.NaN;
			
			return 0.0; // TODO
		}
		
		public static double Log(double x)
		{
			if (x == double.PositiveInfinity)
				return 0;
			if (x < 0)
				return double.NaN;
			return 0.0; // TODO
		}
		
		public static double Log(double x, double tobase)
		{
			if (x == double.PositiveInfinity)
				return 0;
			if (x < 0)
				return double.NaN;
			return 0.0; // TODO
		}

		public static double Log10(double x)
		{
			return Log(x, 10d);
		}


		public static long Max(long x, long y)
		{
			return x > y ? x : y;
		}

		public static int Max(int x, int y)
		{
			return x > y ? x : y;
		}

		public static short Max(short x, short y)
		{
			return x > y ? x : y;
		}

		public static sbyte Max(sbyte x, sbyte y)
		{
			return x > y ? x : y;
		}
		
		public static ulong Max(ulong x, ulong y)
		{
			return x > y ? x : y;
		}

		public static uint Max(uint x, uint y)
		{
			return x > y ? x : y;
		}

		public static byte Max(byte x, byte y)
		{
			return x > y ? x : y;
		}
		
		public static ushort Max(ushort x, ushort y)
		{
			return x > y ? x : y;
		}

		public static decimal Max(decimal x, decimal y)
		{
			return x > y ? x : y;
		}

		public static double Max(double x, double y)
		{
			return x > y ? x : y;
		}

		public static float Max(float x, float y)
		{
			return x > y ? x : y;
		}

		public static long Min(long x, long y)
		{
			return x < y ? x : y;
		}

		public static int Min(int x, int y)
		{
			return x < y ? x : y;
		}

		public static short Min(short x, short y)
		{
			return x < y ? x : y;
		}

		public static sbyte Min(sbyte x, sbyte y)
		{
			return x < y ? x : y;
		}
		
		public static ulong Min(ulong x, ulong y)
		{
			return x < y ? x : y;
		}

		public static uint Min(uint x, uint y)
		{
			return x < y ? x : y;
		}
		
		public static byte Min(byte x, byte y)
		{
			return x < y ? x : y;
		}
		
		public static ushort Min(ushort x, ushort y)
		{
			return x < y ? x : y;
		}

		public static decimal Min(decimal x, decimal y)
		{
			return x < y ? x : y;
		}
		
		public static double Min(double x, double y)
		{
			return x < y ? x : y;
		}

		public static float Min(float x, float y)
		{
			return x < y ? x : y;
		}

		public static double Pow(double x, double y)
		{
			return 0.0; // TODO
		}

		public static decimal Round(decimal x)
		{
			return 0.0m; // TODO
		}

		public static double Round(double x)
		{
			return 0.0; // TODO
		}

		public static double Round(double x, int digits)
		{
			return 0.0; // TODO
		}
		
		public static long Sign(long x)
		{
			if (x == 0)
				return 0;
			return x < 0 ? -1 : 1;
		}

		public static int Sign(int x)
		{
			if (x == 0)
				return 0;
			return x < 0 ? -1 : 1;
		}
		
		public static short Sign(short x)
		{
			if (x == 0)
				return 0;
			return (short)(x < 0 ? -1 : 1);
		}

		public static sbyte Sign(sbyte x)
		{
			if (x == 0)
				return 0;
			return (sbyte)(x < 0 ? -1 : 1);
		}
		
		public static decimal Sign(decimal x)
		{
			if (x == 0)
				return 0;
			return x < 0 ? -1 : 1;
		}

		public static double Sign(double x)
		{
			if (x == 0)
				return 0;
			return x < 0 ? -1 : 1;
		}

		public static float Sign(float x)
		{
			if (x == 0)
				return 0;
			return x < 0 ? -1 : 1;
		}

		public static double Sin(double x)
		{
			return 0.0; // TODO
		}
		
		public static double Sinh(double x)
		{
			return 0.5 * (Math.Exp(x) - Math.Exp(-x));
		}
		
		public static double Sqrt(double x)
		{
			if (x < 0)
				return double.NaN;
			return 0.0; // TODO
		}
		
		public static double Tan(double x)
		{	
			return Sin(x) / Cos(x);
		}
		
		public static double Tanh(double x)
		{
			double expx = Exp(x);
			double expminusx = Exp(-x);
			return (expx - expminusx) / (expx + expminusx);
		}
	}
}
