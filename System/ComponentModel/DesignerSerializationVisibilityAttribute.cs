/*
 * DesignerSerializationVisibilityAttribute.cs - Implementation of 
 *	"System.ComponentModel.DesignerSerializationVisibilityAttribute" 
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
#if CONFIG_COMPONENT_MODEL || CONFIG_EXTENDED_DIAGNOSTICS
	public sealed class DesignerSerializationVisibilityAttribute: Attribute
	{
		private DesignerSerializationVisibility vis;

		public DesignerSerializationVisibilityAttribute(
							DesignerSerializationVisibility vis)
		{
			this.vis = vis;
		}

		public static readonly DesignerSerializationVisibilityAttribute 
			Content = new DesignerSerializationVisibilityAttribute(
								DesignerSerializationVisibility.Content);

		public static readonly DesignerSerializationVisibilityAttribute 
			Hidden = new DesignerSerializationVisibilityAttribute(
								DesignerSerializationVisibility.Hidden);

		public static readonly DesignerSerializationVisibilityAttribute 
			Visible = new DesignerSerializationVisibilityAttribute(
								DesignerSerializationVisibility.Visible);

		public static readonly DesignerSerializationVisibilityAttribute 
			Default = Visible;
 
		public DesignerSerializationVisibility Visibility 
		{
			get
			{
				return vis;
			}
		}

		public override bool Equals(object obj)
 		{
			DesignerSerializationVisibilityAttribute temp;

			temp = (obj as DesignerSerializationVisibilityAttribute);

			if (temp != null)
			{
				return (temp.Visibility == this.vis);
			}
			else
			{
				return false;
			}
		}

		public override int GetHashCode()
		{
			return vis.GetHashCode();
		}

		public override bool IsDefaultAttribute()
		{
			return Equals(Default);
		}

	}
#endif	
}//namespace
