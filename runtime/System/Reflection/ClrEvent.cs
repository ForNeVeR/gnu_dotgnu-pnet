/*
 * ClrEvent.cs - Implementation of the
 *		"System.Reflection.ClrEvent" class.
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
using System.Runtime.CompilerServices;

internal sealed class ClrEvent : EventInfo, IClrProgramItem
{

	// Private data used by the runtime engine.
	private IntPtr privateData;

	// Implement the IClrProgramItem interface.
	public IntPtr ClrHandle
			{
				get
				{
					return privateData;
				}
			}

	// Get the custom attributes attached to this event.
	public override Object[] GetCustomAttributes(bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, inherit);
			}
	public override Object[] GetCustomAttributes(Type type, bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, type, inherit);
			}

	// Determine if custom attributes are defined for this event.
	public override bool IsDefined(Type type, bool inherit)
			{
				return ClrHelpers.IsDefined(this, type, inherit);
			}

	// Override inherited properties.
	public override Type DeclaringType
			{
				get
				{
					return ClrHelpers.GetDeclaringType(this);
				}
			}
	public override Type ReflectedType
			{
				get
				{
					return ClrHelpers.GetReflectedType(this);
				}
			}
	public override String Name
			{
				get
				{
					return ClrHelpers.GetName(this);
				}
			}
	public override EventAttributes Attributes
			{
				get
				{
					return (EventAttributes)
						ClrHelpers.GetMemberAttrs(privateData);
				}
			}
	public override Type EventHandlerType
			{
				get
				{
					return GetEventType(privateData);
				}
			}

	// Get the add method for this event.
	public override MethodInfo GetAddMethod(bool nonPublic)
			{
				return ClrHelpers.GetSemantics
							(privateData,
						 	 MethodSemanticsAttributes.AddOn,
							 nonPublic);
			}

	// Get the raise method for this event.
	public override MethodInfo GetRaiseMethod(bool nonPublic)
			{
				return ClrHelpers.GetSemantics
							(privateData,
						 	 MethodSemanticsAttributes.Fire,
							 nonPublic);
			}

	// Get the remove method for this event.
	public override MethodInfo GetRemoveMethod(bool nonPublic)
			{
				return ClrHelpers.GetSemantics
							(privateData,
						 	 MethodSemanticsAttributes.RemoveOn,
							 nonPublic);
			}

	// Get the type associated with this event item.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static Type GetEventType(IntPtr item);

}; // class ClrEvent

}; // namespace System.Reflection