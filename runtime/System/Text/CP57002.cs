/*
 * CP57002.cs - ISCII code pages 57002-57011.
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

namespace System.Text
{

using System;

// This class provides an abstract base for the ISCII encodings,
// which all have a similar pattern.  Code points 0x00-0x7F are
// the standard ASCII character set, and code points 0x80-0xFF
// are a shifted version of the Unicode character set, starting
// at a fixed offset.

internal abstract class ISCIIEncoding : Encoding
{
	// Internal state.
	protected int shift;
	protected String encodingName;
	protected String webName;

	// Constructor.
	protected ISCIIEncoding(int codePage, int shift,
						    String encodingName, String webName)
			: base(codePage)
			{
				this.shift = shift;
				this.encodingName = encodingName;
				this.webName = webName;
			}

	// Get the number of bytes needed to encode a character buffer.
	public override int GetByteCount(char[] chars, int index, int count)
			{
				if(chars == null)
				{
					throw new ArgumentNullException("chars");
				}
				if(index < 0 || index > chars.Length)
				{
					throw new ArgumentOutOfRangeException
						("index", _("ArgRange_Array"));
				}
				if(count < 0 || count > (chars.Length - index))
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_Array"));
				}
				return count;
			}

	// Convenience wrappers for "GetByteCount".
	public override int GetByteCount(String s)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return s.Length;
			}

	// Get the bytes that result from encoding a character buffer.
	public override int GetBytes(char[] chars, int charIndex, int charCount,
								 byte[] bytes, int byteIndex)
			{
				if(chars == null)
				{
					throw new ArgumentNullException("chars");
				}
				if(bytes == null)
				{
					throw new ArgumentNullException("bytes");
				}
				if(charIndex < 0 || charIndex > chars.Length)
				{
					throw new ArgumentOutOfRangeException
						("charIndex", _("ArgRange_Array"));
				}
				if(charCount < 0 || charCount > (chars.Length - charIndex))
				{
					throw new ArgumentOutOfRangeException
						("charCount", _("ArgRange_Array"));
				}
				if(byteIndex < 0 || byteIndex > bytes.Length)
				{
					throw new ArgumentOutOfRangeException
						("byteIndex", _("ArgRange_Array"));
				}
				if((bytes.Length - byteIndex) < charCount)
				{
					throw new ArgumentException
						(_("Arg_InsufficientSpace"), "bytes");
				}

				// Convert the characters into bytes.
				char ch;
				int posn = byteIndex;
				char first = (char)shift;
				char last = (char)(shift + 0x7F);
				while(charCount-- > 0)
				{
					ch = chars[charIndex++];
					if(ch < (char)0x0080)
					{
						// Regular ASCII subset.
						bytes[posn++] = (byte)ch;
					}
					else if(ch >= first && ch <= last)
					{
						// ISCII range that we need to shift.
						bytes[posn++] = (byte)(ch - first + 0x80);
					}
					else if(ch >= '\uFF01' && ch <= '\uFF5E')
					{
						// ASCII full-width characters.
						bytes[posn++] = (byte)(ch - 0xFEE0);
					}
					else
					{
						bytes[posn++] = (byte)'?';
					}
				}

				// Return the final length of the output.
				return posn - byteIndex;
			}

	// Convenience wrappers for "GetBytes".
	public override int GetBytes(String s, int charIndex, int charCount,
								 byte[] bytes, int byteIndex)
			{
				// Validate the parameters.
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				if(bytes == null)
				{
					throw new ArgumentNullException("bytes");
				}
				if(charIndex < 0 || charIndex > s.Length)
				{
					throw new ArgumentOutOfRangeException
						("charIndex", _("ArgRange_StringIndex"));
				}
				if(charCount < 0 || charCount > (s.Length - charIndex))
				{
					throw new ArgumentOutOfRangeException
						("charCount", _("ArgRange_StringRange"));
				}
				if(byteIndex < 0 || byteIndex > bytes.Length)
				{
					throw new ArgumentOutOfRangeException
						("byteIndex", _("ArgRange_Array"));
				}
				if((bytes.Length - byteIndex) < charCount)
				{
					throw new ArgumentException
						(_("Arg_InsufficientSpace"), "bytes");
				}

				// Convert the characters into bytes.
				char ch;
				int posn = byteIndex;
				char first = (char)shift;
				char last = (char)(shift + 0x7F);
				while(charCount-- > 0)
				{
					ch = s[charIndex++];
					if(ch < (char)0x0080)
					{
						// Regular ASCII subset.
						bytes[posn++] = (byte)ch;
					}
					else if(ch >= first && ch <= last)
					{
						// ISCII range that we need to shift.
						bytes[posn++] = (byte)(ch - first + 0x80);
					}
					else if(ch >= '\uFF01' && ch <= '\uFF5E')
					{
						// ASCII full-width characters.
						bytes[posn++] = (byte)(ch - 0xFEE0);
					}
					else
					{
						bytes[posn++] = (byte)'?';
					}
				}

				// Return the final length of the output.
				return posn - byteIndex;
			}

	// Get the number of characters needed to decode a byte buffer.
	public override int GetCharCount(byte[] bytes, int index, int count)
			{
				if(bytes == null)
				{
					throw new ArgumentNullException("bytes");
				}
				if(index < 0 || index > bytes.Length)
				{
					throw new ArgumentOutOfRangeException
						("index", _("ArgRange_Array"));
				}
				if(count < 0 || count > (bytes.Length - index))
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_Array"));
				}
				return count;
			}

	// Get the characters that result from decoding a byte buffer.
	public override int GetChars(byte[] bytes, int byteIndex, int byteCount,
								 char[] chars, int charIndex)
			{
				// Validate the parameters.
				if(bytes == null)
				{
					throw new ArgumentNullException("bytes");
				}
				if(chars == null)
				{
					throw new ArgumentNullException("chars");
				}
				if(byteIndex < 0 || byteIndex > bytes.Length)
				{
					throw new ArgumentOutOfRangeException
						("byteIndex", _("ArgRange_Array"));
				}
				if(byteCount < 0 || byteCount > (bytes.Length - byteIndex))
				{
					throw new ArgumentOutOfRangeException
						("byteCount", _("ArgRange_Array"));
				}
				if(charIndex < 0 || charIndex > chars.Length)
				{
					throw new ArgumentOutOfRangeException
						("charIndex", _("ArgRange_Array"));
				}
				if((chars.Length - charIndex) < byteCount)
				{
					throw new ArgumentException
						(_("Arg_InsufficientSpace"), "chars");
				}

				// Convert the bytes into characters.
				int count = byteCount;
				int byteval;
				int shift = this.shift - 0x80;
				while(count-- > 0)
				{
					byteval = (int)(bytes[byteIndex++]);
					if(byteval < 0x80)
					{
						// Ordinary ASCII character.
						chars[charIndex++] = (char)byteval;
					}
					else
					{
						// Shift the ISCII character into the Unicode page.
						chars[charIndex++] = (char)(byteval - shift);
					}
				}
				return byteCount;
			}

	// Get the maximum number of bytes needed to encode a
	// specified number of characters.
	public override int GetMaxByteCount(int charCount)
			{
				if(charCount < 0)
				{
					throw new ArgumentOutOfRangeException
						("charCount", _("ArgRange_NonNegative"));
				}
				return charCount;
			}

	// Get the maximum number of characters needed to decode a
	// specified number of bytes.
	public override int GetMaxCharCount(int byteCount)
			{
				if(byteCount < 0)
				{
					throw new ArgumentOutOfRangeException
						("byteCount", _("ArgRange_NonNegative"));
				}
				return byteCount;
			}

#if !ECMA_COMPAT

	// Get the mail body name for this encoding.
	public override String BodyName
			{
				get
				{
					return webName;
				}
			}

	// Get the human-readable name for this encoding.
	public override String EncodingName
			{
				get
				{
					return encodingName;
				}
			}

	// Get the mail agent header name for this encoding.
	public override String HeaderName
			{
				get
				{
					return webName;
				}
			}

	// Get the IANA-preferred Web name for this encoding.
	public override String WebName
			{
				get
				{
					return webName;
				}
			}

#endif // !ECMA_COMPAT

}; // class ISCIIEncoding

// Define the ISCII code pages as subclasses of "ISCIIEncoding".

internal class CP57002 : ISCIIEncoding
{
	public CP57002() : base(57002, 0x0900, "ISCII Devanagari", "x-iscii-de") {}

}; // class CP57002

internal class CP57003 : ISCIIEncoding
{
	public CP57003() : base(57003, 0x0980, "ISCII Bengali", "x-iscii-be") {}

}; // class CP57003

internal class CP57004 : ISCIIEncoding
{
	public CP57004() : base(57004, 0x0B80, "ISCII Tamil", "x-iscii-ta") {}

}; // class CP57004

internal class CP57005 : ISCIIEncoding
{
	public CP57005() : base(57005, 0x0B80, "ISCII Telugu", "x-iscii-te") {}

}; // class CP57005

internal class CP57006 : ISCIIEncoding
{
	// Note: Unicode has a "Sinhala" page, but no "Assamese" page.
	// Until I hear otherwise, I will assume that they are the same
	// thing with different names - Rhys Weatherley, 16 April 2002.
	public CP57006() : base(57006, 0x0D80, "ISCII Assamese", "x-iscii-as") {}

}; // class CP57006

internal class CP57007 : ISCIIEncoding
{
	public CP57007() : base(57007, 0x0B00, "ISCII Oriya", "x-iscii-or") {}

}; // class CP57007

internal class CP57008 : ISCIIEncoding
{
	public CP57008() : base(57008, 0x0C80, "ISCII Kannada", "x-iscii-ka") {}

}; // class CP57008

internal class CP57009 : ISCIIEncoding
{
	public CP57009() : base(57009, 0x0D00, "ISCII Malayalam", "x-iscii-ma") {}

}; // class CP57009

internal class CP57010 : ISCIIEncoding
{
	public CP57010() : base(57010, 0x0A80, "ISCII Gujarati", "x-iscii-gu") {}

}; // class CP57010

internal class CP57011 : ISCIIEncoding
{
	// Note: Unicode has a "Gurmukhi" page, but no "Punjabi" page.
	// Other ISCII-related information on the Internet seems to
	// indicate that they are the same.  Until I hear otherwise,
	// I will assume that they are the same thing with different
	// names - Rhys Weatherley, 16 April 2002.
	public CP57011() : base(57011, 0x0A00, "ISCII Punjabi", "x-iscii-pa") {}

}; // class CP57011

// Define the web encoding name aliases for the above code pages.

internal class ENCx_iscii_de : CP57002
{
	public ENCx_iscii_de() : base() {}

}; // class ENCx_iscii_de

internal class ENCx_iscii_be : CP57003
{
	public ENCx_iscii_be() : base() {}

}; // class ENCx_iscii_be

internal class ENCx_iscii_ta : CP57004
{
	public ENCx_iscii_ta() : base() {}

}; // class ENCx_iscii_ta

internal class ENCx_iscii_te : CP57005
{
	public ENCx_iscii_te() : base() {}

}; // class ENCx_iscii_te

internal class ENCx_iscii_as : CP57006
{
	public ENCx_iscii_as() : base() {}

}; // class ENCx_iscii_as

internal class ENCx_iscii_or : CP57007
{
	public ENCx_iscii_or() : base() {}

}; // class ENCx_iscii_or

internal class ENCx_iscii_ka : CP57008
{
	public ENCx_iscii_ka() : base() {}

}; // class ENCx_iscii_ka

internal class ENCx_iscii_ma : CP57009
{
	public ENCx_iscii_ma() : base() {}

}; // class ENCx_iscii_ma

internal class ENCx_iscii_gu : CP57010
{
	public ENCx_iscii_gu() : base() {}

}; // class ENCx_iscii_gu

internal class ENCx_iscii_pa : CP57011
{
	public ENCx_iscii_pa() : base() {}

}; // class ENCx_iscii_pa

}; // namespace System.Text