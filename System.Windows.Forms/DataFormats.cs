/*
 * DataFormats.cs - Implementation of the
 *			"System.Windows.Forms.DataFormats" class.
 *
 * Copyright (C) 2003  Free Software Foundation, Inc.
 *
 * Contributions from Cecilio Pardo <cpardo@imayhem.com>
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

namespace System.Windows.Forms
{

using System.Collections;

[TODO]
public class DataFormats
{
	public static readonly String Bitmap = "Bitmap";
	public static readonly String CommaSeparatedValue = "Csv";
	public static readonly String Dib = "DeviceIndependentBitmap";
	public static readonly String Dif = "DataInterchangeFormat";
	public static readonly String EnhancedMetafile = "EnhancedMetafile";
	public static readonly String FileDrop = "FileDrop";
	public static readonly String Html = "HTML Format";
	public static readonly String Locale = "Locale";
	public static readonly String MetafilePict = "MetaFilePict";
	public static readonly String OemText = "OEMText";
	public static readonly String Palette = "Palette";
	public static readonly String PenData = "PenData";
	public static readonly String Riff = "RiffAudio";
	public static readonly String Rtf = "Rich Text Format";
	public static readonly String Serializable = "WindowsForms10PersistentObject";
	public static readonly String StringFormat = "System.String";
	public static readonly String SymbolicLink = "SymbolicLink";
	public static readonly String Text = "Text";
	public static readonly String Tiff = "TaggedImageFileFormat";
	public static readonly String UnicodeText = "UnicodeText";
	public static readonly String WaveAudio = "WaveAudio";

	private static ArrayList formats;

	[TODO]
	static DataFormats()
	{
		// TODO: On windows, load extra formats from the registry

		formats = new ArrayList();

		// Numbers taken from MinGW's winuser.h
		formats.Add(new Format(Text, 1));
		formats.Add(new Format(Bitmap, 2));
		formats.Add(new Format(MetafilePict, 3));
		formats.Add(new Format(SymbolicLink, 4));
		formats.Add(new Format(Dif, 5));
		formats.Add(new Format(Tiff, 6));
		formats.Add(new Format(OemText, 7));
		formats.Add(new Format(Dib, 8));
		formats.Add(new Format(Palette, 9));
		formats.Add(new Format(PenData, 10));
		formats.Add(new Format(Riff, 11));
		formats.Add(new Format(WaveAudio, 12));
		formats.Add(new Format(UnicodeText, 13));
		formats.Add(new Format(EnhancedMetafile, 14));
		formats.Add(new Format(FileDrop, 15));
		formats.Add(new Format(Locale, 16));

		// TODO: These values are not predefined, so they need
		//       to be registered with the system, and the id
		//       values obtained upon registering them. Faked
		//       id values are used for now.
		formats.Add(new Format(StringFormat, 1001));
		formats.Add(new Format(CommaSeparatedValue, 1002));
		formats.Add(new Format(Html, 1003));
		formats.Add(new Format(Rtf, 1004));
		formats.Add(new Format(Serializable, 1005));
	}

	private static int GetNextID()
	{
		int maxid = 0;

		foreach (Format f in formats)
		{
			if (f.Id > maxid)
			{
				maxid = f.Id + 1;
			}
		}

		return maxid;
	}

	[TODO]
	public static Format GetFormat(int id)
	{
		// TODO: On Windows, the registry should
		//       be watched for new formats.
		foreach (Format f in formats)
		{
			if (f.Id == id)
			{
				return f;
			}
		}
		return new Format("Format" + id.ToString(), id);
	}

	[TODO]
	public static Format GetFormat(String name)
	{
		// TODO: Add the format to the registry.
		//       The registry should also be
		//       watched for new formats.
		foreach (Format f in formats)
		{
			if (f.Name.Equals(name))
			{
				return f;
			}
		}
		Format f = new Format(name, GetNextID());
		formats.Add(f);

		return f;
	}











	public class Format
	{
		private String name;
		private int id;

		public Format(String name, int id) : base()
		{
			this.id = id;
			this.name = name;
		}

		public int Id
		{
			get { return id; }
		}

		public String Name
		{
			get { return name; }
		}
	}; // class Format

}; // class DataFormats

}; // namespace System.Windows.Forms
