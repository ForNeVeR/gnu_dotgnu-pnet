/*
 * TimeSpan.cs - Implementation of the "System.TimeSpan" class.
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

namespace System
{

public struct TimeSpan : IComparable
{
	internal long value__;

	// Public constants.
	public static readonly TimeSpan MinValue =
			new TimeSpan(unchecked((long)0x8000000000000000));
	public static readonly TimeSpan MaxValue =
			new TimeSpan(unchecked((long)0x7FFFFFFFFFFFFFFF));
	public static readonly TimeSpan Zero = new TimeSpan(0);
	public const long TicksPerMillisecond = 10000L;
	public const long TicksPerSecond = 10000L * 1000L;
	public const long TicksPerMinute = 10000L * 1000L * 60L;
	public const long TicksPerHour = 10000L * 1000L * 60L * 60L;
	public const long TicksPerDay = 10000L * 1000L * 60L * 60L * 24L;

	// Constructors.
	public TimeSpan(long ticks)
			{
				value__ = ticks;
			}
	public TimeSpan(int hours, int minutes, int seconds)
			: this(0, hours, minutes, seconds, 0) {}
	public TimeSpan(int days, int hours, int minutes, int seconds)
			: this(days, hours, minutes, seconds, 0) {}
	public TimeSpan(int days, int hours, int minutes, int seconds,
					int milliseconds)
			{
				Decimal result;
				try
				{
					result = ((Decimal)days) * ((Decimal)TicksPerDay) +
							 ((Decimal)hours) * ((Decimal)TicksPerHour) +
							 ((Decimal)minutes) * ((Decimal)TicksPerMinute) +
							 ((Decimal)seconds) * ((Decimal)TicksPerSecond) +
							 ((Decimal)milliseconds) * 10000.0m;
					value__ = Decimal.ToInt64(result);
				}
				catch(OverflowException)
				{
					throw new ArgumentOutOfRangeException
						(_("ArgRange_TimeSpan"));
				}
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is TimeSpan))
					{
						throw new ArgumentException(_("Arg_MustBeTimeSpan"));
					}
					long value2 = ((TimeSpan)value).value__;
					if(value__ < value2)
					{
						return -1;
					}
					else if(value__ > value2)
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 1;
				}
			}

	// Convert an integer value into two digits.
	private static String TwoDigits(int value)
			{
				char[] result = new char[2];
				result[0] = unchecked((char)((value / 10) + (int)'0'));
				result[1] = unchecked((char)((value % 10) + (int)'0'));
				return new String(result);
			}

	// Formatting.
	public override String ToString()
			{
				int days = Days;
				int hours = Hours;
				int minutes = Minutes;
				int seconds = Seconds;
				int fractional = unchecked((int)(value__ % TicksPerSecond));
				String result;
				if(days != 0)
				{
					result = days.ToString() + ".";
				}
				else
				{
					result = String.Empty;
				}
				result = result + TwoDigits(hours) + ":" +
						 TwoDigits(minutes) + ":" + TwoDigits(seconds);
				if(fractional != 0)
				{
					int test = unchecked((int)(TicksPerSecond / 10));
					int digit;
					result = result + ".";
					while(fractional != 0)
					{
						digit = fractional / test;
						result = result + unchecked((char)(digit + (int)'0'));
						fractional %= test;
						test /= 10;
					}
				}
				return result;
			}

	// Add TimeSpan values.
	public TimeSpan Add(TimeSpan ts)
			{
				checked
				{
					return new TimeSpan(value__ + ts.value__);
				}
			}

	// Compare TimeSpan values.
	public static int Compare(TimeSpan t1, TimeSpan t2)
			{
				if(t1.value__ < t2.value__)
				{
					return -1;
				}
				else if(t1.value__ > t2.value__)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}

	// Return the absolute duration of a TimeSpan value.
	public TimeSpan Duration()
			{
				return new TimeSpan(Math.Abs(value__));
			}

	// Determine if this TimeSpan object is equal to another Object.
	public override bool Equals(Object value)
			{
				if(value is TimeSpan)
				{
					return (value__ == ((TimeSpan)value).value__);
				}
				else
				{
					return false;
				}
			}

	// Determine if two TimeSpan values are equal.
	public static bool Equals(TimeSpan t1, TimeSpan t2)
			{
				return (t1.value__ == t2.value__);
			}

	// Convert a floating point number of days into a TimeSpan.
	public static TimeSpan FromDays(double value)
			{
				if(Double.IsNaN(value))
				{
					throw new ArgumentException(_("Arg_NotANumber"));
				}
				else if(Double.IsNegativeInfinity(value))
				{
					return MaxValue;
				}
				else if(Double.IsPositiveInfinity(value))
				{
					return MinValue;
				}
				checked
				{
					return new TimeSpan((long)(value * (double)TicksPerDay));
				}
			}

	// Convert a floating point number of hours into a TimeSpan.
	public static TimeSpan FromHours(double value)
			{
				if(Double.IsNaN(value))
				{
					throw new ArgumentException(_("Arg_NotANumber"));
				}
				else if(Double.IsNegativeInfinity(value))
				{
					return MaxValue;
				}
				else if(Double.IsPositiveInfinity(value))
				{
					return MinValue;
				}
				checked
				{
					return new TimeSpan((long)(value * (double)TicksPerHour));
				}
			}

	// Convert a floating point number of milliseconds into a TimeSpan.
	public static TimeSpan FromMilliseconds(double value)
			{
				if(Double.IsNaN(value))
				{
					throw new ArgumentException(_("Arg_NotANumber"));
				}
				else if(Double.IsNegativeInfinity(value))
				{
					return MaxValue;
				}
				else if(Double.IsPositiveInfinity(value))
				{
					return MinValue;
				}
				checked
				{
					return new TimeSpan
						((long)(value * (double)TicksPerMillisecond));
				}
			}

	// Convert a floating point number of minutes into a TimeSpan.
	public static TimeSpan FromMinutes(double value)
			{
				if(Double.IsNaN(value))
				{
					throw new ArgumentException(_("Arg_NotANumber"));
				}
				else if(Double.IsNegativeInfinity(value))
				{
					return MaxValue;
				}
				else if(Double.IsPositiveInfinity(value))
				{
					return MinValue;
				}
				checked
				{
					return new TimeSpan
							((long)(value * (double)TicksPerMinute));
				}
			}

	// Convert a floating point number of seconds into a TimeSpan.
	public static TimeSpan FromSeconds(double value)
			{
				if(Double.IsNaN(value))
				{
					throw new ArgumentException(_("Arg_NotANumber"));
				}
				else if(Double.IsNegativeInfinity(value))
				{
					return MaxValue;
				}
				else if(Double.IsPositiveInfinity(value))
				{
					return MinValue;
				}
				checked
				{
					return new TimeSpan
							((long)(value * (double)TicksPerSecond));
				}
			}

	// Convert a number of ticks into a TimeSpan.
	public static TimeSpan FromTicks(long ticks)
			{
				return new TimeSpan(ticks);
			}

	// Get the hash code for this instance.
	public override int GetHashCode()
			{
				return unchecked((int)(value__ ^ (value__ >> 16)));
			}

	// Negate a TimeSpan value.
	public TimeSpan Negate()
			{
				if(value__ != Int64.MinValue)
				{
					return new TimeSpan(unchecked(-value__));
				}
				else
				{
					throw new OverflowException
						(_("Overflow_NegateTwosCompNum"));
				}
			}

	// Parse a string into a TimeSpan value.
	[TODO]
	public static TimeSpan Parse(String s)
			{
				// TODO
				return Zero;
			}

	// Subtract TimeSpan values.
	public TimeSpan Subtract(TimeSpan ts)
			{
				checked
				{
					return new TimeSpan(value__ - ts.value__);
				}
			}

	// Operators.
	public static TimeSpan operator+(TimeSpan t1, TimeSpan t2)
			{
				return t1.Add(t2);
			}
	public static TimeSpan operator-(TimeSpan t1, TimeSpan t2)
			{
				return t1.Subtract(t2);
			}
	public static TimeSpan operator-(TimeSpan ts)
			{
				return ts.Negate();
			}
	public static TimeSpan operator+(TimeSpan ts)
			{
				return ts;
			}
	public static bool operator==(TimeSpan t1, TimeSpan t2)
			{
				return (t1.value__ == t2.value__);
			}
	public static bool operator!=(TimeSpan t1, TimeSpan t2)
			{
				return (t1.value__ != t2.value__);
			}
	public static bool operator>(TimeSpan t1, TimeSpan t2)
			{
				return (t1.value__ > t2.value__);
			}
	public static bool operator>=(TimeSpan t1, TimeSpan t2)
			{
				return (t1.value__ >= t2.value__);
			}
	public static bool operator<(TimeSpan t1, TimeSpan t2)
			{
				return (t1.value__ < t2.value__);
			}
	public static bool operator<=(TimeSpan t1, TimeSpan t2)
			{
				return (t1.value__ <= t2.value__);
			}

	// Properties.
	public int Days
			{
				get
				{
					return unchecked((int)(value__ / TicksPerDay));
				}
			}
	public int Hours
			{
				get
				{
					return unchecked((int)((value__ / TicksPerHour) % 24));
				}
			}
	public int Minutes
			{
				get
				{
					return unchecked((int)((value__ / TicksPerMinute) % 60));
				}
			}
	public int Seconds
			{
				get
				{
					return unchecked((int)((value__ / TicksPerSecond) % 60));
				}
			}
	public int Milliseconds
			{
				get
				{
					return unchecked
						((int)((value__ / TicksPerMillisecond) % 1000));
				}
			}
	public long Ticks
			{
				get
				{
					return value__;
				}
			}
	public double TotalDays
			{
				get
				{
					return ((double)value__) / ((double)TicksPerDay);
				}
			}
	public double TotalHours
			{
				get
				{
					return ((double)value__) / ((double)TicksPerHour);
				}
			}
	public double TotalMinutes
			{
				get
				{
					return ((double)value__) / ((double)TicksPerMinute);
				}
			}
	public double TotalSeconds
			{
				get
				{
					return ((double)value__) / ((double)TicksPerSecond);
				}
			}
	public double TotalMilliseconds
			{
				get
				{
					double temp = ((double)value__) / 1000.0;
					if(temp > 922337203685477.0)
					{
						return 922337203685477.0;
					}
					else if(temp < -922337203685477.0)
					{
						return -922337203685477.0;
					}
					else
					{
						return temp;
					}
				}
			}

}; // class TimeSpan

}; // namespace System
