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
using System.Collections;

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

	// Get a property from this object.  Null if not present.
	internal override Object Get(String name)
			{
				if(name == "length")
				{
					return length;
				}
				else
				{
					return base.Get(name);
				}
			}

	// Get a property from this object by numeric index.
	internal override Object GetIndex(int index)
			{
				if(!isSparse && index >= 0 && ((uint)index) < arrayLen)
				{
					return array[index];
				}
				else
				{
					return base.Get(Convert.ToString(index));
				}
			}

	// Put a property to this object.
	internal override void Put(String name, Object value)
			{
				if(name == "length")
				{
					length = value;
				}
				else
				{
					// TODO
				}
			}

	// Put a property to this object by numeric index.
	internal override void PutIndex(int index, Object value)
			{
				// TODO
			}

	// Determine if this object has a specific property.
	internal override bool HasOwnProperty(String name)
			{
				// TODO
				return false;
			}

	// Delete a property from this object.
	internal override bool Delete(String name)
			{
				// TODO
				return true;
			}

	// Get the default value for this object.
	internal override Object DefaultValue(DefaultValueHint hint)
			{
				// TODO
				return null;
			}

	// Get an enumerator for the properties in this object.
	internal override IEnumerator GetPropertyEnumerator()
			{
				// TODO
				return null;
			}

}; // class ArrayObject

}; // namespace Microsoft.JScript
