/*
 * Path.cs - Implementation of the "System.IO.File" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  FreeDevelopers.net
 * Contributions from Charlie Carnow <carnow@gmx.net>
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
	using Platform;
	using System;
	using System.Security.SecurityException;
	public sealed class File
	{
		public static StreamWriter AppendText(string path) 
		{
			return new StreamWriter(path, true);
		}	
	
		[TODO]
		public static void Copy(string source, string dest) 
		{
		}
	
		[TODO]
		public static void Copy(string src, string dest, bool overwrite) 
		{
		}
	
		public static FileStream Create(string path) 
		{
		return Create(path, FileStream.DefaultBufferSize);
		}

		public static FileStream Create(string path, int buffersize) 
		{	
			return new FileStream(path, FileMode.Create, FileAccess.ReadWrite, FileShare.None, buffersize);
		}
	
		public static StreamWriter CreateText(string path) 
		{
			return new StreamWriter(path, false);
		}

		[TODO]
		public static void Delete(string path) 
		{
			if (path == null) 
			{
				throw new ArgumentNullException();
			}
		
			if (FileMethods.ValidatePathname(path) == false) 
			{
				throw new ArgumentException();
			}
		
			/* TODO: Deleting stuff */			
		}

		public static bool Exists(string path) 
		{
			
			try
			{
				FileStream test = Open(path, FileMode.Open);   
			}
			catch (Exception e) 
			{
				return false;
			} 
			
			test.Close();
			
			return true;
		}
		
		[TODO]
		public static DateTime GetCreationTime(string path)
		{
			return null;
		}	
			
		[TODO]
		public static DateTime GetLastAccessTime(string path) 
		{
			return null;
		}	

		[TODO]
		public static DateTime GetLastWriteTime(string path) 
		{
			return null;
		}	

	
		public static void Move(string src, string dest)
		{
			// TODO: Add Exception catching
			FileStream read = new FileStream(src, FileAccess.Read);
			FileStream write = new FileStream(dest, FileAccess.Write);
			new byte[read.Length] writer;
			read.Read(writer, 0, read.Length);
			write.Write(writer, 0, read.Length);
			read.Close();
			write.Close();
		}
		
		
		public static FileStream Open(string path, FileMode mode) 
		{
			return new FileStream(path, mode, FileAccess.ReadWrite, FileShare.None);
		}
		
		public static FileStream Open(string path, FileMode mode,
					      FileAccess access)
		{
			return new FileStream(path, mode, access, 
						FileShare.None);		
	
		}


		public static FileStream Open(string path, FileMode mode,
					      FileAccess access, FileShare s) 
		{
			return new FileStream(path, mode, access, s);
		}
		
		public static FileStream OpenRead(string path) 
		{
			return new FileStream(path, FileMode.Open, 
					      FileAccess.Read, FileShare.Read);
		}

		public static StreamReader OpenText(string path) 
		{
			return new StreamReader(path);
		}

		public static FileStream OpenWrite(string path) 
		{
			return new FileStream(path, FileMode.OpenOrCreate,
					      FileAccess.Write, FileShare.None);
		}
		
		[TODO]	
		public static void SetCreationTime(string path, DateTime time) 
		{
		}

		[TODO]
		public static void SetLastWriteTime(string path, DateTime time)
		{
		}
	}
		
}			
		
		