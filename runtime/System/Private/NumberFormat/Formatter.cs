/*
 * Formatter.cs - Implementation of the
 *          "System.Private.NumberFormat.Formatter" class.
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

namespace System.Private.NumberFormat
{

using System;
using System.Collections;
using System.Globalization;
using System.Text;

internal abstract class Formatter
{
	// ----------------------------------------------------------------------
	//  Private static variables for formatter
	//
	private const string validformats = "CcDdEeFfGgNnPpRrXx";
	static private IDictionary formats = new Hashtable();

	// ----------------------------------------------------------------------
	//  Protected data for other methods
	//
	static protected readonly char[] decimalDigits =
	{'0','1','2','3','4','5','6','7','8','9'};

	// ----------------------------------------------------------------------
	//  Protected state information
	//
	protected int precision;

	// ----------------------------------------------------------------------
	//  Protected utility methods
	//
	static protected NumberFormatInfo 
		NumberFormatInfo(IFormatProvider provider)
	{
		if(provider == null)
		{
			return System.Globalization.NumberFormatInfo.CurrentInfo;
		}
		else
		{
			NumberFormatInfo nfi =
				(NumberFormatInfo) provider.GetFormat(
								typeof(System.Globalization.NumberFormatInfo));
			if(nfi != null)
			{
				return nfi;
			}
			else
			{
				return System.Globalization.NumberFormatInfo.CurrentInfo;
			}
		}
	}

	static protected bool IsSignedInt(Object o)
	{
		return (o is SByte || o is Int16 || o is Int32 || o is Int64);
	}
	
	static protected bool IsUnsignedInt(Object o)
	{
		return (o is Byte || o is UInt16 || o is UInt32 || o is UInt64);
	}
	
#if CONFIG_EXTENDED_NUMERICS
	static protected bool IsFloat(Object o)
	{
		return (o is Single || o is Double);
	}
	
	static protected bool IsDecimal(Object o)
	{
		return (o is Decimal);
	}

	static protected double OToDouble(Object o)
	{
		double ret;

		if (o is Int32)
		{
			Int32 n = (Int32)o;
			ret = (double)n;
		}
		else if (o is UInt32)
		{
			UInt32 n = (UInt32)o;
			ret = (double)n;
		}
		else if (o is SByte)
		{
			SByte n = (SByte)o;
			ret = (double)n;
		}
		else if (o is Byte)
		{
			Byte n = (Byte)o;
			ret = (double)n;
		}
		else if (o is Int16)
		{
			Int16 n = (Int16)o;
			ret = (double)n;
		}
		else if (o is UInt16)
		{
			UInt16 n = (UInt16)o;
			ret = (double)n;
		}
		else if (o is Int64)
		{
			Int64 n = (Int64)o;
			ret = (double)n;
		}
		else if (o is UInt64)
		{
			UInt64 n = (UInt64)o;
			ret = (double)n;
		}
		else if (o is Single)
		{
			Single n = (Single)o;
			ret = (double)n;
		}
		else if (o is Double)
		{
			ret = (double)o;
		}
		else if (o is Decimal)
		{
			Decimal n = (Decimal)o;
			ret = (double)n;
		}
		else
		{
			throw new FormatException(_("Format_TypeException"));
		}
		return ret;
	}
#endif // CONFIG_EXTENDED_NUMERICS

	static protected ulong OToUlong(Object o)
	{
		ulong ret;

		if (o is Int32)
		{
			Int32 n = (Int32)o;
			ret = (ulong)n;
		}
		else if (o is UInt32)
		{
			UInt32 n = (UInt32)o;
			ret = (ulong)n;
		}
		else if (o is SByte)
		{
			SByte n = (SByte)o;
			ret = (ulong)n;
		}
		else if (o is Byte)
		{
			Byte n = (Byte)o;
			ret = (ulong)n;
		}
		else if (o is Int16)
		{
			Int16 n = (Int16)o;
			ret = (ulong)n;
		}
		else if (o is UInt16)
		{
			UInt16 n = (UInt16)o;
			ret = (ulong)n;
		}
		else if (o is Int64)
		{
			Int64 n = (Int64)o;
			ret = (ulong)n;
		}
		else if (o is UInt64)
		{
			ret = (ulong)o;
		}
#if CONFIG_EXTENDED_NUMERICS
		else if (o is Single)
		{
			Single n = (Single)o;
			ret = (ulong)n;
		}
		else if (o is Double)
		{
			Double n = (Double)o;
			ret = (ulong)n;
		}
		else if (o is Decimal)
		{
			Decimal n = (Decimal)o;
			ret = (ulong)n;
		}
#endif
		else
		{
			throw new FormatException(_("Format_TypeException"));
		}
		return ret;
	}

	static protected long OToLong(Object o)
	{
		long ret;

		if (o is Int32)
		{
			Int32 n = (Int32)o;
			ret = (long)n;
		}
		else if (o is UInt32)
		{
			UInt32 n = (UInt32)o;
			ret = (long)n;
		}
		else if (o is SByte)
		{
			SByte n = (SByte)o;
			ret = (long)n;
		}
		else if (o is Byte)
		{
			Byte n = (Byte)o;
			ret = (long)n;
		}
		else if (o is Int16)
		{
			Int16 n = (Int16)o;
			ret = (long)n;
		}
		else if (o is UInt16)
		{
			UInt16 n = (UInt16)o;
			ret = (long)n;
		}
		else if (o is Int64)
		{
			ret = (long)o;
		}
		else if (o is UInt64)
		{
			UInt64 n = (UInt64)o;
			ret = (long)n;
		}
#if CONFIG_EXTENDED_NUMERICS
		else if (o is Single)
		{
			Single n = (Single)o;
			ret = (long)n;
		}
		else if (o is Double)
		{
			Double n = (Double)o;
			ret = (long)n;
		}
		else if (o is Decimal)
		{
			Decimal n = (Decimal)o;
			ret = (long)n;
		}
#endif
		else
		{
			throw new FormatException(_("Format_TypeException"));
		}
		return ret;
	}

	static protected string FormatAnyRound(Object o, int precision,
										   IFormatProvider provider)
	{
		string ret;

		//  Type validation
		if (IsSignedInt(o) && (OToLong(o) < 0) )
		{
			ulong value=(ulong) -OToLong(o);

			if(value==0)
			{
				// because -(Int64.MinValue) does not exist
				ret = "-9223372036854775808.";
			}
			else
			{
				ret = "-" + Formatter.FormatInteger(value);
			}
		}
		else if (IsSignedInt(o) || IsUnsignedInt(o))
		{
			ret = Formatter.FormatInteger(OToUlong(o));
		}
#if CONFIG_EXTENDED_NUMERICS
		else if (IsDecimal(o))
		{
			// Rounding code
			decimal r;
			int i;

			for (i=0, r=0.5m; i < precision; i++) 
			{
				r *= 0.1m;
			}

			//  Pick the call based on the inputs negativity.
			if ((decimal)o < 0)
			{
				ret = "-" + Formatter.FormatDecimal(-((decimal)o)+r);
			}
			else
			{
				ret = Formatter.FormatDecimal((decimal)o+r);
			}

			if (ret.Length - ret.IndexOf('.') > precision + 1) 
			{
				ret = ret.Substring(0, ret.IndexOf('.')+precision+1);
			}
		}
		else if (IsFloat(o))
		{
			// Beware rounding code
			double val = OToDouble(o);
			if (Double.IsNaN(val))
			{
				return NumberFormatInfo(provider).NaNSymbol;
			}
			else if (Double.IsPositiveInfinity(val))
			{
				return NumberFormatInfo(provider).PositiveInfinitySymbol;
			}
			else if (Double.IsNegativeInfinity(val))
			{
				return NumberFormatInfo(provider).NegativeInfinitySymbol;
			}
			else if (val < 0)
			{
				ret = "-" + 
					Formatter.FormatFloat(
							-val + 5 * Math.Pow(10, -precision - 1)
							,precision);
			}
			else
			{
				ret = Formatter.FormatFloat(
						val + 5 * Math.Pow(10, -precision - 1) 
						,precision);
			}
		}
#endif
		else
		{
			//  This is a bad place to be.
			throw new FormatException(_("Format_TypeException"));	
		}
		return ret;
	}

	static protected string FormatInteger(ulong value)
	{
		//  Note:  CustomFormatter counts on having the trailing decimal
		//  point.  If you're considering taking it out, think hard.
		
		if (value == 0) return ".";

		StringBuilder ret = new StringBuilder(".");
		ulong work = value;

		while (work > 0) {
			ret.Insert(0, decimalDigits[work % 10]);
			work /= 10;
		}

		return ret.ToString();
	}

#if CONFIG_EXTENDED_NUMERICS
	static protected string FormatDecimal(decimal value)
	{
		//  Guard clause(s)
		if (value == 0.0m) return ".";

		//  Variable declarations
		int [] bits = Decimal.GetBits(value);
	    int scale = (bits[3] >> 16) & 0xff;
		decimal work = value;
		StringBuilder ret = new StringBuilder();

		//  Scale the decimal
		for (int i = 0; i<scale; i++) work *= 10.0m;
	   
		//  Pick off one digit at a time
		while (work > 0.0m) 
		{
			ret.Insert(0, decimalDigits[
					(int)(work - Decimal.Truncate(work*0.1m)*10.0m)]);
			work = Decimal.Truncate(work * 0.1m);
		}

		//  Pad out significant digits
		if (ret.Length < scale) 
		{
			ret.Insert(0, "0", scale - ret.Length);
		}

		//  Insert a decimal point
		ret.Insert(ret.Length - scale, '.');

		return ret.ToString();
	}

	static protected string FormatFloat(double value, int precision)
	{
		if (value == 0.0) return ".";

		//  
		int exponent = (int)Math.Floor(Math.Log10(Math.Abs(value)));
		double work = value * Math.Pow(10, 16 - exponent);
		
		//
		//  Build a numeric representation, sans decimal point.
		//
		StringBuilder sb = 
			new StringBuilder(FormatInteger((ulong)Math.Floor(work)));
		sb.Remove(sb.Length-1, 1);    // Ditch the trailing decimal point

		if (sb.Length > precision + exponent + 1)
		{
			sb.Remove(precision+exponent+1, sb.Length-(precision+exponent+1));
		}	

		//
		//  Cases for reinserting the decimal point.
		//
		if (exponent >= -1 && exponent < sb.Length)
		{
			sb.Insert(exponent+1,'.');
		}
		else if (exponent < -1)
		{
			sb.Insert(0,new String('0',-exponent - 1));
			sb.Insert(0,".");
		}
		else 
		{
			sb.Append(new String('0', exponent - sb.Length + 1));
			sb.Append('.');
		}

		//
		//  Remove trailing zeroes.
		//
		while (sb[sb.Length-1] == '0') {
			sb.Remove(sb.Length-1, 1);
		}

		return sb.ToString();
	}
#endif // CONFIG_EXTENDED_NUMERICS

	static protected string GroupInteger(string value, int[] groupSizes,
											string separator)
	{
		if (value == String.Empty) return "0";

		int vindex = value.Length;
		int i = 0;
		StringBuilder ret = new StringBuilder();

		while (vindex > 0)
		{
			if (vindex - groupSizes[i] <= 0 || groupSizes[i] == 0)
			{
				ret.Insert(0, value.Substring(0, vindex));
				vindex = 0;
			}
			else
			{
				vindex -= groupSizes[i];
				ret.Insert(0, value.Substring(vindex, groupSizes[i]));
				ret.Insert(0, separator);
			}
			if (i < groupSizes.Length-1) ++i;
		}

		return ret.ToString();
	}
	
	// ----------------------------------------------------------------------
	//  Public interface
	//

	//
	//  Factory/Singleton method
	//
	public static Formatter CreateFormatter(String format)
	{
		return CreateFormatter(format, null);
	}

	public static Formatter CreateFormatter(String format, IFormatProvider p)
	{
		int precision;
		Formatter ret;

		if (format == null)
		{
			throw new FormatException(_("Format_StringException"));
		}

		// handle empty format
		if(format.Length == 0)
		{
			return new CustomFormatter(format);
		}

		//  Search for cached formats
		if (formats[format] != null)
		{
			return (Formatter) formats[format];
		}

		// Validate the format.  
		// It should be of the form 'X', 'X9', or 'X99'.
		// If it's not, return a CustomFormatter.
		if (validformats.IndexOf(format[0]) == -1 || format.Length > 3)
		{
			return new CustomFormatter(format);
		}

		try 
		{
			precision = (format.Length == 1) ? 
				-1 : Byte.Parse(format.Substring(1));
		}
		catch (FormatException)
		{
			return new CustomFormatter(format);
		}
		
		switch(format[0])	// There's always a yucky switch somewhere
		{
		case 'C':
		case 'c':
			ret = new CurrencyFormatter(precision);
			break;
		
		case 'D':
		case 'd':
			ret = new DecimalFormatter(precision);
			break;
		
		case 'E':
		case 'e':
			ret = new ScientificFormatter(precision, format[0]);
			break;
		
		case 'F':
		case 'f':
			ret = new FixedPointFormatter(precision);
			break;

		case 'G':
		case 'g':
			ret = new GeneralFormatter(precision, format[0]);
			break;

		case 'N':
		case 'n':
			ret = new System.Private.NumberFormat.NumberFormatter(precision);
			break;

		case 'P':
		case 'p':
			ret = new PercentFormatter(precision);
			break;
		
		case 'R':
		case 'r':
			ret = new RoundTripFormatter(precision);
			break;

		case 'X':
		case 'x':
			ret = new HexadecimalFormatter(precision, format[0]);
			break;

		default:
			ret = new CustomFormatter(format);
			break;
		}
		return ret;
	}

	//
	//  Public access method.
	//
	public abstract string Format(Object o, IFormatProvider provider);

} // class Formatter

} // namespace System.Private.Format

