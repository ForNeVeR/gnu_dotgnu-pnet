/*
 * Timer.cs - Implementation of the "System.Threading.Timer" class.
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

using System;

public sealed class Timer : MarshalByRefObject, IDisposable
{
	// Internal state.
	private Thread timerThread;
	private DateTime nextDue;
	private TimeSpan period;
	private TimerCallback callback;
	private Object state;
	private bool shutdown;
	private WaitHandle notifyObject;

	// Constructor.
	public Timer(TimerCallback callback, Object state, int dueTime, int period)
			{
				// Validate the parameters.
				if(callback == null)
				{
					throw new ArgumentNullException("callback");
				}
				if(dueTime < -1)
				{
					throw new ArgumentOutOfRangeException
						("dueTime", _("ArgRange_NonNegOrNegOne"));
				}
				if(period < -1)
				{
					throw new ArgumentOutOfRangeException
						("period", _("ArgRange_NonNegOrNegOne"));
				}

				// Initialize the timer state.
				if(dueTime == -1)
				{
					nextDue = DateTime.MaxValue;
				}
				else
				{
					nextDue = DateTime.UtcNow +
						new TimeSpan(dueTime * TimeSpan.TicksPerMillisecond);
				}
				if(period <= 0)
				{
					this.period = TimeSpan.MaxValue;
				}
				else
				{
					this.period = new TimeSpan
						(period * TimeSpan.TicksPerMillisecond);
				}
				this.callback = callback;
				this.state = state;
				this.shutdown = false;

				// Start the timer thread.
				timerThread = new Thread(new ThreadStart(Run));
				timerThread.IsBackground = true;
				timerThread.Start();
			}
	public Timer(TimerCallback callback, Object state,
				 TimeSpan dueTime, TimeSpan period)
			: this(callback, state,
				   Monitor.TimeSpanToMS(dueTime),
				   Monitor.TimeSpanToMS(period))
			{
				// Nothing to do here.
			}
#if !ECMA_COMPAT
	[CLSCompliant(false)]
	public Timer(TimerCallback callback, Object state,
				 uint dueTime, uint period)
			: this(callback, state, UIntToMS(dueTime), UIntToMS(period))
			{
				// Nothing to do here.
			}
	public Timer(TimerCallback callback, Object state,
				 long dueTime, long period)
			: this(callback, state, LongToMS(dueTime), LongToMS(period))
			{
				// Nothing to do here.
			}
#endif // !ECMA_COMPAT

	// Destructor.
	~Timer()
			{
				DisposeInternal(null);
			}

	// Change the current timer parameters.
	public bool Change(int dueTime, int period)
			{
				// Validate the parameters.
				if(callback == null)
				{
					throw new ArgumentNullException("callback");
				}
				if(dueTime < -1)
				{
					throw new ArgumentOutOfRangeException
						("dueTime", _("ArgRange_NonNegOrNegOne"));
				}
				if(period < -1)
				{
					throw new ArgumentOutOfRangeException
						("period", _("ArgRange_NonNegOrNegOne"));
				}

				// We must lock down this object while we modify it.
				lock(this)
				{
					if(timerThread != null)
					{
						// Change the timer state.
						if(dueTime == -1)
						{
							nextDue = DateTime.MaxValue;
						}
						else
						{
							nextDue = DateTime.UtcNow +
								new TimeSpan
									(dueTime * TimeSpan.TicksPerMillisecond);
						}
						if(period <= 0)
						{
							this.period = TimeSpan.MaxValue;
						}
						else
						{
							this.period = new TimeSpan
								(period * TimeSpan.TicksPerMillisecond);
						}

						// Signal the timer thread to make it wake up
						// and start using the new timeout values.
						Monitor.Pulse(this);
						return true;
					}
					else
					{
						return false;
					}
				}
			}
	public bool Change(TimeSpan dueTime, TimeSpan period)
			{
				return Change(Monitor.TimeSpanToMS(dueTime),
							  Monitor.TimeSpanToMS(period));
			}
#if !ECMA_COMPAT
	[CLSCompliant(false)]
	public bool Change(uint dueTime, uint period)
			{
				return Change(UIntToMS(dueTime), UIntToMS(period));
			}
	public bool Change(long dueTime, long period)
			{
				return Change(LongToMS(dueTime), LongToMS(period));
			}
#endif // !ECMA_COMPAT

	// Dispose of this object.
	public void Dispose()
			{
				DisposeInternal(null);
				GC.SuppressFinalize(this);
			}

	// Dispose of this object and signal a particular wait
	// handle once the timer has been disposed.
	public bool Dispose(WaitHandle notifyObject)
			{
				if(notifyObject == null)
				{
					throw new ArgumentNullException("notifyObject");
				}
				return DisposeInternal(notifyObject);
			}

	// Internal version of "Dispose".
	private bool DisposeInternal(WaitHandle notifyObject)
			{
				lock(this)
				{
					if(timerThread != null)
					{
						this.notifyObject = notifyObject;
						this.shutdown = true;
						this.timerThread = null;
						Monitor.Pulse(this);
						return true;
					}
					else
					{
						return false;
					}
				}
			}

#if !ECMA_COMPAT

	// Convert an unsigned integer value into a milliseconds value.
	private static int UIntToMS(uint value)
			{
				if(value > (uint)(Int32.MaxValue))
				{
					throw new ArgumentOutOfRangeException
						("value", _("ArgRange_NonNegOrNegOne"));
				}
				return (int)value;
			}

	// Convert a long integer value into a milliseconds value.
	private static int LongToMS(long value)
			{
				if(value < (-1L) || value > (long)(Int32.MaxValue))
				{
					throw new ArgumentOutOfRangeException
						("value", _("ArgRange_NonNegOrNegOne"));
				}
				return (int)value;
			}

#endif // !ECMA_COMPAT

	// Method that runs the timer thread.
	private void Run()
			{
				long maxTime = DateTime.MaxValue.Ticks;
				TimeSpan waitTime;
				long longMS, ticks;
				int ms;
				lock(this)
				{
					while(!shutdown)
					{
						// Determine how long to wait until the next
						// timeout is due.
						waitTime = nextDue - DateTime.UtcNow;
						longMS = waitTime.Ticks / TimeSpan.TicksPerMillisecond;
						if(longMS < 0)
						{
							ms = 0;
						}
						else if(longMS > (long)(Int32.MaxValue))
						{
							ms = -1;
						}
						else
						{
							ms = (int)longMS;
						}

						// Wait until the monitor is signalled or times out.
						// This will give up the lock while we are waiting.
						// We will be signalled by the main thread when
						// it wants us to change the timeout or shutdown.
						if(!Monitor.Wait(this, ms))
						{
							// Set "nextDue" to the current time.  This is
							// to prevent the thread from issuing multiple
							// timeouts in quick succession because it has
							// become backlogged, or because the user changed
							// the system clock to warp us into the past.
							nextDue = DateTime.UtcNow;

							// Invoke the timer callback.
							try
							{
								callback(state);
							}
							catch(Exception)
							{
								// Ignore exceptions, to prevent the
								// timer thread from exiting prematurely.
							}

							// Determine when the next timeout should occur.
							if(period == TimeSpan.MaxValue)
							{
								nextDue = DateTime.MaxValue;
							}
							else
							{
								ticks = nextDue.Ticks + period.Ticks;
								if(((ulong)ticks) > ((ulong)maxTime))
								{
									nextDue = DateTime.MaxValue;
								}
								else
								{
									nextDue = new DateTime(ticks);
								}
							}
						}
					}
					if(notifyObject != null)
					{
						// TODO: signal notifyObject that we are exiting.
					}
				}
			}

}; // class Timer

}; // namespace System.Threading
