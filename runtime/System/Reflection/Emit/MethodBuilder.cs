/*
 * MethodBuilder.cs - Implementation of the
 *		"System.Reflection.Emit.MethodBuilder" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.Reflection.Emit
{

#if !ECMA_COMPAT

using System;
using System.Reflection;
using System.Globalization;
using System.Runtime.CompilerServices;

[TODO]
public sealed class MethodBuilder : MethodInfo
{

	// TODO
	[TODO]
	public void CreateMethodBody(byte[] il, int count)
	{
		throw new NotImplementedException("CreateMethodBody");
	}

	[TODO]
	public ParameterBuilder DefineParameter(int position, 
											ParameterAttributes attributes, 
											String strParamName)
	{
		throw new NotImplementedException("DefineParameter");
	}

	[TODO]
	public override MethodInfo GetBaseDefinition()
	{
		throw new NotImplementedException("GetBaseDefinition");
	}

	[TODO]
	public override Object[] GetCustomAttributes(bool inherit)
	{
		throw new NotImplementedException("GetCustomAttributes");
	}

	[TODO]
	public override Object[] GetCustomAttributes(Type attributeType,
												bool inherit)
	{
		throw new NotImplementedException("GetCustomAttributes");
	}

	[TODO]
	public ILGenerator GetILGenerator()
	{
		throw new NotImplementedException("GetILGenerator");
	}

	[TODO]
	public ILGenerator GetILGenerator(int size)
	{
		throw new NotImplementedException("GetILGenerator");
	}

	[TODO]
	public override MethodImplAttributes GetMethodImplementationFlags()
	{
		throw new NotImplementedException("GetMethodImplementationFlags");
	}

	[TODO]
	public override ParameterInfo[] GetParameters()
	{
		throw new NotImplementedException("GetParameters");
	}

	[TODO]
	public MethodToken GetToken()
	{
		throw new NotImplementedException("GetToken");
	}

	[TODO]
	public override Object Invoke(Object obj, BindingFlags invokeAttr, 
								  Binder binder, Object[] parameters, 
								  CultureInfo culture)
	{
		throw new NotImplementedException("Invoke");
	}

	[TODO]
	public override bool IsDefined(Type attribute_type, bool inherit)
	{
		throw new NotImplementedException("IsDefined");
	}

	[TODO]
	public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
	{
		throw new NotImplementedException("SetCustomAttribute");
	}

	[TODO]
	public void SetCustomAttribute(ConstructorInfo con, byte[] binaryAttribute)
	{
		throw new NotImplementedException("SetCustomAttribute");
	}

	[TODO]
	public void SetImplementationFlags(MethodImplAttributes attributes)
	{
		throw new NotImplementedException("SetImplementationFlags");
	}

	[TODO]
	public override MethodAttributes Attributes 
	{ 
		get
		{
			throw new NotImplementedException("Attributes");
		}
	}

	[TODO]
	public override Type DeclaringType 
	{
		get
		{
			throw new NotImplementedException("DeclaringType");
		}
	}

	[TODO]
	public bool InitLocals 
	{
		get
		{
			throw new NotImplementedException("InitLocals");
		}
		set
		{
			throw new NotImplementedException("InitLocals");
		}
	}

	[TODO]
	public override RuntimeMethodHandle MethodHandle
	{
		get
		{
			throw new NotImplementedException("MethodHandle");
		}
	}

	[TODO]
	public override String Name 
	{
		get
		{
			throw new NotImplementedException("Name");
		}
	}

	[TODO]
	public override Type ReflectedType 
	{
		get
		{
			throw new NotImplementedException("ReflectedType");
		}
	}

	[TODO]
	public override Type ReturnType 
	{
		get
		{
			throw new NotImplementedException("ReturnType");
		}
	}

	[TODO]
	public override ICustomAttributeProvider ReturnTypeCustomAttributes
	{
		get
		{
			throw new NotImplementedException("ReturnTypeCustomAttributes");
		}
	}

}; // class MethodBuilder

#endif // !ECMA_COMPAT

}; // namespace System.Reflection.Emit
