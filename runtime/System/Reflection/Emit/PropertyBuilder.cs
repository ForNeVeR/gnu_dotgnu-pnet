/*
 * PropertyBuilder.cs - Implementation of the
 *		"System.Reflection.Emit.PropertyBuilder" class.
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
public sealed class PropertyBuilder : PropertyInfo
{
	[TODO]
	public void AddOtherMethod(MethodBuilder mdBuilder)
	{
		 throw new NotImplementedException("AddOtherMethod");
	}

	[TODO]
	public override MethodInfo[] GetAccessors(bool nonPublic)
	{
		 throw new NotImplementedException("GetAccessors");
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
	public override MethodInfo GetGetMethod(bool nonPublic)
	{
		 throw new NotImplementedException("GetGetMethod");
	}

	[TODO]
	public override ParameterInfo[] GetIndexParameters()
	{
		 throw new NotImplementedException("GetIndexParameters");
	}

	[TODO]
	public override MethodInfo GetSetMethod(bool nonPublic)
	{
		 throw new NotImplementedException("GetSetMethod");
	}

	[TODO]
	public override Object GetValue(Object obj, Object[] index)
	{
		 throw new NotImplementedException("GetValue");
	}

	[TODO]
	public override Object GetValue(Object obj, BindingFlags invokeAttr, 
									Binder binder, Object[] index, 
									CultureInfo culture)
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
	public void SetGetMethod(MethodBuilder mdBuilder)
	{
		 throw new NotImplementedException("SetGetMethod");
	}

	[TODO]
	public void SetSetMethod(MethodBuilder mdBuilder)
	{
		 throw new NotImplementedException("SetSetMethod");
	}

	[TODO]
	public override void SetValue(Object obj, Object value, Object[] index)
	{
		 throw new NotImplementedException("SetValue");
	}

	[TODO]
	public override void SetValue(Object obj, Object value, 
								  BindingFlags invokeAttr, 
								  Binder binder, Object[] index, 
								  CultureInfo culture)
	{
		 throw new NotImplementedException("SetValue");
	}

	[TODO]
	public override PropertyAttributes Attributes 
	{
		get
		{
			throw new NotImplementedException("Attributes");
		}
	}

	[TODO]
	public override bool CanRead 
	{
		get
		{
			throw new NotImplementedException("CanRead");
		}
	}

	[TODO]
	public override bool CanWrite 
	{
		get
		{
			throw new NotImplementedException("CanWrite");
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
	public override String Name 
	{
		get
		{
			throw new NotImplementedException("Name");
		}
	}

	[TODO]
	public PropertyToken PropertyToken 
	{
		get
		{
			throw new NotImplementedException("PropertyToken");
		}
	}

	[TODO]
	public override Type PropertyType 
	{
		get
		{
			throw new NotImplementedException("PropertyType");
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

}; // class PropertyBuilder

#endif // !ECMA_COMPAT

}; // namespace System.Reflection.Emit
