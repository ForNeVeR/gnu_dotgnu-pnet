/*
 * ConfigurationException.cs - Implementation of the
 *		"System.Configuration.ConfigurationException" interface.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.Configuration
{

#if !ECMA_COMPAT

using System;
using System.Xml;
using System.Runtime.Serialization;

[Serializable]
public class ConfigurationException : SystemException
{
	// Internal state.
	private String filename;
	private int line;

	// Constructors.
	public ConfigurationException()
			{
				// Nothing to do here.
			}
	public ConfigurationException(String message)
			: base(message)
			{
				// Nothing to do here.
			}
	public ConfigurationException(String message, Exception inner)
			: base(message, inner)
			{
				// Nothing to do here.
			}
	public ConfigurationException(String message, String filename, int line)
			: base(message)
			{
				this.filename = filename;
				this.line = line;
			}
	public ConfigurationException(String message, Exception inner,
								  String filename, int line)
			: base(message, inner)
			{
				this.filename = filename;
				this.line = line;
			}
	[TODO]
	protected ConfigurationException(SerializationInfo info,
									 StreamingContext context)
			: base(info, context)
			{
				// TODO
			}

	// Get serialization data.
	[TODO]
	public override void GetObjectData(SerializationInfo info,
									   StreamingContext context)
			{
				// TODO
			}

	// Get the exception message, with line number information attached.
	public override String Message
			{
				get
				{
					String baseMsg = base.Message;
					if(filename != null && line != 0)
					{
						return baseMsg + " (" + filename + " line " +
										 line.ToString() + ")";
					}
					else if(filename != null)
					{
						return baseMsg + " (" + filename + ")";
					}
					else if(line != 0)
					{
						return baseMsg + " (line " + line.ToString() + ")";
					}
					else
					{
						return baseMsg;
					}
				}
			}

	// Get the bare message with no line number information.
	public String BareMessage
			{
				get
				{
					return base.Message;
				}
			}

	// Get the filename for the message.
	public String Filename
			{
				get
				{
					return filename;
				}
			}

	// Get the line number for the message.
	public int Line
			{
				get
				{
					return line;
				}
			}

#if SECOND_PASS

	// XmlNode-based constructors.
	public ConfigurationException(String message, XmlNode node)
			: this(message, GetXmlNodeFilename(node),
			       GetXmlNodeLineNumber(node))
			{
				// Nothing to do here.
			}
	public ConfigurationException
				(String message, Exception inner, XmlNode node)
			: this(message, inner, GetXmlNodeFilename(node),
			       GetXmlNodeLineNumber(node))
			{
				// Nothing to do here.
			}

	// Get the filename from an XmlNode object.
	public static String GetXmlNodeFilename(XmlNode node)
			{
				IConfigXmlNode cnode = (node as IConfigXmlNode);
				if(cnode != null)
				{
					return cnode.Filename;
				}
				else
				{
					return String.Empty;
				}
			}

	// Get the line number from an XmlNode object.
	public static int GetXmlNodeLineNumber(XmlNode node)
			{
				IConfigXmlNode cnode = (node as IConfigXmlNode);
				if(cnode != null)
				{
					return cnode.LineNumber;
				}
				else
				{
					return 0;
				}
			}

#endif // SECOND_PASS

}; // class ConfigurationException

#endif // !ECMA_COMPAT

}; // namespace System.Configuration
