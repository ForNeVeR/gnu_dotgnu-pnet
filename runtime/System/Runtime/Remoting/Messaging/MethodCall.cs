/*
 * MethodCall.cs - Implementation of the
 *			"System.Runtime.Remoting.Messaging.MethodCall" class.
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

using System.Collections;
using System.Reflection;
using System.Runtime.Serialization;

[Serializable]
[CLSCompliant(false)]
public class MethodCall : IMethodCallMessage, ISerializable,
						  IMethodMessage, IMessage
{
	// Internal state.
	protected IDictionary ExternalProperties;
	protected IDictionary InternalProperties;

	// Constructors.
	[TODO]
	public MethodCall(Header[] h1)
			{
				// TODO
			}
	[TODO]
	public MethodCall(IMessage msg)
			{
				// TODO
			}

	// Implement the IMethodCallMessage interface.
	[TODO]
	public virtual IDictionary Properties
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public int ArgCount
			{
				get
				{
					// TODO
					return 0;
				}
			}
	[TODO]
	public Object[] Args
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public bool HasVarArgs
			{
				get
				{
					// TODO
					return false;
				}
			}
	[TODO]
	public LogicalCallContext LogicalCallContext
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public MethodBase MethodBase
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public String MethodName
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public Object MethodSignature
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public String TypeName
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public String Uri
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public int InArgCount
			{
				get
				{
					// TODO
					return 0;
				}
			}
	[TODO]
	public Object[] InArgs
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public Object GetArg(int argNum)
			{
				// TODO
				return null;
			}
	[TODO]
	public String GetArgName(int index)
			{
				// TODO
				return null;
			}
	[TODO]
	public Object GetInArg(int argNum)
			{
				// TODO
				return null;
			}
	[TODO]
	public String GetInArgName(int index)
			{
				// TODO
				return null;
			}

	// Implement the ISerializable interface.
	[TODO]
	public void GetObjectData(SerializationInfo info,
							  StreamingContext context)
			{
				// TODO
			}

	// Handle incoming headers.
	[TODO]
	public virtual Object HeaderHandler(Header[] h)
			{
				// TODO
				return null;
			}

	// Initialization helper.
	public virtual void Init()
			{
				// Nothing to do here.
			}

	// Resolve the method.
	[TODO]
	public void ResolveMethod()
			{
				// TODO
			}

	// Set the root object data for this method call.
	[TODO]
	public void RootSetObjectData(SerializationInfo info,
								  StreamingContext context)
			{
				if(info == null)
				{
					throw new ArgumentNullException("info");
				}
				// TODO
			}

	// Set the server identity within this object.
	[TODO]
	internal void SetServerIdentity(Object srvID)
			{
				// TODO
			}

}; // class MethodCall

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Messaging
