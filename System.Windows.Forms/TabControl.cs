/*
 * TabControl.cs - Implementation of the TabControl. Requires TabPage.cs.
 * 
 * Copyright (C) 2003  Neil Cawse.
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
 * 
 * The following are not complete:
 *	Proper alignment for bottom tabs
 *	Left and Right aligned tabs.
 *  Images in tabs.
 *  Disabled Control or disabled tabs.
 *	TabStyle of button or flat.
 *	Auto- repeat on the button when moving tabs left or right
 *  Tooltips
 *  Binding? Component?
 *  
 * At the moment drawing of the tabs is done by ControlPaint.DrawBorder3D.
 * This should be changed to allow better control over the placement of
 * the pixels.
 * Also all painting happens in the form paint event - if we painted
 * outside of this event, we could reduce flicker
 */

namespace System.Windows.Forms
{
	using System;
	using System.Drawing;
	using System.Collections;
	using d = System.Diagnostics.Debug;

	public class TabControl : ContainerControl
	{
		private TabAlignment alignment;
		private TabAppearance appearance;
		private TabDrawMode drawMode;
		private bool hotTrack;
		private ImageList imageList;
		private Size itemSize;
		private bool multiline;
		private Point padding;
		private int selectedIndex;
		private int prevSelectedIndex;
		private int hotTrackIndex;
		private bool showToolTips;
		private TabSizeMode sizeMode;
		private TabPageCollection tabPageCollection;
		private TabPositionInfo positionInfo;
		// Only if not multiline and too many tabs to display on a row
		private bool moveButtonsShowing;
		// Do the move buttons cover the last tab?
		private bool moveButtonsCovered;
		// The virtual start of the tabs if there are too many to show in one row
		private int moveButtonsTabOffset;
		// Tab move buttons when too many tabs on a row
		private Rectangle moveButtonLeftBounds;
		private ButtonState moveButtonLeftState;
		private Rectangle moveButtonRightBounds;
		private ButtonState moveButtonRightState;
		private Size moveButtonSize;

		// Margins for the text on a tab
		private const int tabTextWidthMargin = 5;
		private const int tabTextHeightMargin = 4;
		private const int minimumTabSize = 45;
		// Indent of the actual tabs from the side of the control
		private const int indent = 2;

		// Tab events
		public event DrawItemEventHandler DrawItem;
		public event EventHandler SelectedIndexChanged;
		public TabControl() : base()
		{
			selectedIndex = -1;

			alignment = TabAlignment.Top;
			drawMode = TabDrawMode.Normal;
			itemSize = new Size(42, 21);
			padding = new Point(6, 3);
			moveButtonSize = new Size (17, 17);
			sizeMode = TabSizeMode.Normal;
			appearance = TabAppearance.Normal;
			tabPageCollection = new TabPageCollection(this);
			SetStyle(ControlStyles.UserPaint, true);

			moveButtonLeftState = ButtonState.Normal;
			moveButtonRightState = ButtonState.Normal;
			hotTrackIndex = -1;
		}

		public Control GetControl(int index)
		{
			return tabPageCollection[index];
		}

		protected string GetToolTipText(object item){
			return (tabPageCollection[selectedIndex] as TabPage).ToolTipText;
		}

		protected override Size DefaultSize
		{
			get
			{
				return new Size(200, 100);
			}
		}

		protected virtual void OnSelectedIndexChanged(EventArgs e)
		{
			if (prevSelectedIndex > -1)
			{
				GetChildByIndex( prevSelectedIndex ).Visible = false;
			}
			GetChildByIndex( selectedIndex ).Visible = true;
			
			InvalidateTabs();
			if (SelectedIndexChanged != null)
				SelectedIndexChanged( this, EventArgs.Empty );
		}

		protected void RemoveAll()
		{
			tabPageCollection.Clear();
		}
		
		public TabAlignment Alignment
		{
			get
			{
				return alignment;
			}
			set
			{
				alignment = value;
				PerformLayout(this, "Alignment");
			}
		}

