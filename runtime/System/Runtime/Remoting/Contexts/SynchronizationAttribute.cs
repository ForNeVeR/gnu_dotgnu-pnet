/*
 * SynchronizationAttribute.cs - Implementation of the
 *			"System.Runtime.Remoting.Contexts.SynchronizationAttribute" class.
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

using System.Runtime.Remoting.Messaging;
using System.Runtime.Remoting.Activation;

[Serializable]
[AttributeUsage(AttributeTargets.Class)]
public class SynchronizationAttribute : ContextAttribute
{
	// Internal state.
	private int flag;
	private bool reEntrant;
	private bool locked;

	// Public flag values.
	public const int NOT_SUPPORTED	= 0x0001;
	public const int SUPPORTED		= 0x0002;
	public const int REQUIRED		= 0x0004;
	public const int REQUIRES_NEW	= 0x0008;

	// Constructors.
	public SynchronizationAttribute() : this(REQUIRED, false) {}
	public SynchronizationAttribute(bool reEntrant)
			: this(REQUIRED, reEntrant) {}
	public SynchronizationAttribute(int flag) : this(flag, false) {}
	public SynchronizationAttribute(int flag, bool reEntrant)
			: base("Synchronization")
			{
				if(flag != NOT_SUPPORTED && flag != SUPPORTED &&
				   flag != REQUIRED && flag != REQUIRES_NEW)
				{
					throw new ArgumentException
						(_("Arg_SynchronizationFlag"));
				}
				this.flag = flag;
				this.reEntrant = reEntrant;
			}

	// Determine if this attribute is re-entrant.
	public virtual bool IsReEntrant
			{
				get
				{
					return reEntrant;
				}
			}

	// Get or set the context lock.
	public virtual bool Locked
			{
				get
				{
					return locked;
				}
				set
				{
					locked = value;
				}
			}

	// Create a client context sink and prepend it to a given chain.
	[TODO]
	public virtual IMessageSink GetClientContextSink(IMessageSink nextSink)
			{
				// TODO
				return null;
			}

	// Get the properties for a new construction context.
	[TODO]
	public override void GetPropertiesForNewContext
				(IConstructionCallMessage ctorMsg)
			{
				// TODO
			}

	// Determine if a context is OK with respect to this attribute.
	[TODO]
	public override bool IsContextOK
				(Context ctx, IConstructionCallMessage msg)
			{
				// TODO
				return false;
			}

}; // class SynchronizationAttribute

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Contexts
