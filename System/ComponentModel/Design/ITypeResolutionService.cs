/*
 * ITypeResolutionService.cs - Implementation of the
 *		"System.ComponentModel.Design.ITypeResolutionService" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace System.ComponentModel.Design
{

#if !ECMA_COMPAT

using System.Reflection;

public interface ITypeResolutionService
{
	// Get an assembly by name.
	Assembly GetAssembly(AssemblyName name);
	Assembly GetAssembly(AssemblyName name, bool throwOnError);

	// Get the path of an assembly.
	String GetPathOfAssembly(AssemblyName name);

	// Get a type with a specific name.
	Type GetType(String name);
	Type GetType(String name, bool throwOnError);
	Type GetType(String name, bool throwOnError, bool ignoreCase);

	// Add a reference to a particular assembly.
	void ReferenceAssembly(AssemblyName name);

}; // interface ITypeResolutionService

#endif // !ECMA_COMPAT

}; // namespace System.ComponentModel.Design