		public TabAppearance Appearance
		{
			get
			{
				return appearance;
			}
			set
			{
				appearance = value;
				PerformLayout(this, "Appearance");
			}
		}

		public TabDrawMode DrawMode
		{ 
			get
			{
				return drawMode;
			} 
			set
			{
				drawMode = value;
				PerformLayout(this, "DrawMode");
			}
		}

		public bool HotTrack
		{ 
			get
			{
				return hotTrack;
			}
			set
			{
				if (value != hotTrack)
				{
					hotTrackIndex = -1;
					hotTrack = value;
					InvalidateTabs();
				}
			}
		}

		public ImageList ImageList
		{
			get
			{
				return imageList;
			}
			set
			{
				imageList = value;
				InvalidateTabs();
			}
		}

		public Size ItemSize {
			get
			{
				return itemSize;
			}
			set
			{
				itemSize = value;
				PerformLayout(this, "ItemSize");
			}
		}

		public bool Multiline 
		{ 
			get
			{
				return multiline;
			}
			set
			{
				multiline = value;
				PerformLayout(this, "Multiline");
			}
		}

		public Point Padding
		{
			get
			{
				return padding;
			}
			set
			{
				padding = value;
				PerformLayout(this, "Padding");
			}
		}

		public int RowCount
		{
			get
			{
				return PositionInfo.totalRows;
			}
		}

		public TabPage SelectedTab 
		{
			get
			{
				return tabPageCollection[selectedIndex] as TabPage;
			}
			set
			{
				SelectedIndex = tabPageCollection.GetChildIndex(value);
			}
		}

		public bool ShowToolTips
		{
			get
			{
				return showToolTips;
			}
			set
			{
				showToolTips = value;
			}
		}

		public TabSizeMode SizeMode
		{
			get
			{
				return sizeMode;
			}
			set
			{
				sizeMode = value;
				PerformLayout(this, "SizeMode");
			}
		}

		public int TabCount {
			get
			{
				return tabPageCollection.Count;
			}
		}

		public TabPageCollection TabPages
		{
			get
			{
				return tabPageCollection;
			}
		}
		
		protected override Control.ControlCollection CreateControlsInstance()
		{
			return new TabPageCollection( this );
		}

		
		// Collection of child control TabPages.
		public class TabPageCollection : Control.ControlCollection
		{
			// Owning tab control.
			private TabControl tabOwner;

			public TabPageCollection(TabControl owner) : base(owner)
			{
				this.tabOwner = owner;
			}

			public override void Add(Control control)
			{
				control.Visible = false;
				base.Add(control);
			}

		};

