/*
 * ExternalException.cs - Implementation of the "ExternalException" class 
 *
 * Copyright (C) 2001  Free Software Foundation, Inc.
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

namespace System.Runtime.InteropServices
{

public class ExternalException : SystemException
{

	private int errorCode;

	// Constructors.
	public ExternalException()
		: base(_("Exception_System")) {}
	public ExternalException(String msg)
		: base(msg) {}
	public ExternalException(String msg, Exception inner)
		: base(msg, inner) {}
	public ExternalException(String msg, int errorCode)
		: base(msg)
	{
		this.errorCode=errorCode;
	}

	public virtual int ErrorCode
	{
		get
		{
			return this.errorCode;
		}
	}

}; // class ExternalException

}; // namespace System.Runtime.InteropServices
