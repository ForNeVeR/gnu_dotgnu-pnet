/*
 * XPathExpressionBase.cs - base class for compiled XPathExpressions
 *
 * Copyright (C) 2004  Southern Storm Software, Pty Ltd.
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

namespace System.Xml.XPath.Private
{

using System;
using System.Xml;
using System.Xml.XPath;
using System.Collections;

internal abstract class XPathExpressionBase : XPathExpression
{
	private XPathResultType resultType = XPathResultType.Error;

	public override void AddSort(Object expr, IComparer comparer){}

	public override void AddSort(Object expr, XmlSortOrder order, 
								XmlCaseOrder caseOrder, 
								String lang, XmlDataType dataType){}
	
	public override XPathExpression Clone()
	{
		return null; 
	}

	public override void SetContext(XmlNamespaceManager nsManager){}

	public override String Expression
	{
		get
		{
			return "";
		}
	}

	public override XPathResultType ReturnType
	{
		get
		{
			if(resultType == XPathResultType.Error)
			{
				if(this is Expression)
				{
					return (this as Expression).Compile();
				}
			}
			return resultType;
		}
	}

	internal Object Evaluate(XPathNodeIterator iterator)
	{
		if(this is Expression)
		{
			return ((Expression)this).EvaluateInternal(iterator);
		}
		throw new Exception("Unknown type to Eval:" + this.GetType());
	}

}
}//namespace
