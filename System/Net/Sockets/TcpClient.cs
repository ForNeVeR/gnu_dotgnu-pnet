/*
 * TcpClient.cs - Implementation of the "System.Net.Sockets.TcpClient" class.
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

public class TcpClient
{
	static Socket sock;
	static NetworkStream stream;
	public void Connect(System.Net.IPAddress address, int port)
	{
		IPEndPoint remoteEP = new IPEndPoint(address, port);
		sock.Connect(remoteEP);
		stream = new NetworkStream(sock);
	}

	public void Connect(string hostname, int port)
	{
		IPHostEntry host = Dns.GetHostByName(hostname);
		IPEndPoint remoteEP = new IPEndPoint(host.AddressList[0], port);
		sock.Connect(remoteEP);
		stream = new NetworkStream(sock);
	}

	public void Connect(System.Net.IPEndPoint remoteEP)
	{
		sock.Connect(remoteEP);
	}

	public void Close()
	{
		stream.Close();
		sock.Close();
	}

	protected virtual void Dispose(bool disposing)
	{
		// TODO: Implement dispose function.
	}

	~TcpClient()
	{
		// TODO: Implement finalize function.
	}

	public System.Net.Sockets.NetworkStream GetStream()
	{
		return stream;
	}

	public TcpClient(string hostname, int port)
	{
		sock = new Socket(AddressFamily.InterNetwork,
				  SocketType.Stream,
				  ProtocolType.Tcp);
		IPHostEntry host = Dns.GetHostByName(hostname);
		IPEndPoint remoteEP = new IPEndPoint(host.AddressList[0], port);
		sock.Connect(remoteEP);
		stream = new NetworkStream(sock);
	}

	public TcpClient()
	{
		sock = new Socket(AddressFamily.InterNetwork,
				  SocketType.Stream,
				  ProtocolType.Tcp);
	}

	public TcpClient(System.Net.IPEndPoint localEP)
	{
		sock = new Socket(AddressFamily.InterNetwork,
				  SocketType.Stream,
				  ProtocolType.Tcp);
		sock.Bind(localEP);
	}

	protected bool Active 
	{
		get
		{
			return sock.Connected;
		}
	}

	protected System.Net.Sockets.Socket Client 
	{
		get
		{
			return sock;
		}
	}

	// TODO: Implement LingerState, NoDelay, RecieveBufferSize, 
	// TODO: RecieveTimeout, SendBufferSize, and SendTimeout 
	// TODO: properties.

}

}
#endif // !ECMA_COMPAT
