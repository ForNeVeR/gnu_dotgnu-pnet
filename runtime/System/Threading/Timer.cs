/*
 * Timer.cs - Implementation of the "System.Threading.Timer" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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
	//internal state
	private int dueTime,period;
	private Thread TimerThread;
	private TimerCallback callback;
	private object TimerObject;
	private TimeSpan ts_dt,ts_p;
	//Constructors
	public Timer(TimerCallback callback, object state, int dueTime, int period)
	{
		if(callback==null){
			throw new ArgumentNullException("callback");
		}
		if(dueTime < 0 && dueTime != Timeout.Infinite){
			throw new ArgumentOutOfRangeException("dueTime");
		}
		if(period < 0 && period != Timeout.Infinite){
			throw new ArgumentOutOfRangeException("period");
		}
		this.callback=callback;
		this.dueTime=dueTime;
		this.period=period;
		ThreadStart TimerDelegate = new ThreadStart(TimerFunction);
		this.TimerThread = new Thread(TimerDelegate);
		if(dueTime!=Timeout.Infinite)
			TimerThread.Start();
	}

	public Timer(TimerCallback callback, object state, TimeSpan dueTime, TimeSpan period)
	{
		if(callback==null){
			throw new ArgumentNullException("callback");
		}
		if((dueTime.Milliseconds < 0 && dueTime.Ticks != Timeout.Infinite) || (dueTime.Milliseconds > System.Int32.MaxValue)){
			throw new ArgumentOutOfRangeException("dueTime");
		}
		if((period.Milliseconds < 0 && period.Ticks != Timeout.Infinite) || ( dueTime.Milliseconds > System.Int32.MaxValue)){
			throw new ArgumentOutOfRangeException("period");
		}
		this.callback=callback;
		this.dueTime=dueTime.Milliseconds;
		this.period=period.Milliseconds;
		this.ts_dt=dueTime;
		this.ts_p=period;
		ThreadStart TimerDelegate = new ThreadStart(TimerFunction);
		this.TimerThread = new Thread(TimerDelegate);
		if(dueTime.Ticks!=Timeout.Infinite)
			TimerThread.Start();

	}

	[TODO]
	// Destructor for Timer
	~Timer()
	{
	}

	[TODO]
	// Changes the start time and interval between method invocations for a timer.
	public bool Change(int dueTime, int period)
	{
		if(dueTime < 0 && dueTime != Timeout.Infinite){
			throw new ArgumentOutOfRangeException("dueTime");
		}
		if(period < 0 && period != Timeout.Infinite){
			throw new ArgumentOutOfRangeException("period");
		}
		this.dueTime=dueTime;
		this.period=period;
		return true;
	}

	[TODO]
	//Changes the start time and interval between method invocations for a timer.
	public bool Change(TimeSpan dueTime, TimeSpan period)
	{
		if((dueTime.Milliseconds < 0 && dueTime.Ticks != Timeout.Infinite) || (dueTime.Milliseconds > System.Int32.MaxValue)){
			throw new ArgumentOutOfRangeException("dueTime");
		}
		if((period.Milliseconds < 0 && period.Ticks != Timeout.Infinite) || ( dueTime.Milliseconds > System.Int32.MaxValue)){
			throw new ArgumentOutOfRangeException("period");
		}
		return Change(dueTime.Milliseconds, period.Milliseconds);
	}

	[TODO]
	//Releases the resources held by the current instance.
	public void Dispose()
	{
	}

	// Call the functions of timer
	private void callFunctions()
	{
		Delegate [] TimerFunctionList;
		TimerFunctionList = callback.GetInvocationList();
		TimerCallback TimerMethod;
		for(int i=0;i<TimerFunctionList.Length;i++)
		{
			TimerMethod=(TimerCallback)TimerFunctionList[i];
			TimerMethod(TimerObject);
		}
	}

	// start function for the timer thread.
	private static void TimerFunction()
	{
		// TODO: I had to comment this out because dueTime and callFunctions
		// are not static - Rhys.
		//if(dueTime!=0)
		//	Thread.Sleep(dueTime);
		//callFunctions();
		//while(period!=0 && period !=Timeout.Infinite)
		//{
		//	Thread.Sleep(period);
		//	callFunctions();
		//}
	}

	[TODO]
	//Releases the resources held by the current instance.
	public bool Dispose(WaitHandle notifyObject)
	{
		return true;
	}

}; // class Timer

}; // namespace System.Threading
