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
