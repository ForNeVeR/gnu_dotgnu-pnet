/*
 * WeakReference.cs - Implementation of the "System.WeakReference" class.
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

namespace System
{

#if !ECMA_COMPAT

public class WeakReference
{

	// Internal state.
	private Object obj;
	private bool trackResurrection;

	// Constructors.
	public WeakReference(Object obj) : this(obj, false) {}
	public WeakReference(Object obj, bool trackResurrection)
			{
				// TODO: the current semantics result in "strong" references.
				this.obj = obj;
				this.trackResurrection = trackResurrection;
			}

	// Destructor.
	~WeakReference()
			{
				// TODO
			}

	// Determine if the referenced object is still alive.
	public virtual bool IsAlive
			{
				get
				{
					// TODO
					return (obj != null);
				}
			}

	// Get or set the target of this weak reference.
	public virtual Object Target
			{
				get
				{
					// TODO
					return obj;
				}
				set
				{
					// TODO
					obj = value;
				}
			}

	// Determine if the object is tracked after finalization.
	public virtual bool TrackResurrection
			{
				get
				{
					// TODO
					return trackResurrection;
				}
			}

}; // class WeakReference

#endif // !ECMA_COMPAT

}; // namespace System
