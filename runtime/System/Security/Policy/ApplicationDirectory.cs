/*
 * ApplicationDirectory.cs - Implementation of the
 *		"System.Security.Policy.ApplicationDirectory" class.
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

namespace System.Security.Policy
{

#if !ECMA_COMPAT

[Serializable]
public sealed class ApplicationDirectory
{
	// Internal state.
	private String name;

	// Constructor.
	public ApplicationDirectory(String name)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				this.name = name;
			}

	// Get the directory path from this object.
	public String Directory
			{
				get
				{
					return name;
				}
			}

	// Create a copy of this object.
	public ApplicationDirectory Copy()
			{
				return new ApplicationDirectory(name);
			}

	// Determine if two objects are equal.
	public override bool Equals(Object obj)
			{
				ApplicationDirectory other = (obj as ApplicationDirectory);
				if(other != null)
				{
					return (other.name == name);
				}
				else
				{
					return false;
				}
			}

	// Get the hash code for this instance.
	public override int GetHashCode()
			{
				return name.GetHashCode();
			}

	// Convert this object into a string.
	public override String ToString()
			{
				return "<System.Security.Policy.ApplicationDirectory>\n" +
					   "   <Directory>" +
					   SecurityElement.Escape(name) +
					   "</Directory>\n" +
					   "</System.Security.Policy.ApplicationDirectory>\n";
			}

}; // class ApplicationDirectory

#endif // !ECMA_COMPAT

}; // namespace System.Security.Policy
