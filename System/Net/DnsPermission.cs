/*
 * DnsPermission.cs - Implementation of the "System.Net.DnsPermission" class.
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
 *
 * Contributed by Jason Lee <jason.lee@mac.com>
 * 
 * This program is free software, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace System.Net
{

using System;
using System.Security;
using System.Security.Permissions;

public class DnsPermission : CodeAccessPermission
{

	bool restrictedState = true;

	// Default constructor with option of setting the restricted state
	// to either unrestricted (1) or fully restricted (0)
	public DnsPermission (PermissionState state)
	{
		if(state == PermissionState.Unrestricted)
		{
			restrictedState = false;
		}
		else
		{
			if(state != PermissionState.None)
			{
				throw new ArgumentException("Not a valid state value");
			}
		}
		
	}

	// Methods

	public override IPermission Copy()
	{
		return this;
	}

	[TODO]
	//Once System XML is fully implemented
	public override void FromXml(SecurityElement securityElement)
	{
	}


	public override IPermission Intersect(IPermission target)
	{
		if(target == null)
		{
			throw new ArgumentNullException("IPermission");
		}

		DnsPermission newTarget = target as DnsPermission;
		
		if(newTarget == null)
		{
			throw new ArgumentException();
		}

		if(!restrictedState)
		{
			return newTarget;
		}

		if(!newTarget.restrictedState)
		{
			return this;
		}

		return null;
		
	}

	public override bool IsSubsetOf(IPermission target)
	{
		if(target == null)
		{
			return false;
		}

		DnsPermission newTarget = target as DnsPermission;
		
		if(newTarget == null)
		{
			throw new ArgumentException();
		}
	
		if(!restrictedState)
		{
			if(!newTarget.restrictedState)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		
		return true;
		
	}

	[TODO]
	// Once System XML is fully implemented
	public override SecurityElement ToXml()
	{
		return null;
	}

	public override IPermission Union(IPermission target)
	{
		if(target == null)
		{
			throw new ArgumentNullException("IPermission");
		}

		DnsPermission newTarget = target as DnsPermission;
		if(newTarget == null)
		{
			throw new ArgumentException();
		}
	
		if(IsSubsetOf(newTarget))
		{
			if((!newTarget.restrictedState) || !restrictedState)
			{
				return newTarget;
			}		
		}
		
		return null;
	}

} // class DnsPermission

} // namespace System.Net
