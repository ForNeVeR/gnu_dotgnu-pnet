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
						  IMethodMessage, IMessage, ISerializationRootObject
{
	// Internal state.
	protected IDictionary ExternalProperties;
	protected IDictionary InternalProperties;
	private Object[] args;
	private String methodName;
	private String typeName;
	private String uri;
	private bool hasVarArgs;
	private LogicalCallContext context;
	private MethodBase method;
	private ParameterInfo[] parameters;
	private Object srvID;
	private Type[] signature;

	// Constructors.
	public MethodCall(Header[] h1)
			{
				Init();
				if(h1 != null)
				{
					foreach(Header h in h1)
					{
						ProcessHeader(h.Name, h.Value);
					}
				}
				ResolveMethod();
				AccessCheck();
			}
	public MethodCall(IMessage msg)
			{
				if(msg == null)
				{
					throw new ArgumentNullException("msg");
				}
				Init();
				IDictionaryEnumerator e = msg.Properties.GetEnumerator();
				while(e.MoveNext())
				{
					ProcessHeader(e.Key.ToString(), e.Value);
				}
				ResolveMethod();
				AccessCheck();
			}
	internal MethodCall(SerializationInfo info, StreamingContext context)
			{
				if(info == null)
				{
					throw new ArgumentNullException("info");
				}
				Init();
				RootSetObjectData(info, context);
			}

	// Implement the IMethodCallMessage interface.
	[TODO]
	public virtual IDictionary Properties
			{
				get
				{
					if(InternalProperties == null)
					{
						InternalProperties = new Hashtable();
					}
					if(ExternalProperties == null)
					{
						// TODO: use a message dictionary
						ExternalProperties = new Hashtable();
					}
					return ExternalProperties;
				}
			}
	public int ArgCount
			{
				get
				{
					if(args != null)
					{
						return args.Length;
					}
					else
					{
						return 0;
					}
				}
			}
	public Object[] Args
			{
				get
				{
					return args;
				}
			}
	public bool HasVarArgs
			{
				get
				{
					return hasVarArgs;
				}
			}
	public LogicalCallContext LogicalCallContext
			{
				get
				{
					if(context == null)
					{
						context = new LogicalCallContext();
					}
					return context;
				}
			}
	public MethodBase MethodBase
			{
				get
				{
					return method;
				}
			}
	public String MethodName
			{
				get
				{
					return methodName;
				}
			}
	public Object MethodSignature
			{
				get
				{
					if(signature == null)
					{
						FetchParameters();
						if(parameters != null)
						{
							signature = new Type [parameters.Length];
							int posn;
							for(posn = 0; posn < signature.Length; ++posn)
							{
								signature[posn] =
									parameters[posn].ParameterType;
							}
						}
					}
					return signature;
				}
			}
	public String TypeName
			{
				get
				{
					return typeName;
				}
			}
	public String Uri
			{
				get
				{
					return uri;
				}
				set
				{
					uri = value;
				}
			}
	public int InArgCount
			{
				get
				{
					FetchParameters();
					int count = 0;
					if(parameters != null)
					{
						foreach(ParameterInfo p in parameters)
						{
							if(!(p.ParameterType.IsByRef))
							{
								++count;
							}
						}
					}
					return count;
				}
			}
	public Object[] InArgs
			{
				get
				{
					int count = InArgCount;
					Object[] inArgs = new Object [count];
					if(parameters != null && args != null)
					{
						int posn;
						count = 0;
						for(posn = 0; posn < args.Length; ++posn)
						{
							if(!(parameters[posn].ParameterType.IsByRef))
							{
								inArgs[count++] = args[posn];
							}
						}
					}
					return inArgs;
				}
			}
	public Object GetArg(int argNum)
			{
				return args[argNum];
			}
	public String GetArgName(int index)
			{
				FetchParameters();
				return parameters[index].Name;
			}
	public Object GetInArg(int argNum)
			{
				FetchParameters();
				if(parameters != null && args != null)
				{
					int posn;
					for(posn = 0; posn < args.Length; ++posn)
					{
						if(!(parameters[posn].ParameterType.IsByRef))
						{
							if(argNum == 0)
							{
								return args[posn];
							}
							--argNum;
						}
					}
				}
				throw new IndexOutOfRangeException(_("Arg_InvalidArrayIndex"));
			}
	public String GetInArgName(int index)
			{
				FetchParameters();
				if(parameters != null)
				{
					int posn;
					for(posn = 0; posn < args.Length; ++posn)
					{
						if(!(parameters[posn].ParameterType.IsByRef))
						{
							if(index == 0)
							{
								return parameters[posn].Name;
							}
							--index;
						}
					}
				}
				throw new IndexOutOfRangeException(_("Arg_InvalidArrayIndex"));
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
	internal void SetServerIdentity(Object srvID)
			{
				this.srvID = srvID;
			}

	// Process a header.
	[TODO]
	private void ProcessHeader(String name, Object value)
			{
				// TODO
			}

	// Perform an access check on the resolved method.
	[TODO]
	private void AccessCheck()
			{
				// TODO
			}

	// Fetch the parameter information from the method block.
	private void FetchParameters()
			{
				if(parameters == null && method != null)
				{
					parameters = method.GetParameters();
				}
			}

}; // class MethodCall

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Messaging
