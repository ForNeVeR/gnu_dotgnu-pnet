/*
 * ResXResourceReader.cs - Implementation of the
 *			"System.Resources.ResXResourceReader" class. 
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
using System.Collections;
using System.ComponentModel.Design;
using System.Xml;

public class ResXResourceReader : IResourceReader, IEnumerable, IDisposable
{
	// Internal state.
	private Stream stream;
	private TextReader reader;
	private String fileName;
	private Hashtable loadedData;
#if CONFIG_COMPONENT_MODEL_DESIGN
	private ITypeResolutionService typeResolver;
#endif

	// Constructors.
	public ResXResourceReader(Stream stream)
			{
				this.stream = stream;
			}
	public ResXResourceReader(String fileName)
			{
				this.fileName = fileName;
			}
	public ResXResourceReader(TextReader reader)
			{
				this.reader = reader;
			}
#if CONFIG_COMPONENT_MODEL_DESIGN
	public ResXResourceReader(Stream stream,
							  ITypeResolutionService typeResolver)
			: this(stream)
			{
				this.typeResolver = typeResolver;
			}
	public ResXResourceReader(String fileName,
							  ITypeResolutionService typeResolver)
			: this(fileName)
			{
				this.typeResolver = typeResolver;
			}
	public ResXResourceReader(TextReader reader,
							  ITypeResolutionService typeResolver)
			: this(reader)
			{
				this.typeResolver = typeResolver;
			}
#endif

	// Destructor.
	~ResXResourceReader()
			{
				Dispose(false);
			}

	// Implement the IResourceReader interface.
	public void Close()
			{
				((IDisposable)this).Dispose();
			}
	public IDictionaryEnumerator GetEnumerator()
			{
				if(loadedData == null)
				{
					loadedData = new Hashtable();
					XmlTextReader xmlReader;
					TextReader fileReader = null;
					if(fileName != null)
					{
						fileReader = new StreamReader(fileName);
						xmlReader = new XmlTextReader(fileReader);
					}
					else if(stream != null)
					{
						xmlReader = new XmlTextReader(stream);
					}
					else
					{
						xmlReader = new XmlTextReader(reader);
					}
					try
					{
						Load(xmlReader);
					}
					finally
					{
						xmlReader.Close();
						if(fileReader != null)
						{
							fileReader.Close();
						}
					}
				}
				return loadedData.GetEnumerator();
			}

	// Implement the IDisposable interface.
	void IDisposable.Dispose()
			{
				Dispose(true);
				GC.SuppressFinalize(this);
			}

	// Implement the IEnumerable interface.
	IEnumerator IEnumerable.GetEnumerator()
			{
				return GetEnumerator();
			}

	// Dispose of this resource reader.
	protected virtual void Dispose(bool disposing)
			{
				if(stream != null)
				{
					stream.Close();
					stream = null;
				}
				if(reader != null)
				{
					reader.Close();
					reader = null;
				}
			}

	// Load resources from a file contents string.
	public static ResXResourceReader FromFileContents(String fileContents)
			{
				return new ResXResourceReader
					(new StringReader(fileContents));
			}
#if CONFIG_COMPONENT_MODEL_DESIGN
	public static ResXResourceReader FromFileContents
				(String fileContents, ITypeResolutionService typeResolver)
			{
				return new ResXResourceReader
					(new StringReader(fileContents), typeResolver);
			}
#endif

	// Load the contents of an XML resource stream.
	[TODO]
	private void Load(XmlTextReader reader)
			{
				// TODO
			}

}; // class ResXResourceReader

#endif // !ECMA_COMPAT

}; // namespace System.Resources
