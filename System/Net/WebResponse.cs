/*
 * WebResponse.cs - Implementation of the "System.Net.WebResponse" class.
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
using System.IO;

public abstract class WebResponse : MarshalByRefObject, IDisposable
{
	[TODO]
	protected WebResponse() {}
	
	[TODO]
	public virtual void Close() {}
	
	[TODO]
	public virtual Stream GetResponseStream() {}
	
/*	[TODO] //Compiler gives an error when this thing is on
	void IDisposable.Dispose() {} */
	
	[TODO]
	public virtual long ContentLength { get{}  set{} }
	
	[TODO]
	public virtual String ContentType { get{} set{} }
	
	[TODO]
	public virtual WebHeaderCollection Headers { get{} }
	
	[TODO]
	public virtual Uri ResponseUri { get{} }
	
}; //class WebResponse

}; //namespace System.Net