/*
 * ComponentResourceManager.cs - Implementation of the
 *		"System.ComponentModel.ComponentModel.ComponentResourceManager" class.
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

using System.Resources;
using System.Globalization;

public class ComponentResourceManager : ResourceManager
{
	// Constructors.
	public ComponentResourceManager() {}
	public ComponentResourceManager(Type t) : base(t) {}

	// Apply resources to a property value.
	public void ApplyResources(Object value, String objectName)
			{
				ApplyResources(value, objectName, null);
			}
	[TODO]
	public virtual void ApplyResources
				(Object value, String objectName, CultureInfo culture)
			{
				// Validate the parameters.
				if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				if(objectName == null)
				{
					throw new ArgumentNullException("objectName");
				}

				// Get the default UI culture if necessary.
				if(culture == null)
				{
					culture = CultureInfo.CurrentUICulture;
				}

				// Read the resources and apply them to the component.
				// TODO
			}

}; // class ComponentResourceManager

#endif // CONFIG_COMPONENT_MODEL

}; // namespace System.ComponentModel
