/*
 * ThreadPool.cs - Implementation of the "System.Threading.ThreadPool" class.
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

namespace System.Threading
{

using System.Security;
using System.Security.Permissions;

#if ECMA_COMPAT
internal
#else
public
#endif
sealed class ThreadPool
{
	// Maximum number of threads in the pool.
	private const int MaxWorkerThreads = 16;
	private const int MaxCompletionThreads = 16;

	// Internal state.
	private static int usedWorkerThreads;
	private static int usedCompletionThreads;
	private static WorkItem workItems, lastWorkItem;
	private static WorkItem completionItems, lastCompletionItem;
	private static Thread[] workerThreads;
	private static Thread[] completionThreads;
	private static int numWorkerThreads;
	private static int numCompletionThreads;
	private static Object completionWait;

	// Constructor.
	private ThreadPool() {}

	// Bind an operating system handle to this thread pool
	public static bool BindHandle(IntPtr osHandle)
			{
				// Not used in this implementation.
				return true;
			}

	// Get the number of available threads in the thread pool.
	public static void GetAvailableThreads(out int workerThreads,
										   out int completionPortThreads)
			{
				lock(typeof(ThreadPool))
				{
					workerThreads = MaxWorkerThreads - usedWorkerThreads;
					completionPortThreads =
						MaxCompletionThreads - usedCompletionThreads;
				}
			}

	// Get the maximum number of threads in the thread pool.
	public static void GetMaxThreads(out int workerThreads,
									 out int completionPortThreads)
			{
				workerThreads = MaxWorkerThreads;
				completionPortThreads = MaxCompletionThreads;
			}

	// Queue a new work item within the thread pool.
	public static bool QueueUserWorkItem(WaitCallback callBack, Object state)
			{
				AddWorkItem(new WorkItem(ClrSecurity.GetPermissionsFrom(1),
										 callBack, state));
				return true;
			}
	public static bool QueueUserWorkItem(WaitCallback callBack)
			{
				return QueueUserWorkItem(callBack, null);
			}

	// Queue a new I/O completion item within the thread pool.
	internal static bool QueueCompletionItem
				(WaitCallback callBack, Object state)
			{
				lock(typeof(ThreadPool))
				{
					if(completionWait == null)
					{
						completionWait = new Object();
					}
				}
				AddCompletionItem
					(new WorkItem(ClrSecurity.GetPermissionsFrom(1),
					 callBack, state));
				return true;
			}

	// Queue a new I/O completion item within the thread pool.
	// This version is used by the "System" assembly in ECMA_COMPAT
	// mode when "WaitCallback" is not defined.
	internal static bool QueueCompletionItem
				(AsyncCallback callBack, IAsyncResult state)
			{
				lock(typeof(ThreadPool))
				{
					if(completionWait == null)
					{
						completionWait = new Object();
					}
				}
				AddCompletionItem
					(new WorkItem(ClrSecurity.GetPermissionsFrom(1),
					 callBack, state));
				return true;
			}

	// Queue a new work item within the thread pool after dropping security.
	// This is "unsafe" in that it may elevate security permissions.
	// However, in our implementation we never elevate the security,
	// so the unsafe version is identical to the safe one above.
	public static bool UnsafeQueueUserWorkItem
				(WaitCallback callBack, Object state)
			{
				return QueueUserWorkItem(callBack, state);
			}

	// Register a callback to be invoked when a wait handle is available.
	public static RegisteredWaitHandle RegisterWaitForSingleObject
				(WaitHandle waitObject, WaitOrTimerCallback callBack,
				 Object state, int millisecondsTimeOutInterval,
				 bool executeOnlyOnce)
			{
				if(waitObject == null)
				{
					throw new ArgumentNullException("waitObject");
				}
				if(millisecondsTimeOutInterval < -1)
				{
					throw new ArgumentOutOfRangeException
						("millisecondsTimeOutInterval",
						 _("ArgRange_NonNegOrNegOne"));
				}
				WorkItem item = new WorkItem(ClrSecurity.GetPermissionsFrom(1),
											 waitObject, callBack, state,
											 millisecondsTimeOutInterval,
											 executeOnlyOnce);
				AddWorkItem(item);
				return new RegisteredWaitHandle(item);
			}
	public static RegisteredWaitHandle RegisterWaitForSingleObject
				(WaitHandle waitObject, WaitOrTimerCallback callBack,
				 Object state, long millisecondsTimeOutInterval,
				 bool executeOnlyOnce)
			{
				return RegisterWaitForSingleObject
					(waitObject, callBack, state,
					 Timer.LongToMS(millisecondsTimeOutInterval),
					 executeOnlyOnce);
			}
	public static RegisteredWaitHandle RegisterWaitForSingleObject
				(WaitHandle waitObject, WaitOrTimerCallback callBack,
				 Object state, TimeSpan timeout,
				 bool executeOnlyOnce)
			{
				return RegisterWaitForSingleObject
					(waitObject, callBack, state,
					 Monitor.TimeSpanToMS(timeout), executeOnlyOnce);
			}
	[CLSCompliant(false)]
	public static RegisteredWaitHandle RegisterWaitForSingleObject
				(WaitHandle waitObject, WaitOrTimerCallback callBack,
				 Object state, uint millisecondsTimeOutInterval,
				 bool executeOnlyOnce)
			{
				return RegisterWaitForSingleObject
					(waitObject, callBack, state,
					 Timer.UIntToMS(millisecondsTimeOutInterval),
					 executeOnlyOnce);
			}

	// Register a callback to be invoked when a wait handle is available.
	// This is "unsafe" in that it may elevate security permissions.
	// However, in our implementation we never elevate the security,
	// so the unsafe versions are identical to the safe ones above.
	public static RegisteredWaitHandle UnsafeRegisterWaitForSingleObject
				(WaitHandle waitObject, WaitOrTimerCallback callBack,
				 Object state, int millisecondsTimeOutInterval,
				 bool executeOnlyOnce)
			{
				return RegisterWaitForSingleObject
					(waitObject, callBack, state, millisecondsTimeOutInterval,
					 executeOnlyOnce);
			}
	public static RegisteredWaitHandle UnsafeRegisterWaitForSingleObject
				(WaitHandle waitObject, WaitOrTimerCallback callBack,
				 Object state, long millisecondsTimeOutInterval,
				 bool executeOnlyOnce)
			{
				return RegisterWaitForSingleObject
					(waitObject, callBack, state,
					 Timer.LongToMS(millisecondsTimeOutInterval),
					 executeOnlyOnce);
			}
	public static RegisteredWaitHandle UnsafeRegisterWaitForSingleObject
				(WaitHandle waitObject, WaitOrTimerCallback callBack,
				 Object state, TimeSpan timeout,
				 bool executeOnlyOnce)
			{
				return RegisterWaitForSingleObject
					(waitObject, callBack, state,
					 Monitor.TimeSpanToMS(timeout), executeOnlyOnce);
			}
	[CLSCompliant(false)]
	public static RegisteredWaitHandle UnsafeRegisterWaitForSingleObject
				(WaitHandle waitObject, WaitOrTimerCallback callBack,
				 Object state, uint millisecondsTimeOutInterval,
				 bool executeOnlyOnce)
			{
				return RegisterWaitForSingleObject
					(waitObject, callBack, state,
					 Timer.UIntToMS(millisecondsTimeOutInterval),
					 executeOnlyOnce);
			}

	// Get the next work item to be dispatched.
	private static WorkItem ItemToDispatch()
			{
				lock(typeof(ThreadPool))
				{
					WorkItem item = workItems;
					if(item != null)
					{
						workItems = item.next;
						if(item.next == null)
						{
							lastWorkItem = null;
						}
					}
					return item;
				}
			}

	// Get the next completion item to be dispatched.
	private static WorkItem CompletionItemToDispatch()
			{
				lock(completionWait)
				{
					WorkItem item = completionItems;
					if(item != null)
					{
						completionItems = item.next;
						if(item.next == null)
						{
							lastCompletionItem = null;
						}
					}
					return item;
				}
			}

	// Run a worker thread.
	private static void Work()
			{
				// Assert unrestricted permissions for this thread.
				// The permissions will be modified for each work item
				// to reflect the context that created the work item.
				ClrSecurity.SetPermissions(null, 0);

				// Wait for and dispatch work items.
				WorkItem item;
				lock(typeof(ThreadPool))
				{
					for(;;)
					{
						item = ItemToDispatch();
						if(item == null)
						{
							Monitor.Wait(typeof(ThreadPool));
						}
						else
						{
							++usedWorkerThreads;
							Monitor.Exit(typeof(ThreadPool));
							item.Execute();
							Monitor.Enter(typeof(ThreadPool));
							--usedWorkerThreads;
						}
					}
				}
			}

	// Run a completion thread.
	private static void Complete()
			{
				// Assert unrestricted permissions for this thread.
				// The permissions will be modified for each work item
				// to reflect the context that created the work item.
				ClrSecurity.SetPermissions(null, 0);

				// Wait for and dispatch completion items.
				WorkItem item;
				lock(completionWait)
				{
					for(;;)
					{
						item = CompletionItemToDispatch();
						if(item == null)
						{
							Monitor.Wait(completionWait);
						}
						else
						{
							++usedCompletionThreads;
							Monitor.Exit(completionWait);
							item.Execute();
							Monitor.Enter(completionWait);
							--usedCompletionThreads;
						}
					}
				}
			}

	// Add a work item to the worker queue.
	private static void AddWorkItem(WorkItem item)
			{
				// Add the item to the end of the worker queue.
				lock(typeof(ThreadPool))
				{
					if(lastWorkItem != null)
					{
						lastWorkItem.next = item;
					}
					else
					{
						workItems = item;
					}
					lastWorkItem = item;
				}
				if(!Thread.CanStartThreads())
				{
					// We don't have threads, so execute the items now.
					WorkItem next = ItemToDispatch();
					while(next != null)
					{
						next.Execute();
						next = ItemToDispatch();
					}
				}
				else
				{
					// Determine if we need to spawn a new worker thread.
					lock(typeof(ThreadPool))
					{
						if(workItems != null &&
						   numWorkerThreads < MaxWorkerThreads)
						{
							if(workerThreads == null)
							{
								workerThreads = new Thread [MaxWorkerThreads];
							}
							Thread thread = new Thread(new ThreadStart(Work));
							workerThreads[numWorkerThreads++] = thread;
							thread.IsBackground = true;
							thread.Start();
						}
						Monitor.Pulse(typeof(ThreadPool));
					}
				}
			}

	// Add a work item to the completion queue.
	private static void AddCompletionItem(WorkItem item)
			{
				// Add the item to the end of the worker queue.
				lock(completionWait)
				{
					if(lastCompletionItem != null)
					{
						lastCompletionItem.next = item;
					}
					else
					{
						completionItems = item;
					}
					lastCompletionItem = item;
				}
				if(!Thread.CanStartThreads())
				{
					// We don't have threads, so execute the items now.
					WorkItem next = CompletionItemToDispatch();
					while(next != null)
					{
						next.Execute();
						next = CompletionItemToDispatch();
					}
				}
				else
				{
					// Determine if we need to spawn a new completion thread.
					lock(completionWait)
					{
						if(completionItems != null &&
						   numCompletionThreads < MaxCompletionThreads)
						{
							if(completionThreads == null)
							{
								completionThreads =
									new Thread [MaxCompletionThreads];
							}
							Thread thread =
								new Thread(new ThreadStart(Complete));
							completionThreads[numCompletionThreads++] = thread;
							thread.IsBackground = true;
							thread.Start();
						}
						Monitor.Pulse(completionWait);
					}
				}
			}

	// Structure of a work item.
	internal sealed class WorkItem
	{
		// Internal state.
		public WorkItem next;
		private ClrPermissions permissions;
		private bool userWorkItem;
		private WaitCallback callback;
		private WaitOrTimerCallback callback2;
		private AsyncCallback callback3;
		private Object state;
		private int timeout;
		private bool once;
		internal WaitHandle waitObject;
		internal bool registered;

		// Constructors.
		public WorkItem(ClrPermissions permissions,
						WaitCallback callback, Object state)
				{
					this.permissions = permissions;
					this.userWorkItem = true;
					this.callback = callback;
					this.state = state;
				}
		public WorkItem(ClrPermissions permissions,
						AsyncCallback callback, Object state)
				{
					this.permissions = permissions;
					this.userWorkItem = true;
					this.callback3 = callback;
					this.state = state;
				}
		public WorkItem(ClrPermissions permissions, WaitHandle waitObject,
					    WaitOrTimerCallback callback, Object state,
						int millisecondsTimeOutInterval, bool executeOnlyOnce)
				{
					this.permissions = permissions;
					this.userWorkItem = false;
					this.callback2 = callback;
					this.state = state;
					this.timeout = millisecondsTimeOutInterval;
					this.once = executeOnlyOnce;
					this.registered = true;
				}

		// Execute this work item.
		public void Execute()
				{
					try
					{
						// Set the permissions for this stack frame
						// to the caller's permission set.
						if(permissions != null)
						{
							ClrSecurity.SetPermissions(permissions, 0);
						}

						// Run the work item.
						if(userWorkItem)
						{
							if(callback != null)
							{
								callback(state);
							}
							else if(callback3 != null)
							{
								callback3((IAsyncResult)state);
							}
						}
						else
						{
							do
							{
								if(waitObject.WaitOne(timeout, false))
								{
									if(callback2 != null)
									{
										callback2(state, false);
									}
								}
								else if(registered)
								{
									if(callback2 != null)
									{
										callback2(state, true);
									}
								}
							}
							while(registered && !once);
						}
					}
					catch(Exception)
					{
						// Catch and ignore all exceptions.
					}
				}

	}; // class WorkItem

}; // class ThreadPool

}; // namespace System.Threading
