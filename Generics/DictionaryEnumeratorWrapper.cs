/*
 * DictionaryEnumeratorWrapper.cs - Wrap a non-generic dictionary enumerator
 *		to turn it into a generic one.
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

public sealed class DictionaryEnumeratorWrapper<KeyT, ValueT>
	: IDictionaryIterator<KeyT, ValueT>
{

	// Internal state.
	private System.Collections.IDictionaryEnumerator e;

	// Constructor.
	public DictionaryEnumeratorWrapper
				(System.Collections.IDictionaryEnumerator e)
			{
				if(e == null)
				{
					throw new ArgumentNullException("e");
				}
				this.e = e;
			}

	// Implement the IIterator<ValueT> interface.
	public bool MoveNext()
			{
				return e.MoveNext();
			}
	public void Reset()
			{
				e.Reset();
			}
	public void Remove()
			{
				throw new InvalidOperationException(S._("NotSupp_Remove"));
			}
	public DictionaryEntry<KeyT, ValueT> Current
			{
				get
				{
					System.Collections.DictionaryEntry entry = e.Entry;
					return new DictionaryEntry<KeyT, ValueT>
						((KeyT)(e.Key), (ValueT)(e.Value));
				}
			}

	// Implement the IDictionaryIterator<KeyT, ValueT> interface.
	public KeyT Key
			{
				get
				{
					return (KeyT)(e.Key);
				}
			}
	public ValueT Value
			{
				get
				{
					return (ValueT)(e.Value);
				}
			}

}; // class DictionaryEnumeratorAdapter<KeyT, ValueT>

}; // namespace Generics
