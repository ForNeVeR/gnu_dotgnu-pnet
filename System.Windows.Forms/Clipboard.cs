/*
 * Clipboard.cs - Implementation of the
 *			"System.Windows.Forms.Clipboard" class.
 *
 * Copyright (C) 2003  Free Software Foundation, Inc.
 *
 * Contributions from Cecilio Pardo <cpardo@imayhem.com>
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
using System.Threading;

[TODO]
public sealed class Clipboard
{
	// TODO: This is a simple implementation that only
	//       works inside the same application.

	private static IDataObject obj = null;

	private Clipboard() {}

	[TODO]
	public static IDataObject GetDataObject()
	{
		Thread ct = Thread.CurrentThread;
		if (ct.ApartmentState != ApartmentState.STA)
		{
			throw new ThreadStateException(/* TODO */);
		}
		return obj;
	}

	public static void SetDataObject(Object data)
	{
		SetDataObject(data, false);
	}

	[TODO]
	public static void SetDataObject(Object data, bool copy)
	{
		if (data == null)
		{
			throw new ArgumentNullException("data");
		}
		if ((obj = (data as IDataObject)) == null)
		{
			obj = new DataObject(data);
		}
	}

}; // class Clipboard

}; // namespace System.Windows.Forms
