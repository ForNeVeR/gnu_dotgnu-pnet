/*
 * EditorBrowsableAttribute.cs - Implementation of 
 *						"System.ComponentModel.EditorBrowsableAttribute" 
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
#if !ECMA_COMPAT
	public sealed class EditorBrowsableAttribute: Attribute
	{
		private EditorBrowsableState state;

		public EditorBrowsableAttribute(EditorBrowsableState state)
		{
			this.state = state;
		}

		public EditorBrowsableAttribute()
		{
			state = EditorBrowsableState.Always;
		}

		public EditorBrowsableState State 
		{
			get
			{
				return state;
			}
		}

		public override bool Equals(Object value)
		{
			EditorBrowsableAttribute temp;

			temp = value as EditorBrowsableAttribute;

			if (temp != null)
			{
				return (temp.State == state);
			}
			else
			{
				return false;
			}
		}

		public override int GetHashCode()
		{
			return state.GetHashCode();
		}

	}
#endif	
}//namespace
