/*
 * WebExceptionStatus.cs - Implementation of the "System.Net.Sockets.WebExceptionStatus" class.
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

namespace System.Net
{

public enum WebExceptionStatus
{
	ConnectFailure = 2,
	ConnectionClosed = 8,
	KeepAliveFailure = 12,
	NameResolutionFailure = 1,
	Pending = 13,
	PipelineFailure = 5,
	ProtocolError = 7,
	ProxyNameResolutionFailure = 15,
	ReceiveFailure = 3,
	RequestCanceled = 6,
	SecureChannelFailure = 10,
	SendFailure = 4,
	ServerProtocolViolation = 11,
	Success = 0,
	Timeout = 14,
	TrustFailure = 9
}; // enum WebExceptionStatus

}; // namespace System.Net

