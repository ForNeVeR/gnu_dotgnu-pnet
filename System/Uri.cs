/*
 * Uri.cs - Implementation of "System.Uri".
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
 * Copyright (C) 2002  Gerard Toonstra.
 * Copyright (C) 2002  Rich Baumann.
 *
 * Contributed by Stephen Compall <rushing@sigecom.net>
 * Contributions by Gerard Toonstra <toonstra@ntlworld.com>
 * Contributions by Rich Baumann <biochem333@nyc.rr.com>
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

namespace System
{

using System.Text;
using System.IO;

/*
global TODO:
* adapt to use the authority struct
*/

public class Uri : MarshalByRefObject
{

	// the capital letters are between 0x41 and 0x5A
	// the lowercase letters are between 0x61 and 0x7A
	// the numbers are between 0x30 and 0x39
	// .=2E +=2B -=2D
	// beware, for they are valid scheme chars
	// now I don't need the string anymore FOR VALIDSCHEMECHARS

	// magic strings...
	public static readonly String SchemeDelimiter = "://";
	public static readonly String UriSchemeFile = "file";
	public static readonly String UriSchemeFtp = "ftp";
	public static readonly String UriSchemeGopher = "gopher";
	public static readonly String UriSchemeHttp = "http";
	public static readonly String UriSchemeHttps = "https";
	public static readonly String UriSchemeMailto = "mailto";
	public static readonly String UriSchemeNews = "news";
	public static readonly String UriSchemeNntp = "nntp";

	// end magic strings

	// state. mostly the same as UriBuilder


	// here are some of chiraz's extras
	// note that I did not make them capitalised; I do not want
	// namespace conflict
	// use this.path instead of absolutePath
	private String          absoluteUri;  // the absolute uri to the
					      // resource as originally
					      // passed to the constructor
					      // (don't use)
	// is true if the user had already escaped the URL before it was
	// passed into the constructor. (escaped was true).
	// however, if the user did escape it, but didn't tell the
	// constructor, it is up to the system to detect whether or not
	// it was escaped.
	private bool		userEscaped;
	private UriHostNameType hostNameType;
	// authority, isdefaultport, isloopback

	// Holds the scheme information. (search 0->:)
	// doesn't contain ://
	private String scheme;

	// authority = userinfo+host+port
	// userinfo = username:password
	private String userinfo;
	// host does not contain the port
	private String host;
	// if IsDefaultPort, don't need to print! -1 for none
	private int port;

	// technically optional, but they want a path :)
	// contains the slash
	private String path;

	// doesn't contain the ? mark
	private String query;

	// remember: this is not part of the uri
	// doesn't contain the #
	private String fragment;
	// also known as hash

	// end of state

	// Constructors.
	public Uri(String uriString) : this(uriString, false)
	{
	}

	public Uri(String uriString, bool dontEscape)
	{
		if (uriString == null)
		{
			throw new ArgumentNullException("uriString");
		}

		userEscaped = dontEscape;
		this.absoluteUri = uriString.Trim();

		this.Parse();
		this.Canonicalize();
	}

	public Uri(Uri baseUri, String relativeUri) : this(baseUri, relativeUri, false)
	{
	}

	public Uri(Uri baseUri, String relativeUri, bool dontEscape)
	{
		if (baseUri== null)
			throw new ArgumentNullException("baseUri");

		if (relativeUri== null)
			throw new ArgumentNullException("relativeUri");

		// Making local copies that we use for modification
		String myBaseUri = baseUri.AbsoluteUri.Trim();
		String myRelativeUri = relativeUri.Trim();
		userEscaped = dontEscape;

		int newlastchar;
		for (newlastchar = myBaseUri.Length;
		     myBaseUri[--newlastchar] == '/';)
			; // empty body
		myBaseUri = myBaseUri.Substring(0, newlastchar + 1);

		for (newlastchar = -1; myRelativeUri[++newlastchar] == '/';)
			; // empty body
		myRelativeUri = myRelativeUri.Substring(newlastchar);

		this.absoluteUri = String.Concat(myBaseUri, "/", myRelativeUri);

		this.Parse();
		this.Canonicalize();
	}

