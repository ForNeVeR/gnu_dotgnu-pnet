/*
 * IList.cs - Implementation of the
 *		"System.Collections.Generic.IList" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace System.Collections.Generic
{

#if CONFIG_GENERICS

using System.Runtime.InteropServices;

#if !ECMA_COMPAT
[ComVisible(false)]
#endif
[CLSCompliant(false)]
public interface IList<T> : ICollection<T>
{
	int Add(T item);
	void Clear();
	bool Contains(T item);
	int IndexOf(T item);
	void Insert(int index, T item);
	void Remove(T item);
	void RemoveAt(int index);
	bool IsFixedSize { get; }
	bool IsReadOnly { get; }
	T this[int index] { get; set; }

}; // interface IList<T>

#endif // CONFIG_GENERICS

}; // namespace System.Collections.Generic
