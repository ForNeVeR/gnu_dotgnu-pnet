/*
 * ObjectIDGenerator.cs - Implementation of the
 *			"System.Runtime.Serialization.ObjectIDGenerator" class.
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

namespace System.Runtime.Serialization
{

#if CONFIG_SERIALIZATION

using System.Collections;

public class ObjectIDGenerator
{
	// Internal state.
	private Hashtable table;
	private long nextId;

	// Constructor.
	public ObjectIDGenerator()
			{
				table = new Hashtable();
				nextId = 1;
			}

	// Get an identifier for an object.
	public virtual long GetId(Object obj, out bool firstTime)
			{
				if(obj == null)
				{
					throw new ArgumentNullException("obj");
				}
				Object id = table[obj];
				if(id != null)
				{
					firstTime = false;
					return (long)id;
				}
				else
				{
					firstTime = true;
					table[obj] = (Object)(nextId);
					return nextId++;
				}
			}

	// Determine if an object already has an identifier.
	public virtual long HasId(Object obj, out bool firstTime)
			{
				if(obj == null)
				{
					throw new ArgumentNullException("obj");
				}
				Object id = table[obj];
				if(id != null)
				{
					firstTime = false;
					return (long)id;
				}
				else
				{
					firstTime = true;
					return 0;
				}
			}

}; // class ObjectIDGenerator

#endif // CONFIG_SERIALIZATION

}; // namespace System.Runtime.Serialization
