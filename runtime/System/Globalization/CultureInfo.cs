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
using System.Runtime.CompilerServices;
using System.Text;

public class CultureInfo : ICloneable, IFormatProvider
{

	// Cached culture objects.
	private static CultureInfo invariantCulture;
	private static CultureInfo currentCulture;
	private static bool gettingCurrent;

	// Internal state.
	private int         cultureID;
	private CultureName cultureName;
	private bool		readOnly;
	private Calendar	calendar;
	private Calendar[]	otherCalendars;
	private NumberFormatInfo numberFormat;
	private CompareInfo compareInfo;
	private DateTimeFormatInfo dateTimeFormat;
	private TextInfo	textInfo;
	private bool        userOverride;
	private _I18NCultureHandler handler;

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
					cultureName = CultureNameTable.GetNameInfoByID
						(0x0C0A, true);
					handler = _I18NCultureHandler.GetCultureHandler
						(cultureID, useUserOverride);
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
					cultureName = CultureNameTable.GetNameInfoByID
						(culture, true);
					handler = _I18NCultureHandler.GetCultureHandler
						(cultureID, useUserOverride);
				}
				userOverride = useUserOverride;
			}
	public CultureInfo(String name, bool useUserOverride)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				cultureName = CultureNameTable.GetNameInfoByName(name, true);
				cultureID   = cultureName.cultureID;
				userOverride = useUserOverride;
				handler = _I18NCultureHandler.GetCultureHandler
					(cultureID, useUserOverride);
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

	// Get the current culture identifier from the runtime engine.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static int InternalCultureID();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static String InternalCultureName();

	// Get the current culture object for the running thread.
	public static CultureInfo CurrentCulture
			{
				get
				{
					lock(typeof(CultureInfo))
					{
						if(currentCulture != null)
						{
							return currentCulture;
						}
						if(gettingCurrent)
						{
							// We were recursively called during initialization,
							// so return the invariant culture for now.
							return InvariantCulture;
						}
						gettingCurrent = true;
						int id = InternalCultureID();
						if(id <= 0)
						{
							// Try getting the name instead, in case this
							// engine doesn't know about culture ID's.
							String name = InternalCultureName();
							id = MapNameToID(name, false);
						}
						if(id <= 0 ||
						   _I18NCultureHandler.GetCultureHandler
						   		(id, true) == null)
						{
							currentCulture = InvariantCulture;
						}
						else
						{
							currentCulture = new CultureInfo(id);
							currentCulture.readOnly = true;
						}
						gettingCurrent = false;
						return currentCulture;
					}
				}
			}

	// Get the current UI culture object for the running thread.
	public static CultureInfo CurrentUICulture
			{
				get
				{
					// Just use the current culture, since most platforms
					// do not distinguish between UI and non-UI cultures.
					return CurrentCulture;
				}
			}

	// Get the installed UI culture object for the system.
	public static CultureInfo InstalledUICulture
			{
				get
				{
					// Just use the current culture, since most platforms
					// do not distinguish between UI and non-UI cultures.
					return CurrentCulture;
				}
			}

	// Create a CultureInfo object for a specific culture.
	public static CultureInfo CreateSpecificCulture(String name)
			{
				CultureInfo culture = new CultureInfo(name);
				return culture;
			}

	// Get the list of all cultures supported by this system.
	public static CultureInfo[] GetCultures(CultureTypes types)
			{
				Object obj = Encoding.InvokeI18N("GetCultures", types);
				if(obj != null)
				{
					return (CultureInfo[])obj;
				}
				else if((types & CultureTypes.NeutralCultures) != 0)
				{
					CultureInfo[] cultures = new CultureInfo [1];
					cultures[0] = InvariantCulture;
					return cultures;
				}
				else
				{
					return new CultureInfo [0];
				}
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
	public virtual Calendar Calendar
			{
				get
				{
					lock(this)
					{
						if(calendar == null)
						{
							if(handler != null)
							{
								calendar = handler.CultureCalendar;
								if(calendar == null)
								{
									calendar = new GregorianCalendar();
								}
							}
							else
							{
								calendar = new GregorianCalendar();
							}
						}
						return calendar;
					}
				}
			}

	// Get the comparison rules that are used by the culture.
	public virtual CompareInfo CompareInfo
			{
				get
				{
					lock(this)
					{
						if(compareInfo == null)
						{
							if(handler != null)
							{
								compareInfo = handler.CultureCompareInfo;
								if(compareInfo == null)
								{
									compareInfo =
										CompareInfo.InvariantCompareInfo;
								}
							}
							else
							{
								compareInfo = CompareInfo.InvariantCompareInfo;
							}
						}
						return compareInfo;
					}
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
						else if(handler != null)
						{
							dateTimeFormat = handler.CultureDateTimeFormatInfo;
							if(dateTimeFormat == null)
							{
								dateTimeFormat = new DateTimeFormatInfo();
							}
						}
						else
						{
							dateTimeFormat = new DateTimeFormatInfo();
						}
					}
					if(readOnly)
					{
						// Wrap up the date/time formatting information
						// to make it read-only like the culture.
						dateTimeFormat = DateTimeFormatInfo.ReadOnly
							(dateTimeFormat);
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
						else if(handler != null)
						{
							numberFormat = handler.CultureNumberFormatInfo;
							if(numberFormat == null)
							{
								numberFormat = new NumberFormatInfo();
							}
						}
						else
						{
							numberFormat = new NumberFormatInfo();
						}
					}
					if(readOnly)
					{
						// Wrap up the number formatting information
						// to make it read-only like the culture.
						numberFormat = NumberFormatInfo.ReadOnly
							(numberFormat);
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
	public virtual Calendar[] OptionalCalendars
			{
				get
				{
					lock(this)
					{
						if(otherCalendars == null)
						{
							if(handler != null)
							{
								otherCalendars = handler.CultureOtherCalendars;
							}
							else
							{
								otherCalendars = new Calendar [0];
							}
						}
						return otherCalendars;
					}
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
	public virtual TextInfo TextInfo
			{
				get
				{
					lock(this)
					{
						if(textInfo == null)
						{
							if(handler != null)
							{
								textInfo = handler.CultureTextInfo;
								if(textInfo == null)
								{
									textInfo = new TextInfo(cultureID);
								}
							}
							else
							{
								textInfo = new TextInfo(cultureID);
							}
						}
						return textInfo;
					}
				}
			}

	// Get the 3-letter ISO language name for this culture.
	public virtual String ThreeLetterISOLanguageName
			{
				get
				{
					return cultureName.threeLetterISOName;
				}
			}

	// Get the 3-letter Windows language name for this culture.
	public virtual String ThreeLetterWindowsLanguageName
			{
				get
				{
					return cultureName.threeLetterWindowsName;
				}
			}

	// Get the 2-letter ISO language name for this culture.
	public virtual String TwoLetterISOLanguageName
			{
				get
				{
					return cultureName.twoLetterISOName;
				}
			}

	// Determine if this culture is configured for user overrides.
	public virtual bool UseUserOverride
			{
				get
				{
					return userOverride;
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

	// Map a culture name to an identifier.
	internal static int MapNameToID(String name, bool throwOnError)
			{
				CultureName cultureName =
					CultureNameTable.GetNameInfoByName(name, throwOnError);
				if(cultureName != null)
				{
					return cultureName.cultureID;
				}
				else
				{
					return 0;
				}
			}

}; // class CultureInfo

}; // namespace System.Globalization
