/*
 * DictionaryWrapper.cs - Wrap a non-generic dictionary and make it generic.
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

public sealed class DictionaryWrapper<KeyT, ValueT> : IDictionary<KeyT, ValueT>
{
	// Internal state.
	private System.Collections.IDictionary dict;

	// Constructor.
	public DictionaryWrapper(System.Collections.IDictionary dict)
			{
				if(dict == null)
				{
					throw new ArgumentNullException("dict");
				}
				this.dict = dict;
			}

	// Implement the IDictionary<KeyT, ValueT> interface.
	public void Add(KeyT key, ValueT value)
			{
				dict.Add(key, value);
			}
	public void Clear()
			{
				dict.Clear();
			}
	public bool Contains(KeyT key)
			{
				return dict.Contains(key);
			}
	public IDictionaryIterator<KeyT, ValueT> GetIterator()
			{
				return new DictionaryEnumeratorWrapper<KeyT, ValueT>
						(dict.GetEnumerator());
			}
	public void Remove(KeyT key)
			{
				dict.Remove(key);
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
	public ValueT this[KeyT key]
			{
				get
				{
					return (ValueT)(dict[key]);
				}
				set
				{
					dict[key] = value;
				}
			}
	public ICollection<KeyT> Keys
			{
				get
				{
					return new CollectionWrapper<KeyT>(dict.Keys);
				}
			}
	public ICollection<ValueT> Values
			{
				get
				{
					return new CollectionWrapper<ValueT>(dict.Values);
				}
			}

	// Implement the ICollection<DictionaryEntry<KeyT, ValueT>> interface.
	public void CopyTo(DictionaryEntry<KeyT, ValueT>[] array, int index)
			{
				dict.CopyTo(array, index);
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

	// Implement the IIterable<DictionaryEntry<KeyT, ValueT>> interface.
	IIterator< DictionaryEntry<KeyT, ValueT> >
				IIterable< DictionaryEntry<KeyT, ValueT> >.GetIterator()
			{
				return new EnumeratorWrapper< DictionaryEntry<KeyT, ValueT> >
					(dict.GetEnumerator());
			}

}; // class DictionaryWrapper<KeyT, ValueT>

}; // namespace Generics
