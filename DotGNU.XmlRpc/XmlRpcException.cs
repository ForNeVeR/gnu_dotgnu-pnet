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
using System;
using System.Xml;

namespace DotGNU.XmlRpc
{
  public class XmlRpcException : Exception
  {
    private int faultCode;

    public XmlRpcException( int faultCode, string message ) 
      : base( message )
    {
      base.HResult = faultCode;
    }
  
    public XmlRpcException( Exception e ) : base( e.Message )
    {
      base.HResult = e.HResult;
    }

    public int FaultCode 
    {
      get {
	return this.HResult;
      }
    }    
  }

  public class XmlRpcBadFormatException : XmlRpcException
  {
    public XmlRpcBadFormatException( int faultCode, string message ) 
      : base( faultCode, message ) 
    { 
    }  
  }  

  public class XmlRpcInvalidStateException : XmlRpcException
  {
    public XmlRpcInvalidStateException( string message ) 
      : base( 200, message ) 
    {
    }
  }

  public class XmlRpcInvalidXmlRpcException : ApplicationException
  {
    public XmlRpcInvalidXmlRpcException() { }  
    public XmlRpcInvalidXmlRpcException( String text ) : base(text) { }  
    public XmlRpcInvalidXmlRpcException( String text, Exception iex ) 
	     : base(text, iex) { }  
  }

  public class XmlRpcBadMethodException : ApplicationException
  {
    public XmlRpcBadMethodException() { }  
    public XmlRpcBadMethodException( String text ) : base(text) { }  
    public XmlRpcBadMethodException( String text, Exception iex ) 
	     : base(text, iex) { }  
  }

  public class XmlRpcBadValueException : ApplicationException
  {
    public XmlRpcBadValueException() { }  
    public XmlRpcBadValueException( String text ) : base(text) { }  
    public XmlRpcBadValueException( String text, Exception iex ) 
	     : base(text, iex) { }  
  }

  public class XmlRpcNullReferenceException : ApplicationException
  {
    public XmlRpcNullReferenceException() { }  
    public XmlRpcNullReferenceException( String text ) : base(text) { }  
    public XmlRpcNullReferenceException( String text, Exception iex ) 
	     : base(text, iex) { }  
  }

  public class XmlRpcInvalidParametersException : ApplicationException
  {
    public XmlRpcInvalidParametersException() { }  
    public XmlRpcInvalidParametersException( String text ) : base(text) { }  
    public XmlRpcInvalidParametersException( String text, Exception iex ) 
	     : base(text, iex) { }  
  }

  public class XmlRpcTypeMismatchException : ApplicationException
  {
    public XmlRpcTypeMismatchException() { }  
    public XmlRpcTypeMismatchException( String text ) : base(text) { }  
    public XmlRpcTypeMismatchException( String text, Exception iex ) 
	     : base(text, iex) { }  
  }

  public class XmlRpcBadAssemblyException : ApplicationException
  {
    public XmlRpcBadAssemblyException() { }  
    public XmlRpcBadAssemblyException( String text ) : base(text) { }  
    public XmlRpcBadAssemblyException( String text, Exception iex ) 
	     : base(text, iex) { }  
  }
  
}

