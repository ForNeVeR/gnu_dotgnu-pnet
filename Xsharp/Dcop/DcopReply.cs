/*
 * DcopReply.cs - Encapsulation of a DCOP reply data stream.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace Xsharp.Dcop
{

using System;
using System.IO;
using System.Text;

public class DcopReply : QDataStream
{
	// Constructors.
	public DcopReply(Stream stream) : base(stream)
			{
				// TODO
			}
	public DcopReply(byte[] buffer, int offset, int count)
			: this(new MemoryStream(buffer, offset, count, false)) {}

}; // class DcopReply

}; // namespace Xsharp.Dcop
