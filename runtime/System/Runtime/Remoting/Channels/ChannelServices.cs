/*
 * ChannelServices.cs - Implementation of the
 *			"System.Runtime.Remoting.Channels.ChannelServices" class.
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

namespace System.Runtime.Remoting.Channels
{

#if CONFIG_REMOTING

using System.Collections;
using System.Runtime.Remoting.Messaging;

public sealed class ChannelServices
{
	// This class cannot be instantiated.
	private ChannelServices() {}

	// Get the registered channels.
	[TODO]
	public static IChannel[] RegisteredChannels
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Asynchronously dispatch a message.
	[TODO]
	public static IMessageCtrl AsyncDispatchMessage
				(IMessage msg, IMessageSink replySink)
			{
				if(msg == null)
				{
					throw new ArgumentNullException("msg");
				}
				// TODO
				return null;
			}

	// Create a server channel sink chain.
	[TODO]
	public static IServerChannelSink CreateServerChannelSinkChain
				(IServerChannelSinkProvider provider, IChannelReceiver channel)
			{
				// TODO
				return null;
			}

	// Dispatch in incoming message.
	[TODO]
	public static ServerProcessing DispatchMessage
				(IServerChannelSinkStack sinkStack, IMessage msg,
		    	 out IMessage replyMsg)
			{
				if(msg == null)
				{
					throw new ArgumentNullException("msg");
				}
				// TODO
				replyMsg = null;
				return ServerProcessing.Complete;
			}

	// Get a registered channel.
	[TODO]
	public static IChannel GetChannel(String name)
			{
				// TODO
				return null;
			}

	// Get the sink properties for a channel.
	[TODO]
	public static IDictionary GetChannelSinkProperties(Object obj)
			{
				// TODO
				return null;
			}

	// Get the URL's that can be used to reach an object.
	[TODO]
	public static String[] GetUrlsForObject(MarshalByRefObject obj)
			{
				// TODO
				return null;
			}

	// Register a channel.
	[TODO]
	public static void RegisterChannel(IChannel chnl)
			{
				if(chnl == null)
				{
					throw new ArgumentNullException("chnl");
				}
				// TODO
			}

	// Synchronously dispatch a message.
	[TODO]
	public static IMessage SyncDispatchMessage(IMessage msg)
			{
				if(msg == null)
				{
					throw new ArgumentNullException("msg");
				}
				// TODO
				return null;
			}

	// Unregister a channel.
	[TODO]
	public static void UnregisterChannel(IChannel chnl)
			{
				if(chnl == null)
				{
					throw new ArgumentNullException("chnl");
				}
				// TODO
			}

}; // class ChannelServices

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Channels
