/*
 * TreeNode.cs - Implementation of the
 *			"System.Windows.Forms.TreeNode" class.
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
 */

namespace System.Windows.Forms
{

	using System;
	using System.Drawing;
	using System.Text;
	using System.Globalization;

	public class TreeNode : ICloneable//, ISerializable
	{
		internal int index;
		internal int childCount = 0;
		internal TreeNode[] children;
		internal TreeNode parent;
		internal bool expanded;
		internal Rectangle bounds;
		internal Rectangle heirarchyMarkerBounds;
		// The y coordinate of the marker line.
		internal int markerLineY;
		internal TreeView treeView;
		private string text;
		private bool isChecked;
		private int imageIndex;
		private int selectedImageIndex;
		private TreeNodeCollection nodes;
		private object tag;
		private Font nodeFont;
		// Size of this text in pixels.
		internal Size textSize = Size.Empty;

		public TreeNode()
		{
			imageIndex = -1;
			selectedImageIndex = -1;
		}

		internal TreeNode(TreeView treeView) : this()
		{
			this.treeView = treeView;
		}

		public TreeNode(string text) : this()
		{
			this.text = text;
		}

		public TreeNode(string text, TreeNode[] children) : this()
		{
			this.text = text;
			Nodes.AddRange(children);
		}

		public TreeNode(string text, int imageIndex, int selectedImageIndex) : this(text)
		{
			this.imageIndex = imageIndex;
			this.selectedImageIndex = selectedImageIndex;
		}

		public TreeNode(string text, int imageIndex, int selectedImageIndex, TreeNode[] children) : this (text, imageIndex, selectedImageIndex)
		{
			Nodes.AddRange(children);
		}

		public virtual object Clone()
		{
			TreeNode node = new TreeNode(text, imageIndex, selectedImageIndex);
			if (childCount > 0)
			{
				node.children = new TreeNode[childCount];
				for (int i = 0; i < childCount; i++)
					node.Nodes.Add(children[i].Clone() as TreeNode);
			}
			node.Checked = Checked;
			node.Tag = Tag;
			return node;
		}

		public override string ToString()
		{
			return "TreeNode: " + (text == null ? "" : text);
		}

		// TODO Property Bag
		public Color BackColor
		{
			get
			{
				return Color.Empty;
			}
			set
			{

				// TODO
				Color color = BackColor;
				if (value.IsEmpty)
				{
					if (!color.IsEmpty)
					{
						value = color;
						treeView.ResetView();
					}
					return;      
				}
				if (value != color)
				{
					treeView.ResetView();
				}
			}
		}

		public Rectangle Bounds
		{
			get
			{
				return bounds;
			}
		}

		public bool Checked
		{
			get
			{
				return isChecked;
			}
		
			set
			{
				TreeView treeView = TreeView;
				if (treeView != null)
				{
					TreeViewCancelEventArgs e = new TreeViewCancelEventArgs(this, false, TreeViewAction.Unknown);
					treeView.OnBeforeCheck(e);
					if (e.Cancel)
						return;
					isChecked = value;
					treeView.DrawLine(this);
					treeView.OnAfterCheck(new TreeViewEventArgs(this, TreeViewAction.Unknown));
				}
				else
					isChecked = value;
			}
		}

		public TreeNode FirstNode
		{
			get
			{
				if (childCount == 0)
					return null;
				else
					return children[0];   
			}
		}

		private TreeNode FirstVisibleParent
		{
			get
			{
				TreeNode node = this;
				while (node != null)
				{
					if (node.Bounds.IsEmpty)
						node = node.Parent;
					else
						return node;
				}
					
				return node;  
			}
		}

		// TODO
		public Color ForeColor
		{
			get
			{
				return Color.Empty;
			}
			set
			{
			}
		}
					
		public string FullPath
		{
			get
			{
				StringBuilder s = new StringBuilder();
				GetFullPath(s, TreeView.PathSeparator);
				return s.ToString();
			}
		}

		public IntPtr Handle
		{
			get
			{
				// This is not used in this implementation
				return IntPtr.Zero;
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
				imageIndex = value;
				treeView.DrawLine(this);
			}
		}

