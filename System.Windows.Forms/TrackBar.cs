/*
 * TrackBar.cs - Implementation of the
 *			"System.Windows.Forms.TrackBar" class.
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
#if CONFIG_COMPONENT_MODEL
	using System.ComponentModel;
#endif
	using System.Drawing;
	public class TrackBar : Control
#if CONFIG_COMPONENT_MODEL
		, ISupportInitialize
#endif
	{
		private bool autoSize;
		private int largeChange;
		private int maximum;
		private int minimum;
		private Orientation orientation;
		private int smallChange;
		private int tickFrequency;
		private TickStyle tickStyle;
		private bool initializing;
		private int dimension;
		private int value;
		public event EventHandler Scroll;
		public event EventHandler ValueChanged;

		public TrackBar()
		{
			autoSize = true;
			largeChange = 5;
			maximum = 10;
			smallChange = 1;
			tickFrequency = 1;
			tickStyle = TickStyle.BottomRight;
			orientation = Orientation.Horizontal;
		}

		public bool AutoSize
		{
			get
			{
				return autoSize;
			}

			set
			{
				if (autoSize != value)
				{
					autoSize = value;
					SetSize();
				}
			}
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
				return ImeMode.Disable;
			}
		}

		protected override Size DefaultSize
		{
			get
			{
				return new Size(104, 42);
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

		public override Color ForeColor
		{
			get
			{
				return SystemColors.WindowText;
			}

			set
			{
			}
		}

		public new ImeMode ImeMode
		{
			get
			{
				return base.ImeMode;
			}

			set
			{
				base.ImeMode = value;
			}
		}

		public int LargeChange
		{
			get
			{
				return largeChange;
			}

			set
			{
				if (largeChange != value)
				{
					largeChange = value;
					using (Graphics g = CreateGraphics())
						Draw(g);
				}
			}
		}

		public int Maximum
		{
			get
			{
				return maximum;
			}

			set
			{
				if (maximum != value)
				{
					if (value < minimum)
						minimum = value;
					SetRange(minimum, value);
				}
			}
		}

		public int Minimum
		{
			get
			{
				return minimum;
			}

			set
			{
				if (minimum != value)
				{
					if (value > maximum)
						maximum = value;
					SetRange(value, maximum);
				}
			}
		}

		public Orientation Orientation
		{
			get
			{
				return orientation;
			}

			set
			{
				if (orientation != value)
				{
					orientation = value;
					SetSize();
					using (Graphics g = CreateGraphics())
						Draw(g);
				}
			}
		}

		public int SmallChange
		{
			get
			{
				return smallChange;
			}

			set
			{
				if (smallChange != value)
				{
					smallChange = value;
					using (Graphics g = CreateGraphics())
						Draw(g);
				}
			}
		}

		public override string Text
		{
			get
			{
				return base.Text;
			}

			set
			{
				base.Text = value;
			}
		}

		public TickStyle TickStyle
		{
			get
			{
				return tickStyle;
			}

			set
			{
				if (tickStyle != value)
				{
					tickStyle = value;
					using (Graphics g = CreateGraphics())
						Draw(g);
				}
			}
		}

		public int TickFrequency
		{
			get
			{
				return tickFrequency;
			}

			set
			{
				if (tickFrequency != value)
				{
					tickFrequency = value;
					using (Graphics g = CreateGraphics())
						Draw(g);
				}
			}
		}

		public int Value
		{
			get
			{
				return value;
			}

			set
			{
				if (this.value != value)
				{
					if (value < minimum || value > maximum)
						throw new ArgumentException();
					this.value = value;
					CheckValue();
					// Redraw
					OnValueChanged(EventArgs.Empty);
				}
			}
		}

		private void CheckValue()
		{
			if (initializing)
				return;
			if (value < minimum)
				value = minimum;
			if (value > maximum)
				value = maximum;
		}

		private void SetSize()
		{
			//TODO
			if (orientation == Orientation.Horizontal)
			{
				base.Height = 20;
				base.Width = dimension;
			}
			else
			{
				base.Height = dimension;
				base.Width = 20;
			}
		}

		protected override void CreateHandle()
		{
			base.CreateHandle();
		}

		public virtual void BeginInit()
		{
			initializing = true;
		}

		public virtual void EndInit()
		{
			initializing = false;
			CheckValue();
		}

		protected override bool IsInputKey(Keys keyData)
		{
			switch (keyData & Keys.KeyCode)
			{
				case Keys.Prior:
				case Keys.Next:
				case Keys.End:
				case Keys.Home:
					return true;
				default:
					return base.IsInputKey(keyData);
			}
		}

		protected override void OnHandleCreated(EventArgs e)
		{
			base.OnHandleCreated(e);
			SetSize();
		}

		protected virtual void OnScroll(EventArgs e)
		{
			if (Scroll != null)
				Scroll(this, e);
		}

		protected override void OnMouseWheel(MouseEventArgs e)
		{
			base.OnMouseWheel(e);
		}

		protected virtual void OnValueChanged(EventArgs e)
		{
			if (ValueChanged != null)
				ValueChanged(this, e);
		}

		protected override void OnBackColorChanged(EventArgs e)
		{
			base.OnBackColorChanged(e);
		}

		protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
		{
			if (orientation == Orientation.Horizontal)
				dimension = width;
			else
				dimension = height;
			if (autoSize)
			{
				if (orientation == Orientation.Horizontal)
				{
					if ((specified & BoundsSpecified.Height) != 0)
					{
						// TODO
						height = 20;
					}
				}
				else if ((specified & BoundsSpecified.Width) != 0)
				{
					//TODO
					width = 20;
				}
			}
			base.SetBoundsCore(x, y, width, height, specified);
		}

		public void SetRange(int minValue, int maxValue)
		{
			if (minimum == minValue && maximum == maxValue)
				return;
			if (minValue > maxValue)
				maxValue = minValue;
			minimum = minValue;
			maximum = maxValue;
			if (value < minimum)
				value = minimum;
			if (value > maximum)
				value = maximum;
			using (Graphics g = CreateGraphics())
				Draw(g);
		}

		// Draw the trackbar
		private void Draw(Graphics g)
		{
		}
		
		public override string ToString()
		{
			return base.ToString() + ", Minimum: " + Minimum + ", Maximum: " + Maximum + ", Value: " + Value;
		}

#if !CONFIG_COMPACT_FORMS
		protected override void WndProc(ref Message m)
		{
			// Nothing to do
		}
#endif
		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint (e);
			// Paint based on TickStyle(None, TopLeft, BottomRight, Both)
			// Also Orientation.Horizontal/Vertical
				
		}

	}

}
