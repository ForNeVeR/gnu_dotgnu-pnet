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

namespace System.Net
{

using System;
using System.IO;
using System.Net.Sockets;

public class HttpWebRequest : WebRequest
{

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
		object state=asyncResult.AsyncStater;
	}

	[TODO]
	public override int GetHashCode()
	{
	}
	
	[TODO]
	public override Stream GetRequestStream()
	{
		IPAddress ip=Dns.Resolve(Address.Host);
		IPEndPoint ep = new IPEndPoint(ip,Address.Port);
		server=new Socket(AddressFamily.InterNetwork,SocketType.Stream,
							Protocoltype.TCP);
		server.Connect(ep);
	}
	
	[TODO]
	public override WebResponse GetResponse()
	{
	}

	public string Accept {get; set;}

	public Uri Address {get; set;}

	public bool AllowAutoRedirect {get; set;}

	public bool AllowWriteStreamBuffering {get; set;}

	public string Connection {get; set;}

	public override string ConnectionGroupName {get; set;}

	public override long ContentLength {get; set;}

	public override string Contenttype {get; set;}

	public HttpContinueDelegate ContinueDelegate {get; set;}

	public override ICredentials Credentials {get; set;}

	public string Expect {get; set;}

	public bool HaveResponse {get;}

	public override WebHeaderCollection Headers {get; set;}

	public DateTime IfModifiedSince {get; set;}

	public bool KeepAlive {get; set;}

	public int MaximumAutomaticRedirections {get; set;}

	public string MediaType {get; set;}

	public override string Method {get; set;}

	public bool Pipelined {get; set;}

	public override bool PreAuthenticate {get; set;}

	public Version ProtocolVersion {get; set;}

	public override IWebProxy Proxy {get; set;}

	public string Referer {get; set;}

	public override Uri RequestUri {get;}

	public bool SendChunked {get; set;}

	public ServicePoint ServicePoint {get; set;}

	public override int Timeout {get; set;}

	public string TransferEncoding {get; set;}

	public string UserAgent {get; set;}

	private Socket server {get; set;}

}//class

}//namespace

