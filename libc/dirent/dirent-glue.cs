/*
 * dirent-glue.cs - Glue between dirent and the C# system library.
 *
 * This file is part of the Portable.NET C library.
 * Copyright (C) 2003  Free Software Foundation, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

using System;
using System.Runtime.InteropServices;
using OpenSystem.C;

namespace OpenSystem.C
{

using System.IO;
using System.Security;

public class Directory
{
	// Internal state.
	private long pos;
	private String[] entries;
	private String name;
	private int nameMax;

	// Constructor.
	public Directory(IntPtr cname, IntPtr err, int nameMax)
			{
				this.pos = 0;
				this.nameMax = nameMax;
				try
				{
					name = Marshal.PtrToStringAnsi(cname);
					name = Path.GetFullPath(name);
					String[] tmp = name.Split(Path.DirectorySeparatorChar,
					                          Path.AltDirectorySeparatorChar);
					for(int i = 0; i < tmp.Length; ++i)
					{
						if(tmp[i].Length > nameMax)
						{
							Marshal.WriteInt32(err, 36); // ENAMETOOLONG
							return;
						}
					}
				}
				catch(ArgumentException)
				{
					Marshal.WriteInt32(err, 2); // ENOENT
					return;
				}
				catch(SecurityException)
				{
					Marshal.WriteInt32(err, 13); // EACCES
					return;
				}
				catch(PathTooLongException)
				{
					Marshal.WriteInt32(err, 36); // ENAMETOOLONG
					return;
				}
				Rewind(err);
			}

	// Property.
	public long Pos
			{
				get { return pos; }
				set { pos = value; }
			}

	// Methods.
	public IntPtr Read(IntPtr err, IntPtr len)
			{
				if(pos == entries.Length)
				{
					return IntPtr.Zero;
				}
				if(pos >= 0 && pos < entries.Length)
				{
					String s = entries[pos++];
					int length = s.Length;
					if(length > nameMax)
					{
						Marshal.WriteInt32(err, 75); // EOVERFLOW
						length = nameMax;
					}
					Marshal.WriteInt32(len, length);
					return Marshal.StringToHGlobalAnsi(s);
				}
				Marshal.WriteInt32(err, 2); // ENOENT
				return IntPtr.Zero;
			}
	public void Rewind(IntPtr err)
			{
				try
				{
					entries = System.IO.Directory.GetFileSystemEntries(name);
				}
				catch(ArgumentException)
				{
					Marshal.WriteInt32(err, 2); // ENOENT
					return;
				}
				catch(SecurityException)
				{
					Marshal.WriteInt32(err, 13); // EACCES
					return;
				}
				catch(DirectoryNotFoundException)
				{
					Marshal.WriteInt32(err, 2); // ENOENT
					return;
				}
				catch(PathTooLongException)
				{
					Marshal.WriteInt32(err, 36); // ENAMETOOLONG
					return;
				}
				catch(IOException)
				{
					Marshal.WriteInt32(err, 20); // ENOTDIR
					return;
				}

				int nlen = name.Length;
				if(name[nlen-1] != Path.DirectorySeparatorChar &&
				   name[nlen-1] != Path.AltDirectorySeparatorChar)
				{
					++nlen;
				}
				for(int i = 0; i < entries.Length; ++i)
				{
					entries[i] = entries[i].Substring(0, nlen);
				}
			}

}; // class Directory

}; // namespace OpenSystem.C

__module
{
	// Free the underlying managed object for a pnetC directory stream.
	public static void __syscall_closedir(IntPtr gc_handle, IntPtr err)
			{
				GCHandle handle = (GCHandle)gc_handle;
				Directory dir = handle.Target as Directory;
				if(dir == null)
				{
					Marshal.WriteInt32(err, 9); // EBADF
					return;
				}
				lock(dir)
				{
					handle.Free();
				}
			}

	// Get the underlying managed object for a pnetC directory stream.
	public static IntPtr __syscall_opendir(IntPtr cname, IntPtr err, int nameMax)
			{
				Directory dir = new Directory(cname, err, nameMax);
				if(Marshal.ReadInt32(err) != 0)
				{
					return IntPtr.Zero;
				}
				return (IntPtr)GCHandle.Alloc(dir);
			}

	// Read an entry from a directory stream.
	// Return value must be freed using Marshal::FreeHGlobal(IntPtr).
	public static IntPtr __syscall_readdir(IntPtr gc_handle, IntPtr err, IntPtr len)
			{
				GCHandle handle = (GCHandle)gc_handle;
				Directory dir = handle.Target as Directory;
				if(dir == null)
				{
					Marshal.WriteInt32(err, 9); // EBADF
					return IntPtr.Zero;
				}
				lock(dir)
				{
					return dir.Read(err, len);
				}
			}

	// Rewind a directory stream's position to the beginning.
	public static void __syscall_rewinddir(IntPtr gc_handle, IntPtr err)
			{
				GCHandle handle = (GCHandle)gc_handle;
				Directory dir = handle.Target as Directory;
				if(dir == null)
				{
					Marshal.WriteInt32(err, 9); // EBADF
					return;
				}
				lock(dir)
				{
					dir.Rewind(err);
				}
			}

	// Seek to the given position in a directory stream.
	public static void __syscall_seekdir(IntPtr gc_handle, long pos)
			{
				GCHandle handle = (GCHandle)gc_handle;
				Directory dir = handle.Target as Directory;
				if(dir == null) { return; }
				lock(dir)
				{
					dir.Pos = pos;
				}
			}

	// Get the current position of a directory stream.
	public static long __syscall_telldir(IntPtr gc_handle)
			{
				GCHandle handle = (GCHandle)gc_handle;
				Directory dir = handle.Target as Directory;
				if(dir == null) { return -1l; }
				lock(dir)
				{
					return dir.Pos;
				}
			}

} // __module
