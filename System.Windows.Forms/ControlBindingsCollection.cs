/*
 * ControlBindingsCollection.cs - Implementation of the
 *			"System.Windows.Forms.ControlBindingsCollection" class.
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

namespace System.Windows.Forms
{

public class ControlBindingsCollection: BindingsCollection
{
		[TODO]
	protected internal ControlBindingsCollection()
	{
		//throw new NotImplementedException(".ctor");
	}

	[TODO]
	new public void Add(Binding binding)
	{
		//throw new NotImplementedException("Add");
	}

	[TODO]
	public Binding Add(String propertyName, Object dataSource, String dataMember)
	{
			return null;
		//throw new NotImplementedException("Add");
	}

	[TODO]
	protected override void AddCore(Binding dataBinding)
	{
		//throw new NotImplementedException("AddCore");
	}

	[TODO]
	new public void Clear()
	{
		//throw new NotImplementedException("Clear");
	}

	[TODO]
	protected override void ClearCore()
	{
		//throw new NotImplementedException("ClearCore");
	}

	[TODO]
	new public void Remove(Binding binding)
	{
		//throw new NotImplementedException("Remove");
	}

	[TODO]
	public void RemoveAt(int index)
	{
		//throw new NotImplementedException("RemoveAt");
	}

	[TODO]
	protected override void RemoveCore(Binding dataBinding)
	{
		//throw new NotImplementedException("RemoveCore");
	}

	[TODO]
	public Control Control 
	{
		get
		{
			return null;
			//throw new NotImplementedException("Control");
		}

 	}

	[TODO]
	public Binding this[String propertyName] 
	{
		get
		{
			return null;
			//throw new NotImplementedException("Item");
		}

 	}

}; // class ControlBindingsCollection

}; // namespace System.Windows.Forms
