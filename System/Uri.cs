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

// global TODO:
// find where the Trim(String) function is
// go through the comparison code, do not assume escaping (possibly parse always)

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
	// authority, hostnametype, isdefaultport, isloopback,

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
		this.absoluteUri = Trim(uriString);

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
	String myBaseUri = Trim(baseUri.AbsoluteUri);
	String myRelativeUri = Trim(relativeUri);
	UserEscaped = dontEscape;

	int newlastchar;
	for (newlastchar = myBaseUri.Length; myBaseUri[--newlastchar] == '/';)
		; // empty body
	myBaseUri = myBaseUri.Substring(0, newlastchar + 1);

	for (newlastchar = -1; myRelativeUri[++newlastchar] == '/')
		; // empty body
	myRelativeUri = myRelativeUri.Substring(newlastchar);

	this.AbsoluteUri = String.Concat(myBaseUri, "/", myRelativeUri);

	this.Parse();
	this.Canonicalize();
	}

	// methods
	[TODO]
	protected virtual void Canonicalize()
	{
		int pos = 0;

		if (String.Equals(this.Scheme, "file"))
		{
			// TODO: convert file to platform based file reference
		}

		this.path = this.absolutePath.Replace('\\', '/').Replace("//", "/")
			.Replace("/../", "/").Replace("/./", "/");
	}

	[TODO]
	public static UriHostNameType CheckHostName(String name)
	{
		if (schemeName == null || schemeName.Length == 0)
			return UriHostTypeName.Unknown;

		// TODO: more stuff here
	}

	public static bool CheckSchemeName(String schemeName)
	{
		if (schemeName == null || schemeName.Length == 0)
			return false;

		char charloc = schemeName[0];
		if (charloc < 'a' && charloc > 'z' &&
		    charloc < 'A' && charloc > 'Z')
			return false;
		for (int i = 1; ++i < schemeName.Length;)
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
		if (comparand == null || (comparand is not String && comparand is not Uri))
			return false;
		else if (comparand is String)
			rurib = new Uri((String)comparand);
		else if (comparand is Uri)
			rurib = (Uri)comparand;
		else
			return false;

		// do not check query and fragment
		// this makes the boolean
		return (String.Equals(this.Host, myUri.Host) &&
			  String.Equals(this.AbsolutePath, myUri.AbsolutePath) &&
			  String.Equals(this.Scheme, myUri.Scheme));
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
		for (int i = 0; ++i < str.Length;)
		{
			chk = str[i];
			if (IsExcludedCharacter(chk) || IsReservedCharacter(chk))
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
			throw new ArgumentException(_("Arg_HexDigit"), "digit");
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

	[TODO]
	public String GetLeftPart(UriPartial part)
	{
		if (part == UriPartial.Path)
			return this.ToStringNoFragQuery();

		// TODO: other UriPartials
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
	private static char HexForIndex(char index)
	{
		if (index <= 9)
			return index + '0';
		else
			return index + 55;
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
				mychar = pattern[index+1] - 0x41 + 10;
			}
			else if (pattern[index+1] >= 0x61)
			{
				mychar = pattern[index+1] - 0x61 + 10;
			}
			else
			{
				mychar = pattern[index+1] - 0x30;
			}

			mychar = mychar << 4;

			if (pattern[index+2] >= 0x41)
			{
				mychar = mychar +pattern[index+2] - 0x41 + 10;
			}
			else if (pattern[index+1] >= 0x61)
			{
				mychar = mychar + pattern[index+2] - 0x61 + 10;
			}
			else
			{
				mychar = mychar + pattern[index+2] - 0x30;
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
		return ((pattern[index] == '%') &&
		    IsHexDigit(pattern[index+1]) &&
		    IsHexDigit(pattern[index+2]));
	}

	protected virtual bool IsReservedCharacter(char character)
	{
		return (";/:@&=+$,".IndexOf(character) >= 0);
	}

	[TODO]
	public String MakeRelative(Uri toUri)
	{
		if (String.Equals(toUri.Host, this.Host))
		{
			String thisUri[] = this.AbsolutePath.Split('/');
			String otherUri[] = toUri.AbsolutePath.Split('/');
			int currentItem = 0;
			StringBuilder myStringBuilder = new StringBuilder();

			while ((thisUri.Length >= currentItem) &&
				 (otherUri.Length >= currentItem))
			{
				if (String.Compare(thisUri[currentItem], otherUri[currentItem) == 0)
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

	[TODO]
	protected virtual void Parse()
	// TODO: fix for current private property behavior
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
				throw new UriFormatException(_("Arg_UriScheme"));

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
					fragment = EscapeString(fragment)
			}

			// now, do verification of fields
		}
	}

	// help for Parse()!
	private void ParseAuthority(String authority)
	{
		int interimpos1=0, interimpos2=0;

		interimpos1 = authority.IndexOf('@');
		if (interimpos1 > 0)
		{
			userinfo = authority.Substring(0, interimpos1 - 1);
			interimpos2 = interimpos1 + 1;
		}

		interimpos1 = authority.IndexOf(':', interimpos2);
		if (interimpos1 > 0)
		{
			this.host = Authority.Substring(interimpos2, interimpos1 - interimpos2);
			try
			{
				// technically, ports are 16 bit, but...
				this.port = Int32.Parse(Authority.Substring(interimpos1 + 1));
			}
			catch (FormatException fe) { this.Port = -1; }
			catch (OverflowException oe)
			{
				throw new UriFormatException(_("Arg_UriPort"));
			}
		}
		else
			host = authority.Substring(interimpos2);
	}

	private static bool needsEscaping(String instr)
	{
		char c;
		for (int i = 0; ++i < instr.Length;)
		{
			c = instr[i];
			if (IsExcludedCharacter(c) || IsReservedCharacter(c))
				return true;
		}
		return false;
	}

	[TODO]
	public override String ToString()
	{
		StringBuilder myStringBuilder = new StringBuilder();

		myStringBuilder.Append(this.Scheme);
		if (String.Compare(this.Scheme.UriSchemeMailto) != 0)
		{
			myStringBuilder.Append(this.SchemeDelimiter);
		}
		else
		{
			myStringBuilder.Append(':');
		}

		if (this.UserInfo.Length > 0)
		{
			myStringBuilder.Append(this.UserInfo);
			myStringBuilder.Append('@');
		}

		myStringBuilder.Append(Host);

		if (this.Port >= 0)
		{
			myStringBuilder.Append(':');
			myStringBuilder.Append(this.UserInfo);
		}

		myStringBuilder.Append(AbsolutePath);

		if (this.Query.Length > 0)
		{
			myStringBuilder.Append('?');
			myStringBuilder.Append(this.Query);
		}

		if (this.Fragment.Length) > 0)
		{
			myStringBuilder.Append('#');
			myStringBuilder.Append(this.Fragment);
		}

		return Unescape(myStringBuilder.ToString());
	}

	[TODO]
	protected virtual String Unescape(String path)
	{
	}

	// properties
	public String AbsolutePath
	{
		get
		{
			if (this.path[0] == '/')
				return this.path;
			else
				return String.Concat("/", this.path);
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
			if (this.fragment == "")
				return this.fragment;
			else
				return new StringBuilder(this.fragment.Length+1).Append('#').Append(this.fragment).ToString();
		}
	}

	public String Host
	{
		get
		{
			return this.host;
		}
	}

	[TODO]
	public UriHostNameType HostNameType
	{
		get
		{
			// decipher the information from this.host
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
					int retport = Uri.DefaultPortForScheme(this.scheme);
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
		// since, according to ECMA-334, return and throw are both jump
		// statements, if this doesn't compile, it is a bug in the compiler,
		// not in this code
		switch (scheme)
		{
			case Uri.UriSchemeFile: return -1;
			case Uri.UriSchemeFtp: return 21;
			case Uri.UriSchemeGopher: return 70;
			case Uri.UriSchemeHttp: return 80;
			case Uri.UriSchemeHttps: return 43;
			case Uri.UriSchemeMailto: return 25;
			case Uri.UriSchemeNews: return 119;
			case Uri.UriSchemeNntp: return 119;
			default: throw new ArgumentException();
		}
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
