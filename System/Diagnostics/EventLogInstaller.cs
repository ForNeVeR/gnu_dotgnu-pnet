/*
 * EventLogInstaller.cs - Implementation of the
 *			"System.Diagnostics.EventLogInstaller" class.
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

namespace System.Diagnostics
{

#if false	// TODO - need System.Configuration.Install namespace

#if !ECMA_COMPAT

using System.Collections;
using System.ComponentModel;
using System.Configuration.Install;

public class EventLogInstaller : ComponentInstaller
{
	// Internal state.
	private String logName;
	private String source;
	private UninstallAction uninstallAction;

	// Constructor.
	public EventLogInstaller()
			{
				logName = "";
				source = "";
				uninstallAction = UninstallAction.Remove;
			}

	// Get or set installer properties.
	public String Log
			{
				get
				{
					return logName;
				}
				set
				{
					logName = value;
				}
			}
	public String Source
			{
				get
				{
					return source;
				}
				set
				{
					source = value;
				}
			}
	public UninstallAction UninstallAction
			{
				get
				{
					return uninstallAction;
				}
				set
				{
					uninstallAction = value;
				}
			}

	// Copy configuration information from a component.
	public override CopyFromComponent(IComponent component)
			{
				// TODO
			}

	// Install the event log.
	public override void Install(IDictionary stateSaver)
			{
				// TODO
			}

	// Determine if two installers are equivalent.
	public override bool IsEquivalentInstaller
				(ComponentInstaller otherInstaller)
			{
				EventLogInstaller other = (otherInstaller as EventLogInstaller);
				if(other != null)
				{
					return (other.Log == Log && other.Source == Source);
				}
				else
				{
					return false;
				}
			}

	// Roll back the installation process.
	public override void Rollback(IDictionary savedState)
			{
				// TODO
			}

	// Uninstall this event log.
	public override void Uninstall(IDictionary savedState)
			{
				// TODO
			}

}; // class EventLogInstaller

#endif // !ECMA_COMPAT

#endif // false

}; // namespace System.Diagnostics
