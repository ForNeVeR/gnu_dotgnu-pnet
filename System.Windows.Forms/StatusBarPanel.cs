/*
 * Control.cs - Implementation of the
 *                      "System.Windows.Forms.StatusBarPanel" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 *
 * Contributions from Simon Guindon
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
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

namespace System.Windows.Forms
{
	public class StatusBarPanel
	#if CONFIG_COMPONENT_MODEL
		: Component, ISupportInitialize
	#endif
	{
		private HorizontalAlignment alignment;
		private StatusBarPanelAutoSize autosize;
		private StatusBarPanelBorderStyle borderStyle;
		private Icon icon;
		private int minWidth;
		internal StatusBar parent;
		private StatusBarPanelStyle style;
		private string text;
		private string toolTipText;
		private int width;

		public StatusBarPanel()
		{
			alignment = HorizontalAlignment.Left;
			autosize = StatusBarPanelAutoSize.None;
			borderStyle = StatusBarPanelBorderStyle.Sunken;
			minWidth = 10;
			style = StatusBarPanelStyle.Text;
			text = this.GetType().Name;
			toolTipText = string.Empty;
			width = 100;
		}

	#if CONFIG_COMPONENT_MODEL
		~StatusBarPanel()
		{
			Dispose(false);
		}

		/// Clean up any resources being used.
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
			}
			base.Dispose( disposing );
		}
	#endif

		public void BeginInit()
		{
		}

		public void EndInit()
		{
		}

		public HorizontalAlignment Alignment
		{
			get { return alignment; }
			set { alignment = value; }
		}

		public StatusBarPanelAutoSize AutoSize
		{
			get { return autosize; }
			set { autosize = value; }
		}

		public StatusBarPanelBorderStyle BorderStyle 
		{
			get { return borderStyle; }
			set { borderStyle = value; }
		}

		public Icon Icon 
		{
			get { return icon; }
			set { icon = value; }
		}

		public int MinWidth 
		{
			get { return minWidth; }
			set { minWidth = value; }
		}

		public StatusBar Parent 
		{
			get { return parent; }
		}

		public StatusBarPanelStyle Style 
		{
			get { return style; }
			set { style = value; }
		}

		public string Text
		{
			get { return text; }
			set { text = value; }
		}

		public string ToolTipText 
		{
			get { return toolTipText; }
			set { toolTipText = value; }
		}

		public int Width 
		{
			get { return width; }
			set { width = value; }
		}

		public override string ToString()
		{
			return text;
		}

	}
}