	// methods
	protected virtual void Canonicalize()
	{
		// TODO: `replace' this with something more efficient, i.e.
		// scan like Flex and output to a StringBuilder(path.Length)
		this.path = this.path.Replace('\\', '/');
		while (this.path.IndexOf("//") >= 0) // double-slashes to strip
		{
			this.path = this.path.Replace("//", "/");
		}

		// find out if .. dirs are present
		if (path.IndexOf("/../") > -1 || path.EndsWith("/..")
		    || path.IndexOf("/./") > -1 || path.EndsWith("/.")) 
		{
			path = StripMetaDirectories(path);
		}

		// remove the slash at the end, unless it's alone
		int psize = path.Length; // efficiency
		if (psize > 1)
		{
			if (path[psize-1] == '/')
				path = path.Substring(0, psize-1);
		}
		else
			path = "/";
	}

	// The following takes . or .. directories out of an absolute
	// path. Throws UriFormatException if the ..s try to extend
	// beyond the root dir.
	private static String StripMetaDirectories(String oldpath)
	{
		// use toBeRemoved when you detect a .., and need to
		// remove previous directories because of it. i.e.,
		// /gd/gnuorg/../.. will (going backwards) make
		// toBeRemoved=2 on the .. couple, then decrement it
		// by deleting gnuorg and gd
		int toBeRemoved = 0;

		// in abspath, dirs[0] is ""
		String[] dirs = oldpath.Split('/');

		// the scanner will set not-shown to "" to make the
		// tests at the end faster
		// scan all but 0
		for (int curDir = dirs.Length; --curDir >= 1;)
		{
			if (dirs[curDir] == "..")
			{
				++toBeRemoved;
				// removed w/o affecting toBeRemoved
				dirs[curDir] = "";
			}
			else if (dirs[curDir] == ".")
				dirs[curDir] = ""; // doesn't affect anything
			else if (toBeRemoved > 0) // remove this one
			{
				--toBeRemoved;
				dirs[curDir] = "";
			}
			// if normal state (no .., normal dir) do nothing
		}

		if (toBeRemoved > 0) // wants to delete root
			throw new UriFormatException
			  (S._("Arg_UriPathAbs"));

		StringBuilder newpath = new StringBuilder(oldpath.Length);
		foreach (String dir in dirs)
			if (dir.Length > 0) // visible?
				newpath.Append('/').Append(dir);
		if (newpath.Length == 0)
			newpath.Append('/');
		return newpath.ToString();
	}