		protected override void OnPaint(PaintEventArgs e)
		{
			Rectangle newClient = GetTabBaseBounds();
			// Draw the tab edging
			ControlPaint.DrawBorder3D(e.Graphics, newClient, Border3DStyle.RaisedInner, Border3DSide.Left);
			ControlPaint.DrawBorder3D(e.Graphics, newClient, Border3DStyle.Raised, Border3DSide.Bottom);
			ControlPaint.DrawBorder3D(e.Graphics, newClient, Border3DStyle.Raised, Border3DSide.Right);
			ControlPaint.DrawBorder3D(e.Graphics, newClient, Border3DStyle.RaisedInner, Border3DSide.Top);
			
			// Clear out the space behind the tabs
			using ( Brush background = new SolidBrush(BackColor) )
			{
				e.Graphics.FillRectangle( background, GetTabsBounds());
			}

			// Draw each tab in the tabControl row 0 first, bottom row last, selected item very last
			for( int row = 0; row < PositionInfo.totalRows; row++ )
			{
				for( int i = 0; i < tabPageCollection.Count; i++ )
				{
					if (row == PositionInfo.positions[i].row && i != selectedIndex)
						OnDrawItem( new DrawItemEventArgs(e.Graphics, Font, GetTabRect(i), i, DrawItemState.None, ForeColor, BackColor));
				}
			}
			if (selectedIndex > -1)
			{
				DrawItemState state = Focused?DrawItemState.Focus:DrawItemState.None;
				OnDrawItem( new DrawItemEventArgs(e.Graphics, Font, GetTabRect(selectedIndex), selectedIndex, state, ForeColor, BackColor));
			}

			// Draw the Tab move buttons if necessary
			if (moveButtonsShowing)
			{
				moveButtonLeftBounds = moveButtonRightBounds = new Rectangle( Width - moveButtonSize.Width *2, 2, moveButtonSize.Width, moveButtonSize.Height );
				moveButtonRightBounds.Offset( moveButtonLeftBounds.Width, 0);

				using (SolidBrush b = new SolidBrush( foreColor) )
				{
					ControlPaint.DrawButton(e.Graphics, moveButtonLeftBounds, moveButtonLeftState);
					// Left Arrow
					e.Graphics.FillPolygon(b, new Point[]
					{
						new Point(moveButtonLeftBounds.Left + 5, moveButtonLeftBounds.Top + 8),
						new Point(moveButtonLeftBounds.Left + 8, moveButtonLeftBounds.Top + 5),
						new Point(moveButtonLeftBounds.Left + 8, moveButtonLeftBounds.Top + 11)
					});
					ControlPaint.DrawButton(e.Graphics, moveButtonRightBounds, moveButtonRightState);
					// Right Arrow
					e.Graphics.FillPolygon(b, new Point[]
					{
						new Point(moveButtonRightBounds.Left + 11, moveButtonRightBounds.Top + 8),
						new Point(moveButtonRightBounds.Left + 8, moveButtonRightBounds.Top + 5),
						new Point(moveButtonRightBounds.Left + 8, moveButtonRightBounds.Top + 11)
					});
					
				}
			}
			// Draw the visible TabPage (child controls)
			base.OnPaint (e);
		}

		public Rectangle GetTabRect( int index )
		{
			return PositionInfo.positions[index].bounds;
		}
			
		// Get the number of rows and the positions of each tab
		// This property is used everywhere for tab position and size info
		private TabPositionInfo PositionInfo
		{
			get
			{
				if (positionInfo == null)
				{
					if (tabPageCollection.Count == 0)
						positionInfo = TabPositionInfo.Empty;
					else
					{
						TabPosition[] tabs = new TabPosition[tabPageCollection.Count];

						// Find the width of a row
						int rowWidth;
						if (alignment == TabAlignment.Top || alignment == TabAlignment.Bottom)
							rowWidth = Bounds.Width - indent * 2;
						else
							rowWidth = Bounds.Height - indent * 2;
		
						int maxRow = 0;
						// Size the tabs based on the TabSizeMode
						using ( Graphics graphics = CreateGraphics() )
						{
							if (sizeMode == TabSizeMode.Normal || (sizeMode == TabSizeMode.FillToRight && !multiline ))
								SizeTabsNormal( ref tabs, graphics, rowWidth, out maxRow, false );
							else if (sizeMode == TabSizeMode.Fixed)
								SizeTabsNormal ( ref tabs, graphics, rowWidth, out maxRow, true );
							else if (sizeMode == TabSizeMode.FillToRight)
								SizeTabsFillRight ( ref tabs, graphics, rowWidth, out maxRow );
						}
						// Do we need to move the row that was selected to the bottom of the tabs?
						if (selectedIndex > -1)
						{
							// Find the position of the first tab on the last row
							int last = tabs.Length - 1;
							for(; last > 0 ; last-- )
							{
								if (tabs[last - 1].row != maxRow)
									break;
							}
							// Check to see if we have selected a tab that isnt on the last row and move the tab row down
							if (selectedIndex < last)
								RowToBottom(ref tabs, tabs[selectedIndex].row, maxRow);
						}

						//Find the actual bounds
						LayoutTabBounds( ref tabs, rowWidth );
						positionInfo = new TabPositionInfo(maxRow + 1, tabs);
					}
				}
				return positionInfo;
			}
		}

