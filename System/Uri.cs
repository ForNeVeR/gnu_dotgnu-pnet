/*
 * Uri.cs - Implementation of "System.Uri".
 *
 * Copyright (C) 2002 Free Software Foundation, Inc.
 *
 * Contributed by Stephen Compall <rushing@sigecom.net>
 * Contributions by Gerard Toonstra <toonstra@ntlworld.com>
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


	// here's some of chiraz's extras
	// note that I did not make them capitalised; I do not want namespace conflict
	// use this.path instead of absolutePath
	private String          absoluteUri;  // the absolute uri to the resource as originally passed to the constructor (don't use)
	// is true if the user had already escaped the URL before it was passed
	// into the constructor. (escaped was true).
	// however, if the user did escape it, but didn't tell the constructor, it
	// is up to the system to detect whether or not it was escaped.
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
		for (newlastchar = myBaseUri.Length; myBaseUri[--newlastchar] == '/';)
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
		this.path = this.path.Replace('\\', '/').Replace("//", "/")
			.Replace("/.", "");

		if (path.IndexOf("/../") > -1 || path.EndsWith("/..")) // .. dirs present
			path = new AbsPathScanner(path).Parse();
	}

	// the following parses a path that may have the .. special directory
	// in it. It scans the folders backwards, looking for .., and if it finds it,
	// it removes the previous directory from the path. If that is a .., it removes
	// the previous 2 from the path, and so on.
	private struct AbsPathScanner
	{
		private int originalLength;
		private String[] dirs;
		private bool[] printthisdir;
		public AbsPathScanner(String path)
		{
			originalLength = path.Length;
			dirs = path.Split('/');
			printthisdir = new bool[dirs.Length];
			// TODO: find some sort of efficient SetAll
			for (int i = printthisdir.Length; --i >= 0;)
				printthisdir[i] = true;
		}
		public String Parse()
		{
			for (int curDir = dirs.Length; --curDir >= 1;)
			{
				// Remove(ref int, int) will move curDir back to next one that matters
				if (String.Equals(dirs[curDir], ".."))
					Remove(ref --curDir, 0);
			}
			StringBuilder newpath = new StringBuilder(originalLength);
			for (int i = 0; i < printthisdir.Length; ++i)
			{
				if (printthisdir[i] && dirs[i].Length > 0)
				{
					newpath.Append('/').Append(dirs[i]);
				}
			}
			return newpath.ToString();
		}
		// tbrCache signifies the number of .. instances that have built up besides
		// the current one. If >0, it should call itself.
		private void Remove(ref int toBeRemoved, int tbrCache)
		{
			if (toBeRemoved == 0)
				throw new UriFormatException(_("Arg_UriPathAbs"));
			printthisdir[toBeRemoved] = false;
			if (String.Equals(dirs[toBeRemoved], ".."))
				Remove(ref --toBeRemoved, ++tbrCache);
			else if (tbrCache > 0)
				Remove(ref --toBeRemoved, --tbrCache);
			// after recursion finishes, toBeRemoved will be the last one
			// removed, and the test in the for loop will go back to a visible dir
		}
	}


	[TODO]
	public static UriHostNameType CheckHostName(String name)
	{
		if (name == null || name.Length == 0)
			return UriHostNameType.Unknown;

		bool isDns = true;
		foreach (String tok in name.Split('.'))
		{
			if (!Char.IsLetterOrDigit(tok, 0) || !Char.IsLetterOrDigit(tok, tok.Length - 1)
			    || !CharsAreAlnumDash(tok, 1, tok.Length - 2))
			{
				isDns = false;
				break;
			}
		}
		if (isDns)
			return UriHostNameType.Dns;

		// TODO: make this more efficient (hint: IPAddress is in same assembly)
		try
		{
			System.Net.IPAddress.Parse(name);
			return UriHostNameType.IPv4;
		}
		catch (FormatException)
		{
			// not IPv4
		}
		// TODO: IPv6
		return UriHostNameType.Unknown;
	}

	// check if characters in a String in a given range are alphanumeric or -.
	private CharsAreAlnumDash(String checkthis, int first, int last)
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
		if (comparand == null || (!(comparand is String) && !(comparand is Uri)))
			return false;
		else if (comparand is String)
			rurib = new Uri((String)comparand);
		else if (comparand is Uri)
			rurib = (Uri)comparand;
		else
			return false;

		// do not check query and fragment
		// this makes the boolean
		return (String.Equals(this.Host, rurib.Host) &&
			  String.Equals(this.AbsolutePath, rurib.AbsolutePath) &&
			  String.Equals(this.Scheme, rurib.Scheme));
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
		if (part == UriPartial.Path)
			return this.ToStringNoFragQuery();

		else if (part == UriPartial.Authority)
			return String.Concat(this.scheme, this.schemeDelim(), this.Authority);

		else if (part == UriPartial.Scheme)
			return String.Concat(this.scheme, this.schemeDelim());
		else
			return null;	// TODO
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
		return (character < 0x20 || character > 0x7F ||
			"<>#%\"{}|\\^[]`".IndexOf(character) >= 0);
	}

	public static bool IsHexDigit(char character)
	{
		return
		(
			(character >= '0' && character <= '9') ||
			(character >= 'A' && character <= 'F') ||
			(character >= 'a' && character <= 'f')
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

	[TODO]
	public String MakeRelative(Uri toUri)
	{
		if (String.Equals(toUri.host, this.host))
		{
			String[] thisUri = this.path.Split('/');
			String[] otherUri = toUri.path.Split('/');
			int currentItem = 0;
			StringBuilder myStringBuilder = new StringBuilder();

			while ((thisUri.Length >= currentItem) &&
				 (otherUri.Length >= currentItem))
			{
				if (String.Equals(thisUri[currentItem], otherUri[currentItem]))
				{
					myStringBuilder.Append(otherUri[currentItem]);
					myStringBuilder.Append('/');
				}
				++currentItem;
			}

			if (currentItem == otherUri.Length)
			{
				myStringBuilder.Remove(myStringBuilder.Length - 1, 1);
			}
			else
			{
				for (int i = currentItem; i < otherUri.Length; i++)
				{
					myStringBuilder.Append(otherUri[i]);
				}
			}

			return myStringBuilder.ToString();
		}
		else
		{
			return toUri.AbsoluteUri;
		}
	}

	protected virtual void Parse()
	{
		int curpos = absoluteUri.IndexOf(':');
		int nextpos = 0;

		// Set all to nothing just in case info was left behind somewhere somehow...
		path = "";
		fragment = "";
		host = "";
		port = -1;
		query = "";
		scheme = "";
		userinfo = "";


		if (curpos > 0)
		{
			this.scheme = absoluteUri.Substring(0, curpos).ToLower();

			if (!CheckSchemeName(this.scheme))
				throw new UriFormatException(S._("Arg_UriScheme"));

			// some Uris don't use the // after scheme:
			if (String.Compare(AbsoluteUri, curpos, SchemeDelimiter, 0, 3) == 0)
				curpos += 3;
			else
				++curpos;
		}
		else
		{
			this.scheme = "http";
			this.port = 80;
		}
		// end of scheme parsing
		// curpos is now at the authority

		// put nextpos post-authority
		nextpos = absoluteUri.IndexOfAny(new char[]{'/', '?', '#'}, curpos);
		if (nextpos < 0)
			nextpos = absoluteUri.Length;

		this.ParseAuthority(absoluteUri.Substring(curpos, nextpos - curpos));
		curpos = nextpos;

		if (nextpos < absoluteUri.Length)
		{
			nextpos = absoluteUri.IndexOf('?', curpos);
			if (nextpos > 0)
			{
				// there is query
				query = absoluteUri.Substring(nextpos + 1);
				curpos = nextpos + 1;
			}
			else
			{
				nextpos = absoluteUri.IndexOf('#', curpos);
				if (nextpos > 0)
					// there is fragment
					fragment = absoluteUri.Substring(nextpos + 1);
			}
			if (nextpos == 0)
				path = absoluteUri.Substring(curpos);
			else
				path = absoluteUri.Substring(curpos, nextpos);

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
		}
	}

	// help for Parse()!
	private void ParseAuthority(String authority)
	{
		int interimpos1=0, interimpos2=0;

		interimpos1 = authority.IndexOf('@');
		if (interimpos1 > 0) // there is userinfo
		{
			userinfo = authority.Substring(0, interimpos1 - 1);
			interimpos2 = interimpos1 + 1;
		}

		interimpos1 = authority.IndexOf(':', interimpos2);
		if (interimpos1 > 0) // there is a port
		{
			this.host = authority.Substring(interimpos2, interimpos1 - interimpos2);
			try
			{
				// technically, ports are 16 bit, but...
				this.port = Int32.Parse(authority.Substring(interimpos1 + 1));
			}
			catch (FormatException fe) { this.port = -1; }
			catch (OverflowException oe)
			{
				throw new UriFormatException(S._("Arg_UriPort"));
			}
		}
		else // no port
			host = authority.Substring(interimpos2);

		// now test host, standard says must be IPv4 or DNS
		this.hostNameType = CheckHostName(host);
		if (this.hostNameType != UriHostNameType.Dns &&
			this.hostNameType != UriHostNameType.IPv4)
			throw new UriFormatException(S._("Arg_UriHostName"));
	}

	private static bool needsEscaping(String instr)
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
			++pcntSignIndex; // don't reread the character
			return path[pcntSignIndex];
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
			return (String.Equals(this.host, "localhost") ||
				// according to System.Net.IPAddress, anything in 127.X.Y.Z
				// is loopback. Maybe change this to comply.
				String.Equals(this.host, "127.0.0.1"));
		}
	}

	[TODO]
	public String LocalPath
	{
		get
		{
			if (String.Equals(this.scheme, Uri.UriSchemeFile))
			{
				// aieee! platform-dependent. Internal?
				// TODO
				return null;
			}
			else
				return this.AbsolutePath;
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
