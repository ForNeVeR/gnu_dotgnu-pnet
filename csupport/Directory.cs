/*
 * Directory.cs - Represents a pnetC directory stream.
 *
 * This file is part of the Portable.NET "C language support" library.
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

namespace OpenSystem.C
{

using System;
using System.IO;
using System.Runtime.InteropServices;
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
