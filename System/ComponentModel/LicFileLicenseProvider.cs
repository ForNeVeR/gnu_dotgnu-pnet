/*
 * LicFileLicenseProvider.cs - Implementation of the
 *		"System.ComponentModel.ComponentModel.LicFileLicenseProvider" class.
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

namespace System.ComponentModel
{

#if CONFIG_COMPONENT_MODEL

using System;

public class LicFileLicenseProvider : LicenseProvider
{
	// Constructor.
	public LicFileLicenseProvider() {}

	// Get the license for a type.
	[TODO]
	public override License GetLicense
				(LicenseContext context, Type type, object instance,
				 bool allowExceptions)
			{
				// TODO
				return null;
			}

	// Get the key for a type.
	protected virtual string GetKey(Type type)
			{
				// Don't translate this - needed for compatibility.
				return type.FullName + " is a licensed component.";
			}

	// Determine if a key is valid for a particular type.
	protected virtual bool IsKeyValid(String key, Type type)
			{
				if(key != null)
				{
					return key.StartsWith(GetKey(type));
				}
				else
				{
					return false;
				}
			}

}; // class LicFileLicenseProvider

#endif // CONFIG_COMPONENT_MODEL

}; // namespace System.ComponentModel
