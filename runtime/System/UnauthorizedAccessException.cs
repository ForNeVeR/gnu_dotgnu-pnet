/*
 * UnauthorizedAccessException.cs - Implementation of the
 *		"System.UnauthorizedAccessException" class.
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

public class UnauthorizedAccessException : SystemException
{

	// Constructors.
	public UnauthorizedAccessException()
		: base(_("Exception_Unauthorized")) {}
	public UnauthorizedAccessException(String msg)
		: base(msg) {}
	public UnauthorizedAccessException(String msg, Exception inner)
		: base(msg, inner) {}

	// Get the default message to use for this exception type.
	protected internal override String MessageDefault
			{
				get
				{
					return _("Exception_Unauthorized");
				}
			}

}; // class UnauthorizedAccessException

}; // namespace System