	public static UriHostNameType CheckHostName(String name)
	{
		if (name == null || name.Length == 0)
			return UriHostNameType.Unknown;

		bool isDns = true;
		foreach (String tok in name.Split('.'))
		{
			if (!Char.IsLetterOrDigit(tok, 0)
			    || !Char.IsLetterOrDigit(tok, tok.Length - 1)
			    || !CharsAreAlnumDash(tok, 1, tok.Length - 2))
			{
				isDns = false;
				break;
			}
		}
		if (isDns)
			return UriHostNameType.Dns;

		// TODO: make this more efficient (hint: IPAddress is in
		// same assembly)
		try
		{
			System.Net.IPAddress.Parse(name);
			return UriHostNameType.IPv4;
		}
		catch (FormatException)
		{
			// not IPv4
		}

		// IPv6, see http://search.ietf.org/internet-drafts/draft-ietf-ipngwg-addr-arch-v3-09.txt
		// section 2.2, page 4, for my source in implementation
		// TODO: change to some IPng class/struct
		try
		{
			String[] parts = name.Split(':');
			int dex = parts.Length;

			// 128-bit. The spec allows the last two words
			// to be specified by the old IPv4 method
			// can't be more than 8 parts
			if (parts.Length > 8)
				throw new FormatException();

			// presence of . in the last element means it
			// *must* be the special IPv4
			bool usingIPv4maybe = false;
			if (parts[parts.Length - 1].IndexOf('.') >= 0)
			{
				// only 6 to check now
				dex -= 2;
				usingIPv4maybe = true;
			}

			// *once* in an IPv6 addr, you can specify 0
			// across a range by leaving the part blank,
			// i.e. ::0, ::, F1F0::207.241.30.1
			bool usedZeroCompress = false;
			while (--dex >= 0) // check all hexes
			{
				int pos = 0;
				// handle special case to avoid parsing
				// if we had a zerocompress in pos 1
				if (dex == 0
				    && parts[dex].Length == 0
				    && parts[dex].Length == 0)
					continue;

				// check for 0 compress
				if (parts[dex].Length == 0 && dex != 0)
				{
					// may only use once
					if (usedZeroCompress)
						throw new FormatException();
					else
						usedZeroCompress = true;
				}

				parseHexWord(parts[dex], ref pos);

				// if parseHexWord doesn't move pos to
				// the end, there are bad chars
				if (pos != parts[dex].Length)
					throw new FormatException();
			}

			// we should have had enough items!
			if (!usedZeroCompress
			    && ((usingIPv4maybe && parts.Length < 7)
				|| (!usingIPv4maybe && parts.Length < 8)))
				throw new FormatException();

			// check the last element if believed IPv4
			if (usingIPv4maybe)
				// will throw FormatException if bad
				System.Net.IPAddress.Parse(parts[parts.Length - 1]);

			// if we got this far, it's fine
			return UriHostNameType.IPv6;
		}
		catch (FormatException)
		{
			// not IPv6
		}

		return UriHostNameType.Unknown;
	}

	// check if characters in a String in a given range are
	// alphanumeric or -.
	private static bool CharsAreAlnumDash(String checkthis,
					      int first,
					      int last)
	{
		char check;
		for (; first <= last; ++first)
		{
			check = checkthis[first];
			if (!Char.IsLetterOrDigit(check) && check != '-')
				return false;
		}
		return true;
	}

	// Takes a location in a string, and returns a ushort if it can be
	// hex-parsed into a, well, ushort. Throws FormatException if index
	// doesn't point to a hex char (will fix index).
	private static ushort parseHexWord(String src, ref int index)
	{
		int buildretval = 0;
		int stop;
		for (stop = index + 4; index < stop; ++index)
		{
			if (!IsHexDigit(src[index]))
				break;
			buildretval <<= 4;
			buildretval |= FromHex(src[index]);
		}
		if (stop != index)
			throw new FormatException(S._("Arg_HexDigit"));
		return (ushort)buildretval;
	}

	public static bool CheckSchemeName(String schemeName)
	{
		if (schemeName == null || schemeName.Length == 0)
			return false;

		char charloc = schemeName[0];
		if (charloc < 'a' && charloc > 'z' &&
		    charloc < 'A' && charloc > 'Z')
			return false;
		for (int i = 0; ++i < schemeName.Length;) // starts with 1
		{
			if (!Uri.isValidSchemeChar(schemeName[i]))
				return false;
		}
		return true;
	}

	// support for above method
	private static bool isValidSchemeChar(char character)
	{
		return (
		// check letters
		(character >= 'a' && character <= 'z') ||
		(character >= 'A' && character <= 'Z') ||
		// check numbers
		(character >= '0' && character <= '9') ||
		// check the other three
		character == '.' || character == '+' || character == '-'
		);
	}

	protected virtual void CheckSecurity()
	{
		// do nothing in base class
	}

	public override bool Equals(Object comparand)
	{
		Uri rurib;
		if (comparand == null
		    || (!(comparand is String)
			&& !(comparand is Uri)))
			return false;
		else if (comparand is String)
			rurib = new Uri((String)comparand);
		else if (comparand is Uri)
			rurib = (Uri)comparand;
		else
			return false;

		// do not check query and fragment
		// this makes the boolean
		return (String.Equals(this.Host, rurib.Host)
			&& String.Equals(this.AbsolutePath,
					 rurib.AbsolutePath)
			&& String.Equals(this.Scheme, rurib.Scheme));
	}

	protected virtual void Escape()
	{
		this.path = EscapeString(this.path);
	}

	protected static String EscapeString(String str)
	{
		if (str == null || str.Length == 0)
			return "";

		// assume that all characters are OK for escaping
		// must change code for editable URI
		// also, does not see if string already escaped
		char chk;
		StringBuilder ret = new StringBuilder(str.Length);
		for (int i = 0; i < str.Length; i++)
		{
			chk = str[i];
			if (IsExcludedCharacter(chk) || IsReserved(chk))
				ret.Append(HexEscape(chk));
			else
				ret.Append(chk);
		}
		return ret.ToString();
	}

	public static int FromHex(char digit)
	{
		if (digit >= '0' && digit <= '9')
			return digit - '0';
		else if (digit >= 'A' && digit <= 'F')
			return digit - 55;
		else if (digit >= 'a' && digit <= 'f')
			return digit - 87;
		else
			throw new ArgumentException(S._("Arg_HexDigit"), "digit");
	}

	public override int GetHashCode()
	{
		String full = this.ToString();
		int hash = full.IndexOf('#');
		if (hash == -1)
			return full.GetHashCode();
		else
			return full.Substring(0, hash).GetHashCode();
	}

	public String GetLeftPart(UriPartial part)
	{
		switch (part)
		{
		case UriPartial.Path:
			return this.ToStringNoFragQuery();
		case UriPartial.Authority:
			return String.Concat(this.scheme,
					     this.schemeDelim(),
					     this.Authority);
		case UriPartial.Scheme:
			return String.Concat(this.scheme,
					     this.schemeDelim());
		default:
			throw new ArgumentException(S._("Arg_UriPartial"));
		}
	}

	// gets proper delimiter for current scheme
	private String schemeDelim()
	{
		if (String.Equals(this.scheme, "mailto"))
			return ":";
		else
			return "://";
	}

	public static String HexEscape(char character)
	{
		if (character > 255)
			throw new ArgumentOutOfRangeException("character");
		char[] maker = new char[3];
		maker[0] = '%';
		maker[1] = HexForIndex(character >> 4);
		// 0b00001111 == 0x0F == 15
		maker[2] = HexForIndex(character & 15);
		return new String(maker);
	}

	// support for above method, no error checking
	private static char HexForIndex(int index)
	{
		if (index <= 9)
			return (char)(index + '0');
		else
			return (char)(index + 55);
	}

	public static char HexUnescape(String pattern, ref int index)
	{
		char mychar;

		if ((pattern.Length < (index + 3)) || (index < 0))
		{
			throw new ArgumentOutOfRangeException();
		}
		if (IsHexEncoding(pattern, index))
		{
			if (pattern[index+1] >= 0x41)
			{
				mychar = (char)(pattern[index+1] - 0x41 + 10);
			}
			else if (pattern[index+1] >= 0x61)
			{
				mychar = (char)(pattern[index+1] - 0x61 + 10);
			}
			else
			{
				mychar = (char)(pattern[index+1] - 0x30);
			}

			mychar = (char)(mychar << 4);

			if (pattern[index+2] >= 0x41)
			{
				mychar = (char)(mychar +pattern[index+2] - 0x41 + 10);
			}
			else if (pattern[index+1] >= 0x61)
			{
				mychar = (char)(mychar + pattern[index+2] - 0x61 + 10);
			}
			else
			{
				mychar = (char)(mychar + pattern[index+2] - 0x30);
			}

			return mychar;
		}
		else
		{
			return pattern[index];
		}
	}

	protected virtual bool IsBadFileSystemCharacter(char character)
	{
		return (Array.IndexOf(Path.InvalidPathChars, character)
			>= 0);
	}

	protected static bool IsExcludedCharacter(char character)
	{
		return (character < 0x20 || character > 0x7F
			|| "<>#%\"{}|\\^[]`".IndexOf(character) >= 0);
	}

