/*
 * StringConverter.cs - Implementation of 
 *						"System.ComponentModel.StringConverter" 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  Free Software Foundation,Inc.
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
using System.Globalization;

namespace System.ComponentModel
{
#if !ECMA_COMPAT
	public class StringConverter: TypeConverter
	{
		[TODO]
		public void StringConverter()
		{
			throw new NotImplementedException(".ctor");
		}

		[TODO]
		public override bool CanConvertFrom(ITypeDescriptorContext context, 
											Type sourceType)
		{
			throw new NotImplementedException("CanConvertFrom");
		}

		[TODO]
		public override Object ConvertFrom(ITypeDescriptorContext context, 
								CultureInfo culture,Object value)
		{
			throw new NotImplementedException("ConvertFrom");
		}

	}
#endif	
}//namespace
