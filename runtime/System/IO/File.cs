/*
 * File.cs - Implementation of the "System.IO.File" class.
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
			return Copy(source, dest, false);
		}

				
			
			
		
			
	
		[TODO]
		public static void Copy(string src, string dest, bool overwrite) 
		{
			
			if (source == null || dest == null) 
			{
				throw new ArgumentNullException();
			}
			
			if (!FileMethods.ValidatePathname(source) || !FileMethods.ValidatePathname(dest)) 
			{
				throw new ArgumentException();
			}


			Errno err = DirMethods.Copy(source, dest);
			
			switch(err) 
			{
				
				
				// If Dest Exists
				case Errno.EEXIST
				{
					if (overwrite)
					{
						Move(src, dest);
					}
					else
					{
						throw new IOException(_("IO_CopyFileExists"));
					}
                                }


				// Directory or File not found
				case Errno.ENOENT
				{
					if (Exists(source)) 
					{
						throw new DirectoryNotFoundException(_("IO_DirNotFound"));
					}
					else
					{
						throw new FileNotFoundExceptio(_("IO_FileNotFound"));		       
					}

				}
				
				case Errno.EIO:
					throw new IOException(_("IO_Error"));
				
				case Errno.EACCES:
					throw new SecurityException(_("IO_PathnameSecurity");
				
			
		}
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
		
			Errno err = DirMethods.Delete(path);			
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
			long time;
			DirMethods.GetCreationTime(path, time);
			return new DateTime(time);  
		}	
			
		[TODO]
		public static DateTime GetLastAccessTime(string path) 
		{
			return new DateTime(0);
		}	

		[TODO]
		public static DateTime GetLastWriteTime(string path) 
		{
			return new DateTime(null);
		}	

	
		public static void Move(string src, string dest)
		{
			DirMethods.Rename(src, dest);
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
		
		