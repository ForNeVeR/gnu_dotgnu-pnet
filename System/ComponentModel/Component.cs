/*
 * Component.cs - Implementation of "System.ComponentModel.Component" class 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  Free Software Foundation, Inc.
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
	public class Component: MarshalByRefObject, IDisposable, IComponent
	{
		[TODO]
		public Component()
		{
			throw new NotImplementedException(".ctor");
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
		~Component()
		{
			// TODO
		}

		[TODO]
		protected virtual Object GetService(Type service)
		{
			throw new NotImplementedException("GetService");
		}

		[TODO]
		public override String ToString()
		{
			throw new NotImplementedException("ToString");
		}

		public IContainer Container 
		{
			get
			{
				throw new NotImplementedException("Container");
			}
		}

		protected bool DesignMode 
		{
			get
			{
				throw new NotImplementedException("DesignMode");
			}
		}

		protected EventHandlerList Events 
		{
			get
			{
				throw new NotImplementedException("Events");
			}
		}

		public virtual ISite Site 
		{
			get
			{
				throw new NotImplementedException("Site");
			}
			set
			{
				throw new NotImplementedException("Site");
			}
		}
		public event EventHandler Disposed
		{
			add
			{
				throw new NotImplementedException("Disposed");
			}
			remove
			{
				throw new NotImplementedException("Disposed");
			}
		}
	}
#endif
}//namespace
