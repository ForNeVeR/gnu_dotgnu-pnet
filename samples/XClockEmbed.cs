/*
 * XClockEmbed.cs - Embed "xclock" in an Xsharp app using XC-APPGROUP.
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

public class XClockEmbed : TopLevelWindow
{
	// Main entry point.
	public static void Main(String[] args)
	{
		Application app = new Application("XClockEmbed", args);
		XClockEmbed topLevel = new XClockEmbed("Embedded Clock", 200, 200);
		topLevel.Map();
		app.Run();
		app.Close();
	}

	// Internal state.
	private EmbeddedApplication embed;
	private bool allowClose;

	// Constructor.
	public XClockEmbed(String title, int width, int height)
		: base(title, width, height)
	{
		embed = new Embedder(this, 0, 0, width, height);
		embed.Program = "xclock";
		embed.Launch();
	}

	// Handle resizes of the main application window.
	protected override void OnResize(int width, int height)
	{
		embed.Resize(width, height);
	}

	// Handle close events on the main application window.
	// We pass the close down to the embedded application.
	protected override bool OnClose()
	{
		if(allowClose)
		{
			return true;
		}
		else
		{
			embed.Close();
			return false;
		}
	}

	// Embed window, overriding the "OnClose()" notification.
	private class Embedder : EmbeddedApplication
	{
		// Constructor.
		public Embedder(Widget parent, int x, int y, int width, int height)
			: base(parent, x, y, width, height) {}

		// Receive notification of application closure.
		protected override void OnClose()
		{
			((XClockEmbed)Parent).allowClose = true;
			((XClockEmbed)Parent).Close();
		}

	}; // class Embedder

}; // class XClockEmbed
