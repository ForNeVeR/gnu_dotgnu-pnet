/*
 * ParameterBuilder.cs - Implementation of "System.Reflection.Emit.ParameterBuilder" 
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

#if !ECMA_COMPAT

namespace System.Reflection.Emit
{
	public class ParameterBuilder
	{
		[TODO]
		public virtual ParameterToken GetToken()
		{
			throw new NotImplementedException("GetToken");
		}

		[TODO]
		public virtual void SetConstant(Object defaultValue)
		{
			throw new NotImplementedException("SetConstant");
		}

		[TODO]
		public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
		{
			throw new NotImplementedException("SetCustomAttribute");
		}

		[TODO]
		public void SetCustomAttribute(ConstructorInfo con, byte[] binaryAttribute)
		{
			throw new NotImplementedException("SetCustomAttribute");
		}

		[TODO]
		public virtual void SetMarshal(UnmanagedMarshal unmanagedMarshal)
		{
			throw new NotImplementedException("SetMarshal");
		}

		[TODO]
		public virtual int Attributes 
		{ 
			get
			{
				throw new NotImplementedException("Attributes");
			}
		}

		[TODO]
		public bool IsIn 
		{ 
			get
			{
				throw new NotImplementedException("IsIn");
			}
		}

		[TODO]
		public bool IsOptional 
		{
			get
			{
				throw new NotImplementedException("IsOptional");
			}
		}

		[TODO]
		public bool IsOut 
		{ 
			get
			{
				throw new NotImplementedException("IsOut");
			}
		}

		[TODO]
		public virtual String Name 
		{ 
			get
			{
				throw new NotImplementedException("Name");
			}
		}

		[TODO]
		public virtual int Position 
		{ 
			get
			{
				throw new NotImplementedException("Position");
			}
		}

	}
}//namespace

#endif