	public static bool IsHexDigit(char character)
	{
		return
		(
			(character >= '0' && character <= '9')
			|| (character >= 'A' && character <= 'F')
			|| (character >= 'a' && character <= 'f')
		);
	}

	public static bool IsHexEncoding(String pattern, int index)
	{
		if (pattern.Length - index >= 3)
			return ((pattern[index] == '%') &&
			    IsHexDigit(pattern[index+1]) &&
			    IsHexDigit(pattern[index+2]));
		else
			return false;
	}

	// ECMA specifies that "IsReservedCharacter" is virtual,
	// even though it doesn't make much sense.  We need the
	// method in some static contexts, so we define this
	// private version also.
	private static bool IsReserved(char character)
	{
		return (";/:@&=+$,".IndexOf(character) >= 0);
	}

	protected virtual bool IsReservedCharacter(char character)
	{
		return IsReserved(character);
	}

	// TODO: test
	// this should return a string of the argument uri's address, relative to this uri's address
	// (Rich Baumann - biochem333@nyc.rr.com)
	[TODO]
	public String MakeRelative(Uri toUri)
	{
		if (this.host.Equals(toUri.host))
		{
			if (this.path.Equals(toUri.path)) { return ""; } // return empty string... URIs are identical

			String[] thisUri = this.path.Split('/');  // split the path up at the / chars
			String[] otherUri = toUri.path.Split('/'); // now make tokens for the other uri

			int currentItem = 0; // loop var
			StringBuilder myStringBuilder = new StringBuilder(toUri.path.Length); // temp var (return)

			while ((currentItem < thisUri.Length) && (currentItem < otherUri.Length))
			{
				if (!(thisUri[currentItem].Equals(otherUri[currentItem]))) // check for uri deviations
				{
					break; // if not equal, we've found deviation
				}
				++currentItem;
			}

			// this part assumes that blah/blah/ is never given as blah/blah and vice-versa
			// if this assumption is in error, I don't see how to figure out how many ../ are needed
			bool thisFile = !(this.path.EndsWith("/")); // ends with a file...
			bool otherFile = !(toUri.path.EndsWith("/")); // ...or a path segment

			int tmp = thisUri.Length - currentItem - (thisFile ? 2 : 1); // calculate # of ../ needed
			int tmp2 = otherUri.Length - currentItem - 1; // calculate # of tokens left in otherUri

			for (int i = 0; i < tmp; i++) { myStringBuilder.Append("../"); } // add needed ../

			for (int i = 0; i < tmp2; i++, currentItem++) // go through all remaining otherUri tokens
			{
				myStringBuilder.Append(otherUri[currentItem]); // add next part of path
				myStringBuilder.Append('/'); // add path separator
			}

			if (otherFile) { myStringBuilder.Remove(myStringBuilder.Length - 1, 1); } // ends with a file... strip last /

			return myStringBuilder.ToString(); // return relative
		}
		else
		{
			return toUri.AbsoluteUri; // return absolute... URIs are on on different hosts
		}
	}

