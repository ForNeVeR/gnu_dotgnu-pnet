/*
 * ToolTip.cs - Implementation of the
 *			"System.Windows.Forms.ToolTip" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2003  Free Software Foundation, Inc.
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
using System.ComponentModel;


public sealed class ToolTip : Component, IExtenderProvider
{
	//[TODO]
	public ToolTip()
			{
	
		
			}

	//[TODO]
	public ToolTip(IContainer cont)
			{

			}

//	[TODO]
	public bool Active
			{
				get
				{
					return false;
				}
				set
				{
				}
			}

//	[TODO]
	public int AutomaticDelay
			{
				get
				{
					return 0;
				}
				set
				{

				}
			}

//	[TODO]
	public bool AutoPopDelay 
			{
				get
				{
					return false;
				}
				set
				{

				}
			}

//	[TODO]
	public int ReshowDelay
			{
				get
				{	
					return 0;
				}
				set
				{

				}

			}

//	[TODO]
	public bool ShowAlways 
			{
				get
				{
					return false;
				}
				set
				{

				}
			}

//	[TODO]
	public String GetToolTip(Control control)
			{
				// TODO
				return String.Empty;
			}
//	[TODO]
	public bool CanExtend(Object target)
			{
				// TODO
				return false;
			}

//	[TODO]
	protected override void Dispose(bool disposing)
			{
				// TODO
			}

//	[TODO]
	public void RemoveAll()
			{
				// TODO
			}

//	[TODO]
	public void SetToolTip(Control control, String caption)
			{
				// TODO
			}

//	[TODO]
	public override string ToString()
			{
				// TODO
				return String.Empty;
			}
	
//	[TODO]
	~ToolTip()
			{	
				Dispose(false);
			}	

}	// class ToolTip

}	// namespace System.Windows.Forms

