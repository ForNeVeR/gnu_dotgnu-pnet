/*
 * ScrollableControl.cs - Implementation of the
 *			"System.Windows.Forms.ScrollableControl" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

using System.Drawing;
using System.ComponentModel;

#if CONFIG_COMPONENT_MODEL
[Designer
	("System.Windows.Forms.Design.ScrollableControlDesigner, System.Design")]
#endif
public class ScrollableControl : Control
{
	// Internal state.
	private bool autoScroll;
	private bool hscroll;
	private bool vscroll;
	private Size autoScrollMargin;
	private Size autoScrollMinSize;
	private Point autoScrollPosition;
	private DockPaddingEdges dockPadding;

	// Constructor.
	public ScrollableControl() {}

	// Get or set this control's properties.
	public virtual bool AutoScroll
			{
				get
				{
					return autoScroll;
				}
				set
				{
					autoScroll = value;
				}
			}
	public Size AutoScrollMargin
			{
				get
				{
					return autoScrollMargin;
				}
				set
				{
					if(value.Width < 0)
					{
						throw new ArgumentOutOfRangeException
							("value.Width", S._("SWF_NonNegative"));
					}
					if(value.Height < 0)
					{
						throw new ArgumentOutOfRangeException
							("value.Height", S._("SWF_NonNegative"));
					}
					autoScrollMargin = value;
				}
			}
	public Size AutoScrollMinSize
			{
				get
				{
					return autoScrollMinSize;
				}
				set
				{
					autoScrollMinSize = value;
				}
			}
	public Point AutoScrollPosition
			{
				get
				{
					return autoScrollPosition;
				}
				set
				{
					autoScrollPosition = value;
				}
			}
	protected override CreateParams CreateParams
			{
				get
				{
					return base.CreateParams;
				}
			}
	[TODO]
	public override Rectangle DisplayRectangle
			{
				get
				{
					// TODO: subtract the scroll bars from the client size
					return base.DisplayRectangle;
				}
			}
	public DockPaddingEdges DockPadding
			{
				get
				{
					return dockPadding;
				}
				set
				{
					dockPadding = value;
				}
			}
	protected bool HScroll
			{
				get
				{
					return hscroll;
				}
				set
				{
					hscroll = value;
				}
			}
	protected bool VScroll
			{
				get
				{
					return vscroll;
				}
				set
				{
					vscroll = value;
				}
			}

	// Set the auto scroll margin.
	public void SetAutoScrollMargin(int x, int y)
			{
				if(x < 0)
				{
					x = 0;
				}
				if(y < 0)
				{
					y = 0;
				}
				AutoScrollMargin = new Size(x, y);
			}

	// Handle a layout event.
	[TODO]
	protected override void OnLayout(LayoutEventArgs e)
			{
				// TODO
				base.OnLayout(e);
			}

	// Handle a mouse wheel event.
	[TODO]
	protected override void OnMouseWheel(MouseEventArgs e)
			{
				// TODO
				base.OnMouseWheel(e);
			}

	// Handle a visibility change event.
	[TODO]
	protected override void OnVisibleChanged(EventArgs e)
			{
				// TODO
				base.OnVisibleChanged(e);
			}

	// Inner core of "Scale".
	[TODO]
	protected override void ScaleCore(float dx, float dy)
			{
				// TODO
				base.ScaleCore(dx, dy);
			}

	// Dock padding edge definitions.
	[TODO]
	public class DockPaddingEdges : ICloneable
	{
		// TODO

		public Object Clone() { return this; }

	}; // class DockPaddingEdges

#if !CONFIG_COMPACT_FORMS

	// Process a message.
	public override void WndProc(ref Message m)
			{
				base.WndProc(ref m);
			}

#endif // !CONFIG_COMPACT_FORMS

}; // class ScrollableControl

}; // namespace System.Windows.Forms
