/*
 * NotFiniteNumberException.cs - Implementation of the
 *			"System.NotFiniteNumberException" class.
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

public class NotFiniteNumberException : ArithmeticException
{
	// Internal state.
	private double number;

	// Constructors.
	public NotFiniteNumberException()
		: base(Environment.GetResourceString("Exception_NotFinite"))
		{ number = 0.0; }
	public NotFiniteNumberException(double offendingNumber)
		: base(Environment.GetResourceString("Exception_NotFinite"))
		{ number = offendingNumber; }
	public NotFiniteNumberException(String msg)
		: base(msg) { number = 0.0; }
	public NotFiniteNumberException(String msg, double offendingNumber)
		: base(msg) { number = offendingNumber; }
	public NotFiniteNumberException(String msg, double offendingNumber,
									Exception inner)
		: base(msg, inner) { number = offendingNumber; }

	// Get the offending number that caused this exception.
	public double OffendingNumber
			{
				get
				{
					return number;
				}
			}

}; // class NotFiniteNumberException

}; // namespace System
