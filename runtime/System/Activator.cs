/*
 * Activator.cs - Implementation of the "System.Activator" class.
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

namespace System
{

using System.Reflection;
using System.Runtime.Remoting;
using System.Globalization;
using System.Security.Policy;

#if ECMA_COMPAT
internal
#else
public
#endif
sealed class Activator
{
	// Create a COM object instance.
	public static ObjectHandle CreateComInstanceFrom
			(String assemblyName, String typeName)
	{
		// Validate the arguments.
		if(typeName == null)
		{
			throw new ArgumentNullException("typeName");
		}

		// COM is not yet supported.
		throw new TypeLoadException
				(String.Format
					(_("Exception_ComTypeLoad"),
	 			     (assemblyName == null ? "current" : assemblyName),
					 typeName));
	}

	// Create an object instance from a type using the default constructor.
	public static Object CreateInstance(Type type)
	{
		return CreateInstance(type,
							  BindingFlags.CreateInstance |
							  	BindingFlags.Public |
								BindingFlags.NonPublic |
								BindingFlags.Instance, null,
							  null, null, null);
	}

	// Create an object instance from a type and a list of arguments.
	public static Object CreateInstance(Type type, Object[] args)
	{
		return CreateInstance(type,
							  BindingFlags.CreateInstance |
							  	BindingFlags.Public |
								BindingFlags.NonPublic |
								BindingFlags.Instance, null,
							  args, null, null);
	}

	// Create an object instance from a type, a list of arguments,
	// and a list of activation attributes.
	public static Object CreateInstance(Type type, Object[] args,
										Object[] activationAttributes)
	{
		return CreateInstance(type,
							  BindingFlags.CreateInstance |
							  	BindingFlags.Public |
								BindingFlags.NonPublic |
								BindingFlags.Instance, null,
							  args, null, activationAttributes);
	}

	// Create an object instance from a type, a list of arguments,
	// and binding information.
	public static Object CreateInstance(Type type, BindingFlags bindingAttr,
									    Binder binder, Object[] args,
										CultureInfo culture)
	{
		return CreateInstance(type, bindingAttr, binder, args, culture, null);
	}

	// Create an object instance from a type, a list of arguments,
	// binding information, and a list of activation attributes.
	public static Object CreateInstance(Type type, BindingFlags bindingAttr,
									    Binder binder, Object[] args,
										CultureInfo culture,
										Object[] activationAttributes)
	{
		// TODO
		return null;
	}

	// Create an object instance from a type in another assembly.
	public static ObjectHandle CreateInstance
				(String assemblyName, String typeName)
	{
		return CreateInstance(assemblyName, typeName, false,
							  BindingFlags.CreateInstance |
							  	BindingFlags.Public |
								BindingFlags.NonPublic |
								BindingFlags.Instance, null,
							  null, null, null, null);
	}

	// Create an object instance from a type in another assembly,
	// and a list of activation attributes.
	public static ObjectHandle CreateInstance
				(String assemblyName, String typeName,
				 Object[] activationAttributes)
	{
		return CreateInstance(assemblyName, typeName, false,
							  BindingFlags.CreateInstance |
							  	BindingFlags.Public |
								BindingFlags.NonPublic |
								BindingFlags.Instance, null,
							  null, null, activationAttributes, null);
	}

	// Create an object instance from a type in another assembly,
	// a list of arguments, binding information, and a list of
	// activation attributes.
	public static ObjectHandle CreateInstance
				(String assemblyName, String typeName, bool ignoreCase,
				 BindingFlags bindingAttr, Binder binder,
				 Object[] args, CultureInfo culture,
				 Object[] activationAttributes,
				 Evidence securityInfo)
	{
		// TODO
		return null;
	}

	// Create an object instance from a type in another assembly,
	// using "LoadFrom" instead of "Load" to load the assembly.
	public static ObjectHandle CreateInstanceFrom
				(String assemblyName, String typeName)
	{
		return CreateInstanceFrom(assemblyName, typeName, false,
							      BindingFlags.CreateInstance |
							  	    BindingFlags.Public |
								    BindingFlags.NonPublic |
								    BindingFlags.Instance, null,
							      null, null, null, null);
	}

	// Create an object instance from a type in another assembly,
	// using "LoadFrom" instead of "Load" to load the assembly.
	// Activation attributes are also supplied.
	public static ObjectHandle CreateInstanceFrom
				(String assemblyName, String typeName,
				 Object[] activationAttributes)
	{
		return CreateInstanceFrom(assemblyName, typeName, false,
							      BindingFlags.CreateInstance |
							  	    BindingFlags.Public |
								    BindingFlags.NonPublic |
								    BindingFlags.Instance, null,
							      null, null, activationAttributes, null);
	}

	// Create an object instance from a type in another assembly,
	// using "LoadFrom" instead of "Load" to load the assembly.
	// All relevant information is supplied.
	public static ObjectHandle CreateInstanceFrom
				(String assemblyName, String typeName, bool ignoreCase,
				 BindingFlags bindingAttr, Binder binder,
				 Object[] args, CultureInfo culture,
				 Object[] activationAttributes,
				 Evidence securityInfo)
	{
		// TODO
		return null;
	}

}; // class Activator

}; // namespace System
