/*
 * XsltException.cs - Implementation of "System.Xml.Xsl.XsltException" 
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * 
 * Contributed by Gopal.V 
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

#if !ECMA_COMPAT
#if CONFIG_XSL

using System;
using System.Runtime.Serialization;

namespace System.Xml.Xsl
{
	public class XsltException: System.SystemException
	{
		[TODO]
		public XsltException(String message, Exception innerException)
		{
			throw new NotImplementedException(".ctor");
		}

#if CONFIG_SERIALIZATION

		[TODO]
		protected XsltException(SerializationInfo info, 
								StreamingContext context)
		{
			throw new NotImplementedException(".ctor");
		}

		[TODO]
		public override void GetObjectData(SerializationInfo info, 
											StreamingContext context)
		{
			throw new NotImplementedException("GetObjectData");
		}

#endif

		[TODO]
		public int LineNumber 
		{
 			get
			{
				throw new NotImplementedException("LineNumber");
			}

 		}

		[TODO]
		public int LinePosition 
		{
 			get
			{
				throw new NotImplementedException("LinePosition");
			}

 		}

		[TODO]
		public override String Message 
		{
 			get
			{
				throw new NotImplementedException("Message");
			}

 		}

		[TODO]
		public String SourceUri 
		{
 			get
			{
				throw new NotImplementedException("SourceUri");
			}

 		}

	}
}//namespace
#endif /* CONFIG_XSL */
#endif /* !ECMA_COMPAT */
