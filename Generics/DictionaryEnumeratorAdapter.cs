/*
 * DictionaryEnumeratorAdapter.cs - Adapt a generic dictionary enumerator
 *		into a non-generic one.
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

public sealed class DictionaryEnumeratorAdapter<KeyT, ValueT>
	: System.Collections.IDictionaryEnumerator, System.Collections.IEnumerator
{

	// Internal state.
	private IDictionaryIterator<KeyT, ValueT> e;

	// Constructor.
	public DictionaryEnumeratorAdapter(IDictionaryIterator<KeyT, ValueT> e)
			{
				if(e == null)
				{
					throw new ArgumentNullException("e");
				}
				this.e = e;
			}

	// Implement the non-generic IEnumerator interface.
	public bool MoveNext()
			{
				return e.MoveNext();
			}
	public void Reset()
			{
				e.Reset();
			}
	public Object Current
			{
				get
				{
					return e.Current;
				}
			}

	// Implement the non-generic IDictionaryEnumerator interface.
	public System.Collections.DictionaryEntry Entry
			{
				get
				{
					DictionaryEntry<KeyT, ValueT> entry = e.Entry;
					return new System.Collections.DictionaryEntry
						(e.Key, e.Value);
				}
			}
	public Object Key
			{
				get
				{
					return e.Key;
				}
			}
	public Object Value
			{
				get
				{
					return e.Value;
				}
			}

}; // class DictionaryEnumeratorAdapter<KeyT, ValueT>

}; // namespace Generics
