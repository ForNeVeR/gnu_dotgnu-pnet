/*
 * IDictionary.cs - Generic dictionaries.
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

public interface IDictionary<KeyT, ValueT> : ICollection<ValueT>
{

	void Add(KeyT key, ValueT value);
	void Clear();
	bool Contains(KeyT key);
	new IDictionaryIterator<KeyT, ValueT> GetIterator();
	void Remove(KeyT key);
	bool IsFixedSize { get; }
	bool IsReadOnly { get; }
	ValueT this[KeyT key] { get; set; }
	ICollection<KeyT> Keys { get; }
	ICollection<ValueT> Values { get; }

}; // interface IDictionary<KeyT, ValueT>

}; // namespace Generics