	protected virtual void Parse()
	{
		int curpos = absoluteUri.IndexOf(':');
		int nextpos = 0;

		// Set all to nothing just in case info was left behind
		// somewhere somehow... TODO remove
		path = "";
		fragment = "";
		host = "";
		port = -1;
		query = "";
		scheme = "";
		userinfo = "";


		if (curpos > 0) // scheme specified (or a port delim)
		{
			String maybescheme = absoluteUri.Substring(0, curpos).ToLower();

			// not giving a scheme is equivalent to "http"
			if (!CheckSchemeName(maybescheme))
			{
				try
				{
					new UriAuthority(this.scheme);
				}
				catch (UriFormatException) // not authority
				{
					throw new UriFormatException
					  (S._("Arg_UriScheme"));
				}
				// else is authority
				this.scheme = "http";
				this.port = 80;
			}
			else // ok, it's a real scheme
			{
				// some Uris don't use the // after scheme:
				if (String.Compare(AbsoluteUri, curpos,
						   SchemeDelimiter, 0, 3) == 0)
					curpos += 3;
				else
					++curpos;
			}
		}
		else // scheme not specified
		{
			this.scheme = "http";
			this.port = 80;
		}
		// end of scheme parsing
		// curpos is now at the authority

		// put nextpos post-authority
		nextpos = absoluteUri.IndexOfAny(new char[]{'/', '?', '#'},
						 curpos);
		if (nextpos < 0)
			nextpos = absoluteUri.Length;

		// even if the "scheme" was an authority, we have to
		// redo because we cut off the potential port (:)
		UriAuthority tryauth = new UriAuthority
		  (absoluteUri.Substring(curpos, nextpos - curpos));

		this.userinfo = tryauth.userinfo;
		this.host = tryauth.hostname;
		this.port = tryauth.port;
		this.hostNameType = tryauth.hosttype;
		curpos = nextpos;

		if (nextpos < absoluteUri.Length) // implies curpos also
		{
			nextpos = absoluteUri.IndexOf('?', curpos);
			if (nextpos >= 0)
			{
				// there is query mark
				query = absoluteUri.Substring(nextpos + 1);
			}
			else
			{
				nextpos = absoluteUri.IndexOf('#', curpos);
				if (nextpos >= 0) // there is fragment
					fragment = absoluteUri.Substring(nextpos + 1);
			}
			if (nextpos == -1) // no path nor query
				path = absoluteUri.Substring(curpos);
			else
				path = absoluteUri.Substring(curpos, nextpos - curpos);
		}

		if (!userEscaped)
		{
			if (needsEscaping(path))
				// Escape() only affects path
				this.Escape();
			if (needsEscaping(query))
				query = EscapeString(query);
			if (needsEscaping(fragment))
				fragment = EscapeString(fragment);
		}
		else // user should have escaped
		{
			if (needsEscaping(path)
			    || needsEscaping(query)
			    || needsEscaping(fragment))
				throw new UriFormatException(S._("Arg_UriNotEscaped"));
		}

	}

	public override String ToString()
	{
		StringBuilder myStringBuilder = new StringBuilder(absoluteUri.Length);

		myStringBuilder.Append(this.scheme);

		myStringBuilder.Append(this.schemeDelim());

		if (this.userinfo.Length > 0)
		{
			myStringBuilder.Append(this.userinfo);
			myStringBuilder.Append('@');
		}

		myStringBuilder.Append(host);

		if (this.port >= 0)
		{
			myStringBuilder.Append(':');
			myStringBuilder.Append(this.port);
		}

		myStringBuilder.Append(PathAndQuery);

		if (this.fragment.Length > 0)
		{
			myStringBuilder.Append('#');
			myStringBuilder.Append(this.fragment);
		}

		return Unescape(myStringBuilder.ToString());
	}

	protected virtual String Unescape(String path)
	{
		StringBuilder retStr = new StringBuilder(path.Length);
		int afterPrevPcntSignIndex = 0;

		for (int lastPcntSignIndex = path.IndexOf('%'); lastPcntSignIndex >= 0;
			// String.IndexOf allows up to String.Length to be the startIndex
			// so if this throws, it is an error
			lastPcntSignIndex = path.IndexOf('%', lastPcntSignIndex))
		{
			// append string up to % sign
			retStr.Append(path, afterPrevPcntSignIndex, lastPcntSignIndex-afterPrevPcntSignIndex);
			// get the hex character, or just %, and append
			retStr.Append(HexUnescapeWithUTF8(path, ref lastPcntSignIndex));
			afterPrevPcntSignIndex = lastPcntSignIndex;
		}
		// then push on the rest of the string
		return retStr.Append(path, afterPrevPcntSignIndex,
							 path.Length - afterPrevPcntSignIndex).ToString();
		// and return it
	}

