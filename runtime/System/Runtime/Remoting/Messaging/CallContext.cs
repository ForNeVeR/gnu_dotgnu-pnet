/*
 * CallContext.cs - Implementation of the
 *			"System.Runtime.Remoting.Messaging.CallContext" class.
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

namespace System.Runtime.Remoting.Messaging
{

#if CONFIG_REMOTING

[Serializable]
public sealed class CallContext
{

	// We cannot instantiate this class.
	private CallContext() {}

	// Free the contents of a named data slot.
	[TODO]
	public static void FreeNamedDataSlot(String name)
			{
				// TODO
			}

	// Get the data in a particular named data slot.
	[TODO]
	public static Object GetData(String name)
			{
				// TODO
				return null;
			}

	// Get the headers that were sent with the method call.
	[TODO]
	public static Header[] GetHeaders()
			{
				// TODO
				return null;
			}

	// Set the contents of a named data slot.
	[TODO]
	public static void SetData(String name, Object value)
			{
				// TODO
			}

	// Set the headers to be sent along with the method call.
	[TODO]
	public static void SetHeaders(Header[] headers)
			{
				// TODO
			}

}; // class CallContext

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Messaging
