/*
 * TestMain.cs - Implementation of the "CSUnit.TestMain" class.
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

namespace CSUnit
{

using System;
using System.Reflection;

public sealed class TestMain
{
	// Main entry point for the program.
	public static int Main(String[] args)
			{
				int numArgs = args.Length;
				int argNum = 0;
				String arg;
				bool stopAtFail = false;
				bool showOnlyFailed = false;
				bool listTests = false;
				String filename;
				String typeName;
				Assembly assembly;
				Type type;
				MethodInfo method;
				Test test;
				Test specificTest;
				TestResult result;

				// Parse the command-line options.
				while(argNum < numArgs && args[argNum].StartsWith("-"))
				{
					arg = args[argNum];
					if(arg == "-s" || arg == "--stop-at-fail")
					{
						stopAtFail = true;
					}
					else if(arg == "-f" || arg == "--show-failed")
					{
						showOnlyFailed = true;
					}
					else if(arg == "-l" || arg == "--list")
					{
						listTests = true;
					}
					else if(arg == "-v" || arg == "--version")
					{
						ShowVersion();
						return 0;
					}
					else
					{
						ShowUsage();
						return 1;
					}
					++argNum;
				}

				// Get the filename for the test assembly.
				if(argNum >= numArgs)
				{
					ShowUsage();
					return 1;
				}
				filename = args[argNum++];

				// Get the name of the class within the test assembly.
				if(argNum >= numArgs)
				{
					ShowUsage();
					return 1;
				}
				typeName = args[argNum++];

				// Load the test assembly.  This will throw an
				// exception if something went wrong, which will
				// cause the program to exit with an explaination.
				assembly = Assembly.Load("file://" + filename);

				// Look for the test type within the assembly.
				type = assembly.GetType(typeName, false);
				if(type == null)
				{
					Console.Error.WriteLine
						(typeName + ": type does not exist in " + filename);
					return 1;
				}

				// Call the "type.Suite()" method to construct the
				// top-level test object, which is normally a suite.
				method = type.GetMethod("Suite", BindingFlags.Public |
												 BindingFlags.Static,
									    null, Type.EmptyTypes, null);
				if(method == null)
				{
					// Try again, in case the user prefers lower case names.
					method = type.GetMethod("suite", Type.EmptyTypes);
				}
				if(method == null)
				{
					Console.Error.WriteLine
						(typeName + ".Suite(): method does not exist in " + filename);
					return 1;
				}
				test = (Test)(method.Invoke(null, null));
				if(test == null)
				{
					Console.Error.WriteLine
						(typeName + ".Suite(): method returned null");
					return 1;
				}

				// Construct the TestResult class to collect up the results.
				result = new TestWriterResult
					(Console.Out, stopAtFail, showOnlyFailed);

				// List or run the tests.
				if(listTests)
				{
					if(argNum < numArgs)
					{
						// List only the specified tests.
						while(argNum < numArgs)
						{
							specificTest = test.Find(args[argNum++]);
							if(specificTest == null)
							{
								Console.Error.WriteLine
									(specificTest + ": unknown test name");
							}
							else
							{
								specificTest.List(result);
							}
						}
					}
					else
					{
						// List all tests.
						test.List(result);
					}
				}
				else if(argNum < numArgs)
				{
					// Run only the specified tests.
					try
					{
						while(argNum < numArgs)
						{
							specificTest = test.Find(args[argNum++]);
							if(specificTest == null)
							{
								Console.Error.WriteLine
									(specificTest + ": unknown test name");
							}
							else
							{
								specificTest.Run(result);
							}
						}
					}
					catch(TestStop)
					{
						// Thrown by "TestWriterResult" to stop
						// testing at the first failure.
					}
					result.ReportSummary();
				}
				else
				{
					// Run all tests.
					try
					{
						test.Run(result);
					}
					catch(TestStop)
					{
						// Thrown by "TestWriterResult" to stop
						// testing at the first failure.
					}
					result.ReportSummary();
				}

				// Done.
				return 0;
			}

	// Show the version information for this program.
	private static void ShowVersion()
			{
				Console.WriteLine
					("CSUNIT " + TestVersion.Version +
					 " - C# Unit Testing Framework");
				Console.WriteLine
					("Copyright (c) 2001 Southern Storm Software, Pty Ltd.");
				Console.WriteLine();
				Console.WriteLine
					("CSUNIT comes with ABSOLUTELY NO WARRANTY.  This is free software,");
				Console.WriteLine
					("and you are welcome to redistribute it under the terms of the");
				Console.WriteLine
					("GNU General Public License.  See the file COPYING for further details.");
				Console.WriteLine();
				Console.WriteLine
					("Use the `--help' option to get help on the command-line options.");
			}

	// Show the usage message for this program.
	private static void ShowUsage()
			{
				Console.Error.WriteLine
					("CSUNIT " + TestVersion.Version +
					 " - C# Unit Testing Framework");
				Console.Error.WriteLine
					("Copyright (c) 2001 Southern Storm Software, Pty Ltd.");
				Console.Error.WriteLine();
				Console.Error.WriteLine
					("Usage: csunit [options] assembly type [test ...]");
				Console.Error.WriteLine();
				Console.Error.WriteLine
					("    --stop-at-fail  or -s");
				Console.Error.WriteLine
					("        Stop at the first failed test.");
				Console.Error.WriteLine
					("    --show-failed   or -f");
				Console.Error.WriteLine
					("        Only show tests that have failed.");
				Console.Error.WriteLine
					("    --list          or -l");
				Console.Error.WriteLine
					("        List all test suites and tests that are registered.");
				Console.Error.WriteLine
					("    --version       or -v");
				Console.Error.WriteLine
					("        Print the version of the program.");
				Console.Error.WriteLine
					("    --help");
				Console.Error.WriteLine
					("        Print this help message.");
			}

}; // class TestMain

}; // namespace CSUnit
