/*
 * TestBoolean.cs - Tests for the "Boolean" class.
 *
 * Copyright (C) 2002  Free Software Foundation.
 *
 * Authors: Thong Nguyen (tum@veridicus.com)
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

using CSUnit;
using System;
using System.Collections;
using System.Threading;

public class TestMonitor
	: TestCase
{
	public TestMonitor(String name)
		: base(name)
	{
	}

	protected override void Setup()
	{
	}

	protected override void Cleanup()
	{
	}

	public void TestMonitorSingleThreaded()
	{
		if (!TestThread.IsThreadingSupported)
		{
			return;
		}

		object o = new object();
		
		Monitor.Enter(o);
		Monitor.Enter(o);
		Monitor.Exit(o);
		Monitor.Exit(o);
	}	

	public void TestMonitorExitNoEnter()
	{
		if (!TestThread.IsThreadingSupported)
		{
			return;
		}
		
		object o = new object();
		
		try
		{
			Monitor.Exit(o);
			
			Assert("Expected SynchronizationLockException", false);
		}
		catch (SynchronizationLockException)
		{
		}
	}

	public void TestMonitorEnterExitMismatch()
	{
		if (!TestThread.IsThreadingSupported)
		{
			return;
		}
		
		object o = new object();

		try
		{
			Monitor.Enter(o);
			Monitor.Exit(o);
			Monitor.Exit(o);

			Assert("Expected SynchronizationLockException", false);
		}
		catch (SynchronizationLockException)
		{
		}
	}

	public void TestMonitorEnterExitMultiple()
	{
		if (!TestThread.IsThreadingSupported)
		{
			return;
		}
	
		object o1 = new object();
		object o2 = new object();
		
		Monitor.Enter(o1);
		Monitor.Enter(o2);
		Monitor.Exit(o1);
		Monitor.Exit(o2);
	}

	bool flag;
	object monitor = new object();
	
	private void ExclusionRun1()
	{
		lock (monitor)
		{		
			Thread.Sleep(1000);
			
			flag = true;
		}		
	}
		
	private void ExclusionRun2()
	{
		/* Wait for thread1 to obtain lock */
		
		Thread.Sleep(100);
		
		lock (monitor)
		{
			/* Fails if lock didn't wait for thread1 */
			
			failed = !flag;	
		}
	}
	
	public void TestMonitorExclusion()
	{
		if (!TestThread.IsThreadingSupported)
		{
			return;
		}
		
		Thread thread1, thread2;
		
		flag = false;
		failed = true;
		
		thread1 = new Thread(new ThreadStart(ExclusionRun1));
		thread2 = new Thread(new ThreadStart(ExclusionRun2));	
		
		thread1.Start();
		thread2.Start();

		thread1.Join();
		thread2.Join();
		
		Assert("Exclusion failed", !failed);
	}

	/*
	 * Variables used for monitor thrashing..
	 */

	private int state = 0;
	private bool failed = false;
	private bool stop = false;
	
	private void Run1()
	{
		while (!stop)
		{
			lock (typeof(TestMonitor))
			{
				if (state == 0 || state == 1)
				{
					// OK!				
				}
				else
				{
					failed = true;
					stop = true;

					break;
				}
			}
		}
	}
	
	private void Run2()
	{
		while (!stop)
		{
			lock (typeof(TestMonitor))
			{
				if (state == 1)
				{
					state = 0;
				}
				else
				{
					state = 1;
				}
			}			
		}
	}
	
	public void TestMonitorMultiThreadedThrash()
	{
		Thread thread1, thread2;
		
		if (!TestThread.IsThreadingSupported)
		{
			return;
		}
		
		thread1 = new Thread(new ThreadStart(Run1));
		thread2 = new Thread(new ThreadStart(Run2));

		state = 0;
		failed = false;
		stop = false;

		Console.Write("Thrashing will take 4 seconds ... ");
				
		thread1.Start();
		thread2.Start();
		
		/* Testing shows 4 seconds is *usually* enough to cause
			a failure if monitors aren't working */
					
		Thread.Sleep(4000);
		
		stop = true;

		thread1.Join();
		thread2.Join();
		
		AssertEquals("Monitor locking", failed, false);
	}

	public void TestMonitorExtremeMultiThreadedThrash()
	{
		Thread[] threads;
		
		if (!TestThread.IsThreadingSupported)
		{
			return;
		}

		state = 0;
		failed = false;
		stop = false;		

		threads = new Thread[10];

		Console.Write("Thrashing will take 4 seconds ... ");
				
		for (int i = 0; i < 10; i++)
		{
			if (i % 2 == 0)
			{
				threads[i] = new Thread(new ThreadStart(Run1));
			}
			else
			{
				threads[i] = new Thread(new ThreadStart(Run2));
			}
			
			threads[i].Start();
		}


		/* Testing shows 4 seconds is *usually* enough to cause
			a failure if monitors aren't working */
					
		Thread.Sleep(4000);
		
		stop = true;

		for (int i = 0; i < 10; i++)
		{
			threads[i].Join();
		}
		
		AssertEquals("Monitor locking", failed, false);
	}
	
	public void TestMonitorWaitWithoutLock()
	{
		object o = new object();
		
		try
		{
			Monitor.Wait(o);
		}
		catch (SynchronizationLockException)
		{
			return;
		}
		
		Fail("Monitor.Wait() without a lock should throw a synchronization lock exception");
	}
		
	public void TestMonitorWaitWithLockTimeout()
	{
		object o = new object();
		
		try
		{
			lock (o)
			{
				Monitor.Wait(o, 100);
			}
		}
		catch (SynchronizationLockException)
		{
			Fail("Monitor.Wait() without a lock should throw a synchronization lock exception");
			
			return;
		}	
	}
	
	private class TestEnterFalseLeave
	{
		bool e = false;
		public object o = new object();
		
		public void Run()
		{
			try
			{
				Monitor.Exit(o);
			}
			catch (SynchronizationLockException)
			{
				e = true;
			}
		}
		
		public bool Test()
		{
			Thread thread = new Thread(new ThreadStart(Run));
			
			Monitor.Enter(o);
						
			thread.Start();
			thread.Join();
			
			return e;
		}
	}
	
	public void TestMonitorEnterFalseLeave()
	{
		Assert("Monitor.Exit should throw an exception if the thread doesn't own the lock", new TestEnterFalseLeave().Test());
	}
	
	public class TestWaitThenPulse
	{
		private object o = new object();
		private ArrayList results = new ArrayList();
		
		private void AddResult(int value)
		{
			lock (this)
			{
				results.Add(value);
			}
		}
		
		public void Run1()
		{
			AddResult(1);
			
			lock (o)
			{
				AddResult(2);
				
				Monitor.Wait(o);
				
				AddResult(6);
			}
		}
		
		public void Run2()
		{
			Console.Write("Waiting 3 seconds before pulse ... ");

			Thread.Sleep(3000);

			AddResult(3);
			
			lock (o)
			{
				AddResult(4);
				
				Monitor.Pulse(o);
				
				AddResult(5);
			}		
		}
		
		public bool Test()
		{
			bool success = true;
			Thread thread = new Thread(new ThreadStart(Run2));
			
			thread.Start();
			Run1();
			
			if (results.Count == 6)
			{
				for (int i = 0; i < results.Count; i++)
				{
					if ((int)results[i] != i + 1)
					{
						success = false;
					}
				}
			}
			else
			{
				success = false;
			}
			
			return success;
		}
	}
	
	public void TestMonitorWaitThenPulse()
	{
		if (!TestThread.IsThreadingSupported)
		{
			return;
		}
		
		Assert("Monitor.Wait doesn't work", new TestWaitThenPulse().Test());
	}
}
