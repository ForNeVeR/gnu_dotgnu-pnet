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
	public override String Message
			{
				get
				{
					String msg = base.Message;
					if(paramName != null && paramName.Length != 0)
					{
						return msg + Environment.NewLine +
							   "Parameter name: " + paramName;
					}
					else
					{
						return msg;
					}
				}
			}

}; // class ArgumentException

}; // namespace System
