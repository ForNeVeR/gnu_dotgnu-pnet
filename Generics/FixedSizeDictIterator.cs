/*
 * FixedSizeDictIterator.cs - Wrap an iterator to make it fixed-size.
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

internal sealed class FixedSizeDictIterator<KeyT, ValueT>
	: IDictionaryIterator<KeyT, ValueT>
{
	// Internal state.
	protected IDictionaryIterator<KeyT, ValueT> iterator;

	// Constructor.
	public FixedSizeDictIterator(IDictionaryIterator<KeyT, ValueT> iterator)
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
					(S._("NotSupp_FixedSizeCollection"));
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
					iterator.Value = value;
				}
			}

}; // class FixedSizeDictIterator<KeyT, ValueT>

}; // namespace Generics
