#!/bin/sh
#
# This script can be used to regenerate the CPnnnn.cs files
# from the ".ucm" files.  The list of unsupported code pages
# is at the end of this file.
#
# Copyright (C) 2002  Southern Storm Software, Pty Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

if gcc -o ucm2cp ucm2cp.c ; then
	:
else
	exit 1
fi

./ucm2cp --page 37 --wpage 1252 --name 'IBM EBCDIC (US-Canada)' \
	--webname IBM037 --bodyname IBM037 \
	--headername IBM037 --no-browser-display \
	--no-browser-save --no-mailnews-display \
	--no-mailnews-save ibm-37.ucm >CP37.cs

./ucm2cp --page 437 --wpage 1252 --name 'OEM United States' \
	--webname IBM437 --bodyname IBM437 \
	--headername IBM437 --no-browser-display \
	--no-browser-save --no-mailnews-display \
	--no-mailnews-save ibm-437.ucm >CP437.cs

./ucm2cp --page 500 --wpage 1252 --name 'IBM EBCDIC (International)' \
	--webname IBM500 --bodyname IBM500 \
	--headername IBM500 --no-browser-display \
	--no-browser-save --no-mailnews-display \
	--no-mailnews-save ibm-500.ucm >CP500.cs

./ucm2cp --page 850 --wpage 1252 --name 'Western European (DOS)' \
	--webname ibm850 --bodyname ibm850 \
	--headername ibm850 --no-browser-display \
	--no-browser-save --no-mailnews-display \
	--no-mailnews-save ibm-850.ucm >CP850.cs

./ucm2cp --page 860 --wpage 1252 --name 'Portuguese (DOS)' \
	--webname ibm860 --bodyname ibm860 \
	--headername ibm860 --no-browser-display \
	--no-browser-save --no-mailnews-display \
	--no-mailnews-save ibm-860.ucm >CP860.cs

./ucm2cp --page 861 --wpage 1252 --name 'Icelandic (DOS)' \
	--webname ibm861 --bodyname ibm861 \
	--headername ibm861 --no-browser-display \
	--no-browser-save --no-mailnews-display \
	--no-mailnews-save ibm-861.ucm >CP861.cs

./ucm2cp --page 863 --wpage 1252 --name 'French Canadian (DOS)' \
	--webname IBM863 --bodyname IBM863 \
	--headername IBM863 --no-browser-display \
	--no-browser-save --no-mailnews-display \
	--no-mailnews-save ibm-863.ucm >CP863.cs

./ucm2cp --page 865 --wpage 1252 --name 'Nordic (DOS)' \
	--webname IBM865 --bodyname IBM863 \
	--headername IBM865 --no-browser-display \
	--no-browser-save --no-mailnews-display \
	--no-mailnews-save ibm-865.ucm >CP865.cs

./ucm2cp --page 874 --wpage 874 --name 'Thai (Windows)' \
	--webname windows-874 --bodyname windows-874 \
	--headername windows-874 ibm-874.ucm >CP874.cs

./ucm2cp --page 1250 --wpage 1250 --name 'Central European (Windows)' \
	--webname windows-1250 --bodyname iso-8859-2 \
	--headername windows-1250 ibm-5346.ucm >CP1250.cs

./ucm2cp --page 1251 --wpage 1251 --name 'Cyrillic (Windows)' \
	--webname windows-1251 --bodyname koi8-r \
	--headername windows-1251 ibm-5347.ucm >CP1251.cs

./ucm2cp --page 1252 --wpage 1252 --name 'Western European (Windows)' \
	--webname Windows-1252 --bodyname iso-8859-1 \
	--headername Windows-1252 ibm-5348.ucm >CP1252.cs

./ucm2cp --page 1253 --wpage 1253 --name 'Greek (Windows)' \
	--webname windows-1253 --bodyname iso-8859-7 \
	--headername windows-1253 ibm-5349.ucm >CP1253.cs

