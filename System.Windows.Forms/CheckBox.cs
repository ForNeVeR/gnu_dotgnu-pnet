/*
 * CheckBox.cs - Implementation of the
 *			"System.Windows.Forms.CheckBox" class.
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
using System.Drawing.Drawing2D;
using System.ComponentModel;
using System.Windows.Forms;

namespace System.Windows.Forms
{

public class CheckBox : ButtonBase
{
	// Internal result.
	private Appearance appearance;
	private ContentAlignment checkAlign;
	private bool autoCheck;
	private bool threeState;
	private CheckState state;
	private ImeMode defaultImeMode;
	private Size defaultSize;
	private int checkX;
	private int checkY;
	private int checkSize = 13;

	// Contructor.
	public CheckBox()
			{
				Appearance = Appearance.Normal;
				defaultSize = new Size(104, 24);
				Size = defaultSize;
				checkAlign = ContentAlignment.MiddleLeft;
				TextAlign = ContentAlignment.MiddleLeft;
			}

	internal override void Draw(Graphics graphics)
			{
				DrawBox(graphics);
				DrawText(graphics);
			}

	private void DrawBox(Graphics graphics)
			{
				int checkX = 0;
				int checkY = 0;
		
				switch (checkAlign)
				{
					case ContentAlignment.BottomCenter:
						checkX = (Width / 2) - (checkSize / 2);
						checkY = Height - checkSize - 1;
						break;
					case ContentAlignment.BottomLeft:
						checkX = 0;
						checkY = Height - checkSize - 1;
						break;
					case ContentAlignment.BottomRight:
						checkX = Width - checkSize - 1;
						checkY = Height - checkSize - 1;
						break;
					case ContentAlignment.MiddleCenter:
						checkX = (Width / 2) - (checkSize / 2);
						checkY = (Height / 2) - (checkSize / 2);
						break;
					case ContentAlignment.MiddleLeft:
						checkX = 0;
						checkY = (Height / 2) - (checkSize / 2);
						break;
					case ContentAlignment.MiddleRight:
						checkX = Width - checkSize - 1;
						checkY = (Height / 2) - (checkSize / 2);
						break;
					case ContentAlignment.TopCenter:
						checkX = (Width / 2) - (checkSize / 2);
						checkY = 0;
						break;
					case ContentAlignment.TopLeft:
						checkX = 0;
						checkY = 0;
						break;
					case ContentAlignment.TopRight:
						checkX = Width - checkSize - 1;
						checkY = 0;
						break;
				}
				ButtonState checkState = ButtonState.Normal;
				if (state == CheckState.Checked == true)
				{
					checkState = ButtonState.Checked;
				}
				ControlPaint.DrawCheckBox(graphics, checkX, checkY, checkSize, checkSize, checkState);
			}

	private void DrawText(Graphics graphics)
			{
				int x = 0;
				int y = 2;
				int width = Width - 2;
				int height = Height - 4;
		
				SizeF textSize = graphics.MeasureString(Text, Font);
				StringFormat format = new StringFormat();
				format.Alignment = StringAlignment.Near;
				format.LineAlignment = StringAlignment.Far;

				switch (checkAlign)
				{
					case ContentAlignment.BottomCenter:
						height = checkY + 2;
						break;
					case ContentAlignment.BottomLeft:
						x = checkX + checkSize + 3;
						width -= x;
						break;
					case ContentAlignment.BottomRight:
						width -= checkSize;
						break;
					case ContentAlignment.MiddleCenter:
						break;
					case ContentAlignment.MiddleLeft:
						x = checkX + checkSize + 3;
						width -= x;
						break;
					case ContentAlignment.MiddleRight:
						x = checkX + checkSize + 3;
						width -= x;
						break;
					case ContentAlignment.TopCenter:
						y = checkY + checkSize + 3;
						height -= y - 2;
						break;
					case ContentAlignment.TopLeft:
						x = checkX + checkSize + 3;
						width -= x;
						break;
					case ContentAlignment.TopRight:
						width -= checkSize;
						break;
				}

				switch (TextAlign)
				{
					case ContentAlignment.BottomCenter:
						format.Alignment = StringAlignment.Center;
						format.LineAlignment = StringAlignment.Far;
						break;
					case ContentAlignment.BottomLeft:
						format.Alignment = StringAlignment.Near;
						format.LineAlignment = StringAlignment.Far;
						break;
					case ContentAlignment.BottomRight:
						format.Alignment = StringAlignment.Far;
						format.LineAlignment = StringAlignment.Far;
						break;
					case ContentAlignment.MiddleCenter:
						format.Alignment = StringAlignment.Center;
						format.LineAlignment = StringAlignment.Center;
						break;
					case ContentAlignment.MiddleLeft:
						format.LineAlignment = StringAlignment.Center;
						break;
					case ContentAlignment.MiddleRight:
						format.Alignment = StringAlignment.Far;
						format.LineAlignment = StringAlignment.Center;
						break;
					case ContentAlignment.TopCenter:
						format.Alignment = StringAlignment.Center;
						format.LineAlignment = StringAlignment.Near;
						break;
					case ContentAlignment.TopLeft:
						format.Alignment = StringAlignment.Near;
						format.LineAlignment = StringAlignment.Near;
						break;
					case ContentAlignment.TopRight:
						format.Alignment = StringAlignment.Far;
						format.LineAlignment = StringAlignment.Near;
						break;
				}
				Brush brush = new SolidBrush(ForeColor);
				Rectangle rect = new Rectangle(x, y, width, height);
				graphics.DrawString(Text, Font, brush, rect, format);
				brush.Dispose();
			}

	// Gets or sets the value that determines the appearance of a check box control.
	public Appearance Appearance 
			{
				get
				{
					return appearance; 
				}
				set
				{
					appearance = value;
				}
			}

	// Get or set the "auto check" style for this check box.
	public bool AutoCheck
			{
				get
				{
					return autoCheck;
				}
				set
				{
					if(autoCheck != value)
					{
						autoCheck = value;
					}
				}
			}

	// Gets or sets the horizontal and vertical alignment of a check box on a check box control.
	public ContentAlignment CheckAlign 
			{
				get
				{
					return checkAlign;
				}
				set
				{
					checkAlign = value;
					Invalidate();
				}
			}

	// Get or set the checked state as a simple boolean value.
	public bool Checked
			{
				get
				{
					return (CheckState != CheckState.Unchecked);
				}
				set
				{
					if(value)
					{
						CheckState = CheckState.Checked;
					}
					else
					{
						CheckState = CheckState.Unchecked;
					}
				}
			}

	// Get or set the check box state.
	public CheckState CheckState
			{
				get
				{
					return state;
				}
				set
				{
					if(state != value)
					{
						state = value;
						OnCheckStateChanged(EventArgs.Empty);
					}
				}
			}

	// Gets or sets the site of the control.
	public override ISite Site 
			{
				get 
				{
					return base.Site;
				}
				set
				{
					base.Site = value;
				}
			}

	//  Gets or sets the alignment of the text on the checkbox control.
	public override ContentAlignment TextAlign 
			{
				get
				{
					return base.TextAlign;
				}
				set
				{
					base.TextAlign = value;
					Invalidate();
				}
			}

	// Get or set the "three state" style for this check box.
	public bool ThreeState
			{
				get
				{
					return threeState;
				}
				set
				{
					if(threeState != value)
					{
						threeState = value;
					}
				}
			}
	// Occurs when the value of the Appearance property changes.
	public event EventHandler AppearanceChanged;
	
	// Occurs when the value of the Checked property changes.
	public event EventHandler CheckedChanged;

	// Event that is emitted when the check state changes.
	public event EventHandler CheckStateChanged
			{
				add
				{
					AddHandler(EventId.CheckStateChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.CheckStateChanged, value);
				}
			}
	// Gets the required creation parameters when the control handle is created.
	protected override CreateParams CreateParams 
			{
				get
				{
					return base.CreateParams;
				}
			}
	// Gets the default Input Method Editor (IME) mode supported by this control.
	protected override ImeMode DefaultImeMode 
			{
				get
				{
					return defaultImeMode;
				}
			}
	// Gets the default size of the control.	
	protected override Size DefaultSize 
			{
				get
				{
					return defaultSize;
				}
			}

	protected override AccessibleObject CreateAccessibilityInstance()
			{
				return base.CreateAccessibilityInstance();
			}

	// Raises the AppearanceChanged event.
	protected virtual void OnAppearanceChanged(EventArgs e)
			{
				if (AppearanceChanged != null)
					AppearanceChanged(this, e);
			}

	// Raises the CheckedChanged event.
	protected virtual void OnCheckedChanged(EventArgs e)
			{
				if (CheckedChanged != null)
					CheckedChanged(this, e);
		
			}

	// Emit the CheckStateChanged event.
	protected virtual void OnCheckStateChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.CheckStateChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	
	// Raises the Click event.
	protected override void OnClick(EventArgs e)
			{
				base.OnClick(e);
			}

	// Raises the HandleCreated event.
	protected override void OnHandleCreated(EventArgs e)
			{
				base.OnHandleCreated(e);
			}

	// Raises the MouseUp event.
	protected override void OnMouseUp(MouseEventArgs mevent)
			{
				if (state == CheckState.Unchecked)
				{
					state = CheckState.Checked;
				}
				else
				{
					state = CheckState.Unchecked;
				}
				Invalidate();
				
				this.OnCheckedChanged(mevent);
				this.OnCheckStateChanged(mevent);
				base.OnMouseUp(mevent);
			}

	// Processes a mnemonic character.
	protected override bool ProcessMnemonic(char charCode)
			{
				return false;
			}

	// Set the check box state - used by the toolkit in response to events.
	internal void SetCheckState(CheckState state)
			{
				this.state = state;
			}

}; // class CheckBox

}; // namespace System.Windows.Forms
