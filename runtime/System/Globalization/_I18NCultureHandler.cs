/*
 * _I18NCultureHandler.cs - Implementation of the
 *		"System.Globalization._I18NCultureHandler" class.
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

namespace System.Globalization
{

using System.Text;

// This class exists to allow us to query culture information from
// the I18N plugin infrastructure.  It must not be used in application
// programs.

public abstract class _I18NCultureHandler
{
	// Get the compare information for this culture.
	// Returns NULL to use invariant fallbacks.
	public CompareInfo CultureCompareInfo
			{
				get
				{
					return null;
				}
			}

	// Get the date time format information for this culture.
	// Returns NULL to use invariant fallbacks.
	public DateTimeFormatInfo CultureDateTimeFormatInfo
			{
				get
				{
					return null;
				}
			}

	// Get the number format information for this culture.
	// Returns NULL to use invariant fallbacks.
	public NumberFormatInfo CultureNumberFormatInfo
			{
				get
				{
					return null;
				}
			}

	// Get the text information for this culture.
	// Returns NULL to use invariant fallbacks.
	public TextInfo CultureTextInfo
			{
				get
				{
					return null;
				}
			}

	// Get the default calendar for this culture.
	// Returns NULL to use the standard Gregorian calendar.
	public Calendar CultureCalendar
			{
				get
				{
					return null;
				}
			}

	// Get a list of other calendars for the culture.
	public Calendar[] CultureOtherCalendars
			{
				get
				{
					return new Calendar [0];
				}
			}

	// Get the culture handler for a specific culture.
	internal static _I18NCultureHandler GetCultureHandler
				(int culture, bool useUserOverride)
			{
				Object obj = Encoding.InvokeI18N
						("GetCulture", culture, useUserOverride);
				if(obj == null)
				{
					// Try the neutral culture instead.
					obj = Encoding.InvokeI18N
						("GetCulture", culture & 0x03FF, useUserOverride);
				}
				return (_I18NCultureHandler)obj;
			}

}; // class _I18NCultureHandler

}; // namespace System.Globalization
