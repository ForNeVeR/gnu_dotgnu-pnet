/*
 * ListChangedEventArgs.cs - Implementation of 
 *							"System.ComponentModel.ListChangedEventArgs" 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  Free Software Foundation.
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
	public class ListChangedEventArgs: EventArgs
	{
		private ListChangedType listChangedType;
		private int newIndex;
		private int oldIndex;
	
		public ListChangedEventArgs(ListChangedType listChangedType, 
										int newIndex)
		{
			this.listChangedType = listChangedType;
			this.newIndex = newIndex;
		}

		[TODO]
		public ListChangedEventArgs(ListChangedType listChangedType, 
											PropertyDescriptor propDesc)
		{
			//this.listChangedType = listChangedType;
			throw new NotImplementedException("ListChangedEventArgs");
		}

		public ListChangedEventArgs(ListChangedType listChangedType, 
											int newIndex, int oldIndex)
		{
			this.listChangedType = listChangedType;
			this.newIndex = newIndex;
			this.oldIndex = oldIndex;
		}

		public ListChangedType ListChangedType  
		{
			get
			{
				return listChangedType;
			}
		}

		public int NewIndex 
		{
			get
			{
				return newIndex;
			}
		}

		public int OldIndex 
		{
			get
			{
				return oldIndex;
			}
		}

	}
#endif	
}//namespace
