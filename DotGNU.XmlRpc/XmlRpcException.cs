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
 * $Revision: 1.2 $  $Date: 2006/05/14 11:00:47 $
 * 
 * --------------------------------------------------------------------------
 */
using System;
using System.Xml;
using System.Runtime.Serialization;

namespace DotGNU.XmlRpc
{
  [Serializable()]
  public class XmlRpcException : Exception
  {
    private int faultCode;

    public XmlRpcException()
    {
    }

    public XmlRpcException( string message)
      : base( message )
    {
    }

    public XmlRpcException( int faultCode, string message ) 
      : base( message )
    {
      base.HResult = faultCode;
    }
  
    public XmlRpcException( Exception e ) : base( e.Message )
    {
      base.HResult = e.HResult;
    }

    public XmlRpcException(SerializationInfo info, StreamingContext context)
       : base( info, context )
    {
    }

    public int FaultCode 
    {
      get {
	return this.HResult;
      }
    }    
  }

  [Serializable()]
  public class XmlRpcBadFormatException : XmlRpcException
  {
    public XmlRpcBadFormatException()
      : base()
    {
    }
    public XmlRpcBadFormatException( string message)
      : base( message )
    {
    }
    public XmlRpcBadFormatException( int faultCode, string message ) 
      : base( faultCode, message ) 
    { 
    }  
    public XmlRpcBadFormatException(SerializationInfo info, StreamingContext context)
       : base( info, context )
    {
    }
  }  
  [Serializable()]
  public class XmlRpcInvalidStateException : XmlRpcException
  {
    public XmlRpcInvalidStateException()
      : base()
    {
    }
    public XmlRpcInvalidStateException( string message ) 
      : base( 200, message ) 
    {
    }
    public XmlRpcInvalidStateException(SerializationInfo info, StreamingContext context)
       : base( info, context )
    {
    }
  }

  [Serializable()]
  public class XmlRpcInvalidXmlRpcException : ApplicationException
  {
    public XmlRpcInvalidXmlRpcException() { }  
    public XmlRpcInvalidXmlRpcException( String text ) : base(text) { }  
    public XmlRpcInvalidXmlRpcException( String text, Exception iex ) 
	     : base(text, iex) { }  
    public XmlRpcInvalidXmlRpcException(SerializationInfo info, StreamingContext context)
       : base( info, context )
    {
    }
  }

  [Serializable()]
  public class XmlRpcBadMethodException : ApplicationException
  {
    public XmlRpcBadMethodException() { }  
    public XmlRpcBadMethodException( String text ) : base(text) { }  
    public XmlRpcBadMethodException( String text, Exception iex ) 
	     : base(text, iex) { }  
    public XmlRpcBadMethodException(SerializationInfo info, StreamingContext context)
       : base( info, context )
    {
    }
  }

  [Serializable()]
  public class XmlRpcBadValueException : ApplicationException
  {
    public XmlRpcBadValueException() { }  
    public XmlRpcBadValueException( String text ) : base(text) { }  
    public XmlRpcBadValueException( String text, Exception iex ) 
	     : base(text, iex) { }  
    public XmlRpcBadValueException(SerializationInfo info, StreamingContext context)
       : base( info, context )
    {
    }
  }

  [Serializable()]
  public class XmlRpcNullReferenceException : ApplicationException
  {
    public XmlRpcNullReferenceException() { }  
    public XmlRpcNullReferenceException( String text ) : base(text) { }  
    public XmlRpcNullReferenceException( String text, Exception iex ) 
	     : base(text, iex) { }  
    public XmlRpcNullReferenceException(SerializationInfo info, StreamingContext context)
       : base( info, context )
    {
    }
  }

  [Serializable()]
  public class XmlRpcInvalidParametersException : ApplicationException
  {
    public XmlRpcInvalidParametersException() { }  
    public XmlRpcInvalidParametersException( String text ) : base(text) { }  
    public XmlRpcInvalidParametersException( String text, Exception iex ) 
	     : base(text, iex) { }  
    public XmlRpcInvalidParametersException(SerializationInfo info, StreamingContext context)
       : base( info, context )
    {
    }
  }

  [Serializable()]
  public class XmlRpcTypeMismatchException : ApplicationException
  {
    public XmlRpcTypeMismatchException() { }  
    public XmlRpcTypeMismatchException( String text ) : base(text) { }  
    public XmlRpcTypeMismatchException( String text, Exception iex ) 
	     : base(text, iex) { }  
    public XmlRpcTypeMismatchException(SerializationInfo info, StreamingContext context)
       : base( info, context )
    {
    }
  }

  [Serializable()]
  public class XmlRpcBadAssemblyException : ApplicationException
  {
    public XmlRpcBadAssemblyException() { }  
    public XmlRpcBadAssemblyException( String text ) : base(text) { }  
    public XmlRpcBadAssemblyException( String text, Exception iex ) 
	     : base(text, iex) { }  
    public XmlRpcBadAssemblyException(SerializationInfo info, StreamingContext context)
       : base( info, context )
    {
    }
  }
  
}

