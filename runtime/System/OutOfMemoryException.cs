/*
 * OutOfMemoryException.cs - Implementation of the
 *		"System.OutOfMemoryException" class.
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

public class OutOfMemoryException : SystemException
{

	// Constructors.
	public OutOfMemoryException()
		: base(Environment.GetResourceString("Exception_NoMemory")) {}
	public OutOfMemoryException(String msg)
		: base(msg) {}
	public OutOfMemoryException(String msg, Exception inner)
		: base(msg, inner) {}

	// Internal constructor used by the runtime engine
	// to make the primary "OutOfMemoryException" object,
	// that has no stack trace associated with it.
	internal OutOfMemoryException(int dummy)
		: base(Environment.GetResourceString("Exception_NoMemory"),
			   null, false) {}

}; // class OutOfMemoryException

}; // namespace System
