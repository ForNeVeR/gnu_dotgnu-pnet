/*
 * TreeView.cs - Implementation of the
 *			"System.Windows.Forms.TreeView" class.
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
	using System;
	using System.Drawing;
	using System.Collections;
	using System.Drawing.Drawing2D;

	public class TreeView : Control
	{
		private bool checkBoxes;
		internal TreeNode editNode;
		private bool fullRowSelect;
		private bool hideSelection;
		private bool hotTracking;
		internal int imageIndex;
		private ImageList imageList;
		private int indent;
		private int itemHeight;
		private bool labelEdit;
		internal TreeNodeCollection nodes;
		private string pathSeparator = @"\";
		internal TreeNode root;
		private bool scrollable;
		internal int selectedImageIndex;
		internal TreeNode selectedNode;
		private bool showLines;
		private bool showPlusMinus;
		private bool showRootLines;
		private bool sorted;
		private Pen markerPen;
		private TreeNode topNode;
		private TextBox textBox;
 
		public event TreeViewEventHandler AfterCheck;
		public event TreeViewEventHandler AfterCollapse;
		public event TreeViewEventHandler AfterExpand;
		public event NodeLabelEditEventHandler AfterLabelEdit;
		public event TreeViewEventHandler AfterSelect;
		public event TreeViewCancelEventHandler BeforeCheck;
		public event TreeViewCancelEventHandler BeforeCollapse;
		public event TreeViewCancelEventHandler BeforeExpand;
		public event NodeLabelEditEventHandler BeforeLabelEdit;
		public event TreeViewCancelEventHandler BeforeSelect;
		public event ItemDragEventHandler ItemDrag;
		public TreeView() : base()
		{
			hideSelection = true;
			indent = 19;
			itemHeight = -1;
			scrollable = true;
			BorderStyleInternal = BorderStyle.Fixed3D;
			showLines = true;
			showPlusMinus = true;
			showRootLines = true;
			root = new TreeNode(this);
			root.Expand();
			nodes = new TreeNodeCollection(root);
			SetStyle(ControlStyles.StandardClick, false);
		}
		public void BeginUpdate()
		{
		}
		public void CollapseAll()
		{
			root.Collapse();
		}
		public void EndUpdate()
		{
		}
		public void ExpandAll()
		{
			root.ExpandAll();
		}
		public TreeNode GetNodeAt(int x, int y)
		{
			return null;
		}
		public TreeNode GetNodeAt(Point pt)
		{
			return null;
		}

		public int GetNodeCount(bool includeSubTrees)
		{
			return root.GetNodeCount(includeSubTrees);
		}

		protected override bool IsInputKey(Keys keyData)
		{
			return base.IsInputKey(keyData);
		}

		protected internal virtual void OnAfterCheck(TreeViewEventArgs e)
		{
			if (AfterCheck != null)
				AfterCheck(this, e);
		}

		protected internal virtual void OnAfterCollapse(TreeViewEventArgs e)
		{
			if (AfterCollapse != null)
				AfterCollapse(this, e);
		}
		
		protected virtual void OnAfterExpand(TreeViewEventArgs e)
		{
			if (AfterExpand != null)
				AfterExpand(this, e);
		}

		protected virtual void OnAfterLabelEdit(NodeLabelEditEventArgs e)
		{
			if (AfterLabelEdit != null)
				AfterLabelEdit(this, e);
		}

		protected virtual void OnAfterSelect(TreeViewEventArgs e)
		{
			if (AfterSelect != null)
				AfterSelect(this, e);
		}

		protected internal virtual void OnBeforeCheck(TreeViewCancelEventArgs e)
		{
			if (BeforeCheck != null)
				BeforeCheck(this, e);
		}

		protected internal virtual void OnBeforeCollapse(TreeViewCancelEventArgs e)
		{
			if (BeforeCollapse != null)
				BeforeCollapse(this, e);
		}

		protected virtual void OnBeforeExpand(TreeViewCancelEventArgs e)
		{
			if (BeforeExpand != null)
				BeforeExpand(this, e);
		}

		protected virtual void OnBeforeLabelEdit(NodeLabelEditEventArgs e)
		{
			if (BeforeLabelEdit != null)
				BeforeLabelEdit(this, e);
		}

		protected virtual void OnBeforeSelect(TreeViewCancelEventArgs e)
		{
			if (BeforeSelect != null)
				BeforeSelect(this, e);
		}

		protected virtual void OnItemDrag(ItemDragEventArgs e)
		{
			if (ItemDrag != null)
				ItemDrag(this, e);
		}

		public override string ToString()
		{
			string s = base.ToString();
			if (Nodes != null)
			{
				s = s + ", Nodes.Count: " + Nodes.Count;
				if (Nodes.Count > 0)
					s = s + ", Nodes[0]: " + Nodes[0].ToString();
			}
			return s; 

		}

		public BorderStyle BorderStyle 
		{ 
			get
			{
				return BorderStyleInternal;
			}
			set
			{
				BorderStyleInternal = value;
				ResetView();
			}
		}
		public bool CheckBoxes
		{
			get
			{
				return checkBoxes;
			}
			set
			{
				if (checkBoxes != value)
				{
					checkBoxes = value;
					ResetView();
				}
			}
		}

		protected override Size DefaultSize
		{
			get
			{
				return new Size(121, 97);
			}
		}

		public override Color ForeColor
		{
			get
			{
				// TODO
				return base.ForeColor;
			}
			set
			{
			}
		}
		public bool FullRowSelect
		{
			get
			{
				return fullRowSelect;
			}
			set
			{
				if (fullRowSelect != value)
				{
					fullRowSelect = value;
					ResetView();
				}
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
				if (hideSelection != value)
				{
					hideSelection = value;
					ResetView();
				}
			}
		}
		public bool HotTracking
		{
			get
			{
				return hotTracking;
			}
			set
			{
				if (value != hotTracking)
				{
					hotTracking = value;
				}
			}
		}
		public int ImageIndex
		{
			get
			{
				return imageIndex;
			}
			set
			{
				if (value != imageIndex)
				{
					imageIndex = value;
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
				if (value != imageList)
				{
					imageList = value;
					ResetView();
				}
			}
		}

		public int Indent
		{
			get
			{
				return indent;
			}
			set
			{
				if (value != indent)
				{
					indent = value;
					ResetView();
				}
			}
		}

		public int ItemHeight
		{
			get
			{
				if (itemHeight == -1)
					return FontHeight + 3;
				return itemHeight;
			}
			set
			{
				if (value != itemHeight)
				{
					itemHeight = value;
					ResetView();
				}
			}
		}

		public bool LabelEdit
		{
			get
			{
				return labelEdit;
			}
			set
			{
				if (value != labelEdit)
				{
					labelEdit = value;
				}
			}
		}
		public TreeNodeCollection Nodes
		{
			get
			{
				return nodes;
			}
		}
		public string PathSeparator
		{
			get
			{
				return pathSeparator;
			}
			set
			{
				pathSeparator = value;
			}
		}
		public bool Scrollable
		{
			get
			{
				return scrollable;
			}
			set
			{
				if (value != scrollable)
				{
					scrollable = value;
					ResetView();
				}
			}
		}
		public int SelectedImageIndex
		{
			get
			{
				return selectedImageIndex;
			}
			set
			{
				if (selectedImageIndex != value)
				{
					selectedImageIndex = value;
					ResetView();
				}
			}
		}
		public TreeNode SelectedNode
		{
			get
			{
				return selectedNode;
			}
			set
			{
				if (value != selectedNode)
				{
					// Redraw the old item
					if (selectedNode != null)
					{
						TreeNode oldNode = selectedNode;
						selectedNode = value;
						DrawLine(oldNode);
					}
					else
						selectedNode = value;
					DrawLine(selectedNode);
				}
			}
		}
		public bool ShowLines
		{
			get
			{
				return showLines;
			}
			set
			{
				if (value != showLines)
				{
					showLines = value;
					ResetView();
				}
			}
		}
		public bool ShowPlusMinus
		{
			get
			{
				return showPlusMinus;
			}
			set
			{
				if (value != showPlusMinus)
				{
					showPlusMinus = value;
					ResetView();
				}
			}
		}
		public bool ShowRootLines
		{
			get
			{
				return showRootLines;
			}
			set
			{
				if (value != showRootLines)
				{
					showRootLines = value;
					ResetView();
				}
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
				if (value != sorted)
				{
					sorted = value;
					// TODO could be done better!
					TreeNode[] nodes = new TreeNode[Nodes.Count];
					Nodes.CopyTo(nodes, 0);
					Nodes.Clear();
					Nodes.AddRange(nodes);
				}
			}
		}
		public TreeNode TopNode 
		{
			get
			{
				return topNode;
			}
		}
		public int VisibleCount
		{
			get
			{
				int count = 0;
				IEnumerator nodes = new NodeEnumerator(Nodes);
				while (nodes.MoveNext())
					if ((nodes.Current as TreeNode).bounds != Rectangle.Empty)
						count++;
				return count;
			}
		}

		protected override void OnVisibleChanged(EventArgs e)
		{
			base.OnVisibleChanged (e);
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			// Clear background
			e.Graphics.FillRectangle(SystemBrushes.Window, ClientRectangle);
			Draw(e.Graphics, root);
			base.OnPaint (e);
		}

		// Offset of first node from client area
		private Point Offset
		{
			get
			{
				return new Point(2, 2);
			}
		}


		// Render the treeview starting from startingLine
		internal void Draw(Graphics g, TreeNode startNode)
		{
			int x = Offset.X;
			int y = Offset.Y;
			int line = 0;
			int prevLevel = 0;
			// Are we drawing this node and subsequent nodes
			bool drawing = false;
			// If the topNode is not the first node, then some nodes are offscreen
			bool inView = false;
			StringFormat format = new StringFormat(StringFormatFlags.NoWrap);
			NodeEnumerator nodes = new NodeEnumerator(Nodes);
			while (nodes.MoveNext())
			{
				TreeNode node = nodes.Current as TreeNode;
				if (topNode == null)
					topNode = node;
				if (!drawing)
				{
					if (node == startNode || startNode == root)
					{
						// Clear background
						g.FillRectangle(SystemBrushes.Window, 0, y, ClientSize.Width, ClientSize.Height - y);
						drawing = true;
					}
				}

				if (node == topNode)
					inView = true;

				x += (nodes.level - prevLevel) * indent;
				
				if (drawing)
				{
					node.heirarchyMarkerBounds = new Rectangle(x, y , indent - 6, ItemHeight);
					DrawHeirarchyMarker(g, node);
					node.bounds.Size = g.MeasureString(node.Text, Font).ToSize();
					node.bounds.Location = new Point(x + indent, y + 1);
					if (node == selectedNode && Focused)
					{
						g.FillRectangle(SystemBrushes.Highlight,node.bounds);
						g.DrawString(node.Text, Font, SystemBrushes.HighlightText, node.bounds,format);
						Rectangle r = node.bounds;
						r = new Rectangle(r.X - 1, r.Y - 1, r.Width + 1, r.Height + 1);
						ControlPaint.DrawFocusRectangle(g, r);
					}
					else
						g.DrawString(node.Text, Font, SystemBrushes.ControlText, node.bounds,format);
				}
				if (!inView)
				{
					node.heirarchyMarkerBounds = Rectangle.Empty;
					node.bounds = Rectangle.Empty;
				}

				prevLevel = nodes.level;
				if (inView)
					y += ItemHeight;
				line++;
			}
			// Set node.bounds = Rectangle.Empty for all non displayed nodes
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown (e);
			ProcessMouse(e.X, e.Y);
		}

		protected override void OnLostFocus(EventArgs e)
		{
			base.OnLostFocus (e);
			if (selectedNode != null)
				DrawLine(selectedNode);
		}

		protected override void OnGotFocus(EventArgs e)
		{
			base.OnGotFocus (e);
			if (selectedNode != null)
				DrawLine(selectedNode);
		}


		internal void ProcessMouse(int x, int y)
		{
			IEnumerator nodes = new NodeEnumerator(Nodes);
			while (nodes.MoveNext())
			{
				TreeNode node = nodes.Current as TreeNode;
				if (node.heirarchyMarkerBounds.Contains(x, y))
					node.Toggle();
				if (node.bounds.Contains(x, y))
				{
					if (SelectedNode == nodes.Current && labelEdit)
						node.BeginEdit();
					else
						SelectedNode = node;
				}
			}
		}

		internal void BeginEdit(TreeNode node)
		{
			editNode = node;
			if (textBox == null)
			{
				textBox = new TextBox();
				textBox.BorderStyle = BorderStyle.FixedSingle;
				textBox.Visible = false;
				Controls.Add(textBox);
				textBox.Leave +=new EventHandler(textBox_Leave);
			}
			textBox.Location = node.bounds.Location - new Size(2,2);
			textBox.Size = new Size(node.bounds.Width + 14, node.bounds.Height + 4);
			textBox.Text = editNode.Text;
			textBox.Visible = true;
			textBox.Focus();
			textBox.SelectAll();

		}

		
		private void textBox_Leave(object sender, EventArgs e)
		{	
			EndEdit();
			textBox.Visible = false;
		}

		internal void EndEdit()
		{
			editNode.Text = textBox.Text;
			editNode = null;
		}

		private void DrawHeirarchyMarker(Graphics g, TreeNode node)
		{
			int midX = node.heirarchyMarkerBounds.X + 4;
			int midY = node.heirarchyMarkerBounds.Y + node.heirarchyMarkerBounds.Height / 2;
			int lineRightStart = midX;
			int lineTopEnd = midY;
			if (node.Nodes.Count > 0)
			{
				GraphicsPath path = new GraphicsPath();
				path.AddRectangle(new Rectangle(midX - 4, midY - 4, 8, 8));
				path.AddLine(midX - 2, midY, midX + 2, midY);
				if (!node.IsExpanded)
					path.AddLine(midX, midY - 2, midX, midY + 2);
				g.DrawPath(SystemPens.ControlText, path);
				lineRightStart += 6;
				lineTopEnd -= 6;
			}
			// Draw the right lead line
			if (node.heirarchyMarkerBounds.Right > lineRightStart)
				g.DrawLine(MarkerPen, lineRightStart, midY, node.heirarchyMarkerBounds.Right, midY);
			// Draw the top lead line
			TreeNode lineNode = node.PrevNode;
			if (lineNode == null)
				lineNode = node.Parent;
			if (lineNode != null)
				g.DrawLine(MarkerPen, midX, lineNode.markerLineY, midX, lineTopEnd);
			if (node.Nodes.Count > 0)
				node.markerLineY = midY + 6;
			else
				node.markerLineY = midY;
		}

		// Cached pen used to draw lead lines for nodes
		private Pen MarkerPen
		{
			get
			{
				if (markerPen == null)
				{
					markerPen = new Pen(SystemColors.ControlLight);
					markerPen.DashStyle = DashStyle.Dot;
				}
				return markerPen;
			}
		}

		// Draw from startNode downwards
		internal void Draw(TreeNode startNode)
		{
			if (!Created || !Visible)
				return;
			using (Graphics g = CreateGraphics())
			{
				Draw(g, startNode);
			}
		}

		// Just draw the line, only use if the text, checkbox or icon has changed
		internal void DrawLine(TreeNode node)
		{
			if (!Created || !Visible)
				return;
			using (Graphics g = CreateGraphics())
			{
				// TODO
				Draw(g, node);
			}
		}

		internal void ResetView()
		{
			topNode = null;
			Draw(root);
		}

		// Private enumerator class for all expanded nodes.
		private class NodeEnumerator : IEnumerator
		{
			// Internal state.
			private TreeNodeCollection nodes;
			private TreeNode currentNode;
			private bool first;
			// level of node
			internal int level = 0;

			// Constructor.
			public NodeEnumerator(TreeNodeCollection nodes)
			{
				this.nodes = nodes;
				Reset();
			}

			// Move to the next element in the enumeration order.
			public bool MoveNext()
			{
				if (first)
				{
					if (nodes.Count == 0)
						return false;
					currentNode = nodes[0];
					first = false;
					return true;
				}
				if (currentNode == null)
					return false;
				if (currentNode.Nodes.Count > 0 && currentNode.expanded)
				{
					// If expanded climb into hierarchy
					currentNode = currentNode.Nodes[0];
					level++;
				}
				else
				{
					TreeNode nextNode = currentNode.NextNode;
					while (nextNode == null)
					{
						// We need to move back up.
						// Are we back at the top?
						if (currentNode.Parent == null)
							return false;
						else
						{
							currentNode = currentNode.Parent;
							nextNode = currentNode.NextNode;
							level--;
						}
					}
					currentNode = nextNode;
				}
				return true;

			}

			// Reset the enumeration.
			public void Reset()
			{
				first = true;
				currentNode = null;
				level = 0;
			}

			// Get the current value in the enumeration.
			public Object Current
			{
				get
				{
					if(currentNode == null)
						throw new InvalidOperationException();
					else
						return currentNode;
				}
			}

		}

	}

}
