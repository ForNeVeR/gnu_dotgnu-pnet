/*
 * ResourceManager.cs - Implementation of the
 *		"System.Resources.ResourceManager" class.
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

namespace System.Resources
{

using System;
using System.IO;
using System.Reflection;
using System.Globalization;
using System.Collections;

public class ResourceManager
{
	// Current version supported by this resource manager implementation.
	public static readonly int HeaderVersionNumber = 1;

	// Magic number used in binary resource file headers.
	public static readonly int MagicNumber = unchecked((int)0xBEEFCACE);

	// Internal state.
	protected String BaseNameField;
	protected Assembly MainAssembly;
	protected Hashtable ResourceSets;
	private bool ignoreCase;
	private Type resourceSetType;

	// Constructors.
	protected ResourceManager()
			{
				BaseNameField = null;
				MainAssembly = null;
				ResourceSets = null;
				ignoreCase = false;
				resourceSetType = null;
			}
	public ResourceManager(Type resourceSource)
			{
				if(resourceSource == null)
				{
					throw new ArgumentNullException("resourceSource");
				}
				BaseNameField = resourceSource.FullName;
				MainAssembly = resourceSource.Assembly;
				ignoreCase = false;
				resourceSetType = typeof(ResourceSet);
				InitAssemblyResources();
			}
	public ResourceManager(String baseName, Assembly assembly)
			{
				if(baseName == null)
				{
					throw new ArgumentNullException("baseName");
				}
				if(assembly == null)
				{
					throw new ArgumentNullException("assembly");
				}
				BaseNameField = baseName;
				MainAssembly = assembly;
				ignoreCase = false;
				resourceSetType = typeof(ResourceSet);
				InitAssemblyResources();
			}
	public ResourceManager(String baseName, Assembly assembly,
						   Type usingResourceSet)
			{
				if(baseName == null)
				{
					throw new ArgumentNullException("baseName");
				}
				if(assembly == null)
				{
					throw new ArgumentNullException("assembly");
				}
				BaseNameField = baseName;
				MainAssembly = assembly;
				ignoreCase = false;
				if(usingResourceSet != null)
				{
					if(!usingResourceSet.IsSubclassOf(typeof(ResourceSet)))
					{
						throw new ArgumentException
							(Environment.GetResourceString
								("Arg_MustBeResourceSet"),
							 "usingResourceSet");
					}
					resourceSetType = usingResourceSet;
				}
				else
				{
					resourceSetType = typeof(ResourceSet);
				}
				InitAssemblyResources();
			}

	// Create a resource manager from a file.
	public static ResourceManager CreateFileBasedResourceManager
				(String baseName, String resourceDir, Type usingResourceSet)
			{
				if(baseName == null)
				{
					throw new ArgumentNullException("baseName");
				}
				else if(baseName.EndsWith(".resources"))
				{
					throw new ArgumentException
						(Environment.GetResourceString
							("Arg_EndsWithResources"), "baseName");
				}
				// TODO
				return null;
			}

	// Initialize the resources for the main assembly.
	private void InitAssemblyResources()
			{
				// TODO
				ResourceSets = null;
			}

	// Get the base name for this resource manager.
	public virtual String BaseName
			{
				get
				{
					return BaseNameField;
				}
			}

	// Get or set the "ignore case" property when searching for resources.
	public virtual bool IgnoreCase
			{
				get
				{
					return ignoreCase;
				}
				set
				{
					ignoreCase = value;
				}
			}

	// Get the resource set type that underlies this resource manager.
	public virtual Type ResourceSetType
			{
				get
				{
					return resourceSetType;
				}
			}

	// Get an object from this resource manager.
	public Object GetObject(String name)
			{
				return GetObject(name, null);
			}
	public Object GetObject(String name, CultureInfo culture)
			{
				// TODO
				return null;
			}

	// Get the resource set for a particular culture.
	public virtual ResourceSet GetResourceSet
				(CultureInfo culture, bool createIfNotExists,
				 bool tryParents)
			{
				if(culture == null)
				{
					throw new ArgumentNullException("culture");
				}
				lock(this)
				{
					return InternalGetResourceSet
						(culture, createIfNotExists, tryParents);
				}
			}

	// Get a string from this resource manager.
	public String GetString(String name)
			{
				return GetString(name, null);
			}
	public String GetString(String name, CultureInfo culture)
			{
				// TODO
				return name;
			}

	// Release all cached resources.
	public virtual void ReleaseAllResources()
			{
				// TODO
			}

	// Get the neutral culture to use, based on an assembly's attributes.
	protected static CultureInfo GetNeutralResourcesLanguage(Assembly a)
			{
				// TODO
				return null;
			}

	// Get the satellite contract version from an assembly.
	protected static Version GetSatelliteContractVersion(Assembly a)
			{
				// TODO
				return null;
			}

	// Get the name of a resource file for a particular culture.
	protected virtual String GetResourceFileName(CultureInfo culture)
			{
				// TODO
				return null;
			}

	// Find a resource set for a particular culture.
	protected virtual ResourceSet InternalGetResourceSet
				(CultureInfo culture, bool createIfNotExists,
				 bool tryParents)
			{
				// TODO
				return null;
			}

}; // class ResourceManager

}; // namespace System.Resources
