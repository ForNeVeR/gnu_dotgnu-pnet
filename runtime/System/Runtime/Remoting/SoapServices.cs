/*
 * SoapServices.cs - Implementation of the
 *			"System.Runtime.Remoting.SoapServices" class.
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

namespace System.Runtime.Remoting
{

#if CONFIG_REMOTING

using System.Reflection;

public class SoapServices
{
	// Cannot create instances of this class.
	private SoapServices() {}

	// Standard namespace prefixes, defined by Microsoft.
	public static String XmlNsForClrType
			{
				get
				{
					return "http://schemas.microsoft.com/clr/";
				}
			}
	public static String XmlNsForClrTypeWithAssembly
			{
				get
				{
					return "http://schemas.microsoft.com/clr/assem/";
				}
			}
	public static String XmlNsForClrTypeWithNs
			{
				get
				{
					return "http://schemas.microsoft.com/clr/ns/";
				}
			}
	public static String XmlNsForClrTypeWithNsAndAssembly
			{
				get
				{
					return "http://schemas.microsoft.com/clr/nsassem/";
				}
			}

	// Encode SOAP names.
	[TODO]
	public static String CodeXmlNamespaceForClrTypeNamespace
				(String typeNamespace, String assemblyName)
			{
				// TODO
				return null;
			}

	// Decode SOAP names.
	[TODO]
	public static bool DecodeXmlNamespaceForClrTypeNamespace
				(String inNamespace, out String typeNamespace,
				 out String assemblyName)
			{
				// TODO
				typeNamespace = null;
				assemblyName = null;
				return false;
			}

	// Get field information from SOAP data.
	[TODO]
	public static void GetInteropFieldTypeAndNameFromXmlAttribute
				(Type containingType, String xmlAttribute,
				 String xmlNamespace, out Type type, out String name)
			{
				// TODO
				type = null;
				name = null;
			}
	public static void GetInteropFieldTypeAndNameFromXmlElement
				(Type containingType, String xmlElement,
				 String xmlNamespace, out Type type, out String name)
			{
				// TODO
				type = null;
				name = null;
			}

	// Get type information from SOAP data.
	[TODO]
	public static Type GetInteropTypeFromXmlElement
				(String xmlElement, String xmlNamespace)
			{
				// TODO
				return null;
			}
	[TODO]
	public static Type GetInteropTypeFromXmlType
				(String xmlType, String xmlTypeNamespace)
			{
				// TODO
				return null;
			}

	// Get the SOAP action associated with a method.
	[TODO]
	public static String GetSoapActionFromMethodBase(MethodBase mb)
			{
				// TODO
				return null;
			}

	// Get type and method information from a SOAP action.
	[TODO]
	public static bool GetTypeAndMethodNameFromSoapAcion
				(String soapAction, out String typeName, out String methodName)
			{
				// TODO
				typeName = null;
				methodName = null;
				return false;
			}

	// Get the XML element information for serializing a type.
	[TODO]
	public static bool GetXmlElementForInteropType
				(Type type, out String xmlElement, out String xmlNamespace)
			{
				// TODO
				xmlElement = null;
				xmlNamespace = null;
				return false;
			}

	// Get the namespace to use for a method call.
	[TODO]
	public static String GetXmlNamespaceForMethodCall(MethodBase mb)
			{
				// TODO
				return null;
			}

	// Get the namespace to use for a method response.
	[TODO]
	public static String GetXmlNamespaceForMethodResponse(MethodBase mb)
			{
				// TODO
				return null;
			}

	// Get XML type information for a type.
	[TODO]
	public static bool GetXmlTypeForInteropType
				(Type type, out String xmlType, out String xmlTypeNamespace)
			{
				// TODO
				xmlType = null;
				xmlTypeNamespace = null;
				return false;
			}

	// Determine if a namespace indicates CLR information.
	public static bool IsClrTypeNamespace(String namespaceString)
			{
				if(namespaceString != null)
				{
					return namespaceString.StartsWith
						("http://schemas.microsoft.com/clr/");
				}
				else
				{
					return false;
				}
			}

	// Determine if a SOAP action is valid for a method.
	[TODO]
	public static bool IsSoapActionValidForMethodBase
				(String soapAction, MethodBase mb)
			{
				// TODO
				return false;
			}

	// Preload SOAP handling information.
	[TODO]
	public static void PreLoad(Assembly assembly)
			{
				// TODO
			}
	[TODO]
	public static void PreLoad(Type type)
			{
				// TODO
			}

	// Register an XML element name with a type.
	[TODO]
	public static void RegisterInteropXmlElement
				(String xmlElement, String xmlNamespace, Type type)
			{
				// TODO
			}

	// Register an XML type name with a type.
	[TODO]
	public static void RegisterInteropXmlType
				(String xmlType, String xmlTypeNamespace, Type type)
			{
				// TODO
			}

	// Register a SOAP action for a method.
	[TODO]
	public static void RegisterSoapActionForMethodBase(MethodBase mb)
			{
				// TODO
			}
	[TODO]
	public static void RegisterSoapActionForMethodBase
				(MethodBase mb, String soapAction)
			{
				// TODO
			}

}; // class SoapServices

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting
