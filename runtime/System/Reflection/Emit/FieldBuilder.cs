/*
 * FieldBuilder.cs - Implementation of the
 *		"System.Reflection.Emit.FieldBuilder" class.
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
public sealed class FieldBuilder : FieldInfo
{
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
	public FieldToken GetToken()
	{
		throw new NotImplementedException("GetToken");
	}

	[TODO]
	public override Object GetValue(Object obj)
	{
		throw new NotImplementedException("GetValue");
	}

	[TODO]
	public override bool IsDefined(Type attributeType, bool inherit)
	{
		throw new NotImplementedException("IsDefined");
	}

	[TODO]
	public void SetConstant(Object defaultValue)
	{
		throw new NotImplementedException("SetConstant");
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
	public void SetMarshal(UnmanagedMarshal unmanagedMarshal)
	{
		throw new NotImplementedException("SetMarshal");
	}

	[TODO]
	public void SetOffset(int iOffset)
	{
		throw new NotImplementedException("SetOffset");
	}

	[TODO]
	public override void SetValue(Object obj, Object val, 
								  BindingFlags invokeAttr, 
								  Binder binder, 
								  CultureInfo culture)
	{
		throw new NotImplementedException("SetValue");
	}

	[TODO]
	public override FieldAttributes Attributes 
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
	public override RuntimeFieldHandle FieldHandle 
	{
		get
		{
			throw new NotImplementedException("FieldHandle");
		}
	}

	[TODO]
	public override Type FieldType 
	{
		get
		{
			throw new NotImplementedException("FieldType");
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

	// TODO

}; // class FieldBuilder

#endif // !ECMA_COMPAT

}; // namespace System.Reflection.Emit
