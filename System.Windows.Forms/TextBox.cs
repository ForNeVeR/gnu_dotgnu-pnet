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
	private Brush foreBrush, backBrush, disabledBackBrush;
	private Pen backPen;

	// The position and drawing information for each item
	protected LayoutInfo layout;
	// Start of a selection
	protected int selectionStartActual;
	// Length of the selection, could be negative
	protected int selectionLengthActual;
	// A region of the text that is selected
	private Region selectedRegion;
	
	// Whether the flashing caret is currently hiding
	private bool caretHiding = true;
	// Position to draw caret
	private Rectangle caretBounds;
	private Pen caretPen;

	// Maximum possible X/Y for a region
	private const int maxXY = 4194304;

	// XY offset of the view for text bigger than the text area
	private int xViewOffset;
	private int yViewOffset;

	// Cached Graphics
	private Graphics graphics;

	public TextBox()
	{
		// Trap interesting events.  We do it this way rather
		// than override virtual methods so that the published
		// TextBox API is maintained.
		KeyPress += new KeyPressEventHandler(HandleKeyPress);
		MouseDown += new MouseEventHandler(HandleMouseDown);
		MouseMove += new MouseEventHandler(HandleMouseMove);
		DoubleClick += new EventHandler(HandleDoubleClick);
		Paint += new PaintEventHandler(HandlePaint);
		MultilineChanged +=new EventHandler(HandleMultilineChanged);

		textAlign = HorizontalAlignment.Left;

		BackColor = SystemColors.Window;
		// TODO Get this value from SystemInformation
		CaretSetPosition(0);

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
			// Change all text endings of CR or LF into CRLF
			System.Text.StringBuilder sb = new System.Text.StringBuilder();
			if (value.Length > 0)
			{
				char cNext = value[0];
				if (value.Length > 1)

			// Set the position to the end
			SelectInternal(Text.Length, 0);
			CaretSetEndSelection();

				{
					for (int i = 0; i < value.Length - 1; i++)
					{
						char c = cNext;
						cNext = value[i + 1];
						if (c == '\r' && cNext == '\n')
						{
							sb.Append("\r\n");
							++i;
							if (i < value.Length)
								cNext = value[i + 1];
						}
						else if ( c == '\r' || c == '\n')
						{
							sb.Append("\r\n");
						}
						else
							sb.Append(c);
					}
				}
				if (cNext == '\n' || cNext == '\r')
				{
					sb.Append("\r\n");
				}
				else
					sb.Append(cNext);
			}
			SetTextInternal( sb.ToString());
			ResetView();
			Redraw(ControlGraphics);
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

	public override String[] Lines
	{
		get
		{
			int line = 0;
			int y = -1;
			// Find the number of lines
			for (int i = 0; i < Text.Length; i++)
			{
				int currentY = layout.Items[i].bounds.Y;
				if (currentY != y)
				{
					line++;
					y = currentY;
				}
			}
			string[] lines = new string[line];
			int start = 0;
			line = 0;
			y = 0;
			// Break into strings
			for (int i = 0; i < Text.Length; i++)
			{
				int currentY = layout.Items[i].bounds.Y;
				if (currentY != y)
				{
					if (Text[i - 2] == '\r') // Look for CRLF
					{
						lines[line++] = Text.Substring(start, i - 2 - start);
					}
					else
						lines[line++] = Text.Substring(start, i - 1 - start);
					start = i;
					y = currentY;
				}
			}
			if (start != Text.Length)
			{
				if (Text[Text.Length - 2] == '\r') // Look for CRLF
					lines[line++] = Text.Substring(start, Text.Length - start - 2 /* CRLF */);
				else
					lines[line++] = Text.Substring(start);
					
			}
			
			return lines;
		}
		set
		{
			System.Text.StringBuilder sb = new System.Text.StringBuilder();
			foreach(string s in Lines)
			{
				sb.Append(s);
				sb.Append("\r\n");
			}
			SetTextInternal( sb.ToString());
			ResetView();
			Redraw(ControlGraphics);
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
				
				ResetView();
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
		CaretShow();
		Redraw(ControlGraphics);
	}

	// Process when the control loses the focus
	protected override void OnLeave(EventArgs e)
	{
		base.OnLeave (e);
		// Create a region containing the caret and all visible selected text
		Region update = new Region(caretBounds);
		for (int i = 0; i < Text.Length; i++)
			if (layout.Items[i].type == LayoutInfo.Item.CharType.VisibleChar)
			{
				Rectangle b = layout.Items[i].bounds;
				update.Union(new Region(new Rectangle(b.Left,b.Top,b.Width,b.Height + 1)));
			}
		update.Translate(ClientOrigin.X - xViewOffset, ClientOrigin.Y - yViewOffset);
		AddUpdate(update);
		caretHiding = true;
		Redraw(ControlGraphics);
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

	// Redraw the whole client area
	private void UpdateClientArea()
	{
		AddUpdate(new Rectangle(ClientOrigin.X, ClientOrigin.Y, ClientSize.Width, ClientSize.Height));
	}

	// Handle "KeyPress" events for the text box.
	private void HandleKeyPress(Object sender, KeyPressEventArgs e)
	{
		if (ReadOnly)
			return;
			
		char c = e.KeyChar;

		// Discard enter if not multiline
		if (c=='\r' && !Multiline)
			return;

		// These are handled on key down
		if (c == (char)Keys.Delete || c == (char)Keys.Back)
			return;

		// Discard control characters
		if (c<' ' && c!='\r')
			return;

		if (MaxLength>0 && Text.Length >= MaxLength)
			return;
			
		if (GetInsertMode()==InsertMode.Overwrite && GetSelectionLength()==0)
		{
			int startPos = GetSelectionStart();
			int endPos = ComputeCharRightPos(startPos);
			if (endPos > startPos)
				SelectInternal(startPos, endPos-startPos);
		}

		string strInsert;
			if (c=='\r')
			strInsert = "\r\n";
		else
			strInsert = c.ToString();
		SetSelectionText( strInsert );
		SelectInternal(SelectionStart + strInsert.Length, 0);
		CaretSetPosition(SelectionStart);
		ScrollToCaretNoRedraw();
		Redraw(ControlGraphics);
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

	// Handle "DoubleClick" events for the text box.
	private void HandleDoubleClick(Object sender, EventArgs e)
	{
		ProcessMouse(new MouseEventArgs(MouseButtons.Left, 2, MousePosition.X, MousePosition.Y, 0));
	}

	// Handle "Paint" events for the text box.
	// In our implementation NO painting happens outside of the paint event. This might change because it might not update fast enough
	private void HandlePaint(Object sender, PaintEventArgs e)
	{
		using (Graphics g = CreateNonClientGraphics())
		{
			Redraw(g);
		}
	}

	// Redraw a specific portion of the textbox
	private void Redraw(Graphics g)
	{
		bool focused = Focused;
		if (!Enabled || ReadOnly)
			g.FillRegion(DisabledBackBrush, g.Clip);
		else
			g.FillRegion(BackBrush, g.Clip);
		switch (BorderStyle)
		{
			case(BorderStyle.Fixed3D):
				ControlPaint.DrawBorder3D( g, new Rectangle(0,0,Width, Height), Border3DStyle.Sunken);
				break;
			case (BorderStyle.FixedSingle):
				ControlPaint.DrawBorder( g, new Rectangle(0,0,Width - 1, Height - 1), ForeColor, ButtonBorderStyle.Solid);
				break;
		}
		// Clip to text area
		g.SetClip(new Rectangle(ClientOrigin, ClientSize), System.Drawing.Drawing2D.CombineMode.Intersect);
		// Draw the background of the selected text
		if (focused && selectedRegion != null)
		{
			Region r = selectedRegion.Clone();
			r.Translate(ClientOrigin.X - xViewOffset, ClientOrigin.Y - yViewOffset);
			g.FillRegion(SelectedBackBrush, r);
		}
		DrawText(g, focused);
		if (focused)
			CaretDraw(g);
		Region clip = g.Clip;
		clip.MakeEmpty();
		g.Clip = clip;
	}

	// Handle the event when multiline is changed.
	private void HandleMultilineChanged(object sender, EventArgs e)
	{
		// Force the control to relayout
		LayoutFromText();
		CaretReset();
		Redraw(ControlGraphics);
	}

	private Graphics ControlGraphics
	{
		get
		{
			if (graphics == null)
				graphics = CreateNonClientGraphics();
			return graphics;
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

	private Brush DisabledBackBrush
	{
		get
		{
			if (disabledBackBrush == null)
				disabledBackBrush = new SolidBrush(SystemColors.Control);
			return disabledBackBrush;
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
				measureBounds = new Rectangle(1, 0, ClientSize.Width - 2, maxXY);
			else
				measureBounds = new Rectangle(1, 0, maxXY, ClientSize.Height);
			Region[] bounds;
			if (passwordChar == 0)
				bounds = ControlGraphics.MeasureCharacterRanges(Text, Font, measureBounds, format);
			else
				bounds = ControlGraphics.MeasureCharacterRanges(new string(passwordChar, Text.Length), Font, measureBounds, format);

			// Convert the MeasureCharacterRanges to LayoutInfo
			// MeasureCharacterRanges will return an empty rectangle for all characters
			// that are not visible. We need to figure out the positions of LF and spaces
			// that are swallowed
			Rectangle prevBounds = new Rectangle(CaretXFromAlign, 0, 0, Font.Height);
			LayoutInfo.Item.CharType prevType = LayoutInfo.Item.CharType.VisibleChar;
			for (int i=0; i<Text.Length;i++)
			{
				LayoutInfo.Item item = new LayoutInfo.Item();
				char c = Text[i];
				Rectangle rect = Rectangle.Truncate(bounds[i].GetBounds( ControlGraphics));
				if (c == '\r')
				{
					item.type = LayoutInfo.Item.CharType.CR;
					// Linefeed. The bounds is to the right of the previous character.
					// If the previous character was also a linefeed, we move down a line
					// from the previous bounds.
					if (i > 0 && Text[i-1] == '\n')
						rect = new Rectangle(CaretXFromAlign, prevBounds.Top + Font.Height, 0, Font.Height);
					else
						rect = new Rectangle(prevBounds.Right, prevBounds.Top, 0, prevBounds.Height);
				}
				else if (c == '\n')
				{
					item.type = LayoutInfo.Item.CharType.LF;
					rect = new Rectangle(prevBounds.Right, prevBounds.Top, (int) Font.Size, prevBounds.Height);
						// give this LF a non-empty bounds so that it appears in a selection
				}
				else // c != CR, LF
					item.type = LayoutInfo.Item.CharType.VisibleChar;
				
				if (rect.IsEmpty)
				{
					item.type = LayoutInfo.Item.CharType.OutOfBoundsChar;
					// Look for spaces that are swallowed
					if (c == ' ' && prevType == LayoutInfo.Item.CharType.VisibleChar)
						rect = new Rectangle(prevBounds.Right, prevBounds.Top, 0, prevBounds.Height);
				}
				item.bounds = prevBounds = rect;
				layout.Items[i] = item;
				prevType = item.type;
			}
		}
	}

	// Make sure the caret is visible
	protected override void ScrollToCaretInternal()
	{
		ScrollToCaretNoRedraw();
		Redraw(ControlGraphics);
	}

	// Called to recalculate the offsets if make sure bounds is visible.
	private void ScrollToCaretNoRedraw()
	{
		if (caretBounds.Top - yViewOffset < ClientRectangle.Top)
		{
			UpdateClientArea();
			yViewOffset = caretBounds.Top;
		}
		else if (caretBounds.Bottom - yViewOffset > ClientRectangle.Bottom)
		{
			UpdateClientArea();
			yViewOffset = caretBounds.Bottom - ClientRectangle.Bottom;
		}
		if (caretBounds.Left- xViewOffset < ClientRectangle.Left)
		{
			UpdateClientArea();
			xViewOffset = caretBounds.Left;
		}
		else if (caretBounds.Right - xViewOffset > ClientRectangle.Right)
		{
			UpdateClientArea();
			xViewOffset = caretBounds.Right - ClientRectangle.Right;
		}
	}

	// When changing text, bounds or alignment, we make sure the view includes the position of the first character.
	private void ResetView()
	{
		xViewOffset = 0;
		yViewOffset = 0;
		//Set caret to the beginning
		CaretSetPosition(0);
		if (!Multiline)
		{
			if (textAlign == HorizontalAlignment.Center)
				xViewOffset = maxXY / 2 - ClientSize.Width / 2;
			else if (textAlign == HorizontalAlignment.Right)
				xViewOffset = maxXY - ClientSize.Width + 2;
		}
		else
			ScrollToCaretNoRedraw();
				
	}

	protected class LayoutInfo : ICloneable
	{
		public Item[] Items;

		// The position and info for each draw item/ character
		public class Item
		{
			public enum CharType {OutOfBoundsChar, VisibleChar, CR, LF};
               
			public CharType type;
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
	// Sets the update to redraw only the characters that have changed
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
			Rectangle b = layout.Items[i].bounds;
			b = new Rectangle(b.Left, b.Top, b.Width, b.Height + 1);
			if (selected)
				newRegion.Union(b);
		}
		// Find the region we need to redraw by Xoring with old
		if (selectedRegion != null)
		{
			Region redrawRegion = newRegion.Clone();
			redrawRegion.Xor(selectedRegion);
			redrawRegion.Translate(- xViewOffset + ClientOrigin.X, - yViewOffset + ClientOrigin.Y);
			AddUpdate(redrawRegion);
		}
		else
			AddUpdate(newRegion);
		selectedRegion = newRegion;

	}

	
	// Called to change the text. Sets the update to whats needed to but doesnt change the selection point or caret
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
			Region update = new Region(RectangleF.Empty);
			for (int i=0;i < Math.Max(oldText.Length, text.Length);i++)
			{
				if (i >= oldText.Length)
					update.Union( layout.Items[i].bounds);
				else if (i >= text.Length)
					update.Union( oldLayout.Items[i].bounds);
				else if (Text[i] != oldText[i] || oldLayout.Items[i].bounds != layout.Items[i].bounds)
				{
					update.Union( layout.Items[i].bounds);
					update.Union( oldLayout.Items[i].bounds);
				}
			}
			// Get the offset of the ClientRectangle
			update.Translate( - xViewOffset + ClientOrigin.X, - yViewOffset + ClientOrigin.Y);
			AddUpdate(update);
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

	internal override void OnToggleInsertMode()
	{
		// FIXME: change caret appearance
	}
	
	// Caret navigation
	protected override void MoveCaret(CaretDirection dir, bool extend)
	{
		int startSel = GetSelectionStart();
		
		int newPos = startSel;
		if ((extend && selectionLengthActual>0) || (!extend && IsTowardsTextEnd(dir)))
			newPos += GetSelectionLength();
		
		switch (dir)
		{
		case CaretDirection.Left:
			newPos = ComputeCharLeftPos(newPos);
			break;
		case CaretDirection.Right:
			newPos = ComputeCharRightPos(newPos);
			break;
		case CaretDirection.WordLeft:
			newPos = ComputeWordLeftPos(newPos);
			break;
		case CaretDirection.WordRight:
			newPos = ComputeWordRightPos(newPos);
			break;
		case CaretDirection.LineStart:
			newPos = ComputeLineStartPos(newPos);
			break;
		case CaretDirection.LineEnd:
			newPos = ComputeLineEndPos(newPos);
			break;
		case CaretDirection.LineUp:
			if (caretBounds.Top > caretBounds.Height )
				newPos = ComputeLineOffset(newPos, -1);
			break;
		case CaretDirection.LineDown:
			if (caretBounds.Top < layout.Items[layout.Items.Length - 1].bounds.Top)
				newPos = ComputeLineOffset(newPos, 1);
			break;
		case CaretDirection.PageUp:
			newPos = ComputeLineOffset(newPos, - (int) ClientRectangle.Height / Font.Height);
			break;
		case CaretDirection.PageDown:
			newPos = ComputeLineOffset(newPos, (int) ClientRectangle.Height / Font.Height);
			break;
		case CaretDirection.TextStart:
			newPos = 0;
			break;
		case CaretDirection.TextEnd:
			newPos = Text.Length;
			break;
		}
		
		if (extend)
			UpdateSelectionInternal(newPos);
		else
		{
			SelectInternal(newPos, 0);
			CaretSetPosition(newPos);
			ScrollToCaretNoRedraw();
			Redraw(ControlGraphics);
		}
	}
	
	protected override void DeleteTextOp(CaretDirection dir)
	{
		if (ReadOnly)
			return;

		switch (dir)
		{
			case CaretDirection.WordLeft:
			case CaretDirection.Left:
			{
				if (layout.Items.Length == 0)
					break;
				int widthText = layout.Items[layout.Items.Length - 1].bounds.Right;
				int bottomCaret = caretBounds.Bottom;

				if (GetSelectionLength()>0)
				{
					SetSelectionText("");
					SelectInternal(GetSelectionStart(), 0);
				}
				else
				{
					int startPos = GetSelectionStart();
					int newPos =
						dir==CaretDirection.Left
						?
						ComputeCharLeftPos(startPos)
						:
						ComputeWordLeftPos(startPos);
						
					if (newPos < startPos)
					{
						int nbCharsToDelete = startPos - newPos;
						SetTextInternal(Text.Substring(0, GetSelectionStart() - nbCharsToDelete) + Text.Substring(GetSelectionStart()));
						SelectInternal(GetSelectionStart()-nbCharsToDelete, 0);
					}
				}

				// In the case of multiline we ensure that we recover any blank lines created by backspacing at the bottom (if the text is bigger than the view area).
				// In the case of non multiline, we recover any character space that is now there after deleting
				if (!Multiline && layout.Items.Length > 0)
				{
					if (textAlign == HorizontalAlignment.Center && xViewOffset > maxXY/2 - ClientSize.Width/2)
					{
						xViewOffset -= widthText - layout.Items[layout.Items.Length - 1].bounds.Right;
						xViewOffset = Math.Max(xViewOffset, maxXY/2 - ClientSize.Width/2);
						UpdateClientArea();
					}
					else if (textAlign == HorizontalAlignment.Left && xViewOffset > 0)
					{
						xViewOffset -= widthText - layout.Items[layout.Items.Length - 1].bounds.Right;
						xViewOffset = Math.Max(xViewOffset, 0);
						UpdateClientArea();
					}
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
						UpdateClientArea();
					}
				}
				break;
			}

			case CaretDirection.Right:
			case CaretDirection.WordRight:
			{
				if (GetSelectionLength()>0)
					SetSelectionText("");
				else
				{
					int startPos = GetSelectionStart();
					int newPos =
						dir==CaretDirection.Right
						?
						ComputeCharRightPos(startPos)
						:
						ComputeWordRightPos(startPos);
						
					if (newPos > startPos)
					{
						SetTextInternal(Text.Substring(0, GetSelectionStart()) + Text.Substring(GetSelectionStart() + newPos - startPos));
					}
				}
				SelectInternal(GetSelectionStart(),0);
				break;
			}
		}

		CaretSetEndSelection();
		ScrollToCaretNoRedraw();
		Redraw(ControlGraphics);
	}
	
	// These are the two methods that need to be overridden if the client size is different to the actual size (borders etc.)
	protected override void SetClientSizeCore(int x, int y)
	{
		if (BorderStyle == BorderStyle.None)
			base.SetClientSizeCore(x, y);
		else
			base.SetClientSizeCore (x - 2 * 2, y - 2 * 2);
	}

	// Convert a client size into a window bounds size.
	// This is used to calculate all offsets
	internal override Size ClientToBounds(Size size)
	{
		if (BorderStyle == BorderStyle.None)
			return new Size(size.Width, size.Height);
		else
			return new Size(size.Width + 2 * 2, size.Height + 2 * 2);
	}

	public override Point ClientOrigin
	{
		get
		{
			if (BorderStyle == BorderStyle.None)
				return Point.Empty;
			else
				return new Point(2, 2);
		}
	}

	protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
	{
		// If not Multiline then the control height is the font height
		if (!Multiline)
			height = ClientToBounds(Size.Empty).Height + Font.Height;
		base.SetBoundsCore (x, y, width, height, specified);
		// If the height or width changes then relayout the text
		if ((specified & BoundsSpecified.Height) != 0 | (specified & BoundsSpecified.Width) != 0)
			LayoutFromText();
		ResetView();
	}


	// Paint the text using layout information
	private void DrawText(Graphics g, bool focused)
	{
		for (int i=0; i<Text.Length;i++) 
		{
			LayoutInfo.Item item = layout.Items[i];
			Rectangle bounds = item.bounds;
			bounds.Offset(-xViewOffset + ClientOrigin.X, -yViewOffset + ClientOrigin.Y);
			if (item.type == LayoutInfo.Item.CharType.VisibleChar && g.Clip.IsVisible(bounds))
			{
				Brush fore;
				if (item.selected && focused)
					fore = SelectedForeBrush;
				else
					fore = ForeBrush;

				char c = Text[i];
				if (passwordChar != 0)
					c = passwordChar;
				if (Enabled)
					g.DrawString(c.ToString(), Font, fore, bounds.Location);
				else
					ControlPaint.DrawStringDisabled(g, c.ToString(), Font, BackColor, bounds, StringFormat.GenericDefault);
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
					Point pt = new Point(e.X + ClientOrigin.X,e.Y + ClientOrigin.Y);
					pt.Offset(xViewOffset, yViewOffset);
					
					int closest = CaretGetPosition( pt - new Size(ClientOrigin));
					if (closest >= 0)
						UpdateSelectionInternal(closest);
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
					Point pt = new Point(e.X + ClientOrigin.X,e.Y + ClientOrigin.Y);
					pt.Offset(xViewOffset, yViewOffset);
					int closest = CaretGetPosition( pt - new Size(ClientOrigin));
					if (closest >= 0)
					{
						if (e.Clicks == 2)
						{
							// Select the entire word
							int startPos = closest;
							for (; startPos > 0; --startPos)
							{
								if (!IsWordChar(Text[startPos-1]))
									break;
							}

							int endPos = closest;
							for (; endPos < Text.Length; ++endPos)
							{
								if (!IsWordChar(Text[endPos]))
									break;
							}
							
							SelectInternal(startPos, endPos-startPos);
						}
						else
						{
							SelectInternal(closest, 0);
							mouseDown = true;
							Capture = true;
						}
						
						CaretSetEndSelection();
						// If you click right at the end/beginning, make sure the caret is in view
						ScrollToCaretNoRedraw();
						CaretShow();
						Redraw(ControlGraphics);
					}
				}
				Cursor = Cursors.IBeam;
			}
		}
		else
			mouseDown = false;
	}

	// Takes into account client offset and view offsets
	private void CaretDraw(Graphics g) 
	{
		if (caretHiding)
			return;
		Point p1 = CaretActualBounds.Location;
		Point p2 = p1 + new Size(0, caretBounds.Height - 1);
		g.DrawLine(caretPen, p1, p2);
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
						return maxXY/2;
				default: /*Right*/
					if (Multiline)
						return ClientSize.Width;
					else
						return maxXY;
			}
		}
	}

	// Set the caret bounds from a character position
	// Set update region
	private void CaretSetPosition( int position)
	{
		Rectangle newBounds = Rectangle.Empty;
		int height = Font.Height;
		if (Text.Length == 0)
		{
			newBounds = new Rectangle(CaretXFromAlign, 0, 1, height - 1);
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
				if (item.type == LayoutInfo.Item.CharType.LF)
					newBounds = new Rectangle(CaretXFromAlign, newBounds.Top + height, 1, height - 1);
				else
					newBounds = new Rectangle(newBounds.Right, newBounds.Top, 1, newBounds.Height);			
			}
			else
			{
				newBounds = layout.Items[position].bounds;
				newBounds = new Rectangle(newBounds.Left, newBounds.Top, 1, newBounds.Height);
			}
		}

		// When we change the caret position, find the region to update
		Region region = new Region(newBounds);
		if (!caretHiding)
			region.Xor(caretBounds);
		region.Translate(- xViewOffset + ClientOrigin.X, - yViewOffset + ClientOrigin.Y);
		AddUpdate(region);
		caretBounds = newBounds;
		if (Focused)
			CaretShow();
		
	}

	// Get the caret position of the nearest point relative to pt in layout coordinates
	private int CaretGetPosition(Point pt)
	{
		int prevY = 0;
		int caretPosition = 0;
		Rectangle bounds = new Rectangle(0, 0, 0, Font.Height);
		Rectangle prevBounds = bounds;
		int i=0;
		for (; i<Text.Length;i++) 
		{
			if (layout.Items[i].type != LayoutInfo.Item.CharType.OutOfBoundsChar)
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
						caretPosition -= 2; // (CR, LF)
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
				layout.Items[i-1].type == LayoutInfo.Item.CharType.LF &&
				pt.Y <= prevBounds.Bottom)
				caretPosition -= 2; // (CR, LF)
		}
		
		return caretPosition;
	}

	// Make sure the caret is not hiding, set update region
	private void CaretShow()
	{
		if (caretHiding)
			AddUpdate(CaretActualBounds);
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
				if (Text[end] == '\n')
					break;

		SelectInternal(0, end);
		CaretSetPosition(end);
	}

	private Rectangle CaretActualBounds
	{
		get
		{
			return new Rectangle(caretBounds.Left + ClientOrigin.X - xViewOffset, caretBounds.Top + ClientOrigin.Y - yViewOffset, caretBounds.Width, caretBounds.Height);
		}
	}

	// Sets the caret position to the end of the selection
	private void CaretSetEndSelection()
	{
		// Even if we select backwards, the Caret will be at the end
		CaretSetPosition(GetSelectionStart()+ GetSelectionLength());
	}

	// Change the selection and move the caret. Also make sure its visible
	private void UpdateSelectionInternal(int newPos)
	{
		// We need to select some text
		SelectInternal(selectionStartActual, newPos - selectionStartActual);
		// Caret is always at the end
		CaretSetPosition(newPos);
		ScrollToCaretNoRedraw();
		Redraw(ControlGraphics);
	}

	private static bool IsWordChar(char ch)
	{
		return Char.IsLetterOrDigit(ch);
	}

	private static bool IsTowardsTextEnd(CaretDirection dir)
	{
		return
			dir == CaretDirection.Right
			||
			dir == CaretDirection.WordRight
			||
			dir == CaretDirection.LineEnd
			||
			dir == CaretDirection.LineDown
			||
			dir == CaretDirection.PageDown
			||
			dir == CaretDirection.TextEnd;
	}

	private int ComputeLineStartPos(int fromPos)
	{
		for (; fromPos > 0; --fromPos)
		{
			if (Text[fromPos-1] == '\n')
				break;
		}
		return fromPos;
	}

	private int ComputeLineEndPos(int fromPos)
	{
		for (; fromPos < Text.Length; ++fromPos)
		{
			if (Text[fromPos] == '\r')
				break;
		}
		return fromPos;
	}
	
	private int ComputeCharLeftPos(int fromPos)
	{
		int newPos = fromPos;
		if (newPos > 0) --newPos;
		if (newPos>0 && Text[newPos]=='\n')
			--newPos;
		return newPos;
	}
	
	private int ComputeCharRightPos(int fromPos)
	{
		int newPos = fromPos;
		if (newPos < Text.Length) ++newPos;
		if (newPos<Text.Length && Text[newPos]=='\n')
			++newPos;
		return newPos;
	}
	
	private int ComputeWordLeftPos(int fromPos)
	{
		if (fromPos == 0)
			return 0;
			
		fromPos = ComputeCharLeftPos(fromPos);

		for (; fromPos > 0; --fromPos)
		{
			if (!IsWordChar(Text[fromPos-1]))
				break;
		}

		return fromPos;
	}

	private int ComputeWordRightPos(int fromPos)
	{
		// Move past at least one non-word element
		// to right before the next word element.
		// If the character immediately to our
		// left is a word element, we move one
		// after the first non-word element to
		// our left. Otherwise, we move to the
		// second non-word element to our left.
		for (; fromPos < Text.Length; ++fromPos)
		{
			if (!IsWordChar(Text[fromPos]))
				break;
		}

		for (; fromPos < Text.Length; ++fromPos)
		{
			if (IsWordChar(Text[fromPos]))
				break;
		}

		return fromPos;
	}

	private int ComputeLineOffset(int fromPos, int nbLines)
	{
		int y = (caretBounds.Y + caretBounds.Bottom) / 2 + nbLines * caretBounds.Height;
		if (y > 0)
			return CaretGetPosition(new Point( caretBounds.X, y));
		else
			return 0;
	}

	// Add a region to include in the draw update
	private void AddUpdate(Region region)
	{
		Region clip = ControlGraphics.Clip;
		clip.Union( region);
		ControlGraphics.Clip = clip;
	}

	// Add a rectangle to include in the draw update
	private void AddUpdate(Rectangle rectangle)
	{
		Region clip = ControlGraphics.Clip;
		clip.Union( rectangle);
		ControlGraphics.Clip = clip;
	}
	
#if !CONFIG_COMPACT_FORMS

#endif // !CONFIG_COMPACT_FORMS

}; // class TextBox

}; // namespace System.Windows.Forms
