/*
 * ButtonBase.cs - Implementation of the
 *			"System.Windows.Forms.ButtonBase" class.
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

public abstract class ButtonBase : Control
{
	// Internal state.
	private FlatStyle flatStyle;
	private Image image;
	private ContentAlignment imageAlign;
	private int imageIndex;
	private ImageList imageList;
	private ContentAlignment textAlign;
	private bool isDefault;
	internal bool entered;
	internal bool pressed;
	private bool hasFocus;
	private MouseButtons button;
	private StringFormat format;

	// Contructor.
	protected ButtonBase()
			{
				flatStyle = FlatStyle.Standard;
				imageAlign = ContentAlignment.MiddleCenter;
				imageIndex = -1;
				textAlign = ContentAlignment.MiddleCenter;
				format = new StringFormat();
				SetStringFormat();
				SetStyle(ControlStyles.ResizeRedraw, true);
			}

	// Get or set this control's properties.
	public FlatStyle FlatStyle
			{
				get
				{
					return flatStyle;
				}
				set
				{
					if(flatStyle != value)
					{
						flatStyle = value;
						Redraw();
					}
				}
			}
	public Image Image
			{
				get
				{
					return image;
				}
				set
				{
					if(image != value)
					{
						image = value;
						Redraw();
					}
				}
			}
	public ContentAlignment ImageAlign
			{
				get
				{
					return imageAlign;
				}
				set
				{
					if(imageAlign != value)
					{
						imageAlign = value;
						Redraw();
					}
				}
			}
	public int ImageIndex
			{
				get
				{
					return imageIndex;
				}
				set
				{
					if(imageIndex != value)
					{
						imageIndex = value;
						Redraw();
					}
				}
			}
	public ImageList ImageList
			{
				get
				{
					return imageList;
				}
				set
				{
					if(imageList != value)
					{
						imageList = value;
						Redraw();
					}
				}
			}
	public new ImeMode ImeMode
			{
				get
				{
					return base.ImeMode;
				}
				set
				{
					base.ImeMode = value;
				}
			}
	public virtual ContentAlignment TextAlign
			{
				get
				{
					return textAlign;
				}
				set
				{
					if(textAlign != value)
					{
						textAlign = value;
						SetStringFormat();
						Redraw();
					}
				}
			}
	protected override CreateParams CreateParams
			{
				get
				{
					return base.CreateParams;
				}
			}
	protected override ImeMode DefaultImeMode
			{
				get
				{
					return ImeMode.Disable;
				}
			}
	protected override Size DefaultSize
			{
				get
				{
					return new Size(75, 23);
				}
			}
	protected bool IsDefault
			{
				get
				{
					return isDefault;
				}
				set
				{
					if(isDefault != value)
					{
						isDefault = value;
						Redraw();
					}
				}
			}

#if !CONFIG_COMPACT_FORMS

	// Create the accessibility object for this control.
	protected override AccessibleObject CreateAccessibilityInstance()
			{
				return base.CreateAccessibilityInstance();
			}

#endif

	// Dispose of this instance.
	protected override void Dispose(bool disposing)
			{
				// Nothing to do in this implementation.
				base.Dispose(disposing);
			}

	// Set the correct string format, based on text alignment and RTL info.
	private void SetStringFormat()
			{
				switch(RtlTranslateAlignment(TextAlign))
				{
					case ContentAlignment.TopLeft:
					{
						format.Alignment = StringAlignment.Near;
						format.LineAlignment = StringAlignment.Near;
					}
					break;

					case ContentAlignment.TopCenter:
					{
						format.Alignment = StringAlignment.Center;
						format.LineAlignment = StringAlignment.Near;
					}
					break;

					case ContentAlignment.TopRight:
					{
						format.Alignment = StringAlignment.Far;
						format.LineAlignment = StringAlignment.Near;
					}
					break;

					case ContentAlignment.MiddleLeft:
					{
						format.Alignment = StringAlignment.Near;
						format.LineAlignment = StringAlignment.Center;
					}
					break;

					case ContentAlignment.MiddleCenter:
					{
						format.Alignment = StringAlignment.Center;
						format.LineAlignment = StringAlignment.Center;
					}
					break;

					case ContentAlignment.MiddleRight:
					{
						format.Alignment = StringAlignment.Far;
						format.LineAlignment = StringAlignment.Center;
					}
					break;

					case ContentAlignment.BottomLeft:
					{
						format.Alignment = StringAlignment.Near;
						format.LineAlignment = StringAlignment.Far;
					}
					break;

					case ContentAlignment.BottomCenter:
					{
						format.Alignment = StringAlignment.Center;
						format.LineAlignment = StringAlignment.Far;
					}
					break;

					case ContentAlignment.BottomRight:
					{
						format.Alignment = StringAlignment.Far;
						format.LineAlignment = StringAlignment.Far;
					}
					break;
				}
			}

	// Calculate the current state of the button for its visual appearance.
	internal virtual ButtonState CalculateState()
			{
				ButtonState state = ButtonState.Normal;
				if(Enabled)
				{
					if(entered && pressed)
					{
						state |= ButtonState.Pushed;
						if(flatStyle == FlatStyle.Flat)
						{
							state |= ButtonState.Flat;
						}
					}
					else if(entered)
					{
						if(flatStyle == FlatStyle.Flat)
						{
							state |= ButtonState.Flat;
						}
					}
					else
					{
						if(flatStyle == FlatStyle.Flat ||
						   flatStyle == FlatStyle.Popup)
						{
							state |= ButtonState.Flat;
						}
					}
				}
				else
				{
					state |= ButtonState.Inactive;
					if(flatStyle == FlatStyle.Flat ||
					   flatStyle == FlatStyle.Popup)
					{
						state |= ButtonState.Flat;
					}
				}
				return state;
			}

	// Draw the button in its current state on a Graphics surface.
	private void Draw(Graphics graphics)
			{
				ButtonState state = CalculateState();
				Size clientSize = ClientSize;
				int x = 0;
				int y = 0;
				int width = clientSize.Width;
				int height = clientSize.Height;

				// Draw the border and background.
				ControlPaint.DrawButton
					(graphics, x, y, width, height, state,
					 ForeColor, BackColor, isDefault);

				// TODO: draw the image.

				// Draw the focus rectangle.
				if(hasFocus)
				{
					ControlPaint.DrawFocusRectangle
						(graphics, new Rectangle(x + 4, y + 4,
												 width - 8, height - 8),
						 ForeColor, BackColor);
				}

				// Get the text layout rectangle.
				RectangleF layout = new RectangleF
					(x + 2, y + 2, width - 4, height - 4);
				if(entered && pressed)
				{
					layout.Offset(1.0f, 1.0f);
				}

				// Draw the text on the button.
				String text = Text;
				Font font = Font;
				if(text != null && text != String.Empty)
				{
					if(Enabled)
					{
						Brush brush = new SolidBrush(ForeColor);
						graphics.DrawString(text, font, brush, layout, format);
						brush.Dispose();
					}
					else
					{
						ControlPaint.DrawStringDisabled
							(graphics, text, font, BackColor, layout, format);
					}
				}
			}

	// Redraw the button after a state change.
	private void Redraw()
			{
				// Bail out if the button is not currently visible.
				if(!Visible || !IsHandleCreated)
				{
					return;
				}

				// Redraw the button.
				Graphics graphics = CreateGraphics();
				Draw(graphics);
				graphics.Dispose();
			}

	// Override events from the "Control" class.
	protected override void OnEnabledChanged(EventArgs e)
			{
				Redraw();
				base.OnEnabledChanged(e);
			}
	protected override void OnGotFocus(EventArgs e)
			{
				hasFocus = true;
				Redraw();
				base.OnGotFocus(e);
			}
	[TODO]
	protected override void OnKeyDown(KeyEventArgs e)
			{
				// TODO
				base.OnKeyDown(e);
			}
	[TODO]
	protected override void OnKeyUp(KeyEventArgs e)
			{
				// TODO
				base.OnKeyUp(e);
			}
	[TODO]
	protected override void OnLostFocus(EventArgs e)
			{
				hasFocus = false;
				Redraw();
				base.OnLostFocus(e);
			}
	protected override void OnMouseDown(MouseEventArgs e)
			{
				if(button == MouseButtons.None)
				{
					button = e.Button;
					pressed = true;
					Redraw();
				}
				base.OnMouseDown(e);
			}
	protected override void OnMouseEnter(EventArgs e)
			{
				entered = true;
				Redraw();
				base.OnMouseEnter(e);
			}
	protected override void OnMouseLeave(EventArgs e)
			{
				entered = false;
				Redraw();
				base.OnMouseLeave(e);
			}
	protected override void OnMouseMove(MouseEventArgs e)
			{
				base.OnMouseMove(e);
			}
	protected override void OnMouseUp(MouseEventArgs e)
			{
				if(button == e.Button)
				{
					button = MouseButtons.None;
					pressed = false;
					Redraw();
				}
				base.OnMouseUp(e);
			}
	protected override void OnPaint(PaintEventArgs e)
			{
				Draw(e.Graphics);
				base.OnPaint(e);
			}
	protected override void OnParentChanged(EventArgs e)
			{
				Redraw();
				base.OnParentChanged(e);
			}
	protected override void OnTextChanged(EventArgs e)
			{
				Redraw();
				base.OnTextChanged(e);
			}
	protected override void OnRightToLeftChanged(EventArgs e)
			{
				SetStringFormat();
				Redraw();
				base.OnRightToLeftChanged(e);
			}
	protected override void OnVisibleChanged(EventArgs e)
			{
				if(!Visible)
				{
					entered = false;
					pressed = false;
					hasFocus = false;
					button = MouseButtons.None;
				}
				base.OnVisibleChanged(e);
			}

#if !CONFIG_COMPACT_FORMS

	// Process a message.
	public override void WndProc(ref Message m)
			{
				base.WndProc(ref m);
			}

#endif // !CONFIG_COMPACT_FORMS

}; // class ButtonBase

}; // namespace System.Windows.Forms
