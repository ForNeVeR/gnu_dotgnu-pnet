/*
 * RadioButton.cs - Implementation of the
 *			"System.Windows.Forms.RadioButton" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 *
 * Contributions from Mario Luca Bernardi
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

namespace System.Windows.Forms
{

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using System.Windows.Forms;
using System.Windows.Forms.Themes;

public class RadioButton : ButtonBase
{
	// Internal state.
	private Appearance appearance;
	private ContentAlignment checkAlign;
	private Rectangle content;
	private bool autoCheck;
	private bool isChecked;
	private bool needsLayout;
	private int checkX;
	private int checkY;

	private static readonly int checkSize = 13;

	// Constructor.
	public RadioButton() : base()
			{
				appearance = Appearance.Normal;
				checkAlign = ContentAlignment.MiddleLeft;
				TextAlign = ContentAlignment.MiddleLeft;
				entered = false;
				pressed = false;
				autoCheck = true;
				isChecked = false;
				needsLayout = true;
			}





#if !CONFIG_COMPACT_FORMS
	// Get or set the appearance of this radio button control.
	public Appearance Appearance
			{
				get { return appearance; }
				set
				{
					if (appearance != value)
					{
						if (!Enum.IsDefined(typeof(Appearance), value))
						{
						#if CONFIG_COMPONENT_MODEL
							throw new InvalidEnumArgumentException
								("Appearance", (int)value, typeof(Appearance));
						#else
							throw new ArgumentException
								("Appearance = "+(int)value);
						#endif
						}
						appearance = value;
						needsLayout = true;
						Redraw();
						OnAppearanceChanged(EventArgs.Empty);
					}
				}
			}

	// Get or set the "auto check" style for this radio button.
	public bool AutoCheck
			{
				get { return autoCheck; }
				set
				{
					if (autoCheck != value)
					{
						autoCheck = value;
					}
				}
			}

	// Get or set alignment of the check box on this radio button control.
	public ContentAlignment CheckAlign
			{
				get { return checkAlign; }
				set
				{
					if (checkAlign != value)
					{
						if (!Enum.IsDefined(typeof(ContentAlignment), value))
						{
						#if CONFIG_COMPONENT_MODEL
							throw new InvalidEnumArgumentException
								("CheckAlign", (int)value,
								 typeof(ContentAlignment));
						#else
							throw new ArgumentException
								("CheckAlign = "+(int)value);
						#endif
						}
						checkAlign = value;
						needsLayout = true;
						Redraw();
					}
				}
			}
#endif

	// Get or set the checked state as a simple boolean value.
	public bool Checked
			{
				get { return isChecked; }
				set
				{
					if (isChecked != value)
					{
						isChecked = value;
						Redraw();
						OnCheckedChanged(EventArgs.Empty);
					}
				}
			}

#if !CONFIG_COMPACT_FORMS
	// Gets the required creation parameters when the control handle is
	// created.
	protected override CreateParams CreateParams
			{
				get { return base.CreateParams; }
			}
#endif

#if !CONFIG_COMPACT_FORMS
	// Gets the default size of the control.
	protected
#else
	internal
#endif
	override Size DefaultSize
			{
				get { return new Size(104, 24); }
			}

#if !CONFIG_COMPACT_FORMS
	// Gets or sets the alignment of the text on the RadioButton control.
	public
#else
	internal
#endif
	override ContentAlignment TextAlign
			{
				get { return base.TextAlign; }
				set { base.TextAlign = value; }
			}




	// Calculate the current state of the button for its visual appearance.
	internal override ButtonState CalculateState()
			{
				ButtonState state = ButtonState.Normal;

				if (FlatStyle == FlatStyle.Flat ||
				    (FlatStyle == FlatStyle.Popup && !entered))
				{
					state |= ButtonState.Flat;
				}

				if (isChecked)
				{
					state |= ButtonState.Checked;
					if (appearance == appearance.Button)
					{
						state |= ButtonState.Pushed;
					}
				}

				if (Enabled)
				{
					if (entered && pressed)
					{
						state |= ButtonState.Pushed;
					}
				}
				else
				{
					state |= ButtonState.Inactive;
				}
				return state;
			}

#if !CONFIG_COMPACT_FORMS
	protected override AccessibleObject CreateAccessibilityInstance()
			{
				return base.CreateAccessibilityInstance();
			}
#endif

	// Draw the button in its current state on a Graphics surface.
	internal override void Draw(Graphics graphics)
			{
				// calculate ButtonState needed by DrawRadioButton
				ButtonState state = CalculateState();
				StringFormat format = GetStringFormat();

				if (needsLayout) { LayoutElements(); }

				if (appearance == Appearance.Button)
				{
					ThemeManager.MainPainter.DrawButton
						(graphics, 0, 0, content.Width, content.Height,
						 state, ForeColor, BackColor, false);
				}
				else
				{
					using (Brush bg = CreateBackgroundBrush())
					{
						ThemeManager.MainPainter.DrawRadioButton
							(graphics, checkX, checkY, checkSize, checkSize,
							 state, ForeColor, BackColor, bg);
					}
				}

				// TODO: image drawing

				Rectangle rect = content;
				if (appearance == Appearance.Button)
				{
					int x = content.X;
					int y = content.Y;
					int width = content.Width;
					int height = content.Height;
					x += 2;
					y += 2;
					width -= 4;
					height -= 4;
					if ((state & ButtonState.Pushed) != 0)
					{
						++x;
						++x;
					}
					rect = new Rectangle(x, y, width, height);
				}

				if (Enabled)
				{
					using (Brush brush = new SolidBrush(ForeColor))
					{
						graphics.DrawString(Text, Font, brush, rect, format);
					}
				}
				else
				{
					ControlPaint.DrawStringDisabled(graphics, Text, Font, ForeColor, rect, format);
				}
			}

	// Layout the elements of this radio button control.
	private void LayoutElements()
			{
				needsLayout = false;

				Size clientSize = ClientSize;
				int width = clientSize.Width;
				int height = clientSize.Height;

				if (appearance == Appearance.Button)
				{
					checkX = 0;
					checkY = 0;
					content = new Rectangle(0, 0, width, height);
				}
				else
				{
					switch (checkAlign)
					{
						case ContentAlignment.TopLeft:
						{
							checkX = 0;
							checkY = 0;
							content = new Rectangle(checkSize+3, 0,
							                        width-checkSize-3, height);
						}
						break;

						case ContentAlignment.TopCenter:
						{
							checkX = (width/2) - (checkSize/2);
							checkY = 0;
							content = new Rectangle(0, checkSize+3,
							                        width, height-checkSize-3);
						}
						break;

						case ContentAlignment.TopRight:
						{
							checkX = width - checkSize - 1;
							checkY = 0;
							content = new Rectangle(0, 0, width-checkX, height);
						}
						break;

						case ContentAlignment.MiddleLeft:
						{
							checkX = 0;
							checkY = (height/2) - (checkSize/2);
							content = new Rectangle(checkSize+3, 0,
							                        width-checkSize-3, height);
						}
						break;

						case ContentAlignment.MiddleCenter:
						{
							// for this alignment, the text is placed under
							// the check box of the radio button
							checkX = (width/2) - (checkSize/2);
							checkY = (height/2) - (checkSize/2);
							content = new Rectangle(0, 0, width, height);
						}
						break;

						case ContentAlignment.MiddleRight:
						{
							checkX = width - checkSize - 1;
							checkY = (height/2) - (checkSize/2);
							content = new Rectangle(0, 0, width-checkX, height);
						}
						break;

						case ContentAlignment.BottomLeft:
						{
							checkX = 0;
							checkY = height - checkSize - 1;
							content = new Rectangle(checkSize+3, 0,
							                        width-checkSize-3, height);
						}
						break;

						case ContentAlignment.BottomCenter:
						{
							checkX = (width/2) - (checkSize/2);
							checkY = height - checkSize - 1;
							content = new Rectangle(0, 0,
							                        width, height-checkSize-3);
						}
						break;

						case ContentAlignment.BottomRight:
						{
							checkX = width - checkSize - 1;
							checkY = height - checkSize - 1;
							content = new Rectangle(0, 0, width-checkX, height);
						}
						break;
					}
				}
			}

	// Raises the AppearanceChanged event.
	protected virtual void OnAppearanceChanged(EventArgs e)
			{
				if (AppearanceChanged != null)
				{
					AppearanceChanged(this, e);
				}
			}

	// Raises the CheckedChanged event.
	protected virtual void OnCheckedChanged(EventArgs e)
			{
				if (CheckedChanged != null)
				{
					CheckedChanged(this, e);
				}
			}

	// Raises the Click event.
	protected override void OnClick(EventArgs e)
			{
				if (!isChecked && autoCheck && Enabled)
				{
					if (Parent != null)
					{
						foreach (Control c in Parent.Controls)
						{
							RadioButton rb = c as RadioButton;
							if ((rb != null) && (rb != this))
							{
								rb.Checked = false;
							}
						}
					}
					Checked = true;
				}
				base.OnClick(e);
			}

	// Raises the HandleCreated event.
	protected override void OnHandleCreated(EventArgs e)
			{
				base.OnHandleCreated(e);
			}

	protected override void OnMouseDown(MouseEventArgs e)
			{
				pressed = true;
				Redraw();
				base.OnMouseDown(e);
			}

	// Override MouseEnter event from ButtonBase
	protected override void OnMouseEnter(EventArgs e)
			{
				entered = true;
				Redraw();
				base.OnMouseEnter(e);
			}

	// Override MouseLeave event from ButtonBase
	protected override void OnMouseLeave(EventArgs e)
			{
				entered = false;
				Redraw();
				base.OnMouseLeave(e);
			}

	// Raises the MouseUp event.
	protected override void OnMouseUp(MouseEventArgs e)
			{
				pressed = false;
				OnClick(EventArgs.Empty);
				base.OnMouseUp(e);
			}

	// Clicks this radio button.
	public void PerformClick()
			{
				OnClick(EventArgs.Empty);
			}

	// Processes a mnemonic character.
	protected override bool ProcessMnemonic(char charCode)
			{
				return false;
			}

	// Redraw the button after a state change.
	private void Redraw()
			{
				// Bail out if the button is not currently visible.
				if (!Visible || !IsHandleCreated) { return; }

				// Redraw the button.
				using (Graphics graphics = CreateGraphics())
				{
					Draw(graphics);
				}
			}

	public override string ToString()
			{
				return GetType().FullName.ToString() + ", Checked: " + Checked.ToString();
			}







	// Occurs when the value of the Appearance property changes.
	public event EventHandler AppearanceChanged;

	// Occurs when the value of the Checked property changes.
	public event EventHandler CheckedChanged;

}; // class RadioButton

}; // namespace System.Windows.Forms
