/*
 * ConstructionCall.cs - Implementation of the
 *			"System.Runtime.Remoting.Messaging.ConstructionCall" class.
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
using System.Runtime.Remoting.Activation;

[Serializable]
[CLSCompliant(false)]
public class ConstructionCall : MethodCall, IConstructionCallMessage,
								IMethodMessage, IMessage
{
	// Internal state.
	private IActivator activator;
	private Type activationType;
	private String activationTypeName;
	private Object[] attributes;
	private IList contextProperties;

	// Constructors.
	public ConstructionCall(Header[] h1) : base(h1) {}
	public ConstructionCall(IMessage msg) : base(msg) {}
	internal ConstructionCall(SerializationInfo info,
							  StreamingContext context)
			: base(info, context) {}

	// Implement the IConstructionCallMessage interface.
	[TODO]
	public Type ActivationType
			{
				get
				{
					if(activationType == null && activationTypeName != null)
					{
						// TODO: resolve the type from its name
					}
					return activationType;
				}
			}
	public String ActivationTypeName
			{
				get
				{
					return activationTypeName;
				}
			}
	public IActivator Activator
			{
				get
				{
					return activator;
				}
				set
				{
					activator = value;
				}
			}
	public Object[] CallSiteActivationAttributes
			{
				get
				{
					return attributes;
				}
			}
	public IList ContextProperties
			{
				get
				{
					if(contextProperties == null)
					{
						contextProperties = new ArrayList();
					}
					return contextProperties;
				}
			}

	// Override the parent properties.
	[TODO]
	public override IDictionary Properties
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

}; // class ConstructionCall

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting.Messaging
