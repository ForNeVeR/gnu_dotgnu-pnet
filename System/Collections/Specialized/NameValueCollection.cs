/*
 * NameValueCollection.cs - Implementation of
 *		"System.Collections.Specialized.NameValueCollection".
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

namespace System.Collections.Specialized
{

using System;
using System.Collections;

public class NameValueCollection : ICollection, IEnumerable
{

	// Constructors.
	public NameValueCollection() : this(0) {}
	public NameValueCollection(NameValueCollection col) : this(0, col) {}
	public NameValueCollection(IHashCodeProvider hashProvider,
							   IComparer comparer)
			: this(0, hashProvider, comparer) {}
	[TODO]
	public NameValueCollection(int capacity)
			{
				if(capacity < 0)
				{
					throw new ArgumentOutOfRangeException
						("capacity", S._("ArgRange_NonNegative"));
				}
				// TODO
			}
	[TODO]
	public NameValueCollection(int capacity, NameValueCollection col)
			{
				if(capacity < 0)
				{
					throw new ArgumentOutOfRangeException
						("capacity", S._("ArgRange_NonNegative"));
				}
				if(col == null)
				{
					throw new ArgumentNullException("col");
				}
				// TODO
			}
	[TODO]
	public NameValueCollection(int capacity, IHashCodeProvider hashProvider,
							   IComparer comparer)
			{
				if(capacity < 0)
				{
					throw new ArgumentOutOfRangeException
						("capacity", S._("ArgRange_NonNegative"));
				}
				// TODO
			}

	// Add a name/value pair to this collection.
	[TODO]
	public virtual void Add(String name, String value)
			{
				// TODO
			}

	// Add the contents of another name/value collection to this collection.
	[TODO]
	public void Add(NameValueCollection c)
			{
				if(c == null)
				{
					throw new ArgumentNullException("c");
				}
				// TODO
			}

	// Implement the ICollection interface.
	[TODO]
	public void CopyTo(Array array, int index)
			{
				// TODO
			}
	int ICollection.Count
			{
				[TODO]
				get
				{
					// TODO
					return 0;
				}
			}
	bool ICollection.IsSynchronized
			{
				[TODO]
				get
				{
					// TODO
					return false;
				}
			}
	Object ICollection.SyncRoot
			{
				[TODO]
				get
				{
					// TODO
					return this;
				}
			}

	// Implement the IEnumerable interface.
	[TODO]
	IEnumerator IEnumerable.GetEnumerator()
			{
				// TODO
				return null;
			}

	// Clear the contents of this collection.
	[TODO]
	public void Clear()
			{
				// TODO
			}

	// Get a key at a particular index within this collection.
	[TODO]
	public virtual String GetKey(int index)
			{
				// TODO
				return null;
			}

	// Get a value at a particular index within this collection.
	[TODO]
	public virtual String Get(int index)
			{
				// TODO
				return null;
			}

	// Get an array of values at a particular index within this collection.
	[TODO]
	public virtual String[] GetValues(int index)
			{
				// TODO
				return null;
			}

	// Get the value associcated with a particular name.
	[TODO]
	public virtual String Get(String name)
			{
				// TODO
				return null;
			}

	// Get the array of values associcated with a particular name.
	[TODO]
	public virtual String[] GetValues(String name)
			{
				// TODO
				return null;
			}

	// Determine if the collection has keys that are not null.
	[TODO]
	public bool HasKeys()
			{
				// TODO
				return false;
			}

	// Invalidate cached arrays within this collection.
	[TODO]
	protected void InvalidateCachedArrays()
			{
				// TODO
			}

	// Remove an entry with a specified name from this collection.
	[TODO]
	public virtual void Remove(String name)
			{
				// TODO
			}

	// Set the value associated with a specified name in this collection.
	[TODO]
	public virtual void Set(String name, String value)
			{
				// TODO
			}

	// Get a list of all keys in this collection.
	[TODO]
	public virtual String[] AllKeys
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get or set a specific item within this collection by name.
	public String this[String name]
			{
				get
				{
					return Get(name);
				}
				set
				{
					Set(name, value);
				}
			}

	// Get a specific item within this collection by index.
	public String this[int index]
			{
				get
				{
					return Get(index);
				}
			}

}; // class NameValueCollection

}; // namespace System.Collections.Specialized
