/*
 * ModeField.cs - Implementation of the
 *			"System.Drawing.Printing.ModeField" class.
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

namespace System.Drawing.Printing
{

public enum ModeField
{
	Orientation		= 0,
	PaperSize		= 1,
	PaperLength		= 2,
	PaperWidth		= 3,
	Copies			= 4,
	DefaultSource	= 5,
	PrintQuality	= 6,
	Color			= 7,
	Duplex			= 8,
	YResolution		= 9,
	TTOption		= 10,
	Collate			= 11

}; // enum ModeField

}; // namespace System.Drawing.Printing
