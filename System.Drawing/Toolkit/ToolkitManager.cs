/*
 * ToolkitManager.cs - Implementation of the
 *			"System.Drawing.Toolkit.ToolkitManager" class.
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

namespace System.Drawing.Toolkit
{

using System.Drawing.Printing;
using System.Reflection;
using System.IO;

public sealed class ToolkitManager
{
	// Global state.
	private static IToolkit toolkit;
	private static IToolkitPrintingSystem printingSystem;

	// Get or set the active graphical display toolkit.
	public static IToolkit Toolkit
			{
				get
				{
					lock(typeof(ToolkitManager))
					{
						if(toolkit == null)
						{
							toolkit = CreateDefaultToolkit();
						}
						return toolkit;
					}
				}
				set
				{
					lock(typeof(ToolkitManager))
					{
						toolkit = value;
					}
				}
			}

	// Determine if we currently have a graphical toolkit.
	public static bool HasToolkit
			{
				get
				{
					lock(typeof(ToolkitManager))
					{
						return (toolkit != null);
					}
				}
			}

	// Get or set the active printing system.
	[TODO]
	public static IToolkitPrintingSystem PrintingSystem
			{
				get
				{
					lock(typeof(ToolkitManager))
					{
						if(printingSystem == null)
						{
							// TODO: create a system-specific printing system.
							printingSystem = new NullPrintingSystem();
						}
						return printingSystem;
					}
				}
				set
				{
					lock(typeof(ToolkitManager))
					{
						printingSystem = value;
					}
				}
			}

	// Get a standard paper size object.
	public static PaperSize GetStandardPaperSize(PaperKind kind)
			{
				return new PaperSize(kind);
			}

	// Get a standard paper source object.
	public static PaperSource GetStandardPaperSource(PaperSourceKind kind)
			{
				return new PaperSource(kind, null);
			}

	// Get a standard printer resolution object.
	public static PrinterResolution GetStandardPrinterResolution
				(PrinterResolutionKind kind, int x, int y)
			{
				return new PrinterResolution(kind, x, y);
			}

	// Create a "Graphics" object from an "IToolkitGraphics" handler.
	public static Graphics CreateGraphics(IToolkitGraphics graphics)
			{
				if(graphics == null)
				{
					throw new ArgumentNullException("graphics");
				}
				return new Graphics(graphics);
			}

	// Create the default toolkit.
	private static IToolkit CreateDefaultToolkit()
			{
			#if CONFIG_REFLECTION
				// Determine the name of the toolkit we wish to use.
				String name;
				if(Path.DirectorySeparatorChar == '\\' ||
				   Path.AltDirectorySeparatorChar == '\\')
				{
					name = "System.Drawing.Win32";
				}
				else
				{
					name = "System.Drawing.Xsharp";
				}

				// Load the toolkit's assembly.
				Assembly assembly = Assembly.Load(name);

				// Find the "System.Drawing.Toolkit.DrawingToolkit" class.
				Type type = assembly.GetType
					("System.Drawing.Toolkit.DrawingToolkit");
				if(type == null)
				{
					throw new NotSupportedException();
				}

				// Instantiate "DrawingToolkit" and return it.
				ConstructorInfo ctor = type.GetConstructor(new Type [0]);
				return (IToolkit)(ctor.Invoke(new Object [0]));
			#else
				// TODO: return a null toolkit
				return null;
			#endif
			}

}; // class ToolkitManager

}; // namespace System.Drawing.Toolkit
