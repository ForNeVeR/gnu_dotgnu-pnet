/*
 * Guid.cs - Implementation of the "System.Guid" class.
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

namespace System
{

#if !ECMA_COMPAT

using System.Runtime.CompilerServices;
using System.Private;

public struct Guid : IFormattable, IComparable
{
	// The empty GUID.
	public static readonly Guid Empty;

	// Internal state.
	private int a__;
	private short b__, c__;
	private byte d__, e__, f__, g__, h__, i__, j__, k__;

	// Create a new GUID, initialized to a unique random value.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Guid NewGuid();

	// Constructors.
	public Guid(String g)
			{
				// TODO
				a__ = 0;
				b__ = 0;
				c__ = 0;
				d__ = 0;
				e__ = 0;
				f__ = 0;
				g__ = 0;
				h__ = 0;
				i__ = 0;
				j__ = 0;
				k__ = 0;
			}
	public Guid(byte[] b)
			{
				if(b == null)
				{
					throw new ArgumentNullException("b");
				}
				if(b.Length != 16)
				{
					throw new ArgumentException(_("Arg_GuidArray16"));
				}
				a__ = ((int)(b[0])) | (((int)(b[1])) << 8) |
					  (((int)(b[2])) << 16) | (((int)(b[3])) << 24);
				b__ = (short)(((int)(b[4])) | (((int)(b[5])) << 8));
				c__ = (short)(((int)(b[6])) | (((int)(b[7])) << 8));
				d__ = b[8];
				e__ = b[9];
				f__ = b[10];
				g__ = b[11];
				h__ = b[12];
				i__ = b[13];
				j__ = b[14];
				k__ = b[15];
			}
	public Guid(int a, short b, short c, byte[] d)
			{
				if(d == null)
				{
					throw new ArgumentNullException("d");
				}
				if(d.Length != 8)
				{
					throw new ArgumentException(_("Arg_GuidArray8"));
				}
				a__ = a;
				b__ = b;
				c__ = c;
				d__ = d[0];
				e__ = d[1];
				f__ = d[2];
				g__ = d[3];
				h__ = d[4];
				i__ = d[5];
				j__ = d[6];
				k__ = d[7];
			}
	public Guid(int a, short b, short c, byte d, byte e, byte f, byte g,
			    byte h, byte i, byte j, byte k)
			{
				a__ = a;
				b__ = b;
				c__ = c;
				d__ = d;
				e__ = e;
				f__ = f;
				g__ = g;
				h__ = h;
				i__ = i;
				j__ = j;
				k__ = k;
			}
	[CLSCompliant(false)]
	public Guid(uint a, ushort b, ushort c, byte d, byte e, byte f, byte g,
			    byte h, byte i, byte j, byte k)
			{
				a__ = unchecked((int)a);
				b__ = unchecked((short)b);
				c__ = unchecked((short)c);
				d__ = d;
				e__ = e;
				f__ = f;
				g__ = g;
				h__ = h;
				i__ = i;
				j__ = j;
				k__ = k;
			}

	// Implement the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is Guid))
					{
						throw new ArgumentException(_("Arg_MustBeGuid"));
					}
					Guid temp = (Guid)value;
					if(((uint)a__) < ((uint)(temp.a__)))
					{
						return -1;
					}
					else if(((uint)a__) > ((uint)(temp.a__)))
					{
						return 1;
					}
					if(((ushort)b__) < ((ushort)(temp.b__)))
					{
						return -1;
					}
					else if(((ushort)b__) > ((ushort)(temp.b__)))
					{
						return 1;
					}
					if(((ushort)c__) < ((ushort)(temp.c__)))
					{
						return -1;
					}
					else if(((ushort)c__) > ((ushort)(temp.c__)))
					{
						return 1;
					}
					if(d__ < temp.d__)
					{
						return -1;
					}
					else if(d__ > temp.d__)
					{
						return 1;
					}
					if(e__ < temp.e__)
					{
						return -1;
					}
					else if(e__ > temp.e__)
					{
						return 1;
					}
					if(f__ < temp.f__)
					{
						return -1;
					}
					else if(f__ > temp.f__)
					{
						return 1;
					}
					if(g__ < temp.g__)
					{
						return -1;
					}
					else if(g__ > temp.g__)
					{
						return 1;
					}
					if(h__ < temp.h__)
					{
						return -1;
					}
					else if(h__ > temp.h__)
					{
						return 1;
					}
					if(i__ < temp.i__)
					{
						return -1;
					}
					else if(i__ > temp.i__)
					{
						return 1;
					}
					if(j__ < temp.j__)
					{
						return -1;
					}
					else if(j__ > temp.j__)
					{
						return 1;
					}
					return 0;
				}
				else
				{
					return 1;
				}
			}

	// Determine if two Guid objects are equal.
	public override bool Equals(Object obj)
			{
				if(obj is Guid)
				{
					Guid temp = (Guid)obj;
					return (a__ == temp.a__ &&
					        b__ == temp.b__ &&
					        c__ == temp.c__ &&
					        d__ == temp.d__ &&
					        e__ == temp.e__ &&
					        f__ == temp.f__ &&
					        g__ == temp.g__ &&
					        h__ == temp.h__ &&
					        i__ == temp.i__ &&
					        j__ == temp.j__ &&
					        k__ == temp.k__);
				}
				else
				{
					return false;
				}
			}

	// Get a hash code for this Guid object.
	public override int GetHashCode()
			{
				return (a__ ^ ((int)b__) ^ (((int)(c__)) << 16) ^
						((int)d__) ^ (((int)e__) << 8) ^
						(((int)f__) << 16) ^ (((int)g__) << 24) ^
						((int)h__) ^ (((int)i__) << 8) ^
						(((int)j__) << 16) ^ (((int)k__) << 24));
			}

	// Convert this Guid into a byte array.
	public byte[] ToByteArray()
			{
				byte[] bytes = new byte [16];
				bytes[0]  = (byte)(a__);
				bytes[1]  = (byte)(a__ >> 8);
				bytes[2]  = (byte)(a__ >> 16);
				bytes[3]  = (byte)(a__ >> 24);
				bytes[4]  = (byte)(b__);
				bytes[5]  = (byte)(b__ >> 8);
				bytes[6]  = (byte)(c__);
				bytes[7]  = (byte)(c__ >> 8);
				bytes[8]  = d__;
				bytes[9]  = e__;
				bytes[10] = f__;
				bytes[11] = g__;
				bytes[12] = h__;
				bytes[13] = i__;
				bytes[14] = j__;
				bytes[15] = k__;
				return bytes;
			}

	// Convert this Guid into a string.
	public override String ToString()
			{
				return ToString("D", null);
			}
	public String ToString(String format)
			{
				return ToString(format, null);
			}
	public String ToString(String format, IFormatProvider provider)
			{
				String start, end, sep;
				if(format == "B")
				{
					start = "{";
					end = "}";
					sep = "-";
				}
				else if(format == "D")
				{
					start = "";
					end = "";
					sep = "-";
				}
				else if(format == "N")
				{
					start = "";
					end = "";
					sep = "";
				}
				else if(format == "P")
				{
					start = "(";
					end = ")";
					sep = "-";
				}
				else
				{
					throw new FormatException(_("Format_Guid"));
				}
				return start + NumberFormatter.FormatInBase(a__, 16, 32) +
					   sep   + NumberFormatter.FormatInBase(b__, 16, 16) +
					   sep   + NumberFormatter.FormatInBase(c__, 16, 16) +
					   sep   + NumberFormatter.FormatInBase
					   				((((int)d__) << 8) | ((int)e__), 16, 16) +
					   sep   + NumberFormatter.FormatInBase
					   				((((int)f__) << 8) | ((int)g__), 16, 16) +
					   		   NumberFormatter.FormatInBase
					   				((((int)h__) << 24) |
									 (((int)i__) << 16) |
									 (((int)j__) << 8) |
									  ((int)k__), 16, 32) +
					   end;
			}

	// Operators.
	public static bool operator==(Guid a, Guid b)
			{
				return (a.a__ == b.a__ &&
				        a.b__ == b.b__ &&
				        a.c__ == b.c__ &&
				        a.d__ == b.d__ &&
				        a.e__ == b.e__ &&
				        a.f__ == b.f__ &&
				        a.g__ == b.g__ &&
				        a.h__ == b.h__ &&
				        a.i__ == b.i__ &&
				        a.j__ == b.j__ &&
				        a.k__ == b.k__);
			}
	public static bool operator!=(Guid a, Guid b)
			{
				return (a.a__ != b.a__ ||
				        a.b__ != b.b__ ||
				        a.c__ != b.c__ ||
				        a.d__ != b.d__ ||
				        a.e__ != b.e__ ||
				        a.f__ != b.f__ ||
				        a.g__ != b.g__ ||
				        a.h__ != b.h__ ||
				        a.i__ != b.i__ ||
				        a.j__ != b.j__ ||
				        a.k__ != b.k__);
			}

}; // struct Guid

#endif // !ECMA_COMPAT

}; // namespace System
