/*
 * XmlReader.cs - Implementation of the "System.XmlReader" class.
 *
 * Copyright (C) 2002 Southern Storm Software, Pty Ltd.
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
 
namespace System.Xml
{

using System;
using System.Text;

// This class reads and parses a complete xml file into memory, the
// methods called on it return data from the parses memory file.
// This is a simple however memory inefficient use, but the efficiency
// of parsing should improve.

// Due to the use of XmlNode in the parsing process this class cannot be 
// overloaded from outside without copying XmlNode into the outside lib.

public abstract class XmlReader
{
	//Variables
	internal XmlNode root; //The root node, it can be used to access all other nodes
	internal XmlNode currentnode; //The pointer node

	//Methods
	protected XmlReader() {}
	
	public abstract void Close();
	
	public abstract String GetAttribute(int i);
	
	public abstract String GetAttribute(String name, String namespaceURI);
	
	public abstract String GetAttribute(String name);
	
	public static bool IsName(String str) 
			{
				if (!(str[0] == ':' || str[0] == '_' || IsLetter(str[0])))
					return false;

				for (int x = 1; x == str.Length; x++)
					{
						if (IsNameChar(str[x]) == false)
							return false;
					}
				return true;
			}
	
	public static bool IsNameToken(String str)
			{
				foreach(char c in str)
				{
					if (IsNameChar(c) == false)
						return false;
				}
				return true;
			}
	
	[TODO]
	public virtual bool IsStartElement(String localname, String ns)
			{ return false; }
	
	[TODO]
	public virtual bool IsStartElement(String name) { return false; }
	
	[TODO]
	public virtual bool IsStartElement() { return false; }
	
	public abstract String LookupNamespace(String prefix);
	
	public abstract void MoveToAttribute(int i);
	
	public abstract bool MoveToAttribute(String name, String ns);
	
	public abstract bool MoveToAttribute(String name);
	
	[TODO]
	public virtual XmlNodeType MoveToContent() { return XmlNodeType.None; }
	
	public abstract bool MoveToElement();
	
	public abstract bool MoveToFirstAttribute();
	
	public abstract bool MoveToNextAttribute();
	
	public abstract bool Read();
	
	public abstract bool ReadAttributeValue();
	
	[TODO]
	public virtual String ReadElementString(String localname, String ns)
			{ return null; }
	
	[TODO]
	public virtual String ReadElementString(String name) { return null; }
	
	[TODO]
	public virtual String ReadElementString() { return null; }
	
	[TODO]
	public virtual void ReadEndElement() {}
	
	public abstract String ReadInnerXml();
	
	public abstract String ReadOuterXml();
	
	[TODO]
	public virtual void ReadStartElement(String localname, String ns) {}
	
	[TODO]
	public virtual void ReadStartElement(String name) {}
	
	[TODO]
	public virtual void ReadStartElement() {}
	
	public abstract String ReadString();
	
	public abstract void ResolveEntity();
	
	[TODO]
	public virtual void Skip() {}
	
	public abstract int AttributeCount { get; }
	
	public abstract String BaseURI { get; }
	
	public virtual bool CanResolveEntity
			{
				get
				{
					return false;
				}
			}
	
	public abstract int Depth { get; }
	
	public abstract bool EOF { get; }
	
	[TODO]
	public virtual bool HasAttributes 
			{ 
				get
				{
					// TODO
					return false;
				}
			}
	
	public abstract bool HasValue { get; }
	
	public abstract bool IsDefault { get; }
	
	public abstract bool IsEmptyElement { get; }
	
	public abstract String this[int i] { get; }
	
	public abstract String this[String name] { get; }
	
	public abstract String this[String name, String namespaceURI] { get; }
	
	public abstract String LocalName { get; }
	
	public abstract String Name { get; }
	
	public abstract XmlNameTable NameTable { get; }
	
	public abstract String NamespaceURI { get; }
	
	public abstract XmlNodeType NodeType { get; }
	
	public abstract String Prefix { get; }
	
	public abstract char QuoteChar { get; }
	
	public abstract ReadState ReadState { get; }
	
	public abstract String Value { get; }
	
	public abstract String XmlLang { get; }
	
	public abstract XmlSpace XmlSpace { get; }

	//Internal methods
	protected internal static bool IsNameChar(char theChar)
			{
				return (IsLetter(theChar) || IsDigit(theChar)
					|| theChar == '.' || theChar == '-'
					|| theChar == '_' || theChar == ':'
					|| IsCombiningChar(theChar)
					|| IsExtender(theChar));
			}
	
	protected internal static bool IsLetter(char theChar)
			{
				return (IsBaseChar(theChar) || IsIdeographic(theChar));
			}
	
	protected internal static bool IsDigit(char theChar)
			{
				return ((theChar >= 0x0030 && theChar <= 0x0039) 
					|| (theChar >= 0x0660 && theChar <= 0x0669) 
					|| (theChar >= 0x06F0 && theChar <= 0x06F9) 
					|| (theChar >= 0x0966 && theChar <= 0x096F) 
					|| (theChar >= 0x09E6 && theChar <= 0x09EF) 
					|| (theChar >= 0x0A66 && theChar <= 0x0A6F) 
					|| (theChar >= 0x0AE6 && theChar <= 0x0AEF) 
					|| (theChar >= 0x0B66 && theChar <= 0x0B6F) 
					|| (theChar >= 0x0BE7 && theChar <= 0x0BEF) 
					|| (theChar >= 0x0C66 && theChar <= 0x0C6F) 
					|| (theChar >= 0x0CE6 && theChar <= 0x0CEF) 
					|| (theChar >= 0x0D66 && theChar <= 0x0D6F) 
					|| (theChar >= 0x0E50 && theChar <= 0x0E59) 
					|| (theChar >= 0x0ED0 && theChar <= 0x0ED9) 
					|| (theChar >= 0x0F20 && theChar <= 0x0F29));		
			}
			
	protected internal static bool IsCombiningChar(char theChar)
			{
				return ((theChar >= 0x0300 && theChar <= 0x0345) 
					|| (theChar >= 0x0360 && theChar <= 0x0361) 
					|| (theChar >= 0x0483 && theChar <= 0x0486) 
					|| (theChar >= 0x0591 && theChar <= 0x05A1) 
					|| (theChar >= 0x05A3 && theChar <= 0x05B9) 
					|| (theChar >= 0x05BB && theChar <= 0x05BD) 
					|| theChar == 0x05BF 
					|| (theChar >= 0x05C1 && theChar <= 0x05C2) 
					|| theChar == 0x05C4 
					|| (theChar >= 0x064B && theChar <= 0x0652) 
					|| theChar == 0x0670 
					|| (theChar >= 0x06D6 && theChar <= 0x06DC) 
					|| (theChar >= 0x06DD && theChar <= 0x06DF) 
					|| (theChar >= 0x06E0 && theChar <= 0x06E4) 
					|| (theChar >= 0x06E7 && theChar <= 0x06E8) 
					|| (theChar >= 0x06EA && theChar <= 0x06ED) 
					|| (theChar >= 0x0901 && theChar <= 0x0903) 
					|| theChar == 0x093C 
					|| (theChar >= 0x093E && theChar <= 0x094C) 
					|| theChar == 0x094D 
					|| (theChar >= 0x0951 && theChar <= 0x0954) 
					|| (theChar >= 0x0962 && theChar <= 0x0963) 
					|| (theChar >= 0x0981 && theChar <= 0x0983) 
					|| theChar == 0x09BC || theChar == 0x09BE 
					|| theChar == 0x09BF 
					|| (theChar >= 0x09C0 && theChar <= 0x09C4) 
					|| (theChar >= 0x09C7 && theChar <= 0x09C8) 
					|| (theChar >= 0x09CB && theChar <= 0x09CD) 
					|| theChar == 0x09D7 
					|| (theChar >= 0x09E2 && theChar <= 0x09E3) 
					|| theChar == 0x0A02 || theChar == 0x0A3C 
					|| theChar == 0x0A3E || theChar == 0x0A3F 
					|| (theChar >= 0x0A40 && theChar <= 0x0A42) 
					|| (theChar >= 0x0A47 && theChar <= 0x0A48) 
					|| (theChar >= 0x0A4B && theChar <= 0x0A4D) 
					|| (theChar >= 0x0A70 && theChar <= 0x0A71) 
					|| (theChar >= 0x0A81 && theChar <= 0x0A83) 
					|| theChar == 0x0ABC 
					|| (theChar >= 0x0ABE && theChar <= 0x0AC5) 
					|| (theChar >= 0x0AC7 && theChar <= 0x0AC9) 
					|| (theChar >= 0x0ACB && theChar <= 0x0ACD) 
					|| (theChar >= 0x0B01 && theChar <= 0x0B03) 
					|| theChar == 0x0B3C 
					|| (theChar >= 0x0B3E && theChar <= 0x0B43) 
					|| (theChar >= 0x0B47 && theChar <= 0x0B48) 
					|| (theChar >= 0x0B4B && theChar <= 0x0B4D) 
					|| (theChar >= 0x0B56 && theChar <= 0x0B57) 
					|| (theChar >= 0x0B82 && theChar <= 0x0B83) 
					|| (theChar >= 0x0BBE && theChar <= 0x0BC2) 
					|| (theChar >= 0x0BC6 && theChar <= 0x0BC8) 
					|| (theChar >= 0x0BCA && theChar <= 0x0BCD) 
					|| theChar == 0x0BD7 
					|| (theChar >= 0x0C01 && theChar <= 0x0C03) 
					|| (theChar >= 0x0C3E && theChar <= 0x0C44) 
					|| (theChar >= 0x0C46 && theChar <= 0x0C48) 
					|| (theChar >= 0x0C4A && theChar <= 0x0C4D) 
					|| (theChar >= 0x0C55 && theChar <= 0x0C56) 
					|| (theChar >= 0x0C82 && theChar <= 0x0C83) 
					|| (theChar >= 0x0CBE && theChar <= 0x0CC4) 
					|| (theChar >= 0x0CC6 && theChar <= 0x0CC8) 
					|| (theChar >= 0x0CCA && theChar <= 0x0CCD) 
					|| (theChar >= 0x0CD5 && theChar <= 0x0CD6) 
					|| (theChar >= 0x0D02 && theChar <= 0x0D03) 
					|| (theChar >= 0x0D3E && theChar <= 0x0D43) 
					|| (theChar >= 0x0D46 && theChar <= 0x0D48) 
					|| (theChar >= 0x0D4A && theChar <= 0x0D4D) 
					|| theChar == 0x0D57 || theChar == 0x0E31 
					|| (theChar >= 0x0E34 && theChar <= 0x0E3A) 
					|| (theChar >= 0x0E47 && theChar <= 0x0E4E) 
					|| theChar == 0x0EB1 
					|| (theChar >= 0x0EB4 && theChar <= 0x0EB9) 
					|| (theChar >= 0x0EBB && theChar <= 0x0EBC) 
					|| (theChar >= 0x0EC8 && theChar <= 0x0ECD) 
					|| (theChar >= 0x0F18 && theChar <= 0x0F19) 
					|| theChar == 0x0F35 || theChar == 0x0F37 
					|| theChar == 0x0F39 || theChar == 0x0F3E 
					|| theChar == 0x0F3F 
					|| (theChar >= 0x0F71 && theChar <= 0x0F84) 
					|| (theChar >= 0x0F86 && theChar <= 0x0F8B) 
					|| (theChar >= 0x0F90 && theChar <= 0x0F95) 
					|| theChar == 0x0F97 
					|| (theChar >= 0x0F99 && theChar <= 0x0FAD) 
					|| (theChar >= 0x0FB1 && theChar <= 0x0FB7) 
					|| theChar == 0x0FB9 
					|| (theChar >= 0x20D0 && theChar <= 0x20DC) 
					|| theChar == 0x20E1 
					|| (theChar >= 0x302A && theChar <= 0x302F) 
					|| theChar == 0x3099 || theChar == 0x309A);
			}
			
	protected internal static bool IsExtender (char theChar)
			{
				return (theChar == 0x00B7 || theChar == 0x02D0 
					|| theChar == 0x02D1 || theChar == 0x0387 
					|| theChar == 0x0640 || theChar == 0x0E46 
					|| theChar == 0x0EC6 || theChar == 0x3005 
					|| (theChar >= 0x3031 && theChar <= 0x3035) 
					|| (theChar >= 0x309D && theChar <= 0x309E) 
					|| (theChar >= 0x30FC && theChar <= 0x30FE));
			}

	protected internal static bool IsBaseChar (char theChar)
			{					
				//188 lines of if statement, it's a record :-)
				return ((theChar >= 0x0041 && theChar <= 0x005A) 
					|| (theChar >= 0x0061 && theChar <= 0x007A)
					|| (theChar >= 0x0061 && theChar <= 0x007A)
	  	  		|| (theChar >= 0x00C0 && theChar <= 0x00D6) 
					|| (theChar >= 0x00D8 && theChar <= 0x00F6) 
					|| (theChar >= 0x00F8 && theChar <= 0x00FF) 
					|| (theChar >= 0x0100 && theChar <= 0x0131) 
					|| (theChar >= 0x0134 && theChar <= 0x013E)
					|| (theChar >= 0x0141 && theChar <= 0x0148) 
					|| (theChar >= 0x014A && theChar <= 0x017E) 
					|| (theChar >= 0x0180 && theChar <= 0x01C3) 
					|| (theChar >= 0x01CD && theChar <= 0x01F0) 
					|| (theChar >= 0x01F4 && theChar <= 0x01F5) 
					|| (theChar >= 0x01FA && theChar <= 0x0217)  
					|| (theChar >= 0x0250 && theChar <= 0x02A8) 
					|| (theChar >= 0x02BB && theChar <= 0x02C1) 
					|| theChar == 0x0386 
					|| (theChar >= 0x0388 && theChar <= 0x038A) 
					|| theChar == 0x038C 
					|| (theChar >= 0x038E && theChar <= 0x03A1) 
					|| (theChar >= 0x03A3 && theChar <= 0x03CE) 
					|| (theChar >= 0x03D0 && theChar <= 0x03D6) 
					|| theChar == 0x03DA || theChar == 0x03DC 
					|| theChar == 0x03DE || theChar == 0x03E0 
					|| (theChar >= 0x03E2 && theChar <= 0x03F3) 
					|| (theChar >= 0x0401 && theChar <= 0x040C) 
					|| (theChar >= 0x040E && theChar <= 0x044F) 
					|| (theChar >= 0x0451 && theChar <= 0x045C) 
					|| (theChar >= 0x045E && theChar <= 0x0481) 
					|| (theChar >= 0x0490 && theChar <= 0x04C4) 
					|| (theChar >= 0x04C7 && theChar <= 0x04C8) 
					|| (theChar >= 0x04CB && theChar <= 0x04CC) 
					|| (theChar >= 0x04D0 && theChar <= 0x04EB) 
					|| (theChar >= 0x04EE && theChar <= 0x04F5) 
					|| (theChar >= 0x04F8 && theChar <= 0x04F9) 
					|| (theChar >= 0x0531 && theChar <= 0x0556) 
					|| theChar == 0x0559 
					|| (theChar >= 0x0561 && theChar <= 0x0586) 
					|| (theChar >= 0x05D0 && theChar <= 0x05EA) 
					|| (theChar >= 0x05F0 && theChar <= 0x05F2) 
					|| (theChar >= 0x0621 && theChar <= 0x063A) 
					|| (theChar >= 0x0641 && theChar <= 0x064A) 
					|| (theChar >= 0x0671 && theChar <= 0x06B7) 
					|| (theChar >= 0x06BA && theChar <= 0x06BE) 
					|| (theChar >= 0x06C0 && theChar <= 0x06CE) 
					|| (theChar >= 0x06D0 && theChar <= 0x06D3) 
					|| theChar == 0x06D5 
					|| (theChar >= 0x06E5 && theChar <= 0x06E6) 
					|| (theChar >= 0x0905 && theChar <= 0x0939) 
					|| theChar == 0x093D 
					|| (theChar >= 0x0958 && theChar <= 0x0961) 
					|| (theChar >= 0x0985 && theChar <= 0x098C) 
					|| (theChar >= 0x098F && theChar <= 0x0990) 
					|| (theChar >= 0x0993 && theChar <= 0x09A8) 
					|| (theChar >= 0x09AA && theChar <= 0x09B0) 
					|| theChar == 0x09B2 
					|| (theChar >= 0x09B6 && theChar <= 0x09B9) 
					|| (theChar >= 0x09DC && theChar <= 0x09DD) 
					|| (theChar >= 0x09DF && theChar <= 0x09E1) 
					|| (theChar >= 0x09F0 && theChar <= 0x09F1) 
					|| (theChar >= 0x0A05 && theChar <= 0x0A0A) 
					|| (theChar >= 0x0A0F && theChar <= 0x0A10) 
					|| (theChar >= 0x0A13 && theChar <= 0x0A28) 
					|| (theChar >= 0x0A2A && theChar <= 0x0A30) 
					|| (theChar >= 0x0A32 && theChar <= 0x0A33) 
					|| (theChar >= 0x0A35 && theChar <= 0x0A36) 
					|| (theChar >= 0x0A38 && theChar <= 0x0A39) 
					|| (theChar >= 0x0A59 && theChar <= 0x0A5C) 
					|| theChar == 0x0A5E 
					|| (theChar >= 0x0A72 && theChar <= 0x0A74) 
					|| (theChar >= 0x0A85 && theChar <= 0x0A8B) 
					|| theChar == 0x0A8D 
					|| (theChar >= 0x0A8F && theChar <= 0x0A91) 
					|| (theChar >= 0x0A93 && theChar <= 0x0AA8) 
					|| (theChar >= 0x0AAA && theChar <= 0x0AB0) 
					|| (theChar >= 0x0AB2 && theChar <= 0x0AB3) 
					|| (theChar >= 0x0AB5 && theChar <= 0x0AB9) 
					|| theChar == 0x0ABD || theChar == 0x0AE0 
					|| (theChar >= 0x0B05 && theChar <= 0x0B0C) 
					|| (theChar >= 0x0B0F && theChar <= 0x0B10) 
					|| (theChar >= 0x0B13 && theChar <= 0x0B28) 
					|| (theChar >= 0x0B2A && theChar <= 0x0B30) 
					|| (theChar >= 0x0B32 && theChar <= 0x0B33) 
					|| (theChar >= 0x0B36 && theChar <= 0x0B39) 
					|| theChar == 0x0B3D 
					|| (theChar >= 0x0B5C && theChar <= 0x0B5D) 
					|| (theChar >= 0x0B5F && theChar <= 0x0B61) 
					|| (theChar >= 0x0B85 && theChar <= 0x0B8A) 
					|| (theChar >= 0x0B8E && theChar <= 0x0B90) 
					|| (theChar >= 0x0B92 && theChar <= 0x0B95) 
					|| (theChar >= 0x0B99 && theChar <= 0x0B9A) 
					|| theChar == 0x0B9C 
					|| (theChar >= 0x0B9E && theChar <= 0x0B9F) 
					|| (theChar >= 0x0BA3 && theChar <= 0x0BA4) 
					|| (theChar >= 0x0BA8 && theChar <= 0x0BAA) 
					|| (theChar >= 0x0BAE && theChar <= 0x0BB5) 
					|| (theChar >= 0x0BB7 && theChar <= 0x0BB9) 
					|| (theChar >= 0x0C05 && theChar <= 0x0C0C) 
					|| (theChar >= 0x0C0E && theChar <= 0x0C10) 
					|| (theChar >= 0x0C12 && theChar <= 0x0C28) 
					|| (theChar >= 0x0C2A && theChar <= 0x0C33) 
					|| (theChar >= 0x0C35 && theChar <= 0x0C39) 
					|| (theChar >= 0x0C60 && theChar <= 0x0C61) 
					|| (theChar >= 0x0C85 && theChar <= 0x0C8C) 
					|| (theChar >= 0x0C8E && theChar <= 0x0C90) 
					|| (theChar >= 0x0C92 && theChar <= 0x0CA8) 
					|| (theChar >= 0x0CAA && theChar <= 0x0CB3) 
					|| (theChar >= 0x0CB5 && theChar <= 0x0CB9) 
					|| theChar == 0x0CDE 
					|| (theChar >= 0x0CE0 && theChar <= 0x0CE1) 
					|| (theChar >= 0x0D05 && theChar <= 0x0D0C) 
					|| (theChar >= 0x0D0E && theChar <= 0x0D10) 
					|| (theChar >= 0x0D12 && theChar <= 0x0D28) 
					|| (theChar >= 0x0D2A && theChar <= 0x0D39) 
					|| (theChar >= 0x0D60 && theChar <= 0x0D61) 
					|| (theChar >= 0x0E01 && theChar <= 0x0E2E) 
					|| theChar == 0x0E30 
					|| (theChar >= 0x0E32 && theChar <= 0x0E33) 
					|| (theChar >= 0x0E40 && theChar <= 0x0E45) 
					|| (theChar >= 0x0E81 && theChar <= 0x0E82) 
					|| theChar == 0x0E84 
					|| (theChar >= 0x0E87 && theChar <= 0x0E88) 
					|| theChar == 0x0E8A || theChar == 0x0E8D 
					|| (theChar >= 0x0E94 && theChar <= 0x0E97) 
					|| (theChar >= 0x0E99 && theChar <= 0x0E9F) 
					|| (theChar >= 0x0EA1 && theChar <= 0x0EA3) 
					|| theChar == 0x0EA5 || theChar == 0x0EA7 
					|| (theChar >= 0x0EAA && theChar <= 0x0EAB) 
					|| (theChar >= 0x0EAD && theChar <= 0x0EAE) 
					|| theChar == 0x0EB0 
					|| (theChar >= 0x0EB2 && theChar <= 0x0EB3) 
					|| theChar == 0x0EBD 
					|| (theChar >= 0x0EC0 && theChar <= 0x0EC4) 
					|| (theChar >= 0x0F40 && theChar <= 0x0F47) 
					|| (theChar >= 0x0F49 && theChar <= 0x0F69) 
					|| (theChar >= 0x10A0 && theChar <= 0x10C5) 
					|| (theChar >= 0x10D0 && theChar <= 0x10F6) 
					|| theChar == 0x1100 
					|| (theChar >= 0x1102 && theChar <= 0x1103) 
					|| (theChar >= 0x1105 && theChar <= 0x1107) 
					|| theChar == 0x1109 
					|| (theChar >= 0x110B && theChar <= 0x110C) 
					|| (theChar >= 0x110E && theChar <= 0x1112) 
					|| theChar == 0x113C || theChar == 0x113E 
					|| theChar == 0x1140 || theChar == 0x114C 
					|| theChar == 0x114E || theChar == 0x1150 
					|| (theChar >= 0x1154 && theChar <= 0x1155) 
					|| theChar == 0x1159 
					|| (theChar >= 0x115F && theChar <= 0x1161) 
					|| theChar == 0x1163 || theChar == 0x1165 
					|| theChar == 0x1167 || theChar == 0x1169 
					|| (theChar >= 0x116D && theChar <= 0x116E) 
					|| (theChar >= 0x1172 && theChar <= 0x1173) 
					|| theChar == 0x1175 || theChar == 0x119E 
					|| theChar == 0x11A8 || theChar == 0x11AB 
					|| (theChar >= 0x11AE && theChar <= 0x11AF) 
					|| (theChar >= 0x11B7 && theChar <= 0x11B8) 
					|| theChar == 0x11BA 
					|| (theChar >= 0x11BC && theChar <= 0x11C2) 
					|| theChar == 0x11EB || theChar == 0x11F0 
					|| theChar == 0x11F9 
					|| (theChar >= 0x1E00 && theChar <= 0x1E9B) 
					|| (theChar >= 0x1EA0 && theChar <= 0x1EF9) 
					|| (theChar >= 0x1F00 && theChar <= 0x1F15) 
					|| (theChar >= 0x1F18 && theChar <= 0x1F1D) 
					|| (theChar >= 0x1F20 && theChar <= 0x1F45) 
					|| (theChar >= 0x1F48 && theChar <= 0x1F4D) 
					|| (theChar >= 0x1F50 && theChar <= 0x1F57) 
					|| theChar == 0x1F59 || theChar == 0x1F5B 
					|| theChar == 0x1F5D 
					|| (theChar >= 0x1F5F && theChar <= 0x1F7D) 
					|| (theChar >= 0x1F80 && theChar <= 0x1FB4) 
					|| (theChar >= 0x1FB6 && theChar <= 0x1FBC) 
					|| theChar == 0x1FBE 
					|| (theChar >= 0x1FC2 && theChar <= 0x1FC4) 
					|| (theChar >= 0x1FC6 && theChar <= 0x1FCC) 
					|| (theChar >= 0x1FD0 && theChar <= 0x1FD3) 
					|| (theChar >= 0x1FD6 && theChar <= 0x1FDB) 
					|| (theChar >= 0x1FE0 && theChar <= 0x1FEC) 
					|| (theChar >= 0x1FF2 && theChar <= 0x1FF4) 
					|| (theChar >= 0x1FF6 && theChar <= 0x1FFC) 
					|| theChar == 0x2126 
					|| (theChar >= 0x212A && theChar <= 0x212B) 
					|| theChar == 0x212E 
					|| (theChar >= 0x2180 && theChar <= 0x2182) 
					|| (theChar >= 0x3041 && theChar <= 0x3094) 
					|| (theChar >= 0x30A1 && theChar <= 0x30FA) 
					|| (theChar >= 0x3105 && theChar <= 0x312C) 
					|| (theChar >= 0xAC00 && theChar <= 0xD7A3));
				//Long, wasn't it?									
			} //end method IsBaseChar

	protected internal static bool IsIdeographic (char theChar)
			{
				return ((theChar >= 0x4E00 && theChar <= 0x9FA5) 
					|| theChar == 0x3007 
					|| (theChar >= 0x3021 && theChar <= 0x3029));
			}

}; //class XmlReader

}; //namespace System.Xml

