/*
 * Directory.cs - Implementation of the "System.IO.Directory" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * Contribution from Abhaya Agarwal  <abhayag@iitk.ac.in>
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
	using System;
	using System.Security;
	using Platform;

	public sealed class Directory 
	{
		
		private static PathInfo pathinfo = DirMethods.GetPathInfo();

		private Directory()
		{
			// private constructor... do nothing
		}

		public static void Delete(string path)
		{
			Delete(path,false);	
		}
		
		[TODO]
		public static void Delete(string path, bool recursive)
		{
			if(path==null)
			{
				throw new ArgumentNullException();
			}
			if(path.Length==0 || (path.Trim()).Length==0 || path.IndexOfAny(pathinfo.invalidPathChars)!= -1)
			{
				throw new ArgumentException();
			}

			Errno errno = DirMethods.Delete(path);

			switch(errno)
			{
				case Errno.ENOENT:
					throw new DirectoryNotFoundException(_("IO_DirNotFound"));

				case Errno.ENOTEMPTY:
					if(!recursive)
					{
						throw new IOException(_("IO_Error"));
					}
					else
					{
						// TODO
					}
					break;

				case Errno.EROFS:
					throw new IOException(_("IO_Error"));

				case Errno.EACCES:
					throw new SecurityException(_("IO_PathnameSecurity"));

				// Needs to be confirmed.
				case Errno.ENAMETOOLONG:					
					throw new PathTooLongException();
				// TODO
				// Throw some appropriate exception.
				default:
					throw new ArgumentException();
			}
		}

		public static bool Exists(string path)
		{
			if(path.Length==0 || (path.Trim()).Length==0 || path.IndexOfAny(pathinfo.invalidPathChars)!= -1)
			{	
				throw new ArgumentException();
			}
			if (path == null)
			{
				return false;
			}	
			long ac;
			Errno errno = DirMethods.GetLastAccess(path, out ac);
			switch(errno)
			{
				case Errno.Success:
					return true;
				case Errno.ENOENT:
					throw new DirectoryNotFoundException(_("IO_DirNotFound"));
				case Errno.ENOTDIR:
					return false;
				case Errno.EACCES:
					return false;
				case Errno.ENAMETOOLONG:
					throw new PathTooLongException();
				default:
					return false;
			}
		
		}

		public static DateTime GetCreationTime(string path)
		{
			return File.GetCreationTime(path);
		}

		[TODO]
		public static string GetCurrentDirectory()
		{
			return null;
		}

		[TODO]
		public static string[] GetDirectories(string path)
		{
			return null;
		}

		[TODO]
		public static string[] GetDirectories(string path, string searchPattern)
		{
			return null;
		}

		[TODO]
		public static string GetDirectoryRoot(string path)
		{
			return null;
		}

		[TODO]
		public static string[] GetFileSystemEntries(string path)
		{
			return null;
		}

		[TODO]
		public static string[] GetFileSystemEntries(string path, string searchPattern)
		{
			return null;
		}

		[TODO]
		public static string[] GetFiles(string path)
		{
			return null;
		}

		[TODO]
		public static string[] GetFiles(string path, string searchPattern)
		{
			return null;
		}

		public static DateTime GetLastAccessTime(string path)
		{
			return File.GetLastAccessTime(path);
		}

		public static DateTime GetLastWriteTime(string path)
		{
			return File.GetLastWriteTime(path);
		}

		[TODO]
		public static void Move(string sourceDirName, string destDirName)
		{
		}

		[TODO]
		public static void SetCreationTime(string path, DateTime creationTime)
		{
		}

		[TODO]
		public static void SetCurrentDirectory(string path)
		{
		}

		[TODO]
		public static void SetLastAccessTime(string path, DateTime lastAccessTime)
		{
		}

		[TODO]
		public static void SetLastWriteTime(string path, DateTime lastWriteTime)
		{
		}

	}
}
