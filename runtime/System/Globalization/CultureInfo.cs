/*
 * CultureInfo.cs - Implementation of "System.Globalization.CultureInfo".
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System.Globalization
{

public class CultureInfo : IServiceObjectProvider
{

// TODO

	public static CultureInfo InvariantCulture
			{
				get
				{
					// TODO
					return null;
				}
			}

	public static CultureInfo CurrentCulture
			{
				get
				{
					// TODO
					return null;
				}
			}

	public TextInfo TextInfo
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Implementation of the IServiceObjectProvider interface.
	public Object GetServiceObject(Type service)
	{
		if(service == typeof(System.Globalization.CultureInfo))
		{
			return this;
		}
		else
		{
			return null;
		}
	}

}; // class CultureInfo

}; // namespace System.Globalization