		// Size the tabs for SizeMode normal or fixed
		private void SizeTabsNormal(ref TabPosition[] tabs, Graphics g, int rowWidth, out int row, bool fixedSize)
		{
			int pos = 0;
			row = 0;
			
			for( int i=0;i < tabs.Length; i++)
			{
				if ((tabPageCollection[i] as TabPage).Visible)
				{
					int measuredWidth;
					if (fixedSize)
						measuredWidth = itemSize.Width;
					else
						measuredWidth = (int)g.MeasureString(tabPageCollection[i].Text, Font).Width + tabTextWidthMargin * 2;
					if (measuredWidth < minimumTabSize)
						measuredWidth = minimumTabSize;
					// Are we out of room on that row?
					if (pos + measuredWidth > rowWidth && multiline)
					{
						row++;
						pos = 0;
					}

					tabs[i]=new TabPosition( new Rectangle( pos, 0, measuredWidth, 0 ), row, false );
					pos += measuredWidth -1;
				}
			}
		}

		// Size the tabs for SizeMode fillRight
		private void SizeTabsFillRight(ref TabPosition[] tabs, Graphics g, int rowWidth, out int maxRow)
		{
			// Start by sizing as per normal
			SizeTabsNormal( ref tabs, g, rowWidth, out maxRow, false );

			// Find the total width of all the tabs
			int width = 0;
			for (int i = 0; i < tabs.Length; i++)
			{
				if ((tabPageCollection[i] as TabPage).Visible)
					width += tabs[i].bounds.Width;
			}
			
			// This would be the perfect row width
			int optimalWidth = width / (maxRow + 1);

			int currentPos = 0;
			for ( int row = 0; row <= maxRow; row++ )
			{
				// For the last row pad all the remaining tabs
				if (row == maxRow)
					optimalWidth = rowWidth;
				PadRow( ref tabs, ref currentPos, optimalWidth, row, rowWidth );
			}
		}

		// Pad tab widths evenly, from startPos and ending where total width is greater than the ideal
		private void PadRow (ref TabPosition[] tabs, ref int startPos, int idealWidth, int newRow, int rowWidth)
		{
			int totalWidth = 0;
			int posEnd = startPos;
			for (; posEnd < tabs.Length; posEnd++)
			{
				totalWidth += tabs[posEnd].bounds.Width;
				// We end the count if we have exceeded the ideal width
				if (totalWidth > idealWidth)
					break;
			}

			// How many pixels left for this row
			int pixelsLeft = rowWidth;
			// Starting position
			int posTotal = 0;

			for (int pos = startPos; pos < posEnd; pos++ )
			{
				int thisWidth = pixelsLeft/(posEnd - pos);
				tabs[pos].bounds = new Rectangle(posTotal,0,thisWidth,0);
				tabs[pos].row = newRow;
				pixelsLeft -= thisWidth;
				posTotal += thisWidth;
			}

			// We will start at this position next time
			startPos = posEnd;
		}

		// Using the tabs row and the widths, calculate each tabs actual bounds
		private void LayoutTabBounds(ref TabPosition[] tabs, int rowWidth)
		{
			int down = ActualTabHeight;
			int top = 0;
			int yDirection = 1;
			// At the bottom, we start a row higher
			int extraRow = 0;
			if (alignment == TabAlignment.Bottom)
			{
				yDirection = -1;
				extraRow = 1;
				top = Height;
			}
			
			int lastVisible = -1;
			int leftOffset = 0;
			if (moveButtonsShowing)
				leftOffset = tabs[moveButtonsTabOffset].bounds.Left;
			int totalWidth = 0;
			for( int i=0;i < tabPageCollection.Count; i++)
			{
				if ((tabPageCollection[i] as TabPage).Visible)
				{
					// Move the tabs thats selected by 1
					int selDelta = 0;
					if (i != selectedIndex)
						selDelta = 1;

					TabPosition tab = tabs[i];
					int width = tab.bounds.Width + (1-selDelta)*2*indent;
					// Draw the left edge of tabs that begin a row or are selected
					if (tab.bounds.Left - leftOffset == 0 || selectedIndex == i)
						tab.leftExposed = true;
					tab.bounds = new Rectangle(tab.bounds.Left + indent - (1-selDelta)*2 - leftOffset, yDirection *((tab.row + extraRow ) * down + selDelta) + top, width, down + 1 - selDelta );
					tabs[i] = tab;
					lastVisible = i;
					totalWidth += width;
				}
			}
			moveButtonsShowing = !multiline && totalWidth > rowWidth;
			// Are the tab move buttons covered by a tab. This is used to disallow the tabs moving left anymore
			if (moveButtonsShowing)
				moveButtonsCovered = moveButtonLeftBounds.Left < tabs[lastVisible].bounds.Right;
					
		}

