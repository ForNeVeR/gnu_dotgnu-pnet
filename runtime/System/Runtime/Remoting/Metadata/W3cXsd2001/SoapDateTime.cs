/*
 * SoapDateTime.cs - Implementation of the
 *		"System.Runtime.Remoting.Metadata.W3cXsd2001.SoapDateTime" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace System.Runtime.Remoting.Metadata.W3cXsd2001
{

#if CONFIG_REMOTING

public sealed class SoapDateTime
{
	// Get the schema type for this class.
	public static String XsdType
			{
				get
				{
					return "dateTime";
				}
			}

	// Parse a value into a DateTime instance.
	[TODO]
	public static DateTime Parse(String value)
			{
				// TODO
				return new DateTime(0);
			}

	// Convert a date into a string.
	[TODO]
	public static String ToString(DateTime value)
			{
				// TODO
				return null;
			}

}; // class SoapDateTime

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Metadata.W3cXsd2001
