/*
 * BigNum.cs - Implementation of the
 *		"System.Security.Cryptography.BigNum" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.Security.Cryptography
{

#if !ECMA_COMPAT

using System;

// This class implements arithmetic on big unsigned integers.
// It is intended for use in implementing cryptographic operations
// such as RSA and DSA.  The operators always clear their arguments,
// to prevent information leakage, so this class isn't suitable for
// general-purpose use.  If you want to use the same value twice,
// you must call "Clone()", and then use the cloned copy.

internal sealed class BigNum : ICloneable
{
	// Internal state.
	private uint[] words;
	private int size;

	// Constructors.
	public BigNum(uint value)
			{
				words = new uint [1];
				words[0] = value;
				if(value != 0)
				{
					size = 1;
				}
				else
				{
					size = 0;
				}
			}
	public BigNum(byte[] value)
			{
				// TODO
			}
	private BigNum(uint[] words)
			{
				// Trim leading zeros from the incoming value.
				int size = words.Length;
				while(size > 0 && words[size - 1] == 0)
				{
					--size;
				}
				this.words = words;
				this.size = size;
			}

	// Destructor.
	~BigNum()
			{
				words.Initialize();
			}

	// Clear a big number.
	public void Clear()
			{
				words.Initialize();
				size = 0;
			}

	// Convert a big number into a byte array in big-endian order.
	public byte[] ToBytes()
			{
				byte[] result;
				int resPosn, posn;
				uint top;
				if(size == 0)
				{
					result = new byte [1];
					result[0] = 0;
				}
				else
				{
					top = words[size - 1];
					if((top & (uint)0xFF000000) != 0)
					{
						result = new byte [size * 4];
						resPosn = 0;
						posn = size;
					}
					else if((top & (uint)0x00FF0000) != 0)
					{
						result = new byte [size * 4 - 1];
						result[0] = (byte)(top >> 16);
						result[1] = (byte)(top >> 8);
						result[2] = (byte)top;
						resPosn = 3;
						posn = size - 1;
					}
					else if((top & (uint)0x0000FF00) != 0)
					{
						result = new byte [size * 4 - 2];
						result[0] = (byte)(top >> 8);
						result[1] = (byte)top;
						resPosn = 2;
						posn = size - 1;
					}
					else
					{
						result = new byte [size * 4 - 3];
						result[0] = (byte)top;
						resPosn = 1;
						posn = size - 1;
					}
					while(posn > 0)
					{
						--posn;
						top = words[posn];
						result[resPosn++] = (byte)(top >> 24);
						result[resPosn++] = (byte)(top >> 16);
						result[resPosn++] = (byte)(top >> 8);
						result[resPosn++] = (byte)top;
					}
				}
				return result;
			}

	// Add big numbers.
	public static BigNum operator+(BigNum x, BigNum y)
			{
				// Unpack the parameters for quicker access.
				int xsize = x.size;
				int ysize = y.size;
				uint[] xwords = x.words;
				uint[] ywords = y.words;

				// Handle the simple cases of "add zero" first.
				if(xsize == 0)
				{
					return y;
				}
				else if(ysize == 0)
				{
					return x;
				}

				// Determine the size of the final value.
				int size = xsize;
				if(size < ysize)
				{
					size = ysize;
				}
				if((((ulong)(xwords[xsize - 1])) +
				    ((ulong)(ywords[ysize - 1])) + (ulong)1) >=
					(ulong)0x100000000)
				{
					// We may get a carry out, so allocate one more slot.
					++size;
				}

				// Create the result buffer.
				uint[] words = new uint [size];

				// Perform the addition.
				bool carry = false;
				int index = 0;
				while(index < xsize && index < ysize)
				{
					if(carry)
					{
						if((words[index] = (xwords[index] + ywords[index] + 1))
								<= xwords[index])
						{
							carry = true;
						}
						else
						{
							carry = false;
						}
					}
					else
					{
						if((words[index] = (xwords[index] + ywords[index]))
								< xwords[index])
						{
							carry = true;
						}
						else
						{
							carry = false;
						}
					}
					++index;
				}
				if(xsize > ysize)
				{
					while(index < xsize)
					{
						if(carry)
						{
							if((words[index] = xwords[index] + 1) != 0)
							{
								carry = false;
							}
						}
						else
						{
							words[index] = xwords[index];
						}
						++index;
					}
				}
				else if(xsize < ysize)
				{
					while(index < ysize)
					{
						if(carry)
						{
							if((words[index] = ywords[index] + 1) != 0)
							{
								carry = false;
							}
						}
						else
						{
							words[index] = ywords[index];
						}
						++index;
					}
				}
				if(carry)
				{
					words[index] = 1;
				}

				// Clear the arguments.
				x.Clear();
				y.Clear();

				// Create and return a new BigNum object.
				return new BigNum(words);
			}

	// Subtract big numbers.
	public static BigNum operator-(BigNum x, BigNum y)
			{
				// Unpack the parameters for quicker access.
				int xsize = x.size;
				int ysize = y.size;
				uint[] xwords = x.words;
				uint[] ywords = y.words;

				// Handle the simple cases of "subtract zero" first.
				if(xsize == 0)
				{
					if(ysize != 0)
					{
						y.Clear();
						throw new OverflowException
							(_("Crypto_BigNumUnderflow"));
					}
					return y;
				}
				else if(ysize == 0)
				{
					return x;
				}

				// Handle the simple underflow case.
				if(xsize < ysize ||
				   (xsize == ysize && xwords[xsize - 1] < ywords[xsize - 1]))
				{
					x.Clear();
					y.Clear();
					throw new OverflowException
						(_("Crypto_BigNumUnderflow"));
				}

				// Create the result buffer.
				uint[] words = new uint [xsize];

				// Perform the subtraction.
				bool borrow = false;
				int index = 0;
				while(index < xsize)
				{
					if(borrow)
					{
						if((words[index] = (xwords[index] - ywords[index] - 1))
								>= xwords[index])
						{
							borrow = true;
						}
						else
						{
							borrow = false;
						}
					}
					else
					{
						if((words[index] = (xwords[index] - ywords[index]))
								> xwords[index])
						{
							borrow = true;
						}
						else
						{
							borrow = false;
						}
					}
					++index;
				}

				// Clear the arguments.
				x.Clear();
				y.Clear();

				// Handle underflow of the subtraction.
				if(borrow)
				{
					words.Initialize();
					throw new OverflowException
						(_("Crypto_BigNumUnderflow"));
				}

				// Create and return a new BigNum object.
				return new BigNum(words);
			}

	// Multiply big numbers.
	public static BigNum operator*(BigNum x, BigNum y)
			{
				// Unpack the parameters for quicker access.
				int xsize = x.size;
				int ysize = y.size;
				uint[] xwords = x.words;
				uint[] ywords = y.words;

				// Handle the simple cases first.
				if(xsize == 0)
				{
					// 0 * y
					y.Clear();
					return x;
				}
				else if(xsize == 1 && xwords[0] == 1)
				{
					// 1 * y
					x.Clear();
					return y;
				}
				else if(ysize == 0)
				{
					// x * 0
					x.Clear();
				}
				else if(ysize == 1 && ywords[0] == 1)
				{
					// x * 1
					y.Clear();
					return x;
				}

				// Allocate space for the result.
				uint[] words = new uint [xsize + ysize];

				// Multiply the two values.
				int xposn, yposn;
				ulong temp;
				for(xposn = 0; xposn < xsize; ++xposn)
				{
					temp = 0;
					for(yposn = 0; yposn < ysize; ++yposn)
					{
						temp += ((ulong)(xwords[xposn])) *
								((ulong)(ywords[yposn])) +
								((ulong)(words[xposn + yposn]));
						words[xposn + yposn] = (uint)temp;
						temp >>= 32;
					}
					while(temp != 0)
					{
						temp += (ulong)(words[xposn + yposn]);
						words[xposn + yposn] = (uint)temp;
						temp >>= 32;
					}
				}

				// Clear the arguments.
				x.Clear();
				y.Clear();

				// Create and return a new BigNum object.
				return new BigNum(words);
			}

	// Divide two big numbers and return the quotient and remainder.
	private static BigNum DivRem(BigNum x, BigNum y, out BigNum rem)
			{
				// TODO
				rem = null;
				return null;
			}

	// Divide big numbers.
	public static BigNum operator/(BigNum x, BigNum y)
			{
				BigNum rem;
				BigNum div = DivRem(x, y, out rem);
				rem.Clear();
				x.Clear();
				y.Clear();
				return div;
			}

	// Remainder big numbers.
	public static BigNum operator%(BigNum x, BigNum y)
			{
				BigNum rem;
				BigNum div = DivRem(x, y, out rem);
				div.Clear();
				x.Clear();
				y.Clear();
				return rem;
			}

	// Compute the modulus-based exponential of a big number.
	public static BigNum ModExp(BigNum x, BigNum y, BigNum modulus)
			{
				// TODO
				return null;
			}

	// Implement the ICloneable interface.
	public Object Clone()
			{
				BigNum num = (BigNum)(MemberwiseClone());
				num.words = (uint[])(words.Clone());
				return num;
			}

}; // class BigNum

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
