/*
 * Cursor.cs - Implementation of the
 *			"System.Windows.Forms.Cursor" class.
 *
 * Copyright (C) 2003 Neil Cawse.
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
using System.IO;
	public class Cursor 
	{
		private CursorType type;
		internal enum CursorType
		{
			AppStarting,
			Arrow,
			Cross,
			Default,
			IBeam,
			No,
			SizeAll,
			SizeNS,
			SizeWE,
			SizeNWSE,
			SizeNESW,
			UpArrow,
			WaitCursor,
			Help,
			HSplit,
			VSplit,
			NoMove2D,
			NoMoveHoriz,
			NoMoveVert,
			PanEast,
			PanNE,
			PanNorth,
			PanNW,
			PanSE,
			PanSouth,
			PanSW,
			PanWest,
			Hand
		}
		internal Cursor (CursorType type) 
		{
			this.type = type;
		}

		public Cursor(Stream stream)
		{
		}

		public override bool Equals(object obj) 
		{
			if (obj == null || obj.GetType() != typeof(Cursor))
				return false;
			return (obj as Cursor).type==type;
		}

		public override int GetHashCode()
		{
			throw new NotImplementedException();
		}

		//TODO
		public static Cursor Current
		{
			get
			{
				return null;
			}

			set
			{
			}
		}

		//TODO
		public static void Hide()
		{
		}

		//TODO
		public static void Show()
		{
		}
	}
}
