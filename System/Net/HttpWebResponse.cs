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
using System.Text;
using System.IO;

[TODO]
public class HttpWebResponse : WebResponse
{
	HttpWebRequest req=null;
	Stream stream=null;
	internal HttpWebResponse(HttpWebRequest request,Stream dataStream)
	{
		req=request;
		stream=dataStream;
		ProcessRequest();
	}
	private void ProcessRequest()
	{
		String request=ReadLine();
	}
	private void ProcessHeaders()
	{
		String s;
		if((s=ReadLine())!="\r\n")
		{
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
	[TODO]
	public override void Close()
	{
	/*TODO*/
		throw new NotImplementedException();
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

	[TODO]
	public string GetResponseHeader(string headerName)
	{
	/*TODO*/
		throw new NotImplementedException();
	}

	[TODO]
	public override Stream GetResponseStream()
	{
	/*TODO*/
		throw new NotImplementedException();
	}

	[TODO]
	public string CharacterSet 
	{ 
		get
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public string ContentEncoding 
	{ 
		get
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public override long ContentLength 
	{ 
		get
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public override string ContentType 
	{ 
		get
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public override WebHeaderCollection Headers 
	{
		get
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public DateTime LastModified 
	{ 
		get 
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public string Method 
	{ 
		get
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public Version ProtocolVersion 
	{ 
		get
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public override Uri ResponseUri 
	{ 
		get
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public string Server 
	{
		get
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public HttpStatusCode StatusCode 
	{ 
		get
		{
			throw new NotImplementedException();
		}
	}

	[TODO]
	public string StatusDescription 
	{ 
		get
		{
			throw new NotImplementedException();
		}
	}
}; // class HttpWebResponse

}; //namespace System.Net
