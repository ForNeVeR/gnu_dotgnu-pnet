/*
 * NumericUpDown.cs - Implementation of the
 *			"System.Windows.Forms.NumericUpDown" class.
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
using System.ComponentModel;
using System.Reflection;
using System.Windows.Forms.Themes;

namespace System.Windows.Forms
{

[DefaultEventAttribute("ValueChanged")]
[DefaultPropertyAttribute("Value")]
public class NumericUpDown : UpDownBase, ISupportInitialize
{
	private const int DefaultDecimalPlaces = 0;
	private const bool DefaultHexadecimal = false;
	private const bool DefaultThousandsSeparator = false;
	
	private decimal currentValue;
	private int decimalPlaces;
	private bool hexadecimal;
	private decimal increment;
	private bool initializing;
	private decimal maximum;
	private decimal minimum;
	private bool thousandsSeparator;

	private static int DefaultIncrement
	{
		get
		{
			return 1;
		}
	}

	private static decimal DefaultMaximum
	{
		get
		{
			return 100;
		}
	}

	private static decimal DefaultMinimum
	{
		get
		{
			return 0;
		}
	}

	private static decimal DefaultValue
	{
		get
		{
			return DefaultMinimum;
		}
	}

	public NumericUpDown(): base()
	{
		decimalPlaces = DefaultDecimalPlaces;
		hexadecimal = DefaultHexadecimal;
		increment = DefaultIncrement;
		maximum = DefaultMaximum;
		minimum = DefaultMinimum;
		initializing = false;
		currentValue = DefaultValue;
		thousandsSeparator = DefaultThousandsSeparator;
		UpdateEditText();
	}

	[TODO]
	public void BeginInit()
	{
		initializing = true;
	}

	[DefaultValue(0)]
	public int DecimalPlaces
	{
		get
		{
			return decimalPlaces;
		}
		set
		{
			if ((value < 0) || (value > 99))
			{
				throw new ArgumentException();
			}
			if (decimalPlaces != value)
			{
				decimalPlaces = value;
				UpdateEditText();
			}
		}
	}

	[TODO]
	public void EndInit()
	{
		initializing = false;
	}

	public decimal Increment
	{
		get
		{
			return increment;
		}
		set
		{
			increment = value;
		}
	}

	[RefreshProperties(RefreshProperties.All)]
	public decimal Maximum
	{
		get
		{
			return maximum;
		}
		set
		{
			maximum = Constrain(value);
			if (maximum < currentValue)
			{
				currentValue = maximum;
				UpdateEditText();
			}
		}
	}

	[RefreshProperties(RefreshProperties.All)]
	public decimal Minimum
	{
		get
		{
			return minimum;
		}
		set
		{
			minimum = Constrain(value);
			if (minimum > currentValue)
			{
				currentValue = minimum;
				UpdateEditText();
			}
		}
	}


	public decimal Value
	{
		get
		{
			return currentValue;
		}
		set
		{
			if ((value < minimum) || (value > maximum))
			{
				throw new ArgumentException();
			}
			currentValue = Constrain(value);
			UpdateEditText();
		}
	}

	[TODO]
	private decimal Constrain(decimal value)
	{
		return value;
	}

	public override void UpButton()
	{
		if (currentValue < maximum)
		{
			currentValue += increment;
			if (currentValue > maximum)
			{
				currentValue = maximum;
			}
			UpdateEditText();
		}
	}

	public override void DownButton()
	{
		if (currentValue > minimum)
		{
			currentValue -= increment;
			if (currentValue < minimum)
			{
				currentValue = minimum;
			}
			UpdateEditText();
		}
	}

	[EditorBrowsable(EditorBrowsableState.Never)]
	[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
	[Browsable(false)]
	[Bindable(false)]
	public override string Text
	{
		get
		{
			return base.Text;
		}
		set
		{
			// FIXME
			base.Text = value;

		}
	}


	[Localizable(true)]
	[DefaultValue(false)]
	public bool ThousandsSeparator
	{
		get
		{
			return thousandsSeparator;
		}
		set
		{
			if (thousandsSeparator != value)
			{
				thousandsSeparator = value;
				UpdateEditText();
			}
		}
	}

	protected override void UpdateEditText()
	{
		if (hexadecimal)
		{
			base.Text = currentValue.ToString("X");
		}
		else
		{
			if (thousandsSeparator)
			{
				base.Text = currentValue.ToString("N" + decimalPlaces.ToString());
			}
			else
			{
				base.Text = currentValue.ToString("F" + decimalPlaces.ToString());
			}
		}
	}

	[TODO]
	protected virtual void ValidateEditText()
	{
	}

	[TODO]
	protected override void OnChanged(object source, EventArgs e)
	{
		throw new NotImplementedException("OnChanged");
	}

	public override string ToString()
	{
		return "System.Windows.Forms.NumericUpDown, Minimum = " + minimum.ToString() +
			", Maximun = " + maximum.ToString();
	}

	public event EventHandler ValueChanged;

}; // class DomainUpDown
	
}; // namespace System.Windows.Forms

