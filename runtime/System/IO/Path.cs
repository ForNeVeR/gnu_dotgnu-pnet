/*
 * Path.cs - Implementation of the "System.IO.Path" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * Contributions from Charlie Carnow <carnow@gmx.net>
 * Contributions from Gopal V <gopalv82@symonds.net>
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
	using Platform;
	public sealed class Path 
	{
		// I know of the existence of System.Enviornment
		// But I'm opting to just get the separators
		// from DirMethods.GetPathInfo()
		private static PathInfo p = DirMethods.GetPathInfo();
		public static readonly char DirectorySeparatorChar = p.dirSeparator;
		public static readonly char AltDirectorySeparatorChar = p.altDirSeparator;
		public static readonly char VolumeSeparatorChar = p.volumeSeparator;
		public static readonly char PathSeparatorChar = p.pathSeparator;
		public static readonly char[] InvalidPathChars = p.invalidPathChars;
		private static readonly char[] PathSeparatorChars = { DirectorySeparatorChar, AltDirectorySeparatorChar, VolumeSeparatorChar};
			
		// entropy for GetTempFilename()
		static Random r=new Random();

		public static String ChangeExtension(String path, String extension)
		{
			if(!HasExtension(path))return path;
			String fname=path.Substring(0,path.LastIndexOf('.'));
			return fname+'.'+extension;
		}
		
		
		public static String Combine(String path1, String path2)
		{

			if (path1.Length == 0) 
			{
				return path2;
			}
			else if (path2.Length == 0)
			{
				return path1;
			}
			if (path1 == null || path2 == null)
			{
				throw new ArgumentNullException();
			}
			
			/* TODO:Check for implementation-defined invalid
			 * chars
			 */
			
			if (path1.EndsWith(new String(DirectorySeparatorChar, 1)) == false 
			    && path2.EndsWith(new String(AltDirectorySeparatorChar,1))
				== false) 
			{
				 // Insert at starting at last element
				 path1 = path1.Insert(path1.Length, new String(DirectorySeparatorChar, 1));
		        }

			return String.Concat(path1, path2);
		}
		
		public static String GetDirectoryName(String path)
		{
			if(path==null)return null;
	
			bool isDir=path.EndsWith(""+DirectorySeparatorChar);
			String retval;
			if(!isDir)
			{
				int len=path.Length-GetFileName(path).Length;
				if(len >1 && path[len-1]==DirectorySeparatorChar)len--;
				// skip last slash for all dirs except '/file' => '/'
				retval=path.Substring(0,len);
			}
			else
			{
				if(path.Length==1)return null; // Viz '/'
				if(path.Length==3 && path[1]==VolumeSeparatorChar)
					return null; // C:\
				retval=path.Substring(0,path.Length-1);
			}
			return retval;
		}

		public static String GetFileName(String path)
		{
			if(path==null)return null;
			if(path.Length==0)return String.Empty;
		    char[] separator = {DirectorySeparatorChar,AltDirectorySeparatorChar};
			/* Optimize: Replace with LastIndexOf */
		    string[] dirs = path.Split(separator);
			return dirs[dirs.Length-1];
		}
		
		public static String GetFileNameWithoutExtension(String path)
		{
			String fname=GetFileName(path);
			if(fname.IndexOf('.')==-1)return fname;
			return	fname.Substring(0,fname.LastIndexOf('.'));
		}
		
		public static String GetFullPath(String path)
		{
			if(path==null)return null;

			String dir=path;
			if(!IsPathRooted(dir))dir=DirMethods.GetCurrentDirectory()+'/'+
				path;
			dir=Normalize(dir);	
			// TODO: Permission check
			return dir;
		}
		
		private static String Normalize(String path)
		{
			StringBuilder sb=new StringBuilder(path.Length);
			if(!IsPathRooted(path))return path;
			String []frags=path.Split(new char[]{VolumeSeparatorChar,
									DirectorySeparatorChar});
			for(int i=0;i<frags.Length;i++)
			{
				if(frags[i]=="..")
				{
					if(i==1)
					{
						throw new ArgumentException("path");
					}
					sb.Length=sb.Length-frags[i-1].Length-1; 
					/* Hack : instead of 'removing' , I just trim it 
					   like this , faster this way */
				}
				else if(frags[i].Length>0)
				{
					sb.Append('/');
					sb.Append(frags[i]);
				}
			}
			return sb.ToString();
		}
		
		[TODO]
		public static String GetPathRoot(String path)
		{
			if (path == null) 
			{
				return null;
			}
			
			return path.Substring(0, 1);
		}
		
		/* generate /tmp/pnetXXXXXX */
		public static String GetTempFilename()
		{
			String dir = GetTempPath();
			String pre = "pnet";
			String file;
			while(true)
			{	
				file = dir +DirectorySeparatorChar+ pre + UniqName();
				if (!File.Exists(file))
				{
					return file;
				}				 
			}
			return null;
		}

		private static String UniqName()
		{
			String index="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz_";
			int num=r.Next(); // Take potluck
			String retval="";
			int radix=index.Length;
			for(int i=0;i<6;i++)
			{
				retval = retval + index[num%radix];
				num=num/radix;
			}
			return retval;
		}		
		
		public static String GetTempPath()
		{
			String tmp=Environment.GetEnvironmentVariable("TMPDIR"); // *nix
			if(tmp!=null)return tmp;
			tmp=Environment.GetEnvironmentVariable("TEMP_DIR"); // Win32 (?)
			if(tmp!=null)return tmp;
			return "/tmp"; 
			/* TODO : might just think about returning P_tmpdir from stdio.h */
		}
		
		public static bool HasExtension(String path)
		{
			if(path==null)return false;
			String [] elems=path.Split(DirectorySeparatorChar);
			String filename=elems[elems.Length-1];
			if(filename.IndexOf('.')!=-1)
			{
				return true;
			}
			return false;
		}
		
		
		public static bool IsPathRooted(String path)
		{
			if(path==null)return false;
			if(path.Length==0)return false;
			if(path.Length==1)return (path[0]==DirectorySeparatorChar);	
			return
				(
				(path[0]==DirectorySeparatorChar) || // Unix -> "/foo"
				(path[1]==VolumeSeparatorChar)    // Win32 -> "C:/foo"
				);
		}
	}
}










