/*
 * DirectoryInfo.cs - Implementation of the "System.IO.DirectoryInfo" class.
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

namespace System.IO
{

#if !ECMA_COMPAT

using System;

[Serializable]
public sealed class DirectoryInfo : FileSystemInfo
{
	// Constructor.
	public DirectoryInfo(String path)
			{
				if(path == null)
				{
					throw new ArgumentNullException("path");
				}
				else if(path.IndexOfAny(Path.InvalidPathChars) != -1)
				{
					throw new ArgumentException
						(_("IO_InvalidPathname"), "path");
				}
				OriginalPath = path;
				FullPath = Path.GetFullPath(path);
			}

	// Properties.
	public DirectoryInfo Parent
			{
				get
				{
					return new DirectoryInfo(Path.GetDirectoryName(FullPath));
				}
			}
	[TODO]
	public override bool Exists
			{
				get
				{
					// TODO - make sure that the path is a directory.
					return File.Exists(FullPath);
				}
			}
	public override String Name
			{
				get
				{
					return Path.GetFileName(FullPath);
				}
			}
	public DirectoryInfo Root
			{
				get
				{
					return new DirectoryInfo(Path.GetPathRoot(FullPath));
				}
			}

	// Create a directory.
	[TODO]
	public void Create()
			{
				Directory.CreateDirectory(FullPath);
				// TODO : refresh() ?
			}
	[TODO]
	public DirectoryInfo CreateSubdirectory(String name)
			{
				String dir = Path.Combine(FullPath, Path.GetFileName(name));
				Directory.CreateDirectory(dir);
				// TODO : refresh() ?
				return new DirectoryInfo(dir);
			}

	// Delete the directory represented by this object.
	public override void Delete()
			{
				Directory.Delete(FullPath, false);
			}
	public void Delete(bool recurse)
			{
				Directory.Delete(FullPath, recurse);
			}

	// Move this directory to a new location.
	public void MoveTo(String dest)
			{
				Directory.Move(FullPath, dest);
			}

	// Get the contents of this directory.
	public FileInfo[] GetFiles()
			{
				return GetFiles("*");
			}
	[TODO]
	public FileInfo[] GetFiles(String pattern)
			{
				// TODO
				return null;
			}
	public DirectoryInfo[] GetDirectories()
			{
				return GetDirectories("*");
			}
	[TODO]
	public DirectoryInfo[] GetDirectories(String pattern)
			{
				// TODO
				return null;
			}
	public FileSystemInfo[] GetFileSystemInfos()
			{
				return GetFileSystemInfos("*");
			}
	[TODO]
	public FileSystemInfo[] GetFileSystemInfos(String pattern)
			{
				// TODO
				return null;
			}

	// Convert this object into a string.
	public override String ToString()
			{
				return OriginalPath;
			}

}; // class FileInfo

#endif // !ECMA_COMPAT

}; // namespace System.IO
