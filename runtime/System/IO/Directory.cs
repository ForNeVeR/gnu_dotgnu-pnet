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
	using System.Text;
	using System.Private;
	using System.Collections;
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
		
		public static void Delete(string path, bool recursive)
		{
			Exception e=ValidatePath(path,"path");
			if(e != null) throw e;
	
			// remove any trailing directory sep characters
			if(path.GetChar(path.Length-1) == Path.DirectorySeparatorChar)
				path = path.Substring(0, path.Length - 1);
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
						foreach(string subFile in GetFiles(path))
						{
							File.Delete(path + 
							Path.DirectorySeparatorChar.ToString() + subFile);
						}

						foreach(string subDir in GetDirectories(path))
						{
							if(subDir == "." || subDir == "..")
								continue;
							Delete(path +Path.DirectorySeparatorChar.ToString() 
									+ subDir, recursive);
						}
						// now delete this dir
						Delete(path);
					}
					break;

				case Errno.EROFS:
					throw new IOException(_("IO_Error"));

				case Errno.EACCES:
					throw new SecurityException(_("IO_PathnameSecurity"));

				// Needs to be confirmed.
				case Errno.ENAMETOOLONG:					
					throw new PathTooLongException();

				case Errno.Success:
					return;
				// TODO
				// Throw some appropriate exception.
				default:
					throw new ArgumentException();
			}
		}

		public static bool Exists(string path)
		{
			Exception e=ValidatePath(path,"path");
			if(e != null) throw e;
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

		private static Exception GetErrnoExceptions(Errno errno,String path)
		{
			switch(errno)
			{
				case Errno.ENOENT:
					return new DirectoryNotFoundException(_("IO_DirNotFound"));
				case Errno.ENOTDIR:
					return new IOException(String.Format(_("IO_IsNotDir"),
											path));
				case Errno.EACCES:
					return new SecurityException(_("IO_PathnameSecurity"));
				case Errno.ENAMETOOLONG:
					return new PathTooLongException();
			}
			return null;
		}

		public static string GetCurrentDirectory()
		{
			// TODO: security ?
			return DirMethods.GetCurrentDirectory();
		}

		public static string[] GetDirectories(string path)
		{
			try
			{
				return InternalGetDirectoryEntries(path,"*",
										FileType.directory);
			}
			catch(Exception e)
			{
				throw e; // re-source exception to be like ECMA
			}
		}

		public static string[] GetDirectories(string path, string searchPattern)
		{
			try
			{
				return InternalGetDirectoryEntries(path,searchPattern,
										FileType.directory);
			}
			catch(Exception e)
			{
				throw e;
			}
		}

		public static string GetDirectoryRoot(string path)
		{
			// no difference except for "\\ecmatest"
			// in Directory shoud give "C:\\" and in Path should give "\\"
			return Path.GetPathRoot(path); 
		}

		public static string[] GetFileSystemEntries(string path)
		{
			try
			{
				return InternalGetDirectoryEntries(path,"*",
												(FileType)(-1)); // all types
			}
			catch(Exception e)
			{
				throw e;
			}
		}

		public static string[] GetFileSystemEntries(string path, 
													string searchPattern)
		{
			try
			{
				return InternalGetDirectoryEntries(path,searchPattern,
														(FileType)(-1));
			}
			catch(Exception e)
			{
				throw e;
			}
		}


		public static string[] GetFiles(string path)
		{
			try
			{
				return InternalGetDirectoryEntries(path,"*",
										FileType.regularFile);
			}
			catch(Exception e)
			{
				throw e;
			}
		}

		public static string[] GetFiles(string path, string searchPattern)
		{
			try
			{
				return InternalGetDirectoryEntries(path,searchPattern,
										FileType.regularFile);
			}
			catch(Exception e)
			{
				throw e;
			}
		}
	
		private static String[] InternalGetDirectoryEntries(String path,
														String searchPattern,
														FileType type)
		{
			Exception e=ValidatePath(path,"path");
			if(e != null) throw e;

			InternalFileInfo []dirEnts;
			Errno errno=DirMethods.GetFilesInDirectory(path,out dirEnts);
			
			e=GetErrnoExceptions(errno,path);
			if(e != null) throw e;
			
			GlobMatch fnmatch = new GlobMatch(searchPattern);
			
			ArrayList list=new ArrayList(dirEnts.Length);
			foreach(InternalFileInfo info in dirEnts)
			{
				if(fnmatch.Match(info.fileName) 
						&& 	(type == (FileType)(-1) || info.fileType == type ))
				{
					list.Add(info.fileName);
				}
			}
			String[] retval=new String[list.Count];
			list.CopyTo(retval);
			return retval;
		}

		public static DateTime GetLastAccessTime(string path)
		{
			return File.GetLastAccessTime(path);
		}

		public static DateTime GetLastWriteTime(string path)
		{
			return File.GetLastWriteTime(path);
		}

		public static void Move(string sourceDirName, string destDirName)
		{
			Exception e=ValidatePath(sourceDirName,"sourceDirName");
			if(e != null) throw e;

			e=ValidatePath(destDirName,"destDirName");
			if(e != null) throw e;
			
			Errno errno = DirMethods.Rename(sourceDirName, destDirName);
			e=GetErrnoExceptions(errno, sourceDirName);
			if(e != null) throw e;
		}

		[TODO]
		public static void SetCreationTime(string path, DateTime creationTime)
		{
			Exception e=ValidatePath(path,"path");
			if(e != null) throw e;
		}

		public static void SetCurrentDirectory(string path)
		{
			Exception e=ValidatePath(path,"path");
			if(e != null) throw e;
			
			Errno errno=DirMethods.ChangeDirectory(path);

			e=GetErrnoExceptions(errno,path);
			if(e != null) throw e;
		}

		[TODO]
		public static void SetLastAccessTime(string path, DateTime lastAccessTime)
		{
			Exception e=ValidatePath(path,"path");
			if(e != null) throw e;
		}

		[TODO]
		public static void SetLastWriteTime(string path, DateTime lastWriteTime)
		{
			Exception e=ValidatePath(path,"path");
			if(e != null) throw e;
		}
		
		/* internal class to convert Glob expression to Regexp */
		private class GlobMatch : IDisposable
		{
			Regexp expr;
			String pattern;
			String translated;
			bool disposed=false;
			
			internal GlobMatch(String glob) 
			{
				StringBuilder builder=new StringBuilder();
				pattern=glob;
				char[] arr=glob.ToCharArray();
				for(int i=0;i<arr.Length;i++)
				{
					switch(arr[i])
					{
						case '.':
						{
							if(i < arr.Length-1)
							{
								if(arr[i+1]=='?')
								{
									builder.Append("\\.{0,1}");
									i=i+1;
								}
								else if(arr[i+1]=='*')
								{
									builder.Append(".*");
									i=i+1;
								}
								else
								{
									builder.Append("\\.");
								}
							}
							else
							{
								builder.Append("\\.");
							}
						}
						break;
					case '*':
						builder.Append(".*");
						break;
					case '?':
						builder.Append(".");
						break;
					case '[':
					case ']':
					case '^':
					case '\\':
						builder.Append("\\"+arr[i]);
						break;
					default:
						builder.Append(arr[i]);
						break;
					}
				}
				translated="^"+builder.ToString()+"$"; //perfect match
				expr=new Regexp(translated);
			}

			public bool Match(String input)
			{
				if(input==null)
					throw new ArgumentNullException("input");
				return expr.Match(input);
			}

			public void Dispose()
			{
				if(!disposed)expr.Dispose();
				disposed=true;
			}

			void IDisposable.Dispose()
			{
				this.Dispose();
			}

		} // class GlobMatch

		private static Exception ValidatePath(string path,string name)
		{
			if (path == null)
			{
				return new ArgumentNullException(_("Arg_NotNull"),name);
			}
			if ((path.Trim() == "") || !(FileMethods.ValidatePathname(path))
				|| path.IndexOfAny(Path.InvalidPathChars)!=-1)
			{
				return new ArgumentException(_("IO_InvalidPathname"),name);
			}
			return null;
		}
	} // class Directory 
} // namespace System.IO