		public int Index
		{
			get
			{
				return index;
			}
		}

		public bool IsEditing
		{
			get
			{
				if (TreeView == null)
					return false;
				else
					return TreeView.editNode == this;
			}
		}

		public bool IsExpanded
		{
			get
			{
				return expanded;
			}
		}

		public bool IsSelected
		{
			get
			{
				if (treeView == null)
					return false;
				return treeView.selectedNode == this;
			}
		}

		public bool IsVisible
		{
			get
			{
				if (treeView == null || !treeView.Visible)
					return false;
				return bounds != Rectangle.Empty;
			}
		}

		public TreeNode LastNode
		{
			get
			{
				if (childCount == 0)
					return null;
				else
					return children[childCount - 1];
			}
		}

		public TreeNode NextNode
		{
			get
			{
				if (index < parent.Nodes.Count - 1)
					return parent.Nodes[index + 1];
				else
					return null;
			}
		}

		public TreeNode NextVisibleNode
		{
			get
			{
				
				TreeNode node = FirstVisibleParent;
				if (node != null)
				{
					// TODO
					return null;
				}
				else
					return null;
			}
		}

		// TODO
		public Font NodeFont
		{
			get
			{
				return nodeFont;
			}
			set
			{
				if (value != nodeFont)
				{
					nodeFont = value;
					textSize = Size.Empty;
				}
				if (treeView != null && bounds != Rectangle.Empty)
					treeView.Draw(this);
			}
		}

		public TreeNodeCollection Nodes
		{
			get
			{
				if (nodes == null)
					nodes = new TreeNodeCollection(this);
				return nodes;
			}
		}

		public TreeNode Parent
		{
			get
			{
				if (TreeView != null && parent == TreeView.root)
					return null;
				return parent;
			}
		}

		public TreeNode PrevNode
		{
			get
			{
				if (index > 0 && index <= parent.Nodes.Count)
					return parent.Nodes[index - 1];
				else
					return null;
			}
		}

		public TreeNode PrevVisibleNode
		{
			get
			{
				TreeNode node = FirstVisibleParent;
				if (node != null)
				{
					// TODO
					return null;
				}
				else
					return null;
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
				selectedImageIndex = value;
				treeView.DrawLine(this);
			}
		}

		public object Tag
		{
			get
			{
				return tag;
			}
			set
			{
				tag = value;
			}
		}

		public string Text
		{
			get
			{
				if (text == null)
					return String.Empty;
				else
					return text;
			}
			set
			{
				text = value;
				textSize = Size.Empty;
				if (treeView != null && bounds != Rectangle.Empty)
					treeView.DrawLine(this);
			}
		}

		public TreeView TreeView
		{
			get
			{
				return treeView;
			}
		}

		internal int AddSorted(TreeNode node)
		{
			int pos = 0;
			string text = node.Text;
			if (childCount > 0)
			{
				Globalization.CompareInfo compare = Application.CurrentCulture.CompareInfo;
				// Simple optimization if added in sort order
				if (compare.Compare(children[(childCount - 1)].Text, text) <= 0)
					pos = childCount;
				else
				{
					// Binary Search
					int i = 0;
					int j = childCount;
					while (i < j)
					{
						int mid = (i + j) / 2;
						if (compare.Compare(children[mid].Text, text) <= 0)
							i = mid + 1;
						else
							i = mid;
					}
					pos = i;
				}
			}

			node.SortChildren();
			InsertNodeAt(pos, node);
			if (TreeView != null && node == TreeView.selectedNode)
				TreeView.SelectedNode = node;
			return pos;
		}

		// Not used in this implementation
		public static TreeNode FromHandle(TreeView tree, IntPtr handle)
		{
			return null;
		}

		private void SortChildren()
		{
			if (childCount > 0)
			{
				TreeNode[] sort = new TreeNode[childCount];
				CompareInfo compare = Application.CurrentCulture.CompareInfo;
				for (int i = 0; i < childCount; i++)
				{

					int pos = -1;
					for (int j = 0; j < childCount; j++)
					{
						if (children[j] != null)
						{
							if (pos == -1)
								pos = j;
							else
								if (compare.Compare(children[j].Text, children[pos].Text) < 0)
									pos = j;
						}
					}
					sort[i] = children[pos];
					children[pos] = null;
					sort[i].index = i;
					sort[i].SortChildren();
				}
				children = sort;
			}
		}

