/*
 * NameValueFileSectionHandler.cs - Implementation of the
 *		"System.Configuration.NameValueFileSectionHandler" interface.
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

public class NameValueFileSectionHandler : IConfigurationSectionHandler
{
	// Constructor.
	public NameValueFileSectionHandler() {}

#if SECOND_PASS

	// Create a configuration object for a section.
	[TODO]
	public Object Create(Object parent, Object configContext, XmlNode section)
			{
				// TODO
				return null;
			}

#endif // SECOND_PASS

}; // class NameValueFileSectionHandler

#endif // !ECMA_COMPAT

}; // namespace System.Configuration
