/*
 * ResourceSet.cs - Implementation of the
 *		"System.Resources.ResourceSet" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System.Resources
{

using System;
using System.IO;
using System.Collections;

public class ResourceSet : IDisposable
{
	// Internal state.
	protected IResourceReader Reader;
	protected Hashtable Table;

	// Constructors.
	[TODO]
	protected ResourceSet()
			{
				// TODO
			}
	[TODO]
	public ResourceSet(IResourceReader reader)
			{
				// TODO
			}
	[TODO]
	public ResourceSet(Stream stream)
			{
				// TODO
			}
	[TODO]
	public ResourceSet(String fileName)
			{
				// TODO
			}

	// Close this resource set.
	public void Close()
			{
				Dispose(true);
			}

	// Dispose this resource set.
	[TODO]
	protected virtual void Dispose(bool disposing)
			{
				// TODO
			}

	// Implement IDisposable.
	public void Dispose()
			{
				Dispose(true);
			}

	// Get the default reader type for this resource set.
	[TODO]
	public virtual Type GetDefaultReader()
			{
				// TODO
				return null;
			}

	// Get the default write type for this resource set.
	[TODO]
	public virtual Type GetDefaultWriter()
			{
				// TODO
				return null;
			}

	// Get an object from this resource set.
	[TODO]
	public virtual Object GetObject(String name)
			{
				// TODO
				return null;
			}
	[TODO]
	public virtual Object GetObject(String name, bool ignoreCase)
			{
				// TODO
				return null;
			}

	// Get a string from this resource set.
	[TODO]
	public virtual String GetString(String name)
			{
				// TODO
				return null;
			}
	[TODO]
	public virtual String GetString(String name, bool ignoreCase)
			{
				// TODO
				return null;
			}

	// Read all resources into the hash table.
	[TODO]
	public virtual void ReadResources()
			{
				// TODO
			}

}; // class ResourceSet

}; // namespace System.Resources
