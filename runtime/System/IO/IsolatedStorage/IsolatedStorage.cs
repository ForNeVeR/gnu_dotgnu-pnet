/*
 * IsolatedStorage.cs - Implementation of the
 *		"System.IO.IsolatedStorage.IsolatedStorage" class.
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

namespace System.IO.IsolatedStorage
{

#if !ECMA_COMPAT

using System.Security;
using System.Security.Permissions;

public abstract class IsolatedStorage : MarshalByRefObject
{
	// Internal state.
	private IsolatedStorageScope scope;

	// Constructor.
	protected IsolatedStorage() {}

	// Get the current amount of space that has be used.
	[TODO]
	[CLSCompliant(false)]
	public virtual ulong CurrentSize
			{
				get
				{
					// TODO
					return 0;
				}
			}

	// Get the assembly identity for this isolated storage object.
	[TODO]
	public Object AssemblyIdentity
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get the domain identity for this isolated storage object.
	[TODO]
	public Object DomainIdentity
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get the maximum amount of space that can be used.
	[TODO]
	[CLSCompliant(false)]
	public virtual ulong MaximumSize
			{
				get
				{
					// TODO
					return 0;
				}
			}

	// Get the internal path separator.
	protected virtual char SeparatorInternal
			{
				get
				{
					return '.';
				}
			}

	// Get the external path separator.
	protected virtual char SeparatorExternal
			{
				get
				{
					return Path.DirectorySeparatorChar;
				}
			}

	// Get the scope of this storage object.
	public IsolatedStorageScope Scope
			{
				get
				{
					return scope;
				}
			}

	// Get isolated storage permission information from a permission set.
	protected abstract IsolatedStoragePermission
			GetPermission(PermissionSet ps);

	// Initialise this storage object.
	[TODO]
	protected void InitStore(IsolatedStorageScope scope,
							 Type domainEvidenceType,
							 Type assemblyEvidenceType)
			{
				// TODO
				this.scope = scope;
			}

	// Remove this isolated storage object.
	public abstract void Remove();

}; // class IsolatedStorage

#endif // !ECMA_COMPAT

}; // namespace System.IO.IsolatedStorage
