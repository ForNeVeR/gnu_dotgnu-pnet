/*
 * LicenseManager.cs - Implementation of the
 *		"System.ComponentModel.ComponentModel.LicenseManager" class.
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

namespace System.ComponentModel
{

#if CONFIG_COMPONENT_MODEL

using System;

public sealed class LicenseManager
{
	// Internal state.
	private static LicenseContext currentContext;

	// Cannot instantiate this class.
	private LicenseManager() {}

	// Get or set the current license context.
	public static LicenseContext CurrentContext
			{
				get
				{
					lock(typeof(LicenseManager))
					{
						if(currentContext == null)
						{
							currentContext = new LicenseContext();
						}
						return currentContext;
					}
				}
				set
				{
					lock(typeof(LicenseManager))
					{
						if(currentContext != null)
						{
							throw new InvalidOperationException
								(S._("Invalid_LicenseContextChange"));
						}
						currentContext = value;
					}
				}
			}

	// Get the usage mode for the current license context.
	public static LicenseUsageMode UsageMode
			{
				get
				{
					return CurrentContext.UsageMode;
				}
			}

	// Create an instance of an object, within a particular license context.
	public static Object CreateWithContext
				(Type type, LicenseContext creationContext)
			{
				return CreateWithContext(type, creationContext, new Object [0]);
			}
	[TODO]
	public static Object CreateWithContext
				(Type type, LicenseContext creationContext, Object[] args)
			{
				// TODO
				return null;
			}

	// Determine if a type has a valid license.
	[TODO]
	public static bool IsLicensed(Type type)
			{
				// TODO
				return false;
			}

	// Determine if a valid license can be granted for a type.
	[TODO]
	public static bool IsValid(Type type)
			{
				// TODO
				return false;
			}
	[TODO]
	public static bool IsValid(Type type, Object instance, out License license)
			{
				// TODO
				return false;
			}

	// Lock the license context associated with an object.
	[TODO]
	public static void LockContext(Object contextUser)
			{
				// TODO
			}

	// Unlock the license context associated with an object.
	[TODO]
	public static void UnlockContext(Object contextUser)
			{
				// TODO
			}

	// Validate a license for a type.
	public static void Validate(Type type)
			{
				if(!IsValid(type))
				{
					throw new LicenseException(type);
				}
			}
	public static License Validate(Type type, Object instance)
			{
				License license;
				if(!IsValid(type, instance, out license))
				{
					throw new LicenseException(type, instance);
				}
				return license;
			}

}; // class LicenseManager

#endif // CONFIG_COMPONENT_MODEL

}; // namespace System.ComponentModel
