/*
 * DotGNU XmlRpc implementation
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * $Revision: 1.1 $  $Date: 2004/05/04 17:00:46 $
 * 
 * --------------------------------------------------------------------------
 */
//using System.Reflection;
using System.Collections;
using System.Globalization;
using System.IO;
using System.Xml;

namespace DotGNU.XmlRpc
{
  public class XmlRpcResponse : XmlRpcObject
  {
    public XmlRpcResponse()
    {
    }
    
    public override string ToString() 
    {
      string s =  "XmlRpcResponse contains:\n";
      foreach( object obj in this ) {
	    if( obj != null ) {
		  s += String.Format( "Type: {0}, Value: {1}", obj.GetType(), obj );
        } else {
		  s += String.Format( "Type: null, Value: null" );
		}
      }
      return s;
    }
  }
}

