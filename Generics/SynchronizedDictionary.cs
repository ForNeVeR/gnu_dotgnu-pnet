/*
 * SynchronizedDictionary.cs - Wrap a dictionary to make it synchronized.
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

public class SynchronizedDictionary<KeyT, ValueT>
	: SynchronizedCollection< DictionaryEntry<KeyT, ValueT> >,
	  IDictionary<KeyT, ValueT>
{
	// Internal state.
	protected IDictionary<KeyT, ValueT> dict;

	// Constructor.
	public SynchronizedDictionary(IDictionary<KeyT, ValueT> dict) : base(dict)
			{
				this.dict = dict;
			}

	// Implement the IDictionary<KeyT, ValueT> interface.
	public void Add(KeyT key, ValueT value)
			{
				lock(SyncRoot)
				{
					dict.Add(key, value);
				}
			}
	public void Clear()
			{
				lock(SyncRoot)
				{
					dict.Clear();
				}
			}
	public bool Contains(KeyT key)
			{
				lock(SyncRoot)
				{
					return dict.Contains(key);
				}
			}
	public new IDictionaryIterator<KeyT, ValueT> GetIterator()
			{
				lock(SyncRoot)
				{
					return new SynchronizedDictIterator<KeyT, ValueT>
							(dict, dict.GetIterator());
				}
			}
	public void Remove(KeyT key)
			{
				lock(SyncRoot)
				{
					dict.Remove(key);
				}
			}
	public bool IsFixedSize
			{
				get
				{
					lock(SyncRoot)
					{
						return dict.IsFixedSize;
					}
				}
			}
	public bool IsReadOnly
			{
				get
				{
					lock(SyncRoot)
					{
						return dict.IsReadOnly;
					}
				}
			}
	public ValueT this[KeyT key]
			{
				get
				{
					lock(SyncRoot)
					{
						return dict[key];
					}
				}
				set
				{
					lock(SyncRoot)
					{
						dict[key] = value;
					}
				}
			}
	public ICollection<KeyT> Keys
			{
				get
				{
					lock(SyncRoot)
					{
						return new SynchronizedCollection<KeyT>(dict.Keys);
					}
				}
			}
	public ICollection<ValueT> Values
			{
				get
				{
					lock(SyncRoot)
					{
						return new SynchronizedCollection<ValueT>(dict.Values);
					}
				}
			}

	// Implement the ICloneable interface.
	public override Object Clone()
			{
				if(dict is ICloneable)
				{
					return new SynchronizedDictionary<T>
						((IDictionary<T>)(((ICloneable)dict).Clone()));
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_NotCloneable"));
				}
			}

	// Synchronized dictionary iterator.
	private sealed class SynchronizedDictIterator<KeyT, ValueT>
		: IDictionaryIterator<KeyT, ValueT>
	{
		// Internal state.
		protected IDictionary<KeyT, ValueT> dict;
		protected IDictionaryIterator<KeyT, ValueT> iterator;

		// Constructor.
		public SynchronizedDictIterator
					(IDictionary<KeyT, ValueT> dict,
					 IDictionaryIterator<KeyT, ValueT> iterator)
				{
					this.dict = dict;
					this.iterator = iterator;
				}

		// Implement the IIterator<ValueT> interface.
		public bool MoveNext()
				{
					lock(dict.SyncRoot)
					{
						return iterator.MoveNext();
					}
				}
		public void Reset()
				{
					lock(dict.SyncRoot)
					{
						iterator.Reset();
					}
				}
		public void Remove()
				{
					lock(dict.SyncRoot)
					{
						iterator.Remove();
					}
				}
		public DictionaryEntry<KeyT, ValueT> Current
				{
					get
					{
						lock(dict.SyncRoot)
						{
							return iterator.Current;
						}
					}
				}

		// Implement the IDictionaryIterator<KeyT, ValueT> interface.
		public KeyT Key
				{
					get
					{
						lock(dict.SyncRoot)
						{
							return iterator.Key;
						}
					}
				}
		public ValueT Value
				{
					get
					{
						lock(dict.SyncRoot)
						{
							return iterator.Value;
						}
					}
					set
					{
						lock(dict.SyncRoot)
						{
							iterator.Value = value;
						}
					}
				}

	}; // class SynchronizedDictIterator<KeyT, ValueT>

}; // class SynchronizedDictionary<KeyT, ValueT>

}; // namespace Generics
