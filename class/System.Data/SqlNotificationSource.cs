/*
 * SqlNotificationSource.cs - Implementation of the
 *		"System.Data.SqlClient.SqlNotificationSource" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace System.Data.SqlClient
{

#if NET_2_0 

public enum SqlNotificationSource
{
	Data		= 0,
	Timeout		= 1,
	Object		= 2,
	Database	= 3,
	System		= 4,
	Statement	= 5,
	Environment	= 6,
	Execution	= 7
}; // enum SqlNotificationSource

#endif // NET_2_0

}; // System.Data.SqlClient

