/*
 * SoapFault.cs - Implementation of the
 *			"System.Runtime.Serialization.SoapFault" class.
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

namespace System.Runtime.Serialization.Formatters
{

#if CONFIG_REMOTING

using System.Runtime.Serialization;
using System.Runtime.Remoting.Metadata;

[Serializable]
[SoapType(Embedded=true)]
public sealed class SoapFault : ISerializable
{
	// Internal state.
	private String faultCode;
	private String faultString;
	private String faultActor;
	private Object serverFault;

	// Constructors.
	public SoapFault() {}
	public SoapFault(String faultCode, String faultString,
					 String faultActor, ServerFault serverFault)
			{
				this.faultCode = faultCode;
				this.faultString = faultString;
				this.faultActor = faultActor;
				this.serverFault = serverFault;
			}
	[TODO]
	internal SoapFault(SerializationInfo info, StreamingContext context)
			{
				// TODO
			}

	// Implement the ISerializable interface.
	[TODO]
	public void GetObjectData(SerializationInfo info, StreamingContext context)
			{
				// TODO
			}

	// Get or set the object properties.
	public String FaultActor
			{
				get
				{
					return faultActor;
				}
				set
				{
					faultActor = value;
				}
			}
	public String FaultCode
			{
				get
				{
					return faultCode;
				}
				set
				{
					faultCode = value;
				}
			}
	public String FaultString
			{
				get
				{
					return faultString;
				}
				set
				{
					faultString = value;
				}
			}
	public Object Detail
			{
				get
				{
					return serverFault;
				}
				set
				{
					serverFault = value;
				}
			}

}; // class SoapFault

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Serialization.Formatters
