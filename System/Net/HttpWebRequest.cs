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
using System.Net.Sockets;

public class HttpWebRequest : WebRequest
{


	//Call some sort of constructor from 
	//System.Net.WebRequest.CreateDefault()
	private override void Connect(Uri address)
	{
		this.Address=address;
		
		IPAddress ip=Dns.Resolve(Address.Host);
		IPEndPoint ep = new IPEndPoint(ip,Address.Port);
		server=new Socket(AddressFamily.InterNetwork,SocketType.Stream,
							Protocoltype.TCP);
		server.Connect(ep);
		out=new StreamWriter(this.GetRequestStream());
		HttpWebResponse retval=new HttpWebResponse();
		out.write(this.Method+" "+this.Address.PathAndQuery+" HTTP/"+
					this.Version.Major+""+this.Version.Minor+"\r\n");
		
		/* Accept */
		if(this.Accept!=null)
			out.write("Accept: "+this.Accept+"\r\n");
		else
			out.write("Accept: */*\r\n");
	
		/* Connection */
		if(this.Connection!=null && this.KeepAlive)
			out.write("Connection: "+this.Connection + " Keep-alive \r\n");
		else if (this.KeepAlive)
			out.write("Connection: Keep-alive\r\n");
		else
			out.write("Connection: Close\r\n");
	
		/* Content-Length */	
		if(this.ContentLength!=-1)
			out.write("Content-Length: "+this.ContentLength+"\r\n");
			
		/* Content-Type */
		if(this.ContentType!=null)
			out.write("Content-Type: "+this.ContentType+"\r\n");

		if(this.Expect!=null)
			out.write("Expect: "+this.Expect+"\r\n");

		if(this.IfModifiedSince!=null)
		{
			String format="ddd, dd MMM yyyy HH*:mm:ss GMTzz";//convert to GMT
			out.write("If-Modified-Since: "+this.IfModifiedSince.toString
			(format)+"\r\n");			
		}
		
		/* Implement User-Auth here */
		/* use PreAuthenticate & ICredentials to do the job */
		
		if(this.Referer!=null)
			out.write("Referer: "+this.Referer+"\r\n");

		if(this.TransferEncoding!=null)
			out.write("Transfer-Encoding: "+this.TransferEncoding+"\r\n");

		/* User Agent */
		if(this.UserAgent!=null)
			out.write("User-Agent: "+this.UserAgent+"\r\n");

		headerSent=true;
		out.flush();
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

	[TODO] 
	//need internationalisation
	public void AddRange(string rangeSpecifier,int from,int to)
	{
		if(from <0) 
			throw new ArgumentOutOfRangeException("from",_("Arg_OutOfRange"));
		if(from > to)
			throw new ArgumentOutOfRangeException("from > to ",
				_("Arg_OutOfRange"));
		if(rangeSpecifier == null)
			throw new ArgumentNullException("rangeSpecifier",_("Arg_Null"));

		if(this.Method != null && ! this.Method.Equals("GET"))
			throw new InvalidOperationException("Invalid Method");
		this.Headers.Set("Range",rangeSpecifier+"="+from+"-"+to);
	}

	public void Addrange(string rangeSpecifier, int from)
	{
		if(range <0) 
			throw new ArgumentOutOfRangeException("from",_("Arg_OutOfRange"));
		if(rangeSpecifier == null)
			throw new ArgumentNullException("rangeSpecifier",_("Arg_Null"));
		if(this.Method != null && ! this.Method.Equals("GET"))
			throw new InvalidOperationException("Invalid Method");
		this.Headers.Set("Range",rangeSpecifier+"="+from+"-");
	}
	
	[TODO]
	//unclear about how to go about this
	public override IAsyncResult BeginGetRequestStream(AsyncCallBack callback,
													   object state)
	{
	}

	[TODO]
	//same problem here
	public override IAsyncResult BeginGetResponse(AsyncCallback callback,
												  object state)
	{
	}
	
	[TODO]
	//how can I use IAsyncResult.AsyncWaitHandle
	public override Stream EndGetRequestStream(IAsyncResult asyncResult)
	{
		object state=asyncResult.AsyncState;	
	}
	
	[TODO]
	public override WebResponse EndGetResponse(IAsyncResult asyncResult)
	{
		object state=asyncResult.AsyncState;
	}

	[TODO]
	public override int GetHashCode()
	{
		return 0;//just to make it compiling
	}
	
	[TODO]
	// put in exception handling 
	// and proxy support 
	public override Stream GetRequestStream()
	{
		if(out==null)throw new WebException("not connected");
		if(!canGetRequestStream())
			throw new WebException(" not allowed for " + this.Method);
		return out;
	}
	
	[TODO]
	//needs some clarification on the implementation
	//really need some doubts cleared
	public override WebResponse GetResponse()
	{
	}
/*
 * Implement the Checks for Setting values
 */
	public string Accept 
	{
		get
		{
			return accept;
		}
		set
		{
			if(value !=null)
				this.accept=value;
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
			return this.connection;
		}
		set
		{
			if(value=="Keep-alive" || value=="Close")
			{
				this.connection=value;
			}
			else throw new ArgumentException("Value NOT keep alive or close");
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
			if(headerSent)
				throw new InvalidOperationException(" header already sent");
			this.contentLength=value;
		}
	}

	public override string ContentType 
	{
		get
		{
			return this.contentType;
		}
		set
		{
			this.contentType=value;
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
			this.continueDelegate=false;
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
			return this.expect;
		} 
		set
		{
			if(value.ToLower()=="100-continue") 
				throw new ArgumentException("cannot set "+value);
			this.expect=value;
		}
	}

	public bool HaveResponse 
	{
		get
		{
			return this.haveResponse;
		}
	}
/*
	public override WebHeaderCollection Headers 
	{
		get
		{
			return this.
		} 
		set
		{
			this.=value;
		}
	}*/

	public DateTime IfModifiedSince 
	{
		get
		{
			return this.ifModifiedSince;
		} 
		set
		{
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
			if(value.Major==1 && (value.Minor==0 || value.Minor==1))
			{
				this.=value;
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
			if(headerSent)throw new InvalidOperationException("Header sent");
			//TODO: implement SecurityException
			this.proxy=value;
		}
	}

	public string Referer 
	{
		get
		{
			return this.referer;
		} 
		set
		{
			this.referer=value;
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
			if(headerSent)throw new InvalidOperationException("data sent");
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
			if(this.timeout<0 && this.timeout!=\
			System.Threading.Timeout.Infinite)
				throw new ArgumentOutOfRangeException("timeout < 0");
			this.timeout=value;
		}
	}

	public string TransferEncoding 
	{
		get
		{
			return this.transferEncoding;
		} 
		set
		{
			if(!this.sendChunked)throw new 
				InvalidOperationException(" send chunked set");
			if(String.Compare("Chunked",value,1))
				throw new ArgumentException(" cannot chunk it");
			this.transferEncoding=value;
		}
	}

	public string UserAgent 
	{
		get
		{
			return this.userAgent;
		} 
		set
		{
			this.userAgent=value;
		}
	}
	
// my private vars
// I'm storing all my properties in fields and using properties
// like I did with my javabean get , set methods

	private String accept="*/*";
	private Uri address=null;
	private boolean allowAutoRedirect=true;
	private boolean allowWriteStreamBuffering=true;
	private String connection=null;
	private String connectionGroupName=null;
	private long contentLength=-1;
	private string contentType=null;
	private HttpContinueDelegate ContinueDelegate=null;
	private ICredentials credentials=null;
	private string expect=null;
	private bool haveResponse=false;
/*look manual*/	private WebHeaderCollection headers;
/*look manual*/	private DateTime ifModifiedSince;
	private bool keepAlive=true;
/*look manual*/	private int maximumAutomaticRedirections;
	private string method="GET";
	private bool pipelined=true;
	private bool preAuthenticate=false;
	private Version protocolVersion=System.Net.HttpVersion.Version11;
/*look manual*/	private IWebProxy proxy;
	private string referer=null;
/*look manual*/	private Uri requestUri;
	private bool sendChunked=false;
//	private ServicePoint servicePoint=null;
/*look manual*/	private int timeout;
	private string transferEncoding=null;
	private string userAgent=null;

// my special vars & methods
	private headerSent=false;
	private Stream out=null;
	private Stream in=null;
	
	private boolean canGetRequestStream()
	{
		if((this.method.equals("PUT")) || (this.method.equals("POST")))
			return true;
		return false;
	}
	private boolean isHTTPMethod(String val)
	{
		if(val==null)return false;
		if(val=="GET" || val=="HEAD" || val=="POST" || val=="PUT" ||
			val=="DELETE" || val=="TRACE" || val=="OPTIONS")
			{
				return true;
			}
		return false;
	}

}//class

}//namespace
