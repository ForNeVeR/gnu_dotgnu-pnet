/*
 * CriticalHandle.cs - Implementation of the
 *			"System.Runtime.InteropServices.CriticalHandle" class.
 *
 * Copyright (C) 2004  Southern Storm Software, Pty Ltd.
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

namespace System.Runtime.InteropServices
{

#if CONFIG_FRAMEWORK_2_0
using System.Runtime.ConstrainedExecution;
#else
using System.Runtime.Reliability;
#endif

#if CONFIG_FRAMEWORK_1_2

public abstract class CriticalHandle
#if CONFIG_FRAMEWORK_2_0
	: CriticalFinalizerObject, IDisposable
#else
	: IDisposable
#endif
{
	// Internal state.
	protected IntPtr handle;
	private bool closed;

	// Constructor.
	protected CriticalHandle(IntPtr invalidHandleValue)
			{
				this.handle = invalidHandleValue;
				this.closed = false;
			}

	// Destructor.
	[ReliabilityContract(Consistency.WillNotCorruptState, CER.Success)]
	~CriticalHandle()
			{
				Destroy();
			}

	// Close this handle.
	[ReliabilityContract(Consistency.WillNotCorruptState, CER.Success)]
	public void Close()
			{
				Destroy();
			}

	// Implement the IDisposable interface.
	[ReliabilityContract(Consistency.WillNotCorruptState, CER.Success)]
	public void Dispose()
			{
				Destroy();
			}

	// Release the handle.
	[ReliabilityContract(Consistency.WillNotCorruptState, CER.Success)]
	protected abstract bool ReleaseHandle();

	// Set the handle.
	[ReliabilityContract(Consistency.WillNotCorruptState, CER.Success)]
	protected void SetHandle(IntPtr handle)
			{
				this.handle = handle;
			}

	// Set the handle to invalid.
	[ReliabilityContract(Consistency.WillNotCorruptState, CER.Success)]
	public void SetHandleAsInvalid()
			{
				this.closed = true;
			}

	// Determine if this handle is closed.
	public bool IsClosed
			{
				[ReliabilityContract(Consistency.WillNotCorruptState,
									 CER.Success)]
				get
				{
					return closed;
				}
			}

	// Determine if this handle is invalid.
	public abstract bool IsInvalid
			{
				[ReliabilityContract(Consistency.WillNotCorruptState,
									 CER.Success)]
				get;
			}

	// Destroy this handle.
	private void Destroy()
			{
				if(!IsClosed)
				{
					closed = true;
					if(!IsInvalid)
					{
						ReleaseHandle();
						GC.SuppressFinalize(this);
					}
				}
			}

}; // class CriticalHandle

#endif // CONFIG_FRAMEWORK_1_2

}; // namespace System.Runtime.InteropServices
