/*
 * TcpListener.cs - Implementation of the
 * "System.Net.Sockets.TcpListener" class.
 *
 * Copyright (C) 2002  Francis Rogers.
 *
 * Contributed by Francis Rogers <franny644@comcast.net>
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

// this is a non-ECMA class
#if !ECMA_COMPAT
using System;
using System.Net;
using System.Net.Sockets;

namespace System.Net.Sockets
{

public class TcpListener
{
	private static Socket sock;
	private IPEndPoint endpoint;
	private bool listening;

	public System.Net.Sockets.Socket AcceptSocket()
	{
		return sock.Accept();
	}

	public System.Net.Sockets.TcpClient AcceptTcpClient()
	{
		Socket newsock = sock.Accept();
		TcpClient tcpc = new TcpClient(newsock);
		return tcpc;
	}

	public bool Pending()
	{
		return sock.Poll(0, SelectMode.SelectRead);
	}

	public TcpListener(int port)
	{
		sock = new Socket(AddressFamily.InterNetwork, 
				  SocketType.Stream, ProtocolType.Tcp);
		endpoint = new IPEndPoint(Dns.Resolve(Dns.GetHostName()).AddressList[0], port);
		sock.Bind(endpoint);
		listening = false;
	}

	public TcpListener(System.Net.IPAddress localaddr, int port)
	{
		sock = new Socket(AddressFamily.InterNetwork, 
				  SocketType.Stream, 
				  ProtocolType.Tcp);
		endpoint = new IPEndPoint(localaddr, port);
		sock.Bind(endpoint);
		listening = false;
	}

	public TcpListener(System.Net.IPEndPoint localEP)
	{
		sock = new Socket(AddressFamily.InterNetwork, 
				  SocketType.Stream, 
				  ProtocolType.Tcp);
		sock.Bind(localEP);
		listening = false;
	}

	public void Start()
	{
		sock.Listen((int)SocketOptionName.MaxConnections);
	}

	public void Stop()
	{
		sock.Close();
	}

	protected bool Active
	{
		get
		{
			return listening;
		}
	}

	public System.Net.EndPoint LocalEndpoint
	{
		get
		{
			return endpoint;
		}
	}

	protected System.Net.Sockets.Socket Server
	{
		get
		{
			return sock;
		}
	}
}

}
#endif // !ECMA_COMPAT
