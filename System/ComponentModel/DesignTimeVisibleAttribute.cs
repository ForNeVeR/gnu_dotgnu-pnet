/*
 * DesignTimeVisibleAttribute.cs - Implementation of 
 *				"System.ComponentModel.DesignTimeVisibleAttribute" 
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

namespace System.ComponentModel
{

#if CONFIG_COMPONENT_MODEL
	[AttributeUsage(AttributeTargets.Class | AttributeTargets.Interface)]
	public sealed class DesignTimeVisibleAttribute: Attribute
	{
		
		private bool visible;

		public DesignTimeVisibleAttribute()
		{
			visible = true;
		}

		public DesignTimeVisibleAttribute(bool visible)
		{
			this.visible = visible;
		}

		public override bool Equals(Object value)
		{
			DesignTimeVisibleAttribute temp;
			
			temp = value as DesignTimeVisibleAttribute;

			if (temp != null)
			{
				return (temp.Visible == this.visible);
			}
			else
			{
				return false;
			}
		}

		public override int GetHashCode()
		{
			return visible.GetHashCode();
		}

		public override bool IsDefaultAttribute()
		{
			return Equals(Default);
		}

		public static readonly DesignTimeVisibleAttribute Default = Yes;

		public static readonly DesignTimeVisibleAttribute No = 
								new DesignTimeVisibleAttribute(false);

		public static readonly DesignTimeVisibleAttribute Yes =
								new DesignTimeVisibleAttribute(true);

		public bool Visible 
		{
			get
			{
				return visible;
			}
		}

	}
#endif
}//namespace
