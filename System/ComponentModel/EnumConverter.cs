/*
 * EnumConverter.cs - Implementation of "System.ComponentModel.EnumConverter" 
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
using System.Collections;

namespace System.ComponentModel
{
#if CONFIG_COMPONENT_MODEL
	public class EnumConverter: TypeConverter
	{
		private Type type;

		public EnumConverter(Type type)
		{
			this.type = type;
		}

		public override bool CanConvertFrom(ITypeDescriptorContext context, 
											Type sourceType)
		{
			if (sourceType == typeof(string))
				return true;
			/* TODO: Find a better way ? */
			return base.CanConvertFrom(context, sourceType);
		}

		public override bool CanConvertTo(ITypeDescriptorContext context, 
										Type destinationType)
		{
			/* TODO: Find a better way ? */
			return base.CanConvertTo(context, destinationType);
		}

		[TODO]
		public override Object ConvertFrom(ITypeDescriptorContext context, 
											CultureInfo culture, Object value)
		{
			/* TODO: Find a better way ? */
			string val = value as string;
			if (val == null)
				return base.ConvertFrom(context, culture, value);
			string[] values = val.Split(new char[] {','});
			object temp;

			foreach (string s in values)
			{
				temp = Enum.Parse(type, s, true);
				if (temp != null)
					return temp;
			}
			return null;
		}

		public override Object ConvertTo(ITypeDescriptorContext context, 
				CultureInfo culture, Object value, Type destinationType)
		{
			/* TODO: Find a better way ? */
			if (destinationType == typeof(string))
				return value.ToString();
			return base.ConvertTo(context, culture, value, destinationType);
		}

		public override bool IsValid(ITypeDescriptorContext context, 
									Object value)
		{
			return Enum.IsDefined(type, value);
		}

	}
#endif	
}//namespace
