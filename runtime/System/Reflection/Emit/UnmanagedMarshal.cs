/*
 * UnmanagedMarshal.cs - Implementation of "System.Reflection.Emit.UnmanagedMarshal" 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * 
 * Contributed by Gopal.V <gopalv82@symonds.net> 
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

#if !ECMA_COMPAT

namespace System.Reflection.Emit
{
	public sealed class UnmanagedMarshal
	{
		[TODO]
		public static UnmanagedMarshal DefineByValArray(int elemCount)
		{
			throw new NotImplementedException("DefineByValArray");
		}

		[TODO]
		public static UnmanagedMarshal DefineByValTStr(int elemCount)
		{
			throw new NotImplementedException("DefineByValTStr");
		}

		[TODO]
		public static UnmanagedMarshal DefineLPArray(UnmanagedType elemType)
		{
			throw new NotImplementedException("DefineLPArray");
		}

		[TODO]
		public static UnmanagedMarshal DefineSafeArray(UnmanagedType elemType)
		{
			throw new NotImplementedException("DefineSafeArray");
		}

		[TODO]
		public static UnmanagedMarshal DefineUnmanagedMarshal(UnmanagedType unmanagedType)
		{
			throw new NotImplementedException("DefineUnmanagedMarshal");
		}

		[TODO]
		public UnmanagedType BaseType 
		{
			get
			{
				throw new NotImplementedException("BaseType");
			}
		}

		[TODO]
		public int ElementCount 
		{
			get
			{
				throw new NotImplementedException("ElementCount");
			}
		}

		[TODO]
		public UnmanagedType GetUnmanagedType 
		{
			get
			{
				throw new NotImplementedException("GetUnmanagedType");
			}
		}

		[TODO]
		public Guid IIDGuid 
		{
			get
			{
				throw new NotImplementedException("IIDGuid");
			}
		}

	}
}//namespace

#endif
