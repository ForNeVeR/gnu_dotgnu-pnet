/*
 * WebHeaderCollection.cs - Implementation of the "System.Net.WebHeaderCollection" class.
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
 
namespace System.Net
{

using System;
//using System.Collections.Specialized; Add when it exists

//Add the NameValueCollection base when NameValueCollection is operating
public class WebHeaderCollection /* : NameValueCollection */
{
	[TODO]
	public WebHeaderCollection() {}
	
	[TODO]
	public override void Add(string name, string value) {}
	
	[TODO]
	public void Add(string header) {}
	
	[TODO]
	protected void AddWithoutValidate(string headerName, string headerValue) {}
	
	[TODO]
	public override String[] GetValues(string header) {}
	
	[TODO]
	public static bool IsRestricted(string headerName){}

	[TODO]
	public override void Remove(string name) {}	
	
	[TODO]
	public override void Set(string name, string value) {}
		
}; //class WebHeaderCollection

}; //namespace System.Net

