/*
 * CP28596.cs - Arabic (ISO) code page.
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

// Generated from "ibm-1089.ucm".

namespace System.Text
{

using System;

internal class CP28596 : ByteEncoding
{
	public CP28596()
		: base(28596, ToChars, "Arabic (ISO)",
		       "iso-8859-6", "iso-8859-6", "iso-8859-6",
		       true, true, true, true, 1256)
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
		'\u009C', '\u009D', '\u009E', '\u009F', '\u00A0', '\u003F', 
		'\u003F', '\u003F', '\u00A4', '\u003F', '\u003F', '\u003F', 
		'\u003F', '\u003F', '\u003F', '\u003F', '\u060C', '\u00AD', 
		'\u003F', '\u003F', '\u003F', '\u003F', '\u003F', '\u003F', 
		'\u003F', '\u003F', '\u003F', '\u003F', '\u003F', '\u003F', 
		'\u003F', '\u061B', '\u003F', '\u003F', '\u003F', '\u061F', 
		'\u003F', '\u0621', '\u0622', '\u0623', '\u0624', '\u0625', 
		'\u0626', '\u0627', '\u0628', '\u0629', '\u062A', '\u062B', 
		'\u062C', '\u062D', '\u062E', '\u062F', '\u0630', '\u0631', 
		'\u0632', '\u0633', '\u0634', '\u0635', '\u0636', '\u0637', 
		'\u0638', '\u0639', '\u063A', '\u003F', '\u003F', '\u003F', 
		'\u003F', '\u003F', '\u0640', '\u0641', '\u0642', '\u0643', 
		'\u0644', '\u0645', '\u0646', '\u0647', '\u0648', '\u0649', 
		'\u064A', '\u064B', '\u064C', '\u064D', '\u064E', '\u064F', 
		'\u0650', '\u0651', '\u0652', '\u003F', '\u003F', '\u003F', 
		'\u003F', '\u003F', '\u003F', '\u003F', '\u003F', '\u003F', 
		'\u003F', '\u003F', '\u003F', '\u003F', 
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
				case 0x00A4:
				case 0x00AD:
					break;
				case 0x060C: ch = 0xAC; break;
				case 0x061B: ch = 0xBB; break;
				case 0x061F: ch = 0xBF; break;
				case 0x0621:
				case 0x0622:
				case 0x0623:
				case 0x0624:
				case 0x0625:
				case 0x0626:
				case 0x0627:
				case 0x0628:
				case 0x0629:
				case 0x062A:
				case 0x062B:
				case 0x062C:
				case 0x062D:
				case 0x062E:
				case 0x062F:
				case 0x0630:
				case 0x0631:
				case 0x0632:
				case 0x0633:
				case 0x0634:
				case 0x0635:
				case 0x0636:
				case 0x0637:
				case 0x0638:
				case 0x0639:
				case 0x063A:
					ch -= 0x0560;
					break;
				case 0x0640:
				case 0x0641:
				case 0x0642:
				case 0x0643:
				case 0x0644:
				case 0x0645:
				case 0x0646:
				case 0x0647:
				case 0x0648:
				case 0x0649:
				case 0x064A:
				case 0x064B:
				case 0x064C:
				case 0x064D:
				case 0x064E:
				case 0x064F:
				case 0x0650:
				case 0x0651:
				case 0x0652:
					ch -= 0x0560;
					break;
				case 0x0660:
				case 0x0661:
				case 0x0662:
				case 0x0663:
				case 0x0664:
				case 0x0665:
				case 0x0666:
				case 0x0667:
				case 0x0668:
				case 0x0669:
					ch -= 0x0630;
					break;
				case 0x066A: ch = 0x25; break;
				case 0x066B: ch = 0x2C; break;
				case 0x066C: ch = 0x2E; break;
				case 0x066D: ch = 0x2A; break;
				case 0xFE70: ch = 0xEB; break;
				case 0xFE71: ch = 0xEB; break;
				case 0xFE72: ch = 0xEC; break;
				case 0xFE74: ch = 0xED; break;
				case 0xFE76: ch = 0xEE; break;
				case 0xFE77: ch = 0xEE; break;
				case 0xFE78: ch = 0xEF; break;
				case 0xFE79: ch = 0xEF; break;
				case 0xFE7A: ch = 0xF0; break;
				case 0xFE7B: ch = 0xF0; break;
				case 0xFE7C: ch = 0xF1; break;
				case 0xFE7D: ch = 0xF1; break;
				case 0xFE7E: ch = 0xF2; break;
				case 0xFE7F: ch = 0xF2; break;
				case 0xFE80: ch = 0xC1; break;
				case 0xFE81: ch = 0xC2; break;
				case 0xFE82: ch = 0xC2; break;
				case 0xFE83: ch = 0xC3; break;
				case 0xFE84: ch = 0xC3; break;
				case 0xFE85: ch = 0xC4; break;
				case 0xFE86: ch = 0xC4; break;
				case 0xFE87: ch = 0xC5; break;
				case 0xFE88: ch = 0xC5; break;
				case 0xFE89: ch = 0xC6; break;
				case 0xFE8A: ch = 0xC6; break;
				case 0xFE8B: ch = 0xC6; break;
				case 0xFE8C: ch = 0xC6; break;
				case 0xFE8D: ch = 0xC7; break;
				case 0xFE8E: ch = 0xC7; break;
				case 0xFE8F: ch = 0xC8; break;
				case 0xFE90: ch = 0xC8; break;
				case 0xFE91: ch = 0xC8; break;
				case 0xFE92: ch = 0xC8; break;
				case 0xFE93: ch = 0xC9; break;
				case 0xFE94: ch = 0xC9; break;
				case 0xFE95: ch = 0xCA; break;
				case 0xFE96: ch = 0xCA; break;
				case 0xFE97: ch = 0xCA; break;
				case 0xFE98: ch = 0xCA; break;
				case 0xFE99: ch = 0xCB; break;
				case 0xFE9A: ch = 0xCB; break;
				case 0xFE9B: ch = 0xCB; break;
				case 0xFE9C: ch = 0xCB; break;
				case 0xFE9D: ch = 0xCC; break;
				case 0xFE9E: ch = 0xCC; break;
				case 0xFE9F: ch = 0xCC; break;
				case 0xFEA0: ch = 0xCC; break;
				case 0xFEA1: ch = 0xCD; break;
				case 0xFEA2: ch = 0xCD; break;
				case 0xFEA3: ch = 0xCD; break;
				case 0xFEA4: ch = 0xCD; break;
				case 0xFEA5: ch = 0xCE; break;
				case 0xFEA6: ch = 0xCE; break;
				case 0xFEA7: ch = 0xCE; break;
				case 0xFEA8: ch = 0xCE; break;
				case 0xFEA9: ch = 0xCF; break;
				case 0xFEAA: ch = 0xCF; break;
				case 0xFEAB: ch = 0xD0; break;
				case 0xFEAC: ch = 0xD0; break;
				case 0xFEAD: ch = 0xD1; break;
				case 0xFEAE: ch = 0xD1; break;
				case 0xFEAF: ch = 0xD2; break;
				case 0xFEB0: ch = 0xD2; break;
				case 0xFEB1: ch = 0xD3; break;
				case 0xFEB2: ch = 0xD3; break;
				case 0xFEB3: ch = 0xD3; break;
				case 0xFEB4: ch = 0xD3; break;
				case 0xFEB5: ch = 0xD4; break;
				case 0xFEB6: ch = 0xD4; break;
				case 0xFEB7: ch = 0xD4; break;
				case 0xFEB8: ch = 0xD4; break;
				case 0xFEB9: ch = 0xD5; break;
				case 0xFEBA: ch = 0xD5; break;
				case 0xFEBB: ch = 0xD5; break;
				case 0xFEBC: ch = 0xD5; break;
				case 0xFEBD: ch = 0xD6; break;
				case 0xFEBE: ch = 0xD6; break;
				case 0xFEBF: ch = 0xD6; break;
				case 0xFEC0: ch = 0xD6; break;
				case 0xFEC1: ch = 0xD7; break;
				case 0xFEC2: ch = 0xD7; break;
				case 0xFEC3: ch = 0xD7; break;
				case 0xFEC4: ch = 0xD7; break;
				case 0xFEC5: ch = 0xD8; break;
				case 0xFEC6: ch = 0xD8; break;
				case 0xFEC7: ch = 0xD8; break;
				case 0xFEC8: ch = 0xD8; break;
				case 0xFEC9: ch = 0xD9; break;
				case 0xFECA: ch = 0xD9; break;
				case 0xFECB: ch = 0xD9; break;
				case 0xFECC: ch = 0xD9; break;
				case 0xFECD: ch = 0xDA; break;
				case 0xFECE: ch = 0xDA; break;
				case 0xFECF: ch = 0xDA; break;
				case 0xFED0: ch = 0xDA; break;
				case 0xFED1: ch = 0xE1; break;
				case 0xFED2: ch = 0xE1; break;
				case 0xFED3: ch = 0xE1; break;
				case 0xFED4: ch = 0xE1; break;
				case 0xFED5: ch = 0xE2; break;
				case 0xFED6: ch = 0xE2; break;
				case 0xFED7: ch = 0xE2; break;
				case 0xFED8: ch = 0xE2; break;
				case 0xFED9: ch = 0xE3; break;
				case 0xFEDA: ch = 0xE3; break;
				case 0xFEDB: ch = 0xE3; break;
				case 0xFEDC: ch = 0xE3; break;
				case 0xFEDD: ch = 0xE4; break;
				case 0xFEDE: ch = 0xE4; break;
				case 0xFEDF: ch = 0xE4; break;
				case 0xFEE0: ch = 0xE4; break;
				case 0xFEE1: ch = 0xE5; break;
				case 0xFEE2: ch = 0xE5; break;
				case 0xFEE3: ch = 0xE5; break;
				case 0xFEE4: ch = 0xE5; break;
				case 0xFEE5: ch = 0xE6; break;
				case 0xFEE6: ch = 0xE6; break;
				case 0xFEE7: ch = 0xE6; break;
				case 0xFEE8: ch = 0xE6; break;
				case 0xFEE9: ch = 0xE7; break;
				case 0xFEEA: ch = 0xE7; break;
				case 0xFEEB: ch = 0xE7; break;
				case 0xFEEC: ch = 0xE7; break;
				case 0xFEED: ch = 0xE8; break;
				case 0xFEEE: ch = 0xE8; break;
				case 0xFEEF: ch = 0xE9; break;
				case 0xFEF0: ch = 0xE9; break;
				case 0xFEF1: ch = 0xEA; break;
				case 0xFEF2: ch = 0xEA; break;
				case 0xFEF3: ch = 0xEA; break;
				case 0xFEF4: ch = 0xEA; break;
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
				case 0x00A4:
				case 0x00AD:
					break;
				case 0x060C: ch = 0xAC; break;
				case 0x061B: ch = 0xBB; break;
				case 0x061F: ch = 0xBF; break;
				case 0x0621:
				case 0x0622:
				case 0x0623:
				case 0x0624:
				case 0x0625:
				case 0x0626:
				case 0x0627:
				case 0x0628:
				case 0x0629:
				case 0x062A:
				case 0x062B:
				case 0x062C:
				case 0x062D:
				case 0x062E:
				case 0x062F:
				case 0x0630:
				case 0x0631:
				case 0x0632:
				case 0x0633:
				case 0x0634:
				case 0x0635:
				case 0x0636:
				case 0x0637:
				case 0x0638:
				case 0x0639:
				case 0x063A:
					ch -= 0x0560;
					break;
				case 0x0640:
				case 0x0641:
				case 0x0642:
				case 0x0643:
				case 0x0644:
				case 0x0645:
				case 0x0646:
				case 0x0647:
				case 0x0648:
				case 0x0649:
				case 0x064A:
				case 0x064B:
				case 0x064C:
				case 0x064D:
				case 0x064E:
				case 0x064F:
				case 0x0650:
				case 0x0651:
				case 0x0652:
					ch -= 0x0560;
					break;
				case 0x0660:
				case 0x0661:
				case 0x0662:
				case 0x0663:
				case 0x0664:
				case 0x0665:
				case 0x0666:
				case 0x0667:
				case 0x0668:
				case 0x0669:
					ch -= 0x0630;
					break;
				case 0x066A: ch = 0x25; break;
				case 0x066B: ch = 0x2C; break;
				case 0x066C: ch = 0x2E; break;
				case 0x066D: ch = 0x2A; break;
				case 0xFE70: ch = 0xEB; break;
				case 0xFE71: ch = 0xEB; break;
				case 0xFE72: ch = 0xEC; break;
				case 0xFE74: ch = 0xED; break;
				case 0xFE76: ch = 0xEE; break;
				case 0xFE77: ch = 0xEE; break;
				case 0xFE78: ch = 0xEF; break;
				case 0xFE79: ch = 0xEF; break;
				case 0xFE7A: ch = 0xF0; break;
				case 0xFE7B: ch = 0xF0; break;
				case 0xFE7C: ch = 0xF1; break;
				case 0xFE7D: ch = 0xF1; break;
				case 0xFE7E: ch = 0xF2; break;
				case 0xFE7F: ch = 0xF2; break;
				case 0xFE80: ch = 0xC1; break;
				case 0xFE81: ch = 0xC2; break;
				case 0xFE82: ch = 0xC2; break;
				case 0xFE83: ch = 0xC3; break;
				case 0xFE84: ch = 0xC3; break;
				case 0xFE85: ch = 0xC4; break;
				case 0xFE86: ch = 0xC4; break;
				case 0xFE87: ch = 0xC5; break;
				case 0xFE88: ch = 0xC5; break;
				case 0xFE89: ch = 0xC6; break;
				case 0xFE8A: ch = 0xC6; break;
				case 0xFE8B: ch = 0xC6; break;
				case 0xFE8C: ch = 0xC6; break;
				case 0xFE8D: ch = 0xC7; break;
				case 0xFE8E: ch = 0xC7; break;
				case 0xFE8F: ch = 0xC8; break;
				case 0xFE90: ch = 0xC8; break;
				case 0xFE91: ch = 0xC8; break;
				case 0xFE92: ch = 0xC8; break;
				case 0xFE93: ch = 0xC9; break;
				case 0xFE94: ch = 0xC9; break;
				case 0xFE95: ch = 0xCA; break;
				case 0xFE96: ch = 0xCA; break;
				case 0xFE97: ch = 0xCA; break;
				case 0xFE98: ch = 0xCA; break;
				case 0xFE99: ch = 0xCB; break;
				case 0xFE9A: ch = 0xCB; break;
				case 0xFE9B: ch = 0xCB; break;
				case 0xFE9C: ch = 0xCB; break;
				case 0xFE9D: ch = 0xCC; break;
				case 0xFE9E: ch = 0xCC; break;
				case 0xFE9F: ch = 0xCC; break;
				case 0xFEA0: ch = 0xCC; break;
				case 0xFEA1: ch = 0xCD; break;
				case 0xFEA2: ch = 0xCD; break;
				case 0xFEA3: ch = 0xCD; break;
				case 0xFEA4: ch = 0xCD; break;
				case 0xFEA5: ch = 0xCE; break;
				case 0xFEA6: ch = 0xCE; break;
				case 0xFEA7: ch = 0xCE; break;
				case 0xFEA8: ch = 0xCE; break;
				case 0xFEA9: ch = 0xCF; break;
				case 0xFEAA: ch = 0xCF; break;
				case 0xFEAB: ch = 0xD0; break;
				case 0xFEAC: ch = 0xD0; break;
				case 0xFEAD: ch = 0xD1; break;
				case 0xFEAE: ch = 0xD1; break;
				case 0xFEAF: ch = 0xD2; break;
				case 0xFEB0: ch = 0xD2; break;
				case 0xFEB1: ch = 0xD3; break;
				case 0xFEB2: ch = 0xD3; break;
				case 0xFEB3: ch = 0xD3; break;
				case 0xFEB4: ch = 0xD3; break;
				case 0xFEB5: ch = 0xD4; break;
				case 0xFEB6: ch = 0xD4; break;
				case 0xFEB7: ch = 0xD4; break;
				case 0xFEB8: ch = 0xD4; break;
				case 0xFEB9: ch = 0xD5; break;
				case 0xFEBA: ch = 0xD5; break;
				case 0xFEBB: ch = 0xD5; break;
				case 0xFEBC: ch = 0xD5; break;
				case 0xFEBD: ch = 0xD6; break;
				case 0xFEBE: ch = 0xD6; break;
				case 0xFEBF: ch = 0xD6; break;
				case 0xFEC0: ch = 0xD6; break;
				case 0xFEC1: ch = 0xD7; break;
				case 0xFEC2: ch = 0xD7; break;
				case 0xFEC3: ch = 0xD7; break;
				case 0xFEC4: ch = 0xD7; break;
				case 0xFEC5: ch = 0xD8; break;
				case 0xFEC6: ch = 0xD8; break;
				case 0xFEC7: ch = 0xD8; break;
				case 0xFEC8: ch = 0xD8; break;
				case 0xFEC9: ch = 0xD9; break;
				case 0xFECA: ch = 0xD9; break;
				case 0xFECB: ch = 0xD9; break;
				case 0xFECC: ch = 0xD9; break;
				case 0xFECD: ch = 0xDA; break;
				case 0xFECE: ch = 0xDA; break;
				case 0xFECF: ch = 0xDA; break;
				case 0xFED0: ch = 0xDA; break;
				case 0xFED1: ch = 0xE1; break;
				case 0xFED2: ch = 0xE1; break;
				case 0xFED3: ch = 0xE1; break;
				case 0xFED4: ch = 0xE1; break;
				case 0xFED5: ch = 0xE2; break;
				case 0xFED6: ch = 0xE2; break;
				case 0xFED7: ch = 0xE2; break;
				case 0xFED8: ch = 0xE2; break;
				case 0xFED9: ch = 0xE3; break;
				case 0xFEDA: ch = 0xE3; break;
				case 0xFEDB: ch = 0xE3; break;
				case 0xFEDC: ch = 0xE3; break;
				case 0xFEDD: ch = 0xE4; break;
				case 0xFEDE: ch = 0xE4; break;
				case 0xFEDF: ch = 0xE4; break;
				case 0xFEE0: ch = 0xE4; break;
				case 0xFEE1: ch = 0xE5; break;
				case 0xFEE2: ch = 0xE5; break;
				case 0xFEE3: ch = 0xE5; break;
				case 0xFEE4: ch = 0xE5; break;
				case 0xFEE5: ch = 0xE6; break;
				case 0xFEE6: ch = 0xE6; break;
				case 0xFEE7: ch = 0xE6; break;
				case 0xFEE8: ch = 0xE6; break;
				case 0xFEE9: ch = 0xE7; break;
				case 0xFEEA: ch = 0xE7; break;
				case 0xFEEB: ch = 0xE7; break;
				case 0xFEEC: ch = 0xE7; break;
				case 0xFEED: ch = 0xE8; break;
				case 0xFEEE: ch = 0xE8; break;
				case 0xFEEF: ch = 0xE9; break;
				case 0xFEF0: ch = 0xE9; break;
				case 0xFEF1: ch = 0xEA; break;
				case 0xFEF2: ch = 0xEA; break;
				case 0xFEF3: ch = 0xEA; break;
				case 0xFEF4: ch = 0xEA; break;
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

}; // class CP28596

internal class ENCiso_8859_6 : CP28596
{
	public ENCiso_8859_6() : base() {}

}; // class ENCiso_8859_6

}; // namespace System.Text
