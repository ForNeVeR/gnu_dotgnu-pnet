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
	internal Array array;
	internal uint arrayLen;

	// Constructor.
	internal ArrayObject(ScriptObject prototype)
			: base(prototype)
			{
				array = null;
				arrayLen = 0;
			}
	internal ArrayObject(ScriptObject prototype, uint len)
			: base(prototype)
			{
				array = null;
				arrayLen = len;
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
					double num = Convert.ToNumber(value);
					uint inum = Convert.ToUInt32(value);
					if(num != (double)inum)
					{
						throw new JScriptException
							(JSError.ArrayLengthAssignIncorrect);
					}
					if(array != null && inum < arrayLen &&
					   inum < (uint)(array.Length))
					{
						if(arrayLen < (uint)(array.Length))
						{
							Array.Clear(array, (int)inum,
										(int)(arrayLen - inum));
						}
						else
						{
							Array.Clear(array, (int)inum,
										array.Length - (int)inum);
						}
					}
					arrayLen = inum;
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
					double num = Convert.ToNumber(name);
					uint inum = Convert.ToUInt32(name);
					if(num != (double)inum)
					{
						return base.Get(name);
					}
					else if(inum < arrayLen && array != null &&
							inum < (uint)(array.Length))
					{
						return array.GetValue((int)inum);
					}
					else
					{
						return null;
					}
				}
			}

	// Get a property from this object by numeric index.
	internal override Object GetIndex(int index)
			{
				if(index >= 0 && ((uint)index) < arrayLen &&
				   array != null && index < array.Length)
				{
					return array.GetValue(index);
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
				else if(CanPut(name))
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

	// Wrapper class for wrapping up a native array.
	internal sealed class Wrapper : ArrayObject
	{
		// Constructor.
		public Wrapper(ScriptObject prototype, Array array)
				: base(prototype, (uint)(array.Length))
				{
					this.array = array;
				}

		// Get or set the length of the array.
		public override Object length
				{
					get
					{
						return array.Length;
					}
					set
					{
						throw new JScriptException
							(JSError.AssignmentToReadOnly);
					}
				}

		// Get a property from this object by numeric index.
		internal override Object GetIndex(int index)
				{
					if(index >= 0 && index < array.Length)
					{
						return array.GetValue(index);
					}
					else
					{
						return null;
					}
				}

		// Put a property to this object.
		internal override void Put(String name, Object value)
				{
					if(name == "length")
					{
						length = value;
					}
					else if(CanPut(name))
					{
						double num = Convert.ToNumber(name);
						uint inum = Convert.ToUInt32(name);
						if(num != (double)inum)
						{
							// Force an array index exception.
							inum = (uint)(array.Length);
						}
						array.SetValue(value, (int)inum);
					}
				}

		// Put a property to this object by numeric index.
		internal override void PutIndex(int index, Object value)
				{
					array.SetValue(value, index);
				}

		// Delete a property from this object.
		internal override bool Delete(String name)
				{
					// Deletions are not allowed on native arrays.
					return false;
				}

	}; // class Wrapper

}; // class ArrayObject

}; // namespace Microsoft.JScript
