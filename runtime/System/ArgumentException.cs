/*
 * ArgumentException.cs - Implementation of the
 *		"System.ArgumentException" class.
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

public class ArgumentException : SystemException
{

	// Internal state.
	private String paramName;

	// Constructors.
	public ArgumentException()
		: base(Environment.GetResourceString("Exception_Argument")) {}
	public ArgumentException(String msg)
		: base(msg) {}
	public ArgumentException(String msg, Exception inner)
		: base(msg, inner) {}
	public ArgumentException(String msg, String param, Exception inner)
		: base(msg, inner) { paramName = param; }
	public ArgumentException(String msg, String param)
		: base(msg) { paramName = param; }

	// Properties.
	public virtual String ParamName
			{
				get
				{
					return paramName;
				}
			}

	// Get the default message to use for this exception type.
	protected internal override String MessageDefault
			{
				get
				{
					return Environment.GetResourceString
						("Exception_Argument");
				}
			}

	// Get the extra data to insert into "Exception::ToString()"'s result.
	protected internal override String MessageExtra
			{
				get
				{
					if(paramName != null)
					{
						return String.Format
							   		(Environment.GetResourceString
										("Exception_ArgParamName"),
									 paramName);
					}
					else
					{
						return null;
					}
				}
			}

}; // class ArgumentException

}; // namespace System
