/*
 * XmlCodeExporter.cs - Implementation of the
 *			"System.Xml.Serialization.XmlCodeExporter" class.
 *
 * Copyright (C) 2003  Free Software Foundation, Inc.
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

namespace System.Xml.Serialization
{

#if !ECMA_COMPAT

using System;
using System.Xml;
using System.CodeDom;

[TODO]
public class XmlCodeExporter
{
	[TODO]
	public XmlCodeExporter
				(CodeNamespace codeNamespace)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlCodeExporter
				(CodeNamespace codeNamespace, CodeCompileUnit codeCompileUnit)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public CodeAttributeDeclarationCollection IncludeMetadata
			{
				get
				{
					// TODO
					throw new NotImplementedException("IncludeMetadata");
				}
			}

	[TODO]
	public void AddMappingMetadata
				(CodeAttributeDeclarationCollection metadata,
				 XmlMemberMapping member, String ns)
			{
				// TODO
				throw new NotImplementedException("AddMappingMetadata");
			}

	[TODO]
	public void AddMappingMetadata
				(CodeAttributeDeclarationCollection metadata,
				 XmlTypeMapping mapping, String ns)
			{
				// TODO
				throw new NotImplementedException("AddMappingMetadata");
			}

	[TODO]
	public void AddMappingMetadata
				(CodeAttributeDeclarationCollection metadata,
				 XmlMemberMapping member, String ns, bool forceUseMemberName)
			{
				// TODO
				throw new NotImplementedException("AddMappingMetadata");
			}

	[TODO]
	public void ExportMembersMapping
				(XmlMembersMapping xmlMembersMapping)
			{
				// TODO
				throw new NotImplementedException("ExportMembersMapping");
			}

	[TODO]
	public void ExportTypeMapping
				(XmlTypeMapping xmlTypeMapping)
			{
				// TODO
				throw new NotImplementedException("ExportTypeMapping");
			}

}; // class XmlCodeExporter

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
