/*
 * FontFamily.cs - Implementation of the "System.Drawing.FontFamily" class.
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

namespace System.Drawing
{

using System.Drawing.Text;

public sealed class FontFamily : MarshalByRefObject, IDisposable
{
	// Internal state.
	private GenericFontFamilies genericFamily;
	private String name;
	private FontCollection fontCollection;

	// Constructors.
	public FontFamily(GenericFontFamilies genericFamily)
			{
				this.genericFamily = genericFamily;
				this.fontCollection = null;
				switch(genericFamily)
				{
					case GenericFontFamilies.Serif:
					default:
					{
						this.name = "Times New Roman";
					}
					break;

					case GenericFontFamilies.SansSerif:
					{
						this.name = "Arial";
					}
					break;

					case GenericFontFamilies.Monospace:
					{
						this.name = "Courier New";
					}
					break;
				}
			}
	public FontFamily(String name) : this(name, null) {}
	public FontFamily(String name, FontCollection fontCollection)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				this.name = name;
				this.fontCollection = fontCollection;

				// Intuit the generic family based on common font names.
				if(String.Compare(name, "Times", true) == 0 ||
				   String.Compare(name, "Times New Roman", true) == 0)
				{
					this.genericFamily = GenericFontFamilies.Serif;
				}
				else if(String.Compare(name, "Helvetica", true) == 0 ||
				        String.Compare(name, "Helv", true) == 0 ||
				        String.Compare
							(name, "Microsoft Sans Serif", true) == 0 ||
				        String.Compare(name, "Arial", true) == 0 ||
				        String.Compare(name, 0, "Arial ", 0, 6, true) == 0)
				{
					this.genericFamily = GenericFontFamilies.SansSerif;
				}
				else if(String.Compare(name, "Courier", true) == 0 ||
				        String.Compare(name, "Courier New", true) == 0)
				{
					this.genericFamily = GenericFontFamilies.Monospace;
				}
				else
				{
					this.genericFamily = GenericFontFamilies.Serif;
				}
			}

	// Destructor.
	~FontFamily()
			{
				// Nothing to do here in this implementation.
			}

	// Get a list of all font families on this system.
	public static FontFamily[] Families
			{
				get
				{
					return GetFamilies(null);
				}
			}

	// Get a generic monospace object.
	public static FontFamily GenericMonospace
			{
				get
				{
					return new FontFamily(GenericFontFamilies.Monospace);
				}
			}

	// Get a generic sans-serif object.
	public static FontFamily GenericSansSerif
			{
				get
				{
					return new FontFamily(GenericFontFamilies.SansSerif);
				}
			}

	// Get a generic serif object.
	public static FontFamily GenericSerif
			{
				get
				{
					return new FontFamily(GenericFontFamilies.Serif);
				}
			}

	// Get the name of this font family.
	public String Name
			{
				get
				{
					return name;
				}
			}

	// Dispose of this object.
	public void Dispose()
			{
				// Nothing to do here in this implementation.
			}

	// Determine if two objects are equal.
	public override bool Equals(Object obj)
			{
				FontFamily other = (obj as FontFamily);
				if(other != null)
				{
					return (other.Name == Name);
				}
				else
				{
					return false;
				}
			}

	// Get the cell ascent for a particular style.
	[TODO]
	public int GetCellAscent(FontStyle style)
			{
				// TODO
				return 0;
			}

	// Get the cell descent for a particular style.
	[TODO]
	public int GetCellDescent(FontStyle style)
			{
				// TODO
				return 0;
			}

	// Get the em height for a particular style.
	[TODO]
	public int GetEmHeight(FontStyle style)
			{
				// TODO
				return 0;
			}

	// Get a list of all font families with a specified graphics context.
	[TODO]
	public static FontFamily[] GetFamilies(Graphics graphics)
			{
				// TODO
				return null;
			}

	// Get a hash code for this object.
	public override int GetHashCode()
			{
				return Name.GetHashCode();
			}

	// Get the line spacing for a particular style.
	[TODO]
	public int GetLineSpacing(FontStyle style)
			{
				// TODO
				return 0;
			}

	// Get the name of this font in a specified language.
	public String GetName(int language)
			{
				// We don't support language identifiers.
				return Name;
			}

	// Determine if a particular font style is available.
	public bool IsStyleAvailable(FontStyle style)
			{
				// We assume that all styles are available and that it is
				// up to the toolkit handlers to emulate unknown styles.
				return true;
			}

	// Convert this object into a string.
	public override String ToString()
			{
				return "[FontFamily: Name=" + Name + "]";
			}

}; // class FontFamily

}; // namespace System.Drawing
