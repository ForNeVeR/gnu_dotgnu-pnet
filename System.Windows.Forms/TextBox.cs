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
	private Pen backPen;

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

	// Maximum possible X/Y for a region
	private const int maxXY = 4194304;

	// XY offset of the view for text bigger than the text area
	private int xViewOffset;
	private int yViewOffset;

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
		// TODO Get this value from SystemInformation
		caretFlash = new Timer();
		// Handle when System information changes for interval & system colors
		caretFlash.Interval = 500;
		caretFlash.Tick += new EventHandler(CaretFlash_Tick);
		caretFlash.Enabled = true;

		// Cache the Pen - check what color this should be
		caretPen = new Pen(SystemColors.Highlight);

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

	// TODO Handle this
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
			string text = base.Text;
			if (text == null)
				return string.Empty;
			else
				return text;
		}
		set
		{
			SetTextInternal( value);
			CaretReset();
		}
	}

	// TODO
	public override bool CanUndo
	{
		get
		{
			return false;
		}
	}

	// TODO
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
				// Layout changes
				LayoutFromText();
				
				// TODO
				if (!Multiline && textAlign == HorizontalAlignment.Center)
					xViewOffset = maxXY - ClientSize.Width/2;
				else
				{
					// We set the caret to the end of the text
					// And then we make sure the view can see the caret
					// This behaviour is different in MS, they make sure
					// the beginning of the text if visible
					CaretSetEndSelection();
					ScrollToCaretNoInvalidate();
				}
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
		// If not multline then we dont recognise "Enter"
		else if ((keyData & Keys.Alt) == 0 && (keyData & Keys.KeyCode) == Keys.Enter)
			return false;

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
		invalidRegion.Translate(BorderOffset.X - xViewOffset, BorderOffset.Y - yViewOffset);
		Invalidate( invalidRegion);
		mouseDown = false;
	}

	protected override void OnFontChanged(EventArgs e)
	{
		base.OnFontChanged (e);
		if (!Multiline && AutoSize)
			Height = ClientToBounds(Size.Empty).Height + Font.Height;
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
			CaretSetEndSelection();
			ScrollToCaret();
		}
		else if (e.KeyCode == Keys.Right && GetSelectionStart() < Text.Length)
		{
			SelectInternal( GetSelectionStart() + 1, 0);
			CaretSetEndSelection();
			ScrollToCaret();
		
		}
		else if (e.KeyCode == Keys.Back && (GetSelectionStart() + GetSelectionLength())>0)
		{
			int widthText = layout.Items[layout.Items.Length - 1].bounds.Right;
			int bottomCaret = caretBounds.Bottom;
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
			// In the case of multiline we ensure that we recover any blank lines created by backspacing at the bottom (if the text is bigger than the view area).
			// In the case of non multiline, we recover any character space that is now there after deleting
			if (!Multiline)
			{
				if (textAlign == HorizontalAlignment.Center && xViewOffset > maxXY - ClientSize.Width/2)
				{
					xViewOffset -= widthText - layout.Items[layout.Items.Length - 1].bounds.Right;
					xViewOffset = Math.Max(xViewOffset, maxXY - ClientSize.Width/2);
					InvalidateClientArea();
				}
				else if (textAlign == HorizontalAlignment.Left && xViewOffset > 0)
				{
					xViewOffset -= widthText - layout.Items[layout.Items.Length - 1].bounds.Right;
					xViewOffset = Math.Max(xViewOffset, 0);
					InvalidateClientArea();
				}
				//Move the caret
				CaretSetEndSelection();				
			}
			else
			{
				//Move the caret first
				CaretSetEndSelection();
				if (bottomCaret != caretBounds.Bottom)
				{
					yViewOffset -= bottomCaret - caretBounds.Bottom;
					if (yViewOffset < 0)
						yViewOffset = 0;
					InvalidateClientArea();
				}
			}

		}
		else if (e.KeyCode == Keys.Delete && GetSelectionStart() < Text.Length)
		{
			if (GetSelectionLength() > 0)
				SetSelectionText("");
			else
				SetTextInternal(Text.Substring(0, GetSelectionStart()) + Text.Substring(GetSelectionStart() + 1));
			SelectInternal(GetSelectionStart(),0);
			CaretSetEndSelection();
		}
		else
			return;
		// If processed
		// Force repaint now
		Update();
		e.Handled = true;

	}

	// Invalidate the whole client area
	private void InvalidateClientArea()
	{
		Invalidate(new Rectangle(0, 0, ClientSize.Width, ClientSize.Height));
	}

	// Handle "KeyPress" events for the text box.
	private void HandleKeyPress(Object sender, KeyPressEventArgs e)
	{
		char c = e.KeyChar;
		// Discard enter if not multiline
		if (c == (char)Keys.Enter && !Multiline)
			return;
		// These are handled on key down
		if (c == (char)Keys.Delete || c == (char)Keys.Back)
			return;
			
		if (c==(char)Keys.Enter)
			c = (char)Keys.LineFeed;
		SetSelectionText( c.ToString() );
		SelectInternal( SelectionStart + 1, 0);
		CaretSetPosition(SelectionStart);
		ScrollToCaretInternal();
		// Force a repaint now
		Update();
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
			ControlPaint.DrawBorder3D( e.Graphics, new Rectangle(0,0,Width, Height), Border3DStyle.Sunken);
		// Clip to text area
		e.Graphics.SetClip(new Rectangle(BorderOffset.X, BorderOffset.Y, ClientSize.Width, ClientSize.Height));
		DrawText(e);
		if (Focused)
			CaretDraw(e);
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

	private Pen BackPen
	{
		get
		{
			if (backPen == null)
				backPen = new Pen(BackColor);
			return backPen;
		}
	}

	// Create the drawLayout from the text
	// All rendered in client coordinates.
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
				// We leave 1 pixel on the left and right for the caret
				// Multiline textboxes are inifinite in the y direction
				// non multiline are infinite in the x direction and we scroll when needed
				Rectangle measureBounds;
				if (Multiline)
					measureBounds = new Rectangle(1, 0, ClientSize.Width - 2, maxXY * 2);
				else
					measureBounds = new Rectangle(1, 0, maxXY * 2, ClientSize.Height);
				Region[] bounds = g.MeasureCharacterRanges(Text, Font, measureBounds, format);

				// Convert the MeasureCharacterRanges to LayoutInfo
				// MeasureCharacterRanges will return an empty rectangle for all characters
				// that are not visible. We need to figure out the positions of LF and spaces
				// that are swallowed
				Rectangle prevBounds = new Rectangle(CaretXFromAlign, 0, 0, Font.Height);
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
						if (i > 0 && Text[i-1] == (char)Keys.LineFeed)
							rect = new Rectangle(CaretXFromAlign, prevBounds.Top + Font.Height, 0, Font.Height);
						else
							rect = new Rectangle(prevBounds.Right, prevBounds.Top, 0, prevBounds.Height);
					}
					else // c != linefeed
						item.charVisibility = LayoutInfo.Item.CharVisibility.Visible;
					
					if (rect.IsEmpty)
					{
						item.charVisibility = LayoutInfo.Item.CharVisibility.OutOfBounds;
						// Look for spaces that are swallowed
						if (c == (char)Keys.Space && prevVisibility == LayoutInfo.Item.CharVisibility.Visible)
							rect = new Rectangle(prevBounds.Right, prevBounds.Top, 0, prevBounds.Height);
					}
					item.bounds = prevBounds = rect;
					layout.Items[i] = item;
					prevVisibility = item.charVisibility;
				}
			}
		}
	}

	// Make sure the caret is visible
	protected override void ScrollToCaretInternal()
	{
		if (ScrollToCaretNoInvalidate())
			Invalidate(new Rectangle(Point.Empty, ClientSize));
	}

	// Called to recalculate the offsets if make sure bounds is visible. Returns true if offsets have changed
	private bool ScrollToCaretNoInvalidate()
	{
		bool changed = false;
		if (caretBounds.Bottom - yViewOffset < ClientRectangle.Top)
		{
			changed = true;
			yViewOffset = caretBounds.Top;
		}
		else if (caretBounds.Bottom - yViewOffset > ClientRectangle.Bottom)
		{
			changed = true;
			yViewOffset = caretBounds.Bottom - ClientRectangle.Bottom;
		}
		if (caretBounds.Right - xViewOffset < ClientRectangle.Left)
		{
			changed = true;
			xViewOffset = caretBounds.Left;
		}
		else if (caretBounds.Left - xViewOffset > ClientRectangle.Right - 1)
		{
			changed = true;
			xViewOffset = caretBounds.Right - ClientRectangle.Right + 1;
		}
		return changed;
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
			invalidRegion.Translate(BorderOffset.X - xViewOffset, BorderOffset.Y - yViewOffset);
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
		string oldText = Text;
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
			invalid.Translate(BorderOffset.X - xViewOffset, BorderOffset.Y - yViewOffset);
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
	// This is used to calculate all offsets
	internal override Size ClientToBounds(Size size)
	{
		return new Size(size.Width + BorderOffset.X * 2, size.Height + BorderOffset.Y * 2);
	}

	protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
	{
		// If not Multiline then the control height is the font height
		if (!Multiline)
			height = ClientToBounds(Size.Empty).Height + Font.Height;
		// If the height or width changes then relayout the text
		if ((specified & BoundsSpecified.Height) != 0 | (specified & BoundsSpecified.Width) != 0)
			LayoutFromText();
		base.SetBoundsCore (x, y, width, height, specified);
		//TODO
		ScrollToCaretNoInvalidate();
	}


	// Paint the text using layout information
	private void DrawText(PaintEventArgs e)
	{
		for (int i=0; i<Text.Length;i++) 
		{
			LayoutInfo.Item item = layout.Items[i];
			Rectangle bounds = item.bounds;
			bounds.Offset(-xViewOffset + ClientToBounds(Size.Empty).Width/2, -yViewOffset + ClientToBounds(Size.Empty).Height/2);
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
				e.Graphics.FillRectangle(back, bounds);
				char c = Text[i];
				if (passwordChar != 0)
					c = passwordChar;
				if (Enabled)
					e.Graphics.DrawString(c.ToString(), Font, fore, bounds.Location);
				else
					ControlPaint.DrawStringDisabled(e.Graphics, c.ToString(), Font, ForeColor, bounds, StringFormat.GenericDefault);
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
					if (closest >= 0)
					{
						// We need to select some text
						SelectInternal(selectionStartActual, closest - selectionStartActual);
						// Caret is always at the end
						CaretSetEndSelection();
					}
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
					// We are clicking to move the caret
					int closest = CaretGetPosition(new Point(e.X,e.Y));
					if (closest >= 0)
					{
						SelectInternal( closest,0);
						CaretSetEndSelection();
						// If you click right at the end/beginning, make sure the caret is in view
						ScrollToCaret();
						CaretTimerReset();
						mouseDown = true;
						Capture = true;
					}
				}
				Cursor = Cursors.IBeam;
			}
		}
		else
			mouseDown = false;
	}

	// Takes into account client offset and view offsets
	private void CaretDraw( PaintEventArgs e ) 
	{
		Pen pen = caretHiding? BackPen : caretPen;
		Point p1 = new Point(caretBounds.Left + ClientToBounds(Size.Empty).Width / 2 - xViewOffset, caretBounds.Top + ClientToBounds(Size.Empty).Width / 2 - yViewOffset);
		Point p2 = p1 + new Size(0, caretBounds.Height - 1);
		e.Graphics.DrawLine(pen, p1, p2);
	}

	// Get the default X Position of Caret based on alignment
	private int CaretXFromAlign
	{
		get
		{
			switch (TextAlign)
			{
				case(HorizontalAlignment.Left):
					return 0;
				case(HorizontalAlignment.Center):
					if (Multiline)
						return ClientSize.Width/2;
					else
						return maxXY;
				default: /*Right*/
					if (Multiline)
						return ClientSize.Width;
					else
						return maxXY * 2;
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
			newBounds = new Rectangle(CaretXFromAlign, 0, 0, Font.Height);
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
					newBounds = new Rectangle(CaretXFromAlign, newBounds.Top + Font.Height -1, 0, Font.Height);
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
			region.Translate(ClientToBounds(Size.Empty).Width / 2 - xViewOffset, ClientToBounds(Size.Empty).Height / 2 - yViewOffset);
			Invalidate(region);
			CaretTimerReset();
		}
		caretBounds = newBounds;
	}

	// Get the caret position of the nearest point relative to pt
	private int CaretGetPosition(Point pt)
	{
		pt = PointToClient(pt);
		pt.Offset(xViewOffset, yViewOffset);
		int prevY = 0;
		int caretPosition = 0;
		Rectangle bounds = new Rectangle(0, 0, 0, Font.Height);
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
			Invalidate(new Rectangle(caretBounds.Left + ClientToBounds(Size.Empty).Width / 2 - xViewOffset, caretBounds.Top + ClientToBounds(Size.Empty).Height / 2 - yViewOffset, caretBounds.Width, caretBounds.Height));
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
