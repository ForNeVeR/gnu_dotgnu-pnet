/*
 * TextBox.cs - Implementation of the
 *			"System.Windows.Forms.TextBox" class.
 *
 * Copyright (C) 2003 Neil Cawse
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
	private bool acceptsReturn;
	private char passwordChar;
	private CharacterCasing characterCasing;
	private ScrollBars scrollBars;
	private HorizontalAlignment textAlign;

	private bool mouseDown;
	private Brush selectedForeBrush, selectedBackBrush;
	private Brush foreBrush, backBrush;
	// The position and drawing information for each item
	protected LayoutInfo layout;
	// Start of a selection
	protected int selectionStartActual;
	private Region selectedRegion;
	// Length of the selection, could be negative
	protected int selectionLengthActual;
	
	private bool caretHiding = false;
	private bool caretVisible = false;
	// Position to draw caret
	private Rectangle caretBounds;
	private Timer caretFlash;
	private Pen caretPen;
	private Pen caretPenHiding;

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

		textAlign = HorizontalAlignment.Left;

		BackColor = SystemColors.Window;
		caretFlash = new Timer();
		//TODO Get this value from SystemInformation
		// Handle when System information changes for interval & system colors
		caretFlash.Interval = 500;
		caretFlash.Tick += new EventHandler(caretFlash_Tick);
		caretFlash.Enabled = true;
		caretPen = new Pen(SystemColors.Highlight);
		caretPenHiding = new Pen(BackColor);
	}

	// TODO Handle this
	// Gets or sets a value indicating whether pressing ENTER in a multiline TextBox control creates a new line of text in the control or activates the default button for the form.
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

	// TODO Handle this
	// Gets or sets whether the TextBox control modifies the case of characters as they are typed.
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

	// TODO Handle this
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

	// TODO Handle this
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
			SetTextInternal( value);
			// Set the position to the end
			SelectInternal( value.Length, 0);
			SetCaretPosition(GetSelectionStart()+ GetSelectionLength());
		}
	}

	// Called to change the text. Invalidates what needs to but doesnt change the selection point or caret
	protected override void SetTextInternal( string text)
	{
		// Layout the new text. Compare with old layout, Creating a region for areas that must be updated.
		bool prevLayout = layout != null;
		LayoutInfo oldLayout = null;
		if (prevLayout)
			oldLayout = (LayoutInfo)layout.Clone();
		string oldText = base.Text;
		base.Text = text;
		LayoutFromText(CreateGraphics());
		if (prevLayout)
		{
			Region invalid = new Region();
			invalid.MakeEmpty();
			for (int i=0;i < oldText.Length || i < text.Length;i++)
			{
				if (i >= oldText.Length)
					invalid.Union( layout.Items[i].bounds);
				else if (i >= text.Length)
					invalid.Union( oldLayout.Items[i].bounds);
				else if (Text[i] != oldText[i] || oldLayout.Items[i].bounds != layout.Items[i].bounds)
				{
					invalid.Union( layout.Items[i].bounds);
					invalid.Union( oldLayout.Items[i].bounds);
				}
			}
			Invalidate(invalid);
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
	protected override void OnGotFocus(EventArgs e)
	{
		// Perform the regular focus handling.
		base.OnGotFocus(e);

		// If nothing is selected, then select everything.
		if (selectionLengthActual == 0)
		{
			SelectInternal(0, Text.Length);
			SetCaretPosition(Text.Length);
		}
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
		if (e.KeyCode == Keys.Left && GetSelectionStart()>0)
		{
			SelectInternal( GetSelectionStart() - 1, 0);
			SetCaretPosition(GetSelectionStart()+ GetSelectionLength());
			Update();
			e.Handled = true;
		}
		else if (e.KeyCode == Keys.Right && GetSelectionStart() < Text.Length)
		{
			SelectInternal( GetSelectionStart() + 1, 0);
			SetCaretPosition(GetSelectionStart()+ GetSelectionLength());
			Update();
			e.Handled = true;
		}
		else if (e.KeyCode == Keys.Back && GetSelectionStart()>0)
		{
			if (GetSelectionLength() > 0)
				SetSelectionText("");
			else
			{
				SetTextInternal(Text.Substring(0, GetSelectionStart() -1) + Text.Substring(GetSelectionStart()));
				SetSelectionStart(GetSelectionStart()-1);
			}
			SelectInternal(GetSelectionStart(),0);
			SetCaretPosition(GetSelectionStart()+ GetSelectionLength());
			Update();
			e.Handled = true;
		}
		else if (e.KeyCode == Keys.Delete && GetSelectionStart() < Text.Length)
		{
			if (GetSelectionLength() > 0)
				SetSelectionText("");
			else
				SetTextInternal(Text.Substring(0, GetSelectionStart()) + Text.Substring(GetSelectionStart() + 1));
			SelectInternal(GetSelectionStart(),0);
			SetCaretPosition(GetSelectionStart()+ GetSelectionLength());
			Update();
			e.Handled = true;
		}

	}

	// Handle "KeyPress" events for the text box.
	private void HandleKeyPress(Object sender, KeyPressEventArgs e)
	{
		char c = e.KeyChar;
		if (c != (char)Keys.Delete && c != (char)Keys.Back)
		{
			if (c==(char)Keys.Enter)
				c = (char)Keys.LineFeed;
			SetSelectionText( c.ToString() );
			SelectInternal( SelectionStart + 1, 0);
			SetCaretPosition(SelectionStart);
			Update();
		}
	}

	// Handle "MouseDown" events for the text box.
	private void HandleMouseDown(Object sender, MouseEventArgs e)
	{
		ProcessMouse(e);
	}

	// Handle "MouseMove" events for the text box.
	private void HandleMouseMove(Object sender, MouseEventArgs e)
	{
		ProcessMouse(e);
	}

	// Handle "Paint" events for the text box.
	// In our implementation NO painting happens outside of the paint event. This might change because it might not update fast enough
	private void HandlePaint(Object sender, PaintEventArgs e)
	{
		e.Graphics.FillRectangle(BackBrush, new Rectangle(0,0,Width, Height));
		// For testing:
		//e.Graphics.FillRectangle(new SolidBrush(Color.Red), new Rectangle(0,0,Width, Height));
		//System.Threading.Thread.Sleep(100);
		ControlPaint.DrawBorder3D( e.Graphics, new Rectangle(0,0, Width, Height), Border3DStyle.Sunken);
		DrawText( e );
		DrawCaret( e );
	}

	private Brush SelectedBackBrush
	{
		get
		{
			if (selectedBackBrush == null)
				selectedBackBrush = new SolidBrush(SystemColors.Highlight);
			return selectedBackBrush;
		}
	}

	private Brush ForeBrush
	{
		get
		{
			if (foreBrush == null)
				foreBrush = new SolidBrush(ForeColor);
			return foreBrush;
		}
	}

	private Brush SelectedForeBrush
	{
		get
		{
			if (selectedForeBrush == null)
				selectedForeBrush = new SolidBrush(SystemColors.HighlightText);
			return selectedForeBrush;
		}
	}


	private Brush BackBrush
	{
		get
		{
			if (backBrush == null)
				backBrush = new SolidBrush(BackColor);
			return backBrush;
		}
	}

	// Create the drawLayout from the text
	protected void LayoutFromText( Graphics g)
	{
		layout = new LayoutInfo();
		layout.Items = new LayoutInfo.Item[Text.Length];

		StringFormat format = new StringFormat();
		if (RightToLeft == RightToLeft.Yes)
			format.FormatFlags |= StringFormatFlags.DirectionRightToLeft;
		if (!Multiline)
			format.FormatFlags |= StringFormatFlags.NoWrap;
		if (textAlign == HorizontalAlignment.Left)
			format.Alignment = StringAlignment.Near;
		else if (textAlign == HorizontalAlignment.Right)
			format.Alignment = StringAlignment.Far;
		else if (textAlign == HorizontalAlignment.Center)
			format.Alignment = StringAlignment.Center;
		
		// TODO
		// In the framework, only MeasureCharacterRanges properly measures a string. The way we have to use it here means we create a Character Range object for each character. Inefficient.
	
		CharacterRange[] range = new CharacterRange[Text.Length];
		for (int i = 0; i < Text.Length; i++)
			range[i] = new CharacterRange(i, 1);
		format.SetMeasurableCharacterRanges(range);
		if (range.Length > 0)
		{
			Region[] bounds = g.MeasureCharacterRanges(Text, Font, new Rectangle(2, 2, Width-4, Height-4), format);
			for (int i=0; i<Text.Length;i++) 
			{
				LayoutInfo.Item item = new LayoutInfo.Item();
				RectangleF rect = bounds[i].GetBounds( g);
				item.visible = !rect.IsEmpty;
				if (item.visible)
					item.bounds = Rectangle.Truncate( rect);
				layout.Items[i] = item;
			}
		}	
	}

	protected class LayoutInfo : ICloneable
	{
		public Item[] Items;

		// The position and info for each draw item/ character
		public class Item
		{
			public Rectangle bounds;
			public bool selected;
			public bool visible;
		}

		public object Clone()
		{
			LayoutInfo layout = new LayoutInfo();
			layout.Items = (Item[])Items.Clone();
			return layout;
		}

	}

	// Uses the current selection settings to set what is selected in drawLayout
	// This only invalidates the characters that have changed
	internal override void SelectInternal(int start, int length)
	{
		if (start == selectionStartActual && length == selectionLengthActual)
			return;
		Region newRegion = new Region(RectangleF.Empty);
		selectionStartActual = start;
		selectionLengthActual = length;
		for (int i=0; i<Text.Length;i++) 
		{
			bool selected = (i>=SelectionStart && i < SelectionStart + SelectionLength);
			layout.Items[i].selected = selected;
			if (selected)
				newRegion.Union(layout.Items[i].bounds);
		}
		// Find the region we need to invalidate by Xoring with old
		if (selectedRegion != null)
		{
			Region invalidRegion = newRegion.Clone();
			invalidRegion.Xor(selectedRegion);
			RectangleF b1 = invalidRegion.GetBounds( CreateGraphics());
			Invalidate(invalidRegion);				
		}
		selectedRegion = newRegion;
		RectangleF b = selectedRegion.GetBounds( CreateGraphics());
	}

	// Get the length of the selection.
	internal override int GetSelectionLength()
	{
		return Math.Abs(selectionLengthActual);
	}

	// Get the start of the selection.
	// Our length could be negative
	internal override int GetSelectionStart()
	{
		return Math.Min(selectionStartActual, selectionStartActual + selectionLengthActual);
	}

	// Set the start of the selection and the caret position
	internal override void SetSelectionStart(int start)
	{
		SelectInternal(start, selectionLengthActual);
		SetCaretPosition(GetSelectionStart() + GetSelectionLength());
	}

	// Sets the end of the selection and the caret position
	internal override void SetSelectionLength(int length)
	{
		SelectInternal(selectionStartActual, length);
		SetCaretPosition(GetSelectionStart() + GetSelectionLength());
	}

	// These are the two methods that need to be overridden if the client size is different to the actual size (borders etc.)
	protected override void SetClientSizeCore(int x, int y)
	{
		base.SetClientSizeCore (x - 4, y - 4);
	}

	// Convert a client size into a window bounds size.
	internal override Size ClientToBounds(Size size)
	{
		return new Size(size.Width + 4, size.Height + 4);
	}

	// Paint the text using layout information
	private void DrawText(PaintEventArgs e)
	{
		for (int i=0; i<Text.Length;i++) 
		{
			LayoutInfo.Item item = layout.Items[i];
			if (item.visible /*&& e.ClipRectangle.IntersectsWith(item.bounds)*/)
			{
				Brush back, fore;
				if (item.selected)
				{
					back = SelectedBackBrush;
					fore = SelectedForeBrush;
				}
				else
				{
					back = BackBrush;
					fore = ForeBrush;
				}
				e.Graphics.FillRectangle(back, item.bounds);
				e.Graphics.DrawString(Text.Substring(i,1),Font, fore, item.bounds.Location);
			}
		}
	}


	private void ProcessMouse(MouseEventArgs e)
	{
		if (Enabled) 
		{
			if (mouseDown)
			{
				if (e.Button == MouseButtons.Left)
				{
					int closest = SetCaretPosition(new Point(e.X,e.Y));
					SelectInternal(selectionStartActual, closest - selectionStartActual);
				}
				else 
				{
					mouseDown = false;
					Capture = false;
				}
					
			}
			else
			{
				if (e.Button == MouseButtons.Left) 
				{
					int closest = SetCaretPosition(new Point(e.X,e.Y));
					SelectInternal( closest,0);

					mouseDown = true;
					Capture = true;
				}
				Cursor = Cursors.IBeam;
			}
		}
		else
			mouseDown = false;
	}

	// Called when painting the caret
	private void DrawCaret( PaintEventArgs e ) 
	{
		if (caretHiding)
			e.Graphics.DrawLine(caretPenHiding, caretBounds.Left, caretBounds.Top, caretBounds.Left, caretBounds.Bottom - 1);
		else
			e.Graphics.DrawLine(caretPen, caretBounds.Left, caretBounds.Top, caretBounds.Left, caretBounds.Bottom - 1);
	}

	// Set the caret bounds from a character position
	// Invalidate if needed
	private void SetCaretPosition( int position)
	{
		Rectangle newBounds;
		if (Text.Length == 0)
			newBounds = new Rectangle(0,0,0,Font.Height);
		else
		{
			if (position == Text.Length)
			{
				newBounds = layout.Items[position -1].bounds;
				newBounds = new Rectangle(newBounds.Right, newBounds.Top, 0, newBounds.Height);
			}
			else
			{
				newBounds = layout.Items[position].bounds;
				newBounds = new Rectangle(newBounds.Left, newBounds.Top, 0, newBounds.Height);
			}
		}

		SetCaretInvalidate( newBounds);
	}

	// When we change the caret position, we invalidate & reset the timer
	private void SetCaretInvalidate(Rectangle newBounds)
	{
		Region region = new Region(newBounds);
		region.Xor(caretBounds);
		Invalidate(region);
		caretBounds = newBounds;
		resetCaretTimer();
		CaretVisible = true;
	}

	// Set the caret bounds to the nearest position relative to pt. Returns nearest the position
	private int SetCaretPosition(Point pt)
	{
		int prevY = 0;
		int caretPosition = 0;
		Rectangle bounds = new Rectangle(0, 0, 0, Font.Height);
		Rectangle prevBounds = bounds;
		int i=0;
		for (; i<Text.Length;i++) 
		{
			if (layout.Items[i].visible)
			{
				bounds = layout.Items[i].bounds;
				caretPosition = i;
				if (pt.Y < bounds.Bottom && pt.X < bounds.Left + bounds.Width/2 )
				{
					bounds = new Rectangle(bounds.Left, bounds.Top, 0, bounds.Height);
					break;
				}
				// New Line
				if (bounds.Bottom > prevY)
				{
					if (pt.Y < prevY)
					{
						bounds = new Rectangle(prevBounds.Right, prevBounds.Top, 0, prevBounds.Height);
						// Move position back to end of previous line
						caretPosition--;
						break;
					}
					prevY = bounds.Bottom;
				}
				prevBounds = bounds;
			}
		}
	
		// If at the end set the bounds to the last visible item.
		if (i == Text.Length)
		{
			bounds = new Rectangle(prevBounds.Right, prevBounds.Top, 0, prevBounds.Height);
			caretPosition = Text.Length;
		}
		
		SetCaretInvalidate(bounds);
		return caretPosition;
	}
	
	// Each time the timer fires we flash the caret
	private void caretFlash_Tick(object sender, EventArgs e)
	{
		if (caretVisible)
		{
			caretHiding = !caretHiding;
			Invalidate(caretBounds);
		}
	}

	// Make sure the caret is not hiding and reset timer
	private void resetCaretTimer()
	{
		//Resets the flash timer
		//TODO XSharp Timer seems to hang
		/*caretFlash.Enabled = false;
		caretFlash.Enabled = caretVisible;*/
		caretHiding = false;
	}

	// Set wether or not to show the caret
	private bool CaretVisible 
	{
		set 
		{
			if (caretVisible != value && selectionStartActual>-1)
				Invalidate(caretBounds);
			caretVisible = value;
		}
	}


	public override bool CanUndo
	{
		get
		{
			return false;
		}
	}

	public override String[] Lines
	{
		get
		{
			return null;
		}
		set
		{
		}
	}



#if !CONFIG_COMPACT_FORMS

#endif // !CONFIG_COMPACT_FORMS

}; // class TextBox

}; // namespace System.Windows.Forms
