/*
 * MarshalByValueComponent.cs - 
 *		Implementation of "MarshalByValueComponent" class. 
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

using System;

namespace System.ComponentModel
{
#if !ECMA_COMPAT

	public class MarshalByValueComponent :	IServiceProvider, 
											IDisposable, 
											IComponent
	{
		[TODO]
		public MarshalByValueComponent()
		{
		}

		[TODO]
		public void Dispose()
		{
			throw new NotImplementedException("Dispose");
		}

		[TODO]
		protected virtual void Dispose(bool disposing)
		{
			throw new NotImplementedException("Dispose");
		}

		[TODO]
		protected override void Finalize()
		{
			throw new NotImplementedException("Finalize");
		}

		[TODO]
		public virtual Object GetService(Type service)
		{
			throw new NotImplementedException("GetService");
		}

		public virtual IContainer Container 
		{ 
			get
			{
				throw new NotImplementedException("Container");
			}
		}

		public virtual bool DesignMode 
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

		public event EventHandler Disposed;

	}

#endif // ECMA_COMPAT
}//namespace System.ComponentModel
