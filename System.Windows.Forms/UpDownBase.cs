/*
 * UpDownBase.cs - Implementation of the
 *			"System.Windows.Forms.UpDownBase" class.
 *
 * Copyright (C) 2003 Free Software Foundation
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

using System.Drawing;
using System.Windows.Forms.Themes;
using System.Diagnostics;

namespace System.Windows.Forms
{

[TODO]
public abstract class UpDownBase : ContainerControl
{
	internal enum ButtonID
	{
		None = 0,
		Up = 1,
		Down = 2
	};

	[TODO]
	internal class UpDownButtons : Control
	{
		private UpDownBase parent;

		[TODO]
		internal UpDownButtons(UpDownBase parent) : base()
		{
			this.parent = parent;
		}
 		
		[TODO]
		private void BeginButtonPress(MouseEventArgs e)
		{
			throw new NotImplementedException("BeginButtonPress");
		}

		[TODO]
		protected override AccessibleObject CreateAccessibilityInstance()
		{
			return base.CreateAccessibilityInstance();
		}
			
		[TODO]
		private void EndButtonPress()
		{
			throw new NotImplementedException("EndButtonPress");
		}

		[TODO]
		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown(e);
		}

		[TODO]
		protected override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove(e);
		}

		[TODO]
		protected override void OnMouseUp(MouseEventArgs e)
		{
			base.OnMouseUp(e);
		}

		[TODO]
		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

		[TODO]
		protected virtual void OnUpDown(UpDownEventArgs e)
		{
			throw new NotImplementedException("OnUpDown");
		}

		[TODO]
		protected void StartTimer()
		{
			throw new NotImplementedException("StartTimer");
		}

		[TODO]
		protected void StopTimer()
		{
			throw new NotImplementedException("StopTimer");
		}

		[TODO]
		private void TimerHandler(object source, EventArgs e)
		{
			throw new NotImplementedException("TimerHandler");
		}
		
		public event UpDownEventHandler UpDown;

	}; // class UpDownButtons

	internal class UpDownEdit : TextBox
	{
		private UpDownBase parent;
		private bool doubleClickFired;

		internal UpDownEdit(UpDownBase parent) : base()
		{
			this.parent = parent;
			doubleClickFired = false;
		}
		
		[TODO]
		protected override void OnGotFocus(EventArgs e)
		{
			base.OnGotFocus(e);
		}

		[TODO]
		protected override void OnKeyUp(KeyEventArgs e)
		{
			base.OnKeyUp(e);
		}
		
		[TODO]
		protected override void OnLostFocus(EventArgs e)
		{
			base.OnLostFocus(e);
		}

		[TODO]
		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown(e);
		}

		[TODO]
		protected override void OnMouseUp(MouseEventArgs e)
		{
			base.OnMouseUp(e);
		}
	}; // class UpDownEdit

	private UpDownButtons upDownButtons;
	private UpDownEdit upDownEdit;
	private bool userEdit;

	[TODO]
	public UpDownBase() : base()
	{
		upDownEdit = new UpDownEdit(this);
		upDownButtons = new UpDownButtons(this);
	}

	[TODO]
	public virtual void DownButton()
	{
		throw new NotImplementedException("DownButton");
	}

	[TODO]
	protected virtual void OnChanged(object source, EventArgs e)
	{
		throw new NotImplementedException("OnChanged");
	}

	[TODO]
	protected override void OnFontChanged(EventArgs e)
	{
		base.OnFontChanged(e);
	}

	[TODO]
	protected override void OnHandleCreated(EventArgs e)
	{
		base.OnHandleCreated(e);
	}

	[TODO]
	protected override void OnLayout(LayoutEventArgs e)
	{
		base.OnLayout(e);
	}

	[TODO]
	protected override void OnMouseWheel(MouseEventArgs e)
	{
		base.OnMouseWheel(e);
	}

	[TODO]
	protected virtual void OnTextBoxKeyDown(object source, KeyEventArgs e)
	{
		throw new NotImplementedException("OnTextBoxKeyDown");
	}

	[TODO]
	protected virtual void OnTextBoxKeyPress(object source, KeyPressEventArgs e)
	{
		throw new NotImplementedException("OnTextBoxKeyPress");
	}

	[TODO]
	protected virtual void OnTextBoxLostFocus(object source, EventArgs e)
	{
		throw new NotImplementedException("OnTextBoxLostFocus");
	}

	[TODO]
	protected virtual void OnTextBoxResize(object source, EventArgs e)
	{
		throw new NotImplementedException("OnTextBoxResize");
	}

	[TODO]
	protected virtual void OnTextBoxTextChanged(object source, EventArgs e)
	{
		throw new NotImplementedException("OnTextBoxTextChanged");
	}

	[TODO]
	public void Select(int start, int length)
	{
		throw new NotImplementedException("Select");
	}

	[TODO]
	protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
	{
		base.SetBoundsCore(x, y, width, height, specified);
	}

	[TODO]
	public virtual void UpButton()
	{
		throw new NotImplementedException("UpButton");
	}

	[TODO]
	protected virtual void UpdateEditText()
	{
		throw new NotImplementedException("UpdateEditText");
	}

	internal UpDownButtons UpDownButtonsInternal
	{
		get
		{
			return upDownButtons;
		}
	}

	[TODO]
	protected bool UserEdit
	{
		get
		{
			return userEdit;
		}
		set
		{
			userEdit = value;
		}
	}

	[TODO]
	protected override void Dispose(bool disposing)
	{
		if (disposing)
		{
			upDownEdit.Dispose();
			upDownButtons.Dispose();
		}
		
	}

	public new event EventHandler BackgroundImageChanged;

	public new event EventHandler MouseEnter;

	public new event EventHandler MouseHover;

	public new event EventHandler MouseLeave;

	public new event MouseEventHandler MouseMove;

	public event UpDownEventHandler UpDown
	{
		add { AddHandler(EventId.Scroll,value); }
		remove { RemoveHandler(EventId.Scroll,value); }
	}

}; // class UpDownBase
	
}; // namespace System.Windows.Forms

