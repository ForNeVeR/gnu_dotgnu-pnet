/*
 * FileSystem.cs - Implementation of the
 *			"Microsoft.VisualBasic.FileSystem" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace Microsoft.VisualBasic
{

using System;
using System.IO;
using System.ComponentModel;
using System.Runtime.InteropServices;
using Microsoft.VisualBasic.CompilerServices;

[StandardModule]
public sealed class FileSystem
{
	// This class cannot be instantiated.
	private FileSystem() {}

	// Change directories.
	[TODO]
	public static void ChDir(String Path)
			{
				// TODO
			}

	// Change drives.
	[TODO]
	public static void ChDrive(String Drive)
			{
				// TODO
			}
	[TODO]
	public static void ChDrive(char Drive)
			{
				// TODO
			}

	// Get the current directory.
	[TODO]
	public static String CurDir(char Drive)
			{
				// TODO
				return null;
			}
	public static String CurDir()
			{
				return Directory.GetCurrentDirectory();
			}

	// Scan through a directory.
	[TODO]
	public static String Dir
				(String Pathname,
				 [Optional] [DefaultValue(FileAttribute.Normal)]
				 		FileAttribute Attributes)
			{
				// TODO
				return null;
			}
	[TODO]
	public static String Dir()
			{
				// TODO
				return null;
			}

	// Determine if a particular file is at EOF.
	[TODO]
	public static bool EOF(int FileNumber)
			{
				// TODO
				return false;
			}

	// Get the attributes that were used to open a file.
	[TODO]
	public static OpenMode FileAttr(int FileNumber)
			{
				// TODO
				return OpenMode.Input;
			}

	// Close a group of files.
	[TODO]
	public static void FileClose(int[] FileNumbers)
			{
				// TODO
			}

	// Copy a file.
	[TODO]
	public static void FileCopy(String Source, String Destination)
			{
				// TODO
			}

	// Get the date and time for a specific file.
	[TODO]
	public static DateTime FileDateTime(String PathName)
			{
				// TODO
				return DateTime.MinValue;
			}

	// Get a value from a file.
	[TODO]
	public static void FileGet
				(int FileNumber, ref Decimal Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref double Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref float Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref DateTime Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref char Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref long Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref ValueType Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref Array Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber,
				 [Optional] [DefaultValue(false)] bool ArrayIsDynamic,
				 [Optional] [DefaultValue(false)] bool StringIsFixedLength)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref bool Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref byte Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref short Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref int Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FileGet
				(int FileNumber, ref String Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber,
				 [Optional] [DefaultValue(false)] bool StringIsFixedLength)
			{
				// TODO
			}
	[TODO]
	public static void FileGetObject
				(int FileNumber, ref Object Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}

	// Get the length of a file.
	[TODO]
	public static long FileLen(String PathName)
			{
				// TODO
				return 0;
			}

	// Open a file.
	public static void FileOpen
				(int FileNumber, String FileName, OpenMode Mode,
				 [Optional] [DefaultValue(OpenAccess.Default)]
				 	OpenAccess Access,
				 [Optional] [DefaultValue(OpenShare.Default)]
				 	OpenShare Share,
				 [Optional] [DefaultValue(-1)] int RecordLength)
			{
				// TODO
			}

	// Put a value to a file.
	[TODO]
	public static void FilePut
				(int FileNumber, Decimal Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, double Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, float Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, DateTime Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, char Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, long Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, ValueType Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, Array Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber,
				 [Optional] [DefaultValue(false)] bool ArrayIsDynamic,
				 [Optional] [DefaultValue(false)] bool StringIsFixedLength)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, bool Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, byte Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, short Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, int Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(int FileNumber, String Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber,
				 [Optional] [DefaultValue(false)] bool StringIsFixedLength)
			{
				// TODO
			}
	[TODO]
	public static void FilePut
				(Object FileNumber, Object Value,
			     [Optional] [DefaultValue(-1L)] long RecordNumber)
			{
				// TODO
			}
	[Obsolete("Use FilePutObject instead")]
	public static void FilePutObject
				(int FileNumber, Object Value,
			     [Optional] [DefaultValue(null)] Object RecordNumber)
			{
				throw new ArgumentException(S._("VB_Obsolete"));
			}

	// Set the record width for a file.
	[TODO]
	public static void FileWidth(int FileNumber, int RecordWidth)
			{
				// TODO
			}

	// Find a free file number.
	[TODO]
	public static int FreeFile()
			{
				// TODO
				return 0;
			}

	// Get the attributes on a specific file.
	[TODO]
	public static FileAttribute GetAttr(String PathName)
			{
				// TODO
				return FileAttribute.Normal;
			}

	// Input a file from a file.
	[TODO]
	public static void Input(int FileNumber, ref short Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref byte Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref bool Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref char Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref float Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref double Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref Decimal Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref String Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref DateTime Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref Object Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref long Value)
			{
				// TODO
			}
	[TODO]
	public static void Input(int FileNumber, ref int Value)
			{
				// TODO
			}

	// Input a string from a file.
	[TODO]
	public static String InputString(int FileNumber, int CharCount)
			{
				// TODO
				return null;
			}

	// Kill (delete) a file or directory tree.
	[TODO]
	public static void Kill(String PathName)
			{
				// TODO
			}

	// Get the length of an open file.
	[TODO]
	public static long LOF(int FileNumber)
			{
				// TODO
				return 0;
			}

	// Input a line from a file.
	[TODO]
	public static String LineInput(int FileNumber)
			{
				// TODO
				return null;
			}

	// Get the current location within an open file.
	[TODO]
	public static long Loc(int FileNumber)
			{
				// TODO
				return 0;
			}

	// Lock a record within a file.
	[TODO]
	public static void Lock(int FileNumber, long Record)
			{
				// TODO
			}

	// Lock the entire contents of a file.
	[TODO]
	public static void Lock(int FileNumber)
			{
				// TODO
			}

	// Lock a region within a file.
	[TODO]
	public static void Lock(int FileNumber, long FromRecord, long ToRecord)
			{
				// TODO
			}

	// Make a new directory.
	[TODO]
	public static void MkDir(String Path)
			{
				// TODO
			}

	// Print data to a file.
	[TODO]
	public static void Print(int FileNumber, params Object[] Output)
			{
				// TODO
			}
	[TODO]
	public static void PrintLine(int FileNumber, params Object[] Output)
			{
				// TODO
			}

	// Rename a file or directory.
	[TODO]
	public static void Rename(String OldPath, String NewPath)
			{
				// TODO
			}

	// Close all files and reset the filesystem.
	[TODO]
	public static void Reset()
			{
				// TODO
			}

	// Remove a directory.
	[TODO]
	public static void RmDir(String Path)
			{
				// TODO
			}

	// Convert a count value into a "SpcInfo" value.
	public static SpcInfo SPC(short count)
			{
				return new SpcInfo(count);
			}

	// Seek to a new file position.
	[TODO]
	public static void Seek(int FileNumber, long Position)
			{
				// TODO
			}

	// Get the current seek position within a file.
	[TODO]
	public static long Seek(int FileNumber)
			{
				// TODO
				return 0;
			}

	// Set the attributes on a file.
	[TODO]
	public static void SetAttr(String PathName, FileAttribute Attributes)
			{
				// TODO
			}

	// Create a "TabInfo" object.
	public static TabInfo TAB()
			{
				return new TabInfo(-1);
			}
	public static TabInfo TAB(short Column)
			{
				if(Column >= 1)
				{
					return new TabInfo(Column);
				}
				else
				{
					return new TabInfo(1);
				}
			}

	// Unlock a record within a file.
	[TODO]
	public static void Unlock(int FileNumber, long Record)
			{
				// TODO
			}

	// Unlock the entire contents of a file.
	[TODO]
	public static void Unlock(int FileNumber)
			{
				// TODO
			}

	// Unlock a region within a file.
	[TODO]
	public static void Unlock(int FileNumber, long FromRecord, long ToRecord)
			{
				// TODO
			}

	// Write data to a file.
	[TODO]
	public static void Write(int FileNumber, params Object[] Output)
			{
				// TODO
			}
	[TODO]
	public static void WriteLine(int FileNumber, params Object[] Output)
			{
				// TODO
			}

}; // class FileSystem

}; // namespace Microsoft.VisualBasic
