/*
 * ClrMethod.cs - Implementation of the
 *		"System.Reflection.ClrMethod" class.
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
using System.Globalization;
using System.Runtime.CompilerServices;

internal sealed class ClrMethod : MethodInfo, IClrProgramItem
{

	// Private data used by the runtime engine.  This must be the first field.
	private IntPtr privateData;

	// Cached copy of the parameters.
	private ParameterInfo[] parameters;

	// Implement the IClrProgramItem interface.
	public IntPtr ClrHandle
			{
				get
				{
					return privateData;
				}
			}

	// Get the custom attributes attached to this method.
	public override Object[] GetCustomAttributes(bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, inherit);
			}
	public override Object[] GetCustomAttributes(Type type, bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, type, inherit);
			}

	// Determine if custom attributes are defined for this method.
	public override bool IsDefined(Type type, bool inherit)
			{
				return ClrHelpers.IsDefined(this, type, inherit);
			}

	// Get the parameters for this method.
	public override ParameterInfo[] GetParameters()
			{
				if(parameters != null)
				{
					return parameters;
				}
				int numParams = ClrHelpers.GetNumParameters(privateData);
				int param;
				parameters = new ParameterInfo [numParams];
				for(param = 0; param < numParams; ++param)
				{
					parameters[param] =
						ClrHelpers.GetParameterInfo(this, this, param + 1);
				}
				return parameters;
			}

	// Invoke this method.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override Object Invoke
				(Object obj, BindingFlags invokeAttr, Binder binder,
				 Object[] parameters, CultureInfo culture);

	// Get the method attributes.
	public override MethodAttributes Attributes
			{
				get
				{
					return (MethodAttributes)
						ClrHelpers.GetMemberAttrs(privateData);
				}
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
	public override Type ReturnType
			{
				get
				{
					return ClrHelpers.GetParameterType(privateData, 0);
				}
			}

	// Get the base definition for this method.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override MethodInfo GetBaseDefinition();

	// Get the runtime method handle associated with this method.
#if ECMA_COMPAT
	internal
#else
	public
#endif
	override RuntimeMethodHandle MethodHandle
			{
				get
				{
					return new RuntimeMethodHandle(privateData);
				}
			}

#if !ECMA_COMPAT

	// Get the custom attribute provider for the return type.
	public override ICustomAttributeProvider
				ReturnTypeCustomAttributes
			{
				get
				{
					return ClrHelpers.GetParameterInfo(this, this, 0);
				}
			}

	// Get the calling conventions for this method.
	public override CallingConventions CallingConvention
			{
				get
				{
					return ClrHelpers.GetCallConv(privateData);
				}
			}

	// Get the method implementation flags.
	public override MethodImplAttributes GetMethodImplementationFlags()
			{
				return ClrHelpers.GetImplAttrs(privateData);
			}

#endif // !ECMA_COMPAT

}; // class ClrMethod

}; // namespace System.Reflection
