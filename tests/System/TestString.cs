/*
 * TestString.cs - Tests for the "System.String" class.
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
 * 
 * Authors : Stephen Compall & Gopal.V
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

public class TestString : TestCase
{
	// Constructor.
	public TestString(String name)
			: base(name)
			{
				// Nothing to do here.
			}

	// Set up for the tests.
	protected override void Setup()
			{
				// Nothing to do here.
			}

	// Clean up after the tests.
	protected override void Cleanup()
			{
				// Nothing to do here.
			}

	public void TestIndexOfAcceptsLength()
	{
		try
			"x".IndexOf(a, 1);
		catch (ArgumentOutOfRangeException aoore)
			Fail("IndexOf(char, int) should not throw when passed Length as the int");
	}
	//Methods
	public void TestClone()
	{
		/*TODO*/
	}
	public void TestStringCompare()
	{
/*		String lesser = "abc";
		String medium = "abcd";
		String greater = "xyz";
		String caps = "ABC";*/

		AssertEquals("String.Compare(null,null))",0,String.Compare(null,null));
		AssertEquals("String.Compare(\"abc\",null)",1, 
			String.Compare("abc", null));
			
		Assert("String.Compare(null,\"abc\")", 
			String.Compare(null,"abc") < 0);
		
		Assert("String.Compare(\"abc\",\"xyz\")",
			String.Compare("abc","xyz") < 0);
			
		Assert("String.Compare(\"abc\",\"abcd\")",
			String.Compare("abc","abcd") < 0);
			
		Assert("String.Compare(\"xyz\",\"abc\")",
			String.Compare("xyz","abc") == 0);

		Assert("String.Compare(\"abc\",\"ABC\",true)",
			String.Compare("abc","ABC",true) == 0);

		Assert("String.Compare(\"abc\",\"ABC\",true)",
			String.Compare("abc","ABC",true) == 0);

		Assert("String.Compare(\"abc\",\"ABC\",false)",
			String.Compare("abc","ABC",false) != 0);

		Assert("String.Compare(\"a\",\"A\")",String.Compare("a","A") < 0);
		Assert("String.Compare(\"A\",\"a\")",String.Compare("A","a") > 0);
	
		AssertEquals("String.Compare(\"\",\"\")",String.Compare("",""),0);

		AssertEquals("String.Compare(\"abc\",0,\"xyz\",0,0)",
			String.Compare("abc",0,"xyz",0,0),0);

		AssertEquals("String.Compare(\"abcdabcd\",0,\"ab\",0,2)",
			String.Compare("abcdabcd",0,"ab",0,2),0);
			
		AssertEquals("String.Compare(\"abcdabcd\",4,\"ab\",0,2)",
			String.Compare("abcdabcd",4,"ab",0,2),0);
			
		Assert("String.Compare(\"abcdabcd\",1,\"ab\",0,2)",
			String.Compare("abcdabcd",1,"ab",0,2) < 0 );

		Assert("String.Compare(\"abcdabcd\",0,\"xyz\",0,2)",
			String.Compare("abcdabcd",0,"xyz",0,2) > 0 );
		
		AssertEquals("String.Compare(\"abcdabcd\",0,\"abd\",0,2)",
			String.Compare("abcdabcd",0,"abd",0,2),0);
		
		try
		{
			String.Compare("ab",3,"abc",1,7);
			Fail(" String.Compare(\"ab\",3,\"abc\",1,7) did not throw ArgumentOutOfRangeException");
		}
		catch(ArgumentOutOfRangeException err)
		{
			//Ok, that worked :)
			//move on folks
		}
		
		Assert("String.Compare(\"ABCDC\",1,\"bcd\",0,3,false)",
			String.Compare("ABCDC",1,"bcd",0,3,false) !=0);
			
		Assert("String.Compare(\"ABCDC\",1,\"bcd\",0,3,true)",
			String.Compare("ABCDC",1,"bcd",0,3,true) !=1);
		try
		{
			String.Compare("ab",3,"abc",1,7,true);
			Fail(" String.Compare(\"ab\",3,\"abc\",1,7,true) did not throw ArgumentOutOfRangeException");
		}
		catch(ArgumentOutOfRangeException err)
		{
			//Ok, that worked :)
			//move on folks
		}
			
	/*
		TODO: put in a looped check for Compare ()
	*/
	}

	public void TestStringCompareOrdinal()
	{
	/*
		TODO: doesn't this need checks for all the I18n infos ?
		      what's the sense in doing it for English , which is
		      what Compare() does.
	*/
	}

	public void TestStringCompareTo()
	{
	/*
		NOTE: doesn't this actually call the Compare(this,a) func ?
	*/
		String str="abc";
		Assert("str.CompareTo(\"xyz\")", str.CompareTo("xyz") < 0);
		Assert("str.CompareTo(\"abc\")", str.CompareTo("abc") == 0);
		Assert("str.CompareTo(\"0\")", str.CompareTo("0") > 0);	
	}

	public void TestStringConcat()
	{
		String str1="Fu";
		String str2="Bar";
		AssertEquals("String.Concat(\"Fu\",\"Bar\")",
			String.Concat("Fu","Bar"),"FuBar");
		AssertEquals("String.Concat(\"Foo\",\" \",\"Bar\")",
			String.Concat("Foo"," ","Bar"),"Foo Bar");
		// yup , F Up Beyond All Recognition
	}

	public void TestStringCopy()
	{
		String str1="Foo";
		String str2= String.Copy(str1);
		AssertEquals("String.Copy(str1)==str1",str2,str1);
		try
		{
			String s = String.Copy(null);
			Fail(" String.Copy(null) should throw an ArgumentNullException");
		}
		catch(ArgumentNullException err)
		{
			//ummm... looks good 
		}
	}

	public void TestStringCopyTo()
	{
	/*TODO*/
		String str1 = "FuBar";
		try
		{
			str1.CopyTo(0,(char[])null,0,0);
			Fail("str1.CopyTo(0,(char[])null,0,0) should have throws a ArgumentNullException");
		}
		catch(ArgumentNullException err)
		{
			//worked !
		}
		char[] c = new char[str1.Length];
		for (int i = 0; i < str1.Length ; i++) 
			str1.CopyTo(i, c, i, 1); // copy 1 char at a time
		String str2 = new String(c);
		AssertEquals("str1.CopyTo() char by char",str1,str2);
		// must find a better error message :)
	}

	public void TestStringEndsWith()
	{
	/*TODO*/
		String str = "Foo Bar";
		try 
		{
			bool check = str.EndsWith(null);
			Fail("String.EndsWith(null) should throw an ArgumentNullException");
		} 
		catch (ArgumentNullException) 
		{
			//OK move on 
		}
		Assert("str.EndsWith(\"r\")", str.EndsWith("r"));
		Assert("str.EndsWith(\"Bar\")", str.EndsWith("Bar"));
		Assert("!str.EndsWith(\"Foo\")", !str.EndsWith("Foo"));
	}

	public void TestStringEquals()
	{
	/*
		TODO: really need to see how to I18n here 
	*/
		String foobar = "FooBar";
		String fubar = "FooBar";
		String good_copy = foobar;
		String bad_copy = "I'm bad";
		String bad = null;
		try
		{
			bool q=bad.Equals("nothing");
			Fail("bad.Equals(\"nothing\") should have thrown a ArgumentNullException");
		}
		catch(ArgumentNullException err)
		{
			//great !
		}

		Assert("!foobar.Equals(null)", !foobar.Equals(null));
		Assert("foobar.Equals(fubar)", foobar.Equals(fubar));
		Assert("foobar.Equals(good_copy)", foobar.Equals(good_copy));
		Assert("!foobar.Equals(bad_copy)", !foobar.Equals(bad_copy));

		Assert("String.Equals(null,null)", String.Equals(null, null));
		Assert("String.Equals(foobar,fubar)", String.Equals(foobar, fubar));
		Assert("String.Equals(foobar,bad_copy)", !String.Equals(foobar, bad_copy));
	}

	public void TestStringFormat()
	{
	/*TODO*/
		AssertEquals ("String.Format(\"\",0)", String.Format ("", 0),"");
		
		AssertEquals ("String.Format(\"{0}\",\"FooBar\")", 
		String.Format ("{0}","FooBar"),"FooBar");
		
		AssertEquals ("String.Format(\"{0}\",111)", 
		String.Format ("{0}",111),"111");

		try
		{
			String.Format(null,12);
			Fail("String.Format(null,12) should throw an ArgumentNullException");
			String.Format("Er..",null);
			Fail("String.Format(\"Er...\",null) should throw an ArgumentNullException");
		}
		catch(ArgumentNullException err)
		{
			// all's well 
		}

		try
		{
			String.Format("{-1}",12);
			Fail("String.Format(\"{-1}\",12) should throw an FormatException");
			String.Format("{3}",12);
			Fail("String.Format(\"{3}\",12) should throw an FormatException");
			String.Format("{}",12);
			Fail("String.Format(\"{}\",12) should throw an FormatException");
		}
		catch(FormatException err)
		{
			// all's well 
		}
		AssertEquals("String.Format (\"<{0,5}>\", 12)",
			String.Format ("<{0,5}>", 12),"<   12>");
		
		AssertEquals("String.Format (\"<{0,5}>\", 12)",
			String.Format ("<{0,-5}>", 12),"<12   >");

		AssertEquals ("String.Format(\"The {0} of {1}\",\"Church\",\"Emacs\")"
			String.Format ("The {0} of {1}","Church", "Emacs"),
			"The Church of Emacs");
		
		AssertEquals("String.Format(\"G{0} N{1} U{2}\",\"nu's\",\"ot\",\"nix\")"
			,String.Format ("G{0} N{1} U{2}","nu's", "ot", "nix"),
			"Gnu's Not Unix");

		AssertEquals ("String.Format (\"{0:x8}\", 0xcafebabe),\"cafebabe\")",
			String.Format ("{0:x8}", 0xcafebabe),"cafebabe");

		AssertEquals ("String.Format (\"<{0,5:x3}>\", 0x55)",
			String.Format ("<{0,5:x3}>", 0x55),"<  055>");
	
		AssertEquals ("String.Format (\"<{0,-5:x3}>\", 0x55)",
			String.Format ("<{0,-5:x3}>", 0x55),"<055  >");
			
		AssertEquals ("String.Format (\"if({0}==0){{ .... }}\",\"i\")",
		String.Format ("if({0}==0){{ .... }}","i"),"if(i==0){ .... }");
	}

	public void TestStringGetEnumerator()
	{
		String str = "foobar";
		char[] c = new char[s1.Length];
		CharEnumerator en = str.GetEnumerator();
		AssertNotNull("CharEnumerator en!=null", en);
		
		for (int i = 0; i < str.Length; i++) 
		{
			en.MoveNext();
			c[i] = en.Current;
		}
		String copy = new String(c);
		AssertEquals("Copy by enumerator for string", str, copy);
	}

	public void TestStringGetHashCode()
	{
	/*
		TODO: what do I do here ?. (determinicity check ?)
		      s.GetHashCode() == s.GetHashCode() ?.
	*/
		String s = "foobar";
		AssertEquals("s.GetHashCode() == s.GetHashCode()",
			s.GetHashCode(),s.GetHashCode());
	}

	public void TestStringIndexOf()
	{
		String fubar = "Foo Bar";
		try
		{
			fubar.IndexOf(null);
			Fail("fubar.IndexOf(null) should throw an ArgumentNullException");
			
			fubar.IndexOf(null,0);
			Fail("fubar.IndexOf(null,0) should throw an ArgumentNullException");
			
			fubar.IndexOf(null,0,1);
			Fail("fubar.IndexOf(null,0,1) should throw an ArgumentNullException");
		}
		catch(ArgumentNullException err)
		{
			//all's well
		}
		try
		{
			fubar.IndexOf('f',fubar.Length+1);
			Fail("fubar.IndexOf('f',fubar.Length+1) should throw an ArgumentOutOfRangeException");

			fubar.IndexOf('f',fubar.Length+1,1);
			Fail("fubar.IndexOf('f',fubar.Length+1,1) should throw an ArgumentOutOfRangeException");
			
			fubar.IndexOf("foo",fubar.Length+1);
			Fail("fubar.IndexOf(\"foo\",fubar.Length+1) should throw an ArgumentOutOfRangeException");
			
			fubar.IndexOf("foo",fubar.Length+1,1);
			Fail("fubar.IndexOf(\"foo\",fubar.Length+1,1) should throw an ArgumentOutOfRangeException");
		}
		catch(ArgumentOutOfRangeException err)
		{
			//all's well 
		}
		AssertEquals("fubar.IndexOf('o')", fubar.IndexOf('o'),1);
		AssertEquals("fubar.IndexOf('F')", fubar.IndexOf('F'),0);
		AssertEquals("fubar.IndexOf('z')", fubar.IndexOf('z'),-1);

		AssertEquals("fubar.IndexOf(\"oo\")", fubar.IndexOf("oo"),1);
		AssertEquals("fubar.IndexOf(\"Bar\")", fubar.IndexOf("Bar"),4);
		AssertEquals("fubar.IndexOf(\"qwaz\")", fubar.IndexOf("qwaz"),-1);
		
		AssertEquals("fubar.IndexOf('o',1)", fubar.IndexOf('o',1),1);
		AssertEquals("fubar.IndexOf('o',3)", fubar.IndexOf('o',3),-1);
		AssertEquals("fubar.IndexOf('z',4)", fubar.IndexOf('z',4),-1);
		
		AssertEquals("fubar.IndexOf(\"oo\",1)", fubar.IndexOf("oo",1),1);
		AssertEquals("fubar.IndexOf(\"oo\",2)", fubar.IndexOf("oo",2),-1);

		AssertEquals("fubar.IndexOf('o',1,2)", fubar.IndexOf('o',1,1),1);
		AssertEquals("fubar.IndexOf('o',0,1)", fubar.IndexOf('o',0,1),-1);
		AssertEquals("fubar.IndexOf(' ',1,5)", fubar.IndexOf(' ',1,5),3);

	/*
		TODO: I don't know any more test ideas , do you have one ?
	*/
	}

	public void TestStringIndexOfAny()
	{
		String fubar="mary had a little lamb ....";
		try
		{
			fubar.IndexOfAny(null);
			Fail("fubar.IndexOfAny(null) should throw an ArgumentNullException");
			fubar.IndexOfAny(null,0);
			Fail("fubar.IndexOfAny(null,0) should throw an ArgumentNullException");
			fubar.IndexOfAny(null,0,1);
			Fail("fubar.IndexOfAny(null,0,1) should throw an ArgumentNullException");
		}
		catch(ArgumentNullException err)
		{
			//all's A-OK !
		}
		char[] c={'a','e','i','o','u'};
		try
		{
			fubar.IndexOfAny(c,fubar.Length+1);
			Fail("fubar.IndexOfAny(c,fubar.Length+1) should throw an ArgumentOutOfRangeException");
			fubar.IndexOfAny(c,fubar.Length+1,1);
			Fail("fubar.IndexOfAny(c,fubar.Length+1,1) should throw an ArgumentOutOfRangeException");
			fubar.IndexOfAny(c,fubar.Length-3,5);
			Fail("fubar.IndexOfAny(c,fubar.Length-3,5) should throw an ArgumentOutOfRangeException");
		}
		catch(ArgumentOutOfRangeException)
		{
			//all's well in code and data :)
		}
		
		AssertEquals("fubar.IndexOfAny(c)",fubar.IndexOfAny(c),1); //m`a'
		AssertEquals("fubar.IndexOfAny(c,2)",fubar.IndexOfAny(c,2),6);//h`a
		AssertEquals("fubar.IndexOfAny(c,21)",fubar.IndexOfAny(c,21),-1);
		AssertEquals("fubar.IndexOfAny(c,2,5)",fubar.IndexOfAny(c,2,5),6);
		AssertEquals("fubar.IndexOfAny(c,2,3)",fubar.IndexOfAny(c,2,3),-1);			
	}

	public void TestStringInsert()
	{
	/*TODO*/
	}

	public void TestStringIntern()
	{
	/*TODO*/
	}

	public void TestStringIsInterned()
	{
	/*TODO*/
	}

	public void TestStringJoin()
	{
	/*TODO*/
	}

	public void TestStringLastIndexOf()
	{
	/*TODO*/
	}

	public void TestStringLastIndexOfAny()
	{
	/*TODO*/
	}

	public void TestStringPadLeft()
	{
	/*TODO*/
	}

	public void TestStringPadRight()
	{
	/*TODO*/
	}

	public void TestStringRemove()
	{
	/*TODO*/
	}

	public void TestStringReplace()
	{
	/*TODO*/
	}

	public void TestStringSplit()
	{
	/*TODO*/
	}

	public void TestStringStartsWith()
	{
	/*TODO*/
	}

	public void TestStringSubstring()
	{
	/*TODO*/
	}

	public void TestStringToCharArray()
	{
	/*TODO*/
	}

	public void TestStringToLower()
	{
	/*TODO*/
	}

	public void TestStringToString()
	{
	/*TODO*/
	}

	public void TestStringToUpper()
	{
	/*TODO*/
	}

	public void TestStringTrim()
	{
	/*TODO*/
	}

	public void TestStringTrimEnd()
	{
	/*TODO*/
	}

	public void TestStringTrimStart()
	{
	/*TODO*/
	}

	public void TestStringop_Equality()
	{
	/*TODO*/
	}

	public void TestStringop_Inequality()
	{
	/*TODO*/
	}

	public void TestChars()
	{
	/*TODO*/
	}
	public void TestLength()
	{
		AssertEquals("\"Foo Bar\".Length","Foo Bar".Length,7);
		AssertEquals("\"\".Length","".Length,0);
	}

}; // class TestString
