/*
 * ProjectData.cs - Implementation of the
 *			"Microsoft.VisualBasic.ProjectData" class.
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

namespace Microsoft.VisualBasic.CompilerServices
{

using System;
using System.ComponentModel;

[EditorBrowsable(EditorBrowsableState.Never)]
public sealed class ProjectData
{
	// Cannot instantiate this class.
	private ProjectData() {}

	// Destructor - not used but must be declared.
	~ProjectData() {}

	// Clear the project error.
	[TODO]
	public static void ClearProjectError()
			{
				// TODO
			}

	// Create a new project error.
	[TODO]
	public static Exception CreateProjectError(int hr)
			{
				// TODO
				return null;
			}

	// End the application.
	public static void EndApp()
			{
				FileSystem.Reset();
				Environment.Exit(0);
			}

	// Set the project error.
	[TODO]
	public static void SetProjectError(Exception ex)
			{
				// TODO
			}
	[TODO]
	public static void SetProjectError(Exception ex, int lErl)
			{
				// TODO
			}

}; // class ProjectData

}; // namespace Microsoft.VisualBasic.CompilerServices
