/*
 * SocketOptionLevel.cs - Implementation of the "System.Net.Sockets.SocketOptionName" class.
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

public enum SocketOptionName
{
	AcceptConnection = 2,
	AddMembership = 12,
	AddSourceMembership = 15,
	BlockSource = 17,
	Broadcast = 32,
	BsdUrgent = 2,
	ChecksumCoverage = 20,
	Debug = 1,
	DontFragment = 14,
	DontLinger = -129,
	DontRoute = 16,
	DropMembership = 13,
	DropSourceMembership = 16,
	Error = 4103,
	ExclusiveAddressUse = -5,
	Expedited = 2,
	HeaderIncluded = 2,
	IPOptions = 1,
	IpTimeToLive = 4,
	KeepAlive = 8,
	Linger = 128,
	MaxConnections = 2147483647,
	MulticastInterface = 9,
	MulticastLoopback = 11,
	MulticastTimeToLive = 10,
	NoChecksum = 1,
	NoDelay = 1,
	OutOfBandInline = 256,
	PacketInformation = 19,
	ReceiveBuffer = 4098,
	ReceiveLowWater = 4100,
	ReceiveTimeout = 4102,
	ReuseAddress = 4,
	SendBuffer = 4097,
	SendLowWater = 4099,
	SendTimeout = 4101,
	Type = 4104,
	TypeOfService = 3,
	UnblockSource = 18,
	UseLoopback = 64
}; // enum SocketOptionName

}; // namespace System.Net.Sockets
