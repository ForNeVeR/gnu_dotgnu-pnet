/*
 * ConfigurationSettings.cs - Implementation of the
 *		"System.Configuration.ConfigurationSettings" interface.
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
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

namespace System.Configuration
{

#if !ECMA_COMPAT

using System;
using System.IO;
using System.Collections;
using System.Collections.Specialized;
#if SECOND_PASS
using System.Xml;
#endif
using Platform;

public sealed class ConfigurationSettings
{
	// Internal state.
	private static IConfigurationSystem configSystem;
	private static Exception configError;

	// Constructor - cannot be created by external entities.
	private ConfigurationSettings() {}

	// Get a configuration object for a specific section.
	public static Object GetConfig(String sectionName)
			{
				// Make sure that the configuration system is initialized.
				IConfigurationSystem system;
				lock(typeof(ConfigurationSettings))
				{
					if(configSystem != null)
					{
						system = configSystem;
					}
					else if(configError != null)
					{
						throw configError;
					}
					else
					{
						configSystem = new BuiltinConfigurationSystem();
						try
						{
							configSystem.Init();
						}
						catch(Exception e)
						{
							configError = e;
							throw;
						}
					}
				}

				// Look up the specified configuration item.
				return system.GetConfig(sectionName);
			}

	// Get the application settings.
	public static NameValueCollection AppSettings
			{
				get
				{
					ReadOnlyNameValueCollection settings;
					settings = (ReadOnlyNameValueCollection)
							(GetConfig("appSettings"));
					if(settings == null)
					{
						settings = new ReadOnlyNameValueCollection();
						settings.MakeReadOnly();
					}
					return settings;
				}
			}

	// The builtin configuration system handler.
	private class BuiltinConfigurationSystem : IConfigurationSystem
	{
		// Internal state.
		private bool initialized;

		// Constructor.
		public BuiltinConfigurationSystem()
				{
					initialized = false;
				}

		// Check for a machine configuration file's existence in a directory.
		private static String CheckForMachineConfig(String dir)
				{
					// Bail out if the directory was not specified.
					if(dir == null || dir.Length == 0)
					{
						return null;
					}

					// Build the full pathname and check for its existence.
					String pathname = Path.Combine(dir, "machine.config");
					if(!File.Exists(pathname))
					{
						return null;
					}
					return pathname;
				}

		// Load a configuration file and merge it with the current settings.
		private void Load(String filename)
				{
					// TODO
				}

		// Initialize the configuration system.
		public void Init()
				{
					// Bail out if already initialized.
					if(initialized)
					{
						return;
					}
					try
					{
						// Find the machine configuration file.
						String machineConfigFile;
						machineConfigFile = CheckForMachineConfig
							(InfoMethods.GetUserStorageDir());
						if(machineConfigFile == null)
						{
							machineConfigFile = CheckForMachineConfig
								(InfoMethods.GetGlobalConfigDir());
						}

						// Find the application's configuration file.
						String appConfigFile =
							AppDomain.CurrentDomain.SetupInformation
								.ConfigurationFile;

						// Load the configuration files.
						if(machineConfigFile != null)
						{
							Load(machineConfigFile);
						}
						if(appConfigFile != null && File.Exists(appConfigFile))
						{
							Load(appConfigFile);
						}
					}
					finally
					{
						// The system has been initialized.
						initialized = true;
					}
				}

		// Get the object for a specific configuration key.
		public Object GetConfig(String configKey)
				{
					// TODO
					return null;
				}

	}; // class BuiltinConfigurationSystem

}; // class ConfigurationSettings

#endif // !ECMA_COMPAT

}; // namespace System.Configuration
