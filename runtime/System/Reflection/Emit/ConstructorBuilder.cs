/*
 * ConstructorBuilder.cs - Implementation of the
 *		"System.Reflection.Emit.ConstructorBuilder" class.
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
using System.Security;
using System.Reflection;
using System.Globalization;
using System.Security.Permissions;
using System.Runtime.CompilerServices;

[TODO]
public sealed class ConstructorBuilder : ConstructorInfo
{
	[TODO]
	public void AddDeclarativeSecurity(SecurityAction action, 
										PermissionSet pset)
	{
		throw new NotImplementedException("AddDeclarativeSecurity");
	}

	[TODO]
	public ParameterBuilder DefineParameter(int iSequence, 
											ParameterAttributes attributes, 
											String strParamName)
	{
		throw new NotImplementedException("DefineParameter");
	}

	[TODO]
	public override Object[] GetCustomAttributes(bool inherit)
	{
		throw new NotImplementedException("GetCustomAttributes");
	}

	[TODO]
	public override Object[] GetCustomAttributes(Type attribute_type, 
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
	public override MethodImplAttributes GetMethodImplementationFlags()
	{
		throw new NotImplementedException("GetMethodImplementationFlags");
	}

	[TODO]
	public Module GetModule()
	{
		throw new NotImplementedException("GetModule");
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
	public override Object Invoke(BindingFlags invokeAttr, 
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
	public void SetSymCustomAttribute(String name, byte[] data)
	{
		throw new NotImplementedException("SetSymCustomAttribute");
	}

	[TODO]
	public override String ToString()
	{
		throw new NotImplementedException("ToString");
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
	public Type ReturnType 
	{
		get
		{
			return typeof(Void);
		}
	}

	[TODO]
	public String Signature 
	{
		get
		{
			throw new NotImplementedException("Signature");
		}
	}

	// TODO

}; // class ConstructorBuilder

#endif // !ECMA_COMPAT

}; // namespace System.Reflection.Emit
