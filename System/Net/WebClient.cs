/*
 * WebClient.cs - Implementation of "WebClient" class 
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * 
 * Contributed by Gopal.V 
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

using System;
using System.IO;
using System.ComponentModel;
using System.Collections.Specialized;

namespace System.Net
{
#if !ECMA_COMPAT
	public sealed class WebClient: Component
	{
		private WebRequest webrequest=null;
		private WebResponse webresponse=null;

		private void WriteToStream(Stream inStream, Stream outStream)
		{
			byte[] buf = new byte [4096];
			int read;
			while((read=inStream.Read(buf, 0 , 4096)) > 0)
			{
				outStream.Write(buf,0,read);
			}
		}
		
		public byte[] DownloadData(String address)
		{
			Stream stream = OpenRead(address);
			MemoryStream memory = new MemoryStream();
			WriteToStream(stream, memory);
			return memory.GetBuffer();
		}

		public void DownloadFile(String address, String fileName)
		{
			Stream stream = OpenRead(address);
			FileStream file = File.OpenWrite(fileName);
			WriteToStream(stream, file);
		}


		public Stream OpenRead(String address)
		{
			Uri uri= new Uri(address);
			webrequest = WebRequest.CreateDefault(uri);	
			webresponse = webrequest.GetResponse();
			return webresponse.GetResponseStream();
		}

		[TODO]
		public Stream OpenWrite(String address)
		{
			// POST ?
			throw new NotImplementedException("OpenWrite");
		}

		[TODO]
		public Stream OpenWrite(String address, String method)
		{
			throw new NotImplementedException("OpenWrite");
		}

		[TODO]
		public byte[] UploadData(String address, byte[] data)
		{
			throw new NotImplementedException("UploadData");
		}

		[TODO]
		public byte[] UploadData(String address, String method, byte[] data)
		{
			throw new NotImplementedException("UploadData");
		}

		[TODO]
		public byte[] UploadFile(String address, String fileName)
		{
			throw new NotImplementedException("UploadFile");
		}

		[TODO]
		public byte[] UploadFile(String address, String method, String fileName)
		{
			throw new NotImplementedException("UploadFile");
		}

		[TODO]
		public byte[] UploadValues(String address, NameValueCollection data)
		{
			throw new NotImplementedException("UploadValues");
		}

		[TODO]
		public byte[] UploadValues(String address, String method, NameValueCollection data)
		{
			throw new NotImplementedException("UploadValues");
		}

		[TODO]
		public String BaseAddress 
		{
 			get
			{
				throw new NotImplementedException("BaseAddress");
			}
			set
			{
				throw new NotImplementedException("BaseAddress");
			}
		}

		[TODO]
		public ICredentials Credentials 
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

		public WebHeaderCollection Headers 
		{
 			get
			{
				if(webrequest!=null)
				{
					return webrequest.Headers;
				}
				throw new WebException("Headers not available");
			}
			set
			{
				if(webrequest!=null)
				{
					webrequest.Headers = value;
				}
				throw new WebException("Headers not available");
			}
		}

		[TODO]
		public NameValueCollection QueryString 
		{
 			get
			{
				throw new NotImplementedException("QueryString");
			}
			set
			{
				throw new NotImplementedException("QueryString");
			}
		}

		public WebHeaderCollection ResponseHeaders 
		{
 			get
			{
				if(webresponse!=null)
				{
					return webresponse.Headers;
				}
				throw new WebException("Headers not available");
			}
		}
	}
#endif
}//namespace
