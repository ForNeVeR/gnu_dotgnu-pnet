/*
 * SafeFileHandle.cs - Implementation of the
 *	"Microsoft.Win32.SafeHandles.SafeFileHandle" class.
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

namespace Microsoft.Win32.SafeHandles
{

#if CONFIG_WIN32_SPECIFICS && CONFIG_FRAMEWORK_1_2

using System;
using System.Runtime.InteropServices;
#if CONFIG_FRAMEWORK_2_0
using System.Runtime.ConstrainedExecution;
#else
using System.Runtime.Reliability;
#endif

public sealed class SafeFileHandle : SafeHandleZeroOrMinusOneIsInvalid
{
	// Constructor.
	public SafeFileHandle(IntPtr preexistingHandle, bool ownsHandle)
			: base(ownsHandle)
			{
				SetHandle(preexistingHandle);
			}

	// Close the handle, using the Win32 api's.
	[DllImport("kernel32.dll")]
	extern private static bool CloseHandle(IntPtr handle);

	// Release the handle.
	[ReliabilityContract(Consistency.WillNotCorruptState, CER.Success)]
	protected override bool ReleaseHandle()
			{
				return CloseHandle(handle);
			}

}; // class SafeFileHandle

#endif // CONFIG_WIN32_SPECIFICS && CONFIG_FRAMEWORK_1_2

}; // namespace Microsoft.Win32.SafeHandles
