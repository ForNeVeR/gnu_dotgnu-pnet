/*
 * XmlResolver.cs - Implementation of the "System.Xml.XmlResolver" class.
 *
 * Copyright (C) 2002 Southern Storm Software, Pty Ltd.
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
 
namespace System.Xml
{

using System;
using System.Net;

public abstract class XmlResolver
{

	// Constructor.
	protected XmlResolver() {}

	// Map a URI to the entity it represents.
	public abstract Object GetEntity(Uri absoluteUri, String role,
									 Type ofObjectToReturn);

	// Resolve a relative URI.
	public abstract Uri ResolveUri(Uri baseUri, String relativeUri);

	// Set the credentials to use to resolve Web requests.
	public abstract ICredentials Credentials { set; }

}; // class XmlResolver

}; // namespace System.Xml
