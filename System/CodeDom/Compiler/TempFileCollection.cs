/*
 * TempFileCollection.cs - Implementation of the
 *		System.CodeDom.Compiler.TempFileCollection class.
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

namespace System.CodeDom.Compiler
{

#if CONFIG_CODEDOM

using System.Collections;

public class TempFileCollection : ICollection, IEnumerable, IDisposable
{
	// Internal state.
	private String tempDir;
	private bool keepFiles;
	private Hashtable files;

	// Constructors.
	public TempFileCollection() : this(null, false) {}
	public TempFileCollection(String tempDir) : this(tempDir, false) {}
	public TempFileCollection(String tempDir, bool keepFiles)
			{
				this.tempDir = tempDir;
				this.keepFiles = keepFiles;
				this.files = new Hashtable();
			}

	// Destructor.
	~TempFileCollection()
			{
				Dispose(false);
			}

	// Get an enumerator for this collection.
	public IEnumerator GetEnumerator()
			{
				return files.Keys.GetEnumerator();
			}

	// Properties.
	[TODO]
	public String BasePath
			{
				get
				{
					// TODO
					return null;
				}
			}
	public int Count
			{
				get
				{
					return files.Count;
				}
			}
	public bool KeepFiles
			{
				get
				{
					return keepFiles;
				}
				set
				{
					keepFiles = value;
				}
			}
	public String TempDir
			{
				get
				{
					if(tempDir != null)
					{
						return tempDir;
					}
					else
					{
						return String.Empty;
					}
				}
			}

	// Implement the ICollection interface.
	int ICollection.Count
			{
				get
				{
					return files.Count;
				}
			}
	void ICollection.CopyTo(Array array, int index)
			{
				files.CopyTo(array, index);
			}
	bool ICollection.IsSynchronized
			{
				get
				{
					return false;
				}
			}
	Object ICollection.SyncRoot
			{
				get
				{
					return null;
				}
			}

	// Implement the IEnumerable interface.
	IEnumerator IEnumerable.GetEnumerator()
			{
				return files.Keys.GetEnumerator();
			}

	// Implement the IDisposable interface.
	void IDisposable.Dispose()
			{
				Dispose(true);
			}

	// Add an extension to a temporary file.
	public String AddExtension(String fileExtension)
			{
				return AddExtension(fileExtension, keepFiles);
			}
	[TODO]
	public String AddExtension(String fileExtension, bool keepFile)
			{
				// TODO
				return null;
			}

	// Add a file to this temporary file collection.
	[TODO]
	public void AddFile(String fileName, bool keepFile)
			{
				// TODO
			}

	// Copy the contents of this collection to an array.
	public void CopyTo(String[] fileNames, int index)
			{
				files.Keys.CopyTo(fileNames, index);
			}

	// Delete the temporary files in this collection.
	[TODO]
	public void Delete()
			{
				// TODO
			}

	// Dispose this collection.
	protected virtual void Dispose(bool disposing)
			{
				Delete();
			}

}; // class TempFileCollection

#endif // CONFIG_CODEDOM

}; // namespace System.CodeDom.Compiler
