/*
 * ColorConverter.cs - Implementation of the
 *			"System.Drawing.Printing.ColorConverter" class.
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

namespace System.Drawing
{

using System.ComponentModel;
using System.Globalization;
using System.Collections;

public class ColorConverter : TypeConverter
{
	// Constructor.
	public ColorConverter() {}

	// Determine if we can convert from a given type to "Color".
	public override bool CanConvertFrom
				(ITypeDescriptorContext context, Type sourceType)
			{
				if(sourceType == typeof(String))
				{
					return true;
				}
				else
				{
					return base.CanConvertFrom(context, sourceType);
				}
			}

	// Determine if we can convert to a given type from "Color".
	[TODO]
	public override bool CanConvertTo
				(ITypeDescriptorContext context, Type destinationType)
			{
				// TODO
				return base.CanConvertTo(context, destinationType);
			}

	// Convert from a source type to "Color".
	[TODO]
	public override Object ConvertFrom
				(ITypeDescriptorContext context,
				 CultureInfo culture, Object value)
			{
				// TODO
				return null;
			}

	// Convert from "Color" to a destination type.
	[TODO]
	public override Object ConvertTo
				(ITypeDescriptorContext context,
				 CultureInfo culture, Object value,
				 Type destinationType)
			{
				// TODO
				return null;
			}

	// Return a collection of standard values for this data type.
	[TODO]
	public override TypeConverter.StandardValuesCollection GetStandardValues
				(ITypeDescriptorContext context)
			{
				// TODO
				return null;
			}

	// Determine if "GetStandardValues" is supported.
	public override bool GetStandardValuesSupported
				(ITypeDescriptorContext context)
			{
				return true;
			}

}; // class ColorConverter

}; // namespace System.Drawing