./ucm2cp --page 1254 --wpage 1254 --name 'Turkish (Windows)' \
	--webname windows-1254 --bodyname iso-8859-9 \
	--headername windows-1254 ibm-5350.ucm >CP1254.cs

./ucm2cp --page 1255 --wpage 1255 --name 'Hebrew (Windows)' \
	--webname windows-1255 --bodyname windows-1255 \
	--headername windows-1255 ibm-5351.ucm >CP1255.cs

./ucm2cp --page 1256 --wpage 1256 --name 'Arabic (Windows)' \
	--webname windows-1256 --bodyname windows-1256 \
	--headername windows-1256 ibm-5352.ucm >CP1256.cs

./ucm2cp --page 1257 --wpage 1257 --name 'Baltic (Windows)' \
	--webname windows-1257 --bodyname iso-8859-4 \
	--headername windows-1257 ibm-5353.ucm >CP1257.cs

./ucm2cp --page 1258 --wpage 1258 --name 'Vietnamese (Windows)' \
	--webname windows-1258 --bodyname windows-1258 \
	--headername windows-1258 ibm-5354.ucm >CP1258.cs

./ucm2cp --page 10000 --wpage 1252 --name 'Western European (Mac)' \
	--webname macintosh --bodyname macintosh \
	--headername macintosh --no-browser-display \
	--no-browser-save --no-mailnews-display \
	--no-mailnews-save ibm-1275.ucm >CP10000.cs

./ucm2cp --page 10079 --wpage 1252 --name 'Icelandic (Mac)' \
	--webname x-mac-icelandic --bodyname x-mac-icelandic \
	--headername x-mac-icelandic --no-browser-display \
	--no-browser-save --no-mailnews-display \
	--no-mailnews-save mac-is.ucm >CP10079.cs

./ucm2cp --page 20866 --wpage 1251 --name 'Cyrillic (KOI8-R)' \
	--webname koi8-r --bodyname koi8-r \
	--headername koi8-r ibm-878.ucm >CP20866.cs

./ucm2cp --page 28592 --wpage 1250 --name 'Central European (ISO)' \
	--webname iso-8859-2 --bodyname iso-8859-2 \
	--headername iso-8859-2 ibm-912.ucm >CP28592.cs

./ucm2cp --page 28605 --wpage 1252 --name 'Latin 9 (ISO)' \
	--webname iso-8859-15 --bodyname iso-8859-15 \
	--headername iso-8859-15 --no-browser-display \
	ibm-923.ucm >CP28605.cs

exit 0

# Windows code pages that are handled internally by "Encoding":
#
#  1200  Unicode
#  1201  Unicode (Big-Endian)
#  20127 US-ASCII
#  28591 Western European (ISO)
#  65000 Unicode (UTF-7)
#  65001 Unicode (UTF-8)
#
# Other Windows code pages that aren't done yet:
#
#  932   Japanese (Shift-JIS)
#  936   Chinese Simplified (GB2312)
#  947   Korean
#  950   Chinese Traditional (Big5)
#  20261 T.61
#  50220 Japanese (JIS)
#  50221 Japanese (JIS-Allow 1 byte Kana)
#  50222 Japanese (JIS-Allow 1 byte Kana - SO/SI)
#  50225 Korean (ISO)
#  50227 ISO-2022 Simplified Chinese
#  50229 ISO-2022 Traditional Chinese
#  51932 Japanese (EUC)
#  51936 Chinese Simplified (EUC)
#  52936 Chinese Simplified (HZ)
#  54936 GB 18030-2000 Simplified Chinese
#  57002 ISCII Devanagari
#  57003 ISCII Bengali
#  57004 ISCII Tamil
#  57005 ISCII Telugu
#  57006 ISCII Assamese
#  57007 ISCII Oriya
#  57008 ISCII Kannada
#  57009 ISCII Malayalam
#  57010 ISCII Gujarati
#  57011 ISCII Punjabi
