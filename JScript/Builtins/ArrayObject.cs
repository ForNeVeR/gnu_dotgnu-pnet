/*
 * ArrayObject.cs - Implementation of array objects.
 *
 * Copyright (C) 2003 Southern Storm Software, Pty Ltd.
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
 
namespace Microsoft.JScript
{

using System;

public class ArrayObject : JSObject
{
	// Internal state.
	private Object[] array;
	private uint arrayLen;
	private bool isSparse;

	// Constructor.
	internal ArrayObject(ScriptObject prototype)
			: base(prototype)
			{
				array = null;
				arrayLen = 0;
				isSparse = false;
			}

	// Get or set the length of the array.
	public virtual Object length
			{
				get
				{
					if(arrayLen <= (uint)(Int32.MaxValue))
					{
						return (int)arrayLen;
					}
					else
					{
						return (double)arrayLen;
					}
				}
				set
				{
					// TODO
				}
			}

	// Splice the array slowly.
	protected void SpliceSlowly(uint start, uint deleteCount,
								Object[] args, ArrayObject outArray,
								uint oldLength, uint newLength)
			{
				// This exists for backwards-compatibility, but there
				// is actually no way to call it from user code.
			}

}; // class ArrayObject

}; // namespace Microsoft.JScript
