/*
 * ProtocolType.cs - Implementation of the "System.Net.Sockets.ProtocolType" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
*
 * This program is free software, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace System.Net.Sockets
{

public enum ProtocolType
{
	Ggp = 3,
	IP = 0,
	Icmp = 1,
	Idp = 22,
	Igmp = 2,
	Ipx = 1000,
	ND = 77,
	Pup = 12,
	Raw = 255,
	Spx = 1256,
	SpxII = 1257,
	Tcp = 6,
	Udp = 17,
	Unknown = -1,
	Unspecified = 0
}; // enum ProtocolType

}; // namespace System.Net.Sockets

