/*
 * ScrollBar.cs - Implementation of the "System.Windows.Forms.ScrollBar" class.
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
using System.Windows.Forms.Themes;

public abstract class ScrollBar : Control
{
	// Fields
	private int minimum = 0;
	private int maximum = 100;
	private int value = 0;
	private int largeChange = 10;
	private int smallChange = 1;
	private Rectangle bar = Rectangle.Empty;
	private Rectangle track = Rectangle.Empty;
	private Rectangle increment = Rectangle.Empty;
	private Rectangle decrement = Rectangle.Empty;
	private bool incDown = false;
	private bool decDown = false;
	private bool trackDown = false;
	private int barDown = -1;
	private bool keyDown = false;
	private Timer timer;
	private int delay = 0;
	private int mouseX; // TODO: use Control.MousePosition
	private int mouseY; // TODO: use Control.MousePosition
	private Timer keyTimer;
	internal bool vertical;

	private const int startDelay = 3;

	private Timer idleTimer;
	private MouseEventArgs idleMouse;

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

	// Constructors
	public ScrollBar() : base()
	{
		base.TabStop = false;
		timer = new Timer();
		timer.Interval = 100;
		keyTimer = new Timer(); // keep key and mouse events from...
		timer.Interval = 100; // ...stepping on each other's toes
		idleTimer = new Timer();
		idleTimer.Tick+=new EventHandler(idleTimer_Tick);
		idleTimer.Interval = 1;
	}



	// Properties
	public override Color BackColor
	{
		get { return base.BackColor; }
		set
		{
			if (value == base.BackColor) { return; }
			base.BackColor = value;
			Redraw(false);
		}
	}

	public override Image BackgroundImage
	{
		get { return base.BackgroundImage; }
		set
		{
			if (value == base.BackgroundImage) { return; }
			base.BackgroundImage = value;
			Redraw(false);
		}
	}

	protected override CreateParams CreateParams
	{
		get { return base.CreateParams; }
	}

	private bool DecrementDown
	{
		get { return decDown; }
		set
		{
			if (value == decDown) { return; }
			decDown = value;
			if (value)
			{
				timer.Tick += new EventHandler(Decrement);
				delay = startDelay;
				timer.Start();
			}
			else
			{
				delay = 0;
				timer.Stop();
				timer.Tick -= new EventHandler(Decrement);
			}
		}
	}

	protected override ImeMode DefaultImeMode
	{
		get { return ImeMode.Disable; }
	}

	public override Font Font
	{
		get { return base.Font; }
		set { base.Font = value; }
	}

	public override Color ForeColor
	{
		get { return base.ForeColor; }
		set
		{
			if (value == base.ForeColor) { return; }
			base.ForeColor = value;
			Redraw(false);
		}
	}

	public new ImeMode ImeMode
	{
		get { return base.ImeMode; }
		set { base.ImeMode = value; }
	}

	private bool IncrementDown
	{
		get { return incDown; }
		set
		{
			if (value == incDown) { return; }
			incDown = value;
			if (value)
			{
				timer.Tick += new EventHandler(Increment);
				delay = startDelay;
				timer.Start();
			}
			else
			{
				delay = 0;
				timer.Stop();
				timer.Tick -= new EventHandler(Increment);
			}
		}
	}

	// Value for large scroll jumps
	public int LargeChange
	{
		get { return largeChange; }
		set
		{
			if (value < 0)
			{
				throw new ArgumentOutOfRangeException(/* TODO */);
			}
			
			if(value == largeChange)
				return;
			
			largeChange = value;
			
			Redraw();
		}
	}

	// Maximum value of scroll
	public int Maximum
	{
		get { return maximum; }
		set
		{
			if (value < minimum)
			{
				throw new ArgumentOutOfRangeException(/* TODO */);
			}
			if (value == maximum) { return; }
			maximum = value;
			Redraw(true);
		}
	}

	// Minimum value of scroll
	public int Minimum
	{
		get { return minimum; }
		set
		{
			if (value > maximum)
			{
				throw new ArgumentOutOfRangeException(/* TODO */);
			}
			if (value == minimum) { return; }
			minimum = value;
			Redraw(true);
		}
	}

	public int SmallChange
	{
		get { return smallChange; }
		set
		{
			if (value < 0)
			{
				throw new ArgumentOutOfRangeException(/* TODO */);
			}
			smallChange = value;
		}
	}

	public override string Text
	{
		get { return base.Text; }
		set { base.Text = value; }
	}

	public int Value
	{
		get { return value; }
		set
		{
			if (value > maximum || value < minimum)
			{
				throw new ArgumentOutOfRangeException(/* TODO */);
			}
			if (value == this.value) { return; }

			using (Graphics g = CreateGraphics())
			{
				Rectangle invalid = bar;
				this.value = value;
				SetPositionByValue();
				// Remove old
				Draw(g, false, invalid);
				// Draw bar
				Draw(g, false, bar); //BRL:Redraw??
			}
			OnValueChanged(new EventArgs());
		}
	}



	// Methods
	private void Decrement(Object sender, EventArgs e)
	{
		if (delay > 0)
		{
			--delay;
			if (delay > 0)
			{
				Redraw(false);
				return;
			}
		}

		int tmp = (value-smallChange);
		tmp = tmp > minimum ?
		      tmp : minimum;
		if (value != tmp)
		{
			value = tmp;
			OnValueChanged(new EventArgs());
			OnScroll(new ScrollEventArgs(this,ScrollEventType.SmallDecrement,value));
			SetPositionByValue();
		}
		Redraw(false);
	}

	private void DecrementBig(Object sender, EventArgs e)
	{
		if (delay > 0)
		{
			--delay;
			if (delay > 0)
			{
				Redraw(false);
				return;
			}
		}

		int tmp = (value-largeChange);
		tmp = tmp > minimum ?
		      tmp : minimum;
		if (value == tmp) { return; }
		if (trackDown)
		{
			if (vertical)
			{
				if (mouseY > bar.Y)
				{
					TrackPressed(false,false);
					return;
				}
			}
			else
			{
				if (mouseX > bar.X)
				{
					TrackPressed(false,false);
					return;
				}
			}
		}
		value = tmp;
		OnValueChanged(new EventArgs());
		OnScroll(new ScrollEventArgs(this,ScrollEventType.LargeDecrement,value));
		SetPositionByValue();
		Redraw(false);
	}

	private void Draw(Graphics g, bool layout, Rectangle drawBounds)
	{
		if (!Visible || !IsHandleCreated) { return; }

		if (layout)
		{
			if (vertical)
			{
				LayoutElementsV(ClientSize);
			}
			else
			{
				LayoutElementsH(ClientSize);
			}
		}

		ThemeManager.MainPainter.DrawScrollBar(g,
											   ClientRectangle,
											   drawBounds,
											   ForeColor,BackColor,
											   vertical,Enabled,
											   bar, track,
											   decrement,decDown,
											   increment,incDown);
	}

	private void Increment(Object sender, EventArgs e)
	{
		if (delay > 0)
		{
			--delay;
			if (delay > 0)
			{
				Redraw(false);
				return;
			}
		}

		int v = value + smallChange;
		int v1 = maximum - largeChange + 1;
		if (v1 < v)
			v = v1;
		if (value != v)
		{
			value = v;
			OnValueChanged(new EventArgs());
			OnScroll(new ScrollEventArgs(this,ScrollEventType.SmallIncrement,value));
			SetPositionByValue();
		}
		Redraw(false);
	}

	private void IncrementBig(Object sender, EventArgs e)
	{
		if (delay > 0)
		{
			--delay;
			if (delay > 0)
			{
				Redraw(false);
				return;
			}
		}

		int tmp = (value+largeChange);
		int tmp2 = (maximum-largeChange);
		tmp = tmp < tmp2 ?
		      tmp : tmp2;
		if (value == tmp) { return; }
		if (trackDown)
		{
			if (vertical)
			{
				if (mouseY-bar.Height < bar.Y)
				{
					TrackPressed(false,false);
					return;
				}
			}
			else
			{
				if (mouseX-bar.Width < bar.X)
				{
					TrackPressed(false,false);
					return;
				}
			}
			
		}
		value = tmp;
		OnValueChanged(new EventArgs());
		OnScroll(new ScrollEventArgs(this,ScrollEventType.LargeIncrement,value));
		SetPositionByValue();
		Redraw(false);
	}

	// Sets up the layout rectangles for a HScrollBar's elements
	private void LayoutElementsH(Size s)
	{
		LayoutElementsV(new Size(s.Height, s.Width));
		decrement = SwapRectValues(decrement);
		increment = SwapRectValues(increment);
		track = SwapRectValues(track);
		bar = SwapRectValues(bar);
		if (RightToLeft == RightToLeft.Yes)
		{
			int offset = bar.X-track.X;
			int guiMax = track.Width-bar.Width;

			bar.X = guiMax-offset;
		}
	}

	// Sets up the layout rectangles for a VScrollBar's elements
	private void LayoutElementsV(Size s)
	{
		int trackHeight, thumbHeight, thumbPos, zeroMax, zeroVal;
		double percentage;
		
		// Track
		trackHeight = s.Height - 2 * s.Width;
		this.track  = new Rectangle(0, s.Width, s.Width, trackHeight);
		
		// Decrement and increment buttons
		// Is there enough room to fit both buttons at their
		// preferred size?
		if(s.Height >= s.Width * 2)
		{
			this.decrement = new Rectangle(0, 0, s.Width, s.Width);
			this.increment = new Rectangle(0, s.Width + trackHeight, 
			                               s.Width, s.Width);
		}
		else
		{
			// No.  Split what's left.
			this.decrement = new Rectangle(0, 0, s.Width, s.Height / 2);
			this.increment = new Rectangle(0, s.Height / 2, 
			                               s.Width, s.Height / 2);
		}
		
		// Thumb.
		zeroMax     = this.maximum - this.minimum;
		zeroVal     = this.value - this.minimum;
		percentage  = (double) this.largeChange / zeroMax;
		thumbHeight = (int)( percentage * trackHeight );
		percentage  = (double) zeroVal / zeroMax;
		thumbPos    = (int)( percentage * trackHeight );
		thumbPos    = Math_Min(thumbPos, trackHeight - thumbHeight);
		this.bar    = new Rectangle(0, s.Width + thumbPos, 
		                            s.Width, thumbHeight);
	}

	protected override void OnEnabledChanged(EventArgs e)
	{
		Redraw(false);
		base.OnEnabledChanged(e);
	}

	protected override void OnHandleCreated(EventArgs e)
	{
		base.OnHandleCreated(e);
	}

	protected override void OnKeyDown(KeyEventArgs e)
	{
		if (keyDown) { return; }

		switch (e.KeyCode)
		{
			case Keys.PageUp:
			{
				if (vertical)
				{
					DecrementBig(null,null);
					ScrollKeyPressed(true,-2);
				}
			}
			break;

			case Keys.PageDown:
			{
				if (vertical)
				{
					IncrementBig(null,null);
					ScrollKeyPressed(true,2);
				}
			}
			break;

			case Keys.Home:
			{
				if (!vertical)
				{
					DecrementBig(null,null);
					ScrollKeyPressed(true,-2);
				}
			}
			break;

			case Keys.End:
			{
				if (!vertical)
				{
					IncrementBig(null,null);
					ScrollKeyPressed(true,2);
				}
			}
			break;

			case Keys.Up:
			{
				if (vertical)
				{
					Decrement(null,null);
					ScrollKeyPressed(true,-1);
				}
			}
			break;

			case Keys.Down:
			{
				if (vertical)
				{
					Increment(null,null);
					ScrollKeyPressed(true,1);
				}
			}
			break;

			case Keys.Left:
			{
				if (!vertical)
				{
					Decrement(null,null);
					ScrollKeyPressed(true,-1);
				}
			}
			break;

			case Keys.Right:
			{
				if (!vertical)
				{
					Increment(null,null);
					ScrollKeyPressed(true,1);
				}
			}
			break;
		}
	}

	protected override void OnKeyUp(KeyEventArgs e)
	{
		if (!keyDown) { return; }

		barDown = -1;

		switch (e.KeyCode)
		{
			case Keys.PageUp:
			case Keys.Home:
			{
				ScrollKeyPressed(false,2);
			}
			break;

			case Keys.PageDown:
			case Keys.End:
			{
				ScrollKeyPressed(false,-2);
			}
			break;

			case Keys.Up:
			case Keys.Left:
			{
				ScrollKeyPressed(false,1);
			}
			break;

			case Keys.Down:
			case Keys.Right:
			{
				ScrollKeyPressed(false,-1);
			}
			break;
		}
	}

	protected override void OnMouseDown(MouseEventArgs e)
	{
		Capture = true;
		int x = e.X;
		int y = e.Y;

		mouseX = x;
		mouseY = y;

		if (increment.Contains(x,y))
		{
			Increment(null,null);
			IncrementDown = true;
			Redraw();
		}
		else if (decrement.Contains(x,y))
		{
			Decrement(null,null);
			DecrementDown = true;
			Redraw();
		}
		else if (bar.Contains(x,y))
		{
			barDown = vertical ? y : x;
		}
		else if (track.Contains(x,y))
		{
			if (vertical)
			{
				if (y >= bar.Bottom)
				{
					IncrementBig(null,null);
					TrackPressed(true,true);
				}
				else // y <= bar.Top
				{
					DecrementBig(null,null);
					TrackPressed(true,false);
				}
			}
			else
			{
				bool plus = (x >= bar.Right);
				plus ^= (RightToLeft == RightToLeft.Yes);
				if (plus)
				{
					IncrementBig(null,null);
					TrackPressed(true,true);
				}
				else
				{
					DecrementBig(null,null);
					TrackPressed(true,false);
				}
			}
		}
		base.OnMouseDown(e);
	}

	protected override void OnMouseLeave(EventArgs e)
	{
		bool redraw = (incDown || decDown);

		if (incDown)
		{
			IncrementDown = false;
		}
		else if (decDown)
		{
			DecrementDown = false;
		}
		else if (trackDown)
		{
			TrackPressed(false,false);
		}

		if (redraw) { Redraw(false); }
		base.OnMouseLeave(e);
	}

	protected override void OnMouseEnter(EventArgs e)
	{
		base.OnMouseEnter (e);
	}


	protected override void OnMouseMove(MouseEventArgs e)
	{
		idleMouse = e;
		// Do the actual event when the events have caught up.
		idleTimer.Start();
	}

	private void OnMouseMoveActual(MouseEventArgs e)
	{
		int x = e.X;
		int y = e.Y;

		mouseX = x;
		mouseY = y;

		if (incDown)
		{
			IncrementDown = increment.Contains(x,y);
		}
		else if (decDown)
		{
			DecrementDown = decrement.Contains(x,y);
		}
		else if (barDown != -1)
		{
			if (vertical)
			{
				int bY = bar.Y;
				int tY = track.Y;
				int bHeight = bar.Height;
				int tHeight = track.Height;
				int guiMax = tY+tHeight-bHeight;
				int newPos = bY+y-barDown;
				if (newPos < tY)
				{
					newPos = tY;
				}
				else if (newPos > guiMax)
				{
					newPos = guiMax;
				}
				if (newPos != bY)
				{
					barDown = y;
					Rectangle invalid;
					// What area needs to be drawn with the background?
					if (newPos > bar.Y)
						invalid = new Rectangle(bar.X, bar.Y, bar.Width, newPos - bar.Y);
					else
						invalid = new Rectangle(bar.X, newPos + bar.Height, bar.Width, bar.Y - newPos);
					bar.Y = newPos;
					SetValueByPosition();
					using (Graphics g = CreateGraphics())
					{
						// Draw background
						Draw(g, false, invalid);
						// Draw bar
						Draw(g, false, bar);
					}
					OnValueChanged(new EventArgs());
					OnScroll(new ScrollEventArgs(this,ScrollEventType.ThumbTrack,value));
					
				}
				else
				{
					// if the mouse has gone too far up or
					// down and we're still tracking it,
					// make sure when it comes back that
					// it's tracked by the center of the
					// scroll bar
					barDown = bY+bHeight/2;
				}
			}
			else
			{
				int tX = track.X;
				int bWidth = bar.Width;
				int tWidth = track.Width;
				int guiMax = tX+tWidth-bWidth;
				int newPos = bar.X+x-barDown;
				if (newPos < tX)
				{
					newPos = tX;
				}
				else if (newPos > guiMax)
				{
					newPos = guiMax;
				}
				if (newPos != bar.X)
				{
					barDown = x;
					Rectangle invalid;
					// What area needs to be drawn with the background?
					if (newPos > bar.X)
						invalid = new Rectangle(bar.X, bar.Y, newPos - bar.X, bar.Height);
					else
						invalid = new Rectangle(newPos + bar.Width, bar.Y, bar.X - newPos, bar.Height);
					bar.X = newPos;
					SetValueByPosition();
					Redraw(false);
					OnValueChanged(new EventArgs());
					OnScroll(new ScrollEventArgs(this,ScrollEventType.ThumbTrack,value));
					
				}
				else
				{
					// if the mouse has gone too far right
					// or left and we're still tracking it,
					// make sure when it comes back that
					// it's tracked by the center of the
					// scroll bar
					barDown = bar.X + bWidth/2;
				}
			}
		}
		base.OnMouseMove(e);
	}

	protected override void OnMouseUp(MouseEventArgs e)
	{
		Capture = false;
		mouseX = e.X;
		mouseY = e.Y;

		bool redraw = (incDown || decDown);
		if (incDown)
		{
			IncrementDown = false;
			Redraw();
		}
		else if (decDown)
		{
			DecrementDown = false;
			Redraw();
		}
		else if (trackDown)
		{
			TrackPressed(false,false);
		}
		else if (barDown != -1)
		{
			barDown = -1;
			ScrollEventType type;
			if (value == minimum)
			{
				type = ScrollEventType.First;
			}
			else if (value == maximum-largeChange)
			{
				type = ScrollEventType.Last;
			}
			else
			{
				type = ScrollEventType.ThumbPosition;
			}
			OnScroll(new ScrollEventArgs(this,type,value));
		}

		if (redraw)
		{
			Redraw(false);
		}
		base.OnMouseUp(e);
	}

	protected override void OnPaint(PaintEventArgs e)
	{
		Draw(e.Graphics,true, e.ClipRectangle);
		base.OnPaint(e);
	}

	protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
	{
		base.SetBoundsCore (x, y, width, height, specified);
		Redraw(true);
	}


	protected virtual void OnScroll(ScrollEventArgs e)
	{
		ScrollEventHandler handler;
		handler = (ScrollEventHandler)(GetHandler(EventId.Scroll));
		if (handler != null)
		{
			handler(this,e);
		}
	}

	protected virtual void OnValueChanged(EventArgs e)
	{
		EventHandler handler;
		handler = (EventHandler)(GetHandler(EventId.ValueChanged));
		if (handler != null)
		{
			handler(this,e);
		}
	}

	private void Redraw()
	{
		Redraw(true);
	}

	// Redraw the scrollbar after a state change. Relayout if necessary
	private void Redraw(bool layout)
	{
		// Bail out if the scrollbar is not currently visible.
		if (!Visible || !IsHandleCreated) { return; }
		// Redraw the scrollbar.
		using (Graphics graphics = CreateGraphics())
		{
			Draw(graphics,layout, ClientRectangle);
		}
	}

	private void ScrollKeyPressed(bool pressed, int amount)
	{
		if (pressed == keyDown) { return; }
		keyDown = pressed;
		if (pressed)
		{
			switch (amount)
			{
				case 2:
				{
					timer.Tick += new EventHandler(IncrementBig);
				}
				break;

				case -2:
				{
					timer.Tick += new EventHandler(DecrementBig);
				}
				break;

				case 1:
				{
					timer.Tick += new EventHandler(Increment);
				}
				break;

				case -1:
				{
					timer.Tick += new EventHandler(Decrement);
				}
				break;
			}
			delay = startDelay;
			timer.Start();
		}
		else
		{
			delay = 0;
			timer.Stop();
			timer.Tick -= new EventHandler(IncrementBig);
			timer.Tick -= new EventHandler(DecrementBig);
			timer.Tick -= new EventHandler(Increment);
			timer.Tick -= new EventHandler(Decrement);
		}
	}

	private void SetPositionByValue()
	{
		Size cs = this.ClientSize;
		
		if(vertical)
			LayoutElementsV(cs);
		else
			LayoutElementsH(cs);
	}

	private void SetValueByPosition()
	{
		if(vertical)
		{
			int position = bar.Y - track.Y;
			double percentage = (double) position / track.Height;
			value = (int)(percentage * (maximum - minimum) + minimum);
		}
		else
		{
			int position = bar.X - track.X;
			double percentage = (double) position / track.Width;
			value = (int)(percentage * (maximum - minimum));
			
			if(RightToLeft == RightToLeft.Yes)
			{
				int guiMax = (maximum - largeChange - minimum);
				value = guiMax - value;
			}
			
			value += minimum;
		}
		
		value = Math_Max(minimum, value);
		value = Math_Min(maximum, value);
	}
	private static Rectangle SwapRectValues(Rectangle rect)
	{
		return new Rectangle(rect.Y,rect.X,rect.Height,rect.Width);
	}

	public override string ToString()
	{
		return base.ToString() + ", Minimum: " + minimum + ", Maximum: " +
			maximum + ", Value: " + value;
	}

	private void TrackPressed(bool pressed, bool plus)
	{
		if (pressed == trackDown) { return; }
		trackDown = pressed;
		if (pressed)
		{
			if (plus)
			{
				timer.Tick += new EventHandler(IncrementBig);
			}
			else
			{
				timer.Tick += new EventHandler(DecrementBig);
			}
			delay = startDelay;
			timer.Start();
		}
		else
		{
			delay = 0;
			timer.Stop();
			timer.Tick -= new EventHandler(IncrementBig);
			timer.Tick -= new EventHandler(DecrementBig);
		}
	}

#if !CONFIG_COMPACT_FORMS
	protected override void WndProc(ref Message m)
	{
		base.WndProc(ref m);
	}
#endif // !CONFIG_COMPACT_FORMS

	public event ScrollEventHandler Scroll
	{
		add { AddHandler(EventId.Scroll,value); }
		remove { RemoveHandler(EventId.Scroll,value); }
	}

	public event EventHandler ValueChanged
	{
		add { AddHandler(EventId.ValueChanged,value); }
		remove { RemoveHandler(EventId.ValueChanged,value); }
	}

	private void idleTimer_Tick(object sender, EventArgs e)
	{
		idleTimer.Stop();
		OnMouseMoveActual(idleMouse);
	}
}; // class ScrollBar

}; // namespace System.Windows.Forms
