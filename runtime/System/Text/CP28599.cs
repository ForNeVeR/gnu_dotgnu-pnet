/*
 * CP28599.cs - Latin 5 (ISO) code page.
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

// Generated from "ibm-920.ucm".

namespace System.Text
{

using System;

internal class CP28599 : ByteEncoding
{
	public CP28599()
		: base(28599, ToChars, "Latin 5 (ISO)",
		       "iso-8859-9", "iso-8859-9", "iso-8859-9",
		       true, true, true, true, 1254)
	{}

	private static readonly char[] ToChars = {
		'\u0000', '\u0001', '\u0002', '\u0003', '\u0004', '\u0005', 
		'\u0006', '\u0007', '\u0008', '\u0009', '\u000A', '\u000B', 
		'\u000C', '\u000D', '\u000E', '\u000F', '\u0010', '\u0011', 
		'\u0012', '\u0013', '\u0014', '\u0015', '\u0016', '\u0017', 
		'\u0018', '\u0019', '\u001A', '\u001B', '\u001C', '\u001D', 
		'\u001E', '\u001F', '\u0020', '\u0021', '\u0022', '\u0023', 
		'\u0024', '\u0025', '\u0026', '\u0027', '\u0028', '\u0029', 
		'\u002A', '\u002B', '\u002C', '\u002D', '\u002E', '\u002F', 
		'\u0030', '\u0031', '\u0032', '\u0033', '\u0034', '\u0035', 
		'\u0036', '\u0037', '\u0038', '\u0039', '\u003A', '\u003B', 
		'\u003C', '\u003D', '\u003E', '\u003F', '\u0040', '\u0041', 
		'\u0042', '\u0043', '\u0044', '\u0045', '\u0046', '\u0047', 
		'\u0048', '\u0049', '\u004A', '\u004B', '\u004C', '\u004D', 
		'\u004E', '\u004F', '\u0050', '\u0051', '\u0052', '\u0053', 
		'\u0054', '\u0055', '\u0056', '\u0057', '\u0058', '\u0059', 
		'\u005A', '\u005B', '\u005C', '\u005D', '\u005E', '\u005F', 
		'\u0060', '\u0061', '\u0062', '\u0063', '\u0064', '\u0065', 
		'\u0066', '\u0067', '\u0068', '\u0069', '\u006A', '\u006B', 
		'\u006C', '\u006D', '\u006E', '\u006F', '\u0070', '\u0071', 
		'\u0072', '\u0073', '\u0074', '\u0075', '\u0076', '\u0077', 
		'\u0078', '\u0079', '\u007A', '\u007B', '\u007C', '\u007D', 
		'\u007E', '\u007F', '\u0080', '\u0081', '\u0082', '\u0083', 
		'\u0084', '\u0085', '\u0086', '\u0087', '\u0088', '\u0089', 
		'\u008A', '\u008B', '\u008C', '\u008D', '\u008E', '\u008F', 
		'\u0090', '\u0091', '\u0092', '\u0093', '\u0094', '\u0095', 
		'\u0096', '\u0097', '\u0098', '\u0099', '\u009A', '\u009B', 
		'\u009C', '\u009D', '\u009E', '\u009F', '\u00A0', '\u00A1', 
		'\u00A2', '\u00A3', '\u00A4', '\u00A5', '\u00A6', '\u00A7', 
		'\u00A8', '\u00A9', '\u00AA', '\u00AB', '\u00AC', '\u00AD', 
		'\u00AE', '\u00AF', '\u00B0', '\u00B1', '\u00B2', '\u00B3', 
		'\u00B4', '\u00B5', '\u00B6', '\u00B7', '\u00B8', '\u00B9', 
		'\u00BA', '\u00BB', '\u00BC', '\u00BD', '\u00BE', '\u00BF', 
		'\u00C0', '\u00C1', '\u00C2', '\u00C3', '\u00C4', '\u00C5', 
		'\u00C6', '\u00C7', '\u00C8', '\u00C9', '\u00CA', '\u00CB', 
		'\u00CC', '\u00CD', '\u00CE', '\u00CF', '\u011E', '\u00D1', 
		'\u00D2', '\u00D3', '\u00D4', '\u00D5', '\u00D6', '\u00D7', 
		'\u00D8', '\u00D9', '\u00DA', '\u00DB', '\u00DC', '\u0130', 
		'\u015E', '\u00DF', '\u00E0', '\u00E1', '\u00E2', '\u00E3', 
		'\u00E4', '\u00E5', '\u00E6', '\u00E7', '\u00E8', '\u00E9', 
		'\u00EA', '\u00EB', '\u00EC', '\u00ED', '\u00EE', '\u00EF', 
		'\u011F', '\u00F1', '\u00F2', '\u00F3', '\u00F4', '\u00F5', 
		'\u00F6', '\u00F7', '\u00F8', '\u00F9', '\u00FA', '\u00FB', 
		'\u00FC', '\u0131', '\u015F', '\u00FF', 
	};

	protected override void ToBytes(char[] chars, int charIndex, int charCount,
	                                byte[] bytes, int byteIndex)
	{
		int ch;
		while(charCount > 0)
		{
			ch = (int)(chars[charIndex++]);
			switch(ch)
			{
				case 0x0000:
				case 0x0001:
				case 0x0002:
				case 0x0003:
				case 0x0004:
				case 0x0005:
				case 0x0006:
				case 0x0007:
				case 0x0008:
				case 0x0009:
				case 0x000A:
				case 0x000B:
				case 0x000C:
				case 0x000D:
				case 0x000E:
				case 0x000F:
				case 0x0010:
				case 0x0011:
				case 0x0012:
				case 0x0013:
				case 0x0014:
				case 0x0015:
				case 0x0016:
				case 0x0017:
				case 0x0018:
				case 0x0019:
				case 0x001A:
				case 0x001B:
				case 0x001C:
				case 0x001D:
				case 0x001E:
				case 0x001F:
				case 0x0020:
				case 0x0021:
				case 0x0022:
				case 0x0023:
				case 0x0024:
				case 0x0025:
				case 0x0026:
				case 0x0027:
				case 0x0028:
				case 0x0029:
				case 0x002A:
				case 0x002B:
				case 0x002C:
				case 0x002D:
				case 0x002E:
				case 0x002F:
				case 0x0030:
				case 0x0031:
				case 0x0032:
				case 0x0033:
				case 0x0034:
				case 0x0035:
				case 0x0036:
				case 0x0037:
				case 0x0038:
				case 0x0039:
				case 0x003A:
				case 0x003B:
				case 0x003C:
				case 0x003D:
				case 0x003E:
				case 0x003F:
				case 0x0040:
				case 0x0041:
				case 0x0042:
				case 0x0043:
				case 0x0044:
				case 0x0045:
				case 0x0046:
				case 0x0047:
				case 0x0048:
				case 0x0049:
				case 0x004A:
				case 0x004B:
				case 0x004C:
				case 0x004D:
				case 0x004E:
				case 0x004F:
				case 0x0050:
				case 0x0051:
				case 0x0052:
				case 0x0053:
				case 0x0054:
				case 0x0055:
				case 0x0056:
				case 0x0057:
				case 0x0058:
				case 0x0059:
				case 0x005A:
				case 0x005B:
				case 0x005C:
				case 0x005D:
				case 0x005E:
				case 0x005F:
				case 0x0060:
				case 0x0061:
				case 0x0062:
				case 0x0063:
				case 0x0064:
				case 0x0065:
				case 0x0066:
				case 0x0067:
				case 0x0068:
				case 0x0069:
				case 0x006A:
				case 0x006B:
				case 0x006C:
				case 0x006D:
				case 0x006E:
				case 0x006F:
				case 0x0070:
				case 0x0071:
				case 0x0072:
				case 0x0073:
				case 0x0074:
				case 0x0075:
				case 0x0076:
				case 0x0077:
				case 0x0078:
				case 0x0079:
				case 0x007A:
				case 0x007B:
				case 0x007C:
				case 0x007D:
				case 0x007E:
				case 0x007F:
				case 0x0080:
				case 0x0081:
				case 0x0082:
				case 0x0083:
				case 0x0084:
				case 0x0085:
				case 0x0086:
				case 0x0087:
				case 0x0088:
				case 0x0089:
				case 0x008A:
				case 0x008B:
				case 0x008C:
				case 0x008D:
				case 0x008E:
				case 0x008F:
				case 0x0090:
				case 0x0091:
				case 0x0092:
				case 0x0093:
				case 0x0094:
				case 0x0095:
				case 0x0096:
				case 0x0097:
				case 0x0098:
				case 0x0099:
				case 0x009A:
				case 0x009B:
				case 0x009C:
				case 0x009D:
				case 0x009E:
				case 0x009F:
				case 0x00A0:
				case 0x00A1:
				case 0x00A2:
				case 0x00A3:
				case 0x00A4:
				case 0x00A5:
				case 0x00A6:
				case 0x00A7:
				case 0x00A8:
				case 0x00A9:
				case 0x00AA:
				case 0x00AB:
				case 0x00AC:
				case 0x00AD:
				case 0x00AE:
				case 0x00AF:
				case 0x00B0:
				case 0x00B1:
				case 0x00B2:
				case 0x00B3:
				case 0x00B4:
				case 0x00B5:
				case 0x00B6:
				case 0x00B7:
				case 0x00B8:
				case 0x00B9:
				case 0x00BA:
				case 0x00BB:
				case 0x00BC:
				case 0x00BD:
				case 0x00BE:
				case 0x00BF:
				case 0x00C0:
				case 0x00C1:
				case 0x00C2:
				case 0x00C3:
				case 0x00C4:
				case 0x00C5:
				case 0x00C6:
				case 0x00C7:
				case 0x00C8:
				case 0x00C9:
				case 0x00CA:
				case 0x00CB:
				case 0x00CC:
				case 0x00CD:
				case 0x00CE:
				case 0x00CF:
				case 0x00D1:
				case 0x00D2:
				case 0x00D3:
				case 0x00D4:
				case 0x00D5:
				case 0x00D6:
				case 0x00D7:
				case 0x00D8:
				case 0x00D9:
				case 0x00DA:
				case 0x00DB:
				case 0x00DC:
				case 0x00DF:
				case 0x00E0:
				case 0x00E1:
				case 0x00E2:
				case 0x00E3:
				case 0x00E4:
				case 0x00E5:
				case 0x00E6:
				case 0x00E7:
				case 0x00E8:
				case 0x00E9:
				case 0x00EA:
				case 0x00EB:
				case 0x00EC:
				case 0x00ED:
				case 0x00EE:
				case 0x00EF:
				case 0x00F1:
				case 0x00F2:
				case 0x00F3:
				case 0x00F4:
				case 0x00F5:
				case 0x00F6:
				case 0x00F7:
				case 0x00F8:
				case 0x00F9:
				case 0x00FA:
				case 0x00FB:
				case 0x00FC:
				case 0x00FF:
					break;
				case 0x011E: ch = 0xD0; break;
				case 0x011F: ch = 0xF0; break;
				case 0x0130: ch = 0xDD; break;
				case 0x0131: ch = 0xFD; break;
				case 0x015E: ch = 0xDE; break;
				case 0x015F: ch = 0xFE; break;
				case 0x203E: ch = 0xAF; break;
				case 0xFF01:
				case 0xFF02:
				case 0xFF03:
				case 0xFF04:
				case 0xFF05:
				case 0xFF06:
				case 0xFF07:
				case 0xFF08:
				case 0xFF09:
				case 0xFF0A:
				case 0xFF0B:
				case 0xFF0C:
				case 0xFF0D:
				case 0xFF0E:
				case 0xFF0F:
				case 0xFF10:
				case 0xFF11:
				case 0xFF12:
				case 0xFF13:
				case 0xFF14:
				case 0xFF15:
				case 0xFF16:
				case 0xFF17:
				case 0xFF18:
				case 0xFF19:
				case 0xFF1A:
				case 0xFF1B:
				case 0xFF1C:
				case 0xFF1D:
				case 0xFF1E:
				case 0xFF1F:
				case 0xFF20:
				case 0xFF21:
				case 0xFF22:
				case 0xFF23:
				case 0xFF24:
				case 0xFF25:
				case 0xFF26:
				case 0xFF27:
				case 0xFF28:
				case 0xFF29:
				case 0xFF2A:
				case 0xFF2B:
				case 0xFF2C:
				case 0xFF2D:
				case 0xFF2E:
				case 0xFF2F:
				case 0xFF30:
				case 0xFF31:
				case 0xFF32:
				case 0xFF33:
				case 0xFF34:
				case 0xFF35:
				case 0xFF36:
				case 0xFF37:
				case 0xFF38:
				case 0xFF39:
				case 0xFF3A:
				case 0xFF3B:
				case 0xFF3C:
				case 0xFF3D:
				case 0xFF3E:
				case 0xFF3F:
				case 0xFF40:
				case 0xFF41:
				case 0xFF42:
				case 0xFF43:
				case 0xFF44:
				case 0xFF45:
				case 0xFF46:
				case 0xFF47:
				case 0xFF48:
				case 0xFF49:
				case 0xFF4A:
				case 0xFF4B:
				case 0xFF4C:
				case 0xFF4D:
				case 0xFF4E:
				case 0xFF4F:
				case 0xFF50:
				case 0xFF51:
				case 0xFF52:
				case 0xFF53:
				case 0xFF54:
				case 0xFF55:
				case 0xFF56:
				case 0xFF57:
				case 0xFF58:
				case 0xFF59:
				case 0xFF5A:
				case 0xFF5B:
				case 0xFF5C:
				case 0xFF5D:
				case 0xFF5E:
					ch -= 0xFEE0;
					break;
				default: ch = 0x3F; break;
			}
			bytes[byteIndex++] = (byte)ch;
			--charCount;
		}
	}

	protected override void ToBytes(String s, int charIndex, int charCount,
	                                byte[] bytes, int byteIndex)
	{
		int ch;
		while(charCount > 0)
		{
			ch = (int)(s[charIndex++]);
			switch(ch)
			{
				case 0x0000:
				case 0x0001:
				case 0x0002:
				case 0x0003:
				case 0x0004:
				case 0x0005:
				case 0x0006:
				case 0x0007:
				case 0x0008:
				case 0x0009:
				case 0x000A:
				case 0x000B:
				case 0x000C:
				case 0x000D:
				case 0x000E:
				case 0x000F:
				case 0x0010:
				case 0x0011:
				case 0x0012:
				case 0x0013:
				case 0x0014:
				case 0x0015:
				case 0x0016:
				case 0x0017:
				case 0x0018:
				case 0x0019:
				case 0x001A:
				case 0x001B:
				case 0x001C:
				case 0x001D:
				case 0x001E:
				case 0x001F:
				case 0x0020:
				case 0x0021:
				case 0x0022:
				case 0x0023:
				case 0x0024:
				case 0x0025:
				case 0x0026:
				case 0x0027:
				case 0x0028:
				case 0x0029:
				case 0x002A:
				case 0x002B:
				case 0x002C:
				case 0x002D:
				case 0x002E:
				case 0x002F:
				case 0x0030:
				case 0x0031:
				case 0x0032:
				case 0x0033:
				case 0x0034:
				case 0x0035:
				case 0x0036:
				case 0x0037:
				case 0x0038:
				case 0x0039:
				case 0x003A:
				case 0x003B:
				case 0x003C:
				case 0x003D:
				case 0x003E:
				case 0x003F:
				case 0x0040:
				case 0x0041:
				case 0x0042:
				case 0x0043:
				case 0x0044:
				case 0x0045:
				case 0x0046:
				case 0x0047:
				case 0x0048:
				case 0x0049:
				case 0x004A:
				case 0x004B:
				case 0x004C:
				case 0x004D:
				case 0x004E:
				case 0x004F:
				case 0x0050:
				case 0x0051:
				case 0x0052:
				case 0x0053:
				case 0x0054:
				case 0x0055:
				case 0x0056:
				case 0x0057:
				case 0x0058:
				case 0x0059:
				case 0x005A:
				case 0x005B:
				case 0x005C:
				case 0x005D:
				case 0x005E:
				case 0x005F:
				case 0x0060:
				case 0x0061:
				case 0x0062:
				case 0x0063:
				case 0x0064:
				case 0x0065:
				case 0x0066:
				case 0x0067:
				case 0x0068:
				case 0x0069:
				case 0x006A:
				case 0x006B:
				case 0x006C:
				case 0x006D:
				case 0x006E:
				case 0x006F:
				case 0x0070:
				case 0x0071:
				case 0x0072:
				case 0x0073:
				case 0x0074:
				case 0x0075:
				case 0x0076:
				case 0x0077:
				case 0x0078:
				case 0x0079:
				case 0x007A:
				case 0x007B:
				case 0x007C:
				case 0x007D:
				case 0x007E:
				case 0x007F:
				case 0x0080:
				case 0x0081:
				case 0x0082:
				case 0x0083:
				case 0x0084:
				case 0x0085:
				case 0x0086:
				case 0x0087:
				case 0x0088:
				case 0x0089:
				case 0x008A:
				case 0x008B:
				case 0x008C:
				case 0x008D:
				case 0x008E:
				case 0x008F:
				case 0x0090:
				case 0x0091:
				case 0x0092:
				case 0x0093:
				case 0x0094:
				case 0x0095:
				case 0x0096:
				case 0x0097:
				case 0x0098:
				case 0x0099:
				case 0x009A:
				case 0x009B:
				case 0x009C:
				case 0x009D:
				case 0x009E:
				case 0x009F:
				case 0x00A0:
				case 0x00A1:
				case 0x00A2:
				case 0x00A3:
				case 0x00A4:
				case 0x00A5:
				case 0x00A6:
				case 0x00A7:
				case 0x00A8:
				case 0x00A9:
				case 0x00AA:
				case 0x00AB:
				case 0x00AC:
				case 0x00AD:
				case 0x00AE:
				case 0x00AF:
				case 0x00B0:
				case 0x00B1:
				case 0x00B2:
				case 0x00B3:
				case 0x00B4:
				case 0x00B5:
				case 0x00B6:
				case 0x00B7:
				case 0x00B8:
				case 0x00B9:
				case 0x00BA:
				case 0x00BB:
				case 0x00BC:
				case 0x00BD:
				case 0x00BE:
				case 0x00BF:
				case 0x00C0:
				case 0x00C1:
				case 0x00C2:
				case 0x00C3:
				case 0x00C4:
				case 0x00C5:
				case 0x00C6:
				case 0x00C7:
				case 0x00C8:
				case 0x00C9:
				case 0x00CA:
				case 0x00CB:
				case 0x00CC:
				case 0x00CD:
				case 0x00CE:
				case 0x00CF:
				case 0x00D1:
				case 0x00D2:
				case 0x00D3:
				case 0x00D4:
				case 0x00D5:
				case 0x00D6:
				case 0x00D7:
				case 0x00D8:
				case 0x00D9:
				case 0x00DA:
				case 0x00DB:
				case 0x00DC:
				case 0x00DF:
				case 0x00E0:
				case 0x00E1:
				case 0x00E2:
				case 0x00E3:
				case 0x00E4:
				case 0x00E5:
				case 0x00E6:
				case 0x00E7:
				case 0x00E8:
				case 0x00E9:
				case 0x00EA:
				case 0x00EB:
				case 0x00EC:
				case 0x00ED:
				case 0x00EE:
				case 0x00EF:
				case 0x00F1:
				case 0x00F2:
				case 0x00F3:
				case 0x00F4:
				case 0x00F5:
				case 0x00F6:
				case 0x00F7:
				case 0x00F8:
				case 0x00F9:
				case 0x00FA:
				case 0x00FB:
				case 0x00FC:
				case 0x00FF:
					break;
				case 0x011E: ch = 0xD0; break;
				case 0x011F: ch = 0xF0; break;
				case 0x0130: ch = 0xDD; break;
				case 0x0131: ch = 0xFD; break;
				case 0x015E: ch = 0xDE; break;
				case 0x015F: ch = 0xFE; break;
				case 0x203E: ch = 0xAF; break;
				case 0xFF01:
				case 0xFF02:
				case 0xFF03:
				case 0xFF04:
				case 0xFF05:
				case 0xFF06:
				case 0xFF07:
				case 0xFF08:
				case 0xFF09:
				case 0xFF0A:
				case 0xFF0B:
				case 0xFF0C:
				case 0xFF0D:
				case 0xFF0E:
				case 0xFF0F:
				case 0xFF10:
				case 0xFF11:
				case 0xFF12:
				case 0xFF13:
				case 0xFF14:
				case 0xFF15:
				case 0xFF16:
				case 0xFF17:
				case 0xFF18:
				case 0xFF19:
				case 0xFF1A:
				case 0xFF1B:
				case 0xFF1C:
				case 0xFF1D:
				case 0xFF1E:
				case 0xFF1F:
				case 0xFF20:
				case 0xFF21:
				case 0xFF22:
				case 0xFF23:
				case 0xFF24:
				case 0xFF25:
				case 0xFF26:
				case 0xFF27:
				case 0xFF28:
				case 0xFF29:
				case 0xFF2A:
				case 0xFF2B:
				case 0xFF2C:
				case 0xFF2D:
				case 0xFF2E:
				case 0xFF2F:
				case 0xFF30:
				case 0xFF31:
				case 0xFF32:
				case 0xFF33:
				case 0xFF34:
				case 0xFF35:
				case 0xFF36:
				case 0xFF37:
				case 0xFF38:
				case 0xFF39:
				case 0xFF3A:
				case 0xFF3B:
				case 0xFF3C:
				case 0xFF3D:
				case 0xFF3E:
				case 0xFF3F:
				case 0xFF40:
				case 0xFF41:
				case 0xFF42:
				case 0xFF43:
				case 0xFF44:
				case 0xFF45:
				case 0xFF46:
				case 0xFF47:
				case 0xFF48:
				case 0xFF49:
				case 0xFF4A:
				case 0xFF4B:
				case 0xFF4C:
				case 0xFF4D:
				case 0xFF4E:
				case 0xFF4F:
				case 0xFF50:
				case 0xFF51:
				case 0xFF52:
				case 0xFF53:
				case 0xFF54:
				case 0xFF55:
				case 0xFF56:
				case 0xFF57:
				case 0xFF58:
				case 0xFF59:
				case 0xFF5A:
				case 0xFF5B:
				case 0xFF5C:
				case 0xFF5D:
				case 0xFF5E:
					ch -= 0xFEE0;
					break;
				default: ch = 0x3F; break;
			}
			bytes[byteIndex++] = (byte)ch;
			--charCount;
		}
	}

}; // class CP28599

internal class ENCiso_8859_9 : CP28599
{
	public ENCiso_8859_9() : base() {}

}; // class ENCiso_8859_9

}; // namespace System.Text
