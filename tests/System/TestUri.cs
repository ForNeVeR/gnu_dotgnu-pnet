/*
 * TestUri.cs - Test class for "System.Uri" 
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
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

using CSUnit;
using System;

/*
my sage (cough) advice to anyone writing unit tests...have a little
fun! Leave some little jokes to make future generations of hackers
chortle as they wait for their 2 second long build process to
complete.
*/

public class TestUri : TestCase
 {

	Uri rmsUri, pathEnding, noPathEnding;

	// Constructor.
	public TestUri(String name)	: base(name)
	{
		// Nothing to do here.
	}

	// Set up for the tests.
	protected override void Setup()
	{
	}

	// Clean up after the tests.
	protected override void Cleanup()
	{
		// Nothing to do here.
	}

	private bool constructorTested = false;
	public void TestConstructor()
	{
		try
		{
			if (!constructorTested) // others depend on this one
			{
				String lasturi;
				try // good constructors
				{
					rmsUri = new Uri(lasturi = "ftp://rms@ftp.gnu.org:2538/pub/gnu/?freesoftware=good");
					pathEnding = new Uri(lasturi = "http://www.gnu.org/software/../software/guile/");
					noPathEnding = new Uri(lasturi = "http://www.gnu.org/software/../software/guile");
				}
				catch (Exception)
				{
					Fail("{0} threw an exception it shouldn't have!", lasturi)
				}
			}
		}
		finally
		{
			constructorTested = true;
		}
	}

	public void TestCanonicalize()
	{
		AssertEquals("StripMetaDirectories should keep the ending slash when there is one",
			     pathEnding.AbsolutePath,
			     "/software/guile/");
		AssertEquals("StripMetaDirectories shouldn't have an ending slash when there isn't one",
			     pathEnding.AbsolutePath,
			     "/software/guile/index.html");
		
	}

	public void TestUriCheckHostName()
	{
		AssertEquals("www.gnu.org is a DNS name",
			     Uri.CheckHostName("www.gnu.org"),
			     UriHostNameType.Dns);
		AssertEquals("www.southern.-storm.com.au is not a DNS name",
			     Uri.CheckHostName("www.southern.-storm.com.au"),
			     UriHostNameType.Unknown);
		AssertEquals("www.southern-storm.com.au is a DNS name",
			     Uri.CheckHostName("www.southern-storm.com.au"),
			     UriHostNameType.Dns);
		AssertEquals("127.0.0.1 is an IPv4 address",
			     Uri.CheckHostName("127.0.0.1"),
			     UriHostNameType.IPv4);
		AssertEquals(".63.64.201.1 is not an IPv4 address",
			     Uri.CheckHostName(".63.64.201.1"),
			     UriHostNameType.Unknown);
		AssertEquals("207..211.18.4 is not an IPv4 address",
			     Uri.CheckHostName("207..211.18.4"),
			     UriHostNameType.Unknown);

		// checking IPng
		AssertEquals(":F0F0::0 should have bad IPng zerocompress at beginning",
			     Uri.CheckHostName(":F0F0::0"),
			     UriHostNameType.Unknown);
		AssertEquals("::F0F0:0 allows fake zerocompress at beginning",
			     Uri.CheckHostName("::F0F0:0"),
			     UriHostNameType.IPv6);
		AssertEquals("0:1:2:3:4:5:6:127.0.0.1 has too many elements"
			     Uri.CheckHostName("0:1:2:3:4:5:6:127.0.0.1"),
			     UriHostNameType.Unknown);
		AssertEquals("0:1:2:3:4:5:127.0.0.1 has the right number of elements",
			     Uri.CheckHostName("0:1:2:3:4:5:127.0.0.1"),
			     UriHostNameType.IPv6);
	}

	public void TestUriCheckSchemeName()
	{
		Assert("Anr.7 is a scheme name",
		       Uri.CheckSchemeName("Anr.7"));
		Assert("6thsense is not a scheme name",
		       !Uri.CheckSchemeName("6thsense"));
		Assert("gnu+freedom-limits is a scheme name",
		       Uri.CheckSchemeName("gnu+freedom-limits"));
		// that's GNU plus Freedom minus Limits
	}

	// TestUriCheckSecurity() is not necessary

	public void TestUriEquals()
	{
	/*TODO*/
	}

	public void TestUriEscape()
	{
	/*TODO*/
	}

	public void TestUriEscapeString()
	{
	/*TODO*/
	}

	public void TestUriFromHex()
	{
	/*TODO*/
	}

	public void TestUriGetHashCode()
	{
	/*TODO*/
	}

	public void TestUriGetLeftPart()
	{
	/*TODO*/
	}

	public void TestUriHexEscape()
	{
	/*TODO*/
	}

	public void TestUriHexUnescape()
	{
	/*TODO*/
	}

	public void TestUriIsBadFileSystemCharacter()
	{
	/*TODO*/
	}

	public void TestUriIsExcludedCharacter()
	{
	/*TODO*/
	}

	public void TestUriIsHexDigit()
	{
		// gee, this is a hard one
		Assert("C is a hex digit", Uri.IsHexDigit('C'));
		Assert("9 is a hex digit", Uri.IsHexDigit('9'));
		// incidentally, 0xC9 in binary is...11001001
		Assert("f is a hex digit", Uri.IsHexDigit('f'));
		Assert("G is not a hex digit", !Uri.IsHexDigit('G'));
		Assert("\x00C9 is not a hex digit",
		       !Uri.IsHexDigit('\x00C9')); // I am one funny guy
		Assert("\x20AC is not a hex digit (then again, neither is $)",
		       !Uri.IsHexDigit('\x20AC'));
		// ok, classes like this don't really need all this testing ;)
	}

	public void TestUriIsHexEncoding()
	{
		Assert("\"%c9\", position 0, is hex encoding", Uri.IsHexEncoding("%c9", 0));
		Assert("\"%c9\", position -1, is not hex encoding", !Uri.IsHexEncoding("%c9", -1));
		Assert("\"0x%c9\", position 3, is not hex encoding", !Uri.IsHexEncoding("0x%c9", 3));
		Assert("\"0x%c9\", position 2, is hex encoding", Uri.IsHexEncoding("0x%c9", 2));
		Assert("\"%at\", position 0, is not hex encoding", !Uri.IsHexEncoding("%at", 0));
		Assert("\"%af\", position 100, is not hex encoding", !Uri.IsHexEncoding("%af", 100));
	}

	public void TestUriIsReservedCharacter()
	{
	/*TODO*/
	}

	public void TestUriMakeRelative()
	{
		Uri gnuphil = new Uri("http://www.gnu.org/philosophy/why-free.html");
		Uri gnuoreilly = new Uri("http://www.gnu.org/gnu/thegnuproject.html");
		Uri mozillaftp = new Uri("ftp://ftp.mozilla.org/pub/mozilla/latest/mozilla-i686-pc-linux-gnu-sea.tar.gz");
		Uri mozillahttp = new Uri("http://ftp.mozilla.org/pub/mozilla/latest/mozilla-i686-pc-linux-gnu-sea.tar.gz");
		Uri mandrake = new Uri("ftp://distro.ibiblio.org/pub/Linux/distributions/mandrake/Mandrake/iso/");
		Uri debian = new Uri("ftp://distro.ibiblio.org/pub/Linux/distributions/debian/main/");
		Uri debianrelease = new Uri("ftp://distro.ibiblio.org/pub/Linux/distributions/debian/main/source/Release");

		AssertEquals("Code figures out simple relative Uri correctly (with files)",
			     gnuphil.MakeRelative(gnuoreilly),
			     "../thegnuproject.html");
		AssertEquals("notices different schemes when comparing Uris",
			     mozillaftp.MakeRelative(mozillahttp),
			     mozillahttp.AbsoluteUri);
		AssertEquals("figures out more complex, directory-based relative Uri",
			     mandrake.MakeRelative(debian),
			     "../../../debian/main/");
		AssertEquals("tells difference between files and directorys, by looking for ending slash",
			debianrelease.MakeRelative(debian), "../");
		AssertEquals("correctly goes further into subdirectories",
			debian.MakeRelative(debianrelease), "source/Release");
	}

	// Parse N/A

	public void TestUriToString()
	{
	/*TODO*/
	}

	public void TestUriUnescape()
	{
	/*TODO*/
	}

	public void TestAbsolutePath()
	{
	/*TODO*/
	}
	public void TestAbsoluteUri()
	{
	/*TODO*/
	}
	public void TestAuthority()
	{
		AssertEquals("rmsUri: Authority built correctly", rmsUri.Authority, "rms@ftp.gnu.org:2538");
	}
	public void TestFragment()
	{
	/*TODO*/
	}
	public void TestHost()
	{
		AssertEquals("rmsUri: Host parsed", rmsUri.Host, "ftp.gnu.org");
	}
	public void TestHostNameType()
	{
		AssertEquals("rmsUri: Correct HostNameType detected", rmsUri.HostNameType, UriHostNameType.Dns);
	}
	public void TestIsDefaultPort()
	{
		Assert("rmsUri: 2538 is not default for ftp", rmsUri.IsDefaultPort);
	}
	public void TestIsFile()
	{
	/*TODO*/
	}
	public void TestIsLoopback()
	{
	/*TODO*/
	}
	public void TestLocalPath()
	{
	/*TODO*/
	}
	public void TestPathAndQuery()
	{
	/*TODO*/
	}
	public void TestPort()
	{
		AssertEquals("rmsUri: Port parsed", rmsUri.Port, 2538);
	}
	public void TestQuery()
	{
		AssertEquals("rmsUri: Query parsed", rmsUri.Query, "?freesoftware=good");
	}
	public void TestScheme()
	{
		AssertEquals("rmsUri: Scheme parsed", rmsUri.Scheme, "ftp");
	}
	public void TestUserEscaped()
	{
	/*TODO*/
	}
	public void TestUserInfo()
	{
		AssertEquals("rmsUri: User info parsed", rmsUri.UserInfo, "rms");
	}
}
