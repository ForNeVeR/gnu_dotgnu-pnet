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
			internalSubset, baseURI, xmlLang, xmlSpace, null)	
			{}
			
	public XmlParserContext(XmlNameTable nt, XmlNamespaceManager nsMgr, 
		String docTypeName, String pubId, String sysId, String internalSubset, 
		String baseURI, String xmlLang, XmlSpace xmlSpace, Encoding enc)
			{
				if (nsMgr != null && nt != null)
				{
					if (nsMgr.NameTable != nt)
						throw new XmlException(S._("Xml_WrongNameTable"));
				}
				
				if (nt == null)
					nametable = nsMgr.NameTable;
				else
					nametable = nt;
				
				namespacemanager = nsMgr;
					
				if (docTypeName == null)
					doctypename = String.Empty;
				else	
					doctypename = docTypeName;
					
				if (pubId == null)
					publicid = String.Empty;
				else
					publicid = pubId;
				
				if (sysId == null)
					systemid = String.Empty;
				else
					systemid = sysId;
					
				if (internalSubset == null)
					internalsubset = String.Empty;
				else
					internalsubset = internalSubset;
					
				if (baseURI == null)
					baseuri = String.Empty;
				else
					baseuri = baseURI;
					
				if (xmlLang == null)
					xmllang = String.Empty;
				else
					xmllang = xmlLang;			
				
				xmlspace = xmlSpace;
				
				encoding = enc;				
			}		
			
	//Properties
	public String BaseURI
			{
				get
				{
					return baseuri;
				}
				set
				{
					if (value == null)
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
					if (value == null)
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
					if (value == null)
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
					if (value == null)
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
					if (value == null)
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
					if (value == null)
						xmllang = String.Empty;
					else
						xmllang = value;
				}
			}

	public XmlSpace XmlSpace
			{
				get
				{
					return xmlspace;
				}
				set
				{
					xmlspace = value;
				}
			}

}; //class XmlParserContext

}; //namespace System.Xml