		// Move a particular row Row to the bottom of the tabs (lastRow)
		private void RowToBottom(ref TabPosition[] tabs, int row, int lastRow)
		{
			// Find pos of first element of row
			int rowStart = 0;
			for(; rowStart < tabs.Length; rowStart++)
			{
				if (tabs[rowStart].row == row)
					break;
			}
			// Find pos of last element of row
			int rowEnd = rowStart;
			for(; rowEnd < tabs.Length - 1; rowEnd++)
			{
				if (tabs[rowEnd + 1].row != row)
					break;
			}
			// Change the row to the new row
			for( int i = rowStart; i < rowEnd + 1; i++ )
				tabs[i].row = lastRow;
			// These rows were all moved up
			for( int i = rowEnd + 1; i < tabs.Length; i++ )
				tabs[i].row--;
		}

		// All tab positions and the total rows
		private class TabPositionInfo
		{
			public int totalRows;
			public TabPosition[] positions;
			
			public TabPositionInfo( int totalRows, TabPosition[] positions)
			{
				this.totalRows = totalRows;
				this.positions = positions;
			}

			public static TabPositionInfo Empty
			{
				get 
				{
					return new TabPositionInfo(0, new TabPosition[0]);
				}
			}

		}

		// Each tab
		private struct TabPosition
		{
			public Rectangle bounds;
			public int row;
			public bool leftExposed;
			public TabPosition( Rectangle bounds, int row, bool leftExposed )
			{
				this.bounds = bounds;
				this.row = row;
				this.leftExposed = leftExposed;
			}
		}

		// The currently selected tab
		public int SelectedIndex {
			get
			{
				return selectedIndex;
			}
			set
			{
				if (value != selectedIndex)
				{
					prevSelectedIndex = selectedIndex;
					selectedIndex = value;
					OnSelectedIndexChanged(EventArgs.Empty);
				}
			}
		}

		// This occurs for each tab needing to be drawn
		protected virtual void OnDrawItem( DrawItemEventArgs e )
		{
			e.DrawBackground();
			Rectangle bounds = e.Bounds;
			
			using ( SolidBrush backBrush = new SolidBrush(BackColor) )
			{
				// Clear out the border of tabbase where it is connected to the selected tab
				if ( e.Index == selectedIndex)
					e.Graphics.FillRectangle(backBrush, new Rectangle(e.Bounds.Left + 1, e.Bounds.Bottom, e.Bounds.Width - 3 + 1, 2+1));
			}
			Rectangle borderBounds = new Rectangle(e.Bounds.Left, e.Bounds.Top, e.Bounds.Width, e.Bounds.Height+2);
			
			//TODO: If Appearance is tab then this ok, need to do for button & flat button
			// Draw tab edging
			DrawTab( e.Graphics, borderBounds, alignment, PositionInfo.positions[e.Index].leftExposed );

			// Owner Draw does their own drawing
			if (drawMode == TabDrawMode.Normal)
			{
				//TODO: If imageindex & imagelist then draw
				
				Rectangle textBounds = new Rectangle(bounds.Left + tabTextWidthMargin, bounds.Top + tabTextHeightMargin, bounds.Width - tabTextWidthMargin*2, bounds.Height - tabTextHeightMargin * 2);
				Rectangle focusBounds = textBounds;
				focusBounds.Inflate(1,1);

				if ((e.State & DrawItemState.Focus)>0)
					ControlPaint.DrawFocusRectangle(e.Graphics, focusBounds, ForeColor, BackColor);
				//TODO: Draw disabled
				DrawText( e.Graphics, tabPageCollection[e.Index].Text, textBounds, e.ForeColor, e.BackColor, hotTrack && e.Index == hotTrackIndex );
				
			}

			if(DrawItem != null)
			{
				DrawItem(this, e);
			}
		}

