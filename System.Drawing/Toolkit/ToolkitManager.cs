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
using DotGNU.Images;

[NonStandardExtra]
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

	// Determine if this platform appears to be Unix-ish.
	private static bool IsUnix()
			{
			#if !ECMA_COMPAT
				if(Environment.OSVersion.Platform != (PlatformID)128) /* Unix */
			#else
				if(Path.DirectorySeparatorChar == '\\' ||
				   Path.AltDirectorySeparatorChar == '\\')
			#endif
				{
					return false;
				}
				else
				{
					return true;
				}
			}

	// Get or set the active printing system.
	public static IToolkitPrintingSystem PrintingSystem
			{
				get
				{
					lock(typeof(ToolkitManager))
					{
						if(printingSystem == null)
						{
							if(IsUnix())
							{
								printingSystem = new UnixPrintingSystem();
							}
							else
							{
								// TODO: Win32 printing system.
								printingSystem = new NullPrintingSystem();
							}
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

	// Create a "Graphics" object from an "IToolkitGraphics" handler.
	public static Graphics CreateGraphics(IToolkitGraphics graphics, Rectangle baseWindow)
			{
				if(graphics == null)
				{
					throw new ArgumentNullException("graphics");
				}
				return new Graphics(graphics, baseWindow);
			}
	
	// Create a "Graphics" object from an "IToolkitGraphics" handler.
	public static Graphics CreateGraphics(Graphics graphics, Rectangle baseWindow)
			{
				if(graphics == null)
				{
					throw new ArgumentNullException("graphics");
				}
				return new Graphics(graphics, baseWindow);
			}

	// Create a "Graphics" object from an "IToolkitGraphics" handler.
	// Start with a clip that has already been set in the underlying IToolkitGraphics.
	public static Graphics CreateGraphics(IToolkitGraphics graphics, Region clip)
			{
				if(graphics == null)
				{
					throw new ArgumentNullException("graphics");
				}
				Graphics createdGraphics = new Graphics(graphics);
				createdGraphics.SetClipInternal(clip);
				return createdGraphics;
			}

	// Draw a bitmap-based glyph to a "Graphics" object.  "bits" must be
	// in the form of an xbm bitmap.
	public static void DrawGlyph(Graphics graphics, int x, int y,
								 byte[] bits, int bitsWidth, int bitsHeight,
								 Color color)
			{
				graphics.DrawGlyph(x, y, bits, bitsWidth, bitsHeight, color);
			}

	// Get the raw frame data for an "Image" or "Icon" object.
	public static DotGNU.Images.Frame GetImageFrame(Object image)
			{
				if(image is System.Drawing.Image)
				{
					return ((System.Drawing.Image)image).dgImage.GetFrame(0);
				}
				else if(image is System.Drawing.Icon)
				{
					return ((System.Drawing.Icon)image).frame;
				}
				else
				{
					return null;
				}
			}

	// Convert a brush into an exclusive-OR brush which XOR's the brush
	// against a drawing surface, and also includes inferior child
	// windows in the draw operation.
	public static Brush CreateXorBrush(Brush brush)
			{
				return new XorBrush(brush);
			}

	// Get the override toolkit name.
	private static String GetToolkitOverride()
			{
				String name;

				// Search for "--toolkit" in the command-line options.
				String[] args = Environment.GetCommandLineArgs();
				int index;
				name = null;
				for(index = 1; index < args.Length; ++index)
				{
					if(args[index] == "--toolkit")
					{
						if((index + 1) < args.Length)
						{
							name = args[index + 1];
							break;
						}
					}
					else if(args[index].StartsWith("--toolkit="))
					{
						name = args[index].Substring(10);
						break;
					}
				}

				// Check the environment next.
				if(name == null)
				{
					name = Environment.GetEnvironmentVariable
						("PNET_WINFORMS_TOOLKIT");
				}

				// Bail out if no toolkit name specified.
				if(name == null || name == String.Empty)
				{
					return null;
				}

				// Prepend "System.Drawing." if necessary.
				if(name.IndexOf('.') == -1)
				{
					name = "System.Drawing." + name;
				}
				return name;
			}

	// Create the default toolkit.
	private static IToolkit CreateDefaultToolkit()
			{
			#if CONFIG_REFLECTION
				// Determine the name of the toolkit we wish to use.
				String name = GetToolkitOverride();
				if(name == null)
				{
					if(IsUnix())
					{
						name = "System.Drawing.Xsharp";
					}
					else
					{
						name = "System.Drawing.Win32";
					}
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
				return new NullToolkit();
			#endif
			}

}; // class ToolkitManager

}; // namespace System.Drawing.Toolkit
