/*
 * ResourceWriter.cs - Implementation of the
 *		"System.Resources.ResourceWriter" class.
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

namespace System.Resources
{

#if CONFIG_RUNTIME_INFRA

using System;
using System.IO;
using System.Collections;
using System.Globalization;

#if ECMA_COMPAT
internal
#else
public
#endif
sealed class ResourceWriter : IDisposable, IResourceWriter
{
	// Internal state.
	private Stream stream;
	private bool generateDone;
	private Hashtable table;
	private Hashtable ignoreCaseNames;
	private TextInfo info;

	// Constructors.
	public ResourceWriter(Stream stream)
			{
				if(stream == null)
				{
					throw new ArgumentNullException("stream");
				}
				else if(!stream.CanWrite)
				{
					throw new ArgumentException
						(_("IO_NotSupp_Write"), "stream");
				}
				this.stream = stream;
				generateDone = false;
				table = new Hashtable();
				ignoreCaseNames = new Hashtable();
				info = CultureInfo.InvariantCulture.TextInfo;
			}
	public ResourceWriter(String fileName)
			: this(new FileStream(fileName, FileMode.CreateNew,
								  FileAccess.Write))
			{
				// Nothing to do here.
			}

	// Add a string to this writer.
	private void AddString(String name, String value)
			{
				// See if we already have the name.
				String lowerName = info.ToLower(name);
				if(ignoreCaseNames.Contains(lowerName))
				{
					throw new ArgumentException
						(_("Arg_ResourceAlreadyPresent"), "value");
				}

				// Add the name to "table".
				table.Add(name, value);

				// Add the lower-case name to "ignoreCaseNames".
				ignoreCaseNames.Add(lowerName, String.Empty);
			}

	// Implement the IDisposable interface.
	public void Dispose()
			{
				Close();
			}

	// Implement the IResourceWriter interface.
	public void AddResource(String name, byte[] value)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				else if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				else if(stream == null)
				{
					throw new InvalidOperationException
						(_("Invalid_ResourceWriterClosed"));
				}
				// TODO: we don't support binary resources yet.
			}
	public void AddResource(String name, Object value)
			{
				// TODO: we don't support non-string resources yet.
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				else if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				else if(stream == null)
				{
					throw new InvalidOperationException
						(_("Invalid_ResourceWriterClosed"));
				}
				else if(value is String)
				{
					AddString(name, (String)value);
				}
			}
	public void AddResource(String name, String value)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				else if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				else if(stream == null)
				{
					throw new InvalidOperationException
						(_("Invalid_ResourceWriterClosed"));
				}
				else
				{
					AddString(name, value);
				}
			}
	public void Close()
			{
				if(stream != null)
				{
					if(!generateDone)
					{
						Generate();
					}
					stream.Close();
					stream = null;
				}
			}
	public void Generate()
			{
				// TODO
			}

}; // class ResourceWriter

#endif // CONFIG_RUNTIME_INFRA

}; // namespace System.Resources
