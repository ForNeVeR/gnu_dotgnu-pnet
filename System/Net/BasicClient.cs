/*
 * BasicClient.cs - Implementation of the
 *			"System.Net.BasicClient" class.
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

namespace System.Net
{

using System;
using System.Text;

internal class BasicClient : IAuthenticationModule
{
	// Authenticate a challenge from the server.
	public Authorization Authenticate
		(String challenge, WebRequest request, ICredentials credentials)
	{
		if(credentials==null || challenge==null || request==null
			|| (!challenge.ToLower().StartsWith("basic")))
		{
			return null;
		}
		return AuthenticateInternal(request, credentials);
	}

	// Pre-authenticate a request.
	public Authorization PreAuthenticate
		(WebRequest request, ICredentials credentials)
	{
		if(request==null || credentials==null)
		{
			return null;
		}
		return AuthenticateInternal(request, credentials);
	}

	private Authorization AuthenticateInternal(WebRequest request, 
			ICredentials credentials)
	{
		String user,password,domain;
		NetworkCredential netcredentials=credentials.GetCredential(
					request.RequestUri, "Basic");
		user=netcredentials.UserName;
		password=netcredentials.Password;
		domain=netcredentials.Domain;
		String response=((domain==null || domain=="") ? "" : 
					(domain + "\\"))
					+ user + ":" + password;
		byte[] buf=Encoding.Default.GetBytes(response);
		
		return new Authorization("Basic "+Convert.ToBase64String(buf));
	}


	// Get the authentiation type supported by this module.
	public String AuthenticationType
	{
		get
		{
			return "Basic";
		}
	}

	// Determine if this module supports pre-authentication.
	public bool CanPreAuthenticate 
	{
		get
		{
			return true;
		}
	}

}; // class BasicClient

}; // namespace System.Net
