/*
 * ComboBox.cs - Implementation of the "System.Windows.Forms.ComboBox" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2003  Free Software Foundation, Inc.
 * 
 * Contributions from Simon Guindon, Neil Cawse
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
 
public class ComboBox : ListControl
{
 	public event DrawItemEventHandler DrawItem;
 	public event EventHandler DropDown;
 	public event EventHandler DropDownStyleChanged;
 	public event MeasureItemEventHandler MeasureItem;
 	public event EventHandler SelectionChangeCommitted;

 	private DrawMode drawMode;
 	private ComboBoxStyle dropDownStyle;
 	private int dropDownWidth;
 	private bool integralHeight;
 	private int itemHeight;
 	private ObjectCollection items;
 	private int maxDropDownItems;
 	private int maxLength;
 	private int preferredHeight = 0;
 	private int selectedIndex;
 	private bool sorted;
 	private ButtonState buttonState;
 	private TextBox txtEntry;
 	private PopupControl popup;
 	private VScrollBar scrollbar;
 	
 	public ComboBox()
 	{
 		items = new ObjectCollection(this);

		BorderStyleInternal = BorderStyle.Fixed3D;
		txtEntry = new TextBox();
		txtEntry.BorderStyle = BorderStyle.None;
		this.Controls.Add(txtEntry);
		
		popup = new PopupControl();
		popup.BorderStyleInternal = BorderStyle.FixedSingle;
		popup.BackColor = SystemColors.Window;
		popup.PopDown += new EventHandler(popup_PopDown);

		scrollbar = new VScrollBar();
		scrollbar.Dock = DockStyle.Right;
		scrollbar.BackColor = SystemColors.Control;
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
		DrawCombo(e.Graphics);
 	}

	protected void DrawCombo(Graphics g)
	{
		// Fill in the piece between the textbox and the bottom
		// The textbox could autosize to a smaller size than we need, so we need to fill this in.
		using (Brush b = Enabled ? SystemBrushes.Window : SystemBrushes.Control)
			g.FillRectangle(b, 0, ClientSize.Height - 2, ClientSize.Width, 2);
		DrawButton(g);
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
 		get { return drawMode; }
 		set { drawMode = value; }
 	}

 	public ComboBoxStyle DropDownStyle 
 	{
 		get { return dropDownStyle; }
 		set { dropDownStyle = value; }
 	}

 	public int DropDownWidth 
 	{
 		get { return dropDownWidth; }
 		set { dropDownWidth = value; }
 	}

 	public bool DroppedDown
 	{
 		get { return buttonState == ButtonState.Pushed; }
 		set
		{
			if (value == DroppedDown)
				return;
			if (value)
			{
				buttonState = ButtonState.Pushed;
				using (Graphics g = CreateGraphics())
					DrawButton(g);
				popup.Height = 100;
				// TODO calculate height from items
				popup.Width = Width;
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
		txtEntry.Enabled = Enabled;
 		if (Enabled)
 			buttonState = ButtonState.Normal;
 		else
 		{
 			buttonState = ButtonState.Inactive;
 			Text = "";
 		}
 		base.OnEnabledChanged(e);
 	}

 	public override bool Focused 
 	{
 		get { return base.Focused; }
 	}

 	public bool IntegralHeight 
 	{
 		get { return integralHeight; }
 		set { integralHeight = value; }
 	}

 	public int ItemHeight 
 	{
 		get { return itemHeight; }
 		set { itemHeight = value; }
 	}

 	public ObjectCollection Items 
 	{
 		get { return items; }
 	}

 	public int MaxDropDownItems 
 	{
 		get { return maxDropDownItems; }
 		set { maxDropDownItems = value; }
 	}

 	public int MaxLength 
 	{
 		get { return maxLength; }
 		set { maxLength = value; }
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
 		get { return selectedIndex; }
 		set { selectedIndex = value; }
 	}

 	public object SelectedItem 
 	{
 		get { return base.SelectedValue; }
 		set { base.SelectedValue = value; }
 	}

 	public string SelectedText 
 	{
 		get
		{
			return txtEntry.SelectedText;
		}
 		set 
 		{
 			txtEntry.SelectedText = value; 
 		}
 	}

 	public int SelectionLength 
 	{
 		get { return txtEntry.SelectionLength; }
 		set { txtEntry.SelectionLength = value; }
 	}

 	public int SelectionStart
 	{
 		get { return txtEntry.SelectionStart; }
 		set { txtEntry.SelectionStart = value; }
 	}

 	public bool Sorted 
 	{
 		get { return sorted; }
 		set { sorted = value; }
 	}

 	public override string Text 
 	{
 		get
		{
			return txtEntry.Text;
		}
 		set
		{
			txtEntry.Text = value;
		}
 	}

 	[TODO]
 	public void BeginUpdate()
 	{
 		throw new NotImplementedException("BeginUpdate");
 	}

 	[TODO]
 	public void EndUpdate()
 	{
 		throw new NotImplementedException("EndUpdate");
 	}


 	[TODO]
 	public int GetItemHeight(int index)
 	{
 		throw new NotImplementedException("GetItemHeight");
 	}

 	[TODO]
 	public void Select(int start, int length)
 	{
 		throw new NotImplementedException("Select");
 	}

 	[TODO]
 	public new void Select()
 	{
 		throw new NotImplementedException("Select");
 	}

 	[TODO]
 	protected new virtual void Select(bool directed, bool forward)
 	{
 		throw new NotImplementedException("Select");
 	}

 	[TODO]
 	public void SelectAll()
 	{
 		throw new NotImplementedException("SelectAll");
 	}

 	protected override CreateParams CreateParams 
 	{
 		get { return base.CreateParams; }
 	}

 	protected virtual void AddItemsCore(object[] value)
 	{
 	}

 	protected override bool IsInputKey(Keys keyData)
 	{
 		return base.IsInputKey(keyData);
 	}

 	protected override void OnMouseDown(MouseEventArgs e)
 	{
 		if (e.X >= Width - ButtonSize.Width)
 			DroppedDown = !DroppedDown;
 		base.OnMouseDown(e);
 	}

	private void popup_PopDown(object sender, EventArgs e)
	{
		DroppedDown = false;
	}

 	protected override void OnDataSourceChanged(EventArgs e)
 	{
 		base.OnDataSourceChanged(e);
 	}

 	protected override void OnDisplayMemberChanged(EventArgs e)
 	{
 		base.OnDisplayMemberChanged(e);
 	}

 	protected virtual void OnDrawItem(DrawItemEventArgs e)
 	{
 		if (DrawItem != null)
 		{
 			DrawItem(this, e);
 		}
 	}

 	protected virtual void OnDropDown(EventArgs e)
 	{
 		if (DropDown != null)
 		{
 			DropDown(this, e);
 		}
 	}

 	protected virtual void OnDropDownStyleChanged(EventArgs e)
 	{
 		if (DropDownStyleChanged != null)
 		{
 			DropDownStyleChanged(this, e);
 		}
 	}

 	protected virtual void OnMeasureItem(MeasureItemEventArgs e)
 	{
 		if (MeasureItem != null)
 		{
 			MeasureItem(this, e);
 		}
 	}

 	protected override void OnParentBackColorChanged(EventArgs e)
 	{
 		base.OnParentBackColorChanged(e);
 	}

 	protected override void OnSelectedIndexChanged(EventArgs e)
 	{
 		base.OnSelectedIndexChanged(e);
 	}

 	protected override void OnSelectedValueChanged(EventArgs e)
 	{
 		base.OnSelectedValueChanged(e);
 	}

 	protected virtual void OnSelectionChangeCommitted(EventArgs e)
 	{
 		if (SelectionChangeCommitted != null)
 		{
 			SelectionChangeCommitted(this, e);
 		}
 	}

 	protected override void RefreshItem(int item)
 	{
 	}

 	protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
 	{
		if (DropDownStyle == ComboBoxStyle.DropDown || DropDownStyle == ComboBoxStyle.DropDownList)
			height = PreferredHeight;
		base.SetBoundsCore(x, y, width, height, specified);	
		txtEntry.Size = new Size(ClientSize.Width - ButtonSize.Width, ClientSize.Height + 5);
 	}

 	protected override void SetItemsCore(IList list)
 	{
 	}

 	#if CONFIG_COMPACT_FORMS
 	// Process a message.
 	protected override void WndProc(ref Message m)
 	{
 		base.WndProc(ref m);
 	}
 	#endif // CONFIG_COMPACT_FORMS

 	public class ObjectCollection : IList, ICollection, IEnumerable	
 	{
 		private ComboBox owner;
 		private ArrayList list;

 		public ObjectCollection(ComboBox owner)
 		{
 			this.owner = owner;
 			list = new ArrayList();
 		}

 		// Implement the ICollection interface.
 		public void CopyTo(Array array, int index)
 		{
 			List.CopyTo(array, index);
 		}

 		public virtual int Count
  		{
 			get { return List.Count; }
 		}
 	
 		bool ICollection.IsSynchronized
 		{
 			get { return false; }
  		}

 		Object ICollection.SyncRoot
  		{
 			get { return this; }
  		}

 		// Implement the IEnumerable interface.
 		public IEnumerator GetEnumerator()
  		{
 			return List.GetEnumerator();
 		}

 		// Determine if the collection is read-only.
 		public bool IsReadOnly
  		{
 			get { return false; }
  		}

 		bool IList.IsFixedSize
  		{
 			get { return false; }
  		}

 		// Get the array list that underlies this collection
 		Object IList.this[int index]
  		{
 			get { return List[index]; }
 			set { List[index] = value; }
  		}

 		Object this[int index]
  		{
 			get { return List[index]; }
 			set { List[index] = value; }
  		}

 		protected virtual ArrayList List
 		{
 			get { return list; }
 		}
 	
 		public int Add(Object value)
 		{
 			int result;
 			result =  List.Add(value);
 			owner.Invalidate();
 			return result;
 		}

 		public virtual void AddRange(Object[] items)
 		{
 			List.AddRange(items);
 			owner.Invalidate();
 		}

 		public virtual void Clear()
 		{
 			List.Clear();
 			owner.Invalidate();
 		}

 		public bool Contains(Object value)
 		{
 			return List.Contains(value);
 		}

 		public int IndexOf(Object value)
 		{
 			return List.IndexOf(value);
 		}

 		public void Insert(int index, Object value)
 		{
 			List.Insert(index, value);
 			owner.Invalidate();
 		}

 		public void Remove(Object value)
 		{
 			List.Remove(value);
 			owner.Invalidate();
 		}

 		public void RemoveAt(int index)
 		{
 			List.RemoveAt(index);
 		}

	}; // class ComboBox.ObjectCollection

}; // class ComboBox

}; // namespace System.Windows.Forms
