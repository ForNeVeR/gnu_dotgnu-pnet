/*
 * XmlNamespaceManager.cs - Implementation of the "System.XmlNamespaceManager" class.
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
using System.Collections;

public class XmlNamespaceManager : IEnumerable
{
	//TODO: The scope stuff
	
	private XmlNameTable nametable;

	public XmlNamespaceManager(XmlNameTable nameTable)
			{
				if (nameTable == null)
					throw new ArgumentNullException("nameTable");
				
				this.nametable = nametable;
				
			}
	
	[TODO]
	public virtual void AddNamespace(String prefix, String uri)
			{
				if (prefix == "xml" || prefix == "xmlns")
					throw new ArgumentException("prefix", S._("Xml_InvalidPrefix"));
					
				if (prefix == null)
					throw new ArgumentNullException("prefix");
					
				if (uri == null)
					throw new ArgumentNullException("uri");

				//<!> serves as seperator
				String newentry = prefix + "<!>" + uri;
			
				//PROBLEM: This method should replace the
				//entry in nametable that has the same value,
				//if it already exists. But XmlNameTable 
				//must leave existing entries unchanged 
				//when a String with the same value applies

				//SOLUTION: This class will access the ArrayList in
				//nametable directly to do the job								

			#if false
				// TODO: XmlNameTable does not have ArrayList - Rhys.
				foreach(String s in nametable.ArrayList)
				{
					if (s == newentry)
					{
						nametable.ArrayList.Remove(s);
						nametable.ArrayList.Add(newentry);
						return;
					}
				}
			#endif
				
				return;									
			}
	
	[TODO]
	public virtual IEnumerator GetEnumerator() 
			{
				// TODO: XmlNameTable does not have GetEnumerator - Rhys.
				//return nametable.GetEnumerator();		
				return null;
			}

	[TODO]
	public virtual bool HasNamespace(String prefix) 
			{
			#if false
				// TODO: nametable does not implement ICollection - Rhys.
				foreach (String s in nametable)
				{
					if (s.Split("<!>")[0] == prefix)
						return true;
				}
			#endif

				return false;
			}
	
	[TODO]
	public virtual String LookupNamespace(String prefix) 
			{
				//The default namespace is stored with prefix String.Empty
				//so there isn't need for special searching functions
			#if false
				// TODO: nametable does not implement ICollection - Rhys.
				foreach (String s in nametable)
				{
					if (s.Split("<!>")[0] == prefix)
						{
							return s.Split("<!>")[1];
						}
				}
			#endif
				
				return null;
			}
	
	[TODO]
	public virtual String LookupPrefix(String uri)
			{
				if (uri == null) return null;
				
			#if false
				// TODO: nametable does not implement ICollection - Rhys.
				foreach (String s in nametable)
				{
					if (s.Split("<!>")[1] == uri)
						{
							return s.Split("<!>")[0];
						}
				}
			#endif
				
				return String.Empty;
			}	
	
	[TODO]
	public virtual bool PopScope() { return false; }
	
	[TODO]
	public virtual void PushScope() {}

	public virtual void RemoveNamespace(String prefix, String uri)
			{
				if (prefix == null)
					throw new ArgumentNullException("prefix");
					
				if (uri == null)
					throw new ArgumentNullException("uri");				
				
				//Another ECMA mistake, nametable cannot remove an instance
				//I'll go underground (internal) once more

				//<!> serves as seperator
				String newentry = prefix + "<!>" + uri;
				
				// TODO: nametable does not have ArrayList - Rhys.
				//nametable.ArrayList.Remove(newentry);
				
				return;							
			}
	
	public virtual String DefaultNamespace 
			{ 
				get
				{
					return LookupNamespace(String.Empty);
				}
			}
	
	public XmlNameTable NameTable
			{
				get
				{
					return nametable;			
				}
			}

}; //class XmlNamespaceManager

}; //namespace System.Xml
