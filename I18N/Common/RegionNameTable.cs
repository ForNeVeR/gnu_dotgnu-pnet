/*
 * RegionNameTable.cs - Implementation of the
 *		"System.Private.RegionNameTable" class.
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

namespace I18N.Common
{

#if !ECMA_COMPAT

using System;
using System.Globalization;

internal sealed class RegionNameTable
{

	// Array of all registered region names.
	private static RegionName[] regions;
	private static int numRegions;

	// Useful constants.
	private const int DefaultTableSize = 64;
	private const int TableExtendSize  = 16;

	// Add an item to the region name table.
	public static void Add(RegionName name)
			{
				if(numRegions < regions.Length)
				{
					regions[numRegions++] = name;
				}
				else
				{
					RegionName[] newRegions;
					newRegions = new RegionName
						[numRegions + TableExtendSize];
					Array.Copy(regions, newRegions, regions.Length);
					regions = newRegions;
					regions[numRegions++] = name;
				}
			}

	// Populate the region name table.  Note: just because
	// a region exists in this table doesn't mean that it is
	// actually supported by the rest of the system.
	public static void PopulateNameTable()
	{
		Add(new RegionName
			("SA", 0x0401, "SA", "SAU", "SAU",
			 "Saudi Arabia", true, "\u0631.\u0633.\u200F", "SAR"));
		Add(new RegionName
			("BG", 0x0402, "BG", "BGR", "BGR",
			 "Bulgaria", true, "\u043B\u0432", "BGL"));
		Add(new RegionName
			("ES", 0x0403, "ES", "ESP", "ESP",
			 "Spain", true, "\u20AC", "EUR"));
		Add(new RegionName
			("TW", 0x0404, "TW", "TWN", "TWN",
			 "Taiwan", true, "NT$", "TWD"));
		Add(new RegionName
			("CZ", 0x0405, "CZ", "CZE", "CZE",
			 "Czech Republic", true, "K\u010D", "CZK"));
		Add(new RegionName
			("DK", 0x0406, "DK", "DNK", "DNK",
			 "Denmark", true, "kr", "DKK"));
		Add(new RegionName
			("DE", 0x0407, "DE", "DEU", "DEU",
			 "Germany", true, "\u20AC", "EUR"));
		Add(new RegionName
			("GR", 0x0408, "GR", "GRC", "GRC",
			 "Greece", true, "\u20AC", "EUR"));
		Add(new RegionName
			("US", 0x0409, "US", "USA", "USA",
			 "United States", false, "$", "USD"));
		Add(new RegionName
			("ES", 0x040A, "ES", "ESP", "ESP",
			 "Spain", true, "\u20AC", "EUR"));
		Add(new RegionName
			("FI", 0x040B, "FI", "FIN", "FIN",
			 "Finland", true, "\u20AC", "EUR"));
		Add(new RegionName
			("FR", 0x040C, "FR", "FRA", "FRA",
			 "France", true, "\u20AC", "EUR"));
		Add(new RegionName
			("IL", 0x040D, "IL", "ISR", "ISR",
			 "Israel", true, "\u20AA", "ILS"));
		Add(new RegionName
			("HU", 0x040E, "HU", "HUN", "HUN",
			 "Hungary", true, "Ft", "HUF"));
		Add(new RegionName
			("IS", 0x040F, "IS", "ISL", "ISL",
			 "Iceland", true, "kr.", "ISK"));
		Add(new RegionName
			("IT", 0x0410, "IT", "ITA", "ITA",
			 "Italy", true, "\u20AC", "EUR"));
		Add(new RegionName
			("JP", 0x0411, "JP", "JPN", "JPN",
			 "Japan", true, "\u00A5", "JPY"));
		Add(new RegionName
			("KR", 0x0412, "KR", "KOR", "KOR",
			 "Korea", true, "\u20A9", "KRW"));
		Add(new RegionName
			("NL", 0x0413, "NL", "NLD", "NLD",
			 "Netherlands", true, "\u20AC", "EUR"));
		Add(new RegionName
			("NO", 0x0414, "NO", "NOR", "NOR",
			 "Norway", true, "kr", "NOK"));
		Add(new RegionName
			("PL", 0x0415, "PL", "POL", "POL",
			 "Poland", true, "z\u0142", "PLN"));
		Add(new RegionName
			("BR", 0x0416, "BR", "BRA", "BRA",
			 "Brazil", true, "R$ ", "BRL"));
		Add(new RegionName
			("RO", 0x0418, "RO", "ROM", "ROM",
			 "Romania", true, "lei", "ROL"));
		Add(new RegionName
			("RU", 0x0419, "RU", "RUS", "RUS",
			 "Russia", true, "\u0440.", "RUR"));
		Add(new RegionName
			("HR", 0x041A, "HR", "HRV", "HRV",
			 "Croatia", true, "kn", "HRK"));
		Add(new RegionName
			("SK", 0x041B, "SK", "SVK", "SVK",
			 "Slovakia", true, "Sk", "SKK"));
		Add(new RegionName
			("AL", 0x041C, "AL", "ALB", "ALB",
			 "Albania", true, "Lek", "ALL"));
		Add(new RegionName
			("SE", 0x041D, "SE", "SWE", "SWE",
			 "Sweden", true, "kr", "SEK"));
		Add(new RegionName
			("TH", 0x041E, "TH", "THA", "THA",
			 "Thailand", true, "\u0E3F", "THB"));
		Add(new RegionName
			("TR", 0x041F, "TR", "TUR", "TUR",
			 "Turkey", true, "TL", "TRL"));
		Add(new RegionName
			("PK", 0x0420, "PK", "PAK", "PAK",
			 "Islamic Republic of Pakistan", true, "Rs", "PKR"));
		Add(new RegionName
			("ID", 0x0421, "ID", "IDN", "IDN",
			 "Indonesia", true, "Rp", "IDR"));
		Add(new RegionName
			("UA", 0x0422, "UA", "UKR", "UKR",
			 "Ukraine", true, "\u0433\u0440\u043D.", "UAH"));
		Add(new RegionName
			("BY", 0x0423, "BY", "BLR", "BLR",
			 "Belarus", true, "\u0440.", "BYB"));
		Add(new RegionName
			("SI", 0x0424, "SI", "SVN", "SVN",
			 "Slovenia", true, "SIT", "SIT"));
		Add(new RegionName
			("EE", 0x0425, "EE", "EST", "EST",
			 "Estonia", true, "kr", "EEK"));
		Add(new RegionName
			("LV", 0x0426, "LV", "LVA", "LVA",
			 "Latvia", true, "Ls", "LVL"));
		Add(new RegionName
			("LT", 0x0427, "LT", "LTU", "LTU",
			 "Lithuania", true, "Lt", "LTL"));
		Add(new RegionName
			("IR", 0x0429, "IR", "IRN", "IRN",
			 "Iran", true, "\u0631\u064A\u0627\u0644", "IRR"));
		Add(new RegionName
			("VN", 0x042A, "VN", "VNM", "VNM",
			 "Viet Nam", true, "\u20AB", "VND"));
		Add(new RegionName
			("AM", 0x042B, "AM", "ARM", "ARM",
			 "Armenia", true, "\u0564\u0580.", "AMD"));
		Add(new RegionName
			("AZ", 0x042C, "AZ", "AZE", "AZE",
			 "Azerbaijan", true, "man.", "AZM"));
		Add(new RegionName
			("ES", 0x042D, "ES", "ESP", "ESP",
			 "Spain", true, "\u20AC", "EUR"));
		Add(new RegionName
			("MK", 0x042F, "MK", "MKD", "MKD",
			 "Former Yugoslav Republic of Macedonia", true,
			 "\u0434\u0435\u043D.", "MKD"));
		Add(new RegionName
			("ZA", 0x0436, "ZA", "ZAF", "ZAF",
			 "South Africa", true, "R", "ZAR"));
		Add(new RegionName
			("GE", 0x0437, "GE", "GEO", "GEO",
			 "Georgia", true, "Lari", "GEL"));
		Add(new RegionName
			("FO", 0x0438, "FO", "FRO", "FRO",
			 "Faeroe Islands", true, "kr", "DKK"));
		Add(new RegionName
			("IN", 0x0439, "IN", "IND", "IND",
			 "India", true, "\u0930\u0941", "INR"));
		Add(new RegionName
			("MY", 0x043E, "MY", "MYS", "MYS",
			 "Malaysia", true, "R", "MYR"));
		Add(new RegionName
			("KZ", 0x043F, "KZ", "KAZ", "KAZ",
			 "Kazakhstan", true, "\u0422", "KZT"));
		Add(new RegionName
			("KG", 0x0440, "KG", "KGZ", "KGZ",
			 "Kyrgyzstan", true, "\u0441\u043E\u043C", "KGS"));
		Add(new RegionName
			("KE", 0x0441, "KE", "KEN", "KEN",
			 "Kenya", false, "S", "KES"));
		Add(new RegionName
			("UZ", 0x0443, "UZ", "UZB", "UZB",
			 "Uzbekistan", true, "su'm", "UZS"));
		Add(new RegionName
			("TA", 0x0444, "TA", "TAT", "TAT",
			 "Tatarstan", true, "\u0440.", "RUR"));
		Add(new RegionName
			("IN", 0x0446, "IN", "IND", "IND",
			 "India", true, "\u0930\u0941", "INR"));
		Add(new RegionName
			("IN", 0x0447, "IN", "IND", "IND",
			 "India", true, "\u0930\u0941", "INR"));
		Add(new RegionName
			("IN", 0x0449, "IN", "IND", "IND",
			 "India", true, "\u0930\u0941", "INR"));
		Add(new RegionName
			("IN", 0x044A, "IN", "IND", "IND",
			 "India", true, "\u0930\u0941", "INR"));
		Add(new RegionName
			("IN", 0x044B, "IN", "IND", "IND",
			 "India", true, "\u0930\u0941", "INR"));
		Add(new RegionName
			("IN", 0x044E, "IN", "IND", "IND",
			 "India", true, "\u0930\u0941", "INR"));
		Add(new RegionName
			("IN", 0x044F, "IN", "IND", "IND",
			 "India", true, "\u0930\u0941", "INR"));
		Add(new RegionName
			("MN", 0x0450, "MN", "MNG", "MNG",
			 "Mongolia", true, "\u20AE", "MNT"));
		Add(new RegionName
			("ES", 0x0456, "ES", "ESP", "ESP",
			 "Spain", true, "\u20AC", "EUR"));
		Add(new RegionName
			("IN", 0x0457, "IN", "IND", "IND",
			 "India", true, "\u0930\u0941", "INR"));
		Add(new RegionName
			("SY", 0x045A, "SY", "SYR", "SYR",
			 "Syria", true, "\u0644.\u0633.\u200F", "SYP"));
		Add(new RegionName
			("MV", 0x0465, "MV", "MDV", "MDV",
			 "Maldives", true, "\u0783.", "MVR"));
		Add(new RegionName
			("IQ", 0x0801, "IQ", "IRQ", "IRQ",
			 "Iraq", true, "\u062F.\u0639.\u200F", "IQD"));
		Add(new RegionName
			("CN", 0x0804, "CN", "CHN", "CHN",
			 "People's Republic of China", true, "\uFFE5", "CNY"));
		Add(new RegionName
			("CH", 0x0807, "CH", "CHE", "CHE",
			 "Switzerland", true, "SFr.", "CHF"));
		Add(new RegionName
			("GB", 0x0809, "GB", "GBR", "GBR",
			 "United Kingdom", true, "\u00A3", "GBP"));
		Add(new RegionName
			("MX", 0x080A, "MX", "MEX", "MEX",
			 "Mexico", true, "$", "MXN"));
		Add(new RegionName
			("BE", 0x080C, "BE", "BEL", "BEL",
			 "Belgium", true, "\u20AC", "EUR"));
		Add(new RegionName
			("CH", 0x0810, "CH", "CHE", "CHE",
			 "Switzerland", true, "SFr.", "CHF"));
		Add(new RegionName
			("BE", 0x0813, "BE", "BEL", "BEL",
			 "Belgium", true, "\u20AC", "EUR"));
		Add(new RegionName
			("NO", 0x0814, "NO", "NOR", "NOR",
			 "Norway", true, "kr", "NOK"));
		Add(new RegionName
			("PT", 0x0816, "PT", "PRT", "PRT",
			 "Portugal", true, "\u20AC", "EUR"));
		Add(new RegionName
			("SP", 0x081A, "SP", "SPB", "SPB",
			 "Serbia", true, "Din.", "YUN"));
		Add(new RegionName
			("FI", 0x081D, "FI", "FIN", "FIN",
			 "Finland", true, "\u20AC", "EUR"));
		Add(new RegionName
			("AZ", 0x082C, "AZ", "AZE", "AZE",
			 "Azerbaijan", true, "man.", "AZM"));
		Add(new RegionName
			("BN", 0x083E, "BN", "BRN", "BRN",
			 "Brunei Darussalam", true, "$", "BND"));
		Add(new RegionName
			("UZ", 0x0843, "UZ", "UZB", "UZB",
			 "Uzbekistan", true, "su'm", "UZS"));
		Add(new RegionName
			("EG", 0x0C01, "EG", "EGY", "EGY",
			 "Egypt", true, "\u062C.\u0645.\u200F", "EGP"));
		Add(new RegionName
			("HK", 0x0C04, "HK", "HKG", "HKG",
			 "Hong Kong S.A.R.", true, "HK$", "HKD"));
		Add(new RegionName
			("AT", 0x0C07, "AT", "AUT", "AUT",
			 "Austria", true, "\u20AC", "EUR"));
		Add(new RegionName
			("AU", 0x0C09, "AU", "AUS", "AUS",
			 "Australia", true, "$", "AUD"));
		Add(new RegionName
			("ES", 0x0C0A, "ES", "ESP", "ESP",
			 "Spain", true, "\u20AC", "EUR"));
		Add(new RegionName
			("CA", 0x0C0C, "CA", "CAN", "CAN",
			 "Canada", true, "$", "CAD"));
		Add(new RegionName
			("SP", 0x0C1A, "SP", "SPB", "SPB",
			 "Serbia", true, "Din.", "YUN"));
		Add(new RegionName
			("LY", 0x1001, "LY", "LBY", "LBY",
			 "Libya", true, "\u062F.\u0644.\u200F", "LYD"));
		Add(new RegionName
			("SG", 0x1004, "SG", "SGP", "SGP",
			 "Singapore", false, "$", "SGD"));
		Add(new RegionName
			("LU", 0x1007, "LU", "LUX", "LUX",
			 "Luxembourg", true, "\u20AC", "EUR"));
		Add(new RegionName
			("CA", 0x1009, "CA", "CAN", "CAN",
			 "Canada", true, "$", "CAD"));
		Add(new RegionName
			("GT", 0x100A, "GT", "GTM", "GTM",
			 "Guatemala", true, "Q", "GTQ"));
		Add(new RegionName
			("CH", 0x100C, "CH", "CHE", "CHE",
			 "Switzerland", true, "SFr.", "CHF"));
		Add(new RegionName
			("DZ", 0x1401, "DZ", "DZA", "DZA",
			 "Algeria", true, "\u062F.\u062C.\u200F", "DZD"));
		Add(new RegionName
			("MO", 0x1404, "MO", "MAC", "MCO",
			 "Macau S.A.R.", true, "P", "MOP"));
		Add(new RegionName
			("LI", 0x1407, "LI", "LIE", "LIE",
			 "Liechtenstein", true, "CHF", "CHF"));
		Add(new RegionName
			("NZ", 0x1409, "NZ", "NZL", "NZL",
			 "New Zealand", true, "$", "NZD"));
		Add(new RegionName
			("CR", 0x140A, "CR", "CRI", "CRI",
			 "Costa Rica", true, "\u20A1", "CRC"));
		Add(new RegionName
			("LU", 0x140C, "LU", "LUX", "LUX",
			 "Luxembourg", true, "\u20AC", "EUR"));
		Add(new RegionName
			("MA", 0x1801, "MA", "MAR", "MAR",
			 "Morocco", true, "\u062F.\u0645.\u200F", "MAD"));
		Add(new RegionName
			("IE", 0x1809, "IE", "IRL", "IRL",
			 "Ireland", true, "\u20AC", "EUR"));
		Add(new RegionName
			("PA", 0x180A, "PA", "PAN", "PAN",
			 "Panama", true, "B/.", "PAB"));
		Add(new RegionName
			("MC", 0x180C, "MC", "MCO", "MCO",
			 "Principality of Monaco", true, "\u20AC", "EUR"));
		Add(new RegionName
			("TN", 0x1C01, "TN", "TUN", "TUN",
			 "Tunisia", true, "\u062F.\u062A.\u200F", "TND"));
		Add(new RegionName
			("ZA", 0x1C09, "ZA", "ZAF", "ZAF",
			 "South Africa", true, "R", "ZAR"));
		Add(new RegionName
			("DO", 0x1C0A, "DO", "DOM", "DOM",
			 "Dominican Republic", true, "RD$", "DOP"));
		Add(new RegionName
			("OM", 0x2001, "OM", "OMN", "OMN",
			 "Oman", true, "\u0631.\u0639.\u200F", "OMR"));
		Add(new RegionName
			("JM", 0x2009, "JM", "JAM", "JAM",
			 "Jamaica", false, "J$", "JMD"));
		Add(new RegionName
			("VE", 0x200A, "VE", "VEN", "VEN",
			 "Venezuela", true, "Bs", "VEB"));
		Add(new RegionName
			("YE", 0x2401, "YE", "YEM", "YEM",
			 "Yemen", true, "\u0631.\u064A.\u200F", "YER"));
		Add(new RegionName
			("CB", 0x2409, "CB", "CAR", "CAR",
			 "Caribbean", false, "$", "USD"));
		Add(new RegionName
			("CO", 0x240A, "CO", "COL", "COL",
			 "Colombia", true, "$", "COP"));
		Add(new RegionName
			("SY", 0x2801, "SY", "SYR", "SYR",
			 "Syria", true, "\u0644.\u0633.\u200F", "SYP"));
		Add(new RegionName
			("BZ", 0x2809, "BZ", "BLZ", "BLZ",
			 "Belize", true, "BZ$", "BZD"));
		Add(new RegionName
			("PE", 0x280A, "PE", "PER", "PER",
			 "Peru", true, "S/.", "PEN"));
		Add(new RegionName
			("JO", 0x2C01, "JO", "JOR", "JOR",
			 "Jordan", true, "\u062F.\u0627.\u200F", "JOD"));
		Add(new RegionName
			("TT", 0x2C09, "TT", "TTO", "TTO",
			 "Trinidad and Tobago", true, "TT$", "TTD"));
		Add(new RegionName
			("AR", 0x2C0A, "AR", "ARG", "ARG",
			 "Argentina", true, "$", "ARS"));
		Add(new RegionName
			("LB", 0x3001, "LB", "LBN", "LBN",
			 "Lebanon", true, "\u0644.\u0644.\u200F", "LBP"));
		Add(new RegionName
			("ZW", 0x3009, "ZW", "ZWE", "ZWE",
			 "Zimbabwe", false, "Z$", "ZWD"));
		Add(new RegionName
			("EC", 0x300A, "EC", "ECU", "ECU",
			 "Ecuador", true, "$", "USD"));
		Add(new RegionName
			("KW", 0x3401, "KW", "KWT", "KWT",
			 "Kuwait", true, "\u062F.\u0643.\u200F", "KWD"));
		Add(new RegionName
			("PH", 0x3409, "PH", "PHL", "PHL",
			 "Republic of the Philippines", false, "Php", "PHP"));
		Add(new RegionName
			("CL", 0x340A, "CL", "CHL", "CHL",
			 "Chile", true, "$", "CLP"));
		Add(new RegionName
			("AE", 0x3801, "AE", "ARE", "ARE",
			 "U.A.E.", true, "\u062F.\u0625.\u200F", "AED"));
		Add(new RegionName
			("UY", 0x380A, "UY", "URY", "URY",
			 "Uruguay", true, "$U", "UYU"));
		Add(new RegionName
			("BH", 0x3C01, "BH", "BHR", "BHR",
			 "Bahrain", true, "\u062F.\u0628.\u200F", "BHD"));
		Add(new RegionName
			("PY", 0x3C0A, "PY", "PRY", "PRY",
			 "Paraguay", true, "Gs", "PYG"));
		Add(new RegionName
			("QA", 0x4001, "QA", "QAT", "QAT",
			 "Qatar", true, "\u0631.\u0642.\u200F", "QAR"));
		Add(new RegionName
			("BO", 0x400A, "BO", "BOL", "BOL",
			 "Bolivia", true, "$b", "BOB"));
		Add(new RegionName
			("SV", 0x440A, "SV", "SLV", "SLV",
			 "El Salvador", true, "$", "USD"));
		Add(new RegionName
			("HN", 0x480A, "HN", "HND", "HND",
			 "Honduras", true, "L.", "HNL"));
		Add(new RegionName
			("NI", 0x4C0A, "NI", "NIC", "NIC",
			 "Nicaragua", true, "C$", "NIO"));
		Add(new RegionName
			("PR", 0x500A, "PR", "PRI", "PRI",
			 "Puerto Rico", true, "$", "USD"));
	}

	// Create the region name table.
	public static void CreateNameTable()
			{
				lock(typeof(RegionNameTable))
				{
					// Return immediately if the name table already exists.
					if(regions != null)
					{
						return;
					}

					// Create a new region name table.
					regions = new RegionName [DefaultTableSize];
					numRegions = 0;

					// Populate the region name table.
					PopulateNameTable();
				}
			}

	// Get the name information for a specific region, by name.
	public static RegionName GetNameInfoByName(String name)
			{
				// Create the region name table.
				CreateNameTable();

				// Search for the name in the table.
				int posn = numRegions - 1;
				while(posn >= 0)
				{
					if(regions[posn].name == name)
					{
						return regions[posn];
					}
					--posn;
				}

				// Could not find the region.
				return null;
			}

	// Get the name information for a specific region, by identifier.
	public static RegionName GetNameInfoByID(int regionID)
			{
				// Create the region name table.
				CreateNameTable();

				// Search for the name in the table.
				int posn = numRegions - 1;
				while(posn >= 0)
				{
					if(regions[posn].regionID == regionID)
					{
						return regions[posn];
					}
					--posn;
				}

				// Could not find the region.
				return null;
			}

}; // class RegionNameTable

#endif // !ECMA_COMPAT

}; // namespace I18N.Common
