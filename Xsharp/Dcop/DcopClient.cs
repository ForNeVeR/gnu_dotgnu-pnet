/*
 * DcopClient.cs - ICE client for DCOP requests.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace Xsharp.Dcop
{

using System;
using System.IO;
using System.Diagnostics;
using Xsharp.Ice;

/// <summary>
/// <para>The <see cref="T:Xsharp.Dcop.DcopClient"/> class manages the client
/// side of a DCOP connection.</para>
/// </summary>
public sealed class DcopClient : IceClient
{
	// Internal state.
	private String appId;

	/// <summary>
	/// <para>Construct a DCOP client handler to process DCOP messages.</para>
	/// </summary>
	///
	/// <param name="dpy">
	/// <para>The display to attach to the DCOP connection's message
	/// processor.</para>
	/// </param>
	///
	/// <param name="registerAs">
	/// <para>The name of the application to register with DCOP, or
	/// <see langword="null"/> to register anonymously.</para>
	/// </param>
	///
	/// <param name="addPID">
	/// <para>Set to <see langword="true"/> to add the process ID to
	/// the registered name.</para>
	/// </param>
	///
	/// <exception cref="T:Xsharp.XInvalidOperationException">
	/// <para>Raised if the connection to the DCOP server could not be
	/// established.</para>
	/// </exception>
	public DcopClient(Display dpy, String registerAs, bool addPID)
			: base(dpy, "DCOP", "KDE", "2.0", 2, 0, GetDcopServer(dpy))
			{
				// This type of ICE client uses DCOP key values.
				HasDcopKey = true;

				// Construct the full registration name for the DCOP client.
				if(registerAs == null)
				{
					registerAs = "anonymous";
				}
			#if CONFIG_EXTENDED_DIAGNOSTICS
				if(addPID)
				{
					int pid = Process.GetCurrentProcess().Id;
					if(pid != -1 && pid != 0)
					{
						registerAs += "-" + pid.ToString();
					}
				}
			#endif

				// Register the name with the DCOP server.
				DcopReply reply;
				reply = Call("DCOPServer", "", "registerAs(QCString)",
				             registerAs);
				if(reply != null)
				{
					appId = reply.ReadString();
				}
				else
				{
					appId = registerAs;
				}
			}

	/// <summary>
	/// <para>Construct a DCOP client handler to process DCOP messages.</para>
	/// </summary>
	///
	/// <param name="dpy">
	/// <para>The display to attach to the DCOP connection's message
	/// processor.</para>
	/// </param>
	///
	/// <param name="registerAs">
	/// <para>The name of the application to register with DCOP, or
	/// <see langword="null"/> to register anonymously.  The process ID
	/// will be automatically added to the name.</para>
	/// </param>
	///
	/// <exception cref="T:Xsharp.XInvalidOperationException">
	/// <para>Raised if the connection to the DCOP server could not be
	/// established.</para>
	/// </exception>
	public DcopClient(Display dpy, String registerAs)
			: this(dpy, registerAs, true) {}

	// Get the address of the DCOP server.  Returns null if not found.
	private static String GetDcopServer(Display dpy)
			{
				String value;

				// Bail out if the display is null.
				if(dpy == null)
				{
					throw new ArgumentNullException("dpy");
				}

				// Try the DCOPSERVER environment variable first.
				value = Environment.GetEnvironmentVariable("DCOPSERVER");
				if(value != null && value.Length > 0)
				{
					return value;
				}

				// Locate the DCOP authority file.
				value = Environment.GetEnvironmentVariable("DCOPAUTHORITY");
				if(value == null || value.Length == 0)
				{
					// We need the "HOME" environment variable.
					String home = Environment.GetEnvironmentVariable("HOME");
					if(home == null || home.Length == 0)
					{
						throw new XInvalidOperationException();
					}

					// Get the name of the display, without the screen number.
					String displayName = dpy.displayName;
					int index1 = displayName.LastIndexOf('.');
					int index2 = displayName.LastIndexOf(':');
					if(index1 >= 0 && index1 > index2)
					{
						displayName = displayName.Substring(0, index1);
					}
					displayName = displayName.Replace(":", "_");

					// Get the name of the local host.
					String hostname = Application.Hostname;
					if(hostname == null)
					{
						hostname = "localhost";
					}

					// Build the full name of the authority file.
					value = home + "/.DCOPserver_" + hostname + "_" +
							displayName;
				}

				// Bail out if the authority file does not exist.
				if(!File.Exists(value))
				{
					throw new XInvalidOperationException();
				}

				// Read the first line from the authority file.
				try
				{
					StreamReader reader = new StreamReader(value);
					value = reader.ReadLine();
					reader.Close();
					if(value != null && value.Length != 0)
					{
						return value;
					}
					else
					{
						throw new XInvalidOperationException();
					}
				}
				catch(Exception)
				{
					throw new XInvalidOperationException();
				}
			}

	/// <summary>
	/// <para>Get the application identifier for this DCOP client.</para>
	/// </summary>
	///
	/// <value>
	/// <para>Returns the application identifier.</para>
	/// </value>
	public String AppId
			{
				get
				{
					return appId;
				}
			}

	/// <summary>
	/// <para>Process a message that was received by this ICE client.</para>
	/// </summary>
	///
	/// <param name="opcode">
	/// <para>The minor opcode for the message.</para>
	/// </param>
	///
	/// <param name="key">
	/// <para>The DCOP key for the message, or -1 if not DCOP.</para>
	/// </param>
	///
	/// <param name="buffer">
	/// <para>A buffer containing the body of the message.</para>
	/// </param>
	///
	/// <param name="length">
	/// <para>The length of the data in the message buffer.  The data may
	/// not occupy the full length of <paramref name="buffer"/>.</para>
	/// </param>
	protected override void ProcessMessage
				(int opcode, int key, byte[] buffer, int length)
			{
				// TODO
			}

	public DcopReply Call(String remoteApp, String remoteObject,
					      String remoteFunction, params Object[] parameters)
			{
				MemoryStream mem;
				QDataStream ds;
				byte[] data;

				// Pack up the parameters.
				mem = new MemoryStream();
				ds = new QDataStream(mem);
				if(parameters != null)
				{
					foreach(Object obj in parameters)
					{
						ds.Write(obj);
					}
				}
				data = mem.ToArray();

				// Pack up the full call details, including the parameters.
				mem = new MemoryStream();
				ds = new QDataStream(mem);
				ds.Write(appId);
				ds.Write(remoteApp);
				ds.Write(remoteObject);
				ds.Write(remoteFunction);
				ds.Write(data.Length);
				ds.WriteRaw(data, 0, data.Length);
				data = mem.ToArray();

				return null;
			}

}; // class DcopClient

}; // namespace Xsharp.Dcop
