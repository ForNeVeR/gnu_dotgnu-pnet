/*
 * RankException.cs - Implementation of the "System.RankException" class.
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

public class RankException : SystemException
{

	// Constructors.
	public RankException()
		: base(Environment.GetResourceString("Exception_Rank")) {}
	public RankException(String msg)
		: base(msg) {}
	public RankException(String msg, Exception inner)
		: base(msg, inner) {}

}; // class RankException

}; // namespace System
