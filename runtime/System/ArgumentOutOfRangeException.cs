/*
 * ArgumentOutOfRangeException.cs - Implementation of the
 *		"System.ArgumentOutOfRangeException" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System
{

public class ArgumentOutOfRangeException : ArgumentException
{
	// Standard error message for out of range exceptions.
	private static String preloadedMessage = _("Arg_OutOfRange");

	// Internal state.
	private Object actualValue;

	// Constructors.
	public ArgumentOutOfRangeException()
		: base(preloadedMessage) {}
	public ArgumentOutOfRangeException(String paramName)
		: base(preloadedMessage, paramName) {}
	public ArgumentOutOfRangeException(String paramName, String msg)
		: base(msg, paramName) {}
	public ArgumentOutOfRangeException(String paramName,
									   Object _actualValue, String msg)
		: base(msg, paramName) { actualValue = _actualValue; }

	// Properties.
	public virtual Object ActualValue
			{
				get
				{
					return actualValue;
				}
			}
	public override String Message
			{
				get
				{
					String msg = base.Message;
					if(actualValue == null)
					{
						return msg;
					}
					else
					{
						return msg + Environment.NewLine +
							   String.Format
							   		(_("Arg_OutOfRangeValue"),
									 actualValue.ToString());
					}
				}
			}

	// Get the default message to use for this exception type.
	protected internal override String MessageDefault
			{
				get
				{
					return preloadedMessage;
				}
			}

}; // class ArgumentOutOfRangeException

}; // namespace System
