/*
 * Uri.cs - Implementation of "System.Uri".
 *
 * Copyright (C) 2002 Free Software Foundation, Inc.
 *
 * Contributed by Stephen Compall <rushing@sigecom.net>
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
	private String          absolutePath; // the path to the resource on the host (/uri/blah)
	private String          absolutePathEscaped; // the unescaped version of the absolute path.
	private String          absoluteUri;  // the absolute uri to the resource (in it's entirety)
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
	Uri(String uriString) : this(uriString, false)
	{
	}

	Uri(String uriString, bool dontEscape)
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

	Uri(Uri baseUri, String relativeUri) : this(baseUri, relativeUri, false)
	{
	}

	[TODO]
	public Uri(Uri baseUri, String relativeUri, bool dontEscape)
	// improve efficiency of this code (EndsWith, StartsWith to IndexOf)
	{
	if (baseUri== null)
		throw new ArgumentNullException("baseUri");

	if (relativeUri== null)
		throw new ArgumentNullException("relativeUri");

	// Making local copies that we use for modification
	String myBaseUri = Trim(baseUri.AbsoluteUri);
	String myRelativeUri = Trim(relativeUri);
	UserEscaped = dontEscape;
	while (myBaseUri.EndsWith("/"))
		myBaseUri = myBaseUri.Substring(0, myBaseUri.Length() - 2);

	while (myRelativeUri.StartsWith("/"))
		relativeUri = relativeUri.Substring( 1 );

	this.absoluteUri = baseUri + "/" + relativeUri;

	Parse();
	Canonicalize();
	}

	// methods
	[TODO]
	protected virtual void Canonicalize()
	{
	}

	[TODO]
	public static UriHostNameType CheckHostName(String name)
	{
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

	[TODO]
	public override bool Equals(Object comparand)
	// TODO on this method: account for escape/unescape in strings
	{
		if (comparand == null)
			return false;
		try
		{
			Uri rurib = (Uri) comparand;
			// do not check query and fragment
			// this makes the boolean
			return (this.Host == rurib.Host &&
			    this.Password == rurib.Password && this.Path == rurib.Path &&
			    this.Scheme == rurib.Scheme &&
			    this.Port == rurib.Port && this.UserName == rurib.UserName);
		}
		catch (InvalidCastException ice)
		{
			try
			{
				return this.ToStringNoFragQuery.Equals(Uri.ToStringNoFragQuery((String) comparand));
			}
			catch (InvalidCastException nice)
			{
				return false;
			}
		}
	}

	protected virtual void Escape()
	{
		this.abspath = EscapeString(this.abspath);
	}

	protected static String EscapeString(String str)
	{
		if (this.abspath == null || str.Length == 0)
			return String.Empty;
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
			throw new ArgumentException(_("Exception_Argument_HexDigit"), "digit");
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

	[TODO]
	public static char HexUnescape(String pattern, ref int index)
	// help me out on the meaning of ref here (boxing?)
	{
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

	[TODO]
	public static bool IsHexEncoding(String pattern, int index)
	{
	}

	protected virtual bool IsReservedCharacter(char character)
	{
		return (";/:@&=+$,".IndexOf(character) >= 0);
	}

	[TODO]
	public String MakeRelative(Uri toUri)
	{
	}

	[TODO]
	protected virtual void Parse()
	{
	}

	[TODO]
	public override String ToString()
	{
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
			return this.path;
		}
	}

	[TODO]
	public String AbsoluteUri
	{
		get
		{
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
			if (this.fragment == String.Empty)
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
			String qpath = this.AbsolutePath;
			String pquery = this.Query;
			return new StringBuilder(qpath.Length+pquery.Length).Append(qpath).Append(pquery).ToString();
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
			if (this.query == String.Empty)
				return this.query;
			else
				return new StringBuilder(this.query.Length+1).Append('?').Append(this.query).ToString();
		}
	}

	public String Scheme
	{
		get
		{
			return this.scheme;
		}
	}

	[TODO]
	public bool UserEscaped
	{
		get
		{
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
