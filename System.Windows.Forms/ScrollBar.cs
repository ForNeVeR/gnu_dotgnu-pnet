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
	private Timer keyTimer;
	internal bool vertical;



	// Constructors
	public ScrollBar() : base()
	{
		base.TabStop = false;
		timer = new Timer();
		timer.Interval = 100;
		keyTimer = new Timer(); // keep key and mouse events from...
		timer.Interval = 100; // ...stepping on each other's toes
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
				timer.Start();
			}
			else
			{
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
				timer.Start();
			}
			else
			{
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
			largeChange = value;
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
			Redraw();
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
			Redraw();
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
			this.value = value;
			OnValueChanged(new EventArgs());
		}
	}



	// Methods
	private void Decrement(Object sender, EventArgs e)
	{
		int tmp = (value-smallChange);
		tmp = tmp > minimum ?
		      tmp : minimum;
		if (value != tmp)
		{
			Value = tmp;
			OnScroll(new ScrollEventArgs(this,ScrollEventType.SmallDecrement,value));
			SetPositionByValue();
		}
		Redraw(false);
	}

	private void DecrementBig(Object sender, EventArgs e)
	{
		int tmp = (value-largeChange);
		tmp = tmp > minimum ?
		      tmp : minimum;
		if (value == tmp) { return; }
		Value = tmp;
		OnScroll(new ScrollEventArgs(this,ScrollEventType.LargeDecrement,value));
		SetPositionByValue();
		Redraw(false);
	}

	private void Draw(Graphics g, bool layout)
	{
		if (!Visible || !IsHandleCreated) { return; }

		Size clientSize = ClientSize;
		int x = 0;
		int y = 0;
		int width = clientSize.Width;
		int height = clientSize.Height;

		Rectangle bounds = new Rectangle(x,y,width,height);

		if (layout)
		{
			if (vertical)
			{
				LayoutElementsV(x,y,width,height);
			}
			else
			{
				LayoutElementsH(x,y,width,height);
			}
		}

		using (Brush bgBrush = CreateBackgroundBrush())
		{
			ThemeManager.MainPainter.DrawScrollBar(g,
			                                       bounds,
			                                       ForeColor,BackColor,
			                                       bgBrush,
				                               vertical,Enabled,
			                                       bar, track,
			                                       decrement,decDown,
				                               increment,incDown);
		}
	}

	private void Increment(Object sender, EventArgs e)
	{
		int tmp = (value+smallChange);
		int tmp2 = (maximum-largeChange+1);
		tmp = tmp < tmp2 ?
		      tmp : tmp2;
		if (value != tmp)
		{
			Value = tmp;
			OnScroll(new ScrollEventArgs(this,ScrollEventType.SmallIncrement,value));
			SetPositionByValue();
		}
		Redraw(false);
	}

	private void IncrementBig(Object sender, EventArgs e)
	{
		int tmp = (value+largeChange);
		int tmp2 = (maximum-largeChange+1);
		tmp = tmp < tmp2 ?
		      tmp : tmp2;
		if (value == tmp) { return; }
		Value = tmp;
		OnScroll(new ScrollEventArgs(this,ScrollEventType.LargeIncrement,value));
		SetPositionByValue();
		Redraw(false);
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
			timer.Start();
		}
		else
		{
			timer.Stop();
			timer.Tick -= new EventHandler(IncrementBig);
			timer.Tick -= new EventHandler(DecrementBig);
			timer.Tick -= new EventHandler(Increment);
			timer.Tick -= new EventHandler(Decrement);
		}
	}

	// Sets up the layout rectangles for a HScrollBar's elements
	private void LayoutElementsH(int x, int y, int width, int height)
	{
		LayoutElementsV(y,x,height,width);
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
	private void LayoutElementsV(int x, int y, int width, int height)
	{
		int guiMax = (1+maximum-largeChange-minimum);
		int scrolls = (maximum-minimum)/largeChange;
		int position = (value-minimum) < guiMax ?
		               (value-minimum) : guiMax;

		// layout rectangle for decrement button
		decrement = new Rectangle(x,y,width,width);
		y += width; // increase by decrement size
		height -= width;

		// layout rectangle for the track of the scroll bar
		int trackStart = y;
		int trackRange = height-width;
		track = new Rectangle(x,trackStart,width,trackRange);
		y += trackRange; // increase by track length
		height = width;

		// layout rectangle for increment button
		increment = new Rectangle(x,y,width,width);

		// determine the size of the scrollbar, determine the maximum
		// position of the scroll bar on the track, translate the Value
		// range to the track's visual range, and layout the scroll bar
		int scrollSize = trackRange/scrolls;
		int trackMax = trackRange-scrollSize;
		int scrollPos = (trackMax*position)/guiMax;
		scrollPos = trackMax < scrollPos ?
		            trackMax : scrollPos;
		scrollPos += trackStart;
		bar = new Rectangle(x,scrollPos,width,scrollSize);
	}

	protected override void OnEnabledChanged(EventArgs e)
	{
		Redraw();
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
				IncrementBig(null,null);
				ScrollKeyPressed(true,2);
			}
			break;

			case Keys.PageDown:
			{
				DecrementBig(null,null);
				ScrollKeyPressed(true,-2);
			}
			break;

			case Keys.Up:
			{
				Increment(null,null);
				ScrollKeyPressed(true,1);
			}
			break;

			case Keys.Down:
			{
				Decrement(null,null);
				ScrollKeyPressed(true,-1);
			}
			break;
		}
	}

	protected override void OnKeyUp(KeyEventArgs e)
	{
		if (!keyDown) { return; }

		switch (e.KeyCode)
		{
			case Keys.PageUp:
			{
				ScrollKeyPressed(false,2);
			}
			break;

			case Keys.PageDown:
			{
				ScrollKeyPressed(false,-2);
			}
			break;

			case Keys.Up:
			{
				ScrollKeyPressed(false,1);
			}
			break;

			case Keys.Down:
			{
				ScrollKeyPressed(false,-1);
			}
			break;
		}
	}

	protected override void OnMouseDown(MouseEventArgs e)
	{
		int x = e.X;
		int y = e.Y;
		if (increment.Contains(x,y))
		{
			Increment(null,null);
			IncrementDown = true;
		}
		else if (decrement.Contains(x,y))
		{
			Decrement(null,null);
			DecrementDown = true;
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

	protected override void OnMouseMove(MouseEventArgs e)
	{
		int x = e.X;
		int y = e.Y;
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
					bar.Y = newPos;
					OnScroll(new ScrollEventArgs(this,ScrollEventType.ThumbTrack,value));
					SetValueByPosition();
					Redraw(false);
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
				int bX = bar.X;
				int tX = track.X;
				int bWidth = bar.Width;
				int tWidth = track.Width;
				int guiMax = tX+tWidth-bWidth;
				int newPos = bX+x-barDown;
				if (newPos < tX)
				{
					newPos = tX;
				}
				else if (newPos > guiMax)
				{
					newPos = guiMax;
				}
				if (newPos != bX)
				{
					barDown = x;
					bar.X = newPos;
					OnScroll(new ScrollEventArgs(this,ScrollEventType.ThumbTrack,value));
					SetValueByPosition();
					Redraw(false);
				}
				else
				{
					// if the mouse has gone too far right
					// or left and we're still tracking it,
					// make sure when it comes back that
					// it's tracked by the center of the
					// scroll bar
					barDown = bX+bWidth/2;
				}
			}
		}
		base.OnMouseMove(e);
	}

	protected override void OnMouseUp(MouseEventArgs e)
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
		else if (barDown != -1)
		{
			barDown = -1;
			ScrollEventType type;
			if (value == minimum)
			{
				type = ScrollEventType.First;
			}
			else if (value == maximum-largeChange+1)
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
		Draw(e.Graphics,true);
		base.OnPaint(e);
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

	// Redraw the scrollbar after a state change.
	private void Redraw()
	{
		// Bail out if the scrollbar is not currently visible.
		if (!Visible || !IsHandleCreated) { return; }

		// Redraw the scrollbar.
		using (Graphics graphics = CreateGraphics())
		{
			Draw(graphics,true);
		}
	}
	private void Redraw(bool layout)
	{
		// Bail out if the scrollbar is not currently visible.
		if (!Visible || !IsHandleCreated) { return; }

		// Redraw the scrollbar.
		using (Graphics graphics = CreateGraphics())
		{
			Draw(graphics,layout);
		}
	}

	private void SetPositionByValue()
	{
		int guiMax = (1+maximum-largeChange-minimum);
		int position = (value-minimum) < guiMax ?
		               (value-minimum) : guiMax;
		if (vertical)
		{
			int trackMax = track.Height-bar.Height;
			int trackStart = track.Y;
			int scrollPos = (trackMax*position)/guiMax;
			scrollPos = trackMax < scrollPos ?
			            trackMax : scrollPos;
			scrollPos += trackStart;
			bar.Y = scrollPos;
		}
		else
		{
			int trackMax = track.Width-bar.Width;
			int trackStart = track.X;
			int scrollPos = (trackMax*position)/guiMax;
			scrollPos = trackMax < scrollPos ?
			            trackMax : scrollPos;
			scrollPos += trackStart;
			bar.X = scrollPos;
		}
	}

	private void SetValueByPosition()
	{
		int guiMax = (1+maximum-largeChange-minimum);
		if (vertical)
		{
			int trackMax = track.Height-bar.Height;
			int position = bar.Y-track.Y;
			int newValue = (guiMax*position)/trackMax;
			newValue = guiMax < newValue ?
			           guiMax : newValue;
			newValue += minimum;
			Value = newValue;
		}
		else
		{
			int trackMax = track.Width-bar.Width;
			int position = bar.X-track.X;
			int newValue = (guiMax*position)/trackMax;
			newValue = guiMax < newValue ?
			           guiMax : newValue;
			if (RightToLeft == RightToLeft.Yes)
			{
				newValue = guiMax-newValue;
			}
			newValue += minimum;
			Value = newValue;
		}
	}
	private static Rectangle SwapRectValues(Rectangle rect)
	{
		return new Rectangle(rect.Y,rect.X,rect.Height,rect.Width);
	}

	[TODO]
	public override string ToString()
	{
		// not sure if this is right
		return base.ToString() + ", Value: " + value;
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
			timer.Start();
		}
		else
		{
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

}; // class ScrollBar

}; // namespace System.Windows.Forms
