/*
 * ManagedInstallerClass.cs - Implementation of the
 *	    "System.Configuration.Install.ManagedInstallerClass" class.
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

namespace System.Configuration.Install
{

using System.Runtime.InteropServices;

#if !ECMA_COMPAT
[ComVisible(true)]
[Guid("42EB0342-0393-448f-84AA-D4BEB0283595")]
#endif
public class ManagedInstallerClass : IManagedInstaller
{
	// Entry point that is called by "InstallUtil".
	public static void InstallHelper(String[] args)
			{
				// TODO
			}

	// Not used in this implementation.
	int IManagedInstaller.ManagedInstall(String commandLine, int hInstall)
			{
				return 0;
			}

}; // class ManagedInstallerClass

}; // namespace System.Configuration.Install
