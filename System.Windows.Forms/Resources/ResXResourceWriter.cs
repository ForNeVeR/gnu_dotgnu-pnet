/*
 * ResXResourceWriter.cs - Implementation of the
 *			"System.Resources.ResXResourceWriter" class. 
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

namespace System.Resources
{

#if !ECMA_COMPAT

using System;
using System.IO;

public class ResXResourceWriter : IResourceWriter, IDisposable
{
	// Internal state.
	private Stream stream;
	private String fileName;
	private TextWriter writer;

	// Version of the resource format.
	public static readonly String Version = "1.3";

	// Constructors.
	public ResXResourceWriter(Stream stream)
			{
				this.stream = stream;
			}
	public ResXResourceWriter(String fileName)
			{
				this.fileName = fileName;
			}
	public ResXResourceWriter(TextWriter writer)
			{
				this.writer = writer;
			}

	// Destructor.
	~ResXResourceWriter()
			{
				Dispose(false);
			}

	// Implement the IResourceWriter interface.
	[TODO]
	public void AddResource(String name, byte[] value)
			{
				// TODO
			}
	[TODO]
	public void AddResource(String name, Object value)
			{
				// TODO
			}
	[TODO]
	public void AddResource(String name, String value)
			{
				// TODO
			}
	public void Close()
			{
				Dispose();
			}
	[TODO]
	public void Generate()
			{
				// TODO
			}

	// Dispose of this object.
	public void Dispose()
			{
				Dispose(true);
				GC.SuppressFinalize(this);
			}
	[TODO]
	protected virtual void Dispose(bool disposing)
			{
				// TODO
			}

}; // class ResXResourceWriter

#endif // !ECMA_COMPAT

}; // namespace System.Resources
