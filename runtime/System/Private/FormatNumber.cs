/*
 * FormatNumber.cs - Implementation of "System.Private.FormatNumber".
 *
 * Copyright (C) 2002 Richard Baumann.
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

using System;
using System.Text;

namespace System.Private
{

internal sealed class FormatNumber
{
	private int sign = 0;
	private String whole = "0";
	private String fraction = "0";

	internal int Sign
	{
		get { return sign; }
	}
	internal String Whole
	{
		get { return whole; }
	}
	internal String Fraction
	{
		get { return fraction; }
	}



	internal FormatNumber() : base() {}
	internal FormatNumber(byte val) : this((ulong)val) {}
	internal FormatNumber(short val) : this((long)val) {}
	internal FormatNumber(int val) : this((long)val) {}
	internal FormatNumber(long val) : base()
	{
		sign = Math.Sign(val); //((val > 0) ? 1 : ((val < 0) ? -1 : 0));
		if (sign != 0)
		{
			StringBuilder s = new StringBuilder();
			for (; val != 0; val /= 10)
			{
				s.Insert(0,(char)((val % 10) + '0'));
			}
			whole = s.ToString();
			fraction = "0";
		}
	}
	internal FormatNumber(sbyte val) : this((long)val) {}
	internal FormatNumber(ushort val) : this((ulong)val) {}
	internal FormatNumber(uint val) : this((ulong)val) {}
	internal FormatNumber(ulong val) : base()
	{
		sign = ((val > 0) ? 1 : 0);
		if (sign != 0)
		{
			StringBuilder s = new StringBuilder();
			for (; val != 0; val /= 10)
			{
				s.Insert(0,(char)((val % 10) + '0'));
			}
			whole = s.ToString();
			fraction = "0";
		}
	}
	internal FormatNumber(float val) : this((double)val,false,true) {}
	internal FormatNumber(float val, bool round_trip) : this((double)val,round_trip,true) {}
	internal FormatNumber(double val) : this(val,false,false) {}
	internal FormatNumber(double val, bool round_trip) : this(val,round_trip,false) {}
	internal FormatNumber(double val, bool round_trip, bool is_float) : base()
	{
		int maxlen = (is_float ? 7 : 15) + (round_trip ? 2 : 0);
		sign = Math.Sign(val);
		if (sign != 0)
		{
			double w = Math.Floor(val);
			double f = val - whole;
			double tmp;
			StringBuilder wh = new StringBuilder();
			StringBuilder fr = new StringBuilder();
			while (w != 0)
			{
				tmp = Math.IEEERemainder(w,10.0);
				if (tmp == 9.0)
				{
					wh.Insert(0,"9");
					w -= 9.0;
					w /= 10.0;
				}
				else if (tmp == 8.0)
				{
					wh.Insert(0,"8");
					w -= 8.0;
					w /= 10.0;
				}
				else if (tmp == 7.0)
				{
					wh.Insert(0,"7");
					w -= 7.0;
					w /= 10.0;
				}
				else if (tmp == 6.0)
				{
					wh.Insert(0,"6");
					w -= 6.0;
					w /= 10.0;
				}
				else if (tmp == 5.0)
				{
					wh.Insert(0,"5");
					w -= 5.0;
					w /= 10.0;
				}
				else if (tmp == 4.0)
				{
					wh.Insert(0,"4");
					w -= 4.0;
					w /= 10.0;
				}
				else if (tmp == 3.0)
				{
					wh.Insert(0,"3");
					w -= 3.0;
					w /= 10.0;
				}
				else if (tmp == 2.0)
				{
					wh.Insert(0,"2");
					w -= 2.0;
					w /= 10.0;
				}
				else if (tmp == 1.0)
				{
					wh.Insert(0,"1");
					w -= 1.0;
					w /= 10.0;
				}
				else if (tmp == 0.0)
				{
					wh.Insert(0,"0");
					w -= 0.0;
					w /= 10.0;
				}
				else
				{
					throw new ArgumentException("FIXME: FormatNumber(double,bool,bool)");
				}
			}
			whole = wh.ToString();
			maxlen -= whole.Length;
			int fcount = 0;
			while (Math.Floor(f) != f && fcount <= maxlen)
			{
				f *= 10.0;
				fcount++;
			}
			f = Math.Floor(f);
			while (f != 0)
			{
				fcount--;
				tmp = Math.IEEERemainder(f,10.0);
				if (tmp == 9.0)
				{
					fr.Insert(0,"9");
					f -= 9.0;
					f /= 10.0;
				}
				else if (tmp == 8.0)
				{
					fr.Insert(0,"8");
					f -= 8.0;
					f /= 10.0;
				}
				else if (tmp == 7.0)
				{
					fr.Insert(0,"7");
					f -= 7.0;
					f /= 10.0;
				}
				else if (tmp == 6.0)
				{
					fr.Insert(0,"6");
					f -= 6.0;
					f /= 10.0;
				}
				else if (tmp == 5.0)
				{
					fr.Insert(0,"5");
					f -= 5.0;
					f /= 10.0;
				}
				else if (tmp == 4.0)
				{
					fr.Insert(0,"4");
					f -= 4.0;
					f /= 10.0;
				}
				else if (tmp == 3.0)
				{
					fr.Insert(0,"3");
					f -= 3.0;
					f /= 10.0;
				}
				else if (tmp == 2.0)
				{
					fr.Insert(0,"2");
					f -= 2.0;
					f /= 10.0;
				}
				else if (tmp == 1.0)
				{
					fr.Insert(0,"1");
					f -= 1.0;
					f /= 10.0;
				}
				else if (tmp == 0.0)
				{
					fr.Insert(0,"0");
					f -= 0.0;
					f /= 10.0;
				}
				else
				{
					throw new ArgumentException("FIXME: FormatNumber(double,bool,bool)");
				}
			}
			while (fcount > 0)
			{
				fcount--;
				fr.Insert(0,"0");
			}
			fraction = fr.ToString();
		}
	}
	internal FormatNumber(Decimal val) : base()
	{
		sign = Math.Sign(val);
		if (sign != 0)
		{
			int[] bits = Decimal.GetBits(val);
			int scale = (bits[3] & 0x00FF0000) >> 16;
			bits[3] = 0;
			StringBuilder s = new StringBuilder();
			for (val = new Decimal(bits); val != 0; val /= 10)
			{
				s.Insert(0,(char)((val % 10)+'0'));
			}
			while (scale > s.Length)
			{
				s.Insert(0,"0");
			}
			whole = (scale == s.Length ? "0" : s.ToString(0,s.Length - scale));
			fraction = (scale == 0 ? "0": s.ToString(s.Length - scale,scale));
		}
	}
	internal FormatNumber(FormatNumber num) : base()
	{
		if (num != null)
		{
			this.sign = num.sign;
			this.whole = num.whole;
			this.fraction = num.fraction;
		}
	}

	// returns num / (10^factor)
	internal FormatNumber Scale(int factor) { return Scale(factor, true); }
	internal FormatNumber Scale(int factor, bool copy)
	{
		FormatNumber num;
		if (copy)
		{
			num = new FormatNumber(this);
		}
		else
		{
			num = this;
		}
		if (factor == 0 || sign == 0) {}
		else if (factor > 0)
		{
			StringBuilder w;
			for (w = new StringBuilder(num.whole); factor >= w.Length; w.Insert(0,"0")) {} // XXX.xxx -> 0000XXX.xxx
			num.whole = w.ToString();
			num.fraction = num.whole.Substring(factor,num.whole.Length - factor) + num.fraction; // 0000XXX.xxx -> 0000XXX.000XXXxxx
			num.whole = num.whole.Substring(0,factor); // 0000XXX.000XXXxxx -> 0.000XXXxxx
		}
		else
		{
			factor = -(factor); // this shouldn't be a problem... Strings can't be >= abs(int.MinValue) in length
			StringBuilder f;
			for (f = new StringBuilder(num.fraction); factor >= f.Length; f.Insert(f.Length,"0")) {} // XXX.xxx -> XXX.xxx0000
			num.fraction = f.ToString();
			num.whole = num.whole + num.fraction.Substring(0,factor); // XXX.xxx0000 -> XXXxxx000.xxx0000
			num.fraction = num.fraction.Substring(factor,num.fraction.Length - factor); // XXXxxx000.xxx0000 -> XXXxxx000.0
		}
		num.Trim();
		return num;
	}

	internal FormatNumber Round(int index, bool beforeDot) { return Round(index,beforeDot,true); }
	internal FormatNumber Round(int index, bool beforeDot, bool copy)
	{
		FormatNumber num;
		if (copy)
		{
			num = new FormatNumber(this);
		}
		else
		{
			num = this;
		}
		if (num.sign == 0) { return num; }
		if (beforeDot) // only for engineering format
		{
			if (index >= num.whole.Length)
			{
				throw new ArgumentException("index >= Whole.Length");
			}
			if (num.whole[index] >= '5')
			{
				int i; // index of last non-nine digit above rounding point
				for (i = index - 1; i >= 0 && num.whole[i] == '9'; i--) {}
				StringBuilder s = new StringBuilder(num.whole);
				if (i < 0)
				{
					s.Insert(0,"0"); // 999XXX.xxx -> 0999XXX.xxx
					i = 0; // adjust index to point to prepended '0' digit
				}
				s[i] = (char)(s[i] + 1); // (M)999XXX.xxx -> (N)999XXX.xxx
				i++; // move to next digit
				while (i < s.Length)
				{
					s[i] = '0'; // (N)999XXX.xxx -> (N)000000.xxx (N == all digits above rounding point)
					i++; // move to next digit
				}
				num.whole = s.ToString();
				num.fraction = "0"; // (N)000000.xxx -> (N)000000.0
			}
			else
			{
				char[] chars = num.whole.ToCharArray();
				while (index < chars.Length)
				{
					chars[index] = '0'; // (N)XXX.xxx -> (N)000.xxx
					index++;
				}
				num.whole = new String(chars);
				num.fraction = "0"; // (N)000.xxx -> (N)000.0
			}
		}
		else // for either fixed-point or engineering format
		{
			if (index >= num.fraction.Length)
			{
				throw new ArgumentException("index >= Fraction.Length");
			}
			if (num.fraction[index] >= '5')
			{
				int i; // index of last non-nine digit above rounding point
				for (i = index - 1; i >= 0 && num.fraction[i] == '9'; i--) {}
				if (i < 0)
				{
					num.fraction = "0"; // (M).999xxx -> (M).0

					// (M).0 -> (N).0
					for (i = num.whole.Length - 1; i >= 0 && num.whole[i] == '9'; i--) {}
					if (i < 0)
					{
						num.whole = "0" + num.whole; // 999.0 -> 0999.0
						i = 0; // adjust index to point to prepended '0' digit
					}
					char[] chars = num.whole.ToCharArray();
					chars[i] = (char)(chars[i] + 1); // (M)999.0 -> (N)999.0
					i++; // move to next digit
					while (i < chars.Length)
					{
						chars[i] = '0'; // (N)999.0 -> (N)000.0
						i++; // move to next digit
					}
					num.whole = new String(chars);
				}
				else // if (i >= 0)
				{
					StringBuilder s = new StringBuilder(num.fraction);
					s[i] = (char)(s[i] + 1); // (N).(m)999 -> (N).(n)999
					i++; // move to next digit
					s.Length = i; // (N).(n)999 -> (N).(n)... only keep significant digits above rounding point
					num.fraction = s.ToString();
				}
			}
			else
			{
				int i; // index of last non-zero digit above rounding point
				for (i = index - 1; i >= 0 && num.fraction[i] == '0'; i--) {}
				StringBuilder s = new StringBuilder(num.fraction);
				i++;
				s.Length = i; // (N).(n)000 -> (N).(n)... only keep significant digits above rounding point
				num.fraction = s.ToString();
			}
		}
		num.Trim();
		if (IsZero(num.fraction) && IsZero(num.whole))
		{
			num.sign = 0;
		}
		return num;
	}

	internal FormatNumber SciForm(ref int exp) { return SciForm(ref exp,true); }
	internal FormatNumber SciForm(ref int exp, bool copy)
	{
		FormatNumber num;
		if (copy)
		{
			num = new FormatNumber(this);
		}
		else
		{
			num = this;
		}

		exp = 0;

		if (num.sign == 0)
		{
			return num;
		}

		int i; // index of first non-zero digit in num
		for (i = 0; i < num.whole.Length && num.whole[i] == '0'; i++) {}
		if (i == num.whole.Length)
		{
			for (i = 0; i < num.fraction.Length && num.fraction[i] == '0'; i++) {}
			if (i == num.fraction.Length)
			{
				num.Trim(); // 000.000 -> 0.0
				num.sign = 0;
				return num;
			}
			whole = num.fraction[i].ToString(); // 0.Xxx -> X.Xxx
			if (i+1 == num.fraction.Length)
			{
				fraction = "0"; // X.Xxx -> X.0
			}
			else
			{
				num.fraction = num.fraction.Substring(i+1); // X.Xxx -> X.xx
			}
			exp = -(i+1); // eg. 0.1234 -> 1.234e-1
			return num;
		}
		if (num.whole.Length == 1)
		{
			return num; // X.xxx -> X.xxx
		}
		// Xxx.x -> X.xxx
		char tmp = num.whole[i]; // Xxx.x ... X --> tmp
		whole = whole.Substring(i+1); // Xxx.x -> xx.x
		exp = whole.Length; // eg. 4321 -> 4.321e+3
		fraction = whole + fraction; // xx.x -> xx.xxx
		whole = tmp.ToString(); // xx.xxx -> X.xxx
		num.Trim();
		return num;
	}

	private static bool IsZero(String s)
	{
		int i;
		for (i = 0; i < s.Length && s[i] == '0'; i++) {}
		return (i == s.Length);
	}
	private void Trim()
	{
		int i; // index of first non-zero digit in whole and last non-zero digit in fraction
		for (i = 0; i < whole.Length && whole[i] == '0'; i++) {}
		if (i == whole.Length)
		{
			whole = "0"; // 000.xxx -> 0.xxx
		}
		else
		{
			whole = whole.Substring(i,whole.Length - i); // 000XXX.xxx -> XXX.xxx
		}
		for (i = fraction.Length; i >= 0 && fraction[i] == '0'; i--) {}
		if (i < 0)
		{
			fraction = "0"; // XXX.000 -> XXX.0
		}
		else
		{
			fraction = fraction.Substring(0,i); // XXX.xxx000 -> XXX.xxx
		}
	}

}; // class FormatNumber

}; // namespace System.Private
