/*
 * HttpWebResponse.cs - Implementation of the
 *		"System.Net.HttpWebResponse" class.
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
using System.Text;
using System.Globalization;

public class HttpWebResponse : WebResponse
{
	private HttpWebRequest req=null;
	private Stream stream=null;
	private WebHeaderCollection headers=new WebHeaderCollection();
	private Version version=null;
	private HttpStatusCode code=0;
	private String desc=null;
	internal HttpWebResponse(HttpWebRequest request,Stream dataStream)
	{
		req=request;
		stream=dataStream;
		ProcessRequest();
		headers.SetStrict(false); /* none of the restrictions of Request */
		ProcessHeaders();
	}
	private void ProcessRequest()
	{
		String response=ReadLine();
		int i=response.IndexOfAny(new char[]{' ','\t'});
		if(i==-1) throw new WebException("Invalid Response");
		switch(response.Substring(0,i))
		{
			case "HTTP/1.0":
				version=HttpVersion.Version10;
				break;
			case "HTTP/1.1":
				version=HttpVersion.Version11;
				break;
			default:
				throw new WebException("Unknown Protocol version");
		}
		response=response.Substring(i+1);
		i=response.IndexOfAny(new char[]{' ','\t'});
		if(i==-1) throw new WebException("Invalid Response");
		code=(HttpStatusCode) Int32.Parse(response.Substring(0,i));
		response=response.Substring(i+1);
		desc=response;
	}
	private void ProcessHeaders()
	{
		/* sometimes servers tend to send plain "\n"s */
		for(String s=ReadLine();s.Trim()!="";s=ReadLine())
		{
			headers.Add(s); 
		}
	}
	private String ReadLine()
	{
		StringBuilder builder = new StringBuilder();
		int ch;
		for(;;)
		{
			// Process characters until we reach a line terminator.
			ch=stream.ReadByte();
			if(ch==-1)
			{
				break;
			}
			else if(ch == 13)
			{
				if((ch=stream.ReadByte())==10)
				{
					return builder.ToString();
				}
				else if(ch==-1)
				{
					break;
				}
				else
				{
					builder.Append("\r"+(byte)ch);
					/* that "\r" is added to the stuff */
				}
			}
			else if(ch == 10)
			{
				// This is an LF line terminator.
				return builder.ToString();
			}
			else
			{
				builder.Append((char)ch);
			}
		}
		if(builder.Length!=0) return builder.ToString(); 
		else return null;
	}
	public override void Close()
	{
		this.req.Close();
	}

	[TODO]
	protected virtual void Dispose(bool disposing)
	{
	/*TODO*/
		throw new NotImplementedException();
	}

	[TODO]
	public override int GetHashCode()
	{
	/*TODO*/
		throw new NotImplementedException();
	}

	public string GetResponseHeader(string headerName)
	{
		return headers[headerName];
	}

	public override Stream GetResponseStream()
	{
		return stream;  
	}

	public string CharacterSet 
	{ 
		get
		{
			return "ISO-8859-1";
			/*
				TODO: figure out how to get correct CharacterSet,
				all headers don't have charsets
			*/
		}
	}

	public string ContentEncoding 
	{ 
		get
		{
			return headers["Content-Encoding"];
		}
	}

	public override long ContentLength 
	{ 
		get
		{
			return Int64.Parse(headers["Content-Length"]);
		}
	}

	public override string ContentType 
	{ 
		get
		{
			return headers["Content-Type"];
		}
	}

	public override WebHeaderCollection Headers 
	{
		get
		{
			return headers;
		}
	}

	public DateTime LastModified 
	{ 
		get 
		{
			String format="ddd, dd MMM yyyy HH*:mm:ss GMTzz";//convert to GMT
			return DateTime.ParseExact(headers["Last-Modified"],format,null,
						DateTimeStyles.None);
		}
	}

	public string Method 
	{ 
		get
		{
			return req.Method;
		}
	}

	public Version ProtocolVersion 
	{ 
		get
		{
			return version;
		}
	}

	/* TODO : clarify how redirects work */
	public override Uri ResponseUri 
	{ 
		get
		{
			return req.Address;
		}
	}

	public string Server 
	{
		get
		{
			return headers["Server"];
		}
	}

	public HttpStatusCode StatusCode 
	{ 
		get
		{
			return code;
		}
	}

	public string StatusDescription 
	{ 
		get
		{
			return desc;
		}
	}
}; // class HttpWebResponse

}; //namespace System.Net
