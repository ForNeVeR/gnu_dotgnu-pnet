/*
 * UnicodeEncoding.cs - Implementation of the
 *		"System.Text.UnicodeEncoding" class.
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

namespace System.Text
{

using System;

public class UnicodeEncoding : Encoding
{
	// Magic numbers used by Windows for Unicode.
	internal const int UNICODE_CODE_PAGE     = 1200;
	internal const int BIG_UNICODE_CODE_PAGE = 1201;

	// Internal state.
	private bool bigEndian;
	private bool byteOrderMark;

	// Constructors.
	public UnicodeEncoding() : base(UNICODE_CODE_PAGE)
			{
				bigEndian = false;
				byteOrderMark = true;
			}
	public UnicodeEncoding(bool bigEndian, bool byteOrderMark)
			: base((bigEndian ? BIG_UNICODE_CODE_PAGE : UNICODE_CODE_PAGE))
			{
				this.bigEndian = bigEndian;
				this.byteOrderMark = byteOrderMark;
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
				return count * 2 + (byteOrderMark ? 2 : 0);
			}

	// Convenience wrappers for "GetByteCount".
	public override int GetByteCount(String s)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return s.Length * 2 + (byteOrderMark ? 2 : 0);
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
				if((bytes.Length - byteIndex) <
					(charCount * 2 + (byteOrderMark ? 2 : 0)))
				{
					throw new ArgumentException
						(_("Arg_InsufficientSpace"));
				}
				int posn = byteIndex;
				char ch;
				if(bigEndian)
				{
					if(byteOrderMark)
					{
						bytes[posn++] = (byte)0xFE;
						bytes[posn++] = (byte)0xFF;
					}
					while(charCount-- > 0)
					{
						// TODO: handle surrogates.
						ch = chars[charIndex++];
						bytes[posn++] = (byte)(ch >> 8);
						bytes[posn++] = (byte)ch;
					}
				}
				else
				{
					if(byteOrderMark)
					{
						bytes[posn++] = (byte)0xFF;
						bytes[posn++] = (byte)0xFE;
					}
					while(charCount-- > 0)
					{
						// TODO: handle surrogates.
						ch = chars[charIndex++];
						bytes[posn++] = (byte)ch;
						bytes[posn++] = (byte)(ch >> 8);
					}
				}
				return posn - byteIndex;
			}

	// Convenience wrappers for "GetBytes".
	public override int GetBytes(String s, int charIndex, int charCount,
								 byte[] bytes, int byteIndex)
			{
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
				if((bytes.Length - byteIndex) <
					(charCount * 2 + (byteOrderMark ? 2 : 0)))
				{
					throw new ArgumentException
						(_("Arg_InsufficientSpace"));
				}
				int posn = byteIndex;
				char ch;
				if(bigEndian)
				{
					if(byteOrderMark)
					{
						bytes[posn++] = (byte)0xFE;
						bytes[posn++] = (byte)0xFF;
					}
					while(charCount-- > 0)
					{
						// TODO: handle surrogates.
						ch = s[charIndex++];
						bytes[posn++] = (byte)(ch >> 8);
						bytes[posn++] = (byte)ch;
					}
				}
				else
				{
					if(byteOrderMark)
					{
						bytes[posn++] = (byte)0xFF;
						bytes[posn++] = (byte)0xFE;
					}
					while(charCount-- > 0)
					{
						// TODO: handle surrogates.
						ch = s[charIndex++];
						bytes[posn++] = (byte)ch;
						bytes[posn++] = (byte)(ch >> 8);
					}
				}
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
				if(count >= 2)
				{
					if((bytes[0] == (byte)0xFE && bytes[1] == (byte)0xFF) ||
					   (bytes[0] == (byte)0xFF && bytes[1] == (byte)0xFE))
					{
						return ((count - 1) / 2);
					}
				}
				return count / 2;
			}

	// Get the characters that result from decoding a byte buffer.
	public override int GetChars(byte[] bytes, int byteIndex, int byteCount,
								 char[] chars, int charIndex)
			{
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

				// Determine the byte order in the incoming buffer.
				bool isBigEndian;
				if(byteCount >= 2)
				{
					if(bytes[0] == (byte)0xFE && bytes[1] == (byte)0xFF)
					{
						isBigEndian = true;
						byteCount -= 2;
						byteIndex += 2;
					}
					else if(bytes[0] == (byte)0xFF && bytes[1] == (byte)0xFE)
					{
						isBigEndian = false;
						byteCount -= 2;
						byteIndex += 2;
					}
					else
					{
						isBigEndian = bigEndian;
					}
				}
				else
				{
					isBigEndian = bigEndian;
				}

				// Validate that we have sufficient space in "chars".
				if((chars.Length - charIndex) < (byteCount / 2))
				{
					throw new ArgumentException
						(_("Arg_InsufficientSpace"));
				}

				// Convert the characters.
				int posn = 0;
				if(isBigEndian)
				{
					while(byteCount >= 2)
					{
						// TODO: handle surrogates.
						chars[posn++] =
							((char)((((int)(bytes[byteIndex])) << 8) |
									 ((int)(bytes[byteIndex + 1]))));
						byteIndex += 2;
						byteCount -= 2;
					}
				}
				else
				{
					while(byteCount >= 2)
					{
						// TODO: handle surrogates.
						chars[posn++] =
							((char)((((int)(bytes[byteIndex + 1])) << 8) |
									 ((int)(bytes[byteIndex]))));
						byteIndex += 2;
						byteCount -= 2;
					}
				}
				return posn;
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
				return charCount * 2 + (byteOrderMark ? 2 : 0);
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
				return byteCount / 2;
			}

	// Get a Unicode-specific decoder that is attached to this instance.
	public override Decoder GetDecoder()
			{
				// TODO
				return base.GetDecoder();
			}

	// Get the Unicode preamble.
	public override byte[] GetPreamble()
			{
				byte[] preamble = new byte[2];
				if(bigEndian)
				{
					preamble[0] = (byte)0xFE;
					preamble[1] = (byte)0xFF;
				}
				else
				{
					preamble[0] = (byte)0xFF;
					preamble[1] = (byte)0xFE;
				}
				return preamble;
			}

}; // class UnicodeEncoding

}; // namespace System.Text