	private static char HexUnescapeWithUTF8(String path, ref int pcntSignIndex)
	{
		if (IsHexEncoding(path, pcntSignIndex))
		{
			char c1 = HexUnescape(path, ref pcntSignIndex); // changes pcntSignIndex

			switch (UTF8SizeFor1stByte(c1))
			{
			case 2:
				if (IsHexEncoding(path, pcntSignIndex)) // 2nd byte is Hex encoding
				{
					int psiCopy = pcntSignIndex; // save in case not 2-byte UTF8
					char c2 = HexUnescape(path, ref psiCopy);
					if ((c2 & 0xC0) == 0x80) // is UTF8 2-byte?
					{
						pcntSignIndex = psiCopy;
						return (char)(((c1 & 0x1F) << 6) | (c2 & 0x3F)); // build
					}
				}
				// else all
				goto default;
			case 3:
				if (path.Length - pcntSignIndex >= 6 && IsHexEncoding(path, pcntSignIndex)
					&& IsHexEncoding(path, pcntSignIndex+3)) // 2nd and 3rd bytes are hex encoded
				{
					int psiCopy = pcntSignIndex; // save again
					// psiCopy will change to compensate
					char c2 = HexUnescape(path, ref psiCopy), c3 = HexUnescape(path, ref psiCopy);
					if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80) // is UTF8 3-byte?
					{
						pcntSignIndex = psiCopy;
						return (char)(((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6)
							| (c3 & 0x3F)); // build
					}
				}
				// else all
				goto default;
			default:
				return c1;
			} // switch
		}
		else // following % sign is not hex
		{
			return path[pcntSignIndex++]; // don't reread char, return % (or whatever)
		}
	}

	private static int UTF8SizeFor1stByte(char c)
	{
		if ((c & 0x80) == 0)
			return 1;
		else if ((c & 0xE0) == 0xC0)
			return 2;
		else if ((c & 0xF0) == 0xE0)
			return 3;
		else
			return 0;
	}

	// properties
	public String AbsolutePath
	{
		get
		{
			return this.path;
		}
	}

	public String AbsoluteUri
	{
		get
		{
			return this.absoluteUri;
		}
	}

	public String Authority
	{
		get
		{
			StringBuilder authret = new StringBuilder();
			if (this.userinfo.Length > 0)
				authret.Append(this.userinfo).Append('@');
			authret.Append(host);
			if (!this.IsDefaultPort)
				authret.Append(':').Append(this.port);
			return authret.ToString();
		}
	}

	public String Fragment
	{
		get
		{
			if (this.fragment.Length == 0)
				return this.fragment;
			else
				return String.Concat("#", this.fragment);
		}
	}

	public String Host
	{
		get
		{
			return this.host;
		}
	}

	public UriHostNameType HostNameType
	{
		get
		{
			return this.hostNameType;
		}
	}

	public bool IsDefaultPort
	{
		get
		{
			try
			{
				return (this.port == -1 || this.port == Uri.DefaultPortForScheme(this.scheme));
			}
			catch (ArgumentException ae)
			{
				return false;
			}
		}
	}

	public bool IsFile
	{
		get
		{
			return String.Equals(this.scheme, Uri.UriSchemeFile);
		}
	}

	public bool IsLoopback
	{
		get
		{
			return (String.Equals(this.host, "localhost")
				// according to System.Net.IPAddress,
				// anything in 127.X.Y.Z
				// is loopback. Maybe change this to comply.
				|| String.Equals(this.host, "127.0.0.1"));
		}
	}

	public String LocalPath
	{
		get
		{
			if (String.Equals(this.scheme, Uri.UriSchemeFile) &&
			    Path.DirectorySeparatorChar != '/')
				return this.path.Replace('/', Path.DirectorySeparatorChar);
			else
				return this.path;
		}
	}

	public String PathAndQuery
	{
		get
		{
			String abspath = this.AbsolutePath;
			if (String.Equals(abspath, ""))
				return this.Query;
			else if (String.Equals(this.query, ""))
				return abspath;
			else
				return String.Concat(this.path, "?", this.query);
		}
	}

	public int Port
	{
		get
		{
			if (this.port > -1)
				return this.port;
			else
			{
				try
				{
					return Uri.DefaultPortForScheme(this.scheme);
				}
				catch (ArgumentException ae)
				{
					// also means don't know
					return -1;
				}
			}
		}
	}

