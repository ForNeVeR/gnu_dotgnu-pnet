/*
 * jsrun.cs - Run scripts from the command-line.
 *
 * Copyright (C) 2003 Southern Storm Software, Pty Ltd.
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
 
using System;
using System.IO;
using System.Text;
using Microsoft.JScript;
using Microsoft.JScript.Vsa;
using Microsoft.Vsa;

public sealed class JSRun
{
	// Main entry point for the application.
	public static int Main(String[] args)
			{
#if CONFIG_EXTENDED_NUMERICS && CONFIG_REFLECTION
				StreamReader reader;
				StringBuilder builder;
				String script;
				char[] buffer;
				int len;
				VsaEngine engine;
				IVsaCodeItem item;

				// We need at least one argument.
				if(args.Length == 0)
				{
				#if CONFIG_SMALL_CONSOLE
					Console.WriteLine("Usage: jsrun script [args]");
				#else
					Console.Error.WriteLine("Usage: jsrun script [args]");
				#endif
					return 1;
				}

				// Load the script into memory as a string.
				try
				{
					reader = new StreamReader(args[0]);
				}
				catch(FileNotFoundException)
				{
				#if CONFIG_SMALL_CONSOLE
					Console.WriteLine
						("jsrun: {0}: No such file or directory", args[0]);
				#else
					Console.Error.WriteLine
						("jsrun: {0}: No such file or directory", args[0]);
				#endif
					return 1;
				}
				builder = new StringBuilder();
				buffer = new char [512];
				while((len = reader.Read(buffer, 0, 512)) > 0)
				{
					builder.Append(buffer, 0, len);
				}
				reader.Close();
				script = builder.ToString();

				// Create an engine instance and add the script to it.
				engine = VsaEngine.CreateEngine();
				engine.SetOption("print", true);
				item = (IVsaCodeItem)(engine.Items.CreateItem
						("script1", VsaItemType.Code, VsaItemFlag.None));
				item.SourceText = script;
				item.SetOption("codebase", args[0]);

				// Compile and run the script.
				if(!engine.Compile())
				{
				#if CONFIG_SMALL_CONSOLE
					Console.WriteLine("jsrun: Could not compile script");
				#else
					Console.Error.WriteLine("jsrun: Could not compile script");
				#endif
					return 1;
				}
				engine.Run();

				// Shut down the engine and exit.
				engine.Close();
				return 0;
#else
				Console.WriteLine
					("JScript is not available in this configuration " +
					 "because the library does\n" +
					 "not have sufficient features to support JScript.");
				return 0;
#endif
			}

}; // class JSRun
