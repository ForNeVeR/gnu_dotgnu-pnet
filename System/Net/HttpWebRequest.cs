/*
 * HttpWebRequest.cs - Implementation of the "System.Net.HttpWebRequest" class.
 *
 * Copyright (C) 2002 FSF India
 * 
 * Author: Gopal.V
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


/*
 * Please refer RFC-2616 for more info on HTTP/1.1 Protocol.
 *
 */
namespace System.Net
{

using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Net.Sockets;

public class HttpWebRequest : WebRequest
{
	private String accept="*/*";
	private Uri address=null;
	private Uri originalUri=null;
	private bool allowAutoRedirect=true;
	private bool allowWriteStreamBuffering=true;
	private String connectionGroupName=null;
	private long contentLength=-1;
	private HttpContinueDelegate continueDelegate=null;
	private ICredentials credentials=null;
	private bool haveResponse=false;
	private WebHeaderCollection headers=new WebHeaderCollection();
	private DateTime ifModifiedSince=DateTime.Now;
	private bool keepAlive=true;
	private int maximumAutomaticRedirections=5;
	private string method="GET";
	private bool pipelined=true;
	private bool preAuthenticate=false;
	private Version protocolVersion=System.Net.HttpVersion.Version11;
	private IWebProxy proxy;
	private Uri requestUri;
	private bool sendChunked=false;
//	private ServicePoint servicePoint=null;
	private int timeout;
	private string mediaType=null;

// other useful variables
	protected bool headerSent=false; 
	// so that it is accessible to the nested classes
	private Stream outStream=null;
	private WebResponse response=null;

	internal HttpWebRequest(Uri uri)
	{
		this.address=uri;
		this.originalUri=uri;
		this.method="GET";
	}

	[TODO]
	public override void Abort()
	{
	}
	
	public void AddRange(int from, int to)
	{
		AddRange("bytes",from,to);
	}

	public void AddRange(int range)
	{
		AddRange("bytes",range);
	}

	//need internationalisation
	public void AddRange(string rangeSpecifier,int from,int to)
	{
		if(from <0) 
			throw new ArgumentOutOfRangeException("from",S._("Arg_OutOfRange"));
		if(from > to)
			throw new ArgumentOutOfRangeException("from > to ",
				S._("Arg_OutOfRange"));
		if(rangeSpecifier == null)
			throw new ArgumentNullException("rangeSpecifier",S._("Arg_Null"));

		if(this.Method != null && ! this.Method.Equals("GET"))
			throw new InvalidOperationException("Invalid Method");
		this.Headers.Set("Range",rangeSpecifier+"="+from+"-"+to);
	}

	public void AddRange(string rangeSpecifier, int from)
	{
		if(from <0) 
			throw new ArgumentOutOfRangeException("from",S._("Arg_OutOfRange"));
		if(rangeSpecifier == null)
			throw new ArgumentNullException("rangeSpecifier",S._("Arg_Null"));
		if(this.Method != null && ! this.Method.Equals("GET"))
			throw new InvalidOperationException("Invalid Method");
		this.Headers.Set("Range",rangeSpecifier+"="+from+"-");
	}
	
	[TODO]
	//unclear about how to go about this
	public override IAsyncResult BeginGetRequestStream(AsyncCallback callback,
													   object state)
	{
		return null;
	}

	[TODO]
	//same problem here
	public override IAsyncResult BeginGetResponse(AsyncCallback callback,
												  object state)
	{
		return null;
	}
	
	[TODO]
	//how can I use IAsyncResult.AsyncWaitHandle
	public override Stream EndGetRequestStream(IAsyncResult asyncResult)
	{
		object state=asyncResult.AsyncState;	
		return null;
	}
	
	[TODO]
	public override WebResponse EndGetResponse(IAsyncResult asyncResult)
	{
		object state=asyncResult.AsyncState;
		return null;
	}

	[TODO]
	public override int GetHashCode()
	{
		return 0;//just to make it compiling
	}
	
	// put in exception handling 
	// and proxy support 
	public override Stream GetRequestStream()
	{
		if(!canGetRequestStream())
			throw new WebException(" not allowed for " + this.Method);
		if(outStream==null)
		{
			outStream=new HttpStream(this);
		}
		return outStream;
	}
	
