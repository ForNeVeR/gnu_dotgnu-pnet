/*
 * CredentialCache.cs - Implementation of the "System.Net.CredentialCache" class.
 * 
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * Contributions from Charlie Carnow <carnow@gmx.net>
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
	using System.Collections.ICredentials;
	using System.Collections.Hashtable;
	using System.Collections.IEnumerable;
	using System;
	public class CredentialCache : ICredentials, IEnumerable
	{
		private Hashtable cache;
		private static CredentialCache default;
		public CredentialCache()
		{
			// TODO: Set default to Credentials of the current
			// process
			cache = new Hashtable();
		}
		
		public void Add(Uri uriPrefix, string authtype, NetworkCredential cred)
		{
			if (uriPrefix == null || authType == null) 
			{
				throw new ArgumentNullException();
			}

			if(cache.Contains(uriPrefix) == false) 
			{
				cache[uriPrefix] = new Hashtable();
			}
	
			if(cache[uriPrefix].Contains(authtype))		
			{
				throw new ArgumentException();
			}
			cache[uriPrefix][authType] = cred;
	}

	public NetworkCredential GetCredential(Uri uriPrefix, string authType)
	{
		if (uriPrefix == null || authType == null) 
		{
			throw new ArgumentNullException();
		}
		
		if (cache.Contains(uriPrefix))
		{
			if (cache[uriPrefix].Contains(authType))
			{
				return cache[uriPrefix][authType];
			}
		}
		// Didn't find credential
		return null;

	}
	
	public IEnumerator GetEnumerator()
	{
		return cache.GetEnumerator();
	}
		
	public void Remove(Uri uriPrefix, string authType) 
	{
		if (cache.Contains(uriPrefix))
		{
			if (cache[uriPrefix].Contains(authType)) 
			{
				cache[uriPrefix].Remove(authType);
			}
		}

	}
	
	public static ICredentials DefaultCredentials 
	{
		get
		{
			return default;
		}
	}
   }

}











