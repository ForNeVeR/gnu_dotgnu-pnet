/*
 * ConfigurationSettings.cs - Implementation of the
 *		"System.Configuration.ConfigurationSettings" interface.
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

namespace System.Configuration
{

#if !ECMA_COMPAT

using System;
using System.Collections;
using System.Collections.Specialized;
using System.Xml;

public sealed class ConfigurationSettings
{
	// Internal state.

	// Constructor - cannot be created by external entities.
	private ConfigurationSettings() {}

	// Get a configuration object for a specific section.
	public static Object GetConfig(String sectionName)
			{
				// TODO
				return null;
			}

	// Get the application settings.
	public static NameValueCollection AppSettings
			{
				get
				{
					RONameValueCollection settings;
					settings = (RONameValueCollection)
							(GetConfig("appSettings"));
					if(settings == null)
					{
						settings = new RONameValueCollection();
					}
					return settings;
				}
			}

	// Read-only subclass of "NameValueCollection".
	internal sealed class RONameValueCollection : NameValueCollection
	{
		// Constructor.
		public RONameValueCollection()
			: base(new CaseInsensitiveHashCodeProvider(),
				   new CaseInsensitiveComparer())
			{
				IsReadOnly = true;
			}

	}; // class RONameValueCollection

}; // class ConfigurationSettings

#endif // !ECMA_COMPAT

}; // namespace System.Configuration