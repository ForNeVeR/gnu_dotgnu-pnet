/*
 * DownloadProgressChangedEventArgs.cs - Implementation of the
 *			"System.Net.DownloadProgressChangedEventArgs" class.
 *
 * Copyright (C) 2007  Southern Storm Software, Pty Ltd.
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

#if CONFIG_COMPONENT_MODEL && CONFIG_FRAMEWORK_2_0 && !CONFIG_COMPACT_FRAMEWORK

using System.ComponentModel;

public class DownloadProgressChangedEventArgs : ProgressChangedEventArgs
{
	// Internal state.
	private long bytesReceived;
	private long totalBytesToReceive;

	// Constructors.
	internal DownloadProgressChangedEventArgs(long bytesReceived,
											  long totalBytesToReceive,
											  Object userState)
		: base((int)(totalBytesToReceive == 0 ? 100
			: (int)((bytesReceived * 100) / totalBytesToReceive)), userState)
	{
		this.bytesReceived = bytesReceived;
		this.totalBytesToReceive = totalBytesToReceive;
	}
	
	// Get the number of bytes reveived.
	public long BytesReceived
	{
		get
		{
			return bytesReceived;
		}
	}

	// Get the total number of bytes to reveive.
	public long TotalBytesToReceive
	{
		get
		{
			return totalBytesToReceive;
		}
	}

}; // class DownloadDataCompletedEventArgs

#endif // CONFIG_COMPONENT_MODEL && CONFIG_FRAMEWORK_2_0 && !CONFIG_COMPACT_FRAMEWORK

}; // namespace System.Net
