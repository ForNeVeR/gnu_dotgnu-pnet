/*
 * ClientChannelSinkStack.cs - Implementation of the
 *			"System.Runtime.Remoting.Channels.ClientChannelSinkStack" class.
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

using System.IO;
using System.Runtime.Remoting.Messaging;

public class ClientChannelSinkStack
	: IClientChannelSinkStack, IClientResponseChannelSinkStack
{
	// Constructors.
	[TODO]
	public ClientChannelSinkStack()
			{
				// TODO
			}
	[TODO]
	public ClientChannelSinkStack(IMessageSink replySink)
			{
				// TODO
			}

	// Pop an item from the stack.
	[TODO]
	public Object Pop(IClientChannelSink sink)
			{
				// TODO
				return null;
			}

	// Push an item onto the stack.
	[TODO]
	public void Push(IClientChannelSink sink, Object state)
			{
				// TODO
			}

	// Request asynchronous processing of a response.
	[TODO]
	public void AsyncProcessResponse(ITransportHeaders headers, Stream stream)
			{
				// TODO
			}

	// Dispatch an exception on the reply sink.
	[TODO]
	public void DispatchException(Exception e)
			{
				// TODO
			}

	// Dispatch a reply message on the reply sink.
	[TODO]
	public void DispatchReplyMessage(IMessage msg)
			{
				// TODO
			}

}; // class ClientChannelSinkStack

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Channels
