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
// move const character-strings here, make internal, fix UriBuilder to use
// go through the comparison code, do not assume escaping (possibly parse always)

namespace System
{

using System.Text;

public class Uri : MarshalByRefObject
{

	internal static const String LOCALHOST = "localhost";
	private static const String LOCALHOSTIP = "127.0.0.1";
	private static const String HEXCHARS = "0123456789ABCDEFabcdef";
	private static const String VALIDSCHEMECHARS = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm0123456789.+-";

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

	// Holds the scheme information. (search 0->:)
	private String scheme;

	// authority = userinfo+host+port
	// userinfo = username:password
	private String username;
	private String password;
	private String host;
	private int port;

	// technically optional, but they want a path :)
	private String path;

	// doesn't contain the ? mark
	private String query;

	// remember: this is not part of the uri
	// doesn't contain the #
	private String fragment;
	// also known as hash

	// end of state

	// Constructors.
	[TODO]
	Uri(String uriString)
	{
	}

	[TODO]
	public Uri(String uriString, bool dontEscape)
	{
	}

	[TODO]
	public Uri(Uri baseUri, String relativeUri)
	{
	}

	[TODO]
	public Uri(Uri baseUri, String relativeUri, bool dontEscape)
	{
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
		int charloc = Uri.VALIDSCHEMECHARS.IndexOf(schemeName[0]);
		if (charloc < 0 || charloc > 52) return false;
		for (int i = 1; ++i < schemeName.Length;)
		{
			if (Uri.VALIDSCHEMECHARS.IndexOf(schemeName[i]) == -1)
				return false;
		}
		return true;
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

	[TODO]
	protected virtual void Escape()
	{
	}

	[TODO]
	protected static String EscapeString(String str)
	{
	}

	public static int FromHex(char digit)
	{
		int placement = HEXCHARS.IndexOf(digit);
		if (placement == -1) throw new ArgumentException(_("Exception_Argument_HexDigit"), "digit");
		else if (placement <= 15) return placement;
		else return placement - 6;
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
		char first = Uri.HEXCHARS[character >> 4];
		// 0b00001111 == 0x0F == 15
		char second = Uri.HEXCHARS[character & 15];
		return new StringBuilder(3).Append('%').Append(first).Append(second).ToString();
	}

	[TODO]
	public static char HexUnescape(String pattern, ref int index)
	// help me out on the meaning of ref here (boxing?)
	{
	}

	[TODO]
	protected virtual bool IsBadFileSystemCharacter(char character)
	{
	}

	[TODO]
	protected static bool IsExcludedCharacter(char character)
	{
	}

	[TODO]
	public static bool IsHexDigit(char character)
	{
	}

	[TODO]
	public static bool IsHexEncoding(String pattern, int index)
	{
	}

	[TODO]
	protected virtual bool IsReservedCharacter(char character)
	{
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
	[TODO]
	public String AbsolutePath
	{
		get
		{
		}
	}

	[TODO]
	public String AbsoluteUri
	{
		get
		{
		}
	}

	[TODO]
	public String Authority
	{
		get
		{
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
				return (this.port == Uri.DefaultPortForScheme(this.scheme));
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
			return (String.Equals(this.host, Uri.LOCALHOST) ||
				String.Equals(this.host, Uri.LOCALHOSTIP));
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

	[TODO]
	public String PathAndQuery
	{
		get
		{
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

	[TODO]
	public string UserInfo
	{
		get
		{
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
