/*
 * LifetimeServices.cs - Implementation of the
 *			"System.Runtime.Remoting.Lifetime.LifetimeServices" class.
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

using System.Security.Permissions;

[SecurityPermission(SecurityAction.LinkDemand,
					Flags=SecurityPermissionFlag.Infrastructure)]
public sealed class LifetimeServices
{
	// Internal state.
	private static TimeSpan leaseManagerPollTime;
	private static TimeSpan leaseTime;
	private static TimeSpan renewOnCallTime;
	private static TimeSpan sponsorshipTimeout;

	// Get or set the global lease manager poll time setting.
	public static TimeSpan LeaseManagerPollTime
			{
				get
				{
					return leaseManagerPollTime;
				}
				set
				{
					leaseManagerPollTime = value;
				}
			}

	// Get or set the global lease time setting.
	public static TimeSpan LeaseTime
			{
				get
				{
					return leaseTime;
				}
				set
				{
					leaseTime = value;
				}
			}

	// Get or set the global renew on call time setting.
	public static TimeSpan RenewOnCallTime
			{
				get
				{
					return renewOnCallTime;
				}
				set
				{
					renewOnCallTime = value;
				}
			}

	// Get or set the global sponsorship timeout setting.
	public static TimeSpan SponsorshipTimeout
			{
				get
				{
					return sponsorshipTimeout;
				}
				set
				{
					sponsorshipTimeout = value;
				}
			}

	// Get the default lifetime service object for a marshal-by-ref object.
	[TODO]
	internal static Object GetLifetimeService(MarshalByRefObject obj)
			{
				// TODO
				return null;
			}

	// Initialize a lifetime service object for a marshal-by-ref object.
	[TODO]
	internal static Object InitializeLifetimeService(MarshalByRefObject obj)
			{
				// TODO
				return null;
			}

}; // class LifetimeServices

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Lifetime
