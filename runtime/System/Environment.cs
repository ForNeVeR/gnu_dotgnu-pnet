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
using System.Collections;
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
				try
				{
					newLine = SysCharInfo.GetNewLine();
				}
				catch(NotImplementedException)
				{
					// The runtime engine does not have "SysCharInfo".
					newLine = "\n";
				}

				// Get the path character information.
				try
				{
					PathInfo pathInfo = DirMethods.GetPathInfo();
					DirectorySeparatorChar = pathInfo.dirSeparator;
					AltDirectorySeparatorChar = pathInfo.altDirSeparator;
					VolumeSeparatorChar = pathInfo.volumeSeparator;
					PathSeparatorChar = pathInfo.pathSeparator;
					InvalidPathChars = pathInfo.invalidPathChars;
				}
				catch(NotImplementedException)
				{
					// The runtime engine does not have directory support.
					DirectorySeparatorChar = '/';
					AltDirectorySeparatorChar = '\0';
					VolumeSeparatorChar = '\0';
					PathSeparatorChar = ':';
					InvalidPathChars = null;
				}
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

	// Determine if application shutdown has started.
	[TODO]
	public static bool HasShutdownStarted
			{
				get
				{
					// TODO: set this to true during application finalization.
					return false;
				}
			}

	// Get the stack trace for the current context.
	public static String StackTrace
			{
				get
				{
					// We don't support stack traces.
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
							(_("Exception_NoSystemDir"));
					}
				}
			}

	// Get the version of the runtime engine.
	[TODO]
	public static Version Version
			{
				get
				{
					// TODO
					return new Version();
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
						(_("Exception_NoCmdLine"));
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

	// Get a particular environment variable.
	public static String GetEnvironmentVariable(String variable)
			{
				if(variable == null)
				{
					throw new ArgumentNullException("variable");
				}
				return TaskMethods.GetEnvironmentVariable(variable);
			}

	// Get a dictionary that allows access to the set of
	// environment variables for the current task.
	public static IDictionary GetEnvironmentVariables()
			{
				return new EnvironmentDictionary();
			}

	// Private class that implements a dictionary for environment variables.
	private sealed class EnvironmentDictionary : IDictionary
	{
		public EnvironmentDictionary()
				{
					// Nothing to do here.
				}

		// Add an object to this dictionary.
		public void Add(Object key, Object value)
				{
					throw new NotSupportedException(_("NotSupp_ReadOnly"));
				}

		// Clear this dictionary.
		public void Clear()
				{
					throw new NotSupportedException(_("NotSupp_ReadOnly"));
				}

		// Determine if this dictionary contains a specific key.
		public bool Contains(Object key)
				{
					String keyName = key.ToString();
					if(keyName != null)
					{
						return (TaskMethods.GetEnvironmentVariable(keyName)
									!= null);
					}
					else
					{
						throw new ArgumentNullException("key");
					}
				}

		// Copy the contents of this dictionary to an array.
		public void CopyTo(Array array, int index)
				{
					int count;
					if(array == null)
					{
						throw new ArgumentNullException("array");
					}
					if(index < 0)
					{
						throw new ArgumentOutOfRangeException
							("index", _("ArgRange_Array"));
					}
					if(array.Rank != 1)
					{
						throw new ArgumentException(_("Arg_RankMustBe1"));
					}
					count = TaskMethods.GetEnvironmentCount();
					if(index >= array.Length ||
					   count > (array.Length - index))
					{
						throw new ArgumentException
							(_("Arg_InvalidArrayIndex"));
					}
					int posn;
					for(posn = 0; posn < count; ++posn)
					{
						array.SetValue(new DictionaryEntry
								(TaskMethods.GetEnvironmentKey(posn),
								 TaskMethods.GetEnvironmentValue(posn)),
								index + posn);
					}
				}

		// Enumerate all values in this dictionary.
		IEnumerator IEnumerable.GetEnumerator()
				{
					return new EnvironmentEnumerator();
				}
		public IDictionaryEnumerator GetEnumerator()
				{
					return new EnvironmentEnumerator();
				}

		// Remove a value from this dictionary.
		public void Remove(Object key)
				{
					throw new NotSupportedException(_("NotSupp_ReadOnly"));
				}

		// Count the number of items in this dictionary.
		public int Count
				{
					get
					{
						return TaskMethods.GetEnvironmentCount();
					}
				}

		// Determine if this dictionary has a fixed size.
		public bool IsFixedSize
				{
					get
					{
						return true;
					}
				}

		// Determine if this dictionary is read-only.
		public bool IsReadOnly
				{
					get
					{
						return true;
					}
				}

		// Determine if this dictionary is synchronized.
		public bool IsSynchronized
				{
					get
					{
						return false;
					}
				}

		// Get the synchronization root for this dictionary.
		public Object SyncRoot
				{
					get
					{
						return this;
					}
				}

		// Get a particular object from this dictionary.
		public Object this[Object key]
				{
					get
					{
						String keyName = key.ToString();
						if(keyName != null)
						{
							return TaskMethods.GetEnvironmentVariable(keyName);
						}
						else
						{
							throw new ArgumentNullException("key");
						}
					}
					set
					{
						throw new NotSupportedException(_("NotSupp_ReadOnly"));
					}
				}

		// Get a list of all keys in this dictionary.
		public ICollection Keys
				{
					get
					{
						int count = TaskMethods.GetEnvironmentCount();
						String[] keys = new String [count];
						int posn;
						for(posn = 0; posn < count; ++posn)
						{
							keys[posn] = TaskMethods.GetEnvironmentKey(posn);
						}
						return keys;
					}
				}

		// Get a list of all values in this dictionary.
		public ICollection Values
				{
					get
					{
						int count = TaskMethods.GetEnvironmentCount();
						String[] values = new String [count];
						int posn;
						for(posn = 0; posn < count; ++posn)
						{
							values[posn] =
								TaskMethods.GetEnvironmentValue(posn);
						}
						return values;
					}
				}

	};

	// Private class for enumerating over the contents of the environment.
	private sealed class EnvironmentEnumerator : IDictionaryEnumerator
	{
		private int posn;
		private int count;

		// Constructor.
		public EnvironmentEnumerator()
				{
					posn = -1;
					count = TaskMethods.GetEnvironmentCount();
				}

		// Move to the next item in sequence.
		public bool MoveNext()
				{
					++posn;
					return (posn < count);
				}

		// Reset the enumerator.
		public void Reset()
				{
					posn = -1;
				}

		// Get the current enumerator value.
		public Object Current
				{
					get
					{
						return Entry;
					}
				}

		// Get the current dictionary entry value.
		public DictionaryEntry Entry
				{
					get
					{
						if(posn >= 0 && posn < count)
						{
							return new DictionaryEntry
								(TaskMethods.GetEnvironmentKey(posn),
								 TaskMethods.GetEnvironmentValue(posn));
						}
						else
						{
							throw new InvalidOperationException
								(_("Invalid_BadEnumeratorPosition"));
						}
					}
				}

		// Get the key associated with the current enumerator value.
		public Object Key
				{
					get
					{
						if(posn >= 0 && posn < count)
						{
							return TaskMethods.GetEnvironmentKey(posn);
						}
						else
						{
							throw new InvalidOperationException
								(_("Invalid_BadEnumeratorPosition"));
						}
					}
				}

		// Get the value associated with the current enumerator value.
		public Object Value
				{
					get
					{
						if(posn >= 0 && posn < count)
						{
							return TaskMethods.GetEnvironmentValue(posn);
						}
						else
						{
							throw new InvalidOperationException
								(_("Invalid_BadEnumeratorPosition"));
						}
					}
				}

	};

}; // class Environment

}; // namespace System
