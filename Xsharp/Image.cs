/*
 * Image.cs - Basic image handling for X applications.
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

namespace Xsharp
{

using System;

/// <summary>
/// <para>The <see cref="T:Xsharp.Image"/> class manages an image
/// consisting of a pixmap and an optional mask.  Images may be
/// created in memory or loaded from a file.</para>
/// </summary>
public sealed class Image : IDisposable
{
	// Internal state.
	private Xsharp.Pixmap pixmap;
	private Bitmap mask;

	/// <summary>
	/// <para>Constructs a new <see cref="T:Xsharp.Image"/> instance
	/// that represents an off-screen image.</para>
	/// </summary>
	///
	/// <param name="width">
	/// <para>The width of the new image.</para>
	/// </param>
	///
	/// <param name="height">
	/// <para>The height of the new image.</para>
	/// </param>
	///
	/// <param name="hasMask">
	/// <para>Set to <see langword="null"/> if the optional mask
	/// should also be created.</para>
	/// </param>
	///
	/// <exception cref="T:Xsharp.XException">
	/// <para>The <paramref name="width"/> or <paramref name="height"/>
	/// values are out of range.</para>
	/// </exception>
	public Image(int width, int height, bool hasMask)
			{
				pixmap = new Pixmap(width, height);
				if(hasMask)
				{
					mask = new Bitmap(width, height);
				}
				else
				{
					mask = null;
				}
			}

	/// <summary>
	/// <para>Constructs a new <see cref="T:Xsharp.Image"/> instance
	/// that represents an off-screen image on a particular screen.</para>
	/// </summary>
	///
	/// <param name="screen">
	/// <para>The screen upon which to create the new pixmap.</para>
	/// </param>
	///
	/// <param name="width">
	/// <para>The width of the new image.</para>
	/// </param>
	///
	/// <param name="height">
	/// <para>The height of the new image.</para>
	/// </param>
	///
	/// <param name="hasMask">
	/// <para>Set to <see langword="null"/> if the optional mask
	/// should also be created.</para>
	/// </param>
	///
	/// <exception cref="T:Xsharp.XException">
	/// <para>The <paramref name="width"/> or <paramref name="height"/>
	/// values are out of range.</para>
	/// </exception>
	public Image(Screen screen, int width, int height, bool hasMask)
			{
				pixmap = new Xsharp.Pixmap(screen, width, height);
				if(hasMask)
				{
					mask = new Bitmap(screen, width, height);
				}
				else
				{
					mask = null;
				}
			}

	/// <summary>
	/// <para>Constructs a new <see cref="T:Xsharp.Image"/> instance
	/// that represents an off-screen image that was loaded
	/// from a file.</para>
	/// </summary>
	///
	/// <param name="filename">
	/// <para>The file to load the image from.</para>
	/// </param>
	///
	/// <exception cref="T:System.ArgumentNullException">
	/// <para>The <paramref name="filename"/> parameter is
	/// <see langword="null"/>.</para>
	/// </exception>
	///
	/// <exception cref="T:Xsharp.XInvalidOperationException">
	/// <para>Raised if <paramref name="filename"/> could not be
	/// loaded for some reason.</para>
	/// </exception>
	public Image(String filename) : this(null, filename) {}

	/// <summary>
	/// <para>Constructs a new <see cref="T:Xsharp.Image"/> instance
	/// that represents an off-screen image that was loaded
	/// from a file.</para>
	/// </summary>
	///
	/// <param name="screen">
	/// <para>The screen upon which to create the new pixmap.</para>
	/// </param>
	///
	/// <param name="filename">
	/// <para>The file to load the image from.</para>
	/// </param>
	///
	/// <exception cref="T:System.ArgumentNullException">
	/// <para>The <paramref name="filename"/> parameter is
	/// <see langword="null"/>.</para>
	/// </exception>
	///
	/// <exception cref="T:Xsharp.XInvalidOperationException">
	/// <para>Raised if <paramref name="filename"/> could not be
	/// loaded for some reason.</para>
	/// </exception>
	public Image(Screen screen, String filename)
			{
				Display dpy;
				if(filename == null)
				{
					throw new ArgumentNullException("filename");
				}
				if(screen != null)
				{
					dpy = screen.DisplayOfScreen;
				}
				else
				{
					dpy = Application.Primary.Display;
					screen = dpy.DefaultScreenOfDisplay;
				}
				try
				{
					// Lock down the display while we do this.
					IntPtr display = dpy.Lock();

					// Call "XShmQueryExtension" before "Imlib_init".
					// This is a hack to make sure that "Xext" is loaded
					// into memory before "Imlib" goes looking for it.
					Xlib.XShmQueryExtension(display);

					// Initialize the Imlib library if necessary.
					if(dpy.imlibData == IntPtr.Zero)
					{
						dpy.imlibData = Xlib.Imlib_init(display);
					}

					// Load the file and convert it into a pixmap and mask.
					Xlib.Pixmap pixmap, mask;
					pixmap = Xlib.Pixmap.Zero;
					mask = Xlib.Pixmap.Zero;
					if(Xlib.Imlib_load_file_to_pixmap
							(dpy.imlibData, filename, ref pixmap, ref mask)
									== 0)
					{
						throw new XInvalidOperationException
							(S._("X_InvalidImageFile"));
					}

					// Wrap the returned XID's in Pixmap/Bitmap objects.
					this.pixmap = new Pixmap(dpy, screen, pixmap);
					if(mask != Xlib.Pixmap.Zero)
					{
						this.mask = new Bitmap(dpy, screen, mask);
					}
					else
					{
						this.mask = null;
					}
				}
				finally
				{
					dpy.Unlock();
				}
			}

	/// <summary>
	/// <para>Retrieve the pixmap that is associated with this image.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The pixmap instance, or <see langword="null"/> if disposed.
	/// </para>
	/// </value>
	public Xsharp.Pixmap Pixmap
			{
				get
				{
					return pixmap;
				}
			}

	/// <summary>
	/// <para>Retrieve the mask that is associated with this image.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The mask instance, or <see langword="null"/> if disposed.
	/// </para>
	/// </value>
	public Bitmap Mask
			{
				get
				{
					return mask;
				}
			}

	/// <summary>
	/// <para>Get the width of the image.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The image's width.</para>
	/// </para>
	/// </value>
	public int Width
			{
				get
				{
					return pixmap.Width;
				}
			}

	/// <summary>
	/// <para>Get the height of the image.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The image's height.</para>
	/// </para>
	/// </value>
	public int Height
			{
				get
				{
					return pixmap.Height;
				}
			}

	/// <summary>
	/// <para>Destroy this image if it is currently active.</para>
	/// </summary>
	public void Destroy()
			{
				if(pixmap != null)
				{
					pixmap.Destroy();
					pixmap = null;
				}
				if(mask != null)
				{
					mask.Destroy();
					mask = null;
				}
			}

	/// <summary>
	/// <para>Dispose this image if it is currently active.</para>
	/// </summary>
	///
	/// <remarks>
	/// <para>This method implements the <see cref="T:System.IDisposable"/>
	/// interface.</para>
	/// </remarks>
	public void Dispose()
			{
				Destroy();
			}

} // class Image

} // namespace Xsharp
