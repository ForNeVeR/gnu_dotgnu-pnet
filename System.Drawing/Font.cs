/*
 * Font.cs - Implementation of the "System.Drawing.Font" class.
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

using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Drawing.Toolkit;
using System.ComponentModel;
using System.Drawing.Design;

#if CONFIG_COMPONENT_MODEL
[Editor("System.Drawing.Design.FontEditor, System.Drawing.Design",
		typeof(UITypeEditor))]
[TypeConverter(typeof(FontConverter))]
#endif
[Serializable]
[ComVisible(true)]
public sealed class Font : MarshalByRefObject, ISerializable,
						   ICloneable, IDisposable
{
	// Internal state.
	private IToolkit toolkit;
	private IToolkitFont toolkitFont;
	private FontFamily fontFamily;
	private FontStyle fontStyle;
	private byte gdiCharSet;
	private bool gdiVerticalFont;
	private float size;
	private GraphicsUnit unit;

	// Constructors.
	public Font(Font prototype, FontStyle newStyle)
			: this(prototype.FontFamily, prototype.Size, newStyle,
				   prototype.Unit, prototype.GdiCharSet,
				   prototype.GdiVerticalFont) {}
	public Font(FontFamily family, float emSize)
			: this(family, emSize, FontStyle.Regular,
			       GraphicsUnit.Point, 1, false) {}
	public Font(FontFamily family, float emSize, FontStyle style)
			: this(family, emSize, style, GraphicsUnit.Point, 1, false) {}
	public Font(FontFamily family, float emSize, GraphicsUnit unit)
			: this(family, emSize, FontStyle.Regular, unit, 1, false) {}
	public Font(FontFamily family, float emSize, FontStyle style,
				GraphicsUnit unit)
			: this(family, emSize, style, unit, 1, false) {}
	public Font(FontFamily family, float emSize, FontStyle style,
				GraphicsUnit unit, byte gdiCharSet)
			: this(family, emSize, style, unit, gdiCharSet, false) {}
	public Font(String familyName, float emSize)
			: this(new FontFamily(familyName), emSize,
			       FontStyle.Regular, GraphicsUnit.Point, 1, false) {}
	public Font(String familyName, float emSize, FontStyle style)
			: this(new FontFamily(familyName), emSize,
			       style, GraphicsUnit.Point, 1, false) {}
	public Font(String familyName, float emSize, GraphicsUnit unit)
			: this(new FontFamily(familyName), emSize,
			       FontStyle.Regular, unit, 1, false) {}
	public Font(String familyName, float emSize, FontStyle style,
				GraphicsUnit unit)
			: this(new FontFamily(familyName), emSize,
			       style, unit, 1, false) {}
	public Font(String familyName, float emSize, FontStyle style,
				GraphicsUnit unit, byte gdiCharSet)
			: this(new FontFamily(familyName), emSize,
			       style, unit, gdiCharSet, false) {}
	public Font(String familyName, float emSize, FontStyle style,
				GraphicsUnit unit, byte gdiCharSet, bool gdiVerticalFont)
			: this(new FontFamily(familyName), emSize,
			       style, unit, gdiCharSet, gdiVerticalFont) {}
	public Font(FontFamily family, float emSize, FontStyle style,
				GraphicsUnit unit, byte gdiCharSet, bool gdiVerticalFont)
			{
				if(family == null)
				{
					throw new ArgumentNullException("family");
				}
				this.fontFamily = family;
				this.size = emSize;
				this.fontStyle = style;
				this.unit = unit;
				this.gdiCharSet = gdiCharSet;
				this.gdiVerticalFont = gdiVerticalFont;
			}
	internal Font(SerializationInfo info, StreamingContext context)
			{
				if(info == null)
				{
					throw new ArgumentNullException("info");
				}
				fontFamily = new FontFamily(info.GetString("Name"));
				size = info.GetSingle("Size");
				fontStyle = (FontStyle)info.GetValue
					("Style", typeof(FontStyle));
				unit = (GraphicsUnit)info.GetValue
					("Unit", typeof(GraphicsUnit));
				gdiCharSet = 1;
				gdiVerticalFont = false;
			}

	// Destructor.
	~Font()
			{
				Dispose();
			}

	// Font properties.
#if CONFIG_COMPONENT_MODEL
	[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
#endif
	public bool Bold
			{
				get
				{
					return ((fontStyle & FontStyle.Bold) != 0);
				}
			}
#if CONFIG_COMPONENT_MODEL
	[Browsable(false)]
#endif
	public FontFamily FontFamily
			{
				get
				{
					return fontFamily;
				}
			}
#if CONFIG_COMPONENT_MODEL
	[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
#endif
	public byte GdiCharSet
			{
				get
				{
					return gdiCharSet;
				}
			}
#if CONFIG_COMPONENT_MODEL
	[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
#endif
	public bool GdiVerticalFont
			{
				get
				{
					return gdiVerticalFont;
				}
			}
#if CONFIG_COMPONENT_MODEL
	[Browsable(false)]
#endif
	public int Height
			{
				get
				{
					// TODO: convert "Size" into the expected value.
					return 0;
				}
			}
#if CONFIG_COMPONENT_MODEL
	[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
#endif
	public bool Italic
			{
				get
				{
					return ((fontStyle & FontStyle.Italic) != 0);
				}
			}
#if CONFIG_COMPONENT_MODEL
	[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
	[Editor("System.Drawing.Design.FontNameEditor, System.Drawing.Design",
			typeof(UITypeEditor))]
	[TypeConverter(typeof(FontConverter.FontNameConverter))]
#endif
	public String Name
			{
				get
				{
					// TODO: get from fontFamily object.
					return null;
				}
			}
	public float Size
			{
				get
				{
					return size;
				}
			}
#if CONFIG_COMPONENT_MODEL
	[Browsable(false)]
#endif
	public float SizeInPoints
			{
				get
				{
					// TODO: convert "Size" into the expected value.
					return 0.0f;
				}
			}
#if CONFIG_COMPONENT_MODEL
	[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
#endif
	public bool Strikeout
			{
				get
				{
					return ((fontStyle & FontStyle.Strikeout) != 0);
				}
			}
#if CONFIG_COMPONENT_MODEL
	[Browsable(false)]
#endif
	public FontStyle Style
			{
				get
				{
					return fontStyle;
				}
			}
#if CONFIG_COMPONENT_MODEL
	[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
#endif
	public bool Underline
			{
				get
				{
					return ((fontStyle & FontStyle.Underline) != 0);
				}
			}
#if CONFIG_COMPONENT_MODEL
	[TypeConverter(typeof(FontConverter.FontUnitConverter))]
#endif
	public GraphicsUnit Unit
			{
				get
				{
					return unit;
				}
			}

	// Clone this object.
	public Object Clone()
			{
				lock(this)
				{
					Font font = (Font)(MemberwiseClone());
					font.toolkit = null;
					font.toolkitFont = null;
					return font;
				}
			}

	// Dispose of this object.
	public void Dispose()
			{
				lock(this)
				{
					if(toolkitFont != null)
					{
						toolkitFont.Dispose();
						toolkitFont = null;
					}
					toolkit = null;
				}
			}

	// Determine if two objects are equal.
	public override bool Equals(Object obj)
			{
				Font other = (obj as Font);
				if(other != null)
				{
					return (other.Name == Name &&
							other.Size == Size &&
							other.Style == Style &&
							other.GdiCharSet == GdiCharSet &&
							other.GdiVerticalFont == GdiVerticalFont);
				}
				else
				{
					return false;
				}
			}

	// Get a hash code for this object.
	public override int GetHashCode()
			{
				return Name.GetHashCode() + (((int)size) << 4) +
				       (int)fontStyle;
			}

	// Extract the active font from a native device context.
	[TODO]
	public static Font FromHdc(IntPtr hdc)
			{
				// TODO
				return null;
			}

	// Convert a native font handle into a font object.
	[TODO]
	public static Font FromHfont(IntPtr hfont)
			{
				// TODO
				return null;
			}

	// Convert a native logical font descriptor into a font object.
	public static Font FromLogFont(Object lf)
			{
				return FromLogFont(lf, IntPtr.Zero);
			}
	[TODO]
	public static Font FromLogFont(Object lf, IntPtr hdc)
			{
				// TODO
				return null;
			}

	// Get the height of this font.
	[TODO]
	public float GetHeight()
			{
				// TODO
				return 0.0f;
			}
	[TODO]
	public float GetHeight(Graphics graphics)
			{
				// TODO
				return 0.0f;
			}
	[TODO]
	public float GetHeight(float dpi)
			{
				// TODO
				return 0.0f;
			}

	// Convert this object into a native font handle.
	[TODO]
	public IntPtr ToHfont()
			{
				// TODO
				return IntPtr.Zero;
			}

	// Fill in a native font information structure with info about this font.
	public void ToLogFont(Object lf)
			{
				ToLogFont(lf, null);
			}
	[TODO]
	public void ToLogFont(Object lf, Graphics graphics)
			{
				// TODO
			}

	// Convert this object into a string.
	public override String ToString()
			{
				StringBuilder builder = new StringBuilder();
				builder.Append("[Font: Name=");
				builder.Append(Name);
				builder.Append(" Size=");
				builder.Append(Size);
				builder.Append(" Units=");
				builder.Append((int)Unit);
				builder.Append(" GdiCharSet=");
				builder.Append(GdiCharSet);
				builder.Append(" GdiVerticalFont=");
				builder.Append(GdiVerticalFont);
				builder.Append("]");
				return builder.ToString();
			}

	// Get the serialization information for this object.
	void ISerializable.GetObjectData(SerializationInfo info,
									 StreamingContext context)
			{
				if(info == null)
				{
					throw new ArgumentNullException("info");
				}
				info.AddValue("Name", Name);
				info.AddValue("Size", Size);
				info.AddValue("Style", Style);
				info.AddValue("Unit", Unit);
			}

	// Get the toolkit version of this font for a specific toolkit.
	internal IToolkitFont GetFont(IToolkit toolkit)
			{
				lock(this)
				{
					if(this.toolkitFont == null)
					{
						// We don't yet have a toolkit font yet.
						this.toolkitFont = toolkit.CreateFont(this);
						this.toolkit = toolkit;
						return this.toolkitFont;
					}
					else if(this.toolkit == toolkit)
					{
						// Same toolkit - return the cached pen information.
						return this.toolkitFont;
					}
					else
					{
						// We have a font object for another toolkit,
						// so dispose it and create for this toolkit.
						// We null out "toolkitFont" before calling
						// "CreateFont()" just in case an exception
						// is thrown while creating the toolkit font.
						this.toolkitFont.Dispose();
						this.toolkitFont = null;
						this.toolkitFont = toolkit.CreateFont(this);
						this.toolkit = toolkit;
						return this.toolkitFont;
					}
				}
			}

}; // class Font

}; // namespace System.Drawing
