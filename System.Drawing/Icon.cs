/*
 * Icon.cs - Implementation of the "System.Drawing.Icon" class.
 *
 * Copyright (C) 2003  Neil Cawse.
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

namespace System.Drawing
{

using System;
using System.IO;
using System.Drawing.Imaging;

#if CONFIG_SERIALIZATION
using System.Runtime.Serialization;
#endif

#if CONFIG_COMPONENT_MODEL
using System.ComponentModel;
#endif

public sealed class Icon : MarshalByRefObject, ICloneable
#if CONFIG_SERIALIZATION
	, ISerializable
#endif
{
	private byte[] data;
	private Size size;

	public IntPtr Handle
	{
		get
		{
			// Not used in this implementation
			return IntPtr.Zero;
		}
	}

	public int Height
	{
		get
		{
			return size.Height;
		}
	}

	public Size Size
	{
		get
		{
			return size;
		}
	}

	public int Width
	{
		get
		{
			return size.Width;
		}
	}

	public Icon(string fileName)
	{
		using (FileStream fileStream = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.Read))
		{
			data = new byte[fileStream.Length];
			fileStream.Read(data, 0, data.Length);
		}
		SetSizeFromData();
	}

	public Icon(Icon original, Size size) : this(original, size.Width, size.Height)
	{}

	public Icon(Icon original, int width, int height)
	{
		data = original.data;
		if (data == null)
			size = original.Size;
		else
			size = new Size(width, height);
	}

#if !ECMA_COMPAT
	public Icon(Type type, string resource)
	{
		using(Stream stream = type.Module.Assembly.GetManifestResourceStream(type, resource))
		{
			data = new byte[stream.Length];
			stream.Read(data, 0, data.Length);
		}
		SetSizeFromData();
	}
#endif

	public Icon(Stream stream)
	{
		data = new byte[stream.Length];
		stream.Read(data, 0, data.Length);
	}

	public Icon(Stream stream, int width, int height)
	{
		data = new byte[stream.Length];
		stream.Read(data, 0, data.Length);
		size = new Size(width, height);
	}

	[TODO]
	private void SetSizeFromData()
	{
	}

	object ICloneable.Clone()
	{
		return new System.Drawing.Icon(this, Width, Height);
	}

	public static Icon FromHandle(IntPtr handle)
	{
		// Not implemented
		return null;
	}

	public void Save(Stream outputStream)
	{
		outputStream.Write(data, 0, data.Length);
	}

	public Bitmap ToBitmap()
	{
		Bitmap bitmap = new Bitmap(Size.Width, Size.Height);
		using(Graphics graphics = Graphics.FromImage(bitmap))
			graphics.DrawIcon(this, new Rectangle(0, 0, Size.Width, Size.Height));
		bitmap.MakeTransparent(Color.FromArgb(13, 11, 12));
		return bitmap;
	}

	public override string ToString()
	{
		return "Icon: " + Width + ", " + Height;
	}

#if CONFIG_SERIALIZATION
	void ISerializable.GetObjectData(SerializationInfo si, StreamingContext context)
	{
		if (data != null)
			si.AddValue("IconData", data, typeof(byte[]));
		else
		{
			MemoryStream memoryStream = new MemoryStream();
			Save(memoryStream);
			si.AddValue("IconData", memoryStream.ToArray(), typeof(byte[]));
		}
		si.AddValue("IconSize", size, typeof(Size));
	}
#endif
}

}; // namespace System.Drawing
