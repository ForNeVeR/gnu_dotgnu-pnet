/*
 * RuntimeHelpers.cs - Implementation of the
 *		"System.Reflection.RuntimeHelpers" class.
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
using System.IO;
using System.Globalization;
using System.Runtime.InteropServices;

internal sealed class RuntimeHelpers
{
	// Get the custom attributes for a program item.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Object[] GetCustomAttributes
					(IntPtr privateData, IntPtr runtimeType, bool inherit);
	public static Object[] GetCustomAttributes
					(IntPtr privateData, Type type, bool inherit)
			{
				RuntimeType runtimeType;
				IntPtr typePrivate;
				if(type == null)
				{
					throw new ArgumentNullException("type");
				}
				if((runtimeType = (type as RuntimeType)) == null)
				{
					throw new ArgumentException
						(Environment.GetResourceString("Arg_MustBeType"));
				}
				typePrivate = runtimeType.privateData;
				if(typePrivate == (IntPtr)0)
				{
					throw new ArgumentException
						(Environment.GetResourceString("Arg_MustBeType"));
				}
				return GetCustomAttributes(privateData, typePrivate, inherit);
			}

	// Determine if custom attributes exist on a program item.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool IsDefined(IntPtr privateData,
									    IntPtr runtimeType,
										bool inherit);
	public static bool IsDefined(IntPtr privateData, Type type, bool inherit)
			{
				RuntimeType runtimeType;
				IntPtr typePrivate;
				if(type == null)
				{
					return IsDefined(privateData, (IntPtr)0, inherit);
				}
				if((runtimeType = (type as RuntimeType)) == null)
				{
					throw new ArgumentException
						(Environment.GetResourceString("Arg_MustBeType"));
				}
				typePrivate = runtimeType.privateData;
				if(typePrivate == (IntPtr)0)
				{
					throw new ArgumentException
						(Environment.GetResourceString("Arg_MustBeType"));
				}
				return IsDefined(privateData, typePrivate, inherit);
			}

	// Get the declaring type for a member item.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static RuntimeTypeHandle GetDeclaringType(IntPtr privateData);

	// Get the reflected type for a member item.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static RuntimeTypeHandle GetReflectedType(IntPtr privateData);

	// Get the name of a member item.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static String GetMemberName(IntPtr privateData);

}; // class RuntimeHelpers

}; // namespace System.Reflection
