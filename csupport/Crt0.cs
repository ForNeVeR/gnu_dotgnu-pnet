/*
 * Crt0.cs - Program startup support definitions.
 *
 * This file is part of the Portable.NET "C language support" library.
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace OpenSystem.C
{

using System;
using System.IO;
using System.Collections;
using System.Runtime.InteropServices;
using System.Security;
using System.Reflection;

//
// The "Crt0" class contains methods that support the "crt0" code
// that the compiler outputs into modules with a "main" function.
// The "crt0" code looks something like this:
//
//		public static void .start(String[] args)
//		{
//			try
//			{
//				int argc;
//				IntPtr argv;
//				IntPtr envp;
//				argv = Crt0.GetArgV(args, sizeof(void *), out argc);
//				envp = Crt0.GetEnvironment();
//				Crt0.Startup("libcNN");
//				Crt0.Shutdown(main(argc, argv, envp));
//			}
//			catch(OutOfMemoryException)
//			{
//				throw;
//			}
//			catch(Object e)
//			{
//				throw Crt0.ShutdownWithException(e);
//			}
//		}
//
// If the "main" function does not have all three arguments, the
// compiler will only pass those arguments that "main" specifies.
//
// The "crt0" code is kept deliberately simple, with the bulk of the
// hard work done in this class.  This makes it easier to change the
// startup behaviour by altering "Crt0", without requiring every
// application to be recompiled.
//
// Note: the "crt0" code catches all exceptions, including those
// that don't inherit from "System.Exception".  Out of memory
// exceptions are explicitly excluded because there's nothing that
// can be done in that situation anyway.
//
public unsafe sealed class Crt0
{
	// Internal state.
	private static int pointerSize;
	private static IntPtr argV;
	private static int argC;
	private static IntPtr environ;
	private static Module libcModule;
	private static bool startupDone = false;

	// Get the "argv" and "argc" values for the running task.
	// "mainArgs" is the value passed to the program's entry point.
	// "ptrSize" is the size of pointers in the app's memory model.
	public static IntPtr GetArgV(String[] mainArgs, int ptrSize,
								 out int argc)
			{
				lock(typeof(Crt0))
				{
					// Bail out if we were already called previously.
					if(argV != IntPtr.Zero)
					{
						argc = argC;
						return argV;
					}

					// Record the pointer size for later.
					pointerSize = ptrSize;

					// Get the actual command-line arguments, including
					// the application name.  The application name isn't
					// normally included in "mainArgs", so we have to
					// do the following instead.
					String[] args;
					try
					{
						args = Environment.GetCommandLineArgs();
					}
					catch(NotSupportedException)
					{
						args = null;
					}
					if(args == null)
					{
						// We couldn't get the arguments from the runtime
						// engine directly, so use "mainArgs" and simulate
						// the application name.  This may happen in embedded
						// environments that don't have a real command-line.
						if(mainArgs != null)
						{
							args = new String [mainArgs.Length + 1];
							Array.Copy(mainArgs, 0, args, 1,
									   mainArgs.Length);
						}
						else
						{
							args = new String [1];
						}
						args[0] = "cliapp.exe";
					}

					// Check that the pointer size is in range for the
					// actual platform that we are running on.  If not,
					// then it is unlikely that the application will work.
					if(ptrSize < sizeof(IntPtr))
					{
					#if CONFIG_SMALL_CONSOLE
						Console.Write(args[0]);
						Console.WriteLine
						  (": application is {0}-bit, but platform is {1}-bit",
						   (Object)(ptrSize * 8), (Object)(sizeof(IntPtr) * 8));
					#else
						Console.Error.Write(args[0]);
						Console.Error.WriteLine
						  (": application is {0}-bit, but platform is {1}-bit",
						   (Object)(ptrSize * 8), (Object)(sizeof(IntPtr) * 8));
					#endif
						Environment.Exit(1);
					}

					// Convert "args" into an array of native strings,
					// terminated by a NULL pointer.
					argV = Marshal.AllocHGlobal
						(new IntPtr(ptrSize * (args.Length + 1)));
					if(argV == IntPtr.Zero)
					{
						// We probably don't have permission to allocate
						// memory using "AllocHGlobal".  If that is the
						// case, then bail out.  C is useless without it.
						throw new NotSupportedException
							("Fatal error: cannot allocate unmanaged memory");
					}
					argC = args.Length;
					for(int index = 0; index < argC; ++index)
					{
						Marshal.WriteIntPtr(argV, ptrSize * index,
							Marshal.StringToHGlobalAnsi(args[index]));
					}
					Marshal.WriteIntPtr(argV, ptrSize * argC, IntPtr.Zero);

					// Return the final values to the caller.
					argc = argC;
					return argV;
				}
			}

	// Get the "environ" value for the running task.
	public static IntPtr GetEnvironment()
			{
				lock(typeof(Crt0))
				{
					// Bail out if we were already called previously.
					if(environ != IntPtr.Zero)
					{
						return environ;
					}

					// Get the environment variables for the running task.
					IDictionary env;
					int count;
					IDictionaryEnumerator e;
					try
					{
						env = Environment.GetEnvironmentVariables();
					}
					catch(SecurityException)
					{
						// The runtime engine has decided that we don't have
						// sufficient permissions to get the environment.
						// We continue with an empty environment.
						env = null;
					}
					if(env != null)
					{
						count = env.Count;
						e = env.GetEnumerator();
					}
					else
					{
						count = 0;
						e = null;
					}

					// Allocate an array to hold the converted values.
					environ = Marshal.AllocHGlobal
						(new IntPtr(pointerSize * (count + 1)));

					// Convert the environment variables into native strings.
					int index = 0;
					String value;
					while(index < count && e != null && e.MoveNext())
					{
						value = String.Concat((String)(e.Key), "=",
											  (String)(e.Value));
						Marshal.WriteIntPtr(environ, pointerSize * index,
							Marshal.StringToHGlobalAnsi(value));
						++index;
					}
					Marshal.WriteIntPtr
						(environ, pointerSize * count, IntPtr.Zero);

					// The environment is ready to go.
					return environ;
				}
			}

	// Perform system library startup tasks.  This is normally
	// called just before invoking the program's "main" function.
	// "libcName" is the name of the C system library assembly,
	// which is normally "libc32" or "libc64", depending upon
	// the application's memory model.
	public static void Startup(String libcName)
			{
				Module mainModule;
				Assembly assembly;
				Type type;
				MethodInfo method;
				FieldInfo field;

				// Bail out if we've already done the startup code.
				lock(typeof(Crt0))
				{
					if(startupDone)
					{
						return;
					}
					startupDone = true;
				}

				// Find the module that contains the "main" function.
				mainModule = null;
				try
				{
					assembly = Assembly.GetCallingAssembly();
					type = assembly.GetType("<Module>");
					if(type != null)
					{
						mainModule = type.Module;
					}
				}
				catch(NotImplementedException)
				{
					// The runtime engine probably does not have support
					// for reflection, so there's nothing we can do.
				}

				// Find standard C library's global module.
				libcModule = null;
				try
				{
					assembly = Assembly.Load(libcName);
					type = assembly.GetType("$Module$");
					if(type != null)
					{
						libcModule = type.Module;
					}
				}
				catch(OutOfMemoryException)
				{
					// Send out of memory conditions back up the stack.
					throw;
				}
				catch(Exception)
				{
					// We weren't able to load "libc" for some reason.
				}

				// Set the global "__environ" variable within "libc".
				if(libcModule != null)
				{
					field = libcModule.GetField("__environ");
					if(field != null)
					{
						field.SetValue(null, (Object)environ);
					}
				}

				// Initialize the stdin, stdout, and stderr file descriptors.
			#if CONFIG_SMALL_CONSOLE
				FileTable.SetFileDescriptor(0, Stream.Null);
				FileTable.SetFileDescriptor(1, new ConsoleStream());
				FileTable.SetFileDescriptor(2, new ConsoleStream());
			#else
				FileTable.SetFileDescriptor(0, Console.OpenStandardInput());
				FileTable.SetFileDescriptor(1, Console.OpenStandardOutput());
				FileTable.SetFileDescriptor(2, Console.OpenStandardError());
			#endif

				// Invoke the application's ".init" function, if present.
				if(mainModule != null)
				{
					MethodInfo initMethod = mainModule.GetMethod(".init");
					if(initMethod != null)
					{
						initMethod.Invoke(null, null);
					}
				}

				// Register the app's ".fini" function with "atexit".
				if(mainModule != null)
				{
					MethodInfo finiMethod = mainModule.GetMethod(".fini");
					if(finiMethod != null)
					{
						// TODO
					}
				}
			}

	// Perform system library shutdown tasks and exit the application.
	// This is normally called after invoking the program's "main" function.
	public static void Shutdown(int status)
			{
				// Find the global "exit" function and call it.
				if(libcModule != null)
				{
					MethodInfo method = libcModule.GetMethod("exit");
					if(method != null)
					{
						Object[] args = new Object [1];
						args[0] = (Object)(status);
						method.Invoke(null, args);
					}
				}

				// If we get here, then we weren't able to find "exit",
				// or it returned to us by mistake.  Bail out through
				// "System.Environment".
				Environment.Exit(status);
			}

	// Perform system library shutdown tasks when an exception occurs.
	public static Object ShutdownWithException(Object e)
			{
				// Nothing to do here yet, so return the exception object
				// to the caller to be rethrown to the runtime engine.
				return e;
			}

	// Get the pointer size in the application's memory model.
	// Will be 4 for 32-bit memory models, and 8 for 64-bit models.
	public static int PointerSize
			{
				get
				{
					return pointerSize;
				}
			}

	// Get the module that contains "libc".  Returns "null" if unknown.
	public static Module LibC
			{
				get
				{
					return libcModule;
				}
			}

#if CONFIG_SMALL_CONSOLE

	// Helper class for writing to stdout when the System.Console
	// class does not have "OpenStandardOutput".
	private sealed class ConsoleStream : Stream
	{
		// Constructor.
		public ConsoleStream() {}

		// Stub out all stream functionality.
		public override void Flush() {}
		public override int Read(byte[] buffer, int offset, int count)
				{
					throw new NotSupportedException();
				}
		public override int ReadByte()
				{
					throw new NotSupportedException();
				}
		public override long Seek(long offset, SeekOrigin origin)
				{
					throw new NotSupportedException();
				}
		public override void SetLength(long value)
				{
					throw new NotSupportedException();
				}
		public override void Write(byte[] buffer, int offset, int count)
				{
					// Validate the buffer argument.
					if(buffer == null)
					{
						throw new ArgumentNullException("buffer");
					}
					else if(offset < 0 || offset > buffer.Length)
					{
						throw new ArgumentOutOfRangeException();
					}
					else if(count < 0)
					{
						throw new ArgumentOutOfRangeException();
					}
					else if((buffer.Length - offset) < count)
					{
						throw new ArgumentException();
					}

					// Write the contents of the buffer.
					while(count > 0)
					{
						Console.Write((char)(buffer[offset]));
						++offset;
						--count;
					}
				}
		public override void WriteByte(byte value)
				{
					Console.Write((char)value);
				}
		public override bool CanRead { get { return false; } }
		public override bool CanSeek { get { return false; } }
		public override bool CanWrite { get { return true; } }
		public override long Length
				{
					get
					{
						throw new NotSupportedException();
					}
				}
		public override long Position
				{
					get
					{
						throw new NotSupportedException();
					}
					set
					{
						throw new NotSupportedException();
					}
				}

	}; // class ConsoleStream

#endif // !CONFIG_SMALL_CONSOLE

} // class Crt0

} // namespace OpenSystem.C
