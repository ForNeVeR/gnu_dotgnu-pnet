/*
 * ColorDialog.cs - Implementation of the
 *			"System.Windows.Forms.ColorDialog" class.
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
using System.Drawing;

	public class ColorDialog : CommonDialog
	{
		private int options;
		private Color color;
		private int[] customColors;

		[TODO]
		public virtual bool AllowFullOpen
		{
			get
			{
				return false;
			}

			set
			{
			}
		}

		[TODO]
		public virtual bool AnyColor
		{
			get
			{
				return false;
			}

			set
			{
			}
		}

		public Color Color
		{
			get
			{
				return color;
			}

			set
			{
				if (value.IsEmpty)
					color = Color.Black;
				else
					color = value;
				
			}
		}

		public int[] CustomColors
		{
			get
			{
				return customColors.Clone() as int[];
			}

			set
			{
				int i = 0;
				if (value != null)
					i = Math.Min(value.Length, 16);
				if (i > 0)
					Array.Copy(value, 0, customColors, 0, i);
				for (; i < 16; i++)
					customColors[i] = 0;
			}
		}

		[TODO]
		public virtual bool FullOpen
		{
			get
			{
				return false;
			}

			set
			{
			}
		}

		protected virtual IntPtr Instance
		{
			get
			{
				// Not used in this implementation
				return IntPtr.Zero;
			}
		}

		protected virtual int Options
		{
			get
			{
				return options;
			}
		}

		[TODO]
		public virtual bool ShowHelp
		{
			get
			{
				return false;
			}

			set
			{
			}
		}

		[TODO]
		public virtual bool SolidColorOnly
		{
			get
			{
				return false;
			}

			set
			{
			}
		}

		public ColorDialog()
		{
			customColors = new int[16];
		}

		[TODO]
		public override void Reset()
		{
			options = 0;
			color = Color.Black;
			CustomColors = null;
		}

		protected override bool RunDialog(IntPtr hwndOwner)
		{
			return false;
		}

		public override string ToString()
		{
			return base.ToString() + ", Color: " + Color.ToString();
		}
	}

}
