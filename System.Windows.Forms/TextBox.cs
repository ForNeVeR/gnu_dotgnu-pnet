/*
 * TextBox.cs - Implementation of the
 *			"System.Windows.Forms.TextBox" class.
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

public class TextBox : TextBoxBase
{
	// Internal state.
	private bool acceptsReturn;
	private char passwordChar;
	private CharacterCasing characterCasing;
	private ScrollBars scrollBars;
	private HorizontalAlignment textAlign;

	// Constructor.
	public TextBox()
			{
				// Trap interesting events.  We do it this way rather
				// than override virtual methods so that the published
				// TextBox API is maintained.
				KeyDown += new KeyEventHandler(HandleKeyDown);
				KeyPress += new KeyPressEventHandler(HandleKeyPress);
				MouseDown += new MouseEventHandler(HandleMouseDown);
				MouseMove += new MouseEventHandler(HandleMouseMove);
				Paint += new PaintEventHandler(HandlePaint);
			}

	// Get or set this object's properties.
	public bool AcceptsReturn
			{
				get
				{
					return acceptsReturn;
				}
				set
				{
					acceptsReturn = value;
				}
			}
	public CharacterCasing CharacterCasing
			{
				get
				{
					return characterCasing;
				}
				set
				{
					characterCasing = value;
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
					if(passwordChar != '\0')
					{
						return base.DefaultImeMode;
					}
					else
					{
						return ImeMode.Disable;
					}
				}
			}
	public char PasswordChar
			{
				get
				{
					return passwordChar;
				}
				set
				{
					passwordChar = value;
				}
			}
	public ScrollBars ScrollBars
			{
				get
				{
					return scrollBars;
				}
				set
				{
					scrollBars = value;
				}
			}
	public override int SelectionLength
			{
				get
				{
					return base.SelectionLength;
				}
				set
				{
					base.SelectionLength = value;
				}
			}
	public override String Text
			{
				get
				{
					return base.Text;
				}
				set
				{
					base.Text = value;
				}
			}
	public HorizontalAlignment TextAlign
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
						OnTextAlignChanged(EventArgs.Empty);
					}
				}
			}

	// Event that is emitted when the text alignment changes.
	public event EventHandler TextAlignChanged
			{
				add
				{
					AddHandler(EventId.TextAlignChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.TextAlignChanged, value);
				}
			}

	// Determine if a key is recognized by a control as an input key.
	protected override bool IsInputKey(Keys keyData)
			{
				// If this is a multi-line control, then determine
				// if we need to recognize the "Return" key.  We ignore
				// Alt-Return, but recognize Shift-Return and Ctrl-Return.
				if(Multiline)
				{
					if((keyData & Keys.Alt) == 0)
					{
						if((keyData & Keys.KeyCode) == Keys.Enter)
						{
							return acceptsReturn;
						}
					}
				}
				return base.IsInputKey(keyData);
			}

	// Override the "GotFocus" event.
	[TODO]
	protected override void OnGotFocus(EventArgs e)
			{
				// Perform the regular focus handling.
				base.OnGotFocus(e);

				// If nothing is selected, then select everything.
				// TODO
			}

	// Override the "HandleCreated" event.
	protected override void OnHandleCreated(EventArgs e)
			{
				// Nothing special to do in this implementation.
				base.OnHandleCreated(e);
			}

	// Override the "MouseUp" event.
	[TODO]
	protected override void OnMouseUp(MouseEventArgs e)
			{
				// TODO: OnClick and OnDoubleClick handling
				base.OnMouseUp(e);
			}

	// Raise the "TextAlignChanged" event.
	protected virtual void OnTextAlignChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.TextAlignChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}

	// Handle "KeyDown" events for the text box.
	private void HandleKeyDown(Object sender, KeyEventArgs e)
			{
				// TODO
			}

	// Handle "KeyPress" events for the text box.
	private void HandleKeyPress(Object sender, KeyPressEventArgs e)
			{
				// TODO
			}

	// Handle "MouseDown" events for the text box.
	private void HandleMouseDown(Object sender, MouseEventArgs e)
			{
				// TODO
			}

	// Handle "MouseMove" events for the text box.
	private void HandleMouseMove(Object sender, MouseEventArgs e)
			{
				// TODO
			}

	// Handle "Paint" events for the text box.
	private void HandlePaint(Object sender, PaintEventArgs e)
			{
				// TODO
			}

#if !CONFIG_COMPACT_FORMS

	// Process a message.
	protected override void WndProc(ref Message m)
			{
				base.WndProc(ref m);
			}

#endif // !CONFIG_COMPACT_FORMS

}; // class TextBox

}; // namespace System.Windows.Forms
