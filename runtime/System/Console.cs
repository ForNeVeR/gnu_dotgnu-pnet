/*
 * Console.cs - Implementation of the "System.Console" class.
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

using System.IO;
using Platform;

public sealed class Console
{

	// Cached copies of the stdin, stdout, and stderr streams.
	private static TextReader stdin  = null;
	private static TextWriter stdout = null;
	private static TextWriter stderr = null;

	// Open the standard input stream.
	public static Stream OpenStandardInput()
			{
				return OpenStandardInput(256);
			}
	public static Stream OpenStandardInput(int bufferSize)
			{
				// TODO
				return null;
			}

	// Open the standard output stream.
	public static Stream OpenStandardOutput()
			{
				return OpenStandardOutput(256);
			}
	public static Stream OpenStandardOutput(int bufferSize)
			{
				// TODO
				return null;
			}

	// Open the standard error stream.
	public static Stream OpenStandardError()
			{
				return OpenStandardError(256);
			}
	public static Stream OpenStandardError(int bufferSize)
			{
				// TODO.
				return null;
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
							stdout = new StdWriter(1);
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
							stderr = new StdWriter(2);
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
				try
				{
					return In.Read();
				}
				catch(IOException io)
				{
					// TODO: recognise invalid streams, and
					// set "In" to the null stream.
					throw;
				}
			}

	// Read a line from the standard input stream.
	public static String ReadLine()
			{
				try
				{
					return In.ReadLine();
				}
				catch(IOException io)
				{
					// TODO: recognise invalid streams, and
					// set "In" to the null stream.
					throw;
				}
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
	public static void Write(int value)
			{
				Out.Write(value);
			}
	public static void Write(uint value)
			{
				Out.Write(value);
			}
	public static void Write(long value)
			{
				Out.Write(value);
			}
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
				//Out.Write(value);

				// For testing - fix later.
				if(value != null)
				{
					Stdio.StdWrite(1, value);
				}
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
	public static void WriteLine(int value)
			{
				Out.WriteLine(value);
			}
	public static void WriteLine(uint value)
			{
				Out.WriteLine(value);
			}
	public static void WriteLine(long value)
			{
				Out.WriteLine(value);
			}
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
				//Out.WriteLine(value);

				// For testing - fix later.
				if(value != null)
				{
					Stdio.StdWrite(1, value);
					Stdio.StdWrite(1, "\r\n");
				}
			}

}; // class Console

}; // namespace System
