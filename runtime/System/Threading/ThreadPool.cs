/*
 * ThreadPool.cs - Implementation of "ThreadPool" class 
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * 
 * Contributed by Gopal.V 
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

namespace System.Threading
{
	/* Note: I have this suspicion that non-ECMA builds would actually
	use this class internally ?
	*/
	public sealed class ThreadPool
	{
		[TODO]
		public static bool BindHandle(IntPtr osHandle)
		{
			// win32 Specific ?
			 throw new NotImplementedException("BindHandle");
		}

		[TODO]
		public static void GetAvailableThreads( out int workerThreads,
												out int completionPortThreads)
		{
			 throw new NotImplementedException("GetAvailableThreads");
		}

		[TODO]
		public static void GetMaxThreads(out int workerThreads,
										 out int completionPortThreads)
		{
			 throw new NotImplementedException("GetMaxThreads");
		}

		[TODO]
		public static bool QueueUserWorkItem(WaitCallback callback)
		{
			 throw new NotImplementedException("QueueUserWorkItem");
		}

		[TODO]
		public static bool QueueUserWorkItem(WaitCallback callback, 
											Object state)
		{
			 throw new NotImplementedException("QueueUserWorkItem");
		}

		[TODO]
		public static RegisteredWaitHandle RegisterWaitForSingleObject(
								WaitHandle waitObject, 
								WaitOrTimerCallback callback, 
								Object state, 
								int millisecondsTimeOutInterval, 
								bool executeOnlyOnce)
		{
			 throw new NotImplementedException("RegisterWaitForSingleObject");
		}

		[TODO]
		public static RegisteredWaitHandle RegisterWaitForSingleObject(
								WaitHandle waitObject, 
								WaitOrTimerCallback callback, 
								Object state, 
								long millisecondsTimeOutInterval, 
								bool executeOnlyOnce)
		{
			 throw new NotImplementedException("RegisterWaitForSingleObject");
		}

		[TODO]
		public static RegisteredWaitHandle RegisterWaitForSingleObject(
								WaitHandle waitObject, 
								WaitOrTimerCallback callback, 
								Object state, 
								TimeSpan timeout, 
								bool executeOnlyOnce)
		{
			 throw new NotImplementedException("RegisterWaitForSingleObject");
		}

		[TODO]
		public static RegisteredWaitHandle RegisterWaitForSingleObject(
								WaitHandle waitObject, 
								WaitOrTimerCallback callback, 
								Object state, 
								uint millisecondsTimeOutInterval, 
								bool executeOnlyOnce)
		{
			 throw new NotImplementedException("RegisterWaitForSingleObject");
		}

		[TODO]
		public static bool UnsafeQueueUserWorkItem(
								WaitCallback callback, 
								Object state)
		{
			 throw new NotImplementedException("UnsafeQueueUserWorkItem");
		}

		[TODO]
		public static RegisteredWaitHandle UnsafeRegisterWaitForSingleObject(
								WaitHandle waitObject, 
								WaitOrTimerCallback callback, 
								Object state, 
								int millisecondsTimeOutInterval, 
								bool executeOnlyOnce)
		{
			 throw new NotImplementedException("UnsafeRegisterWaitForSingleObject");
		}

		[TODO]
		public static RegisteredWaitHandle UnsafeRegisterWaitForSingleObject(
								WaitHandle waitObject, 
								WaitOrTimerCallback callback, 
								Object state, 
								long millisecondsTimeOutInterval, 
								bool executeOnlyOnce)
		{
			 throw new NotImplementedException("UnsafeRegisterWaitForSingleObject");
		}

		[TODO]
		public static RegisteredWaitHandle UnsafeRegisterWaitForSingleObject(
								WaitHandle waitObject, 
								WaitOrTimerCallback callback, 
								Object state, 
								TimeSpan timeout, 
								bool executeOnlyOnce)
		{
			 throw new NotImplementedException("UnsafeRegisterWaitForSingleObject");
		}

		[TODO]
		public static RegisteredWaitHandle UnsafeRegisterWaitForSingleObject(
								WaitHandle waitObject, 
								WaitOrTimerCallback callback, 
								Object state, 
								uint millisecondsTimeOutInterval, 
								bool executeOnlyOnce)
		{
			 throw new NotImplementedException("UnsafeRegisterWaitForSingleObject");
		}

	}
}//namespace
#endif
