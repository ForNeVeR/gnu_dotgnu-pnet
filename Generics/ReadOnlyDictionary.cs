/*
 * ReadOnlyDictionary.cs - Wrap a dictionary to make it read-only.
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

public class ReadOnlyDictionary<KeyT, ValueT>
	: ReadOnlyCollection< DictionaryEntry<KeyT, ValueT> >,
	  IDictionary<KeyT, ValueT>
{
	// Internal state.
	protected IDictionary<KeyT, ValueT> dict;

	// Constructor.
	public ReadOnlyDictionary(IDictionary<KeyT, ValueT> dict) : base(dict)
			{
				this.dict = dict;
			}

	// Implement the IDictionary<KeyT, ValueT> interface.
	public void Add(KeyT key, ValueT value)
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
			}
	public void Clear()
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
			}
	public bool Contains(KeyT key)
			{
				return dict.Contains(key);
			}
	public new IDictionaryIterator<KeyT, ValueT> GetIterator()
			{
				return new ReadOnlyDictIterator<KeyT, ValueT>
					(dict.GetIterator());
			}
	public void Remove(KeyT key)
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
			}
	public bool IsFixedSize
			{
				get
				{
					return dict.IsFixedSize;
				}
			}
	public bool IsReadOnly
			{
				get
				{
					return true;
				}
			}
	public ValueT this[KeyT key]
			{
				get
				{
					return dict[key];
				}
				set
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
			}
	public ICollection<KeyT> Keys
			{
				get
				{
					return new ReadOnlyCollection<KeyT>(dict.Keys);
				}
			}
	public ICollection<ValueT> Values
			{
				get
				{
					return new ReadOnlyCollection<ValueT>(dict.Values);
				}
			}

	// Implement the ICloneable interface.
	public override Object Clone()
			{
				if(dict is ICloneable)
				{
					return new ReadOnlyDictionary<T>
						((IDictionary<T>)(((ICloneable)dict).Clone()));
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_NotCloneable"));
				}
			}

	// Read-only iterator class for dicionaries.
	private sealed class ReadOnlyDictIterator<KeyT, ValueT>
		: IDictionaryIterator<KeyT, ValueT>
	{
		// Internal state.
		protected IDictionaryIterator<KeyT, ValueT> iterator;

		// Constructor.
		public ReadOnlyDictIterator(IDictionaryIterator<KeyT, ValueT> iterator)
				{
					this.iterator = iterator;
				}

		// Implement the IIterator<DictionaryEntry<KeyT, ValueT>> interface.
		public bool MoveNext()
				{
					return iterator.MoveNext();
				}
		public void Reset()
				{
					iterator.Reset();
				}
		public void Remove()
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
		public DictionaryEntry<KeyT, ValueT> Current
				{
					get
					{
						return iterator.Current;
					}
				}

		// Implement the IDictionaryIterator<KeyT, ValueT> interface.
		public DictionaryEntry<KeyT, ValueT> Entry
				{
					get
					{
						return iterator.Entry;
					}
				}
		public KeyT Key
				{
					get
					{
						return iterator.Key;
					}
				}
		public ValueT Value
				{
					get
					{
						return iterator.Value;
					}
					set
					{
						throw new InvalidOperationException
							(S._("NotSupp_ReadOnly"));
					}
				}

	}; // class ReadOnlyDictIterator<KeyT, ValueT>

}; // class ReadOnlyDictionary<KeyT, ValueT>

}; // namespace Generics
