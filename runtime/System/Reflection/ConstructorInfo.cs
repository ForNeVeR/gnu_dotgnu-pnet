/*
 * ConstructorInfo.cs - Implementation of the
 *		"System.Reflection.ConstructorInfo" class.
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

namespace System.Reflection
{

#if CONFIG_REFLECTION

using System;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Diagnostics;

#if !ECMA_COMPAT
[ClassInterface(ClassInterfaceType.AutoDual)]
#endif
public abstract class ConstructorInfo : MethodBase
{

	// Name of constructor methods.
	public static readonly String ConstructorName = ".ctor";
	public static readonly String TypeConstructorName = ".cctor";

	// Constructor.
	protected ConstructorInfo() : base() {}

	// Get the member type for this item.
	public override MemberTypes MemberType
			{
				get
				{
					return MemberTypes.Constructor;
				}
			}

	// Invoke this constructor.
#if !ECMA_COMPAT
	[DebuggerStepThrough]
	[DebuggerHidden]
#endif
	public Object Invoke(Object[] parameters)
			{
				return Invoke(BindingFlags.Default, null, parameters, null);
			}
	public abstract Object Invoke(BindingFlags invokeAttr,
								  Binder binder, Object[] parameters,
								  CultureInfo culture);

}; // class ConstructorInfo

#endif // CONFIG_REFLECTION

}; // namespace System.Reflection
