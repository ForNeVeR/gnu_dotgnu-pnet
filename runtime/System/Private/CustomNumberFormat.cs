/*
 * CustomNumberFormat.cs - Implementation of "System.Private.CustomNumberFormat".
 *
 * Copyright (C) 2002 Richard Baumann.
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

using System;
using System.Text;
using System.Globalization;

namespace System.Private
{

internal sealed class CustomNumberFormat
{
	private enum Type
	{
		ZERODIGIT = 0,
		DECIMAL = 1,
		GROUPSCALE = 2,
		PERCENT = 3,
		ENGINEERING = 4,
		LITERAL = 5,
		NONE = 6
	};

	private sealed class Node
	{
		internal Type type;
		internal Node next = null;
		internal Node prev = null;
		internal String data = "";

		internal Node(Type type) : this(type,"") {}
		internal Node(Type type, String data) : base()
		{
			this.type = type;
			this.data = data;
		}

		internal void Add(Node next)
		{
			Node cur = this;
			while (cur.next != null) { cur = cur.next; }
			cur.next = next;
			next.prev = cur;
		}
	}

	private sealed class Buffer
	{
		private char[] buf = null;
		private int pos = 0;
		private static String hex = "0123456789abcdefABCDEF";
		private static String tok = "0#.,%Ee'\";";

		internal Buffer(char[] buf) : base()
		{
			this.buf = buf;
		}

		internal Node Token
		{
			get
			{
				if (pos == buf.Length)
				{
					return null;
				}
				else if (buf[pos] == '0' || buf[pos] == '#')
				{
					int start = pos;
					pos++;
					while (pos < buf.Length && (buf[pos] == '0' || buf[pos] == '#'))
					{
						pos++;
					}
					return new Node(Type.ZERODIGIT,new String(buf,start,pos - start - 1));
				}
				else if (buf[pos] == '.')
				{
					pos++;
					return new Node(Type.DECIMAL,".");
				}
				else if (buf[pos] == ',')
				{
					int start = pos;
					pos++;
					while (pos < buf.Length && buf[pos] == ',') { pos++; }
					return new Node(Type.GROUPSCALE,new String(buf,start,pos - start - 1));
				}
				else if (buf[pos] == '%')
				{
					pos++;
					return new Node(Type.PERCENT,"");
				}
				else if (buf[pos] == 'E' || buf[pos] == 'e')
				{
					pos++;
					if (pos == buf.Length) { throw new FormatException(); } // TODO: add custom msg
					else if (buf[pos] != '-' && buf[pos] != '+' && buf[pos] != '0')
					{
						throw new FormatException(); // TODO: add custom msg
					}
					else
					{
						int start = pos-1;
						if (buf[pos] == '-' || buf[pos] == '+') { pos++; }
						while (pos < buf.Length && buf[pos] == '0') { pos++; }
						return new Node(Type.ENGINEERING,new String(buf,start,pos - start - 1));
					}
				}
				else if (buf[pos] == '\'' || buf[pos] == '"')
				{
					pos++;
					int start = pos-1;//i = pos+1;
					while (pos < buf.Length && buf[pos] != buf[start]) { pos++; }
					pos++;
					return new Node(Type.LITERAL,new String(buf,start,pos - start - 1));
				}
				else
				{
					int start;
					for (start = pos; pos < buf.Length; pos++)
					{
						if (tok.IndexOf(buf[pos]) != -1)
						{
							return new Node(Type.LITERAL,new String(buf,start,pos - start - 1));
						}
					}
					return new Node(Type.LITERAL,new String(buf,start,pos - start - 1));
				}
			}
		}
	}



	private Node[] format = new Node[3] { null, null, null };
	private bool has_dot = false;
	private int before_dot_min = 0;
	private int after_dot_min = 0;
	private int before_dot_max = 0;
	private int after_dot_max = 0;
	private int groups = 0;
	private int scale = 0;
	private String engineering = null;
	private String before_num = null;
	private String after_num = null;
	private String orig = null;
	private NumberFormatInfo info = null;



	internal bool HasDot
	{
		get { return has_dot; }
	}
	internal int BeforeDotMin
	{
		get { return before_dot_min; }
	}
	internal int AfterDotMin
	{
		get { return after_dot_min; }
	}
	internal int BeforeDotMax
	{
		get { return before_dot_max; }
	}
	internal int AfterDotMax
	{
		get { return after_dot_max; }
	}
	internal int Groups
	{
		get { return groups; }
	}
	internal int Scale
	{
		get { return scale; }
	}
	internal String Engineering
	{
		get { return engineering; }
	}
	internal String BeforeNum
	{
		get
		{
			if (before_num == null)
			{
				StringBuilder s = new StringBuilder();
				for (Node cur = format[0]; cur != null; cur = cur.next)
				{
					if (cur.type == Type.LITERAL)
					{
						s.Append(cur.data);
					}
					else if (cur.type == Type.PERCENT)
					{
						s.Append(info.PercentSymbol);
					}
				}
				before_num = s.ToString();
			}
			return before_num;
		}
	}
	internal String AfterNum
	{
		get
		{
			if (after_num == null)
			{
				StringBuilder s = new StringBuilder();
				for (Node cur = format[2]; cur != null; cur = cur.next)
				{
					if (cur.type == Type.LITERAL)
					{
						s.Append(cur.data);
					}
					else if (cur.type == Type.PERCENT)
					{
						s.Append(info.PercentSymbol);
					}
				}
				after_num = s.ToString();
			}
			return after_num;
		}
	}
	internal String Format
	{
		get { return orig; }
		set
		{
			if (value == null)
			{
				value = "";
			}
			orig = value;
			ResetParse();
			ParseFormat();
		}
	}
	internal NumberFormatInfo Info
	{
		get { return info; }
		set
		{
			if (value == null)
			{
				value = new NumberFormatInfo();
			}
			info = value;
			ResetInfo();
		}
	}

	internal CustomNumberFormat(String format, NumberFormatInfo nfi) : base()
	{
		Format = format;
		Info = nfi;
	}

	private void ResetInfo()
	{
		before_num = null;
		after_num = null;
	}

	private void ResetParse()
	{
		format = new Node[3] { null, null, null };
		has_dot = false;
		before_dot_min = 0;
		after_dot_min = 0;
		before_dot_max = 0;
		after_dot_max = 0;
		groups = 0;
		scale = 0;
		engineering = null;
		before_num = null;
		after_num = null;
	}

	private void ParseFormat()
	{
		bool has_percent = false;
		bool has_decimal = false;
		Buffer buf = new Buffer(orig.ToCharArray());

		format[0] = new Node(Type.NONE);
		int section = 0;
		for (Node token = buf.Token, last = format[0]; token != null; last = token, token = buf.Token)
		{
			bool type = (token.type == Type.LITERAL || token.type == Type.ENGINEERING || token.type == Type.PERCENT);
			bool nextsec = (section == 0 && !type) || (section == 1 && type);
			if (nextsec)
			{
				section++;
				format[section] = new Node(Type.NONE);
				last = format[section];
			}
			last.Add(token);
		}

		for (int i = 0; i <= section; i++)
		{
			format[i] = format[i].next; // skip initial NONE node
			if (format[i] != null && i != 1)
			{
				format[i].prev = null; // detach initial NONE node
				for (Node cur = format[i]; cur != null; cur = cur.next)
				{
					if (cur.type == Type.LITERAL) {}
					else if (cur.type == Type.PERCENT)
					{
						has_percent = true;
					}
					else if (cur.type == Type.ENGINEERING)
					{
						if (engineering == null)
						{
							engineering = cur.data;
						}
						else if (engineering != cur.data)
						{
							throw new FormatException(); // TODO: add custom msg
						}
						cur.data = "";
						cur.type = Type.NONE;
					}
					else if (cur.type == Type.ZERODIGIT) // LITERAL+NUMBER+(LITERAL+NUMBER...)
					{
						throw new FormatException();
					}
					else if (cur.type == Type.DECIMAL) // LITERAL+NUMBER+(LITERAL+NUMBER...)
					{
						if (!has_decimal) { throw new FormatException(); } // left most '.' must be the decimal point
						cur.data = "";
						cur.type = Type.NONE;
					}
					else // if (cur.type == Type.GROUPSCALE) // LITERAL+NUMBER+(LITERAL+NUMBER...)
					{
						throw new FormatException();
					}
				}
			}
			else if (format[i] != null)
			{
				format[i].prev = null; // detach initial NONE node
				for (Node cur = format[i]; cur != null; cur = cur.next)
				{
					if (cur.type == Type.ZERODIGIT) {}
					else if (cur.type == Type.DECIMAL)
					{
						if (!has_decimal)
						{
							has_decimal = true;
						}
						else
						{
							cur.data = "";
							cur.type = Type.NONE;
						}
					}
					else // if (cur.type == Type.GROUPSCALE)
					{
						if (has_decimal)
						{
							throw new FormatException(); // TODO: add custom msg
						}

						Node tmp = cur;
						int count = cur.data.Length;
						while (tmp.next != null && tmp.next.type == Type.GROUPSCALE)
						{
							count += tmp.data.Length;
							tmp = tmp.next;
						}

						if (tmp.next == null || tmp.next.type == Type.DECIMAL)
						{
							scale = count;
						}
						else if (count != 1 || cur.prev == null)
						{
							throw new FormatException(); // TODO: add custom msg
						}
						else
						{
							groups++;
						}

						if (tmp.next != null)
						{
							tmp.next.prev = cur;
						}
						cur.next = tmp.next;
						cur.data = "";
						cur.type = Type.NONE;
					}
				}
			}
		}

		// get rid of any NONE nodes
		for (int i = 0; i <= section; i++)
		{
			if (format[i] != null)
			{
				for (Node cur = format[i]; cur != null; cur = cur.next)
				{
					if (cur.type == Type.NONE)
					{
						if (cur.next != null) { cur.next.prev = cur.prev; }
						if (cur.prev != null) { cur.prev.next = cur.next; }
						else { format[i] = cur.next; }
					}
				}
			}
		}
		// compress ZERODIGIT and LITERAL node sequences
		Type[] types = new Type[2] { Type.ZERODIGIT, Type.LITERAL };
		for (int i = 0; i <= section; i++)
		{
			bool found = false;
			if (format[i] != null)
			{
				for (Node cur = format[i]; cur != null; cur = cur.next)
				{
					found = false; // no point going through the loop anymore if the type has already been matched and handled :)
					for (int j = 0; j < types.Length && !found; j++)
					{
						if (cur.type == types[j])
						{
							found = true;
							for (Node tmp = cur.next; tmp != null && tmp.type == types[j]; tmp = tmp.next)
							{
								if (tmp.next != null) { tmp.next.prev = cur; }
								cur.next = tmp.next;
								cur.data += tmp.data;
							}
						}
					}
				}
			}
		}

		int[] first_zero = new int[2] { -1, -1 };
		int[] last_zero = new int[2] { -1, -1 };
		int[] len = new int[2] { 0, 0 };
		int dot = 0;
		for (Node tmp = format[1]; tmp != null; tmp = tmp.next)
		{
			if (tmp.type == Type.DECIMAL)
			{
				dot++;
			}
			else
			{
				len[dot] = tmp.data.Length;
				first_zero[dot] = tmp.data.IndexOf('0');
				last_zero[dot] = tmp.data.LastIndexOf('0');
			}
		}
		before_dot_max = len[0];
		before_dot_min = (first_zero[0] == -1) ? 0 : ((dot == 1) ? (len[0] - first_zero[0]) : (last_zero[0] - first_zero[0] + 1));
		after_dot_max = len[1];
		after_dot_min = (last_zero[1] == -1) ? 0 : (len[1] - last_zero[1]);
		scale = (3 * scale) - (has_percent ? 2 : 0);
		has_dot = has_decimal;
	}

}; // class CustomNumberFormat

}; // namespace System.Private
