/*
 * SynchronizedDictEnumerator.cs - Wrap an enumerator to synchronize it.
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

internal sealed class SynchronizedDictEnumerator<KeyT, ValueT>
		: IDictionaryEnumerator<KeyT, ValueT>
{
	// Internal state.
	protected Object syncRoot;
	protected IDictionaryEnumerator<KeyT, ValueT> enumerator;

	// Constructor.
	public SynchronizedDictEnumerator
				(Object syncRoot,
				 IDictionaryEnumerator<KeyT, ValueT> enumerator)
			{
				this.syncRoot = syncRoot;
				this.enumerator = enumerator;
			}

	// Implement the IEnumerator<ValueT> interface.
	public bool MoveNext()
			{
				lock(syncRoot)
				{
					return enumerator.MoveNext();
				}
			}
	public void Reset()
			{
				lock(syncRoot)
				{
					enumerator.Reset();
				}
			}
	public ValueT Current
			{
				get
				{
					lock(syncRoot)
					{
						return enumerator.Value;
					}
				}
			}

	// Implement the IDictionaryEnumerator<KeyT, ValueT> interface.
	public DictionaryEntry<KeyT, ValueT> Entry
			{
				get
				{
					lock(syncRoot)
					{
						return enumerator.Entry;
					}
				}
			}
	public KeyT Key
			{
				get
				{
					lock(syncRoot)
					{
						return enumerator.Key;
					}
				}
			}
	public ValueT Value
			{
				get
				{
					lock(syncRoot)
					{
						return enumerator.Value;
					}
				}
			}

}; // class SynchronizedDictEnumerator<KeyT, ValueT>

}; // namespace Generics
