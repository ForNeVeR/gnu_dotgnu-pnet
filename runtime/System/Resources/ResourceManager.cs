/*
 * ResourceManager.cs - Implementation of the
 *		"System.Resources.ResourceManager" class.
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
	private String resourceDir;

	// Constructors.
	protected ResourceManager()
			{
				BaseNameField = null;
				MainAssembly = null;
				ResourceSets = new Hashtable();
				ignoreCase = false;
				resourceSetType = null;
				resourceDir = null;
			}
	public ResourceManager(Type resourceSource)
			{
				if(resourceSource == null)
				{
					throw new ArgumentNullException("resourceSource");
				}
				BaseNameField = resourceSource.FullName;
				MainAssembly = resourceSource.Assembly;
				ResourceSets = new Hashtable();
				ignoreCase = false;
				resourceSetType = typeof(BuiltinResourceSet);
				resourceDir = null;
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
				ResourceSets = new Hashtable();
				ignoreCase = false;
				resourceSetType = typeof(BuiltinResourceSet);
				resourceDir = null;
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
				ResourceSets = new Hashtable();
				ignoreCase = false;
				resourceDir = null;
				if(usingResourceSet != null)
				{
					if(!usingResourceSet.IsSubclassOf(typeof(ResourceSet)))
					{
						throw new ArgumentException
							(_("Arg_MustBeResourceSet"),
							 "usingResourceSet");
					}
					resourceSetType = usingResourceSet;
				}
				else
				{
					resourceSetType = typeof(BuiltinResourceSet);
				}
			}
	private ResourceManager(String baseName, String resourceDir,
							Type usingResourceSet)
			{
				BaseNameField = baseName;
				MainAssembly = null;
				ResourceSets = new Hashtable();
				ignoreCase = false;
				resourceSetType = usingResourceSet;
				this.resourceDir = resourceDir;
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
						(_("Arg_EndsWithResources"), "baseName");
				}
				else if(resourceDir == null)
				{
					throw new ArgumentNullException("baseName");
				}
				if(usingResourceSet != null)
				{
					if(!usingResourceSet.IsSubclassOf(typeof(ResourceSet)))
					{
						throw new ArgumentException
							(_("Arg_MustBeResourceSet"),
							 "usingResourceSet");
					}
				}
				else
				{
					usingResourceSet = typeof(BuiltinResourceSet);
				}
				return new ResourceManager(baseName, resourceDir,
										   usingResourceSet);
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
	public virtual Object GetObject(String name)
			{
				return GetObject(name, null);
			}
	public virtual Object GetObject(String name, CultureInfo culture)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				if(culture == null)
				{
					culture = CultureInfo.CurrentCulture;
				}
				lock(this)
				{
					ResourceSet set = InternalGetResourceSet
						(culture, true, true);
					if(set != null)
					{
						return set.GetObject(name);
					}
				}
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
	public virtual String GetString(String name)
			{
				return GetString(name, null);
			}
	public virtual String GetString(String name, CultureInfo culture)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				if(culture == null)
				{
					culture = CultureInfo.CurrentCulture;
				}
				lock(this)
				{
					ResourceSet set = InternalGetResourceSet
						(culture, true, true);
					if(set != null)
					{
						return set.GetString(name);
					}
				}
				return name;
			}

	// Release all cached resources.
	[TODO]
	public virtual void ReleaseAllResources()
			{
				// TODO
			}

	// Get the neutral culture to use, based on an assembly's attributes.
	[TODO]
	protected static CultureInfo GetNeutralResourcesLanguage(Assembly a)
			{
				// TODO
				return null;
			}

	// Get the satellite contract version from an assembly.
	[TODO]
	protected static Version GetSatelliteContractVersion(Assembly a)
			{
				// TODO
				return null;
			}

	// Get the name of a resource file for a particular culture.
	protected virtual String GetResourceFileName(CultureInfo culture)
			{
				if(culture == null || culture.LCID == 0x7F)
				{
					// This is the invariant culture.
					return BaseNameField + ".resources";
				}
				else
				{
					// This is a named culture.
					return BaseNameField + "." + culture.Name + ".resources";
				}
			}

	// Find a resource set for a particular culture.
	protected virtual ResourceSet InternalGetResourceSet
				(CultureInfo culture, bool createIfNotExists,
				 bool tryParents)
			{
				CultureInfo current = culture;
				ResourceSet set;
				do
				{
					// If this is the invariant culture, then stop.
					if(culture.Equals(CultureInfo.InvariantCulture))
					{
						break;
					}

					// See if we have a cached resource set for this culture.
					set = (ResourceSet)(ResourceSets[current.Name]);
					if(set != null)
					{
						return set;
					}

					// Attempt to load a resource for this culture.
					if(createIfNotExists)
					{
						set = AttemptLoad(current);
						if(set != null)
						{
							ResourceSets[current.Name] = set;
							return set;
						}
					}

					// Move up to the parent culture.
					current = current.Parent;
				}
				while(current != null && tryParents);

				// Try looking for the neutral language attribute.
				if(MainAssembly != null)
				{
					current = GetNeutralResourcesLanguage(MainAssembly);
				}
				else
				{
					current = null;
				}
				if(current != null)
				{
					// See if we have a cached resource set for this culture.
					set = (ResourceSet)(ResourceSets[current.Name]);
					if(set != null)
					{
						return set;
					}

					// Attempt to load a resource for this culture.
					if(createIfNotExists)
					{
						set = AttemptLoad(current);
						if(set != null)
						{
							ResourceSets[current.Name] = set;
							return set;
						}
					}
				}

				// Last ditch attempt: try the invariant culture.
				current = CultureInfo.InvariantCulture;
				set = (ResourceSet)(ResourceSets[current.Name]);
				if(set != null)
				{
					return set;
				}
				if(createIfNotExists)
				{
					set = AttemptLoad(current);
					if(set != null)
					{
						ResourceSets[current.Name] = set;
						return set;
					}
				}

				// We could not find an appropriate resource set.
				return null;
			}

	// Attempt to load a resource set for a particular culture.
	private ResourceSet AttemptLoad(CultureInfo culture)
			{
				Stream stream;
				if(MainAssembly != null)
				{
					// Try loading the resources from an assembly.
					stream = MainAssembly.GetManifestResourceStream
							(GetResourceFileName(culture));
					if(stream != null)
					{
						Object[] args = new Object [1];
						args[0] = stream;
						return (ResourceSet)
							(Activator.CreateInstance(resourceSetType, args));
					}
				}
				else if(resourceDir != null)
				{
					// Try loading the resources from a directory.
					try
					{
						stream = new FileStream(resourceDir +
												Path.DirectorySeparatorChar +
												GetResourceFileName(culture),
												FileMode.Open,
												FileAccess.Read);
					}
					catch(IOException)
					{
						// The file or directory probably does not exist.
						return null;
					}
					Object[] args = new Object [1];
					args[0] = stream;
					return (ResourceSet)
						(Activator.CreateInstance(resourceSetType, args));
				}
				return null;
			}

}; // class ResourceManager

}; // namespace System.Resources
