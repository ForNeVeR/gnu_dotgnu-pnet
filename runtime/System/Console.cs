/*
 * Console.cs - Implementation of the "System.Console" class.
 *
 * Copyright (C) 2001, 2003  Southern Storm Software, Pty Ltd.
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

using System.IO;
using System.Text;
using System.Private;
using Platform;

public sealed class Console
{

#if !CONFIG_SMALL_CONSOLE

	// Cached copies of the stdin, stdout, and stderr streams.
	private static TextReader stdin  = null;
	private static TextWriter stdout = null;
	private static TextWriter stderr = null;

	// This class cannot be instantiated.
	private Console() {}

	// Open the standard input stream.
	public static Stream OpenStandardInput()
			{
				return new StdStream(0);
			}
	public static Stream OpenStandardInput(int bufferSize)
			{
				return new StdStream(0);
			}

	// Open the standard output stream.
	public static Stream OpenStandardOutput()
			{
				return new StdStream(1);
			}
	public static Stream OpenStandardOutput(int bufferSize)
			{
				return new StdStream(1);
			}

	// Open the standard error stream.
	public static Stream OpenStandardError()
			{
				return new StdStream(2);
			}
	public static Stream OpenStandardError(int bufferSize)
			{
				return new StdStream(2);
			}

	// Get the standard input stream.
	public static TextReader In
			{
				get
				{
					lock(typeof(Console))
					{
						if(stdin != null)
						{
							return stdin;
						}
						else
						{
							stdin = new StdReader(0);
							return stdin;
						}
					}
				}
			}

	// Get the standard output stream.
	public static TextWriter Out
			{
				get
				{
					lock(typeof(Console))
					{
						if(stdout != null)
						{
							return stdout;
						}
						else
						{
							Encoding encoding = Encoding.Default;
							if(encoding is UTF8Encoding)
							{
								// Disable the preamble if UTF-8.
								encoding = new UTF8Encoding();
							}
							StreamWriter writer = new StreamWriter
								(new StdStream(1), encoding);
							writer.AutoFlush = true;
							SetOut(writer);
							return stdout;
						}
					}
				}
			}

	// Get the standard error stream.
	public static TextWriter Error
			{
				get
				{
					lock(typeof(Console))
					{
						if(stderr != null)
						{
							return stderr;
						}
						else
						{
							Encoding encoding = Encoding.Default;
							if(encoding is UTF8Encoding)
							{
								// Disable the preamble if UTF-8.
								encoding = new UTF8Encoding();
							}
							StreamWriter writer = new StreamWriter
								(new StdStream(2), encoding);
							writer.AutoFlush = true;
							SetError(writer);
							return stderr;
						}
					}
				}
			}

	// Set the standard input stream.
	public static void SetIn(TextReader newIn)
			{
				if(newIn == null)
				{
					throw new ArgumentNullException("newIn");
				}
				stdin = TextReader.Synchronized(newIn);
			}

	// Set the standard output stream.
	public static void SetOut(TextWriter newOut)
			{
				if(newOut == null)
				{
					throw new ArgumentNullException("newOut");
				}
				stdout = TextWriter.Synchronized(newOut);
			}

	// Set the standard error stream.
	public static void SetError(TextWriter newError)
			{
				if(newError == null)
				{
					throw new ArgumentNullException("newError");
				}
				stderr = TextWriter.Synchronized(newError);
			}

	// Read a character from the standard input stream.
	public static int Read()
			{
				NormalMode();
				return In.Read();
			}

	// Read a line from the standard input stream.
	public static String ReadLine()
			{
				NormalMode();
				return In.ReadLine();
			}

	// Write a formatted string to standard output.
	public static void Write(String format, Object arg0)
			{
				Out.Write(format, arg0);
			}
	public static void Write(String format, Object arg0, Object arg1)
			{
				Out.Write(format, arg0, arg1);
			}
	public static void Write(String format, Object arg0, Object arg1,
							 Object arg2)
			{
				Out.Write(format, arg0, arg1, arg2);
			}
	public static void Write(String format, params Object[] args)
			{
				Out.Write(format, args);
			}
#if !ECMA_COMPAT
	[CLSCompliant(false)]
	public static void Write(String format, Object arg0, Object arg1,
							 Object arg2, Object arg3, __arglist)
			{
				ArgIterator iter = new ArgIterator(__arglist);
				Object[] list = new Object [4 + iter.GetRemainingCount()];
				list[0] = arg0;
				list[1] = arg1;
				list[2] = arg2;
				list[3] = arg3;
				int posn = 4;
				while(posn < list.Length)
				{
					list[posn] = TypedReference.ToObject(iter.GetNextArg());
					++posn;
				}
				Out.Write(format, list);
			}
#endif // !ECMA_COMPAT

	// Write primitive values to standard output.
	public static void Write(bool value)
			{
				Out.Write(value);
			}
	public static void Write(char value)
			{
				Out.Write(value);
			}
	public static void Write(char[] value)
			{
				Out.Write(value);
			}
	public static void Write(char[] value, int index, int count)
			{
				Out.Write(value, index, count);
			}
#if CONFIG_EXTENDED_NUMERICS
	public static void Write(double value)
			{
				Out.Write(value);
			}
	public static void Write(Decimal value)
			{
				Out.Write(value);
			}
	public static void Write(float value)
			{
				Out.Write(value);
			}
#endif
	public static void Write(int value)
			{
				Out.Write(value);
			}
	[CLSCompliant(false)]
	public static void Write(uint value)
			{
				Out.Write(value);
			}
	public static void Write(long value)
			{
				Out.Write(value);
			}
	[CLSCompliant(false)]
	public static void Write(ulong value)
			{
				Out.Write(value);
			}
	public static void Write(Object value)
			{
				Out.Write(value);
			}
	public static void Write(String value)
			{
				Out.Write(value);
			}

	// Write a newline to standard output.
	public static void WriteLine()
			{
				Out.WriteLine();
			}

	// Write a formatted string to standard output followed by a newline.
	public static void WriteLine(String format, Object arg0)
			{
				Out.WriteLine(format, arg0);
			}
	public static void WriteLine(String format, Object arg0, Object arg1)
			{
				Out.WriteLine(format, arg0, arg1);
			}
	public static void WriteLine(String format, Object arg0, Object arg1,
							     Object arg2)
			{
				Out.WriteLine(format, arg0, arg1, arg2);
			}
	public static void WriteLine(String format, params Object[] args)
			{
				Out.WriteLine(format, args);
			}
#if !ECMA_COMPAT
	[CLSCompliant(false)]
	public static void WriteLine(String format, Object arg0, Object arg1,
							     Object arg2, Object arg3, __arglist)
			{
				ArgIterator iter = new ArgIterator(__arglist);
				Object[] list = new Object [4 + iter.GetRemainingCount()];
				list[0] = arg0;
				list[1] = arg1;
				list[2] = arg2;
				list[3] = arg3;
				int posn = 4;
				while(posn < list.Length)
				{
					list[posn] = TypedReference.ToObject(iter.GetNextArg());
					++posn;
				}
				Out.WriteLine(format, list);
			}
#endif // !ECMA_COMPAT

	// Write primitive values to standard output followed by a newline.
	public static void WriteLine(bool value)
			{
				Out.WriteLine(value);
			}
	public static void WriteLine(char value)
			{
				Out.WriteLine(value);
			}
	public static void WriteLine(char[] value)
			{
				Out.WriteLine(value);
			}
	public static void WriteLine(char[] value, int index, int count)
			{
				Out.WriteLine(value, index, count);
			}
#if CONFIG_EXTENDED_NUMERICS
	public static void WriteLine(double value)
			{
				Out.WriteLine(value);
			}
	public static void WriteLine(Decimal value)
			{
				Out.WriteLine(value);
			}
	public static void WriteLine(float value)
			{
				Out.WriteLine(value);
			}
#endif
	public static void WriteLine(int value)
			{
				Out.WriteLine(value);
			}
	[CLSCompliant(false)]
	public static void WriteLine(uint value)
			{
				Out.WriteLine(value);
			}
	public static void WriteLine(long value)
			{
				Out.WriteLine(value);
			}
	[CLSCompliant(false)]
	public static void WriteLine(ulong value)
			{
				Out.WriteLine(value);
			}
	public static void WriteLine(Object value)
			{
				Out.WriteLine(value);
			}
	public static void WriteLine(String value)
			{
				Out.WriteLine(value);
			}

#else // CONFIG_SMALL_CONSOLE

	// This class cannot be instantiated.
	private Console() {}

	// Read a line from the standard input stream.
	public static String ReadLine()
			{
				StringBuilder builder = new StringBuilder();
				int ch;
				NormalMode();
				while((ch = Stdio.StdRead(0)) != -1 && ch != '\n')
				{
					if(ch != '\r')
					{
						builder.Append((char)ch);
					}
				}
				if(ch == -1 && builder.Length == 0)
				{
					return null;
				}
				else
				{
					return builder.ToString();
				}
			}

	// Write a formatted string to standard output.
	public static void Write(String format, Object arg0)
			{
				Stdio.StdWrite(1, String.Format(format, arg0));
			}
	public static void Write(String format, Object arg0, Object arg1)
			{
				Stdio.StdWrite(1, String.Format(format, arg0, arg1));
			}
	public static void Write(String format, Object arg0, Object arg1,
							 Object arg2)
			{
				Stdio.StdWrite(1, String.Format(format, arg0, arg1, arg2));
			}
	public static void Write(String format, params Object[] args)
			{
				Stdio.StdWrite(1, String.Format(format, args));
			}

	// Write primitive values to standard output.
	public static void Write(char value)
			{
				Stdio.StdWrite(1, value);
			}
	public static void Write(char[] value)
			{
				if(value == null)
				{
					throw new ArgumentNullException("null");
				}
				foreach(char ch in value)
				{
					Stdio.StdWrite(1, ch);
				}
			}
	public static void Write(char[] value, int index, int count)
			{
				if(value == null)
				{
					throw new ArgumentNullException("null");
				}
				if(index < 0 || index > value.Length)
				{
					throw new ArgumentOutOfRangeException
						("index", _("ArgRange_StringIndex"));
				}
				if(count < 0 || count > (value.Length - index))
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_StringRange"));
				}
				while(count > 0)
				{
					Stdio.StdWrite(1, value[index]);
					++index;
					--count;
				}
			}
	public static void Write(int value)
			{
				Stdio.StdWrite(1, value.ToString());
			}
	public static void Write(Object value)
			{
				if(value != null)
				{
					Stdio.StdWrite(1, value.ToString());
				}
			}
	public static void Write(String value)
			{
				Stdio.StdWrite(1, value);
			}

	// Write a newline to standard output.
	public static void WriteLine()
			{
				Stdio.StdWrite(1, Environment.NewLine);
			}

	// Write a formatted string to standard output followed by a newline.
	public static void WriteLine(String format, Object arg0)
			{
				Write(format, arg0);
				WriteLine();
			}
	public static void WriteLine(String format, Object arg0, Object arg1)
			{
				Write(format, arg0, arg1);
				WriteLine();
			}
	public static void WriteLine(String format, Object arg0, Object arg1,
							     Object arg2)
			{
				Write(format, arg0, arg1, arg2);
				WriteLine();
			}
	public static void WriteLine(String format, params Object[] args)
			{
				Write(format, args);
				WriteLine();
			}

	// Write primitive values to standard output followed by a newline.
	public static void WriteLine(char value)
			{
				Stdio.StdWrite(1, value);
				WriteLine();
			}
	public static void WriteLine(char[] value)
			{
				Write(value);
				WriteLine();
			}
	public static void WriteLine(int value)
			{
				Write(value);
				WriteLine();
			}
	public static void WriteLine(Object value)
			{
				Write(value);
				WriteLine();
			}
	public static void WriteLine(String value)
			{
				Stdio.StdWrite(1, value);
				WriteLine();
			}

#endif // CONFIG_SMALL_CONSOLE

#if CONFIG_EXTENDED_CONSOLE

	// Global state for the extended console.
	private static String title = String.Empty;
	private static bool specialMode = false;
	private static bool treatControlCAsInput = false;

	// Enable the "normal" input mode on the console.
	private static void NormalMode()
			{
				lock(typeof(Console))
				{
					if(specialMode)
					{
						specialMode = false;
						Stdio.SetConsoleMode(Stdio.MODE_NORMAL);
					}
				}
			}

	// Enable the "special" character-at-a-time input mode on the console.
	private static void SpecialMode()
			{
				lock(typeof(Console))
				{
					if(!specialMode)
					{
						specialMode = true;
						if(treatControlCAsInput)
						{
							Stdio.SetConsoleMode(Stdio.MODE_RAW);
						}
						else
						{
							Stdio.SetConsoleMode(Stdio.MODE_CBREAK);
						}
					}
				}
			}

	// Output a beep on the console.
	public static void Beep()
			{
				lock(typeof(Console))
				{
					SpecialMode();
					Stdio.Beep();
				}
			}

	// Clear the display to the current foreground and background color.
	public static void Clear()
			{
				lock(typeof(Console))
				{
					SpecialMode();
					Stdio.Clear();
				}
			}

	// Read a key from the console.  If "intercept" is "false",
	// then the key is echoed to the console.
	public static ConsoleKeyInfo ReadKey()
			{
				return ReadKey(false);
			}
	public static ConsoleKeyInfo ReadKey(bool intercept)
			{
				lock(typeof(Console))
				{
					SpecialMode();
					char ch;
					int key, modifiers;
					Stdio.ReadKey(out ch, out key, out modifiers);
					return new ConsoleKeyInfo
						(ch, (ConsoleKey)key, (ConsoleModifiers)modifiers);
				}
			}

	// Set the cursor position.
	// This is a guess - fix later.
	public static void SetCursorPosition(int x, int y)
			{
				lock(typeof(Console))
				{
					SpecialMode();
					Stdio.SetCursorPosition(x, y);
				}
			}

	// Set the current text foreground and background attributes.
	// This is a guess - fix later.
	public static void SetTextAttribute(ConsoleColor foreground,
										ConsoleColor background)
			{
				lock(typeof(Console))
				{
					SpecialMode();
					Stdio.SetTextAttributes
						(((int)foreground) | (((int)background) << 4));
				}
			}

	// Console properties.
	public static int BufferHeight
			{
				get
				{
					lock(typeof(Console))
					{
						SpecialMode();
						int width, height;
						Stdio.GetBufferSize(out width, out height);
						return height;
					}
				}
			}
	public static int BufferWidth
			{
				get
				{
					lock(typeof(Console))
					{
						SpecialMode();
						int width, height;
						Stdio.GetBufferSize(out width, out height);
						return width;
					}
				}
			}
	public static int CursorLeft
			{
				get
				{
					lock(typeof(Console))
					{
						SpecialMode();
						int x, y;
						Stdio.GetCursorPosition(out x, out y);
						return x;
					}
				}
			}
	public static int CursorTop
			{
				get
				{
					lock(typeof(Console))
					{
						SpecialMode();
						int x, y;
						Stdio.GetCursorPosition(out x, out y);
						return y;
					}
				}
			}
	public static bool KeyAvailable
			{
				get
				{
					lock(typeof(Console))
					{
						SpecialMode();
						return Stdio.KeyAvailable();
					}
				}
			}
	public static String Title
			{
				get
				{
					// Note: we never query the initial console title
					// from the system because it may contain sensitive
					// data that we don't want the program to have access to.
					lock(typeof(Console))
					{
						return title;
					}
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					lock(typeof(Console))
					{
						SpecialMode();
						title = value;
						Stdio.SetConsoleTitle(title);
					}
				}
			}
	public static bool TreatControlCAsInput
			{
				get
				{
					lock(typeof(Console))
					{
						return treatControlCAsInput;
					}
				}
				set
				{
					lock(typeof(Console))
					{
						if(treatControlCAsInput != value)
						{
							specialMode = false;
							treatControlCAsInput = value;
							SpecialMode();
						}
					}
				}
			}
	public static int WindowHeight
			{
				get
				{
					lock(typeof(Console))
					{
						SpecialMode();
						int left, top, width, height;
						Stdio.GetWindowSize
							(out left, out top, out width, out height);
						return height;
					}
				}
			}
	public static int WindowLeft
			{
				get
				{
					lock(typeof(Console))
					{
						SpecialMode();
						int left, top, width, height;
						Stdio.GetWindowSize
							(out left, out top, out width, out height);
						return left;
					}
				}
			}
	public static int WindowTop
			{
				get
				{
					lock(typeof(Console))
					{
						SpecialMode();
						int left, top, width, height;
						Stdio.GetWindowSize
							(out left, out top, out width, out height);
						return top;
					}
				}
			}
	public static int WindowWidth
			{
				get
				{
					lock(typeof(Console))
					{
						SpecialMode();
						int left, top, width, height;
						Stdio.GetWindowSize
							(out left, out top, out width, out height);
						return width;
					}
				}
			}

	// Event that is emitted for cancel keycodes like CTRL+C.
	public static event ConsoleCancelEventHandler CancelKeyPress;

	// Method that is called from the runtime engine to handle "cancel" events.
	// Returns "false" to quit the application, or "true" to continue.
	private static bool HandleCancel(int specialKeys)
			{
				ConsoleCancelEventArgs args;
				args = new ConsoleCancelEventArgs
					((ConsoleSpecialKeys)specialKeys);
				if(CancelKeyPress != null)
				{
					CancelKeyPress(null, args);
				}
				return args.Cancel;
			}

#else // !CONFIG_EXTENDED_CONSOLE

	// Enable the "normal" input mode on the console.
	private static void NormalMode()
			{
				// Nothing to do if we don't have an extended console.
			}

#endif // !CONFIG_EXTENDED_CONSOLE

}; // class Console

}; // namespace System
