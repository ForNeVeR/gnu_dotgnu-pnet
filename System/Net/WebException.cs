/*
 * WebException.cs - Implementation of the "System.Net.WebException" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.Net
{

using System;

public class WebException : InvalidOperationException
{
	//Variables
	private WebResponse myresponse;
	private WebExceptionStatus mystatus; 	
	
	// Constructors.
	public WebException()
		: base(_("Exception_Web")) {}
	public WebException(String msg)
		: base(msg) {}
	public WebException(String msg, Exception inner)
		: base(msg, inner) {}
	public WebException(String msg, WebExceptionStatus status)
		: base(msg)
			{
				myresponse = null;
				mystatus = status;
			}
	public WebException(String msg, Exception inner, 
		WebExceptionStatus status, WebResponse response) 
		: base(msg, inner) 
			{
				myresponse = response;
				mystatus = status;
			}
	
	
	// Get the default message to use for this exception type.
	protected internal override String MessageDefault
			{
				get
				{
					return _("Exception_Web");
				}
			}

	//Properties
	public WebResponse Response 
			{
				get
				{	
					return myresponse;
				}
			}
	
	public WebExceptionStatus Status
			{
				get
				{
					return mystatus;
				}
			} 
		

}; // class WebException

}; // namespace System.Net
