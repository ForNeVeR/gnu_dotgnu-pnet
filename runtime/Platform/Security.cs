/*
 * Security.cs - Implementation of the "Platform.Security" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace Platform
{

using System;
using System.Runtime.CompilerServices;

internal sealed class Security
{
	// Null security manager, which is used as the fallback default.
	private static readonly ISecurityManager Null = new NullSecurityManager();

	// Get or set the security manager for the current thread.
	// If the thread does not have a security manager, then the
	// null security manager will be used.
	public static ISecurityManager SecurityManager
			{
				get
				{
					ISecurityManager mgr = GetSecurityManager();
					if(mgr != null)
					{
						return mgr;
					}
					else
					{
						return Null;
					}
				}
				set
				{
					SetSecurityManager(value);
				}
			}

	// Get the security manager for the current thread, or null
	// if there is no manager for the current thread.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static ISecurityManager GetSecurityManager();

	// Set the security manager for the current thread.  If the
	// manager is set to null, then there will be no manager for
	// the thread.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static void SetSecurityManager(ISecurityManager mgr);

}; // class Security

}; // namespace Platform
