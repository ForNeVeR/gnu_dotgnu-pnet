/*
 * IceClient.cs - Client handler for ICE connections.
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

namespace Xsharp.Ice
{

using System;
using OpenSystem.Platform.X11;

/// <summary>
/// <para>The <see cref="T:Xsharp.Ice.IceClient"/> class manages the client
/// side of an ICE connection for a specific protocol.</para>
/// </summary>
public abstract unsafe class IceClient
{
	// Internal state.
	private Display dpy;
	private int majorOpcode;
	private IceConn *iceConn;
	private bool hasDcopKey;
	private byte[] buffer;

	/// <summary>
	/// <para>Construct an ICE client handler to process messages for
	/// a particular ICE protocol.</para>
	/// </summary>
	///
	/// <param name="dpy">
	/// <para>The display to attach to the ICE connection's message
	/// processor.</para>
	/// </param>
	///
	/// <param name="protocolName">
	/// <para>The name of the protocol to register.</para>
	/// </param>
	///
	/// <param name="vendor">
	/// <para>The name of the vendor for the protocol being registered.</para>
	/// </param>
	///
	/// <param name="release">
	/// <para>The name of the release for the protocol being registered.</para>
	/// </param>
	///
	/// <param name="majorVersion">
	/// <para>The major vesion number for the protocol being registered.</para>
	/// </param>
	///
	/// <param name="minorVersion">
	/// <para>The minor vesion number for the protocol being registered.</para>
	/// </param>
	///
	/// <param name="serverAddress">
	/// <para>The address of the ICE server to connect to.</para>
	/// </param>
	///
	/// <exception cref="T:System.ArgumentNullException">
	/// <para>Raised if <paramref name="protocolName"/>,
	/// <paramref name="vendor"/>, <paramref name="release"/>, or
	/// <paramref name="serverAddress"/> is <see langword="null"/>.
	/// </para>
	/// </exception>
	///
	/// <exception cref="T:Xsharp.XInvalidOperationException">
	/// <para>Raised if the protocol could not be registered or the
	/// connection to the ICE server could not be established.</para>
	/// </exception>
	public IceClient(Display dpy, String protocolName, String vendor,
					 String release, int majorVersion, int minorVersion,
					 String serverAddress)
			{
				// Validate the parameters.
				if(dpy == null)
				{
					throw new ArgumentNullException("dpy");
				}
				if(protocolName == null)
				{
					throw new ArgumentNullException("protocolName");
				}
				if(vendor == null)
				{
					throw new ArgumentNullException("vendor");
				}
				if(release == null)
				{
					throw new ArgumentNullException("release");
				}
				if(serverAddress == null)
				{
					throw new ArgumentNullException("serverAddress");
				}

				// Register the protocol with "libICE".
				IcePoVersionRec version = new IcePoVersionRec();
				version.major_version = majorVersion;
				version.minor_version = minorVersion;
				version.process_msg_proc =
					new IcePoProcessMsgProc(ProcessMessage);
				String[] authNames = new String[] {"MIT-MAGIC-COOKIE-1"};
				IcePoAuthProc authProc =
					new IcePoAuthProc(ICE._IcePoMagicCookie1Proc);
				majorOpcode = (int)ICE.IceRegisterForProtocolSetup
					(protocolName, vendor, release,
					 (Xlib.Xint)1, ref version,
					 (Xlib.Xint)1, authNames, ref authProc, null);
				if(majorOpcode < 0)
				{
					throw new XInvalidOperationException();
				}

				// Open the ICE connection to the server.
				byte[] errorBuffer = new byte [1024];
				iceConn = ICE.IceOpenConnection
					(serverAddress, IntPtr.Zero, XBool.False,
					 (Xlib.Xint)majorOpcode, (Xlib.Xint)1024, errorBuffer);
				if(iceConn == null)
				{
					throw new XInvalidOperationException();
				}

				// We don't want shutdown negotiation.
				ICE.IceSetShutdownNegotiation(iceConn, XBool.False);

				// Perform protocol setup on the connection.
				IceProtocolSetupStatus status;
				Xlib.Xint majorRet, minorRet;
				IntPtr vendorRet, releaseRet;
				status = (IceProtocolSetupStatus)ICE.IceProtocolSetup
					(iceConn, (Xlib.Xint)majorOpcode, IntPtr.Zero,
					 XBool.False, out majorRet, out minorRet,
					 out vendorRet, out releaseRet,
					 (Xlib.Xint)1024, errorBuffer);
				if(status != IceProtocolSetupStatus.IceProtocolSetupSuccess)
				{
					ICE.IceCloseConnection(iceConn);
					iceConn = null;
					throw new XInvalidOperationException();
				}

				// Check the connection status.
				if(ICE.IceConnectionStatus(iceConn) !=
						(Xlib.Xint)(IceConnectStatus.IceConnectAccepted))
				{
					ICE.IceCloseConnection(iceConn);
					iceConn = null;
					throw new XInvalidOperationException();
				}

				// Initialize other state information.
				this.hasDcopKey = false;
				this.buffer = errorBuffer;
				this.dpy = dpy;

				// TODO: register IceConnectionNumber(iceConn) with
				// the select loop.
			}

	/// <summary>
	/// <para>Close the ICE client if it is currently active.</para>
	/// </summary>
	~IceClient()
			{
				Close();
			}

	/// <summary>
	/// <para>Get the major opcode that is being used for ICE requests.</para>
	/// </summary>
	///
	/// <value>
	/// <para>Returns the major opcode value.</para>
	/// </value>
	public int MajorOpcode
			{
				get
				{
					return majorOpcode;
				}
			}

	/// <summary>
	/// <para>Determine if this ICE protocol uses DCOP key values.</para>
	/// </summary>
	///
	/// <value>
	/// <para>Returns <see langword="true"/> if this ICE protocol uses
	/// DCOP key values.</para>
	/// </value>
	///
	/// <remarks>
	/// <para>DCOP keys are 32-bit values that are appended to ICE message
	/// headers to associate requests and replies a single client.</para>
	/// </remarks>
	public bool HasDcopKey
			{
				get
				{
					return hasDcopKey;
				}
				set
				{
					hasDcopKey = value;
				}
			}

	/// <summary>
	/// <para>Close this ICE client connection.</para>
	/// </summary>
	public virtual void Close()
			{
				if(iceConn != null)
				{
					// TODO: deregister the fd from the select loop.
					ICE.IceProtocolShutdown
						(iceConn, (Xlib.Xint)majorOpcode);
					ICE.IceCloseConnection(iceConn);
					iceConn = null;
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
	protected abstract void ProcessMessage
				(int opcode, int key, byte[] buffer, int length);

	// Callback from "libICE" to process a message.
	private void ProcessMessage
				(IntPtr iceConn, IntPtr clientData, Xlib.Xint opcode,
				 Xlib.Xulong length, XBool swap,
				 ref IceReplyWaitInfo replyWait, ref XBool replyReadySet)
			{
				int key, len;

				// Read the DCOP key, if necessary.
				if(hasDcopKey)
				{
					key = ICE.IceReadKey(this.iceConn, buffer);
				}
				else
				{
					key = -1;
				}

				// Read the body of the message.
				len = (int)length;
				if(len < 0)
				{
					len = 0;
				}
				if(len > buffer.Length)
				{
					buffer = new byte [(len + 1023) & ~1023];
				}
				if(!ICE.IceReadData(this.iceConn, len, buffer))
				{
					len = 0;
				}

				// Process the message.
				ProcessMessage((int)opcode, key, buffer, len);
			}

	// Process incoming messages on the ICE connection.
	internal void ProcessIncoming()
			{
				if(iceConn != null)
				{
					XBool replyReady;
					if(ICE.IceProcessMessages
							(iceConn, IntPtr.Zero, out replyReady)
						 != (Xlib.Xint)(IceProcessMessagesStatus.
						 					IceProcessMessagesSuccess))
					{
						Close();
					}
				}
			}

	/// <summary>
	/// <para>Send a message to the ICE server.</para>
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
	protected void SendMessage(int opcode, int key, byte[] buffer, int length)
			{
				if(iceConn != null)
				{
					if(length > 0)
					{
						if(length > buffer.Length)
						{
							length = buffer.Length;
						}
						ICE.IceSendHeader(iceConn, majorOpcode, opcode,
										  length, key, buffer);
						ICE.IceSendData(iceConn, length, buffer);
						ICE.IceFlush(iceConn);
					}
				}
			}

} // class IceClient

} // namespace Xsharp.Ice
