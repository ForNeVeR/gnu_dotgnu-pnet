namespace System.Xml.Private
{

using System;
using System.Xml;
using System.Xml.XPath;


internal class XmlDocumentNavigator : XPathNavigator
{
	public XmlDocumentNavigator(XmlNode node) : base()
	{
	}

	public override XPathNavigator Clone()
	{
		return null;
	}

	public override String GetAttribute(String localName, String namespaceURI)
	{
		return null;
	}


	public override String GetNamespace(String name)
	{
		return null;
	}
	
	public override bool IsSamePosition(XPathNavigator other)
	{
		return false;
	}
	
	public override bool MoveTo(XPathNavigator other)
	{
		return false;
	}

	public override bool MoveToAttribute(String localName, String namespaceURI)
	{
		return false;
	}

	public override bool MoveToFirst()
	{
		return false;
	}

	public override bool MoveToFirstAttribute()
	{
		return false;
	}

	public override bool MoveToFirstChild()
	{
		return false;
	}

	public override bool MoveToFirstNamespace(XPathNamespaceScope namespaceScope)
	{
		return false;
	}

	public override bool MoveToId(String id)
	{
		return false;
	}

	public override bool MoveToNamespace(String name)
	{
		return false;
	}

	public override bool MoveToNext()
	{
		return false;
	}

	public override bool MoveToNextAttribute()
	{
		return false;
	}

	public override bool MoveToNextNamespace(XPathNamespaceScope namespaceScope)
	{
		return false;
	}
	

	public override bool MoveToParent()
	{
		return false;
	}

	public override bool MoveToPrevious()
	{
		return false;
	}

	public override void MoveToRoot()
	{
		return;
	}
	
	public override String BaseURI 
	{
		get
		{
			return null;
		}
	}

	public override bool HasAttributes
	{
		get
		{
			return false;
		}
	}

	public override bool HasChildren
	{
		get
		{
			return false;
		}
	}

	public override bool IsEmptyElement
	{
		get
		{
			return false;
		}
	}

	public override String LocalName
	{
		get
		{
			return null;
		}
	}

	public override String Name
	{
		get
		{
			return null;
		}
	}

	public override XmlNameTable NameTable
	{
		get
		{
			return null;
		}
	}

	public override String NamespaceURI
	{
		get
		{
			return null;
		}
	}

	public override XPathNodeType NodeType
	{
		get
		{
			return XPathNodeType.All;
		}
	}

	public override String Prefix
	{
		get
		{
			return null;
		}
	}

	public override String Value
	{
		get
		{
			return null;
		}
	}

	public override String XmlLang
	{
		get
		{
			return null;
		}
	}
}

}
