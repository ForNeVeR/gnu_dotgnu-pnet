/*
 * AddressFamily.cs - Implementation of the "System.Net.Sockets.AddressFamily" class.
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

namespace System.Net.Sockets
{

public enum AddressFamily
{
	AppleTalk = 16,
	Atm = 22,
	Banyan = 21,
	Ccitt = 10,
	Chaos = 5,
	Cluster = 24,
	DataKit = 9,
	DataLink = 13,
	DecNet = 12,
	Ecma = 8,
	FireFox = 19,
	HyperChannel = 15,
	Ieee12844 = 25,
	ImpLink = 3,
	InterNetwork = 2,
	InterNetworkV6 = 23,
	Ipx = 6,
	Irda = 26,
	Iso = 7,
	Lat = 14,
	NS = 6,
	NetBios = 17,
	NetworkDesigners = 28,
	Osi = 7,
	Pup = 4,
	Sna = 11,
	Unix = 1,
	Unknown = -1,
	Unspecified = 0,
	VoiceView = 18
}; // enum AddressFamily

}; // namespace System.Net.Sockets
