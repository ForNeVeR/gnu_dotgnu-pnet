/*
 * TabPage.cs - Implementation of the TabPage Control.
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

	public class TabPage : Panel
	{
		private int imageIndex;
		private string toolTipText;
		// The TabPage has independant visibility to the control
		private new bool visible;
		
		public TabPage()
		{
			visible = true;
		}

		public TabPage(string text) : base()
		{
			Text = text;
		}

		protected override ControlCollection CreateControlsInstance()
		{
			return new TabPageControlCollection( this );
		}

		//TODO:
		public static TabPage GetTabPageOfComponent(object comp)
		{
			return null;
		}

		// The size of the tab page is set by the parent
		protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified) 
		{
			if (base.Parent != null)
			{
			
				Rectangle fix = (base.Parent as TabControl).DisplayRectangle;
				base.SetBoundsCore(fix.X, fix.Y, fix.Width, fix.Height, BoundsSpecified.All);
			}
			else
				base.SetBoundsCore(x, y, width, height, specified);
		}
		
		public int ImageIndex {
			get
			{
				return imageIndex;
			}
			set
			{
				imageIndex = value;
				if (base.Parent != null)
					(Parent as TabControl).DrawTabs();
			}
		}

		public override string Text
		{
			get
			{
				return base.Text;
			}
			set
			{
				base.Text = value;
				if (base.Parent != null)
					(Parent as TabControl).DrawTabs();
			}
		}

		public new bool Visible
		{
			get
			{
				return visible;
			}
			set
			{
				visible = value;
			}
		}


		public string ToolTipText
		{
			get
			{
				return toolTipText;
			}
			set
			{
				toolTipText = value;
			}
		}

		public class TabPageControlCollection : ControlCollection
		{
			public TabPageControlCollection(TabPage owner) : base( owner )
			{}
		}
	}
}
