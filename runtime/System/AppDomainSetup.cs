/*
 * AppDomainSetup.cs - Implementation of the "System.AppDomainSetup" class.
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

namespace System
{

#if !ECMA_COMPAT

using System;
using System.IO;
using System.Runtime.InteropServices;

[Serializable]
#if CONFIG_COM_INTEROP
[ClassInterface(ClassInterfaceType.None)]
#endif
public sealed class AppDomainSetup : IAppDomainSetup
{
	// Internal state.
	private String applicationBase;
	private String applicationName;
	private String cachePath;
	private String configurationFile;
	private String dynamicBase;
	private String licenseFile;
	private String privateBinPath;
	private String privateBinPathProbe;
	private String shadowCopyDirectories;
	private String shadowCopyFiles;
	private bool disallowPublisherPolicy;
	private bool disallowBindingRedirects;
	private bool disallowCodeDownload;
	private LoaderOptimization loaderOptimization;

	// Constructor.
	public AppDomainSetup()
			{
				// Nothing to do here.
			}

	// Implement the IAppDomainSetup interface.
	public String ApplicationBase
			{
				get
				{
					return applicationBase;
				}
				set
				{
					applicationBase = value;
				}
			}
	public String ApplicationName
			{
				get
				{
					return applicationName;
				}
				set
				{
					applicationName = value;
				}
			}
	public String CachePath
			{
				get
				{
					return cachePath;
				}
				set
				{
					cachePath = value;
				}
			}
	public String ConfigurationFile
			{
				get
				{
					return configurationFile;
				}
				set
				{
					configurationFile = value;
				}
			}
	public String DynamicBase
			{
				get
				{
					if(dynamicBase == null)
					{
						return null;
					}

					if(Path.IsPathRooted(dynamicBase))
					{
						// absolute path
						return dynamicBase;
					}

					return Path.Combine(ApplicationBase, dynamicBase);
				}
				set
				{
					// make sure we don't have the same path for every run
					dynamicBase = Path.Combine(value, 
								applicationName.GetHashCode().ToString("X"));
				}
			}
	public String LicenseFile
			{
				get
				{
					return licenseFile;
				}
				set
				{
					licenseFile = value;
				}
			}
	public String PrivateBinPath
			{
				get
				{
					return privateBinPath;
				}
				set
				{
					privateBinPath = value;
				}
			}
	public String PrivateBinPathProbe
			{
				get
				{
					return privateBinPathProbe;
				}
				set
				{
					privateBinPathProbe = value;
				}
			}
	public String ShadowCopyDirectories
			{
				get
				{
					return shadowCopyDirectories;
				}
				set
				{
					shadowCopyDirectories = value;
				}
			}
	public String ShadowCopyFiles
			{
				get
				{
					return shadowCopyFiles;
				}
				set
				{
					shadowCopyFiles = value;
				}
			}

	// Other properties.
	public bool DisallowBindingRedirects
			{
				get
				{
					return disallowBindingRedirects;
				}
				set
				{
					disallowBindingRedirects = value;
				}
			}
	public bool DisallowCodeDownload
			{
				get
				{
					return disallowCodeDownload;
				}
				set
				{
					disallowCodeDownload = value;
				}
			}
	public bool DisallowPublisherPolicy
			{
				get
				{
					return disallowPublisherPolicy;
				}
				set
				{
					disallowPublisherPolicy = value;
				}
			}
	public LoaderOptimization LoaderOptimization
			{
				get
				{
					return loaderOptimization;
				}
				set
				{
					loaderOptimization = value;
				}
			}

}; // class AppDomainSetup

#endif // !ECMA_COMPAT

}; // namespace System
