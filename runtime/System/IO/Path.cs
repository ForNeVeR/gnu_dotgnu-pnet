/*
 * Path.cs - Implementation of the "System.IO.Path" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
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
	using System;
	using Platform;
	public class System.Path 
	{
		// I know of the existence of System.Enviornment
		// But I'm opting to just get the seperators
		// from DirMethods.getPathInfo()
		private static PathInfo p = DirMethods.GetPathInfo();
		public static readonly char DirectorySeperatorChar = p.dirSeperator;
		public static readonly char AltDirectorySeperatorChar = p.AltDirSeparator;
		public static readonly char VolumeSeparatorChar = p.volumeSeperator;
		public static readonly char PathSeperatorChar = p.pathSeperator;
		public static readonly char[] InvalidPathChars = p.invalidPathChars;
		private static readonly char[] PathSeperatorChars = { DirectorySeperatorChar, AltDirectorySeperatorChar, VolumeSeperatorChar};
		
		private int counter; 
		
				

		[TODO]
		public static String ChangeExtension(String path, String extension)
		{	
			return null;
		}
		
		[TODO]
		public static String Combine(String path1, String path2)
		{
			return null;
		}
		
		[TODO]
		public static String GetDirectoryName(String path)
		{
			return null;
		}

		[TODO]
		public static String GetFileName(String path)
		{
			/* TODO: Allow use of AltDirectorySeperator, etc. */
			/* Seperate Directories */
		      string[] dirs = path.Split(DirectorySeperatorChar);
		      foreach (string x in dirs)
			{
			} /* The last string should be the filename */
			return x;
		}
		
		[TODO]
		public static String GetFileNameWithoutExtension(String path)
		{
			return null;
		}
		
		[TODO]
		public static String GetFullPath(String path)
		{
			return null;
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
		
		/* Based on tempfile.py function mktemp */
		[TODO]
		public static String GetTempFilename()
		{
			// Assume prefix tmp- I don't know a way to
			// get the OS were running on
			string dir = GetTempPath();
			string pre = "tmp";
			string file;
			while(true)
			{	
				counter += 1;
				/* cat the strings together */
				file = dir + pre + NumToString(counter);
				if (!File.exists(file))
				{
					return file;
				}				 
			}
				
		}
		
		[TODO]
		private static String NumToString(int num)
		{
			// TODO: Implement
			return null;
		}
		
		[TODO]
		public static String GetTempPath()
		{
			/* TODO: Implement */
			return "/tmp";
		}
		
		[TODO]
		public static bool HasExtension(String path)
		{
			return false;
		}
		
		
		public static bool IsPathRooted(String path)
		{
			return path.StartsWith(new String(VolumeSeperatorChar, 1));
		}

	}

}
