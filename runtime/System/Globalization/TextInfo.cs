/*
 * TextInfo.cs - Implementation of the "System.TextInfo" class.
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

namespace System.Globalization
{

using System;
using System.Text;

public class TextInfo
{
	/*
	 * Note: This class has been hacked together , it is not
	 * optimised ... And these should be virtual right ?..
	 */
	// Convert characters or strings to lower case.
	public virtual char ToLower(char c)
			{
				if(c>='A' && c<='Z')
					c=(char)(c+'a'-'A');
				return c;
			}
	public virtual String ToLower(String s)
			{
				if(s==null)
				{
					throw new ArgumentNullException("s");
				}
				for(int i=0;i<s.Length;i++)
				{
					s.SetChar(i,ToLower(s[i]));
				}
				return s;
			}

	// Convert characters or strings to upper case.
	public virtual char ToUpper(char c)
			{
				if(c>='a' && c<='z')
					c=(char)(c-'a'+'A');
				return c;
			}
	public virtual String ToUpper(String s)
			{
				if(s==null)
				{
					throw new ArgumentNullException("s");
				}
				for(int i=0;i<s.Length;i++)
				{
					s.SetChar(i,ToUpper(s[i]));
				}
				return s;
			}

	// Convert a string to title case.
	public virtual String ToTitleCase(String s)
			{
				if(s==null)
				{
					throw new ArgumentNullException("s");
				}
				StringBuilder sb=new StringBuilder(s.Length);
				int start=0;
				for(int i=0;i<s.Length;i++)
				{
					if(CharacterInfo.IsSeparator(s[i]))
					{
						String word=(s.Substring(start,i+1-start)).ToLower();
						word.SetChar(0,ToUpper(word[0]));
						sb.Append(word);
						start=i+1;
					}
				}
				return s;
			}

// TODO

}; // class TextInfo

}; // namespace System.Globalization
