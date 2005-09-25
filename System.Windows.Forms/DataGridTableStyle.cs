/*
 * DataGridTableStyle.cs - Implementation of "System.Windows.Forms.DataGridTableStyle" 
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2004  Free Software Foundation, Inc.
 * Copyright (C) 2005  Boris Manojlovic.
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
	using System.ComponentModel;
	using System.Drawing;

	public class DataGridTableStyle : Component, IDataGridEditingService
	{
		private int _rowHeaderWidth;
		private int _preferredRowHeight;
		private int _preferredColumnWidth;
			
		private System.String _mappingName;

		private bool _defaultStyle = true;
		private bool _readOnly = false;
		private bool _rowHeadersVisible;
		private bool _allowSorting;
		private bool _columnHeadersVisible;
		
		private System.Drawing.Color _alternatingBackColor;
		private System.Drawing.Color _selectionForeColor;
		private System.Drawing.Color _selectionBackColor;
		private System.Drawing.Color _linkHoverColor;
		private System.Drawing.Color _linkColor;
		private System.Drawing.Color _headerForeColor;
		private System.Drawing.Color _gridLineColor;
		private System.Drawing.Font _headerFont;
		private System.Drawing.Color _headerBackColor;
		private System.Drawing.Color _backColor;
		private System.Drawing.Color _foreColor; 
		
		private DataGridLineStyle _gridLineStyle;
		

		[TODO]
		public DataGridTableStyle()
		{
			this._allowSorting = true;
			this._alternatingBackColor = System.Drawing.Color.Gold;
			this._backColor = System.Drawing.Color.White;
			this._gridLineColor = Color.Red;
			this._gridLineStyle = System.Windows.Forms.DataGridLineStyle.Solid;
			this._headerBackColor = System.Drawing.Color.AntiqueWhite;
		}

		[TODO]
		public DataGridTableStyle(bool isDefaultTableStyle)
		{
			this._defaultStyle = isDefaultTableStyle;
		}


		[TODO]
		public DataGridTableStyle(System.Windows.Forms.CurrencyManager listManager)
		{
			throw new NotImplementedException(".ctor");
		}

		[TODO]
		public virtual bool BeginEdit(System.Windows.Forms.DataGridColumnStyle gridColumn, int rowNumber)
		{
			throw new NotImplementedException("BeginEdit");
		}

		[TODO]
		protected internal virtual DataGridColumnStyle CreateGridColumn(PropertyDescriptor prop)
		{
			throw new NotImplementedException("CreateGridColumn");
		}

		[TODO]
		protected internal virtual DataGridColumnStyle CreateGridColumn(PropertyDescriptor prop, bool isDefault)
		{
			throw new NotImplementedException("CreateGridColumn");
		}

		[TODO]
		protected override void Dispose(bool disposing)
		{
			base.Dispose();
		}

		[TODO]
		public virtual bool EndEdit(DataGridColumnStyle gridColumn, int rowNumber, bool shouldAbort)
		{
			throw new NotImplementedException("EndEdit");
		}

		[TODO]
		protected virtual void OnAllowSortingChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnAllowSortingChanged");
		}

		[TODO]
		protected virtual void OnAlternatingBackColorChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnAlternatingBackColorChanged");
		}

		[TODO]
		protected virtual void OnBackColorChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnBackColorChanged");
		}

		[TODO]
		protected virtual void OnColumnHeadersVisibleChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnColumnHeadersVisibleChanged");
		}

		[TODO]
		protected virtual void OnForeColorChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnForeColorChanged");
		}

		[TODO]
		protected virtual void OnGridLineColorChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnGridLineColorChanged");
		}

		[TODO]
		protected virtual void OnGridLineStyleChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnGridLineStyleChanged");
		}

		[TODO]
		protected virtual void OnHeaderBackColorChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnHeaderBackColorChanged");
		}

		[TODO]
		protected virtual void OnHeaderFontChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnHeaderFontChanged");
		}

		[TODO]
		protected virtual void OnHeaderForeColorChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnHeaderForeColorChanged");
		}

		[TODO]
		protected virtual void OnLinkColorChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnLinkColorChanged");
		}

		[TODO]
		protected virtual void OnLinkHoverColorChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnLinkHoverColorChanged");
		}

		[TODO]
		protected virtual void OnMappingNameChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnMappingNameChanged");
		}

		[TODO]
		protected virtual void OnPreferredColumnWidthChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnPreferredColumnWidthChanged");
		}

		[TODO]
		protected virtual void OnPreferredRowHeightChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnPreferredRowHeightChanged");
		}

		[TODO]
		protected virtual void OnReadOnlyChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnReadOnlyChanged");
		}

		[TODO]
		protected virtual void OnRowHeaderWidthChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnRowHeaderWidthChanged");
		}

		[TODO]
		protected virtual void OnRowHeadersVisibleChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnRowHeadersVisibleChanged");
		}

		[TODO]
		protected virtual void OnSelectionBackColorChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnSelectionBackColorChanged");
		}

		[TODO]
		protected virtual void OnSelectionForeColorChanged(System.EventArgs e)
		{
			throw new NotImplementedException("OnSelectionForeColorChanged");
		}

		[TODO]
		public void ResetAlternatingBackColor()
		{
			throw new NotImplementedException("ResetAlternatingBackColor");
		}

		[TODO]
		public void ResetBackColor()
		{
			throw new NotImplementedException("ResetBackColor");
		}

		[TODO]
		public void ResetForeColor()
		{
			throw new NotImplementedException("ResetForeColor");
		}

		[TODO]
		public void ResetGridLineColor()
		{
			throw new NotImplementedException("ResetGridLineColor");
		}

		[TODO]
		public void ResetHeaderBackColor()
		{
			throw new NotImplementedException("ResetHeaderBackColor");
		}

		[TODO]
		public void ResetHeaderFont()
		{
			throw new NotImplementedException("ResetHeaderFont");
		}

		[TODO]
		public void ResetHeaderForeColor()
		{
			throw new NotImplementedException("ResetHeaderForeColor");
		}

		[TODO]
		public void ResetLinkColor()
		{
			throw new NotImplementedException("ResetLinkColor");
		}

		[TODO]
		public void ResetLinkHoverColor()
		{
			throw new NotImplementedException("ResetLinkHoverColor");
		}

		[TODO]
		public void ResetSelectionBackColor()
		{
			throw new NotImplementedException("ResetSelectionBackColor");
		}

		[TODO]
		public void ResetSelectionForeColor()
		{
			throw new NotImplementedException("ResetSelectionForeColor");
		}

		[TODO]
		protected virtual bool ShouldSerializeAlternatingBackColor()
		{
			throw new NotImplementedException("ShouldSerializeAlternatingBackColor");
		}

		[TODO]
		protected bool ShouldSerializeBackColor()
		{
			throw new NotImplementedException("ShouldSerializeBackColor");
		}

		[TODO]
		protected bool ShouldSerializeForeColor()
		{
			throw new NotImplementedException("ShouldSerializeForeColor");
		}

		[TODO]
		protected virtual bool ShouldSerializeGridLineColor()
		{
			throw new NotImplementedException("ShouldSerializeGridLineColor");
		}

		[TODO]
		protected virtual bool ShouldSerializeHeaderBackColor()
		{
			throw new NotImplementedException("ShouldSerializeHeaderBackColor");
		}

		[TODO]
		protected virtual bool ShouldSerializeHeaderForeColor()
		{
			throw new NotImplementedException("ShouldSerializeHeaderForeColor");
		}

		[TODO]
		protected virtual bool ShouldSerializeLinkColor()
		{
			throw new NotImplementedException("ShouldSerializeLinkColor");
		}

		[TODO]
		protected virtual bool ShouldSerializeLinkHoverColor()
		{
			throw new NotImplementedException("ShouldSerializeLinkHoverColor");
		}

		[TODO]
		protected bool ShouldSerializePreferredRowHeight()
		{
			throw new NotImplementedException("ShouldSerializePreferredRowHeight");
		}

		[TODO]
		protected bool ShouldSerializeSelectionBackColor()
		{
			throw new NotImplementedException("ShouldSerializeSelectionBackColor");
		}

		[TODO]
		protected virtual bool ShouldSerializeSelectionForeColor()
		{
			throw new NotImplementedException("ShouldSerializeSelectionForeColor");
		}

		[TODO]
		public void add_AllowSortingChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_AllowSortingChanged");
		}

		[TODO]
		public void add_AlternatingBackColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_AlternatingBackColorChanged");
		}

		[TODO]
		public void add_BackColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_BackColorChanged");
		}

		[TODO]
		public void add_ColumnHeadersVisibleChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_ColumnHeadersVisibleChanged");
		}

		[TODO]
		public void add_ForeColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_ForeColorChanged");
		}

		[TODO]
		public void add_GridLineColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_GridLineColorChanged");
		}

		[TODO]
		public void add_GridLineStyleChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_GridLineStyleChanged");
		}

		[TODO]
		public void add_HeaderBackColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_HeaderBackColorChanged");
		}

		[TODO]
		public void add_HeaderFontChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_HeaderFontChanged");
		}

		[TODO]
		public void add_HeaderForeColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_HeaderForeColorChanged");
		}

		[TODO]
		public void add_LinkColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_LinkColorChanged");
		}

		[TODO]
		public void add_LinkHoverColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_LinkHoverColorChanged");
		}

		[TODO]
		public void add_MappingNameChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_MappingNameChanged");
		}

		[TODO]
		public void add_PreferredColumnWidthChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_PreferredColumnWidthChanged");
		}

		[TODO]
		public void add_PreferredRowHeightChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_PreferredRowHeightChanged");
		}

		[TODO]
		public void add_ReadOnlyChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_ReadOnlyChanged");
		}

		[TODO]
		public void add_RowHeaderWidthChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_RowHeaderWidthChanged");
		}

		[TODO]
		public void add_RowHeadersVisibleChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_RowHeadersVisibleChanged");
		}

		[TODO]
		public void add_SelectionBackColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_SelectionBackColorChanged");
		}

		[TODO]
		public void add_SelectionForeColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("add_SelectionForeColorChanged");
		}

		[TODO]
		public void remove_AllowSortingChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_AllowSortingChanged");
		}

		[TODO]
		public void remove_AlternatingBackColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_AlternatingBackColorChanged");
		}

		[TODO]
		public void remove_BackColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_BackColorChanged");
		}

		[TODO]
		public void remove_ColumnHeadersVisibleChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_ColumnHeadersVisibleChanged");
		}

		[TODO]
		public void remove_ForeColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_ForeColorChanged");
		}

		[TODO]
		public void remove_GridLineColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_GridLineColorChanged");
		}

		[TODO]
		public void remove_GridLineStyleChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_GridLineStyleChanged");
		}

		[TODO]
		public void remove_HeaderBackColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_HeaderBackColorChanged");
		}

		[TODO]
		public void remove_HeaderFontChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_HeaderFontChanged");
		}

		[TODO]
		public void remove_HeaderForeColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_HeaderForeColorChanged");
		}

		[TODO]
		public void remove_LinkColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_LinkColorChanged");
		}

		[TODO]
		public void remove_LinkHoverColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_LinkHoverColorChanged");
		}

		[TODO]
		public void remove_MappingNameChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_MappingNameChanged");
		}

		[TODO]
		public void remove_PreferredColumnWidthChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_PreferredColumnWidthChanged");
		}

		[TODO]
		public void remove_PreferredRowHeightChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_PreferredRowHeightChanged");
		}

		[TODO]
		public void remove_ReadOnlyChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_ReadOnlyChanged");
		}

		[TODO]
		public void remove_RowHeaderWidthChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_RowHeaderWidthChanged");
		}

		[TODO]
		public void remove_RowHeadersVisibleChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_RowHeadersVisibleChanged");
		}

		[TODO]
		public void remove_SelectionBackColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_SelectionBackColorChanged");
		}

		[TODO]
		public void remove_SelectionForeColorChanged(System.EventHandler value)
		{
			throw new NotImplementedException("remove_SelectionForeColorChanged");
		}

		public static System.Windows.Forms.DataGridTableStyle DefaultTableStyle;

		[TODO]
		public bool AllowSorting 
		{
 			get
			{
				return this._allowSorting;
			}

 			set
			{
				this._allowSorting = value;
			}

 		}

		[TODO]
		public System.Drawing.Color AlternatingBackColor 
		{
 			get
			{
				return this._alternatingBackColor;
			}

 			set
			{
				this._alternatingBackColor = value;
			}

 		}

		[TODO]
		public System.Drawing.Color BackColor 
		{
 			get
			{
				return this._backColor;
			}

 			set
			{
				this._backColor = value;
			}

 		}

		[TODO]
		public bool ColumnHeadersVisible 
		{
 			get
			{
				return this._columnHeadersVisible;
			}

 			set
			{
				this._columnHeadersVisible = value;
			}

 		}

		[TODO]
		public virtual System.Windows.Forms.DataGrid DataGrid 
		{
 			get
			{
				throw new NotImplementedException("DataGrid");
			}

 			set
			{
				throw new NotImplementedException("DataGrid");
			}

 		}

		[TODO]
		public System.Drawing.Color ForeColor 
		{
 			get
			{
				return this._foreColor;
			}

 			set
			{
				throw new NotImplementedException("ForeColor");
			}

 		}

		[TODO]
		public virtual GridColumnStylesCollection GridColumnStyles 
		{
 			get
			{
				throw new NotImplementedException("GridColumnStyles");
			}

 		}

		[TODO]
		public System.Drawing.Color GridLineColor 
		{
 			get
			{
				return this._gridLineColor;
			}

 			set
			{
				this._gridLineColor = value;
			}

 		}

		[TODO]
		public DataGridLineStyle GridLineStyle 
		{
 			get
			{
				throw new NotImplementedException("GridLineStyle");
			}

 			set
			{
				throw new NotImplementedException("GridLineStyle");
			}

 		}

		[TODO]
		public System.Drawing.Color HeaderBackColor 
		{
 			get
			{
				return this._headerBackColor;
			}

 			set
			{
				this._headerBackColor = value;
			}

 		}

		[TODO]
		public System.Drawing.Font HeaderFont 
		{
 			get
			{
				return this._headerFont;
			}

 			set
			{
				this._headerFont = value;
			}

 		}

		[TODO]
		public System.Drawing.Color HeaderForeColor 
		{
 			get
			{
				return this._headerForeColor;
			}

 			set
			{
				this._headerForeColor = value;
			}

 		}

		[TODO]
		public System.Drawing.Color LinkColor 
		{
 			get
			{
				return this._linkColor;
			}

 			set
			{
				this._linkColor = value;
			}

 		}

		[TODO]
		public System.Drawing.Color LinkHoverColor 
		{
 			get
			{
				return this._linkHoverColor;
			}

 			set
			{
				this._linkHoverColor = value;
			}

 		}

		[TODO]
		public System.String MappingName 
		{
 			get
			{
				return this._mappingName;
			}

 			set
			{
				this._mappingName = value;
			}

 		}

		[TODO]
		public int PreferredColumnWidth 
		{
 			get
			{
				return this._preferredColumnWidth;
			}

 			set
			{
				this._preferredColumnWidth = value;
			}

 		}

		[TODO]
		public int PreferredRowHeight 
		{
 			get
			{
				return this._preferredRowHeight;
			}

 			set
			{
				this._preferredRowHeight = value;
			}

 		}

		[TODO]
		public virtual bool ReadOnly 
		{
 			get
			{
				return this._readOnly;
			}

 			set
			{
				this._readOnly = value;
			}

 		}

		[TODO]
		public int RowHeaderWidth 
		{
 			get
			{
				return this._rowHeaderWidth;
			}

 			set
			{
				this._rowHeaderWidth = value;
			}

 		}

		[TODO]
		public bool RowHeadersVisible 
		{
 			get
			{
				return this._rowHeadersVisible;
			}

 			set
			{
				this._rowHeadersVisible = value;
			}

 		}

		[TODO]
		public System.Drawing.Color SelectionBackColor 
		{
 			get
			{
				return this._selectionBackColor;
			}

 			set
			{
				this._selectionBackColor = value;
			}

 		}

		[TODO]
		public System.Drawing.Color SelectionForeColor 
		{
 			get
			{
				return this._selectionForeColor;
			}

 			set
			{
				this._selectionForeColor = value;
			}

 		}

		public System.EventHandler AllowSortingChanged;

		public System.EventHandler AlternatingBackColorChanged;

		public System.EventHandler BackColorChanged;

		public System.EventHandler ColumnHeadersVisibleChanged;

		public System.EventHandler ForeColorChanged;

		public System.EventHandler GridLineColorChanged;

		public System.EventHandler GridLineStyleChanged;

		public System.EventHandler HeaderBackColorChanged;

		public System.EventHandler HeaderFontChanged;

		public System.EventHandler HeaderForeColorChanged;

		public System.EventHandler LinkColorChanged;

		public System.EventHandler LinkHoverColorChanged;

		public System.EventHandler MappingNameChanged;

		public System.EventHandler PreferredColumnWidthChanged;

		public System.EventHandler PreferredRowHeightChanged;

		public System.EventHandler ReadOnlyChanged;

		public System.EventHandler RowHeaderWidthChanged;

		public System.EventHandler RowHeadersVisibleChanged;

		public System.EventHandler SelectionBackColorChanged;

		public System.EventHandler SelectionForeColorChanged;

	}
}//namespace
