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
		
		/* counter is for the MkTempFile function; its static
		 * so that the only time the MkTempFile function has 
                 * to go through the loop more than once is when
		 * its called for the first time
		 */
		private static int counter; 
		
				

		[TODO]
		public static String ChangeExtension(String path, String extension)
		{
			/* TODO: Figure out how to get the System-Dependant
                         * Extension Separator instead of just assuming it's
			 * a '.'; Mail to the DotGnu Developers mailing
			 * list or me (Charlie Carnow <carnow@gmx.net> 
			 * and the DotGnu Mailing list
                         * on this topic would be nice
                         */
			char[] separator = { '.' };	
			string[] patharray = path.Split(separator);
			string ext = extension;
			return String.Concat(patharray[0], '.', extension);
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
		
		[TODO]
		public static String GetDirectoryName(String path)
		{
			return null;
		}

		[TODO]
		public static String GetFileName(String path)
		{
			/* TODO: Allow use of AltDirectorySeparator, etc. */
			/* Seperate Directories */
		      string y;
		      char[] separator = {DirectorySeparatorChar};
		      string[] dirs = path.Split(separator);
		      y = path;
		      foreach (string x in dirs)
			{
				y = x;
			} /* The last string should be the filename */
			return y;
		}
		
		[TODO]
		public static String GetFileNameWithoutExtension(String path)
		{
			/* TODO: See TODO note at ChangeExtension */
			/* Split at the dot..The first element
                         * of filename should now have the filename without
		         * the extension */
			string[] filename;
			try 
			{
				char[] separator = {'.'};
				filename = GetFileName(path).Split(separator);
			}
			catch (ArgumentException e) 
			{	
				throw new ArgumentException(e.Message, e);
			}
			return filename[0];
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
		
		public static String GetTempFilename()
		{
			// Assume prefix tmp- I don't know a way to
			// get the OS were running on
			string dir = GetTempPath();
			string pre = "tmp";
			string file;
			while(true)
			{	
				/* Counter is a
				counter += 1;
				/* cat the strings together */
				file = dir + pre + NumToString(counter);

			/* TODO: "File" does not exist yet
				if (!File.exists(file))
				{
					return file;
				}				 
			*/
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
			/* TODO: See Note at ChangeExtension */
			char[] separator = {'.'}; 
			return (path.Split('.').Length > 1);		
		}
		
		
		public static bool IsPathRooted(String path)
		{
			return path.StartsWith(new String(VolumeSeparatorChar, 1));
		}

	}

}










