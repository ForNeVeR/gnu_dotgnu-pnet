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
using System.Threading;

#if !ECMA_COMPAT

public class TestThread
	: TestCase
{
	internal static bool isThreadingSupported;
	
	static TestThread()
	{
		try
		{
			Thread thread;
			
			thread = new Thread(new ThreadStart(NullMethod));
			
			thread.Start();
			
			isThreadingSupported = true;
			
		}
		catch (NotSupportedException)
		{
			isThreadingSupported = false;
		}
	}
	
	private static void NullMethod()
	{
	}
	
	internal static bool IsThreadingSupported
	{
		get
		{
			return isThreadingSupported;
		}
	}
	
	public TestThread(String name)
		: base(name)
	{
	}

	protected override void Setup()
	{
	}

	protected override void Cleanup()
	{
	}

	int flag = 0;
	bool failed = false;
	
	private void StartJoinRun()
	{
		flag += Convert.ToInt32(Thread.CurrentThread.Name);
		
		Thread.Sleep(200);
	}
	
	public void TestStartJoin()
	{
		if (!IsThreadingSupported)
		{
			return;
		}
		
		int expected;
		Thread thread;
		Thread[] threads = new Thread[10];
	
		flag = 0;
		expected = 0;
			
		for (int i = 0; i < 10; i++)
		{
			threads[i] = new Thread(new ThreadStart(StartJoinRun));

			threads[i].Name = i.ToString();
						
			threads[i].Start();
			
			expected += i;
		}
		
		for (int i = 0; i < 10; i++)
		{
			threads[i].Join();
		}
		
		AssertEquals("ThreadStartJoin failed", flag, expected);
	}

	private void StartRestartRun()
	{
	}
		
	public void TestStartRestart()
	{
		Thread thread;
		
		if (!IsThreadingSupported)
		{
			return;
		}
		
		thread = new Thread(new ThreadStart(StartRestartRun));
		
		thread.Start();
		
		thread.Join();

		try
		{		
			thread.Start();
			
			Assert("Thread illegally restarted", false);
		}
		catch (ThreadStateException)
		{
		}
	}	
	
	private void ThreadStateRun()
	{
		if (Thread.CurrentThread.ThreadState == ThreadState.Running)
		{
			failed = false;
		}
	}
	
	public void TestThreadState()
	{
		Thread thread;
		
		if (!IsThreadingSupported)
		{
			return;
		}
		
		failed = true;
		
		thread = new Thread(new ThreadStart(ThreadStateRun));
		
		thread.Start();
		thread.Join();
		
		Assert("ThreadState==ThreadState.Running", !failed);
		Assert("ThreadState==ThreadState.Stopped",
			 thread.ThreadState == ThreadState.Stopped);	
		
	}
}

#endif
