/*
 * BaseCollection.cs - Implementation of the
 *			"System.Windows.Forms.BaseCollection" class.
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

namespace System.Windows.Forms
{

using System.Collections;
using System.ComponentModel;

public class BaseCollection : MarshalByRefObject, ICollection, IEnumerable
{
	// Constructor.
	public BaseCollection() {}

	// Implement the ICollection interface.
	public void CopyTo(Array array, int index)
			{
				List.CopyTo(array, index);
			}
	[Browsable(false)]
	[EditorBrowsable(EditorBrowsableState.Advanced)]
	public virtual int Count
			{
				get
				{
					return List.Count;
				}
			}
	[EditorBrowsable(EditorBrowsableState.Advanced)]
	[Browsable(false)]
	public bool IsSynchronized
			{
				get
				{
					return false;
				}
			}
	[EditorBrowsable(EditorBrowsableState.Advanced)]
	[Browsable(false)]
	public Object SyncRoot
			{
				get
				{
					return this;
				}
			}

	// Implement the IEnumerable interface.
	public IEnumerator GetEnumerator()
			{
				return List.GetEnumerator();
			}

	// Determine if the collection is read-only.
	[EditorBrowsable(EditorBrowsableState.Advanced)]
	[Browsable(false)]
	public bool IsReadOnly
			{
				get
				{
					return false;
				}
			}

	// Get the array list that underlies this collection
	protected virtual ArrayList List
			{
				get
				{
					return null;
				}
			}

}; // class BaseCollection

}; // namespace System.Windows.Forms
