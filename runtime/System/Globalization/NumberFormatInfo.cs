/*
 * NumberFormatInfo.cs - Implementation of the
 *        "System.Globalization.NumberFormatInfo" class.
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

namespace System.Globalization
{

using System;

public sealed class NumberFormatInfo : Object, ICloneable, IFormatProvider
{
	// Internal state.
	private static NumberFormatInfo invariantInfo;
	private NumberFormatInfo wrappedAround;
	private int currencyDecimalDigits;
	private String currencyDecimalSeparator;
	private int[] currencyGroupSize;
	private int[] numberGroupSize;
	private int[] percentGroupSize;
	private String currencyGroupSeparator;
	private String currencySymbol;
	private String nanSymbol;
	private int currencyPositivePattern;
	private int currencyNegativePattern;
	private int numberNegativePattern;
	private int percentPositivePattern;
	private int percentNegativePattern;
	private String positiveInfinitySymbol;
	private String negativeInfinitySymbol;
	private String positiveSign;
	private String negativeSign;
	private int numberDecimalDigits;
	private String numberDecimalSeparator;
	private String numberGroupSeparator;
	private int percentDecimalDigits;
	private String percentDecimalSeparator;
	private String percentGroupSeparator;
	private String percentSymbol;
	private String perMilleSymbol;

	// Properties.
	public static NumberFormatInfo InvariantInfo
	{
		get
		{
			if(invariantInfo == null)
			{
				invariantInfo = new NumberFormatInfo(new NumberFormatInfo());
			}
			return invariantInfo;
		}
	}
	public static NumberFormatInfo CurrentInfo
	{
		get
		{
			// TO DO: get the culture-dependent object for the current
			// thread from System.Globalization.CultureInfo.
			return InvariantInfo;
		}
	}
	public int CurrencyDecimalDigits
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.CurrencyDecimalDigits;
			}
			else
			{
				return currencyDecimalDigits;
			}
		}
		set
		{
			if(value < 0 || value > 99)
			{
				throw new ArgumentOutOfRangeException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			currencyDecimalDigits = value;
		}
	}
	public String CurrencyDecimalSeparator
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.CurrencyDecimalSeparator;
			}
			else
			{
				return currencyDecimalSeparator;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			currencyDecimalSeparator = value;
		}
	}
	public bool IsReadOnly
	{
		get
		{
			return (wrappedAround != null);
		}
	}
	public int[] CurrencyGroupSize
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.CurrencyGroupSize;
			}
			else
			{
				return currencyGroupSize;
			}
		}
		set
		{
			validateGroupSize(value);
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			currencyGroupSize = value;
		}
	}
	public int[] NumberGroupSize
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.NumberGroupSize;
			}
			else
			{
				return numberGroupSize;
			}
		}
		set
		{
			validateGroupSize(value);
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			numberGroupSize = value;
		}
	}
	public int[] PercentGroupSize
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.PercentGroupSize;
			}
			else
			{
				return percentGroupSize;
			}
		}
		set
		{
			validateGroupSize(value);
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			percentGroupSize = value;
		}
	}
	public String CurrencyGroupSeparator
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.CurrencyGroupSeparator;
			}
			else
			{
				return currencyGroupSeparator;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			currencyGroupSeparator = value;
		}
	}
	public String CurrencySymbol
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.CurrencySymbol;
			}
			else
			{
				return currencySymbol;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			currencySymbol = value;
		}
	}
	public String NaNSymbol
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.NaNSymbol;
			}
			else
			{
				return nanSymbol;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			nanSymbol = value;
		}
	}
	public int CurrencyPositivePattern
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.CurrencyPositivePattern;
			}
			else
			{
				return currencyPositivePattern;
			}
		}
		set
		{
			if(value < 0 || value > 3)
			{
				throw new ArgumentOutOfRangeException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			currencyPositivePattern = value;
		}
	}
	public int CurrencyNegativePattern
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.CurrencyNegativePattern;
			}
			else
			{
				return currencyNegativePattern;
			}
		}
		set
		{
			if(value < 0 || value > 15)
			{
				throw new ArgumentOutOfRangeException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			currencyNegativePattern = value;
		}
	}
	public int NumberNegativePattern
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.NumberNegativePattern;
			}
			else
			{
				return numberNegativePattern;
			}
		}
		set
		{
			if(value < 0 || value > 4)
			{
				throw new ArgumentOutOfRangeException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			numberNegativePattern = value;
		}
	}
	public int PercentPositivePattern
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.PercentPositivePattern;
			}
			else
			{
				return percentPositivePattern;
			}
		}
		set
		{
			if(value < 0 || value > 2)
			{
				throw new ArgumentOutOfRangeException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			percentPositivePattern = value;
		}
	}
	public int PercentNegativePattern
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.PercentNegativePattern;
			}
			else
			{
				return percentNegativePattern;
			}
		}
		set
		{
			if(value < 0 || value > 2)
			{
				throw new ArgumentOutOfRangeException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			percentNegativePattern = value;
		}
	}
	public String PositiveInfinitySymbol
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.PositiveInfinitySymbol;
			}
			else
			{
				return positiveInfinitySymbol;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			positiveInfinitySymbol = value;
		}
	}
	public String NegativeInfinitySymbol
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.NegativeInfinitySymbol;
			}
			else
			{
				return negativeInfinitySymbol;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			negativeInfinitySymbol = value;
		}
	}
	public String PositiveSign
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.PositiveSign;
			}
			else
			{
				return positiveSign;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			positiveSign = value;
		}
	}
	public String NegativeSign
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.NegativeSign;
			}
			else
			{
				return negativeSign;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			negativeSign = value;
		}
	}
	public int NumberDecimalDigits
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.NumberDecimalDigits;
			}
			else
			{
				return numberDecimalDigits;
			}
		}
		set
		{
			if(value < 0 || value > 99)
			{
				throw new ArgumentOutOfRangeException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			numberDecimalDigits = value;
		}
	}
	public String NumberDecimalSeparator
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.NumberDecimalSeparator;
			}
			else
			{
				return numberDecimalSeparator;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			numberDecimalSeparator = value;
		}
	}
	public String NumberGroupSeparator
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.NumberGroupSeparator;
			}
			else
			{
				return numberGroupSeparator;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			numberGroupSeparator = value;
		}
	}
	public int PercentDecimalDigits
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.PercentDecimalDigits;
			}
			else
			{
				return percentDecimalDigits;
			}
		}
		set
		{
			if(value < 0 || value > 99)
			{
				throw new ArgumentOutOfRangeException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			percentDecimalDigits = value;
		}
	}
	public String PercentDecimalSeparator
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.PercentDecimalSeparator;
			}
			else
			{
				return percentDecimalSeparator;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			percentDecimalSeparator = value;
		}
	}
	public String PercentGroupSeparator
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.PercentGroupSeparator;
			}
			else
			{
				return percentGroupSeparator;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			percentGroupSeparator = value;
		}
	}
	public String PercentSymbol
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.PercentSymbol;
			}
			else
			{
				return percentSymbol;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			percentSymbol = value;
		}
	}
	public String PerMilleSymbol
	{
		get
		{
			if(wrappedAround != null)
			{
				return wrappedAround.PerMilleSymbol;
			}
			else
			{
				return perMilleSymbol;
			}
		}
		set
		{
			if(value == null)
			{
				throw new ArgumentNullException();
			}
			if(wrappedAround != null)
			{
				throw new InvalidOperationException();
			}
			perMilleSymbol = value;
		}
	}

	// Construct the object with default invariant properties.
	public NumberFormatInfo()
	{
		wrappedAround = null;
		currencyDecimalDigits = 2;
		currencyDecimalSeparator = ".";
		currencyGroupSize = new int[1];
		currencyGroupSize[0] = 3;
		numberGroupSize = new int[1];
		numberGroupSize[0] = 3;
		percentGroupSize = new int[1];
		percentGroupSize[0] = 3;
		currencyGroupSeparator = ",";
		currencySymbol = "$";
		nanSymbol = "NaN";
		currencyPositivePattern = 0;
		currencyNegativePattern = 0;
		numberNegativePattern = 0;
		percentPositivePattern = 0;
		percentNegativePattern = 0;
		positiveInfinitySymbol = "Infinity";
		negativeInfinitySymbol = "-Infinity";
		positiveSign = "+";
		negativeSign = "-";
		numberDecimalDigits = 2;
		numberDecimalSeparator = ".";
		numberGroupSeparator = ",";
		percentDecimalDigits = 2;
		percentDecimalSeparator = ".";
		percentGroupSeparator = ",";
		percentSymbol = "%";
		perMilleSymbol = "\u2030";
	}

	// Construct a read-only wrapper for another number format info object.
	private NumberFormatInfo(NumberFormatInfo nfi)
	{
		wrappedAround = nfi;
	}

	// Implementation of the ICloneable interface.
	public Object Clone() { return MemberwiseClone(); }

	// Implementation of the IFormatProvider interface.
	public Object GetFormat(Type formatType)
	{
		if(formatType == typeof(System.Globalization.NumberFormatInfo))
		{
			return this;
		}
		else
		{
			return null;
		}
	}

	// Get the number format information associated with "provider".
	public static NumberFormatInfo GetInstance(IFormatProvider provider)
	{
		if(provider != null)
		{
			Object obj = provider.GetFormat
					(typeof(System.Globalization.NumberFormatInfo));
			if(obj != null)
			{
				return (NumberFormatInfo)obj;
			}
		}
		return CurrentInfo;
	}

	// Convert a number format info object into a read-only version.
	public static NumberFormatInfo ReadOnly(NumberFormatInfo nfi)
	{
		if(nfi == null)
		{
			throw new ArgumentNullException();
		}
		return new NumberFormatInfo(nfi);
	}

	// Validate a number group size array.
	private void validateGroupSize(int[] value)
	{
		if(value == null)
		{
			throw new ArgumentException();
		}
		int posn;
		for(posn = 0; posn < (value.Length - 1); ++posn)
		{
			if(value[posn] < 1 || value[posn] > 9)
			{
				throw new ArgumentException();
			}
		}
		if(value.Length < 1 || value[value.Length - 1] < 0 ||
		   value[value.Length - 1] > 9)
		{
			throw new ArgumentException();
		}
	}

}; // class NumberFormatInfo

}; // namespace System.Globalization