		public void BeginEdit()
		{
			if (treeView != null)
			{
				if (!TreeView.LabelEdit)
					throw new Exception("LabelEdit is false");
				if (!TreeView.Focused)
					TreeView.Focus();
				treeView.BeginEdit(this);
			}
		}

		internal void Clear()
		{
			while (childCount > 0)
				children[(childCount - 1)].Remove();
			children = null;
		}

		public void Collapse()
		{
			CollapseInternal();
			treeView.Draw(this);
		}

		public void CollapseInternal()
		{
			if (!expanded)
				return;
			expanded = false;
			bool selected = false;
			if (childCount > 0)
			{
				for (int i = 0; i < childCount; i++)
				{
					if (treeView.SelectedNode == children[i])
						selected = true;
					children[i].CollapseInternal();
				}
			}
			TreeViewCancelEventArgs eventArgs = new TreeViewCancelEventArgs(this, false, TreeViewAction.Collapse);
			treeView.OnBeforeCollapse(eventArgs);
			if (!eventArgs.Cancel)
				treeView.OnAfterCollapse(new TreeViewEventArgs(this));
			if (selected)
				treeView.SelectedNode = this;
		}

		public void EndEdit(bool cancel)
		{
			if (treeView != null)
				treeView.EndEdit();
		}

		internal void SizeChildrenArray()
		{
			if (children == null)
				children = new TreeNode[10];
			else if (childCount == children.Length)
			{
				TreeNode[] copy = new TreeNode[childCount * 2];
				Array.Copy(children, 0, copy, 0, childCount);
				children = copy;
			}
		}

		public void EnsureVisible()
		{
			// TODO
		}

		public void Expand()
		{
			if (!expanded)
			{
				TreeNode highestNode = null;
				for (TreeNode node = this; node != null; node = node.Parent)
				{
					node.expanded = true;
					highestNode = this;
				}
				// Draw from this node down
				treeView.Draw(highestNode);
			}
		}

		public void ExpandAll()
		{
			Expand();
			for (int i = 0; i < childCount; i++)
				children[i].ExpandAll();
		}

		private void GetFullPath(StringBuilder path, string pathSeparator)
		{
			if (parent == null)
				return;
			parent.GetFullPath(path, pathSeparator);
			if (parent.parent != null)
				path.Append(pathSeparator);
			path.Append(text);
		}

		public int GetNodeCount(bool includeSubTrees)
		{
			int count = childCount;
			if (includeSubTrees)
				for (int i = 0; i < childCount; i++)
					count += children[i].GetNodeCount(true);
			return count;
		}

		public void Remove()
		{
			for (int i = 0; i < childCount; i++)
				children[i].Remove();
			for (int i = index; i < parent.childCount - 1; i++)
			{
				TreeNode node = parent.children[i + 1];
				parent.children[i] = parent.children[i + 1];
				node.index = i;
			}
			parent.childCount--;
			parent = null;
			// If its visible, redraw
			if (treeView != null && bounds != Rectangle.Empty)
				treeView.Draw(this);
			treeView = null;
		}

		public void Toggle()
		{
			if (expanded)
				Collapse();
			else
				Expand();
		}

		internal void InsertNodeAt(int index, TreeNode node)
		{
			SizeChildrenArray();
			node.parent = this;
			node.index = index;
			node.treeView = treeView;
			for (int i = childCount; i > index; i--)
			{
				TreeNode node1 = children[i - 1];
				node1.index = i;
				children[i] = node1;
			}
			childCount++;
			children[index] = node;
			if (treeView != null)
			{
				if (childCount == 1 && IsVisible)
					treeView.DrawLine(this);
				else if (expanded && children[index - 1].IsVisible)
					treeView.Draw(node);
			}
		}


}; // class TreeNode

}; // namespace System.Windows.Forms
