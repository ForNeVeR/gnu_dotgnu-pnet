/*
 * ToolkitFunctions.cs - Implementation of the
 *			"System.Drawing.Toolkit.ToolkitFunctions" class.
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

// Supported window functions for "IToolkitManager.SetFunctions".
public enum ToolkitFunctions
{
	All				= 1 << 0,	// All except those listed below.
	Resize			= 1 << 1,
	Move			= 1 << 2,
	Minimize		= 1 << 3,
	Maximize		= 1 << 4,
	Close			= 1 << 5

}; // enum ToolkitFunctions

}; // namespace System.Drawing.Toolkit
