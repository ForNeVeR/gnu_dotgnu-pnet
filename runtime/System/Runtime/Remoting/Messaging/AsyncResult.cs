/*
 * AsyncResult.cs - Implementation of the
 *			"System.Runtime.Remoting.Messaging.AsyncResult" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.Runtime.Remoting.Messaging
{

#if CONFIG_REFLECTION

using System;
using System.Threading;

// This class is not ECMA-compatible, strictly speaking.  But it
// is required to implement asynchronous delegates.

public class AsyncResult : IAsyncResult
{
	// Internal state.
	private Object del;
	private Object[] args;
	private AsyncCallback callback;
	private Object state;
	private Object result;
	private Exception resultException;
	private bool synchronous;
	private bool completed;
	private bool endInvokeCalled;

	// Construct a new asynchronous result object and begin invocation.
	[TODO]
	internal AsyncResult(Object del, Object[] args,
						 AsyncCallback callback, Object state)
			{
				// Initialize the fields within this class.
				this.del = del;
				this.args = args;
				this.callback = callback;
				this.state = state;
				this.result = null;
				this.resultException = null;
				this.synchronous = false;
				this.completed = false;
				this.endInvokeCalled = false;

				// TODO: create a new thread to perform the async call.

				// We don't have threads, so call the delegate synchronously.
				this.synchronous = true;
				try
				{
					this.result = ((Delegate)del).DynamicInvoke(args);
				}
				catch(Exception e)
				{
					this.resultException = e;
				}
				this.completed = true;
				if(callback != null)
				{
					callback(this);
				}
			}

	// Get the delegate that was invoked.
	public virtual Object AsyncDelegate
			{
				get
				{
					return del;
				}
			}

	// Get the state information for a BeginInvoke callback.
	public virtual Object AsyncState
			{
				get
				{
					return state;
				}
			}

	// Get a wait handle that can be used to wait for the
	// asynchronous delegate call to complete.
	[TODO]
	public virtual WaitHandle AsyncWaitHandle
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Determine if the call completed synchronously.
	public virtual bool CompletedSynchronously
			{
				get
				{
					lock(this)
					{
						return synchronous;
					}
				}
			}

	// Get or set the state which represents if "EndInvoke"
	// has been called for the delegate.
	public bool EndInvokeCalled
			{
				get
				{
					lock(this)
					{
						return endInvokeCalled;
					}
				}
				set
				{
					// Not implemented - use "EndInvoke" instead.
				}
			}

	// Determine if the call has completed yet.
	public virtual bool IsCompleted
			{
				get
				{
					lock(this)
					{
						return completed;
					}
				}
			}

	// End invocation on the delegate in this object.
	[TODO]
	internal Object EndInvoke()
			{
				lock(this)
				{
					if(synchronous)
					{
						endInvokeCalled = true;
						if(resultException != null)
						{
							throw resultException;
						}
						else
						{
							return result;
						}
					}
					else
					{
						if(endInvokeCalled)
						{
							if(resultException != null)
							{
								throw resultException;
							}
							else
							{
								return result;
							}
						}
						// TODO: wait for the async call to complete.
						endInvokeCalled = true;
						if(resultException != null)
						{
							throw resultException;
						}
						else
						{
							return result;
						}
					}
				}
			}

}; // class AsyncResult

#endif // CONFIG_REFLECTION

}; // namespace System.Runtime.Remoting.Messaging
