/*
 * ToolkitEventHandlers.cs - Declarations for event handling delegates.
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

// Simple event handler, with EventArgs parameter.
public delegate void ToolkitSimpleHandler(EventArgs e);

// Handle an expose event, using a particular graphics object to paint.
public delegate void ToolkitExposeHandler(Graphics graphics);

// Handle a key event.
public delegate void ToolkitKeyHandler(ToolkitKeys key);

// Handle a key character event.
public delegate void ToolkitKeyCharHandler(char charCode);

// Handle a mouse event.
public delegate void ToolkitMouseHandler
		(ToolkitMouseButtons buttons, ToolkitKeys modifiers,
		 int clicks, int x, int y, int delta);

}; // namespace System.Drawing.Toolkit
