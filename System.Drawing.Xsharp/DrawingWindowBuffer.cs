/*
 * DrawingWindowBuffer.cs - Window Double Buffer class.
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
namespace System.Drawing.Toolkit
{

using System;

public class DrawingWindowBuffer : IToolkitWindowBuffer, IDisposable
{
	DrawingWindow window;
	Size size;

	public DrawingWindowBuffer(IToolkitWindow windowToBuffer)
	{
		window = windowToBuffer as DrawingWindow;
		CreateBuffer(windowToBuffer.Dimensions.Size);
	}

	private void CreateBuffer(Size size)
	{
		this.size = size;
		// TODO
	}

	private void DeleteBuffer()
	{
		// TODO
	}

	public IToolkitGraphics BeginDoubleBuffer()
	{
		Size newSize = (window as IToolkitWindow).Dimensions.Size;
		// If the size changes, we need to recreate the buffer.
		if (size != newSize)
		{
			DeleteBuffer();
			CreateBuffer(newSize);
		}
		
		// TODO
		return null;
	}

	public void EndDoubleBuffer()
	{
		// TODO
	}

	public void Dispose()
	{
		// TODO
	}

}
}