/*
 * AssemblyBuilder.cs - Implementation of the
 *		"System.Reflection.Emit.AssemblyBuilder" class.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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
using System.Security;
using System.Security.Policy;
using System.Runtime.CompilerServices;
using System.Threading;

public sealed class AssemblyBuilder : Assembly
{
	// Internal state.
	private AssemblyName name;
	private MethodInfo entryPoint;
	private bool isSynchronized;

	// Constructor.  Called from AppDomain.DefineDynamicAssembly.
	[TODO]
	internal AssemblyBuilder(AssemblyName name, AssemblyBuilderAccess access,
							 String directory, Evidence evidence,
							 PermissionSet requiredPermissions,
							 PermissionSet optionalPermissions,
							 PermissionSet refusedPermissions,
							 bool isSynchronized)
			{
				this.name = (AssemblyName)(name.Clone());
				entryPoint = null;
				this.isSynchronized = isSynchronized;
			}

	// Start a synchronized operation on this assembly builder.
	internal void StartSync()
			{
				if(isSynchronized)
				{
					Monitor.Enter(this);
				}
			}

	// End a synchronized operation on this assembly builder.
	internal void EndSync()
			{
				if(isSynchronized)
				{
					Monitor.Exit(this);
				}
			}

	// Get the code base for this assembly builder.
	public override String CodeBase
			{
				get
				{
					throw new NotSupportedException(_("NotSupp_Builder"));
				}
			}

	// Get the location of the assembly manifest file.
	public override String Location
			{
				get
				{
					throw new NotSupportedException(_("NotSupp_Builder"));
				}
			}

	// Get the entry point for this assembly builder.
	public override MethodInfo EntryPoint
			{
				get
				{
					return entryPoint;
				}
			}

	// Set the entry point for this assembly builder.
	public void SetEntryPoint(MethodInfo entryMethod)
			{
				SetEntryPoint(entryMethod, PEFileKinds.ConsoleApplication);
			}
	public void SetEntryPoint(MethodInfo entryMethod, PEFileKinds fileKind)
			{
				if(entryMethod == null)
				{
					throw new ArgumentNullException("entryMethod");
				}
				if(entryMethod.DeclaringType.Assembly != this)
				{
					throw new InvalidOperationException
						(_("Invalid_EntryNotInAssembly"));
				}
			/* TODO
				ClrSetEntryPoint(((MethodBuilder)entryMethod).ClrHandle,
								 fileKind);
			*/
				entryPoint = entryMethod;
			}

	// Internal version of "SetEntryPoint".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private void ClrSetEntryPoint(IntPtr clrMethod,
										 PEFileKinds fileKind);

}; // class AssemblyBuilder

#endif // !ECMA_COMPAT

}; // namespace System.Reflection.Emit
