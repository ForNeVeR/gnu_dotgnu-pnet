/*
 * EventHandlers.cs - Event handler delegate types for X#.
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

namespace Xsharp
{

using System;

/// <summary>
/// <para>The <see cref="T:Xsharp.MovedEventHandler"/> delegate is
/// used to describe widget move events.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that was moved.</para>
/// </param>
///
/// <param name="x">
/// <para>The new X co-ordinate for the widget.</para>
/// </param>
///
/// <param name="y">
/// <para>The new Y co-ordinate for the widget.</para>
/// </param>
public delegate void MovedEventHandler(Widget widget, int x, int y);

/// <summary>
/// <para>The <see cref="T:Xsharp.ResizedEventHandler"/> delegate is
/// used to describe widget resize events.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that was resized.</para>
/// </param>
///
/// <param name="width">
/// <para>The new width for the widget.</para>
/// </param>
///
/// <param name="height">
/// <para>The new height for the widget.</para>
/// </param>
public delegate void ResizedEventHandler(Widget widget, int width, int height);

/// <summary>
/// <para>The <see cref="T:Xsharp.MapStateEventHandler"/> delegate is
/// used to describe widget map state change events.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that was mapped or unmapped.</para>
/// </param>
///
/// <param name="mapped">
/// <para>Set to <see langword="true"/> if the widget has just been
/// mapped; <see langword="false"/> otherwise.</para>
/// </param>
public delegate void MapStateEventHandler(Widget widget, bool mapped);

/// <summary>
/// <para>The <see cref="T:Xsharp.PaintEventHandler"/> delegate is
/// used to describe widget paint events.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that is being painted.</para>
/// </param>
///
/// <param name="graphics">
/// <para>The graphics object to use to repaint the widget.  This
/// graphics object will have been initialised with the widgets foreground and
/// background colors, and with the clipping region set to the area
/// that needs to be repainted.</para>
/// </param>
public delegate void PaintEventHandler(Widget widget, Graphics graphics);

/// <summary>
/// <para>The <see cref="T:Xsharp.MapStateEventHandler"/> delegate is
/// used to describe widget iconic state change events.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that was iconified or de-iconified.</para>
/// </param>
///
/// <param name="iconified">
/// <para>Set to <see langword="true"/> if the widget has just been
/// iconified; <see langword="false"/> if it was de-iconified.</para>
/// </param>
public delegate void IconicStateEventHandler(Widget widget, bool iconified);

/// <summary>
/// <para>The <see cref="T:Xsharp.ClosedEventHandler"/> delegate is
/// used to describe widget close requests.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that is to be closed.</para>
/// </param>
///
/// <returns>
/// <para>Returns <see langword="true"/> if the window can be closed,
/// or <see langword="false"/> if some condition is preventing to
/// window from being closed.</para>
/// </returns>
public delegate bool ClosedEventHandler(Widget widget);

/// <summary>
/// <para>The <see cref="T:Xsharp.ButtonPressEventHandler"/> delegate is
/// used to process button press events.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that received the event.</para>
/// </param>
///
/// <param name="x">
/// <para>The X co-ordinate of the pointer position.</para>
/// </param>
///
/// <param name="y">
/// <para>The Y co-ordinate of the pointer position.</para>
/// </param>
///
/// <param name="button">
/// <para>The button that was pressed.</para>
/// </param>
///
/// <param name="modifiers">
/// <para>Other button and shift flags that were active.</para>
/// </param>
public delegate void ButtonPressEventHandler(Widget widget, int x, int y,
											 ButtonName button,
											 ModifierMask modifiers);

/// <summary>
/// <para>The <see cref="T:Xsharp.ButtonReleaseEventHandler"/> delegate is
/// used to process button release events.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that received the event.</para>
/// </param>
///
/// <param name="x">
/// <para>The X co-ordinate of the pointer position.</para>
/// </param>
///
/// <param name="y">
/// <para>The Y co-ordinate of the pointer position.</para>
/// </param>
///
/// <param name="button">
/// <para>The button that was released.</para>
/// </param>
///
/// <param name="modifiers">
/// <para>Other button and shift flags that were active.</para>
/// </param>
public delegate void ButtonReleaseEventHandler(Widget widget, int x, int y,
											   ButtonName button,
											   ModifierMask modifiers);

/// <summary>
/// <para>The <see cref="T:Xsharp.PointerMotionEventHandler"/> delegate is
/// used to process mouse pointer motion events.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that received the event.</para>
/// </param>
///
/// <param name="x">
/// <para>The X co-ordinate of the pointer position.</para>
/// </param>
///
/// <param name="y">
/// <para>The Y co-ordinate of the pointer position.</para>
/// </param>
///
/// <param name="modifiers">
/// <para>Button and shift flags that were active.</para>
/// </param>
public delegate void PointerMotionEventHandler(Widget widget, int x, int y,
											   ModifierMask modifiers);

/// <summary>
/// <para>The <see cref="T:Xsharp.KeyPressEventHandler"/> delegate is
/// used to process key press events.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that received the event.</para>
/// </param>
///
/// <param name="key">
/// <para>The key code.</para>
/// </param>
///
/// <param name="modifiers">
/// <para>Other button and shift flags that were active.</para>
/// </param>
///
/// <param name="str">
/// <para>The translated string that corresponds to the key, or
/// <see langword="null"/> if the key does not have a translation.</para>
/// </param>
///
/// <returns>
/// <para>Returns <see langword="true"/> if the key has been processed
/// and it should not be passed further up the focus tree.  Returns
/// <see langword="false"/> if the key should be passed further up
/// the focus tree.</para>
/// </returns>
///
/// <remarks>The <paramref name="key"/> parameter indicates the X11
/// symbol that corresponds to the key, which allows cursor control
/// and function keys to be easily distinguished.  The <paramref name="str"/>
/// is primarily of use to text input widgets.</remarks>
public delegate bool KeyPressEventHandler(Widget widget, KeyName key,
										  ModifierMask modifiers, String str);

/// <summary>
/// <para>The <see cref="T:Xsharp.CrossingEventHandler"/> delegate is
/// used to process Enter and Leave events on a widget.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that received the event.</para>
/// </param>
///
/// <param name="child">
/// <para>The child widget that contained the previous or final
/// position, or <see langword="null"/> if no applicable child widget.</para>
/// </param>
///
/// <param name="x">
/// <para>The X co-ordinate of the pointer position.</para>
/// </param>
///
/// <param name="y">
/// <para>The Y co-ordinate of the pointer position.</para>
/// </param>
///
/// <param name="modifiers">
/// <para>Button and shift flags that were active.</para>
/// </param>
///
/// <param name="mode">
/// <para>The notification mode value from the event.</para>
/// </param>
///
/// <param name="detail">
/// <para>The notification detail value from the event.</para>
/// </param>
public delegate void CrossingEventHandler(Widget widget, Widget child,
										  int x, int y,
										  ModifierMask modifiers,
										  CrossingMode mode,
										  CrossingDetail detail);

/// <summary>
/// <para>The <see cref="T:Xsharp.FocusChangeEventHandler"/> delegate is
/// used to process focus change events.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget that received the event.</para>
/// </param>
///
/// <param name="other">
/// <para>If this is a focus in event, <paramref name="other"/> is the
/// previous widget within the same top-level window that had
/// the focus, or <see langword="null"/> if the focus is entering the
/// top-level window from outside.</para>
///
/// <para>If this is a focus out event, <paramref name="other"/> is the
/// new widget within the same top-level window that will receive
/// the focus, or <see langword="null"/> if the focus is leaving the
/// top-level window.</para>
/// </param>
public delegate void FocusChangeEventHandler(Widget widget, Widget other);

/// <summary>
/// <para>The <see cref="T:Xsharp.SensitivityChangedEventHandler"/>
/// delegate is used to process changes in input event sensitivity.</para>
/// </summary>
///
/// <param name="widget">
/// <para>The widget whose sensitivity has changed.</para>
/// </param>
public delegate void SensitivityChangedEventHandler(Widget widget);

} // namespace Xsharp
