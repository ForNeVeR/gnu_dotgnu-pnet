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
	public sealed class Cursors 
	{
		private static Cursor appStarting;
		private static Cursor arrow;
		private static Cursor cross;
		private static Cursor defaultCursor;
		private static Cursor iBeam;
		private static Cursor no;
		private static Cursor sizeAll;
		private static Cursor sizeNESW;
		private static Cursor sizeNS;
		private static Cursor sizeNWSE;
		private static Cursor sizeWE;
		private static Cursor upArrow;
		private static Cursor waitCursor;
		private static Cursor help;
		private static Cursor hSplit;
		private static Cursor vSplit;
		private static Cursor noMove2D;
		private static Cursor noMoveHoriz;
		private static Cursor noMoveVert;
		private static Cursor panEast;
		private static Cursor panNE;
		private static Cursor panNorth;
		private static Cursor panNW;
		private static Cursor panSE;
		private static Cursor panSouth;
		private static Cursor panSW;
		private static Cursor panWest;
		private static Cursor hand;

		public static Cursor AppStarting
		{
			get
			{
				if (appStarting == null)
					appStarting = new Cursor(Cursor.CursorType.AppStarting);
				return appStarting;
			}
		}

		public static Cursor Arrow
		{
			get
			{
				if (arrow == null)
					arrow = new Cursor(Cursor.CursorType.Arrow);
				return arrow;
			}
		}

		public static Cursor Cross
		{
			get
			{
				if (cross == null)
					cross = new Cursor(Cursor.CursorType.Cross);
				return cross;
			}
		}

		public static Cursor Default
		{
			get
			{
				if (defaultCursor == null)
					defaultCursor = new Cursor(Cursor.CursorType.Default);
				return defaultCursor;
			}
		}

		public static Cursor IBeam
		{
			get
			{
				if (iBeam == null)
					iBeam = new Cursor(Cursor.CursorType.IBeam);
				return iBeam;
			}
		}

		public static Cursor No
		{
			get
			{
				if (no == null)
					no = new Cursor(Cursor.CursorType.No);
				return no;
			}
		}
		public static Cursor SizeAll
		{
			get
			{
				if (sizeAll == null)
					sizeAll = new Cursor(Cursor.CursorType.SizeAll);
				return sizeAll;
			}
		}
		public static Cursor SizeNESW
		{
			get
			{
				if (sizeNESW == null)
					sizeNESW = new Cursor(Cursor.CursorType.SizeNESW);
				return sizeNESW;
			}
		}
		public static Cursor SizeNS
		{
			get
			{
				if (sizeNS == null)
					sizeNS = new Cursor(Cursor.CursorType.SizeNS);
				return sizeNS;
			}
		}
		public static Cursor SizeNWSE
		{
			get
			{
				if (sizeNWSE == null)
					sizeNWSE = new Cursor(Cursor.CursorType.SizeNWSE);
				return sizeNWSE;
			}
		}	
		public static Cursor SizeWE
		{
			get
			{
				if (sizeWE == null)
					sizeWE = new Cursor(Cursor.CursorType.SizeWE);
				return sizeWE;
			}
		}
		public static Cursor UpArrow
		{
			get
			{
				if (upArrow == null)
					upArrow = new Cursor(Cursor.CursorType.UpArrow);
				return upArrow;
			}
		}
		public static Cursor WaitCursor
		{
			get
			{
				if (waitCursor == null)
					waitCursor = new Cursor(Cursor.CursorType.WaitCursor);
				return waitCursor;
			}
		}
		public static Cursor Help
		{
			get
			{
				if (help == null)
					help = new Cursor(Cursor.CursorType.Help);
				return help;
			}
		}
		public static Cursor HSplit
		{
			get
			{
				if (hSplit == null)
					hSplit = new Cursor(Cursor.CursorType.HSplit);
				return hSplit;
			}
		}
		public static Cursor VSplit
		{
			get
			{
				if (vSplit == null)
					vSplit = new Cursor(Cursor.CursorType.VSplit);
				return vSplit;
			}
		}
		public static Cursor NoMove2D
		{
			get
			{
				if (noMove2D == null)
					noMove2D = new Cursor(Cursor.CursorType.NoMove2D);
				return noMove2D;
			}
		}
		public static Cursor NoMoveHoriz
		{
			get
			{
				if (noMoveHoriz == null)
					noMoveHoriz = new Cursor(Cursor.CursorType.NoMoveHoriz);
				return noMoveHoriz;
			}
		}
		public static Cursor NoMoveVert
		{
			get
			{
				if (noMoveHoriz == null)
					noMoveVert = new Cursor(Cursor.CursorType.NoMoveVert);
				return noMoveVert;
			}
		}
		public static Cursor PanEast
		{
			get
			{
				if (panEast == null)
					panEast = new Cursor(Cursor.CursorType.PanEast);
				return panEast;
			}
		}
		public static Cursor PanNE
		{
			get
			{
				if (panNE == null)
					panNE = new Cursor(Cursor.CursorType.PanNE);
				return panNE;
			}
		}
		public static Cursor PanNorth
		{
			get
			{
				if (panNorth == null)
					panNorth = new Cursor(Cursor.CursorType.PanNorth);
				return panNorth;
			}
		}
		public static Cursor PanNW
		{
			get
			{
				if (panNW == null)
					panNW = new Cursor(Cursor.CursorType.PanNW);
				return panNW;
			}
		}
		public static Cursor PanSE
		{
			get
			{
				if (panSE == null)
					panSE = new Cursor(Cursor.CursorType.PanSE);
				return panSE;
			}
		}
		public static Cursor PanSouth
		{
			get
			{
				if (panSouth == null)
					panSouth = new Cursor(Cursor.CursorType.PanSouth);
				return panSouth;
			}
		}
		public static Cursor PanSW
		{
			get
			{
				if (panSW == null)
					panSW = new Cursor(Cursor.CursorType.PanSW);
				return panSW;
			}
		}
		public static Cursor West
		{
			get
			{
				if (panWest == null)
					panWest = new Cursor(Cursor.CursorType.PanWest);
				return panWest;
			}
		}
		public static Cursor Hand
		{
			get
			{
				if (hand == null)
					hand = new Cursor(Cursor.CursorType.Hand);
				return hand;
			}
		}
	}

}
