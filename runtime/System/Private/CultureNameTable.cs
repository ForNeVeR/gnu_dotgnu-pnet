/*
 * CultureNameTable.cs - Implementation of the
 *		"System.Private.CultureNameTable" class.
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

namespace System.Private
{

using System;
using System.Globalization;

internal sealed class CultureNameTable
{

	// Array of all registered culture names.
	private static CultureName[] cultures;
	private static int numCultures;

	// Useful constants.
	private const int DefaultTableSize = 64;
	private const int TableExtendSize  = 16;

	// Add an item to the culture name table.
	public static void Add(CultureName name)
			{
				if(numCultures < cultures.Length)
				{
					cultures[numCultures++] = name;
				}
				else
				{
					CultureName[] newCultures;
					newCultures = new CultureName
						[numCultures + TableExtendSize];
					Array.Copy(cultures, newCultures, cultures.Length);
					cultures = newCultures;
					cultures[numCultures++] = name;
				}
			}

	// Populate the culture name table.  Note: just because
	// a culture exists in this table doesn't mean that it is
	// actually supported by the rest of the system.
	public static void PopulateNameTable()
	{
		Add(new CultureName
			("iv",    0x007F, "iv", "IVL", "IVL",
			 "Invariant Language (Invariant Country)",
			 "Invariant Language (Invariant Country)"));
		Add(new CultureName
			("af",    0x0036, "af", "afr", "AFK", "Afrikaans",
			 "Afrikaans"));
		Add(new CultureName
			("af-ZA", 0x0436, "af", "afr", "AFK", "Afrikaans (South Africa)",
			 "Afrikaans (Suid Afrika)"));
		Add(new CultureName
			("sq",    0x001C, "sq", "sqi", "SQI", "Albanian",
			 "shqipe"));
		Add(new CultureName
			("sq-AL", 0x041C, "sq", "sqi", "SQI", "Albanian (Albania)",
			 "shqipe (Shqip\u00EBria)"));
		Add(new CultureName
			("ar",    0x0001, "ar", "ara", "ARA", "Arabic",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629"));
		Add(new CultureName
			("ar-DZ", 0x1401, "ar", "ara", "ARG", "Arabic (Algeria)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0627\u0644\u062C\u0632\u0627\u0626\u0631)"));
		Add(new CultureName
			("ar-BH", 0x3C01, "ar", "ara", "ARH", "Arabic (Bahrain)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0627\u0644\u0628\u062D\u0631\u064A\u0646)"));
		Add(new CultureName
			("ar-EG", 0x0C01, "ar", "ara", "ARE", "Arabic (Egypt)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0645\u0635\u0631)"));
		Add(new CultureName
			("ar-IQ", 0x0801, "ar", "ara", "ARI", "Arabic (Iraq)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0627\u0644\u0639\u0631\u0627\u0642)"));
		Add(new CultureName
			("ar-JO", 0x2C01, "ar", "ara", "ARJ", "Arabic (Jordan)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0627\u0644\u0623\u0631\u062F\u0646)"));
		Add(new CultureName
			("ar-KW", 0x3401, "ar", "ara", "ARK", "Arabic (Kuwait)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0627\u0644\u0643\u0648\u064A\u062A)"));
		Add(new CultureName
			("ar-LB", 0x3001, "ar", "ara", "ARB", "Arabic (Lebanon)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0644\u0628\u0646\u0627\u0646)"));
		Add(new CultureName
			("ar-LY", 0x1001, "ar", "ara", "ARL", "Arabic (Libya)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0644\u064A\u0628\u064A\u0627)"));
		Add(new CultureName
			("ar-MA", 0x1801, "ar", "ara", "ARM", "Arabic (Morocco)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0627\u0644\u0645\u0645\u0644\u0643\u0629 \u0627\u0644\u0645\u063A\u0631\u0628\u064A\u0629)"));
		Add(new CultureName
			("ar-OM", 0x2001, "ar", "ara", "ARO", "Arabic (Oman)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0639\u0645\u0627\u0646)"));
		Add(new CultureName
			("ar-QA", 0x4001, "ar", "ara", "ARQ", "Arabic (Qatar)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0642\u0637\u0631)"));
		Add(new CultureName
			("ar-SA", 0x0401, "ar", "ara", "ARA", "Arabic (Saudi Arabia)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0627\u0644\u0645\u0645\u0644\u0643\u0629 \u0627\u0644\u0639\u0631\u0628\u064A\u0629 \u0627\u0644\u0633\u0639\u0648\u062F\u064A\u0629)"));
		Add(new CultureName
			("ar-SY", 0x2801, "ar", "ara", "ARS", "Arabic (Syria)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0633\u0648\u0631\u064A\u0627)"));
		Add(new CultureName
			("ar-TN", 0x1C01, "ar", "ara", "ART", "Arabic (Tunisia)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u062A\u0648\u0646\u0633)"));
		Add(new CultureName
			("ar-AE", 0x3801, "ar", "ara", "ARU", "Arabic (U.A.E.)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0627\u0644\u0625\u0645\u0627\u0631\u0627\u062A \u0627\u0644\u0639\u0631\u0628\u064A\u0629 \u0627\u0644\u0645\u062A\u062D\u062F\u0629)"));
		Add(new CultureName
			("ar-YE", 0x3801, "ar", "ara", "ARY", "Arabic (Yemen)",
			 "\u0627\u0644\u0639\u0631\u0628\u064A\u0629 (\u0627\u0644\u064A\u0645\u0646)"));
		Add(new CultureName
			("hy",    0x002B, "hy", "hye", "HYE", "Armenian",
			 "\u0540\u0561\u0575\u0565\u0580\u0565\u0576"));
		Add(new CultureName
			("hy-AM", 0x042B, "hy", "hye", "HYE", "Armenian (Armenia)",
			 "\u0540\u0561\u0575\u0565\u0580\u0565\u0576 (\u0540\u0561\u0575\u0561\u057D\u057F\u0561\u0576)"));
		Add(new CultureName
			("az",    0x002C, "az", "aze", "AZE", "Azeri",
			 "Az\u0259rbaycan\u00AD\u0131l\u0131"));
		Add(new CultureName
			("Cy-az-AZ", 0x082C, "az", "aze", "AZE", "Azeri (Cyrillic) (Azerbaijan)",
			 "\u0410\u0437\u04D9\u0440\u0431\u0430\u0458\u04B9\u0430\u043D"));
		Add(new CultureName
			("Lt-az-AZ", 0x042C, "az", "aze", "AZE", "Azeri (Latin) (Azerbaijan)",
			 "Az\u0259rbaycan\u00AD\u0131l\u0131 (Az\u0259rbaycanca)"));
		Add(new CultureName
			("eu",    0x002D, "eu", "eus", "EUQ", "Basque",
			 "euskara"));
		Add(new CultureName
			("eu-ES", 0x042D, "eu", "eus", "EUQ", "Basque (Spain)",
			 "euskara (Espainia)"));
		Add(new CultureName
			("be",    0x002D, "be", "bel", "BEL", "Belarusian",
			 "\u0411\u0435\u043B\u0430\u0440\u0443\u0441\u043A\u0456"));
		Add(new CultureName
			("be-BY", 0x042D, "be", "bel", "BEL", "Belarusian (Belarus)",
			 "\u0411\u0435\u043B\u0430\u0440\u0443\u0441\u043A\u0456 (\u0411\u0435\u043B\u0430\u0440\u0443\u0441\u044C)"));
		Add(new CultureName
			("bg",    0x0002, "bg", "bul", "BGR", "Bulgarian",
			 "\u0431\u044A\u043B\u0433\u0430\u0440\u0441\u043A\u0438"));
		Add(new CultureName
			("bg-BG", 0x0402, "bg", "bul", "BGR", "Bulgarian (Bulgaria)",
			 "\u0431\u044A\u043B\u0433\u0430\u0440\u0441\u043A\u0438 (\u0411\u044A\u043B\u0433\u0430\u0440\u0438\u044F)"));
		Add(new CultureName
			("ca",    0x0003, "ca", "cat", "CAT", "Catalan",
			 "catal\u00E0"));
		Add(new CultureName
			("ca-ES", 0x0403, "ca", "cat", "CAT", "Catalan (Spain)",
			 "catal\u00E0 (Espanya)"));
		Add(new CultureName
			("zh-HK", 0x0C04, "zh", "zho", "ZHH", "Chinese (Hong Kong S.A.R.)",
			 "\u4E2D\u6587(\u7E41\u9AD4) (\u9999\u6E2F\u7279\u522B\u884C\u653F\u533A)"));
		Add(new CultureName
			("zh-MO", 0x1404, "zh", "zho", "ZHM", "Chinese (Macau S.A.R.)",
			 "\u4E2D\u6587(\u7B80\u4F53) (\u6FB3\u95E8\u7279\u522B\u884C\u653F\u533A)"));
		Add(new CultureName
			("zh-CN", 0x0804, "zh", "zho", "CHS", "Chinese (People's Republic of China)",
			 "\u4E2D\u6587(\u7B80\u4F53) (\u4E2D\u534E\u4EBA\u6C11\u5171\u548C\u56FD)"));
		Add(new CultureName
			("zh-CHS", 0x0004, "zh", "zho", "CHS", "Chinese (Simplified)",
			 "\u4E2D\u6587(\u7B80\u4F53)"));
		Add(new CultureName
			("zh-SG", 0x1004, "zh", "zho", "ZHI", "Chinese (Singapore)",
			 "\u4E2D\u6587(\u7B80\u4F53) (\u65B0\u52A0\u5761)"));
		Add(new CultureName
			("zh-TW", 0x0404, "zh", "zho", "CHT", "Chinese (Taiwan)",
			 "\u4E2D\u6587(\u7E41\u9AD4) (\u53F0\u7063)"));
		Add(new CultureName
			("zh-CHT", 0x7C04, "zh", "zho", "CHT", "Chinese (Traditional)",
			 "\u4E2D\u6587(\u7E41\u9AD4)"));
		Add(new CultureName
			("hr",    0x001A, "hr", "hrv", "HRV", "Croatian",
			 "hrvatski"));
		Add(new CultureName
			("hr-HR", 0x041A, "hr", "hrv", "HRV", "Croatian (Croatia)",
			 "hrvatski (Hrvatska)"));
		Add(new CultureName
			("cs",    0x0005, "cs", "ces", "CSY", "Czech",
			 "\u010De\u0161tina"));
		Add(new CultureName
			("cs-CZ", 0x0405, "cs", "ces", "CSY", "Czech (Czech Republic)",
			 "\u010De\u0161tina (\u010Cesk\u00E1\u00A0republika)"));
		Add(new CultureName
			("da",    0x0006, "da", "dan", "DAN", "Danish",
			 "dansk"));
		Add(new CultureName
			("da-DK", 0x0406, "da", "dan", "DAN", "Danish (Denmark)",
			 "dansk (Danmark)"));
		Add(new CultureName
			("div",   0x0065, "div", "div", "DIV", "Dhivehi",
			 "\u078B\u07A8\u0788\u07AC\u0780\u07A8\u0784\u07A6\u0790\u07B0"));
		Add(new CultureName
			("div-MV", 0x0465, "div", "div", "DIV", "Dhivehi (Maldives)",
			 "\u078B\u07A8\u0788\u07AC\u0780\u07A8\u0784\u07A6\u0790\u07B0 (\u078B\u07A8\u0788\u07AC\u0780\u07A8 \u0783\u07A7\u0787\u07B0\u0796\u07AC)"));
		Add(new CultureName
			("nl",    0x0013, "nl", "nld", "NLD", "Dutch",
			 "Nederlands"));
		Add(new CultureName
			("nl-BE", 0x0813, "nl", "nld", "NLB", "Dutch (Belgium)",
			 "Nederlands (Belgi\u00EB)"));
		Add(new CultureName
			("nl-NL", 0x0413, "nl", "nld", "NLD", "Dutch (Netherlands)",
			 "Nederlands (Nederland)"));
		Add(new CultureName
			("en",    0x0009, "en", "eng", "ENU", "English",
			 "English"));
		Add(new CultureName
			("en-AU", 0x0C09, "en", "eng", "ENA", "English (Australia)",
			 "English (Australia)"));
		Add(new CultureName
			("en-BZ", 0x2809, "en", "eng", "ENL", "English (Belize)",
			 "English (Belize)"));
		Add(new CultureName
			("en-CA", 0x1009, "en", "eng", "ENC", "English (Canada)",
			 "English (Canada)"));
		Add(new CultureName
			("en-CB", 0x2409, "en", "eng", "ENB", "English (Caribbean)",
			 "English (Caribbean)"));
		Add(new CultureName
			("en-IE", 0x1809, "en", "eng", "ENI", "English (Ireland)",
			 "English (Eire)"));
		Add(new CultureName
			("en-JM", 0x2009, "en", "eng", "ENJ", "English (Jamaica)",
			 "English (Jamaica)"));
		Add(new CultureName
			("en-NZ", 0x1409, "en", "eng", "ENZ", "English (New Zealand)",
			 "English (New Zealand)"));
		Add(new CultureName
			("en-PH", 0x3409, "en", "eng", "ENP", "English (Republic of the Philippines)",
			 "English (Philippines)"));
		Add(new CultureName
			("en-ZA", 0x1C09, "en", "eng", "ENS", "English (South Africa)",
			 "English (South Africa)"));
		Add(new CultureName
			("en-TT", 0x2C09, "en", "eng", "ENT", "English (Trinidad and Tobago)",
			 "English (Trinidad y Tobago)"));
		Add(new CultureName
			("en-GB", 0x0809, "en", "eng", "ENG", "English (United Kingdom)",
			 "English (United Kingdom)"));
		Add(new CultureName
			("en-US", 0x0409, "en", "eng", "ENU", "English (United States)",
			 "English (United States)"));
		Add(new CultureName
			("en-ZW", 0x3009, "en", "eng", "ENW", "English (Zimbabwe)",
			 "English (Zimbabwe)"));
		Add(new CultureName
			("et",    0x0025, "et", "est", "ETI", "Estonian",
			 "eesti"));
		Add(new CultureName
			("et-EE", 0x0425, "et", "est", "ETI", "Estonian (Estonia)",
			 "eesti (Eesti)"));
		Add(new CultureName
			("fo",    0x0038, "fo", "fao", "FOS", "Faeroese",
			 "f\u00F8royskt"));
		Add(new CultureName
			("fo-FO", 0x0438, "fo", "fao", "FOS", "Faeroese (Faeroe Islands)",
			 "f\u00F8royskt (F\u00F8royar)"));
		Add(new CultureName
			("fa",    0x0029, "fa", "fas", "FAR", "Farsi",
			 "\u0641\u0627\u0631\u0633\u0649"));
		Add(new CultureName
			("fa-IR", 0x0429, "fa", "fas", "FAR", "Farsi (Iran)",
			 "\u0641\u0627\u0631\u0633\u0649 (\u0627\u064A\u0631\u0627\u0646)"));
		Add(new CultureName
			("fi",    0x000B, "fi", "fin", "FIN", "Finnish",
			 "suomi"));
		Add(new CultureName
			("fi-FI", 0x040B, "fi", "fin", "FIN", "Finnish (Finland)",
			 "suomi (Suomi)"));
		Add(new CultureName
			("fr",    0x000C, "fr", "fra", "FRA", "French",
			 "fran\u00E7ais"));
		Add(new CultureName
			("fr-BE", 0x080C, "fr", "fra", "FRB", "French (Belgium)",
			 "fran\u00E7ais (Belgique)"));
		Add(new CultureName
			("fr-CA", 0x0C0C, "fr", "fra", "FRC", "French (Canada)",
			 "fran\u00E7ais (Canada)"));
		Add(new CultureName
			("fr-FR", 0x040C, "fr", "fra", "FRA", "French (France)",
			 "fran\u00E7ais (France)"));
		Add(new CultureName
			("fr-LU", 0x140C, "fr", "fra", "FRL", "French (Luxembourg)",
			 "fran\u00E7ais (Luxembourg)"));
		Add(new CultureName
			("fr-MC", 0x180C, "fr", "fra", "FRM", "French (Principality of Monaco)",
			 "fran\u00E7ais (Principaut\u00E9 de Monaco)"));
		Add(new CultureName
			("fr-CH", 0x100C, "fr", "fra", "FRS", "French (Switzerland)",
			 "fran\u00E7ais (Suisse)"));
		Add(new CultureName
			("gl",    0x0056, "gl", "glg", "GLC", "Galician",
			 "galego"));
		Add(new CultureName
			("gl-ES", 0x0456, "gl", "glg", "GLC", "Galician (Spain)",
			 "galego (Espa\u00F1a)"));
		Add(new CultureName
			("ka",    0x0037, "ka", "kat", "KAT", "Georgian",
			 "\u10E5\u10D0\u10E0\u10D7\u10E3\u10DA\u10D8"));
		Add(new CultureName
			("ka-GE", 0x0437, "ka", "kat", "KAT", "Georgian (Georgia)",
			 "\u10E5\u10D0\u10E0\u10D7\u10E3\u10DA\u10D8 (\u10E1\u10D0\u10E5\u10D0\u10E0\u10D7\u10D5\u10D4\u10DA\u10DD)"));
		Add(new CultureName
			("de",    0x0007, "de", "deu", "DEU", "German",
			 "Deutsch"));
		Add(new CultureName
			("de-AT", 0x0C07, "de", "deu", "DEA", "German (Austria)",
			 "Deutsch (\u00D6sterreich)"));
		Add(new CultureName
			("de-DE", 0x0407, "de", "deu", "DEU", "German (Germany)",
			 "Deutsch (Deutschland)"));
		Add(new CultureName
			("de-LI", 0x1407, "de", "deu", "DEC", "German (Liechtenstein)",
			 "Deutsch (Liechtenstein)"));
		Add(new CultureName
			("de-LU", 0x1007, "de", "deu", "DEL", "German (Luxembourg)",
			 "Deutsch (Luxemburg)"));
		Add(new CultureName
			("de-CH", 0x0807, "de", "deu", "DES", "German (Switzerland)",
			 "Deutsch (Schweiz)"));
		Add(new CultureName
			("el",    0x0008, "el", "ell", "ELL", "Greek",
			 "\u03B5\u03BB\u03BB\u03B7\u03BD\u03B9\u03BA\u03AC"));
		Add(new CultureName
			("el-GR", 0x0408, "el", "ell", "ELL", "Greek (Greece)",
			 "\u03B5\u03BB\u03BB\u03B7\u03BD\u03B9\u03BA\u03AC (\u0395\u03BB\u03BB\u03AC\u03B4\u03B1)"));
		Add(new CultureName
			("gu",    0x0047, "gu", "guj", "GUJ", "Gujarati",
			 "\u0A97\u0AC1\u0A9C\u0AB0\u0ABE\u0AA4\u0AC0"));
		Add(new CultureName
			("gu-IN", 0x0447, "gu", "guj", "GUJ", "Gujarati (India)",
			 "\u0A97\u0AC1\u0A9C\u0AB0\u0ABE\u0AA4\u0AC0 (\u0AAD\u0ABE\u0AB0\u0AA4)"));
		Add(new CultureName
			("he",    0x000D, "he", "heb", "HEB", "Hebrew",
			 "\u05E2\u05D1\u05E8\u05D9\u05EA"));
		Add(new CultureName
			("he-IL", 0x040D, "he", "heb", "HEB", "Hebrew (Israel)",
			 "\u05E2\u05D1\u05E8\u05D9\u05EA (\u05D9\u05E9\u05E8\u05D0\u05DC)"));
		Add(new CultureName
			("hi",    0x0039, "hi", "hin", "HIN", "Hindi",
			 "\u0939\u093F\u0902\u0926\u0940"));
		Add(new CultureName
			("hi-IN", 0x0439, "hi", "hin", "HIN", "Hindi (India)",
			 "\u0939\u093F\u0902\u0926\u0940 (\u092D\u093E\u0930\u0924)"));
		Add(new CultureName
			("hu",    0x000E, "hu", "hun", "HUN", "Hungarian",
			 "Magyar"));
		Add(new CultureName
			("hu-HU", 0x040E, "hu", "hun", "HUN", "Hungarian (Hungary)",
			 "Magyar (Magyarorsz\u00E1g)"));
		Add(new CultureName
			("is",    0x000F, "is", "isl", "ISL", "Icelandic",
			 "\u00EDslenska"));
		Add(new CultureName
			("is-IS", 0x040F, "is", "isl", "ISL", "Icelandic (Iceland)",
			 "\u00EDslenska (\u00CDsland)"));
		Add(new CultureName
			("id",    0x0021, "id", "ind", "IND", "Indonesian",
			 "Bahasa Indonesia"));
		Add(new CultureName
			("id-ID", 0x0421, "id", "ind", "IND", "Indonesian (Indonesia)",
			 "Bahasa Indonesia (Indonesia)"));
		Add(new CultureName
			("it",    0x0010, "it", "ita", "ITA", "Italian",
			 "italiano"));
		Add(new CultureName
			("it-IT", 0x0410, "it", "ita", "ITA", "Italian (Italy)",
			 "italiano (Italia)"));
		Add(new CultureName
			("it-CH", 0x0810, "it", "ita", "ITS", "Italian (Switzerland)",
			 "italiano (Svizzera)"));
		Add(new CultureName
			("ja",    0x0011, "ja", "jpn", "JPN", "Japanese",
			 "\u65E5\u672C\u8A9E"));
		Add(new CultureName
			("ja-JP", 0x0411, "ja", "jpn", "JPN", "Japanese (Japan)",
			 "\u65E5\u672C\u8A9E (\u65E5\u672C)"));
		Add(new CultureName
			("kn",    0x004B, "kn", "kan", "KAN", "Kannada",
			 "\u0C95\u0CA8\u0CCD\u0CA8\u0CA1"));
		Add(new CultureName
			("kn-IN", 0x044B, "kn", "kan", "KAN", "Kannada (India)",
			 "\u0C95\u0CA8\u0CCD\u0CA8\u0CA1 (\u0CAD\u0CBE\u0CB0\u0CA4)"));
		Add(new CultureName
			("kk",    0x003F, "kk", "kaz", "KKZ", "Kazakh",
			 "\u049A\u0430\u0437\u0430\u0449b"));
		Add(new CultureName
			("kk-KZ", 0x043F, "kk", "kaz", "KKZ", "Kazakh (Kazakhstan)",
			 "\u049A\u0430\u0437\u0430\u049B (\u049A\u0430\u0437\u0430\u049B\u0441\u0442\u0430\u043D)"));
		Add(new CultureName
			("kok",   0x0057, "kok", "kok", "KNK", "Konkani",
			 "\u0915\u094B\u0902\u0915\u0923\u0940"));
		Add(new CultureName
			("kok-IN", 0x0457, "kok", "kok", "KNK", "Konkani (India)",
			 "\u0915\u094B\u0902\u0915\u0923\u0940 (\u092D\u093E\u0930\u0924)"));
		Add(new CultureName
			("ko",    0x0012, "ko", "kor", "KOR", "Korean",
			 "\uD55C\uAD6D\uC5B4"));
		Add(new CultureName
			("ko-KR", 0x0412, "ko", "kor", "KOR", "Korean (Korea)",
			 "\uD55C\uAD6D\uC5B4 (\uB300\uD55C\uBBFC\uAD6D)"));
		Add(new CultureName
			("ky",    0x0040, "ky", "kir", "KYR", "Kyrgyz",
			 "\u041A\u044B\u0440\u0433\u044B\u0437"));
		Add(new CultureName
			("ky-KZ", 0x0440, "ky", "kir", "KYR", "Kyrgyz (Kyrgyzstan)",
			 "\u041A\u044B\u0440\u0433\u044B\u0437 (\u041A\u044B\u0440\u0433\u044B\u0437\u0441\u0442\u0430\u043D)"));
		Add(new CultureName
			("lv",    0x0026, "lv", "lav", "LVI", "Latvian",
			 "latvie\u0161u"));
		Add(new CultureName
			("lv-LV", 0x0426, "lv", "lav", "LVI", "Latvian (Latvia)",
			 "latvie\u0161u (Latvija)"));
		Add(new CultureName
			("lt",    0x0027, "lt", "lit", "LTH", "Lithuanian",
			 "lietuvi\u0173"));
		Add(new CultureName
			("lt-LT", 0x0427, "lt", "lit", "LTH", "Lithuanian (Lithuania)",
			 "lietuvi\u0173 (Lietuva)"));
		Add(new CultureName
			("mk",    0x002F, "mk", "mkd", "MKI", "FYRO Macedonian",
			 "\u043C\u0430\u043A\u0435\u0434\u043E\u043D\u0441\u043A\u0438 \u0458\u0430\u0437\u0438\u043A"));
		Add(new CultureName
			("mk-MK", 0x042F, "mk", "mkd", "MKI", "FYRO Macedonian (Former Yugoslav Republic of Macedonia)",
			 "\u043C\u0430\u043A\u0435\u0434\u043E\u043D\u0441\u043A\u0438 \u0458\u0430\u0437\u0438\u043A (\u041C\u0430\u043A\u0435\u0434\u043E\u043D\u0438\u0458\u0430)"));
		Add(new CultureName
			("ms",    0x003E, "ms", "msa", "MSL", "Malay",
			 "Bahasa Malaysia"));
		Add(new CultureName
			("ms-BN", 0x083E, "ms", "msa", "MSB", "Malay (Brunei Darussalam)",
			 "Bahasa Malaysia (Brunei Darussalam)"));
		Add(new CultureName
			("ms-MY", 0x043E, "ms", "msa", "MSL", "Malay (Malaysia)",
			 "Bahasa Malaysia (Malaysia)"));
		Add(new CultureName
			("mr",    0x004E, "mr", "mar", "MAR", "Marathi",
			 "\u092E\u0930\u093E\u0920\u0940"));
		Add(new CultureName
			("mr-IN", 0x044E, "mr", "mar", "MAR", "Marathi (India)",
			 "\u092E\u0930\u093E\u0920\u0940 (\u092D\u093E\u0930\u0924)"));
		Add(new CultureName
			("mn",    0x0050, "mn", "mon", "MON", "Mongolian",
			 "\u041C\u043E\u043D\u0433\u043E\u043B\u00A0\u0445\u044D\u043B"));
		Add(new CultureName
			("mn-MN", 0x0450, "mn", "mon", "MON", "Mongolian (Mongolia)",
			 "\u041C\u043E\u043D\u0433\u043E\u043B\u00A0\u0445\u044D\u043B (\u041C\u043E\u043D\u0433\u043E\u043B\u00A0\u0443\u043B\u0441)"));
		Add(new CultureName
			("no",    0x0014, "no", "nor", "NOR", "Norwegian",
			 "norsk"));
		Add(new CultureName
			("nb-NO", 0x0414, "nb", "nob", "NOR", "Norwegian (Bokm\u00E5l) (Norway)",
			 "norsk (bokm\u00E5l) (Norge)"));
		Add(new CultureName
			("nn-NO", 0x0814, "nn", "nno", "NON", "Norwegian (Nynorsk) (Norway)",
			 "norsk (nynorsk) (Noreg)"));
		Add(new CultureName
			("pl",    0x0015, "pl", "pol", "PLK", "Polish",
			 "polski"));
		Add(new CultureName
			("pl-PL", 0x0415, "pl", "pol", "PLK", "Polish (Poland)",
			 "polski (Polska)"));
		Add(new CultureName
			("pt",    0x0016, "pt", "por", "PTB", "Portuguese",
			 "Portugu\u00EAs"));
		Add(new CultureName
			("pt-BR", 0x0416, "pt", "por", "PTB", "Portuguese (Brazil)",
			 "Portugu\u00EAs (Brasil)"));
		Add(new CultureName
			("pt-PT", 0x0816, "pt", "por", "PTG", "Portuguese (Portugal)",
			 "portugu\u00EAs (Portugal)"));
		Add(new CultureName
			("pa",    0x0046, "pa", "pan", "PAN", "Punjabi",
			 "\u0A2A\u0A70\u0A1C\u0A3E\u0A2C\u0A40"));
		Add(new CultureName
			("pa-IN", 0x0446, "pa", "pan", "PAN", "Punjabi (India)",
			 "\u0A2A\u0A70\u0A1C\u0A3E\u0A2C\u0A40 (\u0A2D\u0A3E\u0A30\u0A24)"));
		Add(new CultureName
			("ro",    0x0018, "ro", "ron", "ROM", "Romanian",
			 "rom\u00E2n\u0103"));
		Add(new CultureName
			("ro-RO", 0x0418, "ro", "ron", "ROM", "Romanian (Romania)",
			 "rom\u00E2n\u0103 (Rom\u00E2nia)"));
		Add(new CultureName
			("ru",    0x0019, "ru", "rus", "RUS", "Russian",
			 "\u0440\u0443\u0441\u0441\u043A\u0438\u0439"));
		Add(new CultureName
			("ru-RU", 0x0419, "ru", "rus", "RUS", "Russian (Russia)",
			 "\u0440\u0443\u0441\u0441\u043A\u0438\u0439 (\u0420\u043E\u0441\u0441\u0438\u044F)"));
		Add(new CultureName
			("sa",    0x004F, "sa", "san", "SAN", "Sanskrit",
			 "\u0938\u0902\u0938\u094D\u0915\u0943\u0924"));
		Add(new CultureName
			("sa-IN", 0x044F, "sa", "san", "SAN", "Sanskrit (India)",
			 "\u0938\u0902\u0938\u094D\u0915\u0943\u0924 (\u092D\u093E\u0930\u0924\u092E\u094D)"));
		Add(new CultureName
			("Cy-sr-SP", 0x0C1A, "sr", "srp", "SRB", "Serbian (Cyrillic) (Serbia)",
			 "\u0441\u0440\u043F\u0441\u043A\u0438 (\u0408\u0443\u0433\u043E\u0441\u043B\u0430\u0432\u0438\u0458\u0430)"));
		Add(new CultureName
			("Lt-sr-SP", 0x081A, "sr", "srp", "SRL", "Serbian (Latin) (Serbia)",
			 "srpski (Srbija)"));
		Add(new CultureName
			("sk",    0x001B, "sk", "slk", "SKY", "Slovak",
			 "sloven\u010Dina"));
		Add(new CultureName
			("sk-SK", 0x041B, "sk", "slk", "SKY", "Slovak (Slovakia)",
			 "sloven\u010Dina (Slovensk\u00E1 republika)"));
		Add(new CultureName
			("sl",    0x0024, "sl", "slv", "SLV", "Slovenian",
			 "slovenski"));
		Add(new CultureName
			("sl-SI", 0x0424, "sl", "slv", "SLV", "Slovenian (Slovenia)",
			 "slovenski (Slovenija)"));
		Add(new CultureName
			("es",    0x000A, "es", "spa", "ESP", "Spanish",
			 "espa\u00F1ol"));
		Add(new CultureName
			("es-AR", 0x2C0A, "es", "spa", "ESS", "Spanish (Argentina)",
			 "Espa\u00F1ol (Argentina)"));
		Add(new CultureName
			("es-BO", 0x400A, "es", "spa", "ESB", "Spanish (Bolivia)",
			 "Espa\u00F1ol (Bolivia)"));
		Add(new CultureName
			("es-CL", 0x340A, "es", "spa", "ESL", "Spanish (Chile)",
			 "Espa\u00F1ol (Chile)"));
		Add(new CultureName
			("es-CO", 0x240A, "es", "spa", "ESO", "Spanish (Colombia)",
			 "Espa\u00F1ol (Colombia)"));
		Add(new CultureName
			("es-CR", 0x140A, "es", "spa", "ESC", "Spanish (Costa Rica)",
			 "Espa\u00F1ol (Costa Rica)"));
		Add(new CultureName
			("es-DO", 0x1C0A, "es", "spa", "ESD", "Spanish (Dominican Republic)",
			 "Espa\u00F1ol (Rep\u00FAblica Dominicana)"));
		Add(new CultureName
			("es-EC", 0x300A, "es", "spa", "ESF", "Spanish (Ecuador)",
			 "Espa\u00F1ol (Ecuador)"));
		Add(new CultureName
			("es-SV", 0x440A, "es", "spa", "ESE", "Spanish (El Salvador)",
			 "Espa\u00F1ol (El Salvador)"));
		Add(new CultureName
			("es-GT", 0x100A, "es", "spa", "ESG", "Spanish (Guatemala)",
			 "Espa\u00F1ol (Guatemala)"));
		Add(new CultureName
			("es-HN", 0x480A, "es", "spa", "ESH", "Spanish (Honduras)",
			 "Espa\u00F1ol (Honduras)"));
		Add(new CultureName
			("es-MX", 0x080A, "es", "spa", "ESM", "Spanish (Mexico)",
			 "Espa\u00F1ol (M\u00E9xico)"));
		Add(new CultureName
			("es-NI", 0x4C0A, "es", "spa", "ESI", "Spanish (Nicaragua)",
			 "Espa\u00F1ol (Nicaragua)"));
		Add(new CultureName
			("es-PA", 0x180A, "es", "spa", "ESA", "Spanish (Panama)",
			 "Espa\u00F1ol (Panam\u00E1)"));
		Add(new CultureName
			("es-PY", 0x3C0A, "es", "spa", "ESZ", "Spanish (Paraguay)",
			 "Espa\u00F1ol (Paraguay)"));
		Add(new CultureName
			("es-PE", 0x280A, "es", "spa", "ESR", "Spanish (Peru)",
			 "Espa\u00F1ol (Per\u00FA)"));
		Add(new CultureName
			("es-PR", 0x500A, "es", "spa", "ESU", "Spanish (Puerto Rico)",
			 "Espa\u00F1ol (Puerto Rico)"));
		Add(new CultureName
			("es-ES", 0x0C0A, "es", "spa", "ESN", "Spanish (Spain)",
			 "espa\u00F1ol (Espa\u00F1a)"));
		Add(new CultureName
			("es-UY", 0x380A, "es", "spa", "ESY", "Spanish (Uruguay)",
			 "Espa\u00F1ol (Uruguay)"));
		Add(new CultureName
			("es-VE", 0x200A, "es", "spa", "ESV", "Spanish (Venezuela)",
			 "Espa\u00F1ol (Republica Bolivariana de Venezuela)"));
		Add(new CultureName
			("sw",    0x0041, "sw", "swa", "SWK", "Swahili",
			 "Kiswahili"));
		Add(new CultureName
			("sw-KE", 0x0441, "sw", "swa", "SWK", "Swahili (Kenya)",
			 "Kiswahili (Kenya)"));
		Add(new CultureName
			("sv",    0x001D, "sv", "swe", "SVE", "Swedish",
			 "svenska"));
		Add(new CultureName
			("sv-FI", 0x081D, "sv", "swe", "SVF", "Swedish (Finland)",
			 "svenska (Finland)"));
		Add(new CultureName
			("sv-SE", 0x041D, "sv", "swe", "SVE", "Swedish (Sweden)",
			 "svenska (Sverige)"));
		Add(new CultureName
			("syr",   0x005A, "syr", "syr", "SYR", "Syriac",
			 "\u0723\u0718\u072A\u071D\u071D\u0710"));
		Add(new CultureName
			("syr-SY", 0x045A, "syr", "syr", "SYR", "Syriac (Syria)",
			 "\u0723\u0718\u072A\u071D\u071D\u0710 (\u0633\u0648\u0631\u064A\u0627)"));
		Add(new CultureName
			("ta",    0x0049, "ta", "tam", "TAM", "Tamil",
			 "\u0BA4\u0BAE\u0BBF\u0BB4\u0BCD"));
		Add(new CultureName
			("ta-IN", 0x0449, "ta", "tam", "TAM", "Tamil (India)",
			 "\u0BA4\u0BAE\u0BBF\u0BB4\u0BCD (\u0B87\u0BA8\u0BCD\u0BA4\u0BBF\u0BAF\u0BBE)"));
		Add(new CultureName
			("tt",    0x0044, "tt", "tat", "TTT", "Tatar",
			 "\u0422\u0430\u0442\u0430\u0440"));
		Add(new CultureName
			("tt-TA", 0x0444, "tt", "tat", "TTT", "Tatar (Tatarstan)",
			 "\u0422\u0430\u0442\u0430\u0440 (\u0422\u0430\u0442\u0430\u0440\u0441\u0442\u0430\u043D)"));
		Add(new CultureName
			("te",    0x004A, "te", "tel", "TEL", "Telugu",
			 "\u0C24\u0C46\u0C32\u0C41\u0C17\u0C41"));
		Add(new CultureName
			("te-IN", 0x044A, "te", "tel", "TEL", "Telugu (India)",
			 "\u0C24\u0C46\u0C32\u0C41\u0C17\u0C41 (\u0C2D\u0C3E\u0C30\u0C24\u00A0\u0C26\u0C47\u0C36\u0C02)"));
		Add(new CultureName
			("th",    0x001E, "th", "tha", "THA", "Thai",
			 "\u0E44\u0E17\u0E22"));
		Add(new CultureName
			("th-TH", 0x041E, "th", "tha", "THA", "Thai (Thailand)",
			 "\u0E44\u0E17\u0E22 (\u0E44\u0E17\u0E22)"));
		Add(new CultureName
			("tr",    0x001F, "tr", "tur", "TRK", "Turkish",
			 "T\u00FCrk\u00E7e"));
		Add(new CultureName
			("tr-TR", 0x041F, "tr", "tur", "TRK", "Turkish (Turkey)",
			 "T\u00FCrk\u00E7e (T\u00FCrkiye)"));
		Add(new CultureName
			("uk",    0x0022, "uk", "ukr", "UKR", "Ukrainian",
			 "\u0443\u043A\u0440\u0430\u0457\u043D\u044C\u0441\u043A\u0430"));
		Add(new CultureName
			("uk-UA", 0x0422, "uk", "ukr", "UKR", "Ukrainian (Ukraine)",
			 "\u0443\u043A\u0440\u0430\u0457\u043D\u044C\u0441\u043A\u0430 (\u0423\u043A\u0440\u0430\u0457\u043D\u0430)"));
		Add(new CultureName
			("ur",    0x0020, "ur", "urd", "URD", "Urdu",
			 "\u0671\u0631\u062F\u0648"));
		Add(new CultureName
			("ur-PK", 0x0420, "ur", "urd", "URD", "Urdu (Islamic Republic of Pakistan)",
			 "\u0627\u064F\u0631\u062F\u0648 (\u067E\u0627\u06A9\u0633\u062A\u0627\u0646)"));
		Add(new CultureName
			("uz",    0x0043, "uz", "uzb", "UZB", "Uzbek",
			 "U'zbek"));
		Add(new CultureName
			("Cy-uz-UZ", 0x0843, "uz", "uzb", "UZB", "Uzbek (Cyrillic) (Uzbekistan)",
			 "\u040E\u0437\u0431\u0435\u043A (\u040E\u0437\u0431\u0435\u043A\u0438\u0441\u0442\u043E\u043D)"));
		Add(new CultureName
			("Lt-uz-UZ", 0x0443, "uz", "uzb", "UZB", "Uzbek (Latin) (Uzbekistan)",
			 "U'zbek (U'zbekiston Respublikasi)"));
		Add(new CultureName
			("vi",    0x002A, "vi", "vie", "VIT", "Vietnamese",
			 "Ti\u00EA\u0301ng Vi\u00EA\u0323t Nam"));
		Add(new CultureName
			("vi-VN", 0x042A, "vi", "vie", "VIT", "Vietnamese (Viet Nam)",
			 "Ti\u00EA\u0301ng Vi\u00EA\u0323t Nam (Vi\u00EA\u0323t Nam)"));
	}

	// Create the culture name table.
	public static void CreateNameTable()
			{
				lock(typeof(CultureNameTable))
				{
					// Return immediately if the name table already exists.
					if(cultures != null)
					{
						return;
					}

					// Create a new culture name table.
					cultures = new CultureName [DefaultTableSize];
					numCultures = 0;

					// Populate the culture name table.
					PopulateNameTable();
				}
			}

	// Get the name information for a specific culture, by name.
	public static CultureName GetNameInfoByName(String name, bool throwOnError)
			{
				// Create the culture name table.
				CreateNameTable();

				// Search for the name in the table.
				int posn = numCultures - 1;
				while(posn >= 0)
				{
					if(cultures[posn].name == name)
					{
						return cultures[posn];
					}
					--posn;
				}

				// Could not find the culture.
				if(throwOnError)
				{
					throw new ArgumentException(_("Arg_InvalidCulture"));
				}
				return null;
			}

	// Get the name information for a specific culture, by identifier.
	public static CultureName GetNameInfoByID(int cultureID, bool throwOnError)
			{
				// Create the culture name table.
				CreateNameTable();

				// Search for the name in the table.
				int posn = numCultures - 1;
				while(posn >= 0)
				{
					if(cultures[posn].cultureID == cultureID)
					{
						return cultures[posn];
					}
					--posn;
				}

				// Could not find the culture.
				if(throwOnError)
				{
					throw new ArgumentException(_("Arg_InvalidCulture"));
				}
				return null;
			}

}; // class CultureNameTable

}; // namespace System.Private
