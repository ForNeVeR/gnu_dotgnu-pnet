/*
 * RefreshEventArgs.cs - Implementation of 
 *							"System.ComponentModel.RefreshEventArgs" 
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
	public class RefreshEventArgs: EventArgs
	{
		private Object componentChanged;
		private Type typeChanged;

		public RefreshEventArgs(Object componentChanged)
		{
			this.componentChanged = componentChanged;
		}

		public RefreshEventArgs(Type typeChanged)
		{
			this.typeChanged = typeChanged;
		}

		public Object ComponentChanged 
		{
			get
			{
				return componentChanged;
			}
		}

		public Type TypeChanged 
		{
			get
			{
				return typeChanged;
			}
		}

	}
#endif	
}//namespace
