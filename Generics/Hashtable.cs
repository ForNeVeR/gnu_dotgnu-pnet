/*
 * Hashtable.cs - Generic hash table class.
 *
 * Copyright (c) 2003  Southern Storm Software, Pty Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

namespace Generics
{

using System;

public class Hashtable<KeyT, ValueT> : IDictionary<KeyT, ValueT>, ICloneable
{
	// Contents of a hash bucket.
	private struct HashBucket<KeyT, ValueT>
	{
	    public bool hasEntry;
		public KeyT key;
		public ValueT value;

	}; // struct HashBucket

	// Internal state.
	private IHashCodeProvider<KeyT> hcp__;
	private IComparer<ValueT> comparer__;
	private int capacity;
	private int capacityLimit;
	private int num;
	private float loadFactor;
	private HashBucket<KeyT, ValueT>[] table;

	// Table of the first 400 prime numbers.
	private static readonly int[] primes = {
		2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
		53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107,
		109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
		173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
		233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283,
		293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359,
		367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431,
		433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491,
		499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571,
		577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641,
		643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709,
		719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787,
		797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859,
		863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941,
		947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019,
		1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087,
		1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153,
		1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229,
		1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297,
		1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381,
		1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453,
		1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523,
		1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597,
		1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663,
		1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741,
		1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823,
		1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901,
		1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993,
		1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063,
		2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129, 2131,
		2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213, 2221,
		2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287, 2293,
		2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371,
		2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423, 2437,
		2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539,
		2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617, 2621,
		2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687, 2689,
		2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741};

	// Constructors.
	public Hashtable()
			{
				hcp__ = null;
				comparer__ = null;
				capacity = 0;
				capacityLimit = 0;
				num = 0;
				try
				{
					loadFactor = 1.0f;
				}
				catch(NotImplementedException)
				{
					// The runtime engine does not support floating point,
					// but we still need hash tables when no FP.
				}
				table = null;
			}
	public Hashtable(int capacity)
			{
				if(capacity < 0)
				{
					throw new ArgumentOutOfRangeException
						("capacity", S._("ArgRange_NonNegative"));
				}
				hcp__ = null;
				comparer__ = null;
				this.capacity = capacity;
				capacityLimit = capacity;
				num = 0;
				try
				{
					loadFactor = 1.0f;
				}
				catch(NotImplementedException)
				{
					// The runtime engine does not support floating point,
					// but we still need hash tables when no FP.
				}
				if(capacity != 0)
				{
					table = new HashBucket<KeyT, ValueT> [capacity];
				}
				else
				{
					table = null;
				}
			}
	public Hashtable(IHashCodeProvider<KeyT> hcp, IComparer<KeyT> comparer)
			{
				hcp__ = hcp;
				comparer__ = comparer;
				capacity = 0;
				capacityLimit = 0;
				num = 0;
				try
				{
					loadFactor = 1.0f;
				}
				catch(NotImplementedException)
				{
					// The runtime engine does not support floating point,
					// but we still need hash tables when no FP.
				}
				table = null;
			}
	public Hashtable(int capacity, IHashCodeProvider<KeyT> hcp,
					 IComparer<KeyT> comparer)
			{
				if(capacity < 0)
				{
					throw new ArgumentOutOfRangeException
						("capacity", S._("ArgRange_NonNegative"));
				}
				hcp__ = hcp;
				comparer__ = comparer;
				this.capacity = capacity;
				capacityLimit = capacity;
				num = 0;
				try
				{
					loadFactor = 1.0f;
				}
				catch(NotImplementedException)
				{
					// The runtime engine does not support floating point,
					// but we still need hash tables when no FP.
				}
				if(capacity != 0)
				{
					table = new HashBucket<KeyT, ValueT> [capacity];
				}
				else
				{
					table = null;
				}
			}
	public Hashtable(IDictionary<KeyT, ValueT> d)
			{
				if(d == null)
				{
					throw new ArgumentNullException("d");
				}
				hcp__ = null;
				comparer__ = null;
				capacity = d.Count;
				capacityLimit = capacity;
				num = 0;
				try
				{
					loadFactor = 1.0f;
				}
				catch(NotImplementedException)
				{
					// The runtime engine does not support floating point,
					// but we still need hash tables when no FP.
				}
				if(capacity != 0)
				{
					table = new HashBucket<KeyT, ValueT> [capacity];
				}
				else
				{
					table = null;
				}
				AddDictionaryContents(d);
			}
	public Hashtable(IDictionary<KeyT, ValueT> d, IHashCodeProvider<KeyT> hcp,
					 IComparer<KeyT> comparer)
			{
				if(d == null)
				{
					throw new ArgumentNullException("d");
				}
				hcp__ = hcp;
				comparer__ = comparer;
				capacity = d.Count;
				capacityLimit = capacity;
				num = 0;
				try
				{
					loadFactor = 1.0f;
				}
				catch(NotImplementedException)
				{
					// The runtime engine does not support floating point,
					// but we still need hash tables when no FP.
				}
				if(capacity != 0)
				{
					table = new HashBucket<KeyT, ValueT> [capacity];
				}
				else
				{
					table = null;
				}
				AddDictionaryContents(d);
			}
	public Hashtable(int capacity, float loadFactor)
			{
				if(capacity < 0)
				{
					throw new ArgumentOutOfRangeException
						("capacity", S._("ArgRange_NonNegative"));
				}
				if(loadFactor >= 0.1f && loadFactor <= 1.0f)
				{
					hcp__ = null;
					comparer__ = null;
					this.capacity = capacity;
					capacityLimit = (int)(capacity * loadFactor);
					num = 0;
					this.loadFactor = loadFactor;
					if(capacity != 0)
					{
						table = new HashBucket<KeyT, ValueT> [capacity];
					}
					else
					{
						table = null;
					}
				}
				else
				{
					throw new ArgumentOutOfRangeException
						("loadFactor", S._("ArgRange_HashLoadFactor"));
				}
			}
	public Hashtable(int capacity, float loadFactor,
					 IHashCodeProvider<KeyT> hcp, IComparer<KeyT> comparer)
			{
				if(capacity < 0)
				{
					throw new ArgumentOutOfRangeException
						("capacity", S._("ArgRange_NonNegative"));
				}
				if(loadFactor >= 0.1f && loadFactor <= 1.0f)
				{
					hcp__ = hcp;
					comparer__ = comparer;
					this.capacity = capacity;
					capacityLimit = (int)(capacity * loadFactor);
					num = 0;
					this.loadFactor = loadFactor;
					if(capacity != 0)
					{
						table = new HashBucket<KeyT, ValueT> [capacity];
					}
					else
					{
						table = null;
					}
				}
				else
				{
					throw new ArgumentOutOfRangeException
						("loadFactor", S._("ArgRange_HashLoadFactor"));
				}
			}
	public Hashtable(IDictionary<KeyT, ValueT> d, float loadFactor)
			{
				if(d == null)
				{
					throw new ArgumentNullException("d");
				}
				if(loadFactor >= 0.1f && loadFactor <= 1.0f)
				{
					hcp__ = null;
					comparer__ = null;
					capacity = d.Count;
					capacityLimit = (int)(capacity * loadFactor);
					num = 0;
					this.loadFactor = loadFactor;
					if(capacity != 0)
					{
						table = new HashBucket<KeyT, ValueT> [capacity];
					}
					else
					{
						table = null;
					}
					AddDictionaryContents(d);
				}
				else
				{
					throw new ArgumentOutOfRangeException
						("loadFactor", S._("ArgRange_HashLoadFactor"));
				}
			}
	public Hashtable(IDictionary<KeyT, ValueT> d, float loadFactor,
					 IHashCodeProvider<KeyT> hcp, IComparer<KeyT> comparer)
			{
				if(d == null)
				{
					throw new ArgumentNullException("d");
				}
				if(loadFactor >= 0.1f && loadFactor <= 1.0f)
				{
					hcp__ = hcp;
					comparer__ = comparer;
					capacity = d.Count;
					capacityLimit = (int)(capacity * loadFactor);
					num = 0;
					this.loadFactor = loadFactor;
					if(capacity != 0)
					{
						table = new HashBucket<KeyT, ValueT> [capacity];
					}
					else
					{
						table = null;
					}
					AddDictionaryContents(d);
				}
				else
				{
					throw new ArgumentOutOfRangeException
						("loadFactor", S._("ArgRange_HashLoadFactor"));
				}
			}

	// Add the contents of a dictionary to this hash table.
	private void AddDictionaryContents(IDictionary<KeyT, ValueT> d)
			{
				IDictionaryIterator<KeyT, ValueT> iterator = d.GetIterator();
				while(iterator.MoveNext())
				{
					Add(iterator.Key, iterator.Value);
				}
			}

	// Add a hash entry to the table directly, with no error checking.
	// This assumes that there is at least one spare bucket available.
	private void AddDirect(KeyT key, ValueT value)
			{
				int hash = GetHash(key);
				hash = (int)(((uint)hash) % ((uint)capacity));
				for(;;)
				{
					if(!(table[hash].hasEntry))
					{
						// We've found an empty slot, so add the entry.
						table[hash].key = key;
						table[hash].value = value;
						break;
					}
					hash = (hash + 1) % capacity;
				}
				++num;
			}

	// Expand the hash table and add a new entry.
	private void ExpandAndAdd(KeyT key, ValueT value)
			{
				HashBucket[] orig;
				int origSize;
				int newCapacity;

				// Copy the original table.
				orig = table;
				origSize = capacity;

				// Expand the size of the hash table.
				if(capacity == 0)
				{
					newCapacity = 2;
				}
				else
				{
					newCapacity = capacity * 2;
					if(newCapacity <= primes[primes.Length - 1])
					{
						// Search for the next value in the "primes" table.
						int left, right, middle;
						left = 0;
						right = primes.Length - 1;
						while(left <= right)
						{
							middle = (left + right) / 2;
							if(newCapacity < primes[middle])
							{
								right = middle - 1;
							}
							else if(newCapacity > primes[middle])
							{
								left = middle + 1;
							}
							else
							{
								right = middle;
								break;
							}
						}
						newCapacity = primes[right];
					}
					else
					{
						// We've run out of primes, so make the number odd
						// and assume that the result is close enough to
						// prime that it will be useful for our purposes.
						++newCapacity;
					}
				}
				table = new HashBucket<KeyT, ValueT> [newCapacity];
				capacity = newCapacity;
				num = 0;

				// Determine the new capacity limit.
				try
				{
					capacityLimit = (int)(capacity * loadFactor);
				}
				catch(NotImplementedException)
				{
					// The runtime engine does not support floating point,
					// so assume a load factor of 1.
					capacityLimit = capacity;
				}

				// Copy the original entries to the new table.
				while(origSize > 0)
				{
					--origSize;
					if(orig[origSize].key != null)
					{
						AddDirect(orig[origSize].key, orig[origSize].value);
					}
				}

				// Add the new entry to the hash table.
				AddDirect(key, value);
			}

	// Implement the ICloneable interface.
	public virtual Object Clone()
			{
				Hashtable<KeyT, ValueT> hashtab =
					(Hashtable<KeyT, ValueT>)(MemberwiseClone());
				if(capacity > 0)
				{
					hashtab.table = new HashBucket<KeyT, ValueT> [capacity];
					Array.Copy(table, hashtab.table, capacity);
				}
				return hashtab;
			}

	// Implement the ICollection< DictionaryEntry<KeyT, ValueT> > interface.
	public virtual void CopyTo
				(DictionaryEntry<KeyT, ValueT>[] array, int index)
			{
				if(array == null)
				{
					throw new ArgumentNullException("array");
				}
				else if(index < array.GetLowerBound(0))
				{
					throw new ArgumentOutOfRangeException
						("index", S._("Arg_InvalidArrayIndex"));
				}
				else if(index > (array.GetLength(0) - num))
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}
				else
				{
					IDictionaryIterator<KeyT, ValueT> iterator;
					iterator = GetIterator();
					while(iterator.MoveNext())
					{
						array[index++] = iterator.Entry;
					}
				}
			}
	public virtual int Count
			{
				get
				{
					return num;
				}
			}
	public virtual bool IsSynchronized
			{
				get
				{
					return false;
				}
			}
	public virtual Object SyncRoot
			{
				get
				{
					return this;
				}
			}

	// Implement the IDictionary<KeyT, ValueT> interface.
	public virtual void Add(KeyT key, ValueT value)
			{
				// Find an empty slot to add the entry, or expand
				// the table if there are no free slots.
				int hash = GetHash(key);
				if(capacity == 0)
				{
					ExpandAndAdd(key, value);
					return;
				}
				hash = (int)(((uint)hash) % ((uint)capacity));
				int count = capacity;
				while(count > 0)
				{
					if(!(table[hash].hasEntry))
					{
						// We've found an empty slot.  Check the capacity.
						if(num >= capacityLimit)
						{
							// We must increase the capacity before adding.
							ExpandAndAdd(key, value);
						}
						else
						{
							// Add the entry to the empty slot.
							table[hash].key = key;
							table[hash].value = value;
							++num;
						}
						return;
					}
					else if(KeyEquals(table[hash].key, key))
					{
						// There is already an entry with the key.
						throw new ArgumentException
							(_("Arg_ExistingEntry"));
					}
					hash = (hash + 1) % capacity;
					--count;
				}
				ExpandAndAdd(key, value);
			}
	public virtual void Clear()
			{
				if(table != null)
				{
					Array.Clear(table, 0, capacity);
				}
				num = 0;
			}
	public virtual bool Contains(KeyT key)
			{
				return ContainsKey(key);
			}
	public virtual IDictionaryIterator<KeyT, ValueT> GetIterator()
			{
				return new HashtableIterator<KeyT, ValueT>(this);
			}
	public virtual void Remove(KeyT key)
			{
				int hash = GetHash(key);
				if(capacity == 0)
				{
					return;
				}
				hash = (int)(((uint)hash) % ((uint)capacity));
				int count = capacity;
				while(count > 0)
				{
					if(!(table[hash].hasEntry))
					{
						break;
					}
					else if(KeyEquals(table[hash].key, key))
					{
						table[hash].hasEntry = false;
						--num;
						break;
					}
					hash = (hash + 1) % capacity;
					--count;
				}
			}
	public virtual bool IsFixedSize
			{
				get
				{
					return false;
				}
			}
	public virtual bool IsReadOnly
			{
				get
				{
					return false;
				}
			}
	[IndexerName("Item")]
	public virtual ValueT this[KeyT key]
			{
				get
				{
					// Find an existing entry with the specified key.
					int hash = GetHash(key);
					if(capacity == 0)
					{
						return null;
					}
					hash = (int)(((uint)hash) % ((uint)capacity));
					int count = capacity;
					while(count > 0)
					{
						if(!(table[hash].hasEntry))
						{
							break;
						}
						else if(KeyEquals(table[hash].key, key))
						{
							return table[hash].value;
						}
						hash = (hash + 1) % capacity;
						--count;
					}
					return null;
				}
				set
				{
					// Find an existing entry and replace it, or
					// add a new entry to the table if not found.
					int hash = GetHash(key);
					if(capacity == 0)
					{
						ExpandAndAdd(key, value);
						return;
					}
					hash = (int)(((uint)hash) % ((uint)capacity));
					int count = capacity;
					while(count > 0)
					{
						if(!(table[hash].hasEntry))
						{
							// We've found an empty slot.  Check the capacity.
							if(num >= capacityLimit)
							{
								// We must increase the capacity before adding.
								ExpandAndAdd(key, value);
							}
							else
							{
								// Add the entry to the empty slot.
								table[hash].key = key;
								table[hash].value = value;
								++num;
							}
							return;
						}
						else if(KeyEquals(table[hash].key, key))
						{
							// There is already an entry with the key,
							// so replace its value.
							table[hash].value = value;
							return;
						}
						hash = (hash + 1) % capacity;
						--count;
					}
					ExpandAndAdd(key, value);
				}
			}
	public virtual ICollection<KeyT> Keys
			{
				get
				{
					KeyT[] array = new KeyT [num];
					int posn, index;
					Object key;
					index = 0;
					for(posn = 0; posn < capacity; ++posn)
					{
						if(table[posn].hasEntry)
						{
							array[index++] = table[posn].key;
						}
					}
					return array;
				}
			}
	public virtual ICollection<ValueT> Values
			{
				get
				{
					ValueT[] array = new ValueT [num];
					int posn, index;
					index = 0;
					for(posn = 0; posn < capacity; ++posn)
					{
						if(table[posn].hasEntry)
						{
							array[index++] = table[posn].value;
						}
					}
					return array;
				}
			}

	// Implement the IIterable< DictionaryEntry<KeyT, ValueT> > interface.
	IIterator IIterable< DictionaryEntry<KeyT, ValueT> >.GetIterator()
			{
				return GetIterator();
			}

	// Determine if this hash table contains a specific key.
	public virtual bool ContainsKey(KeyT key)
			{
				int hash = GetHash(key);
				if(capacity == 0)
				{
					return false;
				}
				hash = (int)(((uint)hash) % ((uint)capacity));
				int count = capacity;
				while(count > 0)
				{
					if(!(table[hash].hasEntry))
					{
						break;
					}
					else if(KeyEquals(table[hash].key, key))
					{
						return true;
					}
					hash = (hash + 1) % capacity;
					--count;
				}
				return false;
			}

	// Determine if this hash table contains a specific value.
	public virtual bool ContainsValue(ValueT value)
			{
				int posn;
				for(posn = capacity - 1; posn >= 0; --posn)
				{
					if(table[posn].hasEntry && table[posn].value == value)
					{
						return true;
					}
				}
				return false;
			}

	// Get the hash value for a key.
	protected virtual int GetHash(KeyT key)
			{
				if(((Object)key) == null)
				{
					throw new ArgumentNullException("key");
				}
				IHashCodeProvider<KeyT> provider = hcp;
				if(provider != null)
				{
					return provider.GetHashCode(key);
				}
				else
				{
					return key.GetHashCode();
				}
			}

	// Determine if an item is equal to a key value.
	protected virtual bool KeyEquals(KeyT item, KeyT key)
			{
				if(((Object)item) == null)
				{
					throw new ArgumentNullException("item");
				}
				if(((Object)key) == null)
				{
					throw new ArgumentNullException("key");
				}
				IComparer<KeyT> cmp = comparer;
				if(cmp != null)
				{
					return (cmp.Compare(item, key) == 0);
				}
				else
				{
					return item.Equals(key);
				}
			}

	// Wrap a Hashtable object to make it synchronized.
	public static Hashtable<KeyT, ValueT> Synchronized
				(Hashtable<KeyT, ValueT> table)
			{
				if(table == null)
				{
					throw new ArgumentNullException("table");
				}
				else if(table.IsSynchronized)
				{
					return table;
				}
				else
				{
					return new SynchronizedHashtable<KeyT, ValueT>(table);
				}
			}

	// Get the hash code provider that is being used by this instance.
	protected virtual IHashCodeProvider<KeyT> hcp
			{
				get
				{
					return hcp__;
				}
			}

	// Get the comparer that is being used by this instance.
	protected virtual IComparer<KeyT> comparer
			{
				get
				{
					return comparer__;
				}
			}

	// Synchronized hash table implementation.
	//
	// Note: We lock every operation on the underlying hash table,
	// even if it is a read or iterator operation.  This is because
	// we cannot guarantee correct behaviour in symmetric multi-processing
	// environments if we only lock write operations.
	private sealed class SynchronizedHashtable<KeyT, ValueT>
			: Hashtable<KeyT, ValueT>
	{
		// Internal state.
		private new Hashtable<KeyT, ValueT> table;

		// Constructor.
		public SynchronizedHashtable(Hashtable<KeyT, ValueT> table)
				{
					this.table = table;
				}

		// Implement the ICloneable interface.
		public override Object Clone()
				{
					lock(SyncRoot)
					{
						return new SynchronizedHashtable<KeyT, ValueT>
							((Hashtable<KeyT, ValueT>)(table.Clone()));
					}
				}

		// Implement the ICollection interface.
		public override void CopyTo
					(DictionaryEntry<KeyT, ValueT>[] array, int index)
				{
					lock(SyncRoot)
					{
						table.CopyTo(array, index);
					}
				}
		public override int Count
				{
					get
					{
						lock(SyncRoot)
						{
							return table.Count;
						}
					}
				}
		public override bool IsSynchronized
				{
					get
					{
						return true;
					}
				}
		public override Object SyncRoot
				{
					get
					{
						return table.SyncRoot;
					}
				}

		// Implement the IDictionary interface.
		public override void Add(KeyT key, ValueT value)
				{
					lock(SyncRoot)
					{
						table.Add(key, value);
					}
				}
		public override void Clear()
				{
					lock(SyncRoot)
					{
						table.Clear();
					}
				}
		public override bool Contains(KeyT key)
				{
					lock(SyncRoot)
					{
						return table.Contains(key);
					}
				}
		public override IDictionaryIterator<KeyT, ValueT> GetIterator()
				{
					lock(SyncRoot)
					{
						return new SynchronizedDictIterator<KeyT, ValueT>
							(SyncRoot, table.GetIterator());
					}
				}
		public override void Remove(KeyT key)
				{
					lock(SyncRoot)
					{
						table.Remove(key);
					}
				}
		public override bool IsFixedSize
				{
					get
					{
						lock(SyncRoot)
						{
							return table.IsFixedSize;
						}
					}
				}
		public override bool IsReadOnly
				{
					get
					{
						lock(SyncRoot)
						{
							return table.IsReadOnly;
						}
					}
				}
		public override ValueT this[KeyT key]
				{
					get
					{
						lock(SyncRoot)
						{
							return table[key];
						}
					}
					set
					{
						lock(SyncRoot)
						{
							table[key] = value;
						}
					}
				}
		public override ICollection<KeyT> Keys
				{
					get
					{
						lock(SyncRoot)
						{
							return table.Keys;
						}
					}
				}
		public override ICollection<ValueT> Values
				{
					get
					{
						lock(SyncRoot)
						{
							return table.Values;
						}
					}
				}
	
		// Determine if this hash table contains a specific key.
		public override bool ContainsKey(KeyT key)
				{
					lock(SyncRoot)
					{
						return table.ContainsKey(key);
					}
				}

		// Determine if this hash table contains a specific value.
		public override bool ContainsValue(ValueT value)
				{
					lock(SyncRoot)
					{
						return table.ContainsValue(value);
					}
				}

		// Get the hash value for a key.
		protected override int GetHash(KeyT key)
				{
					// We don't lock this because it does not modify
					// the underlying hash table, or access fields
					// that may be modified by other threads.
					return table.GetHash(key);
				}

		// Determine if an item is equal to a key value.
		protected override bool KeyEquals(KeyT item, KeyT key)
				{
					// We don't lock this because it does not modify
					// the underlying hash table, or access fields
					// that may be modified by other threads.
					return table.KeyEquals(item, key);
				}

		// Get the hash code provider that is being used by this instance.
		protected override IHashCodeProvider<KeyT> hcp
				{
					get
					{
						// We don't lock this because it does not modify
						// the underlying hash table, or access fields
						// that may be modified by other threads.
						return table.hcp;
					}
				}

		// Get the comparer that is being used by this instance.
		protected override IComparer<KeyT> comparer
				{
					get
					{
						// We don't lock this because it does not modify
						// the underlying hash table, or access fields
						// that may be modified by other threads.
						return table.comparer;
					}
				}

	}; // SynchronizedHashtable<KeyT, ValueT>

	// Hashtable collection and dictionary iterator.
	private class HashtableIterator<KeyT, ValueT>
		: IDictionaryIterator<KeyT, ValueT>
	{
		// Internal state.
		protected Hashtable<KeyT, ValueT> table;
		protected int		posn;

		// Constructor.
		public HashtableIterator(Hashtable<KeyT, ValueT> table)
				{
					this.table = table;
					posn = -1;
				}

		// Implement the IIterator<ValueT> interface.
		public bool MoveNext()
				{
					while(++posn < table.capacity)
					{
						if(table.table[posn].hasEntry)
						{
							return true;
						}
					}
					posn = table.capacity;
					return false;
				}
		public void Reset()
				{
					posn = -1;
				}
		public void Remove()
				{
					if(posn < 0 || posn >= table.capacity ||
					   !(table.table[posn].hasEntry))
					{
						throw new InvalidOperationException
							(S._("Invalid_BadIteratorPosition"));
					}
					table.table[posn].hasEntry = false;
					--(table.num);
				}
		public DictionaryEntry<KeyT, ValueT> Current
				{
					get
					{
						return Entry;
					}
				}

		// Implement the IDictionaryIterator<KeyT, ValueT> interface.
		public DictionaryEntry<KeyT, ValueT> Entry
				{
					get
					{
						if(posn < 0 || posn >= table.capacity ||
					       !(table.table[posn].hasEntry))
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						return new DictionaryEntry<KeyT, ValueT>
								(table.table[posn].key,
							     table.table[posn].value);
					}
				}
		public KeyT Key
				{
					get
					{
						if(posn < 0 || posn >= table.capacity ||
					       !(table.table[posn].hasEntry))
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						return table.table[posn].key;
					}
				}
		public ValueT Value
				{
					get
					{
						if(posn < 0 || posn >= table.capacity ||
					       !(table.table[posn].hasEntry))
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						return table.table[posn].value;
					}
					set
					{
						if(posn < 0 || posn >= table.capacity ||
					       !(table.table[posn].hasEntry))
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						table.table[posn].value = value;
					}
				}

	}; // HashtableIterator<KeyT, ValueT>

}; // class Hashtable<KeyT, ValueT>

}; // namespace Generics
