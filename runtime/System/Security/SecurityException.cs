/*
 * SecurityException.cs - Implementation of the
 *		"System.Security.SecurityException" class.
 *
 * Copyright (C) 2001, 2003  Southern Storm Software, Pty Ltd.
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

namespace System.Security
{

using System;
using System.Runtime.Serialization;

public class SecurityException : SystemException
{
#if !ECMA_COMPAT
	// Internal state.
	private String permissionState;
	private Type permissionType;
#endif

	// Constructors.
	public SecurityException()
			: base(_("Exception_Security")) {}
	public SecurityException(String msg)
			: base(msg) {}
	public SecurityException(String msg, Exception inner)
			: base(msg, inner) {}
#if !ECMA_COMPAT
	protected SecurityException(SerializationInfo info,
								StreamingContext context)
			: base(info, context)
			{
				permissionState = info.GetString("PermissionState");
			}
#endif

	// Get the default message to use for this exception type.
	internal override String MessageDefault
			{
				get
				{
					return _("Exception_Security");
				}
			}

	// Get the default HResult value for this type of exception.
	internal override uint HResultDefault
			{
				get
				{
					return 0x8013150a;
				}
			}

#if !ECMA_COMPAT

	// Get the permission type.
	public Type PermissionType
			{
				get
				{
					return permissionType;
				}
			}

	// Get the permission state.
	public String PermissionState
			{
				get
				{
					return permissionState;
				}
			}

	// Get the serialization data for this object.
	public override void GetObjectData(SerializationInfo info,
									   StreamingContext context)
			{
				base.GetObjectData(info, context);
				info.AddValue("PermissionState", permissionState);
			}

#endif // !ECMA_COMPAT

}; // class SecurityException

}; // namespace System.Security
