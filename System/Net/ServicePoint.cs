/*
 * ServicePoint.cs - Implementation of the "System.Net.ServicePoint" class.
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


public class ServicePoint
{
	public override int GetHashCode() {}
	
	public Uri Address { get{} }
	
	public int ConnectionLimit { get{} set{} }
	
	public String ConnectionName { get{} }
	
	public int CurrentConnections { get{} }
	
	public DateTime IdleSince { get{} }
	
	public int MaxIdleTime { get{} set{} }
	
	public virtual Version ProtocolVersion { get{} }
	
	public bool SupportsPipelining { get{} }
}; //class ServicePoint

}; //namespace System.Net