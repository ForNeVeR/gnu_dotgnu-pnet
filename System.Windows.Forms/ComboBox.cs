/*
 * ComboBox.cs - Implementation of the "System.Windows.Forms.ComboBox" class.
 *
 * Copyright (C) 2003  Neil Cawse.
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2003  Free Software Foundation, Inc.
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

namespace System.Windows.Forms
{
	using System;
	using System.Collections;
	using System.ComponentModel;
	using System.Drawing;
	using System.Text;

 
	public class ComboBox : ListControl
	{
		private DrawMode drawMode;
		private ComboBoxStyle dropDownStyle;
		private int dropDownWidth;
		private bool integralHeight;
		private int itemHeight;
		private ObjectCollection items;
		private int maxDropDownItems;
		private int preferredHeight = 0;
		private int selectedIndex;
		private bool sorted;
		private ButtonState buttonState;
		private TextBox textEntry;
		private PopupControl popup;
		private VScrollBar scrollbar;
		private bool droppedDown;
		private int updateCount;
		private int popupMouseItem;
		private int popupDrawWidth;
		private int actualItemHeight;
		private int[] itemHeights;
		private int totalItemHeight;
 	
		public ComboBox()
		{
			textEntry = new TextBox();
			BackColor = SystemColors.Window;
			BorderStyleInternal = BorderStyle.Fixed3D;
			dropDownStyle = ComboBoxStyle.DropDown;
			integralHeight = true;
			maxDropDownItems = 8;
			dropDownWidth = Width;
			updateCount = 0;
			itemHeight = FontHeight + 2;
			selectedIndex = -1;
			SetStyle(ControlStyles.ResizeRedraw, true);
			textEntry.BorderStyle = BorderStyle.None;
			this.Controls.Add(textEntry);
		
			popup = new PopupControl();
			popup.BorderStyleInternal = BorderStyle.FixedSingle;
			popup.BackColor = SystemColors.Window;
			popup.PopDown += new EventHandler(popup_PopDown);
			popup.Paint +=new PaintEventHandler(popup_Paint);
			popup.MouseDown+=new MouseEventHandler(popup_MouseDown);
			popup.MouseMove+=new MouseEventHandler(popup_MouseMove);
			popup.MouseUp+=new MouseEventHandler(popup_MouseUp);

			scrollbar = new VScrollBar();
			scrollbar.Dock = DockStyle.Right;
			scrollbar.BackColor = SystemColors.Control;
			scrollbar.ValueChanged+=new EventHandler(scrollbar_ValueChanged);
			popup.Controls.Add(scrollbar);

		}

		protected override Size DefaultSize
		{
			get
			{
				return new Size(121, PreferredHeight);
			}
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			Draw(e.Graphics);
		}

		private void Draw()
		{
			using (Graphics g = CreateGraphics())
				Draw(g);
		}

		private void Draw(Graphics g)
		{
			// TODO: fix this
			// Fill in the piece between the textbox and the bottom
			// The textbox could autosize to a smaller size than we need, so we need to fill this in.
			using (Brush b = Enabled ? new SolidBrush(BackColor) : SystemBrushes.Control)
				g.FillRectangle(b, 0, ClientSize.Height - 2, ClientSize.Width, 2);
			DrawButton(g);
			if (DropDownStyle == ComboBoxStyle.DropDownList && selectedIndex != -1)
			{
				Rectangle bounds = new Rectangle(textEntry.Left, textEntry.Top, textEntry.Width - 1, textEntry.Height -1);
				g.FillRectangle(SystemBrushes.Highlight, bounds);
				// HACK bug in DrawString
				g.SetClip(bounds);
				g.DrawString(Text, Font, SystemBrushes.HighlightText, bounds);
			}
		}

		private Size ButtonSize
		{
			get
			{
				return new Size(17, 17);
			}
		}

		private void DrawButton(Graphics g)
		{
			ControlPaint.DrawComboButton(g, ClientSize.Width - ButtonSize.Width, 0, ButtonSize.Width, ButtonSize.Height, buttonState);
		}

		public DrawMode DrawMode 
		{
			get
			{
				return drawMode;
			}
			set
			{
				drawMode = value;
				GetItemHeights();
				Draw();
			}
		}

		public ComboBoxStyle DropDownStyle 
		{
			get
			{
				return dropDownStyle;
			}
			set
			{
				dropDownStyle = value;
				textEntry.Visible = (dropDownStyle != ComboBoxStyle.DropDownList);
				Draw();
				OnDropDownStyleChanged(EventArgs.Empty);
			}
		}

		public int DropDownWidth 
		{
			get
			{
				return dropDownWidth;
			}
			set 
			{
				if (value < 1)
					throw new ArgumentException();
				if (value < Width)
					dropDownWidth = Width;
				else
					dropDownWidth = value;
				Draw();
			}
		}

		public bool DroppedDown
		{
			get
			{
				return droppedDown;
			}
			set
			{
				if (value == droppedDown)
					return;
				droppedDown = value;
				if (value)
				{
					//TODO: not working
					textEntry.SelectAll();
					actualItemHeight = itemHeight;
					if (drawMode == DrawMode.Normal)
						actualItemHeight = Font.Height + 2;

					buttonState = ButtonState.Pushed;
					using (Graphics g = CreateGraphics())
						DrawButton(g);
					scrollbar.Maximum = items.Count - 1;
					scrollbar.LargeChange = maxDropDownItems;
					scrollbar.Value = (selectedIndex==-1) ? 0 :  selectedIndex;
					if (items.Count < MaxDropDownItems)
						popup.Height = items.Count * actualItemHeight;
					else
						popup.Height = maxDropDownItems * actualItemHeight;
					popupMouseItem = selectedIndex;
					popup.Width = DropDownWidth;
					popupDrawWidth = popup.ClientSize.Width - scrollbar.Width;
					popup.Location = Parent.PointToScreen(new Point(Left, Bottom));
					popup.Visible = true;
				}
				else
				{
					buttonState = ButtonState.Normal;
					using (Graphics g = CreateGraphics())
						DrawButton(g);
					popup.Visible = false;
				}
			}
		}

		protected override void OnEnabledChanged(EventArgs e)
		{
			textEntry.Enabled = Enabled;
			if (Enabled)
				buttonState = ButtonState.Normal;
			else
			{
				buttonState = ButtonState.Inactive;
				//Text = "";
			}
			base.OnEnabledChanged(e);
		}

		public override bool Focused 
		{
			get
			{
				return base.Focused;
			}
		}

		public bool IntegralHeight 
		{
			get
			{
				return integralHeight;
			}
			set
			{
				integralHeight = value;
				Draw();
			}
		}

		public int ItemHeight 
		{
			get
			{
				return itemHeight;
			}
			set
			{
				if (value == itemHeight)
					return;
				itemHeight = value;
				GetItemHeights();
				Draw();
			}
		}

		private void GetItemHeights()
		{
			if (this.DrawMode == DrawMode.OwnerDrawVariable)
			{
				itemHeights = new int[Items.Count];
				using (Graphics g = CreateGraphics())
				{
					for (int i = 0; i < Items.Count; i++)
					{
						MeasureItemEventArgs args = new MeasureItemEventArgs(g, i, itemHeight);
						this.OnMeasureItem(args);
						itemHeights[i] = args.ItemHeight;
						totalItemHeight += args.ItemHeight;
					}
				}
			}
			else
				itemHeights = null;
		}

		public ObjectCollection Items 
		{
			get
			{
				if (items == null)
					items = new ObjectCollection(this);
				return items;
			}
		}

		public int MaxDropDownItems 
		{
			get { return maxDropDownItems; }
			set { maxDropDownItems = value; }
		}

		public int MaxLength 
		{
			get
			{
				return textEntry.MaxLength;
			}
			set
			{
				textEntry.MaxLength = value;
			}
		}

		public int PreferredHeight 
		{
			get
			{
				if (preferredHeight == 0)
					preferredHeight = FontHeight + SystemInformation.BorderSize.Height * 4 + 3;
				return preferredHeight;
			}
		}

		public override int SelectedIndex 
		{
			get
			{
				return selectedIndex;
			}
			set
			{
				int count = 0;
				if (Items != null)
					count = Items.Count;
				if (value < -1 || value >= count)
					throw new ArgumentOutOfRangeException();
				selectedIndex = value;
				SetText();
				if (IsHandleCreated)
					base.OnTextChanged(EventArgs.Empty);
				OnSelectedItemChanged(EventArgs.Empty);
				base.OnSelectedIndexChanged(EventArgs.Empty);
			}
		}

		// When the selection changes, change the textbox text.
		private void SetText()
		{
			if (selectedIndex == -1)
				textEntry.Text = "";
			else
				textEntry.Text = GetItemText(Items[selectedIndex]);
		}

		public object SelectedItem 
		{
			get
			{
				return Items[selectedIndex];
			}
			set
			{
				SelectedIndex = Items.IndexOf(value);
			}
		}

		public string SelectedText 
		{
			get
			{
				return textEntry.SelectedText;
			}
			set 
			{
				if (DropDownStyle == ComboBoxStyle.Simple)
					return;
				textEntry.SelectedText = value; 
			}
		}

		public int SelectionLength 
		{
			get
			{
				return textEntry.SelectionLength;
			}
			set
			{
				textEntry.SelectionLength = value;
			}
		}

		public int SelectionStart
		{
			get
			{
				return textEntry.SelectionStart;
			}
			set
			{
				textEntry.SelectionStart = value;
			}
		}

		public bool Sorted 
		{
			get
			{
				return sorted;
			}
			set
			{
				if (sorted == value)
					return;
				sorted = value;
				Items.Sort();
				SelectedIndex = -1;
			}
		}

		public override string Text 
		{
			get
			{
				return textEntry.Text;
			}
			set
			{
				textEntry.Text = value;
				if (value == null)
				{
					SelectedIndex = -1;
					return; 
				}
				
				for (int i = 0; i < Items.Count; i++)
				{
					if (string.Compare(value, items[i].ToString()) == 0)
					{
						SelectedIndex = i;
						return; 
					}

				}
			}
		}

		public void BeginUpdate()
		{
			updateCount++;
		}

		public void EndUpdate()
		{
			updateCount--;
			if (updateCount <= 0)
				Draw();
		}

		public int GetItemHeight(int index)
		{
			return ItemHeight;
		}

		public void Select(int start, int length)
		{
			textEntry.Select(start, length);
		}

		public void SelectAll()
		{
			textEntry.SelectAll();
		}

		protected override CreateParams CreateParams 
		{
			get
			{
				return base.CreateParams;
			}
		}

		protected virtual void AddItemsCore(object[] value)
		{
			if (value == null | value.Length == 0)
				return;
			Items.AddRange(value);
		}

		protected override bool IsInputKey(Keys keyData)
		{
			Keys key = keyData & (Keys.Alt | Keys.KeyCode);
			if (this.DroppedDown && (key == Keys.Enter || key == Keys.Right))
				return true; 
			return base.IsInputKey(keyData);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown(e);
			if (e.X >= Width - ButtonSize.Width | DropDownStyle == ComboBoxStyle.DropDownList)
				DroppedDown = !DroppedDown;
		}

		private void popup_PopDown(object sender, EventArgs e)
		{
			DroppedDown = false;
		}

		[TODO]
		protected override void OnDataSourceChanged(EventArgs e)
		{
			base.OnDataSourceChanged(e);
		}

		[TODO]
		protected override void OnDisplayMemberChanged(EventArgs e)
		{
			base.OnDisplayMemberChanged(e);
		}

		protected virtual void OnDrawItem(DrawItemEventArgs e)
		{
			// Invoke the event handler.
			DrawItemEventHandler handler = (DrawItemEventHandler)(GetHandler(EventId.DrawItem));
			if(handler != null)
				handler(this, e);
		}

		protected virtual void OnDropDown(EventArgs e)
		{
			// Invoke the event handler.
			EventHandler handler = (EventHandler)(GetHandler(EventId.DropDown));
			if(handler != null)
				handler(this, e);
		}

		protected virtual void OnDropDownStyleChanged(EventArgs e)
		{
			// Invoke the event handler.
			EventHandler handler = (EventHandler)(GetHandler(EventId.DropDownStyleChanged));
			if(handler != null)
				handler(this, e);
		}

		protected virtual void OnMeasureItem(MeasureItemEventArgs e)
		{
			// Invoke the event handler.
			MeasureItemEventHandler handler = (MeasureItemEventHandler)(GetHandler(EventId.MeasureItem));
			if(handler != null)
				handler(this, e);
		}

		protected override void OnParentBackColorChanged(EventArgs e)
		{
			base.OnParentBackColorChanged(e);
		}

		protected override void OnBackColorChanged(EventArgs e)
		{
			base.OnBackColorChanged (e);
			Draw();
		}

		protected override void OnSelectedIndexChanged(EventArgs e)
		{
			base.OnSelectedIndexChanged(e);
			// Invoke the event handler.
			EventHandler handler = (EventHandler)(GetHandler(EventId.SelectedIndexChanged));
			if(handler != null)
				handler(this, e);
		}

		protected virtual void OnSelectedItemChanged(EventArgs e)
		{
			// Invoke the event handler.
			EventHandler handler = (EventHandler)(GetHandler(EventId.SelectedItemChanged));
			if(handler != null)
				handler(this, e);
		}

		protected override void OnSelectedValueChanged(EventArgs e)
		{
			base.OnSelectedValueChanged(e);
		}

		protected override void OnResize(EventArgs e)
		{
			base.OnResize (e);
		}

		// This event is raised when a new item is selected and that change to that item is completed.
		protected virtual void OnSelectionChangeCommitted(EventArgs e)
		{
			// Invoke the event handler.
			EventHandler handler = (EventHandler)(GetHandler(EventId.SelectionChangeCommitted));
			if(handler != null)
				handler(this, e);
		}

		protected override void OnFontChanged(EventArgs e)
		{
			base.OnFontChanged (e);
			GetItemHeights();
			Draw();
		}

		protected override void OnForeColorChanged(EventArgs e)
		{
			base.OnForeColorChanged (e);
		}

		protected override void OnHandleCreated(EventArgs e)
		{
			base.OnHandleCreated (e);
		}

		protected override void OnHandleDestroyed(EventArgs e)
		{
			base.OnHandleDestroyed (e);
		}

		[TODO]
		protected override void RefreshItem(int index)
		{
		}

		protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
		{
			if (DropDownStyle == ComboBoxStyle.DropDown || DropDownStyle == ComboBoxStyle.DropDownList)
				height = PreferredHeight;
			base.SetBoundsCore(x, y, width, height, specified);	
			textEntry.Size = new Size(ClientSize.Width - ButtonSize.Width, ClientSize.Height + 5);
		}

		protected override void SetItemsCore(IList list)
		{
			Items.Clear();
			Items.AddRange(list);
		}

		protected override void SetItemCore(int index, object value)
		{
			Items[index] = value;
		}

		public override Color BackColor
		{
			get
			{
				return base.BackColor;
			}
			set
			{
				base.BackColor = value;
				textEntry.backColor = value;
			}
		}

		public override Color ForeColor
		{
			get
			{
				return base.ForeColor;
			}
			set
			{
				base.ForeColor = value;
			}
		}

		public int FindString(string s)
		{
			return FindStringInternal(s, Items, -1, false); 
		} 

		public int FindString(string s, int startIndex)
		{
			return FindStringInternal(s, Items, startIndex, false); 
		}

		public int FindStringExact(string s)
		{
			return FindStringInternal(s, Items, -1, true);
		} 

		public int FindStringExact(string s, int startIndex)
		{
			return FindStringInternal(s, Items, startIndex, true); 
		} 

		public event DrawItemEventHandler DrawItem
		{
			add
			{
				AddHandler(EventId.DrawItem, value);
			}
			remove
			{
				RemoveHandler(EventId.DrawItem, value);
			}
		}

		public event EventHandler DropDown
		{
			add
			{
				AddHandler(EventId.DropDown, value);
			}
			remove
			{
				RemoveHandler(EventId.DropDown, value);
			}
		}
		public event EventHandler DropDownStyleChanged
		{
			add
			{
				AddHandler(EventId.DropDownStyleChanged, value);
			}
			remove
			{
				RemoveHandler(EventId.DropDownStyleChanged, value);
			}
		}

		public event MeasureItemEventHandler MeasureItem
		{
			add
			{
				AddHandler(EventId.MeasureItem, value);
				GetItemHeights();
			}
			remove
			{
				RemoveHandler(EventId.MeasureItem, value);
				GetItemHeights();
			}
		}

		public event EventHandler SelectionChangeCommitted
		{
			add
			{
				AddHandler(EventId.SelectionChangeCommitted, value);
			}
			remove
			{
				RemoveHandler(EventId.SelectionChangeCommitted, value);
			}
		}

		public override string ToString()
		{
			return base.ToString () + ", Items.Count: " + ((Items == null) ? 0 : Items.Count).ToString();
		}

		public override Image BackgroundImage
		{
			get
			{
				return base.BackgroundImage;
			}
			set
			{
				base.BackgroundImage = value;
			}
		}


		private void popup_Paint(Object sender, PaintEventArgs e)
		{
			PopupDraw(e.Graphics, -1);
		}

		// Draw the items in the popup. Pos is the item to draw or -1 for all
		private void PopupDraw(Graphics g, int pos)
		{
			using (Brush foreBrush = new SolidBrush(ForeColor), 
					   backBrush = new SolidBrush(BackColor))
			{
				int y = 0;
				g.SetClip(new Rectangle(0, 0, popupDrawWidth, popup.Height));
				for (int i = scrollbar.Value; i < items.Count; i++)
				{
					if (pos == -1 | i == pos)
					{
						Rectangle layout = new Rectangle(0, y, popupDrawWidth, actualItemHeight);
						if (DrawMode == DrawMode.Normal)
						{
							if (popupMouseItem == i)
							{
								g.FillRectangle(SystemBrushes.Highlight, layout); 
								g.DrawString(items[i].ToString(), Font, SystemBrushes.HighlightText, 0, y);
							}
							else
							{
								g.FillRectangle(backBrush, layout); 
								g.DrawString(items[i].ToString(), Font, foreBrush, 0, y);
							}
						}
						else
						{
							if (DrawMode == DrawMode.OwnerDrawVariable)
								layout = new Rectangle(0, y, popupDrawWidth, itemHeights[i]);
							DrawItemState state = DrawItemState.NoAccelerator | DrawItemState.NoFocusRect;
							if (popupMouseItem == i)
								state |= DrawItemState.Selected;
							DrawItemEventArgs drawItemEventArgs = new DrawItemEventArgs(g, Font, layout, i, state);
							OnDrawItem(drawItemEventArgs);
						}
					}
					if (DrawMode == DrawMode.OwnerDrawVariable)
						y += itemHeights[i];
					else
						y += actualItemHeight;
					if (y > popup.Height)
						break;
				}
			}
		}
		
		private void scrollbar_ValueChanged(object sender, EventArgs e)
		{
			using (Graphics g = popup.CreateGraphics())
				PopupDraw(g, -1);
		}

		private void popup_MouseDown(Object sender, MouseEventArgs e)
		{
			ChangeHighlighting(e.X, e.Y);
		}

		private void popup_MouseUp(Object sender, MouseEventArgs e)
		{
			if (e.Y > 0 & e.X < popupDrawWidth)
			{
				SelectedIndex = ItemFromY(e.Y);
				DroppedDown = false;
				if (DropDownStyle == ComboBoxStyle.DropDownList)
					Draw();
				else
				{
					textEntry.Focus();
					SelectAll();
				}
			}
		}

		private void popup_MouseMove(Object sender, MouseEventArgs e)
		{
			ChangeHighlighting(e.X, e.Y);
		}

		private void ChangeHighlighting(int x, int y)
		{
			if (x > popupDrawWidth || y < 0)
				return;
			int newItem = ItemFromY(y);
			if (newItem != popupMouseItem)
			{
				int oldItem = popupMouseItem;
				popupMouseItem = newItem;
				using (Graphics g = popup.CreateGraphics())
				{
					// Clear the highlighting on the old.
					PopupDraw(g, oldItem);
					// Highlight the new.
					PopupDraw(g, popupMouseItem);
				}
			}
		}

		private int ItemFromY(int y)
		{
			if (DrawMode != DrawMode.OwnerDrawVariable)
				return y / actualItemHeight + scrollbar.Value;
			int itemY = 0;
			for (int i = scrollbar.Value; i < items.Count; i++)
			{
				itemY += itemHeights[i];
				if (y < itemY)
					return i;
			}
			return -1;
		}


#if CONFIG_COMPACT_FORMS
 	// Process a message.
 	protected override void WndProc(ref Message m)
 	{
 		base.WndProc(ref m);
 	}
#endif // CONFIG_COMPACT_FORMS

		public class ObjectCollection: IList, ICollection, IEnumerable
		{
			private ComboBox owner;
			private ArrayList list;

			public ObjectCollection(ComboBox owner)
			{
				this.owner = owner;
			}

			private IComparer comparer;
			private IComparer Comparer
			{
				get
				{
					if (comparer == null)
						comparer = new ItemComparer(owner);
					return comparer;
				}
			}

			private ArrayList List
			{
				get
				{
					if (list == null)
						list = new ArrayList();
					return list;
				}
			}

			public virtual int Count
			{
				get
				{
					return List.Count;
				}
			}

			public virtual bool IsReadOnly
			{
				get
				{
					return false;
				}
			}

			public virtual object this[int index]
			{
				get
				{
					return List[index];
				}

				set
				{
					List[index] = value;
				}
			}

			object ICollection.SyncRoot
			{
				get
				{
					return this;
				}
			}

			bool ICollection.IsSynchronized
			{
				get
				{
					return false;
				}
			}

			bool IList.IsFixedSize
			{
				get
				{
					return false;
				}
			}

			public int Add(object item)
			{
				int i = List.Add(item);
				if (owner.sorted)
				{
					List.Sort(Comparer);
					i = List.IndexOf(item);
				}
				return i;
			}

			int IList.Add(object item)
			{
				return Add(item);
			}

			public void AddRange(object[] items)
			{
				AddRange(items as IList);
			}

			internal void AddRange(IList items)
			{
				List.AddRange(items);
				if (owner.sorted)
					List.Sort(Comparer);
			}

			public virtual void Clear()
			{
				owner.selectedIndex = -1;
				List.Clear();
			}

			public virtual bool Contains(object value)
			{
				return IndexOf(value) != -1;
			}

			public void CopyTo(object[] dest, int arrayIndex)
			{
				List.CopyTo(dest, arrayIndex);
			}

			void ICollection.CopyTo(Array dest, int index)
			{
				List.CopyTo(dest, index);
			}

			public virtual IEnumerator GetEnumerator()
			{
				return List.GetEnumerator();
			}

			public virtual int IndexOf(object value)
			{
				return List.IndexOf(value);
			}

			public virtual void Insert(int index, object item)
			{
				if (owner.sorted)
					Add(item);
				else
					List.Insert(index, item);
			}

			public virtual void RemoveAt(int index)
			{
				List.RemoveAt(index);
				if (index < owner.selectedIndex)
					owner.selectedIndex--;
			}

			public virtual void Remove(object value)
			{
				int i = List.IndexOf(value);
				if (i != -1)
					RemoveAt(i);
			}

			internal void Sort()
			{
				List.Sort(Comparer);
			}

			private class ItemComparer: IComparer
			{
				private ComboBox owner;

				public ItemComparer(ComboBox owner)
				{
					this.owner = owner;
				}

				public virtual int Compare(object item1, object item2)
				{
					if (item1 == null)
					{
						if (item2 == null)
							return 0;
						else
							return -1;
					}
					if (item2 == null)
						return 1;
					string s1 = owner.GetItemText(item1);
					string s2 = owner.GetItemText(item2);
					return Application.CurrentCulture.CompareInfo.Compare(s1, s2);
				}
			}
		}
	}; // class ComboBox

}; // namespace System.Windows.Forms
