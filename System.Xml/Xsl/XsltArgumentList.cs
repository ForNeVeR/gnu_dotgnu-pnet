/*
 * XsltArgumentList.cs - Implementation of "System.Xml.Xsl.XsltArgumentList" 
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

using System;
namespace System.Xml.Xsl
{
	public sealed class XsltArgumentList
	{
		[TODO]
		public XsltArgumentList()
		{
			throw new NotImplementedException(".ctor");
		}

		[TODO]
		public void AddExtensionObject(String namespaceUri, Object extension)
		{
			throw new NotImplementedException("AddExtensionObject");
		}

		[TODO]
		public void AddParam(String name, String namespaceUri, Object parameter)
		{
			throw new NotImplementedException("AddParam");
		}

		[TODO]
		public void Clear()
		{
			throw new NotImplementedException("Clear");
		}

		[TODO]
		public Object GetExtensionObject(String namespaceUri)
		{
			throw new NotImplementedException("GetExtensionObject");
		}

		[TODO]
		public Object GetParam(String name, String namespaceUri)
		{
			throw new NotImplementedException("GetParam");
		}

		[TODO]
		public Object RemoveExtensionObject(String namespaceUri)
		{
			throw new NotImplementedException("RemoveExtensionObject");
		}

		[TODO]
		public Object RemoveParam(String name, String namespaceUri)
		{
			throw new NotImplementedException("RemoveParam");
		}

	}
}//namespace
#endif
