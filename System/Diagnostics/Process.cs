/*
 * Process.cs - Implementation of "System.Diagnostics.Process" 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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
using System.IO;

namespace System.Diagnostics
{
	[TODO]
	public class Process
	{
		[TODO]
		public Process()
		{
			throw new NotImplementedException(".ctor");
		}

		[TODO]
		public void Close()
		{
			throw new NotImplementedException("Close");
		}

		[TODO]
		public bool CloseMainWindow()
		{
			throw new NotImplementedException("CloseMainWindow");
		}

		[TODO]
		protected virtual void Dispose(bool disposing)
		{
			throw new NotImplementedException("Dispose");
		}

		[TODO]
		public static void EnterDebugMode()
		{
			throw new NotImplementedException("EnterDebugMode");
		}

		[TODO]
		public static Process GetCurrentProcess()
		{
			throw new NotImplementedException("GetCurrentProcess");
		}

		[TODO]
		public static Process GetProcessById(int processId)
		{
			throw new NotImplementedException("GetProcessById");
		}

		[TODO]
		public static Process GetProcessById(int processId, String machineName)
		{
			throw new NotImplementedException("GetProcessById");
		}

		[TODO]
		public static Process[] GetProcesses()
		{
			throw new NotImplementedException("GetProcesses");
		}

		[TODO]
		public static Process[] GetProcesses(String machineName)
		{
			throw new NotImplementedException("GetProcesses");
		}

		[TODO]
		public static Process[] GetProcessesByName(String processName)
		{
			throw new NotImplementedException("GetProcessesByName");
		}

		[TODO]
		public static Process[] GetProcessesByName(String processName, 
													String machineName)
		{
			throw new NotImplementedException("GetProcessesByName");
		}

		[TODO]
		public void Kill()
		{
			throw new NotImplementedException("Kill");
		}

		[TODO]
		public static void LeaveDebugMode()
		{
			throw new NotImplementedException("LeaveDebugMode");
		}

		[TODO]
		protected void OnExited()
		{
			throw new NotImplementedException("OnExited");
		}

		[TODO]
		public void Refresh()
		{
			throw new NotImplementedException("Refresh");
		}

		[TODO]
		public bool Start()
		{
			throw new NotImplementedException("Start");
		}

		[TODO]
		public static Process Start(ProcessStartInfo startInfo)
		{
			throw new NotImplementedException("Start");
		}

		[TODO]
		public static Process Start(String fileName)
		{
			throw new NotImplementedException("Start");
		}

		[TODO]
		public static Process Start(String fileName, String arguments)
		{
			throw new NotImplementedException("Start");
		}

		[TODO]
		public override String ToString()
		{
			throw new NotImplementedException("ToString");
		}

		[TODO]
		public void WaitForExit()
		{
			throw new NotImplementedException("WaitForExit");
		}

		[TODO]
		public bool WaitForExit(int milliseconds)
		{
			throw new NotImplementedException("WaitForExit");
		}

		[TODO]
		public bool WaitForInputIdle()
		{
			throw new NotImplementedException("WaitForInputIdle");
		}

		[TODO]
		public bool WaitForInputIdle(int milliseconds)
		{
			throw new NotImplementedException("WaitForInputIdle");
		}

		[TODO]
		public int BasePriority 
		{ 
			get 
			{
				throw new NotImplementedException("BasePriority");
			}
		}
		[TODO]
		public bool EnableRaisingEvents 
		{ 
			get 
			{
				throw new NotImplementedException("EnableRaisingEvents");
			}

			set 
			{
				throw new NotImplementedException("EnableRaisingEvents");
			}
		}
		[TODO]
		public int ExitCode 
		{ 
			get 
			{
				throw new NotImplementedException("ExitCode");
			}
		}
		[TODO]
		public DateTime ExitTime 
		{ 
			get
			{
				throw new NotImplementedException("ExitTime");
			}
		}
		[TODO]
		public IntPtr Handle 
		{ 
			get
			{
				throw new NotImplementedException("Handle");
			}
		}
		[TODO]
		public int HandleCount 
		{ 
			get
			{
				throw new NotImplementedException("HandleCount");
			}
		}
		[TODO]
		public bool HasExited 
		{ 
			get
			{
				throw new NotImplementedException("HasExited");
			}
		}
		[TODO]
		public int Id 
		{ 
			get
			{
				throw new NotImplementedException("Id");
			}
		}
		[TODO]
		public String MachineName 
		{ 
			get
			{
				throw new NotImplementedException("MachineName");
			}
		}
		[TODO]
		public int MainModule 
		{ 
			get
			{
				throw new NotImplementedException("MainModule");
			}
		}
		[TODO]
		public IntPtr MainWindowHandle 
		{ 
			get
			{
				throw new NotImplementedException("MainWindowHandle");
			}
		}
		[TODO]
		public String MainWindowTitle 
		{ 
			get
			{
				throw new NotImplementedException("MainWindowTitle");
			}
		}
		[TODO]
		public IntPtr MaxWorkingSet 
		{ 
			get
			{
				throw new NotImplementedException("MaxWorkingSet");
			}
		 
			set
			{
				throw new NotImplementedException("MaxWorkingSet");
			}
		}
		[TODO]
		public IntPtr MinWorkingSet 
		{ 
			get
			{
				throw new NotImplementedException("MinWorkingSet");
			}
		 
			set
			{
				throw new NotImplementedException("MinWorkingSet");
			}
		}
		[TODO]
		public int Modules 
		{ 
			get
			{
				throw new NotImplementedException("Modules");
			}
		}
		[TODO]
		public int NonpagedSystemMemorySize 
		{ 
			get
			{
				throw new NotImplementedException("NonpagedSystemMemorySize");
			}
		}
		[TODO]
		public int PagedMemorySize 
		{ 
			get
			{
				throw new NotImplementedException("PagedMemorySize");
			}
		}
		[TODO]
		public int PagedSystemMemorySize 
		{ 
			get
			{
				throw new NotImplementedException("PagedSystemMemorySize");
			}
		}
		[TODO]
		public int PeakPagedMemorySize 
		{ 
			get
			{
				throw new NotImplementedException("PeakPagedMemorySize");
			}
		}
		[TODO]
		public int PeakVirtualMemorySize 
		{ 
			get
			{
				throw new NotImplementedException("PeakVirtualMemorySize");
			}
		}
		[TODO]
		public int PeakWorkingSet 
		{ 
			get
			{
				throw new NotImplementedException("PeakWorkingSet");
			}
		}
		[TODO]
		public bool PriorityBoostEnabled 
		{ 
			get
			{
				throw new NotImplementedException("PriorityBoostEnabled");
			}
		 
			set
			{
				throw new NotImplementedException("PriorityBoostEnabled");
			}
		}
		[TODO]
		public int PriorityClass 
		{ 
			get
			{
				throw new NotImplementedException("PriorityClass");
			}
		 
			set
			{
				throw new NotImplementedException("PriorityClass");
			}
		}
		[TODO]
		public int PrivateMemorySize 
		{ 
			get
			{
				throw new NotImplementedException("PrivateMemorySize");
			}
		}
		[TODO]
		public TimeSpan PrivilegedProcessorTime 
		{ 
			get
			{
				throw new NotImplementedException("PrivilegedProcessorTime");
			}
		}
		[TODO]
		public String ProcessName 
		{ 
			get
			{
				throw new NotImplementedException("ProcessName");
			}
		}
		[TODO]
		public IntPtr ProcessorAffinity 
		{ 
			get
			{
				throw new NotImplementedException("ProcessorAffinity");
			}
		 
			set
			{
				throw new NotImplementedException("ProcessorAffinity");
			}
		}
		[TODO]
		public bool Responding 
		{ 
			get
			{
				throw new NotImplementedException("Responding");
			}
		}
		[TODO]
		public StreamReader StandardError 
		{ 
			get
			{
				throw new NotImplementedException("StandardError");
			}
		}
		[TODO]
		public StreamWriter StandardInput 
		{ 
			get
			{
				throw new NotImplementedException("StandardInput");
			}
		}
		[TODO]
		public StreamReader StandardOutput 
		{ 
			get
			{
				throw new NotImplementedException("StandardOutput");
			}
		}
		[TODO]
		public ProcessStartInfo StartInfo 
		{ 
			get
			{
				throw new NotImplementedException("StartInfo");
			}
		 
			set
			{
				throw new NotImplementedException("StartInfo");
			}
		}
		[TODO]
		public DateTime StartTime 
		{ 
			get
			{
				throw new NotImplementedException("StartTime");
			}
		}
		[TODO]
		public int SynchronizingObject 
		{ 
			get
			{
				throw new NotImplementedException("SynchronizingObject");
			}
		 
			set
			{
				throw new NotImplementedException("SynchronizingObject");
			}
		}
		[TODO]
		public int Threads 
		{ 
			get
			{
				throw new NotImplementedException("Threads");
			}
		}
		[TODO]
		public TimeSpan TotalProcessorTime 
		{ 
			get
			{
				throw new NotImplementedException("TotalProcessorTime");
			}
		}
		[TODO]
		public TimeSpan UserProcessorTime 
		{ 
			get
			{
				throw new NotImplementedException("UserProcessorTime");
			}
		}
		[TODO]
		public int VirtualMemorySize 
		{ 
			get
			{
				throw new NotImplementedException("VirtualMemorySize");
			}
		}
		[TODO]
		public int WorkingSet 
		{ 
			get
			{
				throw new NotImplementedException("WorkingSet");
			}
		}
	}
}//namespace
