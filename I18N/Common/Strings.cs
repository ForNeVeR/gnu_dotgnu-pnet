/*
 * Strings.cs - Implementation of the "I18N.Common.Strings" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace I18N.Common
{

using System;
using System.Reflection;
using System.Resources;

// This class provides string resource support to the rest
// of the I18N library assemblies.

public sealed class Strings
{
	// Cached copy of the resources for this assembly.
	private static ResourceManager resources = null;

	// Helper for obtaining string resources for this assembly.
	public static String GetString(String tag)
			{
				lock(typeof(Strings))
				{
					if(resources == null)
					{
						resources = new ResourceManager
							("I18N", Assembly.GetExecutingAssembly());
					}
					return resources.GetString(tag, null);
				}
			}

}; // class Strings

}; // namespace I18N.Common
