/*
 * PictureBox.cs - Implementation of the "System.Windows.Forms.PictureBox" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2003  Free Software Foundation, Inc.
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

[TODO]
public class PictureBox : Control
{
	[TODO]
	public PictureBox()
	{
	}

	[TODO]
	public override bool AllowDrop
	{
		get
		{
			throw new NotImplementedException("AllowDrop");
		}
		set
		{
			throw new NotImplementedException("AllowDrop");
		}
	}

	[TODO]
	public BorderStyle BorderStyle
	{
		get
		{
			throw new NotImplementedException("BorderStyle");
		}
		set
		{
			throw new NotImplementedException("BorderStyle");
		}
	}

	[TODO]
	public new bool CausesValidation
	{
		get
		{
			throw new NotImplementedException("CausesValidation");
		}
		set
		{
			throw new NotImplementedException("CausesValidation");
		}
	}

	protected override CreateParams CreateParams
	{
		get
		{
			return base.CreateParams;
		}
	}

	protected override ImeMode DefaultImeMode
	{
		get
		{
			return base.DefaultImeMode;
		}
	}

	protected override Size DefaultSize
	{
		get
		{
			return base.DefaultSize;
		}
	}

	public override Font Font
	{
		get
		{
			return base.Font;
		}
		set
		{
			base.Font = value;
		}
	}

	[TODO]
	public override Color ForeColor
	{
		get
		{
			throw new NotImplementedException("ForeColor");
		}
		set
		{
			throw new NotImplementedException("ForeColor");
		}
	}

	[TODO]
	public Image Image
	{
		get
		{
			throw new NotImplementedException("Image");
		}
		set
		{
			throw new NotImplementedException("Image");
		}
	}

	[TODO]
	public new ImeMode ImeMode
	{
		get
		{
			throw new NotImplementedException("ImeMode");
		}
		set
		{
			throw new NotImplementedException("ImeMode");
		}
	}

	public override RightToLeft RightToLeft
	{
		get
		{
			return base.RightToLeft;
		}
		set
		{
			base.RightToLeft = value;
		}
	}

	[TODO]
	public PictureBoxSizeMode SizeMode
	{
		get
		{
			throw new NotImplementedException("SizeMode");
		}
		set
		{
			throw new NotImplementedException("SizeMode");
		}
	}

	public new int TabIndex
	{
		get
		{
			return base.TabIndex;
		}
		set
		{
			base.TabIndex = value;
		}
	}

	public new bool TabStop
	{
		get
		{
			return base.TabStop;
		}
		set
		{
			base.TabStop = value;
		}
	}

	[TODO]
	public override string Text
	{
		get
		{
			throw new NotImplementedException("Text");
		}
		set
		{
			throw new NotImplementedException("Text");
		}
	}

	[TODO]
	protected override void Dispose(bool disposing)
	{
		throw new NotImplementedException("Dispose");
	}

	[TODO]
	protected override void OnEnabledChanged(EventArgs e)
	{
		throw new NotImplementedException("OnEnabledChanged");
	}

	[TODO]
	protected override void OnPaint(PaintEventArgs e)
	{
		throw new NotImplementedException("OnPaint");
	}

	[TODO]
	protected override void OnParentChanged(EventArgs e)
	{
		throw new NotImplementedException("OnParentChanged");
	}

	[TODO]
	protected override void OnResize(EventArgs e)
	{
		base.OnResize(e);
	}

	[TODO]
	protected virtual void OnSizeModeChanged(EventArgs e)
	{
		throw new NotImplementedException("OnSizeModeChanged");
	}

	[TODO]
	protected override void OnVisibleChanged(EventArgs e)
	{
		base.OnVisibleChanged(e);
	}

	protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
	{
		base.SetBoundsCore(x, y, width, height, specified);
	}

	[TODO]
	public override string ToString()
	{
		throw new NotImplementedException("ToString");
	}

	[TODO]
	public new event EventHandler CausesValidationChanged
	{
		add
		{
			throw new NotImplementedException("CausesValidationChanged");
		}
		remove
		{
			throw new NotImplementedException("CausesValidationChanged");
		}
	}

	[TODO]
	public new event EventHandler Enter
	{
		add
		{
			throw new NotImplementedException("Enter");
		}
		remove
		{
			throw new NotImplementedException("Enter");
		}
	}

	[TODO]
	public new event EventHandler FontChanged
	{
		add
		{
			throw new NotImplementedException("FontChanged");
		}
		remove
		{
			throw new NotImplementedException("FontChanged");
		}
	}

	[TODO]
	public new event EventHandler ForeColorChanged
	{
		add
		{
			throw new NotImplementedException("ForeColorChanged");
		}
		remove
		{
			throw new NotImplementedException("ForeColorChanged");
		}
	}

	[TODO]
	public new event EventHandler ImeModeChanged
	{
		add
		{
			throw new NotImplementedException("ImeModeChanged");
		}
		remove
		{
			throw new NotImplementedException("ImeModeChanged");
		}
	}

	[TODO]
	public new event KeyEventHandler KeyDown
	{
		add
		{
			throw new NotImplementedException("KeyDown");
		}
		remove
		{
			throw new NotImplementedException("KeyDown");
		}
	}

	[TODO]
	public new event KeyPressEventHandler KeyPress
	{
		add
		{
			throw new NotImplementedException("KeyPress");
		}
		remove
		{
			throw new NotImplementedException("KeyPress");
		}
	}

	[TODO]
	public new event KeyEventHandler KeyUp
	{
		add
		{
			throw new NotImplementedException("KeyUp");
		}
		remove
		{
			throw new NotImplementedException("KeyUp");
		}
	}

	[TODO]
	public new event EventHandler Leave
	{
		add
		{
			throw new NotImplementedException("Leave");
		}
		remove
		{
			throw new NotImplementedException("Leave");
		}
	}

	[TODO]
	public new event EventHandler RightToLeftChanged
	{
		add
		{
			throw new NotImplementedException("RightToLeftChanged");
		}
		remove
		{
			throw new NotImplementedException("RightToLeftChanged");
		}
	}

	[TODO]
	public event EventHandler SizeModeChanged
	{
		add
		{
			throw new NotImplementedException("SizeModeChanged");
		}
		remove
		{
			throw new NotImplementedException("SizeModeChanged");
		}
	}

	[TODO]
	public new event EventHandler TabIndexChanged
	{
		add
		{
			throw new NotImplementedException("TabIndexChanged");
		}
		remove
		{
			throw new NotImplementedException("TabIndexChanged");
		}
	}

	[TODO]
	public new event EventHandler TabStopChanged
	{
		add
		{
			throw new NotImplementedException("TabStopChanged");
		}
		remove
		{
			throw new NotImplementedException("TabStopChanged");
		}
	}

	[TODO]
	public new event EventHandler TextChanged
	{
		add
		{
			throw new NotImplementedException("TextChanged");
		}
		remove
		{
			throw new NotImplementedException("TextChanged");
		}
	}

}; // class PictureBox

}; // namespace System.Windows.Forms
