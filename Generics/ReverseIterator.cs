/*
 * ReverseIterator.cs - Wrap an iterator to reverse its traversal direction.
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

namespace Generics
{

using System;

public sealed class ReverseIterator<T> : IEnumerator<T>, IIterator<T>
{
	// Internal state.
	protected IIterator<T> iterator;

	// Constructor.
	public ReverseIterator(IIterator<T> iterator)
			{
				this.iterator = iterator;
			}

	// Implement the IEnumerator<T> interface.
	public bool MoveNext()
			{
				lock(syncRoot)
				{
					return iterator.MovePrev();
				}
			}
	public void Reset()
			{
				lock(syncRoot)
				{
					iterator.Reset();
				}
			}
	T IEnumerator<T>.Current
			{
				get
				{
					lock(syncRoot)
					{
						return ((IEnumerator<T>)iterator).Current;
					}
				}
			}

	// Implement the IIterator<T> interface.
	public bool MovePrev()
			{
				lock(syncRoot)
				{
					return iterator.MoveNext();
				}
			}
	public T Current
			{
				get
				{
					lock(syncRoot)
					{
						return iterator.Current;
					}
				}
				set
				{
					lock(syncRoot)
					{
						iterator.Current = value;
					}
				}
			}

}; // class ReverseIterator<T>

}; // namespace Generics
