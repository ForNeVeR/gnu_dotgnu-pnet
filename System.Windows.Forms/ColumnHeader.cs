/*
 * ColumnHeader.cs - Implementation of the
 *			"System.Windows.Forms.ColumnHeader" class.
 *
 * Copyright (C) 2003  Neil Cawse.
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

namespace System.Windows.Forms
{

using System;
using System.Reflection;
using System.ComponentModel;

#if CONFIG_COMPONENT_MODEL
	public class ColumnHeader : Component
#else
	public class ColumnHeader
#endif
	{
		private ListView listView;
		internal int width;
		private HorizontalAlignment textAlign;
		internal string text;

		public ColumnHeader()
		{
			width = 60;
			textAlign = HorizontalAlignment.Left;
		}

		internal ListView OwnerListview
		{
			get
			{
				return listView;
			}

			set
			{
				listView = value;
			}
		}

		public ListView ListView
		{
			get
			{
				return listView;
			}
		}

		[TODO]
		public int Index
		{
			get
			{
				if (listView == null)
					return -1;
				else
					return 0; //TODO
			}
		}

		[TODO]
		public HorizontalAlignment TextAlign
		{
			get
			{
				return textAlign;
			}

			set
			{
				if (value == textAlign || listView == null)
					return;
				if (Index == 0)
					textAlign = HorizontalAlignment.Left;
				else
					textAlign = value;

				//if (listView != null)
					//TODO Update ListView
			}
		}

		[TODO]
		public int Width
		{
			get
			{
				return width;
			}

			set
			{
				width = value;
				//if (listView != null)
					//TODO Set Column width
			}
		}

		public virtual object Clone()
		{
			ColumnHeader columnHeader = null;
			Type type = base.GetType();
			if (type == typeof(ColumnHeader))
				columnHeader = new ColumnHeader();
		#if !ECMA_COMPAT
			else
				columnHeader = Activator.CreateInstance(type) as ColumnHeader;
		#else
			else
				columnHeader = type.InvokeMember
					(String.Empty, BindingFlags.CreateInstance |
								   BindingFlags.Public |
								   BindingFlags.Instance,
					 null, null, null, null, null, null) as ColumnHeader;
		#endif
			columnHeader.textAlign = TextAlign;
			columnHeader.text = text;
			columnHeader.Width = width;
			return columnHeader;
		}

		[TODO]
		public string Text
		{
			get
			{
				if (text == null)
					return "ColumnHeader";
				else
					return text;
			}

			set
			{
				if (value == null)
					text = "";
				else
					text = value;
				//if (listView != null)
					//TODO Update ListView
			}
		}


		public override string ToString()
		{
			return "ColumnHeader: Text: " + Text;
		}
	}
}