	[TODO]
	//needs some clarification on the implementation
	//really need some doubts cleared
	public override WebResponse GetResponse()
	{
		if(response!=null) return response;
		if(outStream==null)
		{
			outStream=new HttpStream(this);
			/* which is the response stream as well */
		}
		this.response=new HttpWebResponse(this,this.outStream);
		return this.response; 
	}
/*
 * Implement the Checks for Setting values
 */
	public string Accept 
	{
		get
		{
			return headers["Accept"];
		}
		set
		{
			CheckHeadersSent();
			headers.SetInternal("Accept",value);
		}
	}
	
	public Uri Address 
	{
		get
		{
			return this.address;	
		}
	}

	public bool AllowAutoRedirect 
	{
		get
		{
			return this.allowAutoRedirect;
		}
		
		set
		{
			this.allowAutoRedirect=value;
		}
	}

	public bool AllowWriteStreamBuffering 
	{
		get 
		{
			return this.allowWriteStreamBuffering;	
		}
		set
		{
			this.allowWriteStreamBuffering=value;
		}
	}

	public string Connection 
	{
		get
		{
			return headers["Connection"];
		}
		set
		{
			CheckHeadersSent();
			String str;
			if(value!=null)str=value.ToLower().Trim();
			if(str==null || str.Length==0)
			{
				headers.RemoveInternal("Connection");
				return;
			}
			if(str=="keep-alive" || str=="close")
			{
				headers.SetInternal("Connecton",str);
			}
			else throw new ArgumentException("Value NOT keep-alive or close");
		}
	}

	public override string ConnectionGroupName 
	{
		get
		{
			return this.connectionGroupName;
		}
		set
		{
			this.connectionGroupName=value;
		}
	}

	public override long ContentLength 
	{
		get
		{
			return this.contentLength;
		}
		set
		{
			if(value<0) 
				throw new ArgumentOutOfRangeException("Content-Length < 0");
			CheckHeadersSent();
			this.contentLength=value;
			this.headers.SetInternal("Content-Length",value.ToString());
		}
	}

	public override string ContentType 
	{
		get
		{
			return headers["Content-Type"];
		}
		set
		{
			CheckHeadersSent();
			this.headers.SetInternal("Content-Type",value);
			// I should really check for <type>/<subtype> in it ;)
		}
	}

	public HttpContinueDelegate ContinueDelegate 
	{
		get
		{
			return this.continueDelegate;
		}
		set
		{
			this.continueDelegate=value;
		}
	}

	public override ICredentials Credentials 
	{
		get
		{
			return this.credentials;
		}
		set
		{
			this.credentials=value;
		}
	}

	public string Expect 
	{
		get
		{
			return headers["Expect"];
		} 
		set
		{
			CheckHeadersSent();
			if(value.ToLower()=="100-continue") 
				throw new ArgumentException("cannot set 100-continue");
			headers.SetInternal("Expect",value);
		}
	}

	public bool HaveResponse 
	{
		get
		{
			return this.haveResponse;
		}
	}

	public override WebHeaderCollection Headers 
	{
		get
		{
			return this.headers;
		} 
		set
		{
			CheckHeadersSent();
			this.headers=value;
		}
	}

	public DateTime IfModifiedSince 
	{
		/* avoid the thunk of Headers */
		get
		{
			return this.ifModifiedSince;
		} 
		set
		{
			CheckHeadersSent();
			String format="ddd, dd MMM yyyy HH*:mm:ss GMTzz";//convert to GMT
			headers.SetInternal("IfModifiedSince", value.ToString(format));
			this.ifModifiedSince=value;
		}
	}

	public bool KeepAlive 
	{
		get
		{
			return this.keepAlive;
		} 
		set
		{
			CheckHeadersSent();
			this.keepAlive=value;
		}
	}

	public int MaximumAutomaticRedirections 
	{
		get
		{
			return this.maximumAutomaticRedirections;
		} 
		set
		{
			if(value<=0)
				throw new ArgumentException("redirections <= 0");
			this.maximumAutomaticRedirections=value;
		}
	}

	public string MediaType 
	{
		get
		{
			return this.mediaType;
		} 
		set
		{
			CheckHeadersSent();
			this.mediaType=value;
		}
	}

	public override string Method 
	{
		get
		{
			return this.method;
		} 
		set
		{
			CheckHeadersSent();
			if(isHTTPMethod(value))
				this.method=value;
			else 
				throw new ArgumentException("invalid method");
		}
	}

