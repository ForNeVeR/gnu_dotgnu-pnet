/*
 * Context.cs - Implementation of the
 *			"System.Runtime.Remoting.Contexts.Context" class.
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

namespace System.Runtime.Remoting.Contexts
{

#if CONFIG_REMOTING

public class Context
{
	// Internal state.
	private int contextID;
	private IContextProperty[] properties;

	// Constructor.
	[TODO]
	public Context()
			{
				// TODO
			}

	// Destructor.
	~Context()
			{
				// TODO
			}

	// Allocate a local data slot.
	[TODO]
	public static LocalDataStoreSlot AllocateDataSlot()
			{
				// TODO
				return null;
			}

	// Allocate a named local data slot.
	[TODO]
	public static LocalDataStoreSlot AllocateNamedDataSlot(String name)
			{
				// TODO
				return null;
			}

	// Perform a cross-context callback.
	[TODO]
	public void DoCallBack(CrossContextDelegate deleg)
			{
				// TODO
			}

	// Free a named local data slot.
	[TODO]
	public static void FreeNamedDataSlot(String name)
			{
				// TODO
			}

	// Freeze this context.
	[TODO]
	public virtual void Freeze()
			{
				// TODO
			}

	// Get the data stored in a particular data store slot.
	[TODO]
	public static Object GetData(LocalDataStoreSlot slot)
			{
				// TODO
				return null;
			}

	// Get a named data store slot.
	[TODO]
	public static LocalDataStoreSlot GetNamedDataSlot(String name)
			{
				// TODO
				return null;
			}

	// Get a property from this context.
	[TODO]
	public IContextProperty GetProperty(String name)
			{
				// TODO
				return null;
			}

	// Register a dynamic property.
	[TODO]
	public static bool RegisterDynamicProperty
				(IDynamicProperty prop, ContextBoundObject obj, Context ctx)
			{
				// TODO
				return false;
			}

	// Set the value of a local data store slot.
	[TODO]
	public static void SetData(LocalDataStoreSlot slot, Object value)
			{
				// TODO
			}

	// Set a property on this context.
	[TODO]
	public void SetProperty(IContextProperty prop)
			{
				// TODO
			}

	// Convert this object into a string.
	[TODO]
	public override String ToString()
			{
				// TODO
				return null;
			}

	// Unregister a dynamic property.
	[TODO]
	public static bool UnregisterDynamicProperty
				(String name, ContextBoundObject obj, Context ctx)
			{
				// TODO
				return false;
			}

	// Get the identifier for this context.
	public virtual int ContextID
			{
				get
				{
					return contextID;
				}
			}

	// Get the properties on this context.
	public virtual IContextProperty[] ContextProperties
			{
				get
				{
					return properties;
				}
			}

	// Get the default context.
	[TODO]
	public static Context DefaultContext
			{
				get
				{
					// TODO
					return null;
				}
			}

}; // class Context

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Contexts
