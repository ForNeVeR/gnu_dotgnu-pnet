
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