		// Draw the actual tab for each alignment.
		private void DrawTab( Graphics graphics, Rectangle bounds, TabAlignment alignment, bool leftEdge )
		{
			Border3DSide left = Border3DSide.Left;
			Border3DSide top = Border3DSide.Top;
			Border3DSide right = Border3DSide.Right;
			if (alignment == TabAlignment.Bottom)
				top = Border3DSide.Bottom;
			if ( leftEdge )
				ControlPaint.DrawBorder3D(graphics, bounds, Border3DStyle.RaisedInner, left);
			ControlPaint.DrawBorder3D(graphics, bounds, Border3DStyle.RaisedInner, top);
			ControlPaint.DrawBorder3D(graphics, bounds, Border3DStyle.Raised, right);
		}

		// Draw the text on the tab using the right colors
		private void DrawText(Graphics graphics, string text, Rectangle bounds, Color foreColor, Color backColor, bool hotTrack)
		{

			Color color = hotTrack ? SystemColors.HotTrack : ForeColor;
			using (Brush brush = new SolidBrush( color )) 
			{
				graphics.DrawString( text, Font, brush, bounds );
			}
		}

		// Select the tab that is clicked
		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown (e);
			if (moveButtonsShowing && moveButtonLeftBounds.Contains( e.X, e.Y ))
			{
				if (moveButtonsTabOffset > 0)
				{
					moveButtonsTabOffset--;
					positionInfo = null;
					InvalidateTabs();
				}
				moveButtonLeftState = ButtonState.Pushed;
				Invalidate( moveButtonLeftBounds );
			}
			else if (moveButtonsShowing && moveButtonRightBounds.Contains(e.X, e.Y))
			{
				if (moveButtonsTabOffset < tabPageCollection.Count - 1 && moveButtonsCovered)
				{
					moveButtonsTabOffset++;
					positionInfo = null;
					InvalidateTabs();
				}
				moveButtonRightState = ButtonState.Pushed;
				Invalidate( moveButtonRightBounds );
			}
			else
			{
				moveButtonRightState = ButtonState.Normal;
				Focus();
				SelectedIndex = GetMouseOverTab( e.X, e.Y );
			}
		}

		// Find which tab the mouse is over
		private int GetMouseOverTab(int x, int y)
		{
			TabPosition[] tabs = PositionInfo.positions;
			for( int i = 0; i < tabs.Length; i++ )
			{
				if ( tabs[i].bounds.Contains( x , y ) )
					return i;
			}
			return -1;
		}

		// Reset the tab move buttons
		protected override void OnMouseUp(MouseEventArgs e)
		{
			if (moveButtonLeftState != ButtonState.Normal)
			{
				moveButtonLeftState = ButtonState.Normal;
				Invalidate( moveButtonLeftBounds );
			}
			if (moveButtonRightState != ButtonState.Normal)
			{
				moveButtonRightState = ButtonState.Normal;
				Invalidate( moveButtonRightBounds );
			}
			base.OnMouseUp (e);
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			if (hotTrack)
			{
				int prevHotTrackIndex = hotTrackIndex;
				hotTrackIndex = GetMouseOverTab( e.X, e.Y );
				// Dont draw more than we have to
				if (hotTrackIndex > -1 && hotTrackIndex != prevHotTrackIndex)
				{
					if (prevHotTrackIndex > -1)
						Invalidate( positionInfo.positions[prevHotTrackIndex].bounds );
					Invalidate( positionInfo.positions[hotTrackIndex].bounds );
				}
				else if (hotTrackIndex == -1 && prevHotTrackIndex > -1)
					Invalidate( positionInfo.positions[prevHotTrackIndex].bounds );

			}
			base.OnMouseMove (e);
		}

