/*
 * DateTimeStyles.cs - Implementation of
 *		"System.Globalization.DateTimeStyles".
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

[Flags]
public enum DateTimeStyles
{

	None                 = 0x0000,
	AllowLeadingWhite    = 0x0001,
	AllowTrailingWhite   = 0x0002,
	AllowInnerWhite      = 0x0004,
	AllowWhiteSpaces     = 0x0007,
	NoCurrentDateDefault = 0x0008,
	AdjustToUniversal    = 0x0010

}; // enum DateTimeStyles

}; // namespace System.Globalization
