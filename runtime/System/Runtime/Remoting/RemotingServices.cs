/*
 * RemotingServices.cs - Implementation of the
 *			"System.Runtime.Remoting.RemotingServices" class.
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

namespace System.Runtime.Remoting
{

#if CONFIG_REMOTING

using System.Reflection;
using System.Runtime.Serialization;
using System.Runtime.Remoting.Messaging;
using System.Runtime.Remoting.Proxies;

public sealed class RemotingServices
{
	// This class cannot be instantiated.
	private RemotingServices() {}

	// Connect to a service and create a proxy object.
	public static Object Connect(Type classToProxy, String url)
			{
				return Connect(classToProxy, url, null);
			}
	[TODO]
	public static Object Connect(Type classToProxy, String url, Object data)
			{
				// TODO
				return null;
			}

	// Disconnect an object from all remote accesses.
	[TODO]
	public static bool Disconnect(MarshalByRefObject o)
			{
				// TODO
				return false;
			}

	// Execute a specific message.
	[TODO]
	public static IMethodReturnMessage ExecuteMessage
				(MarshalByRefObject target, IMethodCallMessage reqMsg)
			{
				// TODO
				return null;
			}

	// Get the envoy chain for a specific proxy object.
	[TODO]
	public static IMessageSink GetEnvoyChainForProxy
				(MarshalByRefObject obj)
			{
				// TODO
				return null;
			}

	// Get a lifetime service object.
	[TODO]
	public static Object GetLifetimeService(MarshalByRefObject obj)
			{
				// TODO
				return null;
			}

	// Get the method associated with a specific message.
	[TODO]
	public static MethodBase GetMethodBaseFromMethodMessage(IMethodMessage msg)
			{
				// TODO
				return null;
			}

	// Serialize an object.
	[TODO]
	public static void GetObjectData(Object obj, SerializationInfo info,
									 StreamingContext context)
			{
				// TODO
			}

	// Get the URI for a specific object.
	[TODO]
	public static String GetObjectUri(MarshalByRefObject obj)
			{
				// TODO
				return null;
			}

	// Get an object reference that represents an object proxy.
	[TODO]
	public static ObjRef GetObjRefForProxy(MarshalByRefObject obj)
			{
				// TODO
				return null;
			}

	// Get the real proxy underlying a transparent one.
	[TODO]
	public static RealProxy GetRealProxy(Object obj)
			{
				// TODO
				return null;
			}

	// Get the server type associated with a particular URI.
	[TODO]
	public static Type GetServerTypeForUri(String URI)
			{
				// TODO
				return null;
			}

	// Get the session ID from a particular message.
	[TODO]
	public static String GetSessionIdForMethodMessage(IMethodMessage msg)
			{
				// TODO
				return null;
			}

	// Determine if a method is overloaded.
	[TODO]
	public static bool IsMethodOverloaded(IMethodMessage msg)
			{
				// TODO
				return false;
			}

	// Determine if an object is outside the current application domain.
	[TODO]
	public static bool IsObjectOutOfAppDomain(Object tp)
			{
				// TODO
				return false;
			}

	// Determine if an object is outside the current context.
	[TODO]
	public static bool IsObjectOutOfContext(Object tp)
			{
				// TODO
				return false;
			}

	// Determine if a method is one-way.
	[TODO]
	public static bool IsOneWay(MethodBase method)
			{
				// TODO
				return false;
			}

	// Determine if an object is a transparent proxy.
	[TODO]
	public static bool IsTransparentProxy(Object proxy)
			{
				// TODO
				return false;
			}

	// Marshal an object.
	public static ObjRef Marshal(MarshalByRefObject Obj)
			{
				return Marshal(Obj, null, null);
			}
	public static ObjRef Marshal(MarshalByRefObject Obj, String URI)
			{
				return Marshal(Obj, URI, null);
			}
	[TODO]
	public static ObjRef Marshal(MarshalByRefObject Obj, String ObjURI,
								 Type RequestedType)
			{
				// TODO
				return null;
			}

	// Set the URI to use to marshal an object.
	[TODO]
	public static void SetObjectUriForMarshal
				(MarshalByRefObject obj, String uri)
			{
				// TODO
			}

	// Create a proxy for an object reference.
	public static Object Unmarshal(ObjRef objectRef)
			{
				return Unmarshal(objectRef, false);
			}
	[TODO]
	public static Object Unmarshal(ObjRef objectRef, bool fRefine)
			{
				// TODO
				return null;
			}

}; // class RemotingServices

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting
