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
using System.Text;

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
	private bool inTextChangedEvent;
	private VScrollBar vScrollBar;
	private HScrollBar hScrollBar;
	
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

	// Height chosen, if not multiline could be different from actual
	private int chosenHeight;

	// Helpers, to replace the missing "Math" class in some profiles.
	private static int Math_Max(int a, int b)
	{
		if(a > b)
		{
			return a;
		}
		else
		{
			return b;
		}
	}
	private static int Math_Min(int a, int b)
	{
		if(a < b)
		{
			return a;
		}
		else
		{
			return b;
		}
	}
	private static int Math_Abs(int a)
	{
		if(a < 0)
		{
			return -a;
		}
		else
		{
			return a;
		}
	}

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
		WordWrapChanged +=new EventHandler(HandleWordWrapChanged);

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
			if (scrollBars == value)
				return;
			scrollBars = value;
			layout = null;
		}
	}

	private void SetupScrollBars()
	{
		// Set vertical scrollbar
		if (scrollBars == ScrollBars.Both || scrollBars == ScrollBars.Vertical)
		{
			if (vScrollBar == null)
			{
				vScrollBar = new VScrollBar();
				vScrollBar.backColor = SystemColors.ScrollBar;
				vScrollBar.ValueChanged+=new EventHandler(vScrollBar_ValueChanged);
				SetScrollBarPositions();
				Controls.Add(vScrollBar);
			}
			vScrollBar.Maximum = 0;
		}
		else if (vScrollBar != null)
		{
			Controls.Remove(vScrollBar);
			vScrollBar = null;
		}
		// Set horizontal scrollbar
		// There is no scrollbar if the textbox is multiline
		if ((scrollBars == ScrollBars.Both || scrollBars == ScrollBars.Horizontal))
		{
			if (hScrollBar == null)
			{
				hScrollBar = new HScrollBar();
				hScrollBar.backColor = SystemColors.ScrollBar;
				hScrollBar.ValueChanged+=new EventHandler(hScrollBar_ValueChanged);
				SetScrollBarPositions();
				Controls.Add(hScrollBar);
			}
			hScrollBar.Maximum = 0;
		}
		else if (hScrollBar != null)
		{
			Controls.Remove(hScrollBar);
			hScrollBar = null;
		}
	}

	// Setup the positions of the scrollBar depending on the combination
	private void SetScrollBarPositions()
	{
		if (vScrollBar == null && hScrollBar == null)
			return;
		
		int width;
		if (vScrollBar == null)
			width = ClientRectangle.Width;
		else
		{
			width = ClientRectangle.Width - vScrollBar.Width;
			if (width < 5)
			{
				width = ClientRectangle.Width;
				vScrollBar.Visible = false;
			}
			else
				vScrollBar.Visible = true;
		}

		int height;
		if (hScrollBar == null)
			height = ClientRectangle.Height;
		else
		{
			height = ClientRectangle.Height - hScrollBar.Height;
			if (height < 5)
			{
				height = ClientRectangle.Height;
				hScrollBar.Visible = false;
			}
			else
				hScrollBar.Visible = true;
		}


		if (vScrollBar != null)
		{
			vScrollBar.Bounds = new Rectangle(ClientRectangle.Width - vScrollBar.Width, 0, vScrollBar.Width, height);
			
			int remainder = TextDrawArea.Height % Font.Height;
			int maximum = MaxTextDimensions.Height + remainder;
			
			if (maximum < TextDrawArea.Height)
				maximum = TextDrawArea.Height;
			vScrollBar.Enabled = (maximum != TextDrawArea.Height);
			vScrollBar.Maximum = maximum;
			vScrollBar.SmallChange = Font.Height;
			vScrollBar.LargeChange = TextDrawArea.Height + 1;
		}
		if (hScrollBar != null)
		{
			hScrollBar.Bounds = new Rectangle(0, ClientRectangle.Height - hScrollBar.Height, width, hScrollBar.Height);
			int maximum = MaxTextDimensions.Width;
			if (maximum < TextDrawArea.Width)
				maximum = TextDrawArea.Width;
			hScrollBar.Enabled = (maximum != TextDrawArea.Width);
			hScrollBar.Maximum = maximum;
			hScrollBar.SmallChange = 5;
			hScrollBar.LargeChange = TextDrawArea.Width + 1;
		}
	}

	private Size MaxTextDimensions
	{
		get
		{
			// TODO handle case right to left
			int x = 0;
			int y = 0;
			if (Text.Length > 0)
			{
				y = layout.Items[Text.Length - 1].bounds.Bottom;
				x = layout.Items[Text.Length - 1].bounds.Right;
			}
			return new Size(x, y);
		}
	}

	private void vScrollBar_ValueChanged(object sender, EventArgs e)
	{
		YViewOffset = vScrollBar.Value;
		SetClipAllText();
		Redraw(ControlGraphics);
	}

	private void hScrollBar_ValueChanged(object sender, EventArgs e)
	{
		XViewOffset = hScrollBar.Value;
		SetClipAllText();
		Redraw(ControlGraphics);
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
				char cPrevious = (char)0;
				for (int i = 0; i < value.Length; i++)
				{
					char c = value[i];
					bool isNext = (i < value.Length - 1);
					char cNext = (char)0;
					if (isNext)
						cNext = value[i+1];
					if ((!isNext || cNext != '\n') && c == '\r')
						sb.Append("\r\n");
					else if (c=='\n' && cPrevious != '\r')
						sb.Append("\r\n");
					else
						sb.Append(c);
					cPrevious = c;
				}
			}
			SetTextActual( sb.ToString());
			// Set the position to the end
			SelectInternal(Text.Length, 0);
			if (!inTextChangedEvent)
			{
				if (IsHandleCreated)
				{
					CaretSetEndSelection();
					ResetView();
					Redraw(ControlGraphics);
				}
				OnTextChanged(EventArgs.Empty);
			}

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
			// Break into strings
			for (int i = 0; i < Text.Length; i++)
			{
				if (Text[i] == '\r' && i < Text.Length - 1 && Text[i+1] == '\n') // Look for CRLF
				{
					lines[line++] = Text.Substring(start, i - start);
					i+=2;
					start = i;
				}
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
			SetTextActual( sb.ToString());
			ResetView();
			Redraw(ControlGraphics);
			OnTextChanged(EventArgs.Empty);
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
				if (IsHandleCreated)
				{
					// Layout changes
					LayoutFromText(Text);
					SetScrollBarPositions();
					ResetView();
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
			if (layout.Items[i].selected)
			{
				Rectangle b = layout.Items[i].bounds;
				update.Union(new Region(new Rectangle(b.Left,b.Top,b.Width,b.Height + 1)));
			}
		update.Translate(- XViewOffset, - YViewOffset);
		AddUpdate(update);
		caretHiding = true;
		Redraw(ControlGraphics);
		mouseDown = false;
		// We dont need to update any selection
		selectedRegion = null;
	}

	protected override void OnFontChanged(EventArgs e)
	{
		base.OnFontChanged (e);
		if (!Multiline && AutoSize)
			//*
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

	// Handle "KeyPress" events for the text box.
	private void HandleKeyPress(Object sender, KeyPressEventArgs e)
	{
		if (ReadOnly)
			return;
			
		char c = e.KeyChar;

		// Discard enter if not multiline
		if (c=='\r' && !Multiline)
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
		OnTextChanged(EventArgs.Empty);

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
		Point pt = PointToClient(MousePosition);
		ProcessMouse(new MouseEventArgs(MouseButtons.Left, 2, pt.X, pt.Y, 0));
	}

	// Handle "Paint" events for the text box.
	// In our implementation NO painting happens outside of the paint event. This might change because it might not update fast enough
	private void HandlePaint(Object sender, PaintEventArgs e)
	{
		Redraw(e.Graphics);
	}

	// Redraw a specific portion of the textbox
	private void Redraw(Graphics g)
	{
		if (layout == null)
		{
			LayoutFromText(Text);
			SetupScrollBars();
			ResetView();
		}
		// Draw scrollbar corner if both are visible
		if (vScrollBar != null && hScrollBar != null && vScrollBar.Visible && hScrollBar.Visible)
			g.FillRectangle(SystemBrushes.Control, hScrollBar.Right, vScrollBar.Bottom, vScrollBar.Width, hScrollBar.Height);
		// Only allow updates in the TextDrawArea
		Region clip = g.Clip;
		clip.Intersect(new Region(TextDrawArea));
		g.SetClip(clip, Drawing.Drawing2D.CombineMode.Replace);

		bool focused = Focused;
		if (!Enabled || ReadOnly)
			g.FillRegion(DisabledBackBrush, g.Clip);
		else
			g.FillRegion(BackBrush, g.Clip);
		// Draw the background of the selected text
		if (focused && selectedRegion != null)
		{
			Region r = selectedRegion.Clone();
			r.Translate(- XViewOffset, - YViewOffset);
			g.FillRegion(SelectedBackBrush, r);
		}
		DrawText(g, focused);
		if (focused)
			CaretDraw(g);
		SetClipEmpty();
	}

	// Handle the event when multiline is changed.
	private void HandleMultilineChanged(object sender, EventArgs e)
	{
		layout = null;
		// Set back the actual chosen height
		// Will cause LayoutFromText to be called
		Height = chosenHeight;
	}

	private Graphics ControlGraphics
	{
		get
		{
			if (graphics == null)
			{
				graphics = CreateGraphics();
				SetClipEmpty();
			}
			return graphics;
		}
	}

	private void SetClipEmpty()
	{
		Region r = new Region();
		r.MakeEmpty();
		ControlGraphics.SetClip(r, Drawing.Drawing2D.CombineMode.Replace);
	}

	private void SetClipAllText()
	{
		Region r = new Region(TextDrawArea);
		ControlGraphics.SetClip(r, Drawing.Drawing2D.CombineMode.Replace);
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
	protected void LayoutFromText(String newText)
	{
		if (!IsHandleCreated)
			return;
		if (layout == null)
		{
			layout = new LayoutInfo();
			layout.Items = new LayoutInfo.Item[0];
		}

		// Optimization - only re-layout from the beginning of the last line modified.
		// Find posLine, the position of the beginning of the line we must start updating from.
		// yLine is the y coordinate of this point.
		int yLine = 1;
		int posLine = 0;
		for (int i = 0; i < layout.Items.Length; i++)
		{
			if (i > newText.Length - 1)
				break;
			
			if (layout.Items[i].bounds.Top != yLine)
			{
				posLine = i;
				yLine = layout.Items[i].bounds.Top;
			}
			if (newText[i] != Text[i])
				break;
		}
		// We leave 1 pixel on the left and right for the caret
		// Multiline textboxes are infinite in the y direction
		// non multiline are infinite in the x direction and we scroll when needed
		// This is the area we need to lay the text into.
		Rectangle measureBounds;
		if (Multiline)
				measureBounds = new Rectangle(1, yLine - 1, TextDrawArea.Width - 2, maxXY - (yLine - 1));
			else
				measureBounds = new Rectangle(1, yLine - 1, maxXY, TextDrawArea.Height - (yLine - 1));

		string measureText = newText.Substring(posLine);

		// Convert the control settings to a StringFormat
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
	
		// To use MeasureCharacterRanges, we need to specify that we want to measure each character individually.
		// This MS api isnt very clever!
		CharacterRange[] range = new CharacterRange[measureText.Length];
		for (int i = 0; i < measureText.Length; i++)
			range[i] = new CharacterRange(i, 1);
		format.SetMeasurableCharacterRanges(range);
		
		Region[] bounds;
		if (measureText.Length == 0)
			bounds = new Region[0];
		else if (passwordChar == 0)
			bounds = ControlGraphics.MeasureCharacterRanges(measureText, Font, measureBounds, format);
		else
			bounds = ControlGraphics.MeasureCharacterRanges(new string(passwordChar, measureText.Length), Font, measureBounds, format);
		LayoutInfo.Item[] newItems = new LayoutInfo.Item[newText.Length];
		// Copy in the previously measured items.
		Array.Copy(layout.Items, 0, newItems, 0, posLine);
		layout.Items = newItems;
		
		// Convert the MeasureCharacterRanges to LayoutInfo
		// MeasureCharacterRanges will return an empty rectangle for all characters
		// that are not visible. We need to figure out the positions of LF and spaces
		// that are swallowed
		Rectangle prevBounds;
		if (posLine == 0)
			prevBounds = new Rectangle(CaretXFromAlign, 0, 0, Font.Height);
		else
			prevBounds = layout.Items[posLine - 1].bounds;
		LayoutInfo.Item.CharType prevType = LayoutInfo.Item.CharType.VisibleChar;
		for (int i = posLine; i < newText.Length;i++)
		{
			LayoutInfo.Item item = new LayoutInfo.Item();
			char c = newText[i];
			Rectangle rect = Rectangle.Truncate(bounds[i - posLine].GetBounds( ControlGraphics));
			if (c == '\r')
			{
				item.type = LayoutInfo.Item.CharType.CR;
				// Return. The bounds is to the right of the previous character.
				// If the previous character was also a linefeed, we move down a line
				// from the previous bounds.
				if (i > 0 && newText[i-1] == '\n')
					rect = new Rectangle(CaretXFromAlign, prevBounds.Top + Font.Height, 0, Font.Height);
				else
					rect = new Rectangle(prevBounds.Right, prevBounds.Top, 2, prevBounds.Height);
			}
			else if (c == '\n')
			{
				item.type = LayoutInfo.Item.CharType.LF;
				rect = new Rectangle(prevBounds.Right, prevBounds.Top, 2, prevBounds.Height);
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

	// Make sure the caret is visible
	protected override void ScrollToCaretInternal()
	{
		ScrollToCaretNoRedraw();
		Redraw(ControlGraphics);
	}

	// Called to recalculate the offsets if make sure bounds is visible.
	private void ScrollToCaretNoRedraw()
	{
		if (caretBounds.Top - YViewOffset < TextDrawArea.Top)
		{
			YViewOffset = caretBounds.Top;
		}
		else if (caretBounds.Bottom - YViewOffset > TextDrawArea.Bottom)
		{
			YViewOffset = caretBounds.Bottom - TextDrawArea.Bottom + 1;
		}
		if (caretBounds.Left- XViewOffset < TextDrawArea.Left)
		{
			XViewOffset = caretBounds.Left;
		}
		else if (caretBounds.Right - XViewOffset > TextDrawArea.Right)
		{
			XViewOffset = caretBounds.Right - TextDrawArea.Right;
		}
	}

	// When changing text, bounds or alignment, we make sure the view includes the position of the first character.
	private void ResetView()
	{
		XViewOffset = 0;
		YViewOffset = 0;
		//Set caret to the beginning
		CaretSetPosition(0);
		if (!Multiline)
		{
			if (textAlign == HorizontalAlignment.Center)
				XViewOffset = maxXY / 2 - TextDrawArea.Width / 2;
			else if (textAlign == HorizontalAlignment.Right)
				XViewOffset = maxXY - TextDrawArea.Width + 2;
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
		if (!IsHandleCreated)
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
			redrawRegion.Translate(-XViewOffset, -YViewOffset);
			// Only allow updates in the TextDrawArea
			redrawRegion.Intersect(new Region(TextDrawArea));
			AddUpdate(redrawRegion);
		}
		else
			AddUpdate(newRegion);
		selectedRegion = newRegion;

	}

	protected override void SetTextInternal(string text)
	{
		SetTextActual(text);
		Redraw(ControlGraphics);
	}

	
	// Called to change the text. Sets the update to whats needed to but doesnt change the selection point or caret
	private void SetTextActual( string text)
	{
		if (!IsHandleCreated)
		{
			(this as Control).text = text;
			return;
		}
		// Layout the new text. Compare with old layout, Creating a region for areas that must be updated.
		bool prevLayout = layout != null;
		LayoutInfo oldLayout = null;
		if (prevLayout)
			oldLayout = (LayoutInfo)layout.Clone();
		string oldText = Text;
		LayoutFromText(text);
		// We must not trigger the onTextChanged event yet else this controls text could be change in the event!
		(this as Control).text = text;
		SetScrollBarPositions();
		if (prevLayout)
		{
			Region update = new Region(RectangleF.Empty);
			for (int i=0;i < Math_Max(oldText.Length, text.Length);i++)
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
			// Get the offset of the TextDrawArea
			update.Translate( - XViewOffset, - YViewOffset);
			AddUpdate(update);
		}
	}

	// Get the length of the selection.
	internal override int GetSelectionLength()
	{
		return Math_Abs(selectionLengthActual);
	}

	// Get the start of the selection.
	// Our length could be negative
	internal override int GetSelectionStart()
	{
		return Math_Min(selectionStartActual, selectionStartActual + selectionLengthActual);
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
			if (caretBounds.Top >= caretBounds.Height )
				newPos = ComputeLineOffset(newPos, -1);
			break;
		case CaretDirection.LineDown:
			if (caretBounds.Top < layout.Items[layout.Items.Length - 1].bounds.Top)
				newPos = ComputeLineOffset(newPos, 1);
			break;
		case CaretDirection.PageUp:
			newPos = ComputeLineOffset(newPos, - (int) TextDrawArea.Height / Font.Height);
			break;
		case CaretDirection.PageDown:
			newPos = ComputeLineOffset(newPos, (int) TextDrawArea.Height / Font.Height);
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
						SetTextActual(Text.Substring(0, GetSelectionStart() - nbCharsToDelete) + Text.Substring(GetSelectionStart()));
						SelectInternal(GetSelectionStart()-nbCharsToDelete, 0);
					}
				}
				OnTextChanged(EventArgs.Empty);

				// In the case of multiline we ensure that we recover any blank lines created by backspacing at the bottom (if the text is bigger than the view area).
				// In the case of non multiline, we recover any character space that is now there after deleting
				if (!Multiline && layout.Items.Length > 0)
				{
					if (textAlign == HorizontalAlignment.Center && XViewOffset > maxXY/2 - TextDrawArea.Width/2)
					{
						XViewOffset -= widthText - layout.Items[layout.Items.Length - 1].bounds.Right;
						XViewOffset = Math_Max(XViewOffset, maxXY/2 - TextDrawArea.Width/2);
						SetClipAllText();
					}
					else if (textAlign == HorizontalAlignment.Left && XViewOffset > 0)
					{
						XViewOffset -= widthText - layout.Items[layout.Items.Length - 1].bounds.Right;
						XViewOffset = Math_Max(XViewOffset, 0);
						SetClipAllText();
					}
				}
				else
				{
					//Move the caret first
					CaretSetEndSelection();
					if (bottomCaret != caretBounds.Bottom)
					{
						int yViewOffset = YViewOffset - bottomCaret + caretBounds.Bottom - 1;
						if (yViewOffset < 0)
							yViewOffset = 0;
						YViewOffset = yViewOffset;
						SetClipAllText();
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
						SetTextActual(Text.Substring(0, GetSelectionStart()) + Text.Substring(GetSelectionStart() + newPos - startPos));
					}
				}
				SelectInternal(GetSelectionStart(),0);
				OnTextChanged(EventArgs.Empty);
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

	protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
	{
		chosenHeight = height;
		// If not Multiline then the control height is the font height
		if (!Multiline)
			height = ClientToBounds(Size.Empty).Height + Font.Height + 1;
		base.SetBoundsCore (x, y, width, height, specified);
		if (!IsHandleCreated)
			return;
		// If the height or width changes then relayout the text
		if ((specified & BoundsSpecified.Height) != 0 | (specified & BoundsSpecified.Width) != 0)
		{
			// ControlGraphics must be recreated.
			if (graphics != null)
				graphics.Dispose();
			graphics = null;
			LayoutFromText(Text);
			SetScrollBarPositions();
		}
		ResetView();
	}

	// Paint the text using layout information
	private void DrawText(Graphics g, bool focused)
	{
		Font font = Font;
		StringBuilder s = new StringBuilder();
		Brush prevFore = null;
		int x = -1;
		int y = -1;
		int textLength = 0;
		if (text != null)
			textLength = text.Length;
		for (int i=0; i < textLength;i++) 
		{
			LayoutInfo.Item item = layout.Items[i];
			Rectangle bounds = item.bounds;
			bounds.Offset(-XViewOffset, -YViewOffset);
			if (item.type == LayoutInfo.Item.CharType.VisibleChar)
			{
				Brush fore;
				if (item.selected && focused)
					fore = SelectedForeBrush;
				else
					fore = ForeBrush;
				if (bounds.Top == y && prevFore == fore)
				{
					if (passwordChar != 0)
						s.Append(passwordChar);
					else
						s.Append(text[i]);
				}
				else
				{
					if (s.Length > 0)
					{
						if (Enabled )
							g.DrawString(s.ToString(), font, prevFore, new Point (x, y));
						else
							ControlPaint.DrawStringDisabled(g, s.ToString(), font, BackColor, new Rectangle(x, y, int.MaxValue, int.MaxValue), StringFormat.GenericDefault);
					}
					s.Remove(0, s.Length);
					if (passwordChar != 0)
						s.Append(passwordChar);
					else
						s.Append(text[i]);
					x = bounds.X;
					y = bounds.Y;
				}
				prevFore = fore;
			}
		}
		if (s.Length != 0)
		{
			if (Enabled )
				g.DrawString(s.ToString(), Font, prevFore, new Point (x, y));
			else
				ControlPaint.DrawStringDisabled(g, s.ToString(), font, BackColor, new Rectangle(x, y, int.MaxValue, int.MaxValue), StringFormat.GenericDefault);
		}
	}

	private int previousClosest = -1;

	// Handle all mouse processing
	private void ProcessMouse(MouseEventArgs e)
	{
		if (Enabled) 
		{
			if (mouseDown)
			{
				if (e.Button == MouseButtons.Left)
				{		
					Point pt = new Point(e.X,e.Y);
					pt.Offset(XViewOffset, YViewOffset);
					
					int closest = CaretGetPosition(pt);
					if (closest >= 0 && previousClosest != closest)
						UpdateSelectionInternal(closest);
					previousClosest = closest;
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
					Point pt = new Point(e.X,e.Y);
					previousClosest = -1;
					pt.Offset(XViewOffset, YViewOffset);
					int closest = CaretGetPosition(pt);
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
					return 1;
				case(HorizontalAlignment.Center):
					if (Multiline)
						return TextDrawArea.Width/2;
					else
						return maxXY/2;
				default: /*Right*/
					if (Multiline)
						return TextDrawArea.Width;
					else
						return maxXY;
			}
		}
	}

	// Set the caret bounds from a character position
	// Set update region
	internal override void CaretSetPosition( int position)
	{
		if (!IsHandleCreated)
			return;
		Rectangle newBounds = Rectangle.Empty;
		int height = Font.Height;
		if (Text.Length == 0)
		{
			newBounds = new Rectangle(CaretXFromAlign, 1, 1, height);
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
					newBounds = new Rectangle(CaretXFromAlign, newBounds.Top + height, 1, height);
				else
					newBounds = new Rectangle(newBounds.Right, newBounds.Top, 1, newBounds.Height + 1);			
			}
			else
			{
				newBounds = layout.Items[position].bounds;
				newBounds = new Rectangle(newBounds.Left, newBounds.Top, 1, newBounds.Height + 1);
			}
		}

		// This looks better.
		if (newBounds.X == 0)
			newBounds.X = 1;

		// When we change the caret position, find the region to update
		Region region = new Region(newBounds);
		if (!caretHiding)
			region.Xor(caretBounds);
		region.Translate(- XViewOffset, - YViewOffset);
		AddUpdate(region);
		caretBounds = newBounds;
		if (Focused)
			CaretShow();
		
	}

	// Get the caret position of the nearest point relative to pt in layout coordinates
	private int CaretGetPosition(Point pt)
	{
		int prevY = int.MinValue;
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

		// CR's only get selected, so if this position is the beginning of a selection then select the LF as well
		if (caretPosition < (GetSelectionStart()+ GetSelectionLength()) && Text.Length > 1 && layout.Items[caretPosition].type == LayoutInfo.Item.CharType.LF && layout.Items[caretPosition - 1].type == LayoutInfo.Item.CharType.CR)
			caretPosition--;
	
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
			return new Rectangle(caretBounds.Left - XViewOffset, caretBounds.Top - YViewOffset, caretBounds.Width, caretBounds.Height);
		}
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

	protected override void SetBorderStyle(BorderStyle borderStyle)
	{
		BorderStyleInternal = borderStyle;
		// ControlGraphics must be recreated.
		if (graphics != null)
			graphics.Dispose();
		graphics = null;
	}

	protected override void OnTextChanged(EventArgs e)
	{
		inTextChangedEvent = true;
		base.OnTextChanged (e);
		inTextChangedEvent = false;
	}

	private void HandleWordWrapChanged(object sender, EventArgs e)
	{
		layout = null;
	}

	private Rectangle TextDrawArea
	{
		get
		{
			Rectangle rect = ClientRectangle;
			if (vScrollBar != null && vScrollBar.Visible)
				rect.Width -= vScrollBar.Width;
			if (hScrollBar != null && hScrollBar.Visible)
				rect.Height -= hScrollBar.Height;
			return rect;
		}
	}

	private int XViewOffset
	{
		get
		{
			return xViewOffset;
		}
		set
		{
			if (value == xViewOffset)
				return;
			// Make sure the entire textbox is redrawn
			SetClipAllText();
			xViewOffset = value;
			if (hScrollBar != null)
				hScrollBar.Value = xViewOffset;
		}
	}

	private int YViewOffset
	{
		get
		{
			return yViewOffset;
		}
		set
		{
			if (value == yViewOffset)
				return;
			// Make sure the entire textbox is redrawn
			SetClipAllText();
			yViewOffset = value;
			if (vScrollBar != null && vScrollBar.Visible)
				vScrollBar.Value = yViewOffset;
		}
	}
	
#if !CONFIG_COMPACT_FORMS

#endif // !CONFIG_COMPACT_FORMS

	
}; // class TextBox

}; // namespace System.Windows.Forms
