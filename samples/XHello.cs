/*
 * XHello.cs - Sample program for Xsharp.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 *
 * This program is free software, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

using System;
using Xsharp;

public class XHello
{
	// Main entry point.
	public static void Main(String[] args)
	{
		Application app = new Application("XHello", args);
		TopLevelWindow topLevel = new TopLevelWindow("Hello World!", 200, 100);
		topLevel.Paint += new PaintEventHandler(PaintHello);
		topLevel.Map();
		app.Run();
		app.Close();
	}

	// Paint the top-level window.
	public static void PaintHello(Widget widget, Graphics graphics)
	{
		graphics.DrawLine(0, 0, widget.Width, widget.Height);
		graphics.DrawLine(0, widget.Height, widget.Width, 0);
	}

} // class XHello
