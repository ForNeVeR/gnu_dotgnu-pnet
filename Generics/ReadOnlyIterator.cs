/*
 * ReadOnlyIterator.cs - Wrap an iterator to make it read-only.
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

internal sealed class ReadOnlyIterator<T> : IEnumerator<T>, IIterator<T>
{
	// Internal state.
	protected IIterator<T> iterator;

	// Constructor.
	public ReadOnlyIterator(IIterator<T> iterator)
			{
				this.iterator = iterator;
			}

	// Implement the IEnumerator<T> interface.
	public bool MoveNext()
			{
				return iterator.MoveNext();
			}
	public void Reset()
			{
				iterator.Reset();
			}
	T IEnumerator<T>.Current
			{
				get
				{
					return ((IEnumerator<T>)iterator).Current;
				}
			}

	// Implement the IIterator<T> interface.
	public bool MovePrev()
			{
				return iterator.MovePrev();
			}
	public T Current
			{
				get
				{
					return iterator.Current;
				}
				set
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
			}

}; // class ReadOnlyIterator<T>

}; // namespace Generics
