/*
 * RealProxy.cs - Implementation of the
 *			"System.Runtime.Remoting.Proxies.RealProxy" class.
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

namespace System.Runtime.Remoting.Proxies
{

#if CONFIG_REMOTING

using System.Runtime.Serialization;
using System.Runtime.Remoting.Activation;
using System.Runtime.Remoting.Messaging;

public abstract class RealProxy
{
	// Internal state.
	private Type type;
	private Object stubData;

	// Constructor.
	[TODO]
	protected RealProxy()
			{
				// TODO
			}
	[TODO]
	protected RealProxy(Type classToProxy)
			{
				// TODO
			}
	[TODO]
	protected RealProxy(Type classToProxy, IntPtr stub, Object stubData)
			{
				// TODO
			}

	// Create an object reference of the specified type.
	[TODO]
	public virtual ObjRef CreateObjRef(Type requestedType)
			{
				// TODO
				return null;
			}

	// Get the unmanaged IUnknown proxy instance.
	[TODO]
	public virtual IntPtr GetCOMIUnknown(bool fIsMarshalled)
			{
				// TODO
				return IntPtr.Zero;
			}

	// Get the serialization data for this instance.
	[TODO]
	public virtual void GetObjectData(SerializationInfo info,
									  StreamingContext context)
			{
				// TODO
			}

	// Get the type being proxied by this instance.
	public virtual Type GetProxiedType()
			{
				return type;
			}

	// Get the stub data within a proxy.
	public static Object GetStubData(RealProxy rp)
			{
				if(rp != null)
				{
					return rp.stubData;
				}
				else
				{
					return null;
				}
			}

	// Get a transparent proxy for the current instance.
	[TODO]
	public virtual Object GetTransparentProxy()
			{
				// TODO
				return null;
			}

	// Initialize a server object.
	[TODO]
	public IConstructionReturnMessage InitializeServerObject
				(IConstructionCallMessage ctorMsg)
			{
				// TODO
				return null;
			}

	// Invoke a message on the object underlying this proxy.
	public abstract IMessage Invoke(IMessage msg);

	// Set the unmanaged IUnknown information for this object.
	[TODO]
	public virtual void SetCOMIUnknown(IntPtr i)
			{
				// TODO
			}

	// Set the stub data for a particular proxy.
	public static void SetStubData(RealProxy rp, Object stubData)
			{
				if(rp != null)
				{
					rp.stubData = stubData;
				}
			}

	// Get a COM interface for a particular Guid on this object.
	[TODO]
	public virtual IntPtr SupportsInterface(ref Guid iid)
			{
				// TODO
				return IntPtr.Zero;
			}

	// Attach this proxy to a remote server.
	[TODO]
	protected void AttachServer(MarshalByRefObject s)
			{
				// TODO
			}

	// Detach this proxy from a remote server.
	[TODO]
	protected MarshalByRefObject DetachServer()
			{
				// TODO
				return null;
			}

	// Get the unwrapped server instance for this proxy.
	[TODO]
	protected MarshalByRefObject GetUnwrappedServer()
			{
				// TODO
				return null;
			}

}; // class RealProxy

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Proxies
