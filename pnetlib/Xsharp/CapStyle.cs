/*
 * CapStyle.cs - GC line capping styles.
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
/// <para>The <see cref="T:Xsharp.CapStyle"/> enumeration specifies
/// the line capping style for graphics objects
/// (<see cref="T:Xsharp.Graphics"/>).
/// </para>
/// </summary>
public enum CapStyle
{

	CapNotLast    = 0,
	CapButt       = 1,
	CapRound      = 2,
	CapProjecting = 3

} // enum CapStyle

} // namespace Xsharp