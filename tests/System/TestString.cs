/*
 * TestString.cs - Tests for the "System.String" class.
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
 *
 * Authors : Stephen Compall, Gopal.V, & Richard Baumann
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

	//Methods
	public void TestClone()
	{
		String fubar = "Foo Bar";
		AssertEquals("fubar.Clone() as String",(String)fubar.Clone(),fubar);
		AssertEquals("fubar.Clone() as Object",fubar.Clone(),(Object)fubar);
		AssertEquals("((ICloneable) fubar).Clone() as String",(String)((ICloneable) fubar).Clone(),fubar);
		AssertEquals("((ICloneable) fubar).Clone() as Object",((ICloneable) fubar).Clone(),(Object)fubar);
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
			String.Compare("xyz","abc") > 0);

		Assert("String.Compare(\"abc\",\"ABC\",true)",
			String.Compare("abc","ABC",true) == 0);

		Assert("String.Compare(\"abc\",\"ABC\",true)",
			String.Compare("abc","ABC",true) == 0);

		Assert("String.Compare(\"abc\",\"ABC\",false)",
			String.Compare("abc","ABC",false) != 0);

		Assert("String.Compare(\"a\",\"A\")",String.Compare("a","A") > 0);
		Assert("String.Compare(\"A\",\"a\")",String.Compare("A","a") < 0);
	
		AssertEquals("String.Compare(\"\",\"\")",String.Compare("",""),0);

		AssertEquals("String.Compare(\"abc\",0,\"xyz\",0,0)",
			String.Compare("abc",0,"xyz",0,0),0);

		AssertEquals("String.Compare(\"abcdabcd\",0,\"ab\",0,2)",
			String.Compare("abcdabcd",0,"ab",0,2),0);
			
		AssertEquals("String.Compare(\"abcdabcd\",4,\"ab\",0,2)",
			String.Compare("abcdabcd",4,"ab",0,2),0);
			
		Assert("String.Compare(\"abcdabcd\",1,\"ab\",0,2)",
			String.Compare("abcdabcd",1,"ab",0,2) > 0 );

		Assert("String.Compare(\"abcdabcd\",0,\"xyz\",0,2)",
			String.Compare("abcdabcd",0,"xyz",0,2) < 0 );
		
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
		catch(NullReferenceException err)
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
		Assert("str.EndsWith(\"Foo Bar\")", str.EndsWith("Foo Bar"));
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
		Assert("!String.Equals(foobar,bad_copy)", !String.Equals(foobar, bad_copy));
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

		AssertEquals ("String.Format(\"The {0} of {1}\",\"Church\",\"Emacs\")",
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
		char[] c = new char[str.Length];
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
		String str1 = "foobar";
		String str2 = String.Copy(str1);
		AssertEquals("str1.GetHashCode() == str1.GetHashCode()",
			str1.GetHashCode(),str1.GetHashCode());
		AssertEquals("str1.GetHashCode() == str2.GetHashCode()",
			str1.GetHashCode(),str2.GetHashCode());
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

		try
		{
			fubar.IndexOf('h', fubar.Length); // shouldn't throw per ECMA
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("IndexOf(char, int) should not throw when passed Length as the int");
		}
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
		String fubar = "FooBar";
		AssertEquals("fubar.Insert(3,\" \")",fubar.Insert(3," "),"Foo Bar");
		AssertEquals("fubar.Insert(0,\" \")",fubar.Insert(0," ")," FooBar");
		AssertEquals("fubar.Insert(fubar.Length,\" \")",fubar.Insert(fubar.Length," "),"FooBar ");
		try
		{
			fubar.Insert(0,null);
			Fail("fubar.Insert(0,null) should throw an ArgumentNullException");
		}
		catch(ArgumentNullException)
		{
			// checks out ok
		}
		try
		{
			fubar.Insert(fubar.Length+1," ");
			Fail("fubar.Insert(fubar.Length+1,\" \") should throw an ArgumentOutOfRangeException");
		}
		catch(ArgumentOutOfRangeException)
		{
			// this works too
		}
	}

	public void TestStringIntern()
	{
		String foobar = "if the sun refused to shine, I don't mind, I don't mind... if the mountains fell in the sea, let it be, it aint me - hendrix";
		String fubar = foobar.Substring(0,10);
		fubar += foobar.Substring(10,foobar.Length-10);
		fubar = String.Intern(fubar);
		AssertEquals("String.Intern(fubar)",(Object)foobar,(Object)fubar);
		try
		{
			String.Intern(null);
			Fail("String.Intern(null) should throw an ArgumentNullException");
		}
		catch (ArgumentNullException)
		{
			// this works
		}
	}

	public void TestStringIsInterned()
	{
		// I can't imagine anyone using a string like this in pnetlib or the unit tests, so this should work
		char[] fu = new char[13] { 'q','w','e','r','t','y','u','i','o','p','\t','\0','\n' };
		String foobar = new String(fu);
		String fubar = String.IsInterned(foobar);
		Assert("String.IsInterned(foobar)",fubar == null);
		try
		{
			String.IsInterned(null);
			Fail("String.IsInterned(null) should throw an ArgumentNullException");
		}
		catch (ArgumentNullException)
		{
			// all is good
		}
	}

	public void TestStringJoin()
	{
		String fu = " fu ";
		String fu2 = "";
		String[] foo = new String[6] { "foo","ofo","oof","FOO","OFO","OOF" };
		String[] foo2 = new String[3] { "","","" };
		String[] foo3 = new String[0] { };
		AssertEquals("String.Join(fu,foo)",String.Join(fu,foo),"foo fu ofo fu oof fu FOO fu OFO fu OOF");
		AssertEquals("String.Join(fu,foo,0,foo.Length)",String.Join(fu,foo,0,foo.Length),"foo fu ofo fu oof fu FOO fu OFO fu OOF");
		AssertEquals("String.Join(fu,foo,0,1)",String.Join(fu,foo,0,1),"foo");
		AssertEquals("String.Join(fu,foo,1,1)",String.Join(fu,foo,1,1),"ofo");
		AssertEquals("String.Join(fu,foo,2,1)",String.Join(fu,foo,2,1),"oof");
		AssertEquals("String.Join(fu,foo,3,1)",String.Join(fu,foo,3,1),"FOO");
		AssertEquals("String.Join(fu,foo,4,1)",String.Join(fu,foo,4,1),"OFO");
		AssertEquals("String.Join(fu,foo,5,1)",String.Join(fu,foo,5,1),"OOF");
		AssertEquals("String.Join(fu,foo,0,2)",String.Join(fu,foo,0,2),"foo fu ofo");
		AssertEquals("String.Join(fu,foo,1,2)",String.Join(fu,foo,1,2),"ofo fu oof");
		AssertEquals("String.Join(fu,foo,2,2)",String.Join(fu,foo,2,2),"oof fu FOO");
		AssertEquals("String.Join(fu,foo,3,2)",String.Join(fu,foo,3,2),"FOO fu OFO");
		AssertEquals("String.Join(fu,foo,4,2)",String.Join(fu,foo,4,2),"OFO fu OOF");
		AssertEquals("String.Join(fu,foo,0,3)",String.Join(fu,foo,0,3),"foo fu ofo fu oof");
		AssertEquals("String.Join(fu,foo,1,3)",String.Join(fu,foo,1,3),"ofo fu oof fu FOO");
		AssertEquals("String.Join(fu,foo,2,3)",String.Join(fu,foo,2,3),"oof fu FOO fu OFO");
		AssertEquals("String.Join(fu,foo,3,3)",String.Join(fu,foo,3,3),"FOO fu OFO fu OOF");
		AssertEquals("String.Join(fu,foo,0,4)",String.Join(fu,foo,0,4),"foo fu ofo fu oof fu FOO");
		AssertEquals("String.Join(fu,foo,1,4)",String.Join(fu,foo,1,4),"ofo fu oof fu FOO fu OFO");
		AssertEquals("String.Join(fu,foo,2,4)",String.Join(fu,foo,2,4),"oof fu FOO fu OFO fu OOF");
		AssertEquals("String.Join(fu,foo,0,5)",String.Join(fu,foo,0,5),"foo fu ofo fu oof fu FOO fu OFO");
		AssertEquals("String.Join(fu,foo,1,5)",String.Join(fu,foo,1,5),"ofo fu oof fu FOO fu OFO fu OOF");
		AssertEquals("String.Join(fu,foo,0,0)",String.Join(fu,foo,0,0),"");
		AssertEquals("String.Join(fu2,foo2,0,foo2.Length)",String.Join(fu2,foo2,0,foo2.Length),"");
		AssertEquals("String.Join(fu,foo3,0,foo3.Length)",String.Join(fu,foo3,0,foo3.Length),"");
		try
		{
			String.Join(fu,null);
			Fail("String.Join(fu,null) should throw an ArgumentNullException");
		}
		catch (ArgumentNullException)
		{
			// works
		}
		try
		{
			String.Join(fu,foo,0,foo.Length+1);
			Fail("String.Join(fu,foo,0,foo.Length+1) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// works
		}
	}

	public void TestStringLastIndexOf()
	{
		String foo = "Foo Bar foo bar fu bar Fu Bar";

		/* String.LastIndexOf(char) */
		AssertEquals("foo.LastIndexOf('r')",foo.LastIndexOf('r'),28);
		AssertEquals("foo.LastIndexOf('a')",foo.LastIndexOf('a'),27);
		AssertEquals("foo.LastIndexOf('B')",foo.LastIndexOf('B'),26);
		AssertEquals("foo.LastIndexOf(' ')",foo.LastIndexOf(' '),25);
		AssertEquals("foo.LastIndexOf('u')",foo.LastIndexOf('u'),24);
		AssertEquals("foo.LastIndexOf('F')",foo.LastIndexOf('F'),23);
		AssertEquals("foo.LastIndexOf('b')",foo.LastIndexOf('b'),19);
		AssertEquals("foo.LastIndexOf('f')",foo.LastIndexOf('f'),16);
		AssertEquals("foo.LastIndexOf('o')",foo.LastIndexOf('o'),10);
		AssertEquals("foo.LastIndexOf('c')",foo.LastIndexOf('c'),-1);

		/* String.LastIndexOf(char,int) */
		AssertEquals("foo.LastIndexOf('f',16)",foo.LastIndexOf('f',16),16);
		AssertEquals("foo.LastIndexOf('f',15)",foo.LastIndexOf('f',15),8);
		AssertEquals("foo.LastIndexOf('f',7)",foo.LastIndexOf('f',7),-1);
		AssertEquals("foo.LastIndexOf('f',foo.Length-1)",foo.LastIndexOf('f',foo.Length-1),16);
		try
		{
			AssertEquals("foo.LastIndexOf('f',foo.Length)",foo.LastIndexOf('f',foo.Length),16); // don't ask me
			Fail("foo.LastIndexOf('f',foo.Length) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
		}
		try
		{
			foo.LastIndexOf('f',-1);
			Fail("foo.LastIndexOf('f',-1) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// nothing wrong here
		}
		try
		{
			foo.LastIndexOf('f',foo.Length+1);
			Fail("foo.LastIndexOf('f',foo.Length+1) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// or here
		}

		/* String.LastIndexOf(char,int,int) */
		AssertEquals("foo.LastIndexOf('f',16,17)",foo.LastIndexOf('f',16,17),16);
		AssertEquals("foo.LastIndexOf('f',15,16)",foo.LastIndexOf('f',15,16),8);
		AssertEquals("foo.LastIndexOf('f',15,7)",foo.LastIndexOf('f',15,7),-1);
		AssertEquals("foo.LastIndexOf('f',foo.Length-1,1)",foo.LastIndexOf('f',foo.Length-1,1),-1);
		AssertEquals("foo.LastIndexOf('r',foo.Length-1,1)",foo.LastIndexOf('r',foo.Length-1,0),-1);
		AssertEquals("foo.LastIndexOf('r',foo.Length-1,1)",foo.LastIndexOf('r',foo.Length-1,1),28);
		AssertEquals("foo.LastIndexOf('F',0,1)",foo.LastIndexOf('F',0,1),0);
		AssertEquals("foo.LastIndexOf('F',1,1)",foo.LastIndexOf('F',1,1),-1);
		try
		{
			AssertEquals("foo.LastIndexOf('r',foo.Length,0)",foo.LastIndexOf('r',foo.Length,0),-1); // ask the ECMA
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf('r',foo.Length,0) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			AssertEquals("foo.LastIndexOf('r',foo.Length,1)",foo.LastIndexOf('r',foo.Length,1),-1); // b/c these are
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf('r',foo.Length,1) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			AssertEquals("foo.LastIndexOf('r',foo.Length,2)",foo.LastIndexOf('r',foo.Length,2),28); // all valid,
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf('r',foo.Length,2) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			AssertEquals("foo.LastIndexOf('f',foo.Length+10,0)",foo.LastIndexOf('f',foo.Length+10,0),-1); // believe
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf('f',foo.Length+10,0) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			AssertEquals("foo.LastIndexOf('f',foo.Length+10,1)",foo.LastIndexOf('f',foo.Length+10,1),-1); // it or not.
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf('f',foo.Length+10,1) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			AssertEquals("foo.LastIndexOf('f',foo.Length+10,11)",foo.LastIndexOf('f',foo.Length+10,11),-1); // amazing,
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf('f',foo.Length+10,11) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			AssertEquals("foo.LastIndexOf('r',foo.Length+10,11)",foo.LastIndexOf('r',foo.Length+10,11),28); // isn't it?
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf('r',foo.Length+10,11) should NOT throw an ArgumentOutOfRangeException");
		}	
		try
		{
			foo.LastIndexOf('f',-1,0);
			Fail("foo.LastIndexOf('f',-1,0) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// a-ok
		}
		try
		{
			foo.LastIndexOf('f',0,-1);
			Fail("foo.LastIndexOf('f',0,-1) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// no problems here
		}
		try
		{
			foo.LastIndexOf('f',0,2);
			Fail("foo.LastIndexOf('f',0,2) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// blah
		}

		/* String.LastIndexOf(String) */
		AssertEquals("foo.LastIndexOf(\"Bar\")",foo.LastIndexOf("Bar"),26);
		AssertEquals("foo.LastIndexOf(\"Fu\")",foo.LastIndexOf("Fu"),23);
		AssertEquals("foo.LastIndexOf(\"bar\")",foo.LastIndexOf("bar"),19);
		AssertEquals("foo.LastIndexOf(\"fu\")",foo.LastIndexOf("fu"),16);
		AssertEquals("foo.LastIndexOf(\"foo\")",foo.LastIndexOf("foo"),8);
		AssertEquals("foo.LastIndexOf(\"Foo\")",foo.LastIndexOf("Foo"),0);
		AssertEquals("foo.LastIndexOf(\"blah\")",foo.LastIndexOf("blah"),-1);
		AssertEquals("foo.LastIndexOf(\"\")",foo.LastIndexOf(""),0);
		try
		{
			foo.LastIndexOf((String)null);
			Fail("foo.LastIndexOf((String)null) should throw an ArgumentNullException");
		}
		catch (ArgumentNullException)
		{
			// looks good so far
		}

		/* String.LastIndexOf(String,int) */
		AssertEquals("foo.LastIndexOf(\"Bar\",foo.Length-1)",foo.LastIndexOf("Bar",foo.Length-1),26);
		AssertEquals("foo.LastIndexOf(\"Bar\",foo.Length-2)",foo.LastIndexOf("Bar",foo.Length-2),-1);
		AssertEquals("foo.LastIndexOf(\"Fu\",foo.Length-3)",foo.LastIndexOf("Fu",foo.Length-3),23);
		AssertEquals("foo.LastIndexOf(\"Fu\",foo.Length-6)",foo.LastIndexOf("Fu",foo.Length-6),-1);
		try
		{
			AssertEquals("foo.LastIndexOf(\"\",0)",foo.LastIndexOf("",0),0); // this is absurd,
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf(\"\",0) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			AssertEquals("foo.LastIndexOf(\"\",1)",foo.LastIndexOf("",1),0); // as is this
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf(\"\",1) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			foo.LastIndexOf((String)null,0);
			Fail("foo.LastIndexOf((String)null,0) should throw an ArgumentNullException");
		}
		catch (ArgumentNullException)
		{
			// move along, nothing to see here
		}
		try
		{
			foo.LastIndexOf("foo",-1);
			Fail("foo.LastIndexOf(\"foo\",-1) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// this works
		}
		try
		{
			foo.LastIndexOf("foo",foo.Length);
			Fail("foo.LastIndexOf(\"foo\",foo.Length) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// this checks out
		}
		try
		{
			foo.LastIndexOf("foo",foo.Length+1);
			Fail("foo.LastIndexOf(\"foo\",foo.Length+1) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// this checks out
		}

		/* String.LastIndexOf(String,int,int) */
		AssertEquals("foo.LastIndexOf(\"Bar\",foo.Length-1,foo.Length)",foo.LastIndexOf("Bar",foo.Length-1,foo.Length),26);
		AssertEquals("foo.LastIndexOf(\"Bar\",foo.Length-2,foo.Length-1)",foo.LastIndexOf("Bar",foo.Length-2,foo.Length-1),-1);
		AssertEquals("foo.LastIndexOf(\"Bar\",foo.Length-1,3)",foo.LastIndexOf("Bar",foo.Length-1,3),26);
		AssertEquals("foo.LastIndexOf(\"Bar\",foo.Length-2,3)",foo.LastIndexOf("Bar",foo.Length-2,3),-1);
		AssertEquals("foo.LastIndexOf(\"Bar\",foo.Length-1,2)",foo.LastIndexOf("Bar",foo.Length-1,2),-1);
		AssertEquals("foo.LastIndexOf(\"Fu\",foo.Length-3,foo.Length-2)",foo.LastIndexOf("Fu",foo.Length-3,foo.Length-2),23);
		AssertEquals("foo.LastIndexOf(\"Fu\",foo.Length-4,foo.Length-3)",foo.LastIndexOf("Fu",foo.Length-4,foo.Length-3),-1);
		AssertEquals("foo.LastIndexOf(\"Fu\",foo.Length-3,3)",foo.LastIndexOf("Fu",foo.Length-3,3),23);
		AssertEquals("foo.LastIndexOf(\"Fu\",foo.Length-4,3)",foo.LastIndexOf("Fu",foo.Length-4,3),-1);
		AssertEquals("foo.LastIndexOf(\"Fu\",foo.Length-6,foo.Length-5)",foo.LastIndexOf("Fu",foo.Length-6,foo.Length-5),-1);
		try
		{
			AssertEquals("foo.LastIndexOf(\"\",0,0)",foo.LastIndexOf("",0,0),0); // and the absurdity continues
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf(\"\",0,0) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			AssertEquals("foo.LastIndexOf(\"\",0,1)",foo.LastIndexOf("",0,1),0); // need I say more?
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf(\"\",0,1) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			AssertEquals("foo.LastIndexOf(\"\",1,0)",foo.LastIndexOf("",1,0),0); // ok, "more"
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf(\"\",1,0) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			AssertEquals("foo.LastIndexOf(\"\",1,1)",foo.LastIndexOf("",1,1),0); // and more
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("foo.LastIndexOf(\"\",1,1) should NOT throw an ArgumentOutOfRangeException");
		}
		try
		{
			foo.LastIndexOf((String)null,0,0);
			Fail("foo.LastIndexOf((String)null,0,0) should throw an ArgumentNullException");
		}
		catch (ArgumentNullException)
		{
			// doing good
		}
		try
		{
			foo.LastIndexOf("foo",-1,0);
			Fail("foo.LastIndexOf(\"foo\",-1,0) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// even better
		}
		try
		{
			foo.LastIndexOf("foo",0,-1);
			Fail("foo.LastIndexOf(\"foo\",0,-1) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// doing great, so far
		}
		try
		{
			foo.LastIndexOf("foo",0,2);
			Fail("foo.LastIndexOf(\"foo\",0,2) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// yay! we made it
		}
	}

	public void TestStringLastIndexOfAny()
	{
		String foo = "Foo Bar foo bar fu bar Fu Bar";

		/* String.LastIndexOfAny(char[]) */
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'})",28,foo.LastIndexOfAny(new char[] {'r','a','B'}));
		AssertEquals("foo.LastIndexOfAny(new char[] {'f','a','B'})",27,foo.LastIndexOfAny(new char[] {'f','a','B'}));
		AssertEquals("foo.LastIndexOfAny(new char[] {'r'})",28,foo.LastIndexOfAny(new char[] {'r'}));
		AssertEquals("foo.LastIndexOfAny(new char[] {'B'})",26,foo.LastIndexOfAny(new char[] {'B'}));
		AssertEquals("foo.LastIndexOfAny(new char[] {'B','B'})",26,foo.LastIndexOfAny(new char[] {'B','B'}));
		AssertEquals("foo.LastIndexOfAny(new char[] {'B','u'})",26,foo.LastIndexOfAny(new char[] {'B','u'}));
		AssertEquals("foo.LastIndexOfAny(new char[] {'u','B'})",26,foo.LastIndexOfAny(new char[] {'u','B'}));
		AssertEquals("foo.LastIndexOfAny(new char[] {'u','f'})",foo.LastIndexOfAny(new char[] {'u','f'}),24);
		AssertEquals("foo.LastIndexOfAny(new char[] {'F','f'})",23,foo.LastIndexOfAny(new char[] {'F','f'}));
		AssertEquals("foo.LastIndexOfAny(new char[] {'F','q'})",23,foo.LastIndexOfAny(new char[] {'F','q'}));
		AssertEquals("foo.LastIndexOfAny(new char[] {'p','q'})",-1,foo.LastIndexOfAny(new char[] {'p','q'}));
		try
		{
			foo.LastIndexOfAny((char[])null);
			Fail("foo.LastIndexOfAny((char[])null) should throw an ArgumentNullException");
		}
		catch (ArgumentNullException)
		{
			// this is good
		}

		/* String.LastIndexOfAny(char[],int) */
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length-1)",28,foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length-1));
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length-2)",27,foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length-2));
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'},4)",4,foo.LastIndexOfAny(new char[] {'r','a','B'},4));
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'},5)",5,foo.LastIndexOfAny(new char[] {'r','a','B'},5));
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'},6)",6,foo.LastIndexOfAny(new char[] {'r','a','B'},6));
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'},1)",-1,foo.LastIndexOfAny(new char[] {'r','a','B'},1));
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'},0)",-1,foo.LastIndexOfAny(new char[] {'r','a','B'},0));
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'},3)",-1,foo.LastIndexOfAny(new char[] {'r','a','B'},3));
		try
		{
			foo.LastIndexOfAny((char[])null,0);
			Fail("foo.LastIndexOfAny((char[])null,0) should throw an ArgumentNullException");
		}
		catch (ArgumentNullException)
		{
			// caught it
		}
		try
		{
			foo.LastIndexOfAny(new char[] {'r','a','B'},-1);
			Fail("foo.LastIndexOfAny(new char[] {'r','a','B'},-1); should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// caught another one
		}
		try
		{
			foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length);
			Fail("foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length); should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// caught this one too
		}
		try
		{
			foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length+1);
			Fail("foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length+1); should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// and this one
		}

		/* String.LastIndexOfAny(char[],int,int) */
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length-1,1)",foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length-1,1),28);
		AssertEquals("foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length-2,2)",foo.LastIndexOfAny(new char[] {'r','a','B'},foo.Length-2,2),27);
		AssertEquals("foo.LastIndexOfAny(new char[] {'B'},foo.Length-1,foo.Length)",foo.LastIndexOfAny(new char[] {'B'},foo.Length-1,foo.Length),26);
		AssertEquals("foo.LastIndexOfAny(new char[] {'F'},foo.Length-10,foo.Length-10)",foo.LastIndexOfAny(new char[] {'F'},foo.Length-10,foo.Length-10),-1);
		try
		{
			foo.LastIndexOfAny((char[])null,0,0);
			Fail("foo.LastIndexOfAny((char[])null,0,0) should throw an ArgumentNullException");
		}
		catch (ArgumentNullException)
		{
			// all good here
		}
		try
		{
			foo.LastIndexOfAny(new char[] {'r','a','B'},-1,0);
			Fail("foo.LastIndexOfAny(new char[] {'r','a','B'},-1,0); should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// and here
		}
		try
		{
			foo.LastIndexOfAny(new char[] {'r','a','B'},0,-1);
			Fail("foo.LastIndexOfAny(new char[] {'r','a','B'},0,-1); should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// and here too
		}
		try
		{
			foo.LastIndexOfAny(new char[] {'r','a','B'},0,2);
			Fail("foo.LastIndexOfAny(new char[] {'r','a','B'},0,2); should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// and here as well
		}
		try
		{
			foo.LastIndexOfAny(new char[] {'F'},foo.Length,foo.Length);
			Fail("foo.LastIndexOfAny(new char[] {'F'},foo.Length,foo.Length) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// and here as well
		}
		try
		{
			foo.LastIndexOfAny(new char[] {'F'},foo.Length-10,10);
			Fail ("foo.LastIndexOfAny(new char[] {'F'},foo.Length-10,10) should thrown an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// and here as well
		}
	}

	public void TestStringPadLeft()
	{
		String foo = "FooBar";

		/* String.PadLeft(int) */
		AssertEquals("foo.PadLeft(foo.Length)",foo.PadLeft(foo.Length),foo);
		AssertEquals("foo.PadLeft(foo.Length+1)",foo.PadLeft(foo.Length+1),(" FooBar"));
		AssertEquals("foo.PadLeft(foo.Length-1)",foo.PadLeft(foo.Length-1),foo);
		AssertEquals("foo.PadLeft(0)",foo.PadLeft(0),foo);
		try
		{
			foo.PadLeft(-1);
			Fail("foo.PadLeft(-1) should throw an ArgumentException");
		}
		catch (ArgumentException)
		{
			// range check works
		}

		/* String.PadLeft(int,char) */
		AssertEquals("foo.PadLeft(foo.Length,'_')",foo.PadLeft(foo.Length,'_'),foo);
		AssertEquals("foo.PadLeft(foo.Length+1,'_')",foo.PadLeft(foo.Length+1,'_'),("_FooBar"));
		AssertEquals("foo.PadLeft(foo.Length-1,'_')",foo.PadLeft(foo.Length-1,'_'),foo);
		AssertEquals("foo.PadLeft(0,'_')",foo.PadLeft(0,'_'),foo);
		try
		{
			foo.PadLeft(-1,'_');
			Fail("foo.PadLeft(-1,'_') should throw an ArgumentException");
		}
		catch (ArgumentException)
		{
			// range check works here too
		}
	}

	public void TestStringPadRight()
	{
		String foo = "FooBar";

		/* String.PadRight(int) */
		AssertEquals("foo.PadRight(foo.Length)",foo.PadRight(foo.Length),foo);
		AssertEquals("foo.PadRight(foo.Length+1)",foo.PadRight(foo.Length+1),("FooBar "));
		AssertEquals("foo.PadRight(foo.Length-1)",foo.PadRight(foo.Length-1),foo);
		AssertEquals("foo.PadRight(0)",foo.PadRight(0),foo);
		try
		{
			foo.PadRight(-1);
			Fail("foo.PadRight(-1) should throw an ArgumentException");
		}
		catch (ArgumentException)
		{
			// range check works
		}

		/* String.PadRight(int,char) */
		AssertEquals("foo.PadRight(foo.Length,'_')",foo.PadRight(foo.Length,'_'),foo);
		AssertEquals("foo.PadRight(foo.Length+1,'_')",foo.PadRight(foo.Length+1,'_'),("FooBar_"));
		AssertEquals("foo.PadRight(foo.Length-1,'_')",foo.PadRight(foo.Length-1,'_'),foo);
		AssertEquals("foo.PadRight(0,'_')",foo.PadRight(0,'_'),foo);
		try
		{
			foo.PadRight(-1,'_');
			Fail("foo.PadRight(-1,'_') should throw an ArgumentException");
		}
		catch (ArgumentException)
		{
			// range check works here too
		}
	}

	public void TestStringRemove()
	{
		String foo = "Foo Bar";
		AssertEquals("foo.Remove(0,foo.Length)",foo.Remove(0,foo.Length),"");
		AssertEquals("foo.Remove(1,foo.Length-1)",foo.Remove(1,foo.Length-1),"F");
		AssertEquals("foo.Remove(0,1)",foo.Remove(0,1),"oo Bar");
		AssertEquals("foo.Remove(0,0)",foo.Remove(0,0),foo);
		AssertEquals("foo.Remove(foo.Length,0)",foo.Remove(foo.Length,0),foo);
		AssertEquals("foo.Remove(3,1)",foo.Remove(3,1),"FooBar");
		AssertEquals("foo.Remove(foo.Length-1,1)",foo.Remove(foo.Length-1,1),"Foo Ba");
		try
		{
			foo.Remove(-1,0);
			Fail("foo.Remove(-1,0) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// blah
		}
		try
		{
			foo.Remove(0,-1);
			Fail("foo.Remove(0,-1) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// blah blah
		}
		try
		{
			foo.Remove(0,foo.Length+1);
			Fail("foo.Remove(0,foo.Length+1) should throw an ArgumentOutOfRangeException");
		}
		catch (ArgumentOutOfRangeException)
		{
			// blah blah blah
		}
	}

	public void TestStringReplace()
	{
		String foo = "Foo Bar";

		/* String.Replace(char,char) */
		AssertEquals("foo.Replace('F','f')",foo.Replace('F','f'),"foo Bar");
		AssertEquals("foo.Replace(' ','_')",foo.Replace(' ','_'),"Foo_Bar");
		AssertEquals("foo.Replace('r','R')",foo.Replace('r','R'),"Foo BaR");
		AssertEquals("foo.Replace('_',' ')",foo.Replace('_',' '),foo);

		/* String.Replace(String,String) */
		AssertEquals("foo.Replace(\"Foo\",\"Fu\")",foo.Replace("Foo","Fu"),"Fu Bar");
		AssertEquals("foo.Replace(\"Fu\",\"Foo\")",foo.Replace("Fu","Foo"),foo);
		AssertEquals("foo.Replace(\"Foo Bar\",\"\")",foo.Replace("Foo Bar",""),"");
		AssertEquals("foo.Replace(\"Foo Bar\",null)",foo.Replace("Foo Bar",null),"");
		AssertEquals("foo.Replace(null,\"Foo Bar\")",foo.Replace(null,"Foo Bar"),foo); // I'm beginning to hate the ECMA -- Rich
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
		// if you pass Length, SHOULD NOT THROW!
		// per ECMA spec
		try
		{
			"x".Substring(1, 0);
			"x".Substring(1);
		}
		catch (ArgumentOutOfRangeException)
		{
			Fail("Substring should not throw when passed Length as the startIndex!");
		}
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
		String foo = "Foo Bar";
		String fu = "Fu Bar";
		Assert("!(foo == fu)",!(foo == fu));
		Assert("foo == foo",foo == foo);
		Assert("fu == fu",fu == fu);
		Assert("foo == String.Copy(foo)",foo == String.Copy(foo));
		Assert("fu == String.Copy(fu)",fu == String.Copy(fu));
	}

	public void TestStringop_Inequality()
	{
		String foo = "Foo Bar";
		String fu = "Fu Bar";
		Assert("foo != fu",foo != fu);
		Assert("!(foo != foo)",!(foo != foo));
		Assert("!(fu != fu)",!(fu != fu));
		Assert("!(foo != String.Copy(foo))",!(foo != String.Copy(foo)));
		Assert("!(fu != String.Copy(fu))",!(fu != String.Copy(fu)));
	}

	public void TestChars()
	{
		char[] fu = new char[] { 'F', 'o', 'o', ' ', 'B', 'a', 'r' };
		String foo = new String(fu);
		for (int i = 0; i < foo.Length; i++)
		{
			Assert("foo["+i+"] == fu["+i+"]",foo[i] == fu[i]);
		}
		try
		{
			int i = foo[-1];
			Fail("foo[-1] should throw an IndexOutOfRangeException");
		}
		catch (IndexOutOfRangeException)
		{
			// works here
		}
		try
		{
			int i = foo[foo.Length];
			Fail("foo[foo.Length] should throw an IndexOutOfRangeException");
		}
		catch (IndexOutOfRangeException)
		{
			// and here
		}
		try
		{
			int i = foo[foo.Length+1];
			Fail("foo[foo.Length+1] should throw an IndexOutOfRangeException");
		}
		catch (IndexOutOfRangeException)
		{
			// and here
		}
	}
	public void TestLength()
	{
		AssertEquals("\"Foo Bar\".Length","Foo Bar".Length,7);
		AssertEquals("\"\".Length","".Length,0);
	}

}; // class TestString
