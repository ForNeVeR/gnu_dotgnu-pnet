/*
 * ClientSponsor.cs - Implementation of the
 *			"System.Runtime.Remoting.Lifetime.ClientSponsor" class.
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

namespace System.Runtime.Remoting.Lifetime
{

#if CONFIG_REMOTING

public class ClientSponsor : MarshalByRefObject, ISponsor
{
	// Internal state.
	private TimeSpan renewalTime;

	// Constructors.
	public ClientSponsor() : this(new TimeSpan(0, 2, 0)) {}
	public ClientSponsor(TimeSpan renewalTime)
			{
				this.renewalTime = renewalTime;
			}

	// Get or set the renewal time.
	public TimeSpan RenewalTime
			{
				get
				{
					return renewalTime;
				}
				set
				{
					renewalTime = value;
				}
			}

	// Empty the registration list.
	[TODO]
	public void Close()
			{
				// TODO
			}

	// Initialize the lifetime service for this object.
	public override Object InitializeLifetimeService()
			{
				// Nothing to do here.
				return null;
			}

	// Register an object for sponsorship.
	[TODO]
	public bool Register(MarshalByRefObject obj)
			{
				// TODO
				return false;
			}

	// Request renewal of a lease.
	[TODO]
	public TimeSpan Renewal(ILease lease)
			{
				// TODO
				return new TimeSpan(0);
			}

	// Unregister an object from the sponsorship list.
	[TODO]
	public void Unregister(MarshalByRefObject obj)
			{
				// TODO
			}

}; // class ClientSponsor

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Lifetime
