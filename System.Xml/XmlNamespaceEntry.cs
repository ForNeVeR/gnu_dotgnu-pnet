/*
 * XmlNamespaceEntry.cs - Implementation of the "System.XmlNamespaceEntry" class.
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

#if !ECMA_COMPAT
//Used for storing namespace info in
public class XmlNamespaceEntry
{
	private String element; //The element the namespace is associated with
	private String name; //The name of the namespace
	private String url; //The url where the info for the namespace is
	
	public XmlNamespaceEntry(String myelement, String myname, String myurl)
	{
		element = myelement;
		name = myname;
		url = myurl;
	}
	
	public String Element
	{
		get
		{
			return element;
		}
		set
		{
			element = value;
		}
	}

	public String Name
	{
		get
		{
			return name;
		}
		set
		{
			name = value;
		}
	}
	
	public String Url
	{
		get
		{
			return url;
		}
		set
		{
			url = value;
		}
	}
	
}; //class XmlNamespaceEntry
#endif

}; //namespace System.Xml
