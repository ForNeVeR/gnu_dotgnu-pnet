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
	public static readonly TimeSpan Empty = Zero;
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
					value__ = result.ToInt64();
				}
				catch(OverflowException)
				{
					throw new ArgumentOutOfRangeException
						(Environment.GetResourceString
							("ArgRange_TimeSpan"));
				}
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is TimeSpan))
					{
						throw new ArgumentException
							(Environment.GetResourceString
								("Arg_MustBeTimeSpan"));
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

	// Formatting.
	public override String ToString()
			{
				// TODO
				return "";
			}

// TODO

}; // class TimeSpan

}; // namespace System
