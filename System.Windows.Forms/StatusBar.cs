/*
 * Control.cs - Implementation of the
 *                      "System.Windows.Forms.StatusBar" class.
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
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Text;
using System.Windows.Forms;

namespace System.Windows.Forms
{
	//[Serializable]
	//public delegate void StatusBarDrawItemEventHandler(object sender,
	//StatusBarDrawItemEventArgs sbdevent);

	//[Serializable]
	//public delegate void StatusBarPanelClickEventHandler(object sender,
	//StatusBarPanelClickEventArgs e);

	public class StatusBar : Control
	{
		public event StatusBarDrawItemEventHandler DrawItem;
		public event StatusBarPanelClickEventHandler PanelClick;
		
		private StatusBarPanelCollection panels;
		private bool showPanels;
		private bool sizingGrip;

		public StatusBar()
		{
			Name = "StatusBar";
			Height = 22;
			Dock = DockStyle.Bottom;
			showPanels = false;
			sizingGrip = true;
			panels = new StatusBarPanelCollection(this);
		}

		~StatusBar()
		{
			Dispose(false);
		}

		/// Clean up any resources being used.
		protected override void Dispose(bool disposing)
		{
			if(disposing)
			{
			}
			base.Dispose(disposing);
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			//ControlPaint.DrawBorder3D(e.Graphics, 0, 0, Width, Height, Border3DStyle.SunkenInner, Border3DSide.Top);
			
			if (showPanels == false)
			{
				DrawSimpleText(e, 0, 0, Width, Height, Text);
			}
			else
			{
				int left = 0;
				Border3DStyle style = Border3DStyle.Sunken;

				for (int i=0;i<panels.Count;i++)
				{
					switch (panels[i].BorderStyle)
					{
						case StatusBarPanelBorderStyle.None:
							style = Border3DStyle.Flat;
							break;
						case StatusBarPanelBorderStyle.Raised:
							style = Border3DStyle.Raised;
							break;
						case StatusBarPanelBorderStyle.Sunken:
							style = Border3DStyle.SunkenOuter;
							break;
					}
					ControlPaint.DrawBorder3D(e.Graphics, left, 0, panels[i].Width, Height, style, Border3DSide.All);
					DrawSimpleText(e, left, 0, panels[i].Width, Height,  panels[i].Text);
					left += panels[i].Width +2;
				}
				
			}

			if (sizingGrip == true)
			{
				ControlPaint.DrawSizeGrip(e.Graphics, BackColor, Width - 15, Height - 13, 15, 13);
			}
			base.OnPaint(e);
		}

		private void DrawSimpleText(PaintEventArgs e, int left, int top, int right, int bottom, string text)
		{
			// Draw the text within the label.
			Font font = Font;
			RectangleF layout = (RectangleF)Rectangle.FromLTRB(left, top, right, bottom);

			StringFormat format = new StringFormat();
			format.Alignment = StringAlignment.Near;
			format.LineAlignment = StringAlignment.Center;

			if(text != null && text != String.Empty)
			{
				if(Enabled)
				{
					Brush brush = new SolidBrush(ForeColor);
					e.Graphics.DrawString(text, font, brush, layout, format);
					brush.Dispose();
				}
				else
				{
					ControlPaint.DrawStringDisabled(e.Graphics, text, font, BackColor, layout, format);
				}
			}
		}

		public override Color BackColor
		{
			get { return base.BackColor; }
			set { base.BackColor = value; }
		}

		public override Image BackgroundImage
		{
			get { return base.BackgroundImage; }
			set { base.BackgroundImage = value; }
		}

		public override DockStyle Dock 
		{
			get { return base.Dock; }
			set { base.Dock = value; }
		}

		public override Font Font 
		{
			get { return base.Font; }
			set { base.Font = value; }
		}

		//[DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
		public StatusBarPanelCollection Panels 
		{
			get { return panels; }
			//set { statusBarPanelCollection = value; }
		}

		public bool ShowPanels 
		{
			get { return showPanels; }
			set 
			{
				showPanels = value; 
				Invalidate();
			}
		}

		public bool SizingGrip 
		{
			get { return sizingGrip; }
			set 
			{ 
				sizingGrip = value;
				Invalidate();
			}
		}

		public override string Text 
		{
			get { return base.Text; }
			set 
			{
				base.Text = value;
				Invalidate();
			}
		}

		protected override CreateParams CreateParams 
		{
			get { return base.CreateParams; }
		}

		protected override ImeMode DefaultImeMode 
		{
			get { return base.DefaultImeMode; }
		}

		protected override Size DefaultSize 
		{
			get { return base.DefaultSize; }
		}

		protected override void CreateHandle()
		{
			base.CreateHandle();
		}

		protected virtual void OnDrawItem(StatusBarDrawItemEventArgs e)
		{
			if (DrawItem != null)
			{
				DrawItem(this, e);
			}
		}

		protected override void OnHandleCreated(EventArgs e)
		{
			base.OnHandleCreated(e);
		}

		protected override void OnHandleDestroyed(EventArgs e)
		{
			base.OnHandleDestroyed(e);
		}

		protected override void OnLayout(LayoutEventArgs e)
		{
			base.OnLayout(e);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown(e);
		}

		protected virtual void OnPanelClick(StatusBarPanelClickEventArgs e)
		{
			if (PanelClick != null)
			{
				PanelClick(this, e);
			}
		}

		protected override void OnResize(EventArgs e)
		{
			Invalidate();
			base.OnResize(e);
		}
		
		#if !CONFIG_COMPACT_FORMS
		// Process a message.
		protected override void WndProc(ref Message m)
		{
			base.WndProc(ref m);
		}
		#endif // !CONFIG_COMPACT_FORMS
		
	}
	/*
	public class StatusBarDrawItemEventArgs : DrawItemEventArgs
	{
		private StatusBarPanel panel;

		public StatusBarDrawItemEventArgs(Graphics g, Font font, Rectangle r, int itemId, DrawItemState itemState, StatusBarPanel panel) : base(g, font, r, itemId, itemState)
		{
			this.panel = panel;
		}
		public StatusBarDrawItemEventArgs(Graphics g, Font font, Rectangle r, int itemId, DrawItemState itemState, StatusBarPanel panel, Color foreColor, Color backColor) : base(g, font, r, itemId, itemState, foreColor, backColor)
		{
			this.panel = panel;
		}

	}

	public class StatusBarPanelClickEventArgs : MouseEventArgs
	{
		private StatusBarPanel panel;

		public StatusBarPanelClickEventArgs(StatusBarPanel statusBarPanel, MouseButtons button, int clicks, int x, int y) : base(button, clicks, x, y, 0)
		{
			panel = statusBarPanel;
		}
	}*/
}
