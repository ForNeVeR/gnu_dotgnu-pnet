/*
 * Cursors.cs - Implementation of the
 *			"System.Windows.Forms.Cursors" class.
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
	public class Cursors 
	{
		public static Cursor Arrow 
		{
			get { return new Cursor(Cursor.CursorType.Arrow); }
		}
		public static Cursor WaitCursor 
		{
			get { return new Cursor(Cursor.CursorType.WaitCursor); }
		}
		public static Cursor SizeNS 
		{
			get { return new Cursor(Cursor.CursorType.SizeNS); }
		}
		public static Cursor SizeWE 
		{
			get { return new Cursor(Cursor.CursorType.SizeWE); }
		}
		public static Cursor SizeNWSE 
		{
			get { return new Cursor(Cursor.CursorType.SizeNWSE); }
		}
		public static Cursor SizeNESW 
		{
			get { return new Cursor(Cursor.CursorType.SizeNESW); }
		}
		public static Cursor IBeam 
		{
			get { return new Cursor(Cursor.CursorType.IBeam); }
		}

		public static Cursor Default
		{
			get
			{
				return Cursors.Arrow;
			}
		}
	}

}