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
	// Length of the selection, could be negative
	protected int selectionLengthActual;
	private Region selectedRegion;
	
	// Whether the flashing caret is currently hiding
	private bool caretHiding = false;
	// Position to draw caret
	private Rectangle caretBounds;
	private Timer caretFlash;
	private Pen caretPen;
	private Pen caretPenHiding;

	// Maximum possible X/Y for a region
	private const int maxXY = 4194304;

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
		MultilineChanged +=new EventHandler(HandleMultilineChanged);

		textAlign = HorizontalAlignment.Left;

		BackColor = SystemColors.Window;
		caretFlash = new Timer();
		//TODO Get this value from SystemInformation
		// Handle when System information changes for interval & system colors
		caretFlash.Interval = 500;
		caretFlash.Tick += new EventHandler(CaretFlash_Tick);
		caretFlash.Enabled = true;
		caretPen = new Pen(SystemColors.Highlight);
		caretPenHiding = new Pen(BackColor);
		
		Text = string.Empty;
	}

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
			CaretReset();
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
				// Caret will change position
				CaretSetEndSelection();
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

	// Process when the control receives the focus
	protected override void OnEnter(EventArgs e)
	{
		// Perform the regular focus handling.
		base.OnEnter(e);
		CaretTimerReset();
	}

	// Process when the control loses the focus
	protected override void OnLeave(EventArgs e)
	{
		base.OnLeave (e);
		// Create a region containing the caret and all visible selected text
		Region invalidRegion = new Region(caretBounds);
		for (int i = 0; i < Text.Length; i++)
			if (layout.Items[i].charVisibility == LayoutInfo.Item.CharVisibility.Visible)
				invalidRegion.Union(new Region(layout.Items[i].bounds));
		Invalidate( invalidRegion);
	}

	protected override void OnFontChanged(EventArgs e)
	{
		base.OnFontChanged (e);
		if (!Multiline && AutoSize)
			Height = BorderOffset.Y * 2 + Font.Height;
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
			SelectInternal( GetSelectionStart() - 1, 0);
		else if (e.KeyCode == Keys.Right && GetSelectionStart() < Text.Length)
			SelectInternal( GetSelectionStart() + 1, 0);
		else if (e.KeyCode == Keys.Back && (GetSelectionStart() + GetSelectionLength())>0)
		{
			if (GetSelectionLength() > 0)
			{
				SetSelectionText("");
				SelectInternal(GetSelectionStart(),0);
			}
			else
			{
				SetTextInternal(Text.Substring(0, GetSelectionStart() -1) + Text.Substring(GetSelectionStart()));
				SelectInternal(GetSelectionStart()-1, 0);
			}
		}
		else if (e.KeyCode == Keys.Delete && GetSelectionStart() < Text.Length)
		{
			if (GetSelectionLength() > 0)
				SetSelectionText("");
			else
				SetTextInternal(Text.Substring(0, GetSelectionStart()) + Text.Substring(GetSelectionStart() + 1));
			SelectInternal(GetSelectionStart(),0);
		}
		else
			return;
		// If processed
		CaretSetEndSelection();
		// Force repaint
		Update();
		e.Handled = true;

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
			CaretSetPosition(SelectionStart);
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
		if (BorderStyle != BorderStyle.None)
			ControlPaint.DrawBorder3D( e.Graphics, new Rectangle(0,0, Width, Height), Border3DStyle.Sunken);
		DrawText( e );
		if (Focused)
			CaretDraw( e );
	}

	// Handle the event when multiline is changed.
	private void HandleMultilineChanged(object sender, EventArgs e)
	{
		// Force the control to relayout
		LayoutFromText();
		CaretReset();
	}

	// Used to get the border offset depending on border style
	private Point BorderOffset
	{
		get
		{
			if (BorderStyle == BorderStyle.None)
				return new Point(0,0);
			else
				return new Point(2,2);
		}
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
	protected void LayoutFromText()
	{
		using (Graphics g = CreateGraphics())
		{
			layout = new LayoutInfo();
			layout.Items = new LayoutInfo.Item[Text.Length];

			// Convert control settings to a StringFormat
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
		
			// We want to measure each character individually
			CharacterRange[] range = new CharacterRange[Text.Length];
			for (int i = 0; i < Text.Length; i++)
				range[i] = new CharacterRange(i, 1);
			format.SetMeasurableCharacterRanges(range);
			if (range.Length > 0)
			{
				// We are infinite in one direction
				Rectangle measureBounds;
				if (Multiline)
					measureBounds = new Rectangle(BorderOffset.X, BorderOffset.Y, Width-BorderOffset.X * 2, maxXY * 2);
				else
					measureBounds = new Rectangle(BorderOffset.X, BorderOffset.Y, maxXY * 2, Height-BorderOffset.Y*2);
				Region[] bounds = g.MeasureCharacterRanges(Text, Font, measureBounds, format);

				// Convert the MeasureCharacterRanges to LayoutInfo
				// MeasureCharacterRanges will return an empty rectangle for all characters
				// that are not visible. We need to figure out the positions of LF and spaces
				// that are swallowed
				Rectangle prevBounds = new Rectangle(CaretXFromAlign, BorderOffset.Y, 0, Font.Height);
				LayoutInfo.Item.CharVisibility prevVisibility = LayoutInfo.Item.CharVisibility.Visible;
				for (int i=0; i<Text.Length;i++)
				{
					LayoutInfo.Item item = new LayoutInfo.Item();
					char c = Text[i];
					Rectangle rect = Rectangle.Truncate(bounds[i].GetBounds( g));
					if (c == (char) Keys.LineFeed)
					{
						item.charVisibility = LayoutInfo.Item.CharVisibility.LineFeed;
						// Linefeed. The bounds is to the right of the previous character.
						// If the previous character was also a linefeed, we move down a line
						// from the previous bounds.
						if (i > 0 && Text[i-1] == 10)
							rect = new Rectangle(CaretXFromAlign, prevBounds.Top + Font.Height, 0, Font.Height);
						else
							rect = new Rectangle(prevBounds.Right, prevBounds.Top, 0, prevBounds.Height);
					}
					else // c != 10
						item.charVisibility = LayoutInfo.Item.CharVisibility.Visible;
					
					if (rect.IsEmpty)
					{
						item.charVisibility = LayoutInfo.Item.CharVisibility.OutOfBounds;
						// Look for spaces that are swallowed
						if (c == (char)32 && prevVisibility == LayoutInfo.Item.CharVisibility.Visible)
							rect = new Rectangle(prevBounds.Right, prevBounds.Top, 0, prevBounds.Height);
					}
					item.bounds = prevBounds = rect;
					layout.Items[i] = item;
					prevVisibility = item.charVisibility;
				}
			}
		}
	}

	protected class LayoutInfo : ICloneable
	{
		public Item[] Items;

		// The position and info for each draw item/ character
		public class Item
		{
			public enum CharVisibility {OutOfBounds, Visible, LineFeed};
               
			public CharVisibility charVisibility;
			public Rectangle bounds;
			public bool selected;
		}


		public object Clone()
		{
			LayoutInfo layout = new LayoutInfo();
			layout.Items = (Item[])Items.Clone();
			return layout;
		}

	}

	// Uses the current selection positions to set what is selected in drawLayout
	// This only invalidates the characters that have changed
	internal override void SelectInternal(int start, int length)
	{
		if (start == selectionStartActual && length == selectionLengthActual)
			return;
		Region newRegion = new Region(RectangleF.Empty);
		selectionStartActual = start;
		selectionLengthActual = length;
		for (int i = 0; i < Text.Length; i++) 
		{
			bool selected = (i>=GetSelectionStart() && i < GetSelectionStart() + GetSelectionLength());
			layout.Items[i].selected = selected;
			if (selected)
				newRegion.Union(layout.Items[i].bounds);
		}
		// Find the region we need to invalidate by Xoring with old
		if (selectedRegion != null)
		{
			Region invalidRegion = newRegion.Clone();
			invalidRegion.Xor(selectedRegion);
			using (Graphics g = CreateGraphics())
			{
				invalidRegion.GetBounds(g);
			}
			Invalidate(invalidRegion);				
		}
		selectedRegion = newRegion;
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
		LayoutFromText();
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
		CaretSetPosition(GetSelectionStart() + GetSelectionLength());
	}

	// Sets the end of the selection and the caret position
	internal override void SetSelectionLength(int length)
	{
		SelectInternal(selectionStartActual, length);
		CaretSetPosition(GetSelectionStart() + GetSelectionLength());
	}

	// These are the two methods that need to be overridden if the client size is different to the actual size (borders etc.)
	protected override void SetClientSizeCore(int x, int y)
	{
		base.SetClientSizeCore (x - BorderOffset.X * 2, y - BorderOffset.Y * 2);
	}

	// Convert a client size into a window bounds size.
	internal override Size ClientToBounds(Size size)
	{
		return new Size(size.Width + BorderOffset.X * 2, size.Height + BorderOffset.Y * 2);
	}

	protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
	{
		// If not Multiline then the control height is the font height
		if (!Multiline)
			height = BorderOffset.Y * 2 + Font.Height;
		// If the height or width changes then relayout the text
		if ((specified & BoundsSpecified.Height) != 0 | (specified & BoundsSpecified.Width) != 0)
			LayoutFromText();
		base.SetBoundsCore (x, y, width, height, specified);
	}


	// Paint the text using layout information
	private void DrawText(PaintEventArgs e)
	{
		for (int i=0; i<Text.Length;i++) 
		{
			LayoutInfo.Item item = layout.Items[i];
			if (item.charVisibility == LayoutInfo.Item.CharVisibility.Visible)
			{
				Brush back, fore;
				if (item.selected && Focused)
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

	// Handle all mouse processing
	private void ProcessMouse(MouseEventArgs e)
	{
		if (Enabled) 
		{
			if (mouseDown)
			{
				if (e.Button == MouseButtons.Left)
				{
					int closest = CaretGetPosition(new Point(e.X,e.Y));
					SelectInternal(selectionStartActual, closest - selectionStartActual);
					CaretSetEndSelection();
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
					int closest = CaretGetPosition(new Point(e.X,e.Y));
					SelectInternal( closest,0);
					CaretSetEndSelection();
					CaretTimerReset();
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
	private void CaretDraw( PaintEventArgs e ) 
	{
		if (caretHiding)
			e.Graphics.DrawLine(caretPenHiding, caretBounds.Left, caretBounds.Top, caretBounds.Left, caretBounds.Bottom - 1);
		else
			e.Graphics.DrawLine(caretPen, caretBounds.Left, caretBounds.Top, caretBounds.Left, caretBounds.Bottom - 1);
	}

	// Get the X Position of Caret based on alignment
	private int CaretXFromAlign
	{
		get
		{
			switch (TextAlign)
			{
				case(HorizontalAlignment.Left):
					return BorderOffset.X;
				case(HorizontalAlignment.Center):
					return (Width-1)/2 - BorderOffset.X;
				default: /*Right*/
					return Width - BorderOffset.X - 1;
			}
		}
	}

	// Set the caret bounds from a character position
	// Invalidate if needed
	private void CaretSetPosition( int position)
	{
		Rectangle newBounds = Rectangle.Empty;
		if (Text.Length == 0)
		{
			newBounds = new Rectangle(CaretXFromAlign, BorderOffset.Y, 0, Font.Height);
		}
		else
		{
			if (position == Text.Length)
			{
				// If the last character is a linefeed, position ourselves at the
				// beginning of the following line. Otherwise, position ourselves
				// immediately to the right of the last character.
				LayoutInfo.Item item = layout.Items[position -1];
				newBounds = item.bounds;
				if (item.charVisibility == LayoutInfo.Item.CharVisibility.LineFeed)
					newBounds = new Rectangle(CaretXFromAlign, newBounds.Top + Font.Height, 0, Font.Height);
				else
					newBounds = new Rectangle(newBounds.Right, newBounds.Top, 0, newBounds.Height);			
			}
			else
			{
				newBounds = layout.Items[position].bounds;
				newBounds = new Rectangle(newBounds.Left, newBounds.Top, 0, newBounds.Height);
			}
		}

		// When we change the caret position, we invalidate & reset the timer
		if (Focused)
		{
			Region region = new Region(newBounds);
			region.Xor(caretBounds);
			Invalidate(region);
			CaretTimerReset();
		}
		caretBounds = newBounds;
	}

	// Get the caret position of the nearest point relative to pt
	private int CaretGetPosition(Point pt)
	{
		int prevY = 0;
		int caretPosition = 0;
		Rectangle bounds = new Rectangle(BorderOffset.X, BorderOffset.Y, 0, Font.Height);
		Rectangle prevBounds = bounds;
		int i=0;
		for (; i<Text.Length;i++) 
		{
			if (layout.Items[i].charVisibility != LayoutInfo.Item.CharVisibility.OutOfBounds)
			{
				bounds = layout.Items[i].bounds;
				caretPosition = i;
				if (pt.Y < bounds.Bottom && pt.X < bounds.Left + bounds.Width/2 )
				{
					break;
				}
				// New Line
				if (bounds.Bottom > prevY)
				{
					if (pt.Y < prevY)
					{
						// Move position back to end of previous line
						caretPosition--;
						break;
					}
					prevY = bounds.Bottom;
				}
				prevBounds = bounds;
			}
		}
	
		if (i == Text.Length)
		{
			// If the last character is a linefeed, set the caret to Text.Length only
			// if they clicked underneath the linefeed.
			caretPosition = i;
			if (Text.Length > 0 &&
				layout.Items[i-1].charVisibility == LayoutInfo.Item.CharVisibility.LineFeed &&
				pt.Y <= prevBounds.Bottom)
				--caretPosition;
		}
		
		return caretPosition;
	}
	
	// Each time the timer fires we flash the caret
	private void CaretFlash_Tick(object sender, EventArgs e)
	{
		if (Focused)
		{
			caretHiding = !caretHiding;
			Invalidate(caretBounds);
		}
	}

	// Make sure the caret is not hiding and reset timer
	private void CaretTimerReset()
	{
		//Resets the flash timer
		caretFlash.Enabled = false;
		caretFlash.Enabled = Focused;
		caretHiding = false;
	}

	// Reset the caret position to the end of the first visible line
	private void CaretReset()
	{
		int end = 0;
		if (Multiline)
			end = Text.Length;
		else
			// Position before the first linefeed
			for (; end < Text.Length; end++)
				if (Text[end] == (char)Keys.LineFeed)
					break;

		SelectInternal(0, end);
		CaretSetPosition(end);
	}

	// Sets the caret position to the end of the selection
	private void CaretSetEndSelection()
	{
		// Even if we select backwards, the Caret will be at the end
		CaretSetPosition(GetSelectionStart()+ GetSelectionLength());
	}

#if !CONFIG_COMPACT_FORMS

#endif // !CONFIG_COMPACT_FORMS

}; // class TextBox

}; // namespace System.Windows.Forms
