/*
 * CultureInfo.cs - Implementation of "System.Globalization.CultureInfo".
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

namespace System.Globalization
{

using System;
using System.Private;

public class CultureInfo : ICloneable, IFormatProvider
{

	// Cached culture objects.
	private static CultureInfo invariantCulture;

	// Internal state.
	private int         cultureID;
	private CultureName cultureName;
	private bool		readOnly;
	private Calendar	calendar;
	private NumberFormatInfo numberFormat;
	private DateTimeFormatInfo dateTimeFormat;
	private TextInfo	textInfo=null;

	// Culture identifier for "es-ES" with traditional sort rules.
	private const int TraditionalSpanish = 0x040A;

	// Constructors.
	public CultureInfo(int culture)
			: this(culture, true)
			{
				// Nothing to do here.
			}
	public CultureInfo(String name)
			: this(name, true)
			{
				// Nothing to do here.
			}
	public CultureInfo(int culture, bool useUserOverride)
			{
				if(culture < 0)
				{
					throw new ArgumentOutOfRangeException
						("culture", _("ArgRange_NonNegative"));
				}
				if(culture == TraditionalSpanish)
				{
					cultureID   = culture;
					cultureName = CultureNameTable.GetNameInfoByID(0x0C0A);
				}
				else if(culture == 0x007F)
				{
					cultureID   = 0x007F;
					cultureName =
						new CultureName
							("iv",    0x007F, "iv", "IVL", "IVL",
							 "Invariant Language (Invariant Country)",
							 "Invariant Language (Invariant Country)");
				}
				else
				{
					cultureID   = culture;
					cultureName = CultureNameTable.GetNameInfoByID(culture);
				}
				InitCulture();
			}
	public CultureInfo(String name, bool useUserOverride)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				cultureName = CultureNameTable.GetNameInfoByName(name);
				cultureID   = cultureName.cultureID;
				InitCulture();
			}

	// Initialize the platform-specific culture routines.
	private void InitCulture()
			{
			#if false
				CultureName newName;

				// Attempt to get an exact match.
				handle = CultureMethods.GetCulture(cultureName.name,
												   cultureID);
				if(handle != IntPtr.Zero)
				{
					return;
				}

				// Attempt to get the neutral version of the culture.
				if((cultureID & ~0x03FF) != 0)
				{
					newName = CultureNameTable.GetNameInfoByID
						(cultureID & 0x03FF);
					handle = CultureMethods.GetCulture
						(newName.name, newName.cultureID);
					if(handle != IntPtr.Zero)
					{
						return;
					}
				}

				// Attempt to get the default country for the culture.
				if((cultureID & ~0x03FF) != 0x0400 && cultureID != 0x0C0A)
				{
					newName = CultureNameTable.GetNameInfoByID
						((cultureID & 0x03FF) | 0x0400);
					handle = CultureMethods.GetCulture
						(newName.name, newName.cultureID);
					if(handle != IntPtr.Zero)
					{
						return;
					}
				}

				// The system does not support the specified culture.
				throw new ArgumentException(_("Arg_InvalidCulture"));
			#endif
			}

	// Get the invariant culture object.
	public static CultureInfo InvariantCulture
			{
				get
				{
					lock(typeof(CultureInfo))
					{
						if(invariantCulture != null)
						{
							return invariantCulture;
						}
						invariantCulture = new CultureInfo(0x007F);
						invariantCulture.readOnly = true;
						return invariantCulture;
					}
				}
			}

	// Get the current culture object for the running thread.
	[TODO]
	public static CultureInfo CurrentCulture
			{
				get
				{
					// TODO.
					return InvariantCulture;
				}
			}

	// Get the current UI culture object for the running thread.
	[TODO]
	public static CultureInfo CurrentUICulture
			{
				get
				{
					// TODO.
					return null;
				}
			}

	// Get the installed UI culture object for the system.
	[TODO]
	public static CultureInfo InstalledUICulture
			{
				get
				{
					// TODO.
					return null;
				}
			}

	// Create a CultureInfo object for a specific culture.
	public static CultureInfo CreateSpecificCulture(String name)
			{
				CultureInfo culture = new CultureInfo(name);
				return culture;
			}

	// Get the list of all cultures supported by this system.
	[TODO]
	public static CultureInfo[] GetCultures(CultureTypes types)
			{
				// TODO
				return new CultureInfo [0];
			}

	// Wrap a culture information object to make it read-only.
	public static CultureInfo ReadOnly(CultureInfo ci)
			{
				if(ci == null)
				{
					throw new ArgumentNullException("ci");
				}
				if(ci.IsReadOnly)
				{
					return ci;
				}
				else
				{
					CultureInfo culture = (CultureInfo)(ci.MemberwiseClone());
					culture.readOnly = true;
					return culture;
				}
			}

	// Get the default calendar that is used by the culture.
	[TODO]
	public virtual Calendar Calendar
			{
				get
				{
					lock(this)
					{
						if(calendar != null)
						{
							return calendar;
						}
						// TODO: handle non-Gregorian calendars.
						calendar = new GregorianCalendar();
						return calendar;
					}
				}
			}

	// Get the comparison rules that are used by the culture.
	[TODO]
	public virtual CompareInfo CompareInfo
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get or set the date and time formatting information for this culture.
	public virtual DateTimeFormatInfo DateTimeFormat
			{
				get
				{
					if(dateTimeFormat == null)
					{
						if(cultureID == 0x007F && readOnly)
						{
							// This is the invariant culture, so always
							// return the invariant date time format info.
							return DateTimeFormatInfo.InvariantInfo;
						}
						else
						{
							dateTimeFormat = new DateTimeFormatInfo(this);
						}
					}
					return dateTimeFormat;
				}
				set
				{
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					dateTimeFormat = value;
				}
			}

	// Get the display name for this culture.
	public virtual String DisplayName
			{
				get
				{
					// We use the prevailing resources to translate
					// the english name into a display name.
					return _(cultureName.englishName);
				}
			}

	// Get the English display name for this culture.
	public virtual String EnglishName
			{
				get
				{
					return cultureName.englishName;
				}
			}

	// Determine if this is a neutral culture.
	public virtual bool IsNeutralCulture
			{
				get
				{
					return ((cultureID & ~0x3FF) == 0);
				}
			}

	// Determine if this culture instance is read-only.
	public virtual bool IsReadOnly
			{
				get
				{
					return readOnly;
				}
			}

	// Get the culture identifier for this instance.
	public virtual int LCID
			{
				get
				{
					return cultureID;
				}
			}

	// Get the culture name.
	public virtual String Name
			{
				get
				{
					return cultureName.name;
				}
			}

	// Get the culture's full name in the native language.
	public virtual String NativeName
			{
				get
				{
					return cultureName.nativeName;
				}
			}

	// Get or set the number formatting information for this culture.
	public virtual NumberFormatInfo NumberFormat
			{
				get
				{
					if(numberFormat == null)
					{
						if(cultureID == 0x007F && readOnly)
						{
							// This is the invariant culture, so always
							// return the invariant number format info.
							return NumberFormatInfo.InvariantInfo;
						}
						else
						{
							numberFormat = new NumberFormatInfo(this);
						}
					}
					return numberFormat;
				}
				set
				{
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					numberFormat = value;
				}
			}

	// Get the optional calendars for this instance.
	[TODO]
	public virtual Calendar[] OptionalCalendars
			{
				get
				{
					// TODO
					return new Calendar [0];
				}
			}

	// Get the parent culture.
	public virtual CultureInfo Parent
			{
				get
				{
					if((cultureID & ~0x03FF) == 0)
					{
						return InvariantCulture;
					}
					else
					{
						return new CultureInfo(cultureID & 0x03FF);
					}
				}
			}

	// Get the text writing system associated with this culture.
	[TODO]
	public virtual TextInfo TextInfo
			{
				get
				{
					if(this.textInfo==null)
					{
						this.textInfo=new TextInfo();
					}
					// TODO
					return this.textInfo;
				}
			}

	// Get the 3-letter ISO language name for this culture.
	[TODO]
	public virtual String ThreeLetterISOLanguageName
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get the 3-letter Windows language name for this culture.
	[TODO]
	public virtual String ThreeLetterWindowsLanguageName
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get the 2-letter ISO language name for this culture.
	[TODO]
	public virtual String TwoLetterISOLanguageName
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Determine if this culture is configured for user overrides.
	[TODO]
	public virtual bool UseUserOverride
			{
				get
				{
					// TODO
					return false;
				}
			}

	// Implementation of the ICloneable interface.
	public Object Clone()
			{
				return MemberwiseClone();
			}

	// Implementation of the IFormatProvider interface.
	public Object GetFormat(Type formatType)
			{
				if(formatType == typeof(System.Globalization.CultureInfo))
				{
					return this;
				}
				else
				{
					return null;
				}
			}

}; // class CultureInfo

}; // namespace System.Globalization
