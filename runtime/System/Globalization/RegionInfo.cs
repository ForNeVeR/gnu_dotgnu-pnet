/*
 * RegionInfo.cs - Implementation of "System.Globalization.RegionInfo".
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#if !ECMA_COMPAT

using System;
using System.Private;

public class RegionInfo
{
	// Internal state.
	private RegionName regionName;

	// The current region.
	private static RegionInfo currentRegion;

	// Constructors.
	public RegionInfo(int culture)
			{
				regionName = RegionNameTable.GetNameInfoByID(culture, true);
			}
	public RegionInfo(String name)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				regionName = RegionNameTable.GetNameInfoByName(name, true);
			}

	// Get the region for the current culture.
	public static RegionInfo CurrentRegion
			{
				get
				{
					lock(typeof(RegionInfo))
					{
						if(currentRegion == null)
						{
							currentRegion =
								new RegionInfo(CultureInfo.CurrentCulture.LCID);
						}
						return currentRegion;
					}
				}
			}

	// Get the region properties.
	public virtual String CurrencySymbol
			{
				get
				{
					return regionName.currencySymbol;
				}
			}
	public virtual String DisplayName
			{
				get
				{
					// The "DisplayName" returns the name of the region
					// in the installation language, not the current locale.
					// In our system, the installation language is always
					// going to be English.
					return regionName.englishName;
				}
			}
	public virtual String EnglishName
			{
				get
				{
					return regionName.englishName;
				}
			}
	public virtual bool IsMetric
			{
				get
				{
					return regionName.isMetric;
				}
			}
	public virtual String ISOCurrencySymbol
			{
				get
				{
					return regionName.isoCurrencySymbol;
				}
			}
	public virtual String Name
			{
				get
				{
					return regionName.name;
				}
			}
	public virtual String ThreeLetterISORegionName
			{
				get
				{
					return regionName.threeLetterISOName;
				}
			}
	public virtual String ThreeLetterWindowsRegionName
			{
				get
				{
					return regionName.threeLetterWindowsName;
				}
			}
	public virtual String TwoLetterISORegionName
			{
				get
				{
					return regionName.twoLetterISOName;
				}
			}

	// Determine if two region information objects are equal.
	public override bool Equals(Object obj)
			{
				RegionInfo other = (obj as RegionInfo);
				if(other != null)
				{
					return (regionName.regionID ==
							other.regionName.regionID);
				}
				else
				{
					return false;
				}
			}

	// Get the hash code for this object.
	public override int GetHashCode()
			{
				return regionName.regionID;
			}

	// Convert this region into a string.
	public override String ToString()
			{
				return regionName.threeLetterISOName;
			}

}; // class RegionInfo

#endif // CONFIG_REFLECTION

}; // namespace System.Globalization
