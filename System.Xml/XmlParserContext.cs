/*
 * XmlParserContext.cs - Implementation of the "System.XmlParserContext" class.
 *
 * Copyright (C) 2002 Southern Storm Software, Pty Ltd.
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
 
namespace System.Xml
{

using System;
using System.Text;

public class XmlParserContext
{
	//Internal variables
	private String baseuri;
	private String doctypename;
	private Encoding encoding;
	private String internalsubset;
	private XmlNameTable nametable;
	private XmlNamespaceManager namespacemanager;
	private String publicid;
	private String systemid;
	private String xmllang;
	private XmlSpace xmlspace;
		
	//Constructors
	public XmlParserContext(XmlNameTable nt, XmlNamespaceManager nsMgr, 
		String xmlLang, XmlSpace xmlSpace) 
		: this(nt, nsMgr, null, null, null, null, 
			String.Empty, xmlLang, xmlSpace, null )
			{}
	
	public XmlParserContext(XmlNameTable nt, XmlNamespaceManager nsMgr, 
		String xmlLang, XmlSpace xmlSpace, Encoding enc)
		: this(nt, nsMgr, null, null, null, null, 
			System.String.Empty, xmlLang, xmlSpace, enc)
			{}
			
	public XmlParserContext(XmlNameTable nt, XmlNamespaceManager nsMgr, 
		String docTypeName, String pubId, String sysId, String internalSubset, 
		String baseURI, String xmlLang, XmlSpace xmlSpace)
		:this(nt, nsMgr, docTypeName, pubId, sysId, 
			internalSubset, baseUri, xmlLang, xmlSpace, null)	
			{}
			
	public XmlParserContext(XmlNameTable nt, XmlNamespaceManager nsMgr, 
		String docTypeName, String pubId, String sysId, String internalSubset, 
		String baseURI, String xmlLang, XmlSpace xmlSpace, Encoding enc)
			{
				if (!((nsMgr.NameTable == nt) || (nt == 0)))
					throw new XmlException("nt", _("Xml_WrongNameTable"));
				
				if (nt == 0)
					nametable = nsMgr.NameTable;
				else
					nametable = nt;
				
				namespacemanager = nsMgr;
					
				if (docTypeName == 0)
					doctypename = String.Empty;
				else	
					doctypename = docTypeName;
					
				if (pubId == 0)
					pubid = String.Empty;
				else
					pubid = pubId;
				
				if (sysId == 0)
					sysid = String.Empty;
				else
					sysid = sysId;
					
				if (internalSubset == 0)
					internalsubset = String.Empty;
				else
					internalsubset = internalSubset;
					
				if (baseUri == 0)
					baseuri = String.Empty;
				else
					baseuri = baseUri;
					
				if (xmlLang == 0)
					xmllang = String.Empty;
				else
					xmllang = xmlLang;			
				
				xmlspace = xmlSpace;
				
				encoding = enc;				
			}		
			
	//Properties
	public String BaseUri
			{
				get
				{
					return baseuri;
				}
				set
				{
					if (value == 0)
						baseuri = String.Empty;
					else
						baseuri = value;
				}
			}		
	
	public String DocTypeName
			{
				get
				{
					return doctypename;
				}
				set
				{
					if (value == 0)
						doctypename = String.Empty;
					else
						doctypename = value;
				}
			}
			
	public Encoding Encoding
			{
				get
				{
					return encoding;
				}
				set
				{
					encoding = value;
				}
			}
	
	public String InternalSubset
			{
				get
				{
					return internalsubset;
				}
				set
				{
					if (value == 0)
						internalsubset = String.Empty;
					else
						internalsubset = value;
				}
			}
	
	public XmlNameTable NameTable
			{
				get
				{
					return nametable;
				}
				set
				{
					nametable = value;
				}
			}
	
	public XmlNamespaceManager NamespaceManager
			{
				get
				{
					return namespacemanager;
				}
				set
				{
					namespacemanager = value;
				}
			}
	
	public String PublicId
			{
				get
				{
					return publicid;
				}
				set
				{
					if (value == 0)
						publicid = String.Empty;
					else
						publicid = value;
				}
			}	
			
	public String SystemId
			{
				get
				{
					return systemid;
				}
				set
				{
					if (value == 0)
						systemid = String.Empty;
					else
						systemid = value;
				}
			}	

	public String XmlLang
			{
				get
				{
					return xmllang;
				}
				set
				{
					if (value == 0)
						xmllang = String.Empty;
					else
						xmllang = value;
				}
			}						
			
}; //class XmlParserContext

}; //namespace System.Xml