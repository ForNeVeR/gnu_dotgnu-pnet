/*
 * AppSettingsReader.cs - Implementation of the
 *		"System.Configuration.AppSettingsReader" interface.
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

namespace System.Configuration
{

#if !ECMA_COMPAT

using System;

public class AppSettingsReader
{
	// Constructor.
	public AppSettingsReader() {}

	// Get a specific application setting value.
	[TODO]
	public Object GetValue(String key, Type type)
			{
				if(key == null)
				{
					throw new ArgumentNullException("key");
				}
				if(type == null)
				{
					throw new ArgumentNullException("type");
				}
				// TODO
				return null;
			}

}; // class AppSettingsReader

#endif // !ECMA_COMPAT

}; // namespace System.Configuration
