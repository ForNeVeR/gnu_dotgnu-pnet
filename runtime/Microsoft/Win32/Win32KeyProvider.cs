/*
 * Win32KeyProvider.cs - Implementation of the
 *			"Microsoft.Win32.Win32KeyProvider" class.
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

namespace Microsoft.Win32
{

#if !ECMA_COMPAT

using System;

// This class implements registry functionality for Win32 systems.

internal sealed class Win32KeyProvider : IRegistryKeyProvider
{
	// Internal state.
	private String name;

	// Constructor.
	public Win32KeyProvider(String name)
			{
				this.name = name;
			}

	// Determine if we should use the Win32 registry.
	public static bool IsWin32()
			{
				// TODO
				return false;
			}

	// Close a reference to this key and flush any modifications to disk.
	public void Close(bool writable)
			{
				// TODO
			}

	// Create a subkey underneath this particular registry key.
	public IRegistryKeyProvider CreateSubKey(String subkey)
			{
				// TODO
				return null;
			}

	// Delete this key entry.
	public void Delete()
			{
				// TODO
			}

	// Delete this key entry and all of its descendents.
	public void DeleteTree()
			{
				// TODO
			}

	// Delete a particular value underneath this registry key.
	// Returns false if the value is missing.
	public bool DeleteValue(String name)
			{
				// TODO
				return false;
			}

	// Flush all modifications to this registry key.
	public void Flush()
			{
				// TODO
			}

	// Get the names of all subkeys underneath this registry key.
	public String[] GetSubKeyNames()
			{
				// TODO
				return null;
			}

	// Get a value from underneath this registry key.
	public Object GetValue(String name, Object defaultValue)
			{
				// TODO
				return defaultValue;
			}

	// Get the names of all values underneath this registry key.
	public String[] GetValueNames()
			{
				// TODO
				return null;
			}

	// Open a subkey.
	public IRegistryKeyProvider OpenSubKey(String name, bool writable)
			{
				// TODO
				return null;
			}

	// Set a value under this registry key.
	public void SetValue(String name, Object value)
			{
				// TODO
			}

	// Get the name of this registry key provider.
	public String Name
			{
				get
				{
					return name;
				}
			}

	// Get the number of subkeys underneath this key.
	public int SubKeyCount
			{
				get
				{
					return 0;
				}
			}

	// Get the number of values that are associated with this key.
	public int ValueCount
			{
				get
				{
					return 0;
				}
			}

}; // class Win32KeyProvider

#endif // !ECMA_COMPAT

}; // namespace Microsoft.Win32
