/*
 * KeyValuePair.cs - Implementation of the
 *		"System.Collections.Generic.KeyValuePair" class.
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
public struct KeyValuePair<K,V>
{
	// Internal state.
	private K key;
	private V value;

	// Constructor.
	public KeyValuePair(K key, V value)
			{
				this.key = key;
				this.value = value;
			}

	// Properties.
	public K Key
			{
				get
				{
					return key;
				}
				set
				{
					key = value;
				}
			}
	public V Value
			{
				get
				{
					return this.value;
				}
				set
				{
					this.value = value;
				}
			}

}; // struct KeyValuePair<K,V>

#endif // CONFIG_GENERICS

}; // namespace System.Collections.Generic