		protected override void OnMouseLeave(EventArgs e)
		{
			if (hotTrack && hotTrackIndex > -1)
			{
				int prevHottrackIndex = hotTrackIndex;
				hotTrackIndex = -1;
				Invalidate( positionInfo.positions[prevHottrackIndex].bounds );
			}
			base.OnMouseLeave (e);
		}

		// Display Area of the tab pages
		public override Rectangle DisplayRectangle 
		{
			get
			{
				Rectangle rect = GetTabBaseBounds();
				return new Rectangle( rect.Left + padding.X, rect.Top + padding.Y, rect.Width - 2* padding.X, rect.Height - 2 * padding.Y );
			}
		}

		// Each tab height the excluding border
		private int ActualTabHeight
		{
			get
			{
				return  ItemSize.Height - 3;
			}
		}

		// Returns the dimensions of the base (piece without tabs)
		private Rectangle GetTabBaseBounds()
		{
			int offset = TabOffset;
			switch( alignment )
			{
				case TabAlignment.Left:
					return new Rectangle( offset, 0, Bounds.Width - offset, Bounds.Height );
				case TabAlignment.Top:
					return new Rectangle( 0, offset, Bounds.Width, Bounds.Height - offset );
				case TabAlignment.Right:
					return new Rectangle( 0, 0, Bounds.Width - offset, Bounds.Height );
				case TabAlignment.Bottom:
					return new Rectangle( 0, 0, Bounds.Width, Bounds.Height - offset );
				default:
					return Rectangle.Empty;
			}
		}

		// Height in pixels of tab
		private int TabOffset
		{
			get
			{
				return ActualTabHeight * PositionInfo.totalRows;
			}
		}

		// Returns the dimensions of the tab area
		private Rectangle GetTabsBounds()
		{
			int offset = TabOffset;
			switch( alignment )
			{
				case TabAlignment.Left:
					return new Rectangle( 0, 0, offset, Bounds.Height );
				case TabAlignment.Top:
					return new Rectangle( 0, 0, Bounds.Width, offset );
				case TabAlignment.Right:
					return new Rectangle( Bounds.Width - offset, 0, offset, Bounds.Height );
				case TabAlignment.Bottom:
					return new Rectangle( 0, Bounds.Height - offset, Bounds.Width, offset );
				default:
					return Rectangle.Empty;
			}
		}

		// Because we overwrite the border of the tab base, we need to invalidate more than the tab base bounds
		internal void InvalidateTabs()
		{
			Rectangle bounds = GetTabsBounds();
			switch (alignment)
			{
				case TabAlignment.Left:
					Invalidate( new Rectangle(bounds.Left, bounds.Top, bounds.Width + 1, bounds.Height));
					break;
				case TabAlignment.Top:
					Invalidate( new Rectangle(bounds.Left, bounds.Top, bounds.Width, bounds.Height + 1));
					break;
				case TabAlignment.Right:
					Invalidate( new Rectangle(bounds.Left - 1, bounds.Top, bounds.Width + 1, bounds.Height));
					break;
				case TabAlignment.Bottom:
					Invalidate( new Rectangle(bounds.Left, bounds.Top - 1, bounds.Width, bounds.Height + 1));
					break;
			}
		}

		//HACK: This is a bug in Contol.cs. Remove this when its fixed
		// One way or the other, PerformLayout should be called when a control is created
		protected override void OnCreateControl()
		{
			base.OnCreateControl ();
			PerformLayout();
		}

		// The event thats called when controls need to relayout their contents
		protected override void OnLayout(LayoutEventArgs e)
		{
			base.OnLayout (e);
			// Force the position info to update
			positionInfo = null;
			// This forces the correct Bounds for all TabPages and is automatically called whenever the bounds could change eg. adding new tab
			foreach( TabPage tabPage in tabPageCollection)
				tabPage.Bounds = Rectangle.Empty;
			
		}

	}
}
