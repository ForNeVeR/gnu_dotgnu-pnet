/*
 * Application.cs - Implementation of the
 *			"System.Windows.Forms.Application" class.
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

namespace System.Windows.Forms
{

using Microsoft.Win32;
using System.Globalization;
using System.Threading;
using System.IO;
using System.Drawing.Toolkit;

public sealed class Application
{
	// Cannot instantiate this class.
	private Application() {}

#if !CONFIG_COMPACT_FORMS

	// Internal state.
	[ThreadStatic] private static InputLanguage inputLanguage;
	private static String safeTopLevelCaptionFormat = "{0}";

	// Determine if it is possible to quit this application.
	public static bool AllowQuit
			{
				get
				{
					// Returns false for applet usage, which we don't have yet.
					return true;
				}
			}

	// Get the common data path for data shared between all users.
	[TODO]
	public static String CommonAppDataPath
			{
				get
				{
					// TODO
					return null;
				}
			}

#if CONFIG_WIN32_SPECIFICS

	// Get the common registry key for data shared between all users.
	[TODO]
	public static RegistryKey CommonAppDataRegistry
			{
				get
				{
					// TODO
					return null;
				}
			}

#endif

	// Get the company name for this application
	[TODO]
	public static String CompanyName
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get or set the culture for the current thread.
	public static CultureInfo CurrentCulture
			{
				get
				{
					return CultureInfo.CurrentCulture;
				}
				set
				{
				#if !ECMA_COMPAT
					Thread.CurrentThread.CurrentCulture = value;
				#endif
				}
			}

	// Get or set the input language for the current thread.
	public static InputLanguage CurrentInputLanguage
			{
				get
				{
					return inputLanguage;
				}
				set
				{
					inputLanguage = value;
				}
			}

	// Get the executable path for this application.
	public static String ExecutablePath
			{
				get
				{
					return (Environment.GetCommandLineArgs())[0];
				}
			}

	// Get the local user application data path.
	[TODO]
	public static String LocalUserAppDataPath
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Determine if a message loop exists on this thread.
	public static bool MessageLoop
			{
				get
				{
					return true;
				}
			}

	// Get the product name associated with this application.
	[TODO]
	public static String ProductName
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get the product version associated with this application.
	[TODO]
	public static String ProductVersion
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get or set the top-level warning caption format.
	public static String SafeTopLevelCaptionFormat
			{
				get
				{
					return safeTopLevelCaptionFormat;
				}
				set
				{
					safeTopLevelCaptionFormat = value;
				}
			}

	// Get the startup path for the executable.
	public static String StartupPath
			{
				get
				{
					return Path.GetDirectoryName(ExecutablePath);
				}
			}

	// Get the user application data path.
	[TODO]
	public static String UserAppDataPath
			{
				get
				{
					// TODO
					return null;
				}
			}

#if CONFIG_WIN32_SPECIFICS

	// Get the registry key for user-specific data.
	[TODO]
	public static RegistryKey UserAppDataRegistry
			{
				get
				{
					// TODO
					return null;
				}
			}

#endif

	// Add a message filter.
	public static void AddMessageFilter(IMessageFilter value)
			{
				// We don't use message filters in this implementation.
			}

	// Enable Windows XP visual styles.
	public static void EnableVisualStyles()
			{
				// Not used in this implementation.
			}

	// Exit the message loop on the current thread and close all windows.
	public static void ExitThread()
			{
				// We only allow one message loop in this implementation,
				// so "ExitThread" is the same as "Exit".
				Exit();
			}

#if !ECMA_COMPAT

	// Initialize OLE on the current thread.
	public static ApartmentState OleRequired()
			{
				// Not used in this implementation.
				return ApartmentState.Unknown;
			}

#endif

	// Raise the thread exception event.
	public static void OnThreadException(Exception t)
			{
				if(ThreadException != null)
				{
					ThreadException(null, new ThreadExceptionEventArgs(t));
				}
			}

	// Remove a message filter.
	public static void RemoveMessageFilter(IMessageFilter value)
			{
				// We don't use message filters in this implementation.
			}

	// Event that is raised when the application is about to exit.
	public static event EventHandler ApplicationExit;

	// Event that is raised when the message loop is entering the idle state.
	public static event EventHandler Idle;

	// Event that is raised for an untrapped thread exception.
	public static event ThreadExceptionEventHandler ThreadException;

	// Event that is raised when the current thread is about to exit.
	public static event EventHandler ThreadExit;

#endif // !CONFIG_COMPACT_FORMS

	// The thread that is running the main message loop.
	private static Thread mainThread;

	// Process all events that are currently in the message queue.
	public static void DoEvents()
			{
				// Make sure that this thread has a message loop running.
				lock(typeof(Application))
				{
					if(mainThread != Thread.CurrentThread)
					{
						return;
					}
				}

				// Process pending events.
				ToolkitManager.Toolkit.ProcessEvents(false);
			}

	// Tell the application to exit.
	public static void Exit()
			{
				lock(typeof(Application))
				{
					if(mainThread != null)
					{
						ToolkitManager.Toolkit.Quit();
					}
				}
			}

	// Inner version of "Run".  In this implementation we only allow a
	// message loop to be running on one of the threads.
	private static void RunMessageLoop()
			{
				// Make sure that we are the only message loop.
				lock(typeof(Application))
				{
					if(mainThread != null)
					{
						throw new InvalidOperationException
							(S._("SWF_MessageLoopAlreadRunning"));
					}
					mainThread = Thread.CurrentThread;
				}

				// Run the main message processing loop.
				IToolkit toolkit = ToolkitManager.Toolkit;
				try
				{
					for(;;)
					{
						// Process events in the queue.
						if(!toolkit.ProcessEvents(false))
						{
						#if !CONFIG_COMPACT_FORMS
							// There were no events, so raise "Idle".
							if(Idle != null)
							{
								Idle(null, EventArgs.Empty);
							}
						#endif

							// Block until the next event, or quit, arrives.
							if(!toolkit.ProcessEvents(true))
							{
								break;
							}
						}
					}
				}
				finally
				{
					// Reset the "mainThread" variable because there
					// is no message loop any more.
					lock(typeof(Application))
					{
						mainThread = null;
					}
				}

			#if !CONFIG_COMPACT_FORMS

				// Raise the "ThreadExit" event.
				if(ThreadExit != null)
				{
					ThreadExit(null, EventArgs.Empty);
				}

				// Raise the "ApplicationExit" event.
				if(ApplicationExit != null)
				{
					ApplicationExit(null, EventArgs.Empty);
				}

			#endif
			}

	// Make the specified form visible and run the main loop.
	// The loop will exit when "Exit" is called.
	[TODO]
	public static void Run(Form mainForm)
			{
				// TODO
				if(mainForm != null)
				{
					mainForm.Show();
					Form.activeForm = mainForm;
				}
				RunMessageLoop();
			}

#if !CONFIG_COMPACT_FORMS

	// Run the main message loop on this thread.
	public static void Run()
			{
				RunMessageLoop();
			}

	// Run the main message loop for an application context.
	[TODO]
	public static void Run(ApplicationContext context)
			{
				// TODO
				RunMessageLoop();
			}

#endif // !CONFIG_COMPACT_FORMS

}; // class Application

}; // namespace System.Windows.Forms
