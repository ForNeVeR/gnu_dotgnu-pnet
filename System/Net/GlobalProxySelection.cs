/*
 * GlobalProxySelection.cs - Implementation of the "System.Net.GlobalProxySelection" class.
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
using System.Net;

public class GlobalProxySelection 
{

	private static IWebProxy globalProxy = new EmptyProxy();

	// Constructors

#if !ECMA_COMPAT
	public
#else
	protected 
#endif
	GlobalProxySelection()
	{

	}

	// Methods

	public static IWebProxy GetEmptyWebProxy()
	{
		return new EmptyProxy();
	}

	// Properties

	// TODO: Figure out security permissions (see spec)
	public static IWebProxy Select
	{
		get
		{
			return globalProxy;
		}
		
		set
		{
			if(value == null)
			{
				globalProxy = GetEmptyWebProxy();
			}
			else
			{
				globalProxy= value;
			}
		}
	}

	// Internal Classes

	// See IWebProxy.cs
	internal class EmptyProxy : IWebProxy
	{

		ICredentials credentials = null;

		internal EmptyProxy()	{}

		public Uri GetProxy(Uri destination)
		{
			return destination;
		}

		public bool IsBypassed(Uri host)
		{
			return true;
		}
		
		public ICredentials Credentials
		{
			get
			{
				return credentials;
			}
			set
			{
				credentials = value;
			}
		}
	}
	
} // class GlobalProxySelection

} // namespace System.net
