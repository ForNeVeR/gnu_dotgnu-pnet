/*
 * NameValueSectionHandler.cs - Implementation of the
 *		"System.Configuration.NameValueSectionHandler" interface.
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
using System.Xml;

public class NameValueSectionHandler : IConfigurationSectionHandler
{
	// Constructor.
	public NameValueSectionHandler() {}

	// Get the name of the key attribute tag.
	protected virtual String KeyAttributeName
			{
				get
				{
					return "key";
				}
			}

	// Get the name of the value attribute tag.
	protected virtual String ValueAttributeName
			{
				get
				{
					return "value";
				}
			}

#if SECOND_PASS

	// Create a configuration object for a section.
	[TODO]
	Object Create(Object parent, Object configContext, XmlNode section)
			{
				// TODO
				return null;
			}

#endif // SECOND_PASS

}; // class NameValueSectionHandler

#endif // !ECMA_COMPAT

}; // namespace System.Configuration
