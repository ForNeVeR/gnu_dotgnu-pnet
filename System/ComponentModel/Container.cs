/*
 * Container.cs - Implementation of "System.ComponentModel.Container" class 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.ComponentModel
{
#if !ECMA_COMPAT
	public class Container: IDisposable, IContainer
	{
		[TODO]
		public void Container()
		{
			throw new NotImplementedException(".ctor");
		}

		[TODO]
		public virtual void Add(IComponent component)
		{
			throw new NotImplementedException("Add");
		}

		[TODO]
		public virtual void Add(IComponent component, String name)
		{
			throw new NotImplementedException("Add");
		}

		[TODO]
		protected virtual ISite CreateSite(IComponent component, String name)
		{
			throw new NotImplementedException("CreateSite");
		}

		[TODO]
		public void Dispose()
		{
			throw new NotImplementedException("Dispose");
		}

		[TODO]
		protected virtual void Dispose(bool release_all)
		{
			throw new NotImplementedException("Dispose");
		}

		[TODO]
		protected override void Finalize()
		{
			throw new NotImplementedException("Finalize");
		}

		[TODO]
		protected virtual Object GetService(Type service)
		{
			throw new NotImplementedException("GetService");
		}

		[TODO]
		public virtual void Remove(IComponent component)
		{
			throw new NotImplementedException("Remove");
		}

		[TODO]
		public virtual ComponentCollection Components 
		{
			get
			{
				throw new NotImplementedException("Components");
			}
		}

	}
#endif	
}//namespace
