/*
 * RuntimePropertyInfo.cs - Implementation of the
 *		"System.Reflection.RuntimePropertyInfo" class.
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

namespace System.Reflection
{

using System;

internal sealed class RuntimePropertyInfo : PropertyInfo
{

	// Private data used by the runtime engine.
	private IntPtr privateData;

	// Get the custom attributes attached to this property.
	public override Object[] GetCustomAttributes(bool inherit)
			{
				return RuntimeHelpers.GetCustomAttributes
							(privateData, (IntPtr)0, inherit);
			}
	public override Object[] GetCustomAttributes(Type type, bool inherit)
			{
				return RuntimeHelpers.GetCustomAttributes
							(privateData, type, inherit);
			}

	// Determine if custom attributes are defined for this property.
	public override bool IsDefined(Type type, bool inherit)
			{
				return RuntimeHelpers.IsDefined(privateData, type, inherit);
			}

	// Override inherited properties.
	public override Type DeclaringType
			{
				get
				{
					return RuntimeType.GetTypeFromHandleImpl
							(RuntimeHelpers.GetDeclaringType(privateData));
				}
			}
	public override Type ReflectedType
			{
				get
				{
					return RuntimeType.GetTypeFromHandleImpl
							(RuntimeHelpers.GetReflectedType(privateData));
				}
			}
	public override String Name
			{
				get
				{
					return RuntimeHelpers.GetMemberName(privateData);
				}
			}

}; // class RuntimePropertyInfo

}; // namespace System.Reflection
