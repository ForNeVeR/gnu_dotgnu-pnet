/*
 * XCrossingEvent.cs - Definitions for X event structures.
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
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

namespace Xsharp.Events
{

using System;
using System.Runtime.InteropServices;

// Border crossing event.
[StructLayout(LayoutKind.Sequential)]
internal struct XCrossingEvent
{
	// Structure fields.
	XAnyEvent			common__;
	public Xlib.Window	root;
	public Xlib.Window	subwindow;
	public Xlib.Time	time;
	public Xlib.Xint	x__;
	public Xlib.Xint	y__;
	public Xlib.Xint	x_root__;
	public Xlib.Xint	y_root__;
	public Xlib.Xint	mode__;
	public Xlib.Xint	detail__;
	public Xlib.Bool	same_screen__;
	public Xlib.Bool	focus__;
	public Xlib.Xuint	state__;

	// Access parent class fields.
	public int type           { get { return common__.type; } }
	public uint serial        { get { return common__.serial; } }
	public bool send_event    { get { return common__.send_event; } }
	public IntPtr display     { get { return common__.display; } }
	public Xlib.Window window { get { return common__.window; } }

	// Convert odd fields into types that are useful.
	public int x              { get { return (int)x__; } }
	public int y              { get { return (int)y__; } }
	public int x_root         { get { return (int)x_root__; } }
	public int y_root         { get { return (int)y_root__; } }
	public CrossingMode mode  { get { return (CrossingMode)(int)mode__; } }
	public CrossingDetail detail
		{ get { return (CrossingDetail)(int)detail__; } }
	public bool same_screen
		{ get { return (same_screen__ != Xlib.Bool.False); } }
	public bool focus
		{ get { return (focus__ != Xlib.Bool.False); } }
	public ModifierMask state { get { return (ModifierMask)(uint)state__; } }

} // struct XCrossingEvent

} // namespace Xsharp.Events
