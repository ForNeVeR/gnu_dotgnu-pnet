/*
 * SynchronizedIterator.cs - Wrap an iterator to synchronize it.
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

internal sealed class SynchronizedIterator<T> : IIterator<T>
{
	// Internal state.
	protected Object       syncRoot;
	protected IIterator<T> iterator;

	// Constructor.
	public SynchronizedIterator(Object syncRoot, IIterator<T> iterator)
			{
				this.syncRoot = syncRoot;
				this.iterator = iterator;
			}

	// Implement the IIterator<T> interface.
	public bool MoveNext()
			{
				lock(syncRoot)
				{
					return iterator.MoveNext();
				}
			}
	public void Reset()
			{
				lock(syncRoot)
				{
					iterator.Reset();
				}
			}
	public void Remove()
			{
				lock(syncRoot)
				{
					iterator.Remove();
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
			}

}; // class SynchronizedIterator<T>

}; // namespace Generics
