/*
 * DictionaryAdapter.cs - Adapt a generic dictionary into a non-generic one.
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

public sealed class DictionaryAdapter<KeyT, ValueT>
	: System.Collections.IDictionary, System.Collections.ICollection,
	  System.Collections.IEnumerable
{

	// Internal state.
	private IDictionary<KeyT, ValueT> dict;

	// Constructor.
	public DictionaryAdapter(IDictionary<KeyT, ValueT> dict)
			{
				if(dict == null)
				{
					throw new ArgumentNullException("dict");
				}
				this.dict = dict;
			}

	// Implement the non-generic IDictionary interface.
	public void Add(Object key, Object value)
			{
				if(key == null)
				{
					// Cannot use null keys with non-generic dictionaries.
					throw new ArgumentNullException("key");
				}
				else if(!(key is KeyT))
				{
					// Wrong type of key to add to this dictionary.
					throw new InvalidOperationException
						(S._("Invalid_KeyType"));
				}
				if(typeof(ValueT).IsValueType)
				{
					if(value == null || !(value is ValueT))
					{
						// Wrong type of value to add to this dictionary.
						throw new InvalidOperationException
							(S._("Invalid_ValueType"));
					}
				}
				else
				{
					if(value != null && !(value is ValueT))
					{
						// Wrong type of value to add to this dictionary.
						throw new InvalidOperationException
							(S._("Invalid_ValueType"));
					}
				}
				dict.Add((KeyT)key, (ValueT)value);
			}
	public void Clear()
			{
				dict.Clear();
			}
	public bool Contains(Object key)
			{
				if(key == null)
				{
					throw new ArgumentNullException("key");
				}
				else if(key is KeyT)
				{
					return dict.Contains((KeyT)key);
				}
				else
				{
					return false;
				}
			}
	public IDictionaryEnumerator GetEnumerator()
			{
				return new DictionaryEnumeratorAdapter(dict.GetIterator());
			}
	public void Remove(Object key)
			{
				if(key == null)
				{
					throw new ArgumentNullException("key");
				}
				else if(key is KeyT)
				{
					dict.Remove((KeyT)key);
				}
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
					return dict.IsReadOnly;
				}
			}
	public Object this[Object key]
			{
				get
				{
					return dict[(KeyT)key];
				}
				set
				{
					if(key == null)
					{
						// Cannot use null keys with non-generic dictionaries.
						throw new ArgumentNullException("key");
					}
					else if(!(key is KeyT))
					{
						// Wrong type of key to add to this dictionary.
						throw new InvalidOperationException
							(S._("Invalid_KeyType"));
					}
					if(typeof(ValueT).IsValueType)
					{
						if(value == null || !(value is ValueT))
						{
							// Wrong type of value to add to this dictionary.
							throw new InvalidOperationException
								(S._("Invalid_ValueType"));
						}
					}
					else
					{
						if(value != null && !(value is ValueT))
						{
							// Wrong type of value to add to this dictionary.
							throw new InvalidOperationException
								(S._("Invalid_ValueType"));
						}
					}
					dict[(KeyT)key] = (ValueT)value;
				}
			}
	public ICollection Keys
			{
				get
				{
					return new CollectionAdapter<KeyT>(dict.Keys);
				}
			}
	public ICollection Values
			{
				get
				{
					return new CollectionAdapter<ValueT>(dict.Values);
				}
			}

	// Implement the non-generic ICollection interface.
	public void CopyTo(Array array, int index)
			{
				IDictionaryIterator<KeyT, ValueT> iterator = dict.GetIterator();
				while(iterator.MoveNext())
				{
					array.SetValue(iterator.Value, index++);
				}
			}
	public int Count
			{
				get
				{
					return dict.Count;
				}
			}
	public bool IsSynchronized
			{
				get
				{
					return dict.IsSynchronized;
				}
			}
	public Object SyncRoot
			{
				get
				{
					return dict.SyncRoot;
				}
			}

	// Implement the non-generic IEnumerable interface.
	public System.Collections.IEnumerator
				System.Collections.IEnumerable.GetEnumerator()
			{
				return new EnumeratorAdapter<ValueT>
					(((ICollection<ValueT>)dict).GetIterator());
			}

}; // class DictionaryAdapter<KeyT, ValueT>

}; // namespace Generics
