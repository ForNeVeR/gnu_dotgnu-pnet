/*
 * Assembly.cs - Implementation of the "System.Reflection.Assembly" class.
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
using System.Runtime.CompilerServices;

public class Assembly : IClrProgramItem
{

	// Built-in handle for the assembly.  This must be the first field.
	private IntPtr privateData;

	// Private constructor.  Normally called by the runtime engine only.
	internal Assembly() {}

	// Implement the IClrProgramItem interface.
	public IntPtr ClrHandle
			{
				get
				{
					return privateData;
				}
			}

	// Create an instance of a specific type within this assembly.
	public Object CreateInstance(String typeName)
			{
				return CreateInstance(typeName, false,
									  BindingFlags.Public |
									  BindingFlags.Instance,
									  null, null, null, null);
			}
	public Object CreateInstance(String typeName, bool ignoreCase)
			{
				return CreateInstance(typeName, ignoreCase,
									  BindingFlags.Public |
									  BindingFlags.Instance,
									  null, null, null, null);
			}
	public Object CreateInstance(String typeName, bool ignoreCase,
								 BindingFlags bindingAttr, Binder binder,
								 Object[] args, CultureInfo culture,
								 Object[] activationAttributes)
			{
				Type type = GetType(typeName, false, ignoreCase);
				if(type == null)
				{
					return null;
				}
				return Activator.CreateInstance(type, bindingAttr,
												binder, args, culture,
												activationAttributes);
			}

	// Create a qualified type name.
	public static String CreateQualifiedName(String assemblyName,
											 String typeName)
			{
				return typeName + ", " + assemblyName;
			}

	// Get the assembly that called the method that called this method.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Assembly GetCallingAssembly();

	// Get the custom attributes associated with this assembly.
	public virtual Object[] GetCustomAttributes(bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, inherit);
			}
	public virtual Object[] GetCustomAttributes(Type type, bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, type, inherit);
			}

	// Determine if custom attributes are associated with this assembly.
	public virtual bool IsDefined(Type type, bool inherit)
			{
				return ClrHelpers.IsDefined(this, type, inherit);
			}

	// Get the assembly that contained the program entry point.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Assembly GetEntryAssembly();

	// Get the assembly that called this method.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Assembly GetExecutingAssembly();

	// Get an array of all exported types in an assembly.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual Type[] GetExportedTypes();

#if false
	// Get a file stream for a particular public manifest resource.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual FileStream GetFile(String name);

	// Get file streams for all public manifest resources.
	public virtual FileStream[] GetFiles(bool getResourceModules)
			{
				// TODO
				return null;
			}
	public virtual FileStream[] GetFiles()
			{
				return GetFiles(false);
			}
#endif

	// Get a particular type from this assembly.
	public virtual Type GetType(String typeName)
			{
				return GetType(typeName, false, false);
			}
	public virtual Type GetType(String typeName, bool throwOnError)
			{
				return GetType(typeName, throwOnError, false);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public Type GetType(String typeName, bool throwOnError,
							   bool ignoreCase);

	// Get an array of all types in an assembly.
	public virtual Type[] GetTypes()
			{
				// TODO
				return null;
			}

	// Load a particular assembly.
	public static Assembly Load(String assemblyString)
			{
				// TODO
				return null;
			}

	// Convert this assembly into a string.
	public override String ToString()
			{
				String name = FullName;
				if(name != null)
				{
					return name;
				}
				else
				{
					return base.ToString();
				}
			}

	// Get the assembly that a particular type resides in.
	public static Assembly GetAssembly(Type type)
			{
				if(type == null)
				{
					throw new ArgumentNullException("type");
				}
				Module module = type.Module;
				if(module == null)
				{
					return null;
				}
				else
				{
					return module.Assembly;
				}
			}

	// Get the full name associated with this assembly.
	public virtual String FullName
			{
				get
				{
					return GetFullName();
				}
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private String GetFullName();

}; // class Assembly

}; // namespace System.Reflection