	public bool Pipelined 
	{
		get
		{
			return this.pipelined;
		} 
		set
		{
			CheckHeadersSent();
			this.pipelined=value;
		}
	}

	public override bool PreAuthenticate 
	{
		get
		{
			return this.preAuthenticate;
		} 
		set
		{
			this.preAuthenticate=value;
		}
	}

	public Version ProtocolVersion 
	{
		get
		{
			return this.protocolVersion;
		} 
		set
		{
			CheckHeadersSent();
			if(value.Major==1 && (value.Minor==0 || value.Minor==1))
			{
				this.protocolVersion=value;
			}
			else 
				throw new ArgumentException("Wrong args");
		}
	}

	[TODO]
	public override IWebProxy Proxy 
	{
		get
		{
			return this.proxy;
		} 
		set
		{
			if(value==null)throw new ArgumentNullException("Proxy null");
			CheckHeadersSent();
			//TODO: implement SecurityException
			this.proxy=value;
		}
	}

	public string Referer 
	{
		get
		{
			return headers["Referer"];
		} 
		set
		{
			CheckHeadersSent();
			headers.SetInternal("Referer",value);
		}
	}

	public override Uri RequestUri 
	{
		get
		{
			return this.requestUri;
		}
	}

	public bool SendChunked 
	{
		get
		{
			return this.sendChunked;
		} 
		set
		{
			CheckHeadersSent();
			this.sendChunked=value;
		}
	}
/*
	public ServicePoint ServicePoint 
	{
		get
		{
			return this.servicePoint;
		} 
	}
*/

	public override int Timeout 
	{
		get
		{
			return this.timeout;
		} 
		set
		{
			if(this.timeout<0 && this.timeout!=
			System.Threading.Timeout.Infinite)
				throw new ArgumentOutOfRangeException("timeout < 0");
			this.timeout=value;
		}
	}

	public string TransferEncoding 
	{
		get
		{
			return headers["Transfer-Encoding"];
		} 
		set
		{
			CheckHeadersSent();
			if(!this.sendChunked)throw new 
				InvalidOperationException(" send chunked set");
			if(String.Compare("Chunked",value,true) == 0)
				throw new ArgumentException(" cannot chunk it");
			headers.SetInternal("Transfer-Encoding",value);
		}
	}

	public string UserAgent 
	{
		get
		{
			return headers["User-Agent"];
		} 
		set
		{
			headers.SetInternal("User-Agent",value);
		}
	}
	

	private void CheckHeadersSent()
	{
		if(headerSent)
		{
			throw new InvalidOperationException("Headers already sent");
		}
	}
	private bool canGetRequestStream()
	{
		if((this.method.Equals("PUT")) || (this.method.Equals("POST")))
			return true;
		return false;
	}
	private bool isHTTPMethod(String val)
	{
		if(val==null)return false;
		if(val=="GET" || val=="HEAD" || val=="POST" || val=="PUT" ||
			val=="DELETE" || val=="TRACE" || val=="OPTIONS")
			{
				return true;
			}
		return false;
	}
	private class HttpStream : NetworkStream
	{	
		private HttpWebRequest request;
		public HttpStream(HttpWebRequest req) :
			base(HttpStream.OpenSocket(req),true)
		{
			this.request=req;
			SendHeaders();
		}
		private static Socket OpenSocket(HttpWebRequest req)
		{
			IPAddress ip=Dns.Resolve(req.Address.Host).AddressList[0];
			IPEndPoint ep = new IPEndPoint(ip,req.Address.Port);
			Socket server=new 
					Socket(AddressFamily.InterNetwork,SocketType.Stream,
							ProtocolType.Tcp);
			server.Connect(ep);
			return server;
		}
		private void SendHeaders()
		{
			StreamWriter writer=new StreamWriter(this);
			request.headerSent=true; 
			/* fake it before sending to allow for atomicity */
			String requestString= request.Method+" "+
			//		request.Address.PathAndQuery+
					"/"+
					" HTTP/"+request.protocolVersion.Major+
					"."+request.protocolVersion.Minor+"\r\n";
			writer.Write(requestString);
			writer.Write(request.Headers.ToString());
			writer.Write("\r\n");// terminating CRLF
			writer.Flush();
		}
	} //internal class
}//class

}//namespace

