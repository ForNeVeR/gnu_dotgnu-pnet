/*
 * NameTable.cs - Implementation of the "System.NameTable" class.
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
using System.Collections;

public class NameTable : XmlNameTable
{
	private ArrayList myarraylist = new ArrayList();
	
	public NameTable() {}
	
	public override String Add(String key)
	{
		if (key == null)
			throw new ArgumentNullException("key");		
		
		foreach(String s in myarraylist)
		{
			if (s == key)
			{
				return s;
			}
		}
		
		myarraylist.Add(key);
		return key;
	}
	
	public override String Add(char[] key, int start, int len)
	{
		if (len < 0)
			throw new ArgumentOutOfRangeException("len");
		
		if ((len > key.Length - start) && (len != 0))
			throw new IndexOutOfRangeException("key");
		
		if ((start < 0 || start >= key.Length) && (len != 0))
			throw new IndexOutOfRangeException("start");
		
		String newstr = new String(key, start, len);
		
		return Add(newstr);
	}
	
	public override String Get(String value)
	{
		if (value == null)
			throw new ArgumentNullException("value");		
		
		foreach(String s in myarraylist)
		{
			if (s == value)
			{
				return s;
			}
		}
		
		return null;
	}
	
	public override String Get(char[] value, int offset, int length)
	{
		if (length < 0)
			throw new ArgumentOutOfRangeException("length");
		
		if ((length > value.Length - offset) && (length != 0))
			throw new IndexOutOfRangeException("value");
		
		if ((offset < 0 || offset >= value.Length) && (length != 0))
			throw new IndexOutOfRangeException("offset");
		
		String newstr = new String(value, offset, length);
		
		return Get(newstr);
	}

}; //class NameTable

}; //namespace System.Xml
