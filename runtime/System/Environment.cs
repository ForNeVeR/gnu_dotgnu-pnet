/*
 * Environment.cs - Implementation of the "System.Environment" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System
{

using System.Security;
using Platform;

public sealed class Environment
{
	// Internal state.
	private static String newLine;
	private static int exitCode = 0;

	// Separators for directories, paths, etc.
	public static readonly char   DirectorySeparatorChar;
	public static readonly char   AltDirectorySeparatorChar;
	public static readonly char   VolumeSeparatorChar;
	public static readonly char   PathSeparatorChar;
	public static readonly char[] InvalidPathChars;

	// Initialize the environment state.
	static Environment()
			{
				// Get the newline string.
				newLine = SysCharInfo.GetNewLine();

				// Get the path character information.
				PathInfo pathInfo = DirMethods.GetPathInfo();
				DirectorySeparatorChar = pathInfo.dirSeparator;
				AltDirectorySeparatorChar = pathInfo.altDirSeparator;
				VolumeSeparatorChar = pathInfo.volumeSeparator;
				PathSeparatorChar = pathInfo.pathSeparator;
				InvalidPathChars = pathInfo.invalidPathChars;
			}

	// Get resources from "mscorlib.dll".
	internal static String GetResourceString(String tag)
			{
				return tag;
			}

	// Get the platform-specific newline string.
	public static String NewLine
			{
				get
				{
					return newLine;
				}
			}

	// Exit from the current process.
	public static void Exit(int exitCode)
			{
				TaskMethods.Exit(exitCode);
			}

	// Get or set the process exit code.
	public static int ExitCode
			{
				get
				{
					return exitCode;
				}
				set
				{
					if(exitCode != value)
					{
						exitCode = value;
						TaskMethods.SetExitCode(value);
					}
				}
			}

	// Determine if we are running on Windows 95.
	public static bool IsWin95
			{
				get
				{
					// As far as the caller is concerned, Win95 doesn't exist.
					return false;
				}
			}

	// Get the stack trace for the current context.
	public static String StackTrace
			{
				get
				{
					// We don't support trace traces.
					return String.Empty;
				}
			}

	// Get the "System" directory.
	public static String SystemDirectory
			{
				get
				{
					String dir = DirMethods.GetSystemDirectory();
					if(dir != null)
					{
						return dir;
					}
					else
					{
						throw new NotSupportedException
							(Environment.GetResourceString
								("Exception_NoSystemDir"));
					}
				}
			}

	// Get the command line arguments.
	public static String[] GetCommandLineArgs()
			{
				String[] args = TaskMethods.GetCommandLineArgs();
				if(args != null)
				{
					return args;
				}
				else
				{
					throw new NotSupportedException
						(Environment.GetResourceString
							("Exception_NoCmdLine"));
				}
			}

	// Get the command line as a single string.
	public static String CommandLine
			{
				get
				{
					String[] args = GetCommandLineArgs();
					return String.Join(" ", args);
				}
			}

	// Get the number of milliseconds since the last reboot.
	public static int TickCount
			{
				get
				{
					return TimeMethods.GetUpTime();
				}
			}

}; // class Environment

}; // namespace System
