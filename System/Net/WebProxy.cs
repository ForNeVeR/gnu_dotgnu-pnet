/*
 * WebProxy.cs - Implementation of the "System.Net.WebProxy" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2003  Free Software Foundation, Inc.
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

using System.Collections;
using System.Runtime.Serialization;

[TODO]
public class WebProxy : IWebProxy
#if CONFIG_SERIALIZATION
	, ISerializable
#endif
{
	[TODO]
	public WebProxy()
	{
		throw new NotImplementedException(".ctor");
	}

	[TODO]
	public WebProxy(string Address)
	{
		throw new NotImplementedException(".ctor");
	}

	[TODO]
	public WebProxy(Uri Address)
	{
		throw new NotImplementedException(".ctor");
	}

#if CONFIG_SERIALIZATION
	[TODO]
	protected WebProxy(SerializationInfo serializationInfo, StreamingContext streamingContext)
	{
		throw new NotImplementedException(".ctor");
	}
#endif

	[TODO]
	public WebProxy(string Address, bool BypassOnLocal)
	{
		throw new NotImplementedException(".ctor");
	}

	[TODO]
	public WebProxy(string Host, int Port)
	{
		throw new NotImplementedException(".ctor");
	}

	[TODO]
	public WebProxy(Uri Address, bool BypassOnLocal)
	{
		throw new NotImplementedException(".ctor");
	}

	[TODO]
	public WebProxy(string Address, bool BypassOnLocal, string[] BypassList)
	{
		throw new NotImplementedException(".ctor");
	}

	[TODO]
	public WebProxy(Uri Address, bool BypassOnLocal, string[] BypassList)
	{
		throw new NotImplementedException(".ctor");
	}

	[TODO]
	public WebProxy(string Address, bool BypassOnLocal, string[] BypassList, ICredentials Credentials)
	{
		throw new NotImplementedException(".ctor");
	}

	[TODO]
	public WebProxy(Uri Address, bool BypassOnLocal, string[] BypassList, ICredentials Credentials)
	{
		throw new NotImplementedException(".ctor");
	}

	[TODO]
	public Uri Address
	{
		get
		{
			throw new NotImplementedException("Address");
		}
		set
		{
			throw new NotImplementedException("Address");
		}
	}

	[TODO]
	public ArrayList BypassArrayList
	{
		get
		{
			throw new NotImplementedException("BypassArrayList");
		}
	}

	[TODO]
	public string[] BypassList
	{
		get
		{
			throw new NotImplementedException("BypassList");
		}
		set
		{
			throw new NotImplementedException("BypassList");
		}
	}

	[TODO]
	public bool BypassProxyOnLocal
	{
		get
		{
			throw new NotImplementedException("BypassProxyOnLocal");
		}
		set
		{
			throw new NotImplementedException("BypassProxyOnLocal");
		}
	}

	[TODO]
	public virtual ICredentials Credentials
	{
		get
		{
			throw new NotImplementedException("Credentials");
		}
		set
		{
			throw new NotImplementedException("Credentials");
		}
	}

	[TODO]
	public static WebProxy GetDefaultProxy()
	{
		throw new NotImplementedException("GetDefaultProxy");
	}

	[TODO]
	public virtual Uri GetProxy(Uri destination)
	{
		throw new NotImplementedException("GetProxy");
	}

	[TODO]
	public virtual bool IsBypassed(Uri host)
	{
		throw new NotImplementedException("IsBypassed");
	}

#if CONFIG_SERIALIZATION
	[TODO]
	void ISerializable.GetObjectData(SerializationInfo serializationInfo, StreamingContext streamingContext)
	{
		throw new NotImplementedException("ISerializable.GetObjectData");
	}
#endif

}; // class WebProxy

}; // namespace System.Net
