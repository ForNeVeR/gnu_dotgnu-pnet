/*
 * TextBoxBase.cs - Implementation of the
 *			"System.Windows.Forms.TextBoxBase" class.
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

public abstract class TextBoxBase : Control
{
	// Internal state.
	private bool acceptsTab;
	private bool autoSize;
	private bool hideSelection;
	private bool modified;
	private bool multiline;
	private bool readOnly;
	private bool wordWrap;
	private BorderStyle borderStyle;
	private int maxLength;

	// Constructor.
	internal TextBoxBase()
			{
				acceptsTab = false;
				autoSize = true;
				hideSelection = true;
				modified = false;
				multiline = false;
				readOnly = false;
				wordWrap = true;
				borderStyle = BorderStyle.Fixed3D;
				maxLength = 32767;
			}

	// Get or set this object's properties.
	public bool AcceptsTab
			{
				get
				{
					return acceptsTab;
				}
				set
				{
					if(acceptsTab != value)
					{
						acceptsTab = value;
						OnAcceptsTabChanged(EventArgs.Empty);
					}
				}
			}

	public virtual bool AutoSize
			{
				get
				{
					return autoSize;
				}
				set
				{
					if(autoSize != value)
					{
						autoSize = value;
						OnAutoSizeChanged(EventArgs.Empty);
					}
				}
			}
	
	public BorderStyle BorderStyle
			{
				get
				{
					return borderStyle;
				}
				set
				{
					if(borderStyle != value)
					{
						borderStyle = value;
						OnBorderStyleChanged(EventArgs.Empty);
					}
				}
			}

	abstract public bool CanUndo
			{ get; }

	protected override Size DefaultSize
			{
				get
				{
					return new Size(100, PreferredHeight);
				}
			}
	
	public bool HideSelection
			{
				get
				{
					return hideSelection;
				}
				set
				{
					if(hideSelection != value)
					{
						hideSelection = value;
						OnHideSelectionChanged(EventArgs.Empty);
					}
				}
			}

	public abstract String[] Lines
			{ get; set; }

	public virtual int MaxLength
			{
				get
				{
					return maxLength;
				}
				set
				{
					if(value < 0)
					{
						throw new ArgumentException
							(S._("SWF_NonNegative"));
					}
					maxLength = value;
				}
			}

	public bool Modified
			{
				get
				{
					return modified;
				}
				set
				{
					if(modified != value)
					{
						modified = value;
						OnModifiedChanged(EventArgs.Empty);
					}
				}
			}

	public virtual bool Multiline
			{
				get
				{
					return multiline;
				}
				set
				{
					if(multiline != value)
					{
						multiline = value;
						OnMultilineChanged(EventArgs.Empty);
					}
				}
			}

	public int PreferredHeight
			{
				get
				{
					if(borderStyle == BorderStyle.None)
					{
						return FontHeight;
					}
					else
					{
						return FontHeight + 3 +
							   SystemInformation.BorderSize.Height * 4;
					}
				}
			}

	public bool ReadOnly
			{
				get
				{
					return readOnly;
				}
				set
				{
					if(readOnly != value)
					{
						readOnly = value;
						OnReadOnlyChanged(EventArgs.Empty);
					}
				}
			}

	public virtual String SelectedText
			{
				get
				{
					int start = SelectionStart;
					int length = SelectionLength;
					if(start >= 0)
					{
						return Text.Substring(start, length);
					}
					else
					{
						return null;
					}
				}
				set
				{
					SetSelectionText(value);
					ClearUndo();
				}
			}

	public virtual int SelectionLength
			{
				get
				{
					return GetSelectionLength();
				}
				set
				{
					if(value < 0)
					{
						throw new ArgumentException
							(S._("SWF_NonNegative"));
					}
					SetSelectionLength(value);
				}
			}

	public virtual int SelectionStart
			{
				get
				{
					return GetSelectionStart();
				}
				set
				{
					if(value < 0)
					{
						throw new ArgumentException
							(S._("SWF_NonNegative"));
					}
					SetSelectionStart(value);
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
	public virtual int TextLength
			{
				get
				{
					String text = Text;
					if(text != null)
					{
						return text.Length;
					}
					else
					{
						return 0;
					}
				}
			}
	public bool WordWrap
			{
				get
				{
					return wordWrap;
				}
				set
				{
					wordWrap = value;
				}
			}

	// Append text to this control.
	[TODO]
	public void AppendText(String text)
			{
				// TODO
			}

	// Clear all text from this control.
	public void Clear()
			{
				Text = null;
			}

	// Clear undo information for this control.
	[TODO]
	public void ClearUndo()
			{
				// TODO
			}

	// Copy the current selection to the clipboard.
	[TODO]
	public void Copy()
			{
				// TODO
			}

	// Create the handle for this control.
	protected override void CreateHandle()
			{
				// Let the base class do the work.
				base.CreateHandle();
			}

	// Cut the current selection to the clipboard.
	[TODO]
	public void Cut()
			{
				// TODO
			}

	// Determine if a key is recognized by a control as an input key.
	protected override bool IsInputKey(Keys keyData)
			{
				if((keyData & Keys.Alt) == 0)
				{
					Keys code = (keyData & Keys.KeyCode);
					if(code == Keys.Tab)
					{
						if(!Multiline || !acceptsTab)
						{
							return false;
						}
						if((keyData & Keys.Control) != 0)
						{
							return false;
						}
						else
						{
							return true;
						}
					}
					else if(code == Keys.Escape)
					{
						if(Multiline)
						{
							return false;
						}
					}
					else if(code >= Keys.Prior && code <= Keys.Home)
					{
						return true;
					}
				}
				return base.IsInputKey(keyData);
			}

	// Paste the clipboard and replace the current selection.
	[TODO]
	public void Paste()
			{
				// TODO
			}

	// Process a dialog key.
	[TODO]
	protected override bool ProcessDialogKey(Keys keyData)
			{
				// TODO
				return base.ProcessDialogKey(keyData);
			}

	// Scroll the text box to make the caret visible.
	public void ScrollToCaret()
			{
				ScrollToCaretInternal();
			}

	// Move the selection.
	public void Select(int start, int length)
			{
				if(start < 0)
				{
					throw new ArgumentException
						(S._("SWF_NonNegative"), "start");
				}
				if(length < 0)
				{
					throw new ArgumentException
						(S._("SWF_NonNegative"), "length");
				}
				SelectInternal( start, length);
			}

	// Select all text in the control.
	public void SelectAll()
			{
				Select(0, TextLength);
			}

	// Inner core of "SetBounds".
	[TODO]
	protected override void SetBoundsCore
				(int x, int y, int width, int height,
				 BoundsSpecified specified)
			{
				// TODO
				base.SetBoundsCore(x, y, width, height, specified);
			}

	// Convert this object into a string.
	public override String ToString()
			{
				String text = Text;
				if(text != null && text.Length > 40)
				{
					text = text.Substring(0, 40) + " ...";
				}
				return base.ToString() + ", Text: " + text;
			}

	// Undo the last editing operation.
	[TODO]
	public void Undo()
			{
				// TODO
			}

	// Events that may be emitted by this control.
	public event EventHandler AcceptsTabChanged
			{
				add
				{
					AddHandler(EventId.AcceptsTabChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.AcceptsTabChanged, value);
				}
			}

	public event EventHandler AutoSizeChanged
			{
				add
				{
					AddHandler(EventId.AutoSizeChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.AutoSizeChanged, value);
				}
			}

	public new event EventHandler Click
			{
				add
				{
					AddHandler(EventId.TextBoxClick, value);
				}
				remove
				{
					RemoveHandler(EventId.TextBoxClick, value);
				}
			}

	public event EventHandler HideSelectionChanged
			{
				add
				{
					AddHandler(EventId.HideSelectionChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.HideSelectionChanged, value);
				}
			}

	public event EventHandler ModifiedChanged
			{
				add
				{
					AddHandler(EventId.ModifiedChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.ModifiedChanged, value);
				}
			}

	public event EventHandler MultilineChanged
			{
				add
				{
					AddHandler(EventId.MultilineChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.MultilineChanged, value);
				}
			}

	public event EventHandler ReadOnlyChanged
			{
				add
				{
					AddHandler(EventId.ReadOnlyChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.ReadOnlyChanged, value);
				}
			}

	// Dispatch events from this control.
	protected virtual void OnAcceptsTabChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.AcceptsTabChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}

	protected virtual void OnAutoSizeChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.AutoSizeChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}

	protected virtual void OnBorderStyleChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.BorderStyleChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}

	[TODO]
	protected override void OnFontChanged(EventArgs e)
			{
				base.OnFontChanged(e);
				// TODO: adjust the height of the control to match the font
			}

	protected override void OnHandleCreated(EventArgs e)
			{
				base.OnHandleCreated(e);
			}

	protected override void OnHandleDestroyed(EventArgs e)
			{
				base.OnHandleDestroyed(e);
			}

	protected virtual void OnHideSelectionChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.HideSelectionChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}

	protected virtual void OnModifiedChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.ModifiedChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}

	protected virtual void OnMultilineChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.MultilineChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}

	protected virtual void OnReadOnlyChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.ReadOnlyChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}

	// Set the selection text to a new value.
	internal virtual void SetSelectionText(String value)
			{
				int start = SelectionStart;
				int length = SelectionLength;
				String text = Text;
				int vlength;
				String newText;
				if(start >= 0)
				{
					if(value == null)
					{
						value = String.Empty;
					}
					vlength = value.Length;
					newText = text.Substring(0, start) + value +
							  text.Substring(start + length);
					SetTextInternal(newText);
				}
			}

	// Get the length of the selection.
	abstract internal int GetSelectionLength();

	// Set the length of the selection.
	abstract internal void SetSelectionLength(int value);

	// Get the start of the selection.
	abstract internal int GetSelectionStart();

	// Set the start of the selection.
	abstract internal void SetSelectionStart(int value);

	// Change the text selection
	abstract internal void SelectInternal( int start, int length);

	abstract protected void SetTextInternal( string text);

	// Make sure the caret is visible
	abstract protected void ScrollToCaretInternal();

#if !CONFIG_COMPACT_FORMS

	// Process a message.
	protected override void WndProc(ref Message m)
			{
				base.WndProc(ref m);
			}

#endif // !CONFIG_COMPACT_FORMS

}; // class TextBoxBase

}; // namespace System.Windows.Forms
