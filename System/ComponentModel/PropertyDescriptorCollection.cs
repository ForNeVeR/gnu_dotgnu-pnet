/*
 * PropertyDescriptorCollection.cs - Implementation of the
 *	"System.ComponentModel.ComponentModel.PropertyDescriptorCollection" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.ComponentModel
{

#if CONFIG_COMPONENT_MODEL

using System.Collections;
using System.Runtime.InteropServices;

[TODO]
[ComVisible(true)]
public class PropertyDescriptorCollection : IDictionary, IEnumerable, 
											ICollection, IList
{
	// TODO
	public static readonly PropertyDescriptorCollection Empty =
								new PropertyDescriptorCollection(null);

	[TODO]
	public PropertyDescriptorCollection() 
	{
	}
	
	[TODO]
	public PropertyDescriptorCollection(PropertyDescriptor [] properties) 
	{
	}

	[TODO]
	public int Add(PropertyDescriptor value)
	{
		 throw new NotImplementedException("Add");
	}

	[TODO]
	public void Clear()
	{
		 throw new NotImplementedException("Clear");
	}

	[TODO]
	public bool Contains(PropertyDescriptor value)
	{
		 throw new NotImplementedException("Contains");
	}

	[TODO]
	public void CopyTo(Array array, int index)
	{
		 throw new NotImplementedException("CopyTo");
	}

	[TODO]
	public virtual PropertyDescriptor Find(String name, bool ignoreCase)
	{
		 throw new NotImplementedException("Find");
	}

	[TODO]
	public virtual IEnumerator GetEnumerator()
	{
		 throw new NotImplementedException("GetEnumerator");
	}

	[TODO]
	public int IndexOf(PropertyDescriptor value)
	{
		 throw new NotImplementedException("IndexOf");
	}

	[TODO]
	public void Insert(int index, PropertyDescriptor value)
	{
		 throw new NotImplementedException("Insert");
	}

	[TODO]
	protected void InternalSort(IComparer ic)
	{
		 throw new NotImplementedException("InternalSort");
	}

	[TODO]
	protected void InternalSort(String[] order)
	{
		 throw new NotImplementedException("InternalSort");
	}

	[TODO]
	public void Remove(PropertyDescriptor value)
	{
		 throw new NotImplementedException("Remove");
	}

	[TODO]
	public void RemoveAt(int index)
	{
		 throw new NotImplementedException("RemoveAt");
	}

	[TODO]
	public virtual PropertyDescriptorCollection Sort()
	{
		 throw new NotImplementedException("Sort");
	}

	[TODO]
	public virtual PropertyDescriptorCollection Sort(IComparer ic)
	{
		 throw new NotImplementedException("Sort");
	}

	[TODO]
	public virtual PropertyDescriptorCollection Sort(String []names)
	{
		 throw new NotImplementedException("Sort");
	}

	[TODO]
	public int Count 
	{
		get
		{
			throw new NotImplementedException("Count");
		}
	}

	[TODO]
	public bool IsReadOnly 
	{
		get
		{
			throw new NotImplementedException("IsReadOnly");
		}
	}

	[TODO]
	public bool IsSynchronized 
	{
		get
		{
			throw new NotImplementedException("IsSynchronized");
		}
	}

	[TODO]
	public virtual PropertyDescriptor this[String name] 
	{
		get
		{
			throw new NotImplementedException("Item");
		}
	}

	[TODO]
	public virtual PropertyDescriptor this[int index] 
	{
		get
		{
			throw new NotImplementedException("Item");
		}
	}

	// NOTE: only the missing interface cases have been implemented , others
	// are automatically obtained from the various virtual methods above.
	// Implementor might find it necessary to split them out into seperate
	// explicit interface methods.

	// IList implements

	[TODO]
	int IList.Add(Object value)
	{
		throw new NotImplementedException();
	}

	[TODO]
	bool IList.Contains(Object value)
	{
		throw new NotImplementedException();
	}

	[TODO]
	int IList.IndexOf(Object value)
	{
		throw new NotImplementedException();
	}

	[TODO]
	void IList.Insert(int index, Object value)
	{
		throw new NotImplementedException();
	}

	[TODO]
	void IList.Remove(Object value)
	{
		throw new NotImplementedException();
	}

	[TODO]
	bool IList.IsFixedSize 
	{
		get
		{
			throw new NotImplementedException();
		}
	}
	
	[TODO]
	Object IList.this[int index] 
	{
		get
		{
			throw new NotImplementedException();
		}
		set
		{
			throw new NotImplementedException();
		}
	}

	// ICollection implements
	Object ICollection.SyncRoot 
	{ 
		get
		{
			throw new NotImplementedException();
		}
	}

	// IDictionary implements

	void IDictionary.Add(Object key, Object value)
	{
		throw new NotImplementedException();
	}

	bool IDictionary.Contains(Object key)
	{
		throw new NotImplementedException();
	}

	IDictionaryEnumerator IDictionary.GetEnumerator()
	{
		throw new NotImplementedException();
	}

	void IDictionary.Remove(Object key)
	{
		throw new NotImplementedException();
	}

	bool IDictionary.IsFixedSize
	{
		get
		{
			throw new NotImplementedException();
		}
	}

	Object IDictionary.this[Object key]
	{
		get
		{
			throw new NotImplementedException();
		}
		set
		{
			throw new NotImplementedException();
		}
	}

	ICollection IDictionary.Keys
	{
		get
		{
			throw new NotImplementedException();
		}
	}
	
	ICollection IDictionary.Values
	{
		get
		{
			throw new NotImplementedException();
		}
	}
	

}; // class PropertyDescriptorCollection

#endif // CONFIG_COMPONENT_MODEL

}; // namespace System.ComponentModel
