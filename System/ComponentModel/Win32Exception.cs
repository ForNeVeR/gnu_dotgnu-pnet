/*
 * Win32Exception.cs - Implementation of "System.ComponentModel.Win32Exception" 
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  Free Software Foundation,Inc.
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

using System;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;

namespace System.ComponentModel
{
#if !ECMA_COMPAT
	public class Win32Exception: ExternalException
	{
		private int nativeErrorCode;

		public Win32Exception() : base()
		{
			HResult = unchecked((int)0x80004005);
		}
		public Win32Exception(int error) : base()
		{
			nativeErrorCode = error;
			HResult = unchecked((int)0x80004005);
		}
		public Win32Exception(int error, String message) : base(message)
		{
			nativeErrorCode = error;
			HResult = unchecked((int)0x80004005);
		}
		internal Win32Exception(String message) : base (message)
		{
			HResult = unchecked((int)0x80004005);
		}
		internal Win32Exception(String message,Exception inner) 
			: base (message,inner)
		{
			HResult = unchecked((int)0x80004005);
		}
#if CONFIG_SERIALIZATION
		protected Win32Exception(SerializationInfo info,
								 StreamingContext context)
			: base(info, context)
		{
			nativeErrorCode = info.GetInt32("NativeErrorCode");
		}
#endif

		public int NativeErrorCode 
		{
			get
			{
				return nativeErrorCode;
			}
		}

#if CONFIG_SERIALIZATION
		public override void GetObjectData(SerializationInfo info,
										   StreamingContext context)
		{
			base.GetObjectData(info, context);
			info.AddValue("NativeErrorCode", nativeErrorCode);
		}
#endif

	}
#endif	
}//namespace