	public String Query
	{
		get
		{
			// gets with the ?
			if (this.query == "")
				return this.query;
			else
				return String.Concat("?", this.query);
		}
	}

	public String Scheme
	{
		get
		{
			return this.scheme;
		}
	}

	public bool UserEscaped
	{
		get
		{
			return this.userEscaped;
		}
	}

	public string UserInfo
	{
		get
		{
			return this.userinfo;
		}
	}


	// my junk

	// to group the authority stuff together
	private struct UriAuthority
	{
		public String userinfo, hostname;
		public int port;
		public UriHostNameType hosttype;

		public UriAuthority(String authority)
		{
			int interimpos1=0, interimpos2=0;

			// check for userinfo delimiter
			interimpos1 = authority.IndexOf('@');
			if (interimpos1 > 0)
			{
				this.userinfo = authority.Substring
				  (0, interimpos1 - 1);

				interimpos2 = interimpos1 + 1;
			}

			// check remainder for an explicit port
			interimpos1 = authority.IndexOf(':', interimpos2);
			if (interimpos1 > 0)
			{
				this.hostname = authority.Substring
					(interimpos2, interimpos1
					 - interimpos2);
				try
				{
					// technically, ports are 16 bit,
					// but...
					this.port = Int32.Parse
					  (authority.Substring
					   (interimpos1 + 1));
				}
				// this is not a real port, just use
				// default
				catch (FormatException fe)
				{
					this.port = -1;
				}
				// the number is too big
				catch (OverflowException oe)
				{
					throw new UriFormatException
					  (S._("Arg_UriPort"));
				}
			}
			else // no port indicated
			{
				// use rest of string
				this.hostname = authority.Substring
				  (interimpos2);

				this.port = -1;
			}

			// now test host, standard says must be IPv4 or DNS
			this.hosttype = CheckHostName(this.hostname);
			if (this.hosttype != UriHostNameType.Dns
			    && this.hosttype != UriHostNameType.IPv4)
			{
				throw new UriFormatException
				  (S._("Arg_UriHostName"));
			}
		}
	}

	// use this to get the default port for the scheme
	// makes it really easy to add support for new schemes
	// just use a switch/case or something in implementation
	internal static int DefaultPortForScheme(String scheme)
	{
		// We have to do this with if statements because switch
		// cannot use "readonly" fields as case labels.
		if(scheme == Uri.UriSchemeFile)
			return -1;
		else if(scheme == Uri.UriSchemeFtp)
			return 21;
		else if(scheme == Uri.UriSchemeGopher)
			return 70;
		else if(scheme == Uri.UriSchemeHttp)
			return 80;
		else if(scheme == Uri.UriSchemeHttps)
			return 443;
		else if(scheme == Uri.UriSchemeMailto)
			return 25;
		else if(scheme == Uri.UriSchemeNews)
			return 119;
		else if(scheme == Uri.UriSchemeNntp)
			return 119;
		else
			throw new ArgumentException();
	}

	// for use by UriBuilder
	internal static String impl_EscapeString(String str)
	{
		return EscapeString(str);
	}

	internal static bool needsEscaping(String instr)
	{
		char c;
		for (int i = 0; i < instr.Length; i++)
		{
			c = instr[i];
			if (IsExcludedCharacter(c) || IsReserved(c))
				return true;
		}
		return false;
	}

	// for use by comparators, which want none of this
	internal String ToStringNoFragQuery()
	{
		return Uri.ToStringNoFragQuery(this.ToString());
	}

	internal static String ToStringNoFragQuery(String uri)
	{
		int queryloc = uri.IndexOf('?');
		int hashloc = uri.IndexOf('#');
		if (queryloc <= hashloc && queryloc != -1)
			return uri.Substring(0, queryloc);
		else if (hashloc > -1)
			return uri.Substring(0, hashloc);
		else
			return uri;
	}
}; // class Uri

}; // namespace System
