/*
 * WebRequest.cs - Implementation of the "System.Net.WebRequest" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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
using System.IO;
using System.Collections;

public class WebRequest : MarshalByRefObject
{
	private String              connectionGroupName;
	private Int64               contentLength;
	private String              contentType;
	private ICredentials        credentials;
	private WebHeaderCollection headers;
	private String              method;
	private Boolean             preAuthenticate;
	private IWebProxy           proxy;
	private Uri                 requestUri;
	private Int32               timeout;
	private static Hashtable    prefixes;

	protected WebRequest()
	{
		connectionGroupName = "";
		contentLength = 0;
		contentType = "";
		credentials = null;
		method = "";
		preAuthenticate = false;
		proxy = null;
		requestUri = "";
		timeout = 0;
		prefixes.Clear();
	}

	public virtual void Abort()
	{
		throw new NotSupportedException("Abort", _("Exception_NotSupported"));
	}

	public virtual IAsyncResult BeginGetRequestStream(AsyncCallback callback, object state)
	{
		throw new NotSupportedException("BeginGetRequestStream", _("Exception_NotSupported"));
	}

	public virtual IAsyncResult BeginGetResponse(AsyncCallback callback, object state)
	{
		throw new NotSupportedException("BeginGetResponse", _("Exception_NotSupported"));
	}

	[ TODO ]
	public static WebRequest Create(string requestUriString)
	{
		if (requestUriString == null)
		{
			throw new ArgumentNullException("Create", _("Arg_NotNull"));
		}

		Uri requestUri = new Uri(requestUriString);		

		return Create(requestUri);
	}

	[ TODO ]
	public static WebRequest Create(Uri requestUri)
	{
		// TODO: Match Uri against complete Uri and scheme and find out
		// if it is supported, otherwise throw up an error
		// throw new NotSupportedException("Create", _("Exception_NotSupported"));

		// TODO: the URI format is invalid
		// throw new UriFormatException("requestUriString", _(""));

		// TODO: this client does not have the permission to connect to the URI or
		// the URI that the request is redirected to.
		// throw new SecurityException("requestUriString", _(""));
	}

	[ TODO ]
	public static WebRequest CreateDefault(Uri requestUri)
	{
		// TODO: Find out if just scheme is supported, otherwise throw up an error
		// do not check for the complete Uri (in the case of longer Uri's passed).
		// see spec
		// throw new NotSupportedException("CreateDefault", _("Exception_NotSupported"));

		// TODO: the URI format is invalid
		// throw new UriFormatException("requestUriString", _(""));

		// TODO: this client does not have the permission to connect to the URI or
		// the URI that the request is redirected to.
		// throw new SecurityException("requestUriString", _(""));
	}

	public virtual Stream EndGetRequestStream(IAsyncResult asyncResult)
	{
		throw new NotSupportedException("EndGetRequestStream", _("Exception_NotSupported"));
	}

	public virtual WebResponse EndGetResponse(IAsyncResult asyncResult)
	{
		throw new NotSupportedException("EndGetResponse", _("Exception_NotSupported"));
	}

	public virtual Stream GetRequestStream()	
	{
		throw new NotSupportedException("GetRequestStream", _("Exception_NotSupported"));
	}

	public virtual WebResponse GetResponse()
	{
		throw new NotSupportedException("GetResponse", _("Exception_NotSupported"));
	}

	[ TODO ]
	public static bool RegisterPrefix(string prefix, IWebRequestCreate creator)
	{
		if (prefix== null)
		{
			throw new ArgumentNullException("prefix", _("Arg_NotNull"));
		}

		if (creator== null)
		{
			throw new ArgumentNullException("creator", _("Arg_NotNull"));
		}

		// TODO: further implementation...
	}

	public virtual string ConnectionGroupName 
	{ 
		get
		{
			throw new NotSupportedException("ConnectionGroupName ", _("Exception_NotSupported"));
		}
		
		set
		{
			throw new NotSupportedException("ConnectionGroupName ", _("Exception_NotSupported"));
		} 
	}

	public virtual long ContentLength
	{
		get
		{
			throw new NotSupportedException("ContentLength", _("Exception_NotSupported"));
		}
		
		set
		{
			throw new NotSupportedException("ContentLength", _("Exception_NotSupported"));
		}
	}

	public virtual string ContentType
	{
		get
		{
			throw new NotSupportedException("ContentType", _("Exception_NotSupported"));
		}
		
		set
		{
			throw new NotSupportedException("ContentType", _("Exception_NotSupported"));
		}
	}

	public virtual ICredentials Credentials
	{
		get
		{
			throw new NotSupportedException("Credentials", _("Exception_NotSupported"));
		}
		
		set
		{
			throw new NotSupportedException("Credentials", _("Exception_NotSupported"));
		}
	}

	public virtual WebHeaderCollection Headers
	{
		get
		{
			throw new NotSupportedException("Headers", _("Exception_NotSupported"));
		}
		
		set
		{
			throw new NotSupportedException("Headers", _("Exception_NotSupported"));
		}
	}

	public virtual string Method
	{
		get
		{
			throw new NotSupportedException("Method", _("Exception_NotSupported"));
		}
		
		set
		{
			throw new NotSupportedException("Method", _("Exception_NotSupported"));
		}
	}

	public virtual bool PreAuthenticate
	{
		get
		{
			throw new NotSupportedException("PreAuthenticate", _("Exception_NotSupported"));
		}
		
		set
		{
			throw new NotSupportedException("PreAuthenticate", _("Exception_NotSupported"));
		} 
	}

	public virtual IWebProxy Proxy
	{
		get
		{
			throw new NotSupportedException("Proxy", _("Exception_NotSupported"));
		}
		
		set
		{
			throw new NotSupportedException("Proxy", _("Exception_NotSupported"));
		}
	}

	public virtual Uri RequestUri
	{
		get
		{
			throw new NotSupportedException("RequestUri", _("Exception_NotSupported"));
		}
		
		set
		{
			throw new NotSupportedException("RequestUri", _("Exception_NotSupported"));
		}
	}

	public virtual int Timeout
	{
		get
		{
			throw new NotSupportedException("Timeout", _("Exception_NotSupported"));
		}
		
		set
		{
			throw new NotSupportedException("Timeout", _("Exception_NotSupported"));
		} 
	}
	
}; // class WebRequest

}; // namespace System.Net
