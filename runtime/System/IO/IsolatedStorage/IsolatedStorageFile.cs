/*
 * IsolatedStorageFile.cs - Implementation of the
 *		"System.IO.IsolatedStorage.IsolatedStorageFile" class.
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

namespace System.IO.IsolatedStorage
{

#if !ECMA_COMPAT

using System.Collections;
using System.Security;
using System.Security.Permissions;
using System.Security.Policy;

public sealed class IsolatedStorageFile : IsolatedStorage, IDisposable
{
	// Constructor.
	internal IsolatedStorageFile() {}

	// Destructor.
	~IsolatedStorageFile()
			{
				Close();
			}

	// Get the current amount of space that has be used.
	[TODO]
	[CLSCompliant(false)]
	public override ulong CurrentSize
			{
				get
				{
					// TODO
					return 0;
				}
			}

	// Get the maximum amount of space that can be used.
	[TODO]
	[CLSCompliant(false)]
	public override ulong MaximumSize
			{
				get
				{
					// TODO
					return base.MaximumSize;
				}
			}

	// Close this storage area.
	[TODO]
	public void Close()
			{
				// TODO
			}

	// Create a directory within this storage area.
	[TODO]
	public void CreateDirectory(String dir)
			{
				// TODO
			}

	// Delete a file from this storage area.
	[TODO]
	public void DeleteFile(String file)
			{
				// TODO
			}

	// Delete a directory from this storage area.
	[TODO]
	public void DeleteDirectory(String dir)
			{
				// TODO
			}

	// Dispose this storage area.
	public void Dispose()
			{
				Close();
			}

	// Get a list of directories from a storage area directory.
	[TODO]
	public String[] GetDirectoryNames(String searchPattern)
			{
				// TODO
				return null;
			}

	// Get a list of files from a storage area directory.
	[TODO]
	public String[] GetFileNames(String searchPattern)
			{
				// TODO
				return null;
			}

	// Get isolated storage permission information from a permission set.
	protected override IsolatedStoragePermission
				GetPermission(PermissionSet ps)
			{
				if(ps == null)
				{
					return null;
				}
				else if(ps.IsUnrestricted())
				{
					return new IsolatedStorageFilePermission
						(PermissionState.Unrestricted);
				}
				else
				{
					return (IsolatedStoragePermission)
					  (ps.GetPermission(typeof(IsolatedStorageFilePermission)));
				}
			}

	// Get an isolated storage area.
	[TODO]
	public static IsolatedStorageFile GetStore
				(IsolatedStorageScope scope, Object domainIdentity,
				 Object assemblyIdentity)
			{
				// TODO
				return null;
			}
	[TODO]
	public static IsolatedStorageFile GetStore
				(IsolatedStorageScope scope, Type domainEvidenceType,
				 Type assemblyEvidenceType)
			{
				// TODO
				return null;
			}
	[TODO]
	public static IsolatedStorageFile GetStore
				(IsolatedStorageScope scope, Evidence domainEvidence,
				 Type domainEvidenceType, Evidence assemblyEvidence,
				 Type assemblyEvidenceType)
			{
				// TODO
				return null;
			}

	// Get the user storage area for the current assembly.
	[TODO]
	public static IsolatedStorageFile GetUserStoreForAssembly()
			{
				// TODO
				return null;
			}

	// Get the user storage area for the current domain.
	[TODO]
	public static IsolatedStorageFile GetUserStoreForDomain()
			{
				// TODO
				return null;
			}

	// Remove this isolated storage object.
	[TODO]
	public override void Remove()
			{
				// TODO
			}

	// Remove all stores of a particular type.
	[TODO]
	public static void Remove(IsolatedStorageScope scope)
			{
				// TODO
			}

	// Enumerate all storage areas of a particular type.
	[TODO]
	public static IEnumerator GetEnumerator(IsolatedStorageScope scope)
			{
				// TODO
				return null;
			}

	// Get the base directory for this isolated storage area.
	internal String BaseDirectory
			{
				get
				{
					// TODO
					return null;
				}
			}

}; // class IsolatedStorageFile

#endif // !ECMA_COMPAT

}; // namespace System.IO.IsolatedStorage
