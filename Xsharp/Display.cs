/*
 * Display.cs - Access an X display server.
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
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

namespace Xsharp
{

using System;
using System.Collections;
using System.Threading;
using Xsharp.Events;

/// <summary>
/// <para>The <see cref="T:Xsharp.Display"/> class manages connections
/// to X display servers.</para>
///
/// <para>The application normally obtains a <see cref="T:Xsharp.Display"/>
/// instance by creating an instance of <see cref="T:Xsharp.Application"/>.
/// </para>
/// </summary>
public sealed class Display : IDisposable
{
	// Internal state.
	internal IntPtr dpy;
	private String displayName;
	private Screen[] screens;
	private int defaultScreen;
	private Application app;
	private bool quit;
	private bool pendingExposes;
	private InputOutputWidget exposeList;
	private Xlib.Cursor[] cursors;
	internal Xlib.Time knownEventTime;
	internal Hashtable handleMap;
	private static bool threadsInited;
	internal Xlib.Atom wmProtocols;
	internal Xlib.Atom wmDeleteWindow;
	internal Xlib.Atom wmTakeFocus;
	internal Xlib.Atom wmMwmHints;
	internal ButtonName selectButton;
	internal ButtonName menuButton;
	internal Hashtable fonts;
	internal BuiltinBitmaps bitmaps;
	internal Timer timerQueue;
	internal IntPtr imlibData;

	// Constructor.
	private Display(IntPtr dpy, String displayName, Application app)
			{
				// Copy parameters in from the create process.
				this.dpy = dpy;
				this.displayName = displayName;
				this.app = app;

				// Create objects for each of the display screens.
				int nscreens = (int)(Xlib.XScreenCount(dpy));
				screens = new Screen [nscreens];
				for(int scr = 0; scr < nscreens; ++scr)
				{
					screens[scr] = new Screen
						(this, scr, Xlib.XScreenOfDisplay(dpy, scr));
				}

				// Get the index of the default screen.
				defaultScreen = (int)(Xlib.XDefaultScreen(dpy));

				// Create an array to hold the standard cursors.
				cursors = new Xlib.Cursor [(int)(CursorType.XC_num_glyphs)];

				// Reset the time of the last known event.
				knownEventTime = Xlib.Time.CurrentTime;

				// Construct the window handle map if not already present.
				if(handleMap == null)
				{
					handleMap = new Hashtable();
				}

				// Initialize the standard window manager atoms that we use.
				wmProtocols = Xlib.XInternAtom
					(dpy, "WM_PROTOCOLS", Xlib.Bool.False);
				wmDeleteWindow = Xlib.XInternAtom
					(dpy, "WM_DELETE_WINDOW", Xlib.Bool.False);
				wmTakeFocus = Xlib.XInternAtom
					(dpy, "WM_TAKE_FOCUS", Xlib.Bool.False);
				wmMwmHints = Xlib.XInternAtom
					(dpy, "_MOTIF_WM_HINTS", Xlib.Bool.False);

				// Which buttons should we use for "Select" and "Menu"?
				byte[] buttons = new byte [5];
				if(Xlib.XGetPointerMapping(dpy, buttons, 5) == 3)
				{
					menuButton = ButtonName.Button3;
				}
				else
				{
					menuButton = ButtonName.Button2;
				}
				selectButton = ButtonName.Button1;

				// Construct the font map.
				fonts = new Hashtable();

				// Load the builtin bitmaps.
				bitmaps = new BuiltinBitmaps(this);
			}

	/// <summary>
	/// <para>Close the X display connection if it is currently active.</para>
	/// </summary>
	~Display()
			{
				Close();
			}

	/// <summary>
	/// <para>Close the X display connection if it is currently active.</para>
	/// </summary>
	///
	/// <remarks>
	/// <para>This method implements the <see cref="T:System.IDisposable"/>
	/// interface.</para>
	/// </remarks>
	public void Dispose()
			{
				Close();
			}

	// Internal version of "Open()" that is called once the
	// type lock has been acquired.
	private static Display OpenInternal(String displayName, Application app)
			{
				try
				{
					try
					{
						// Initialize Xlib thread support.
						if(!threadsInited)
						{
							threadsInited = true;
							Xlib.XInitThreads();
						}
					}
					catch(MissingMethodException)
					{
						// The "X11" library may not have support for
						// threads, which is OK(-ish) because we will
						// be locking every access to the display anyway.
					}
					IntPtr dpy = Xlib.XOpenDisplay(displayName);
					if(dpy != IntPtr.Zero)
					{
						// We have opened the display successfully.
						return new Display(dpy, displayName, app);
					}
					else
					{
						// We were unable to connect to the display.
						if(displayName != null)
						{
							throw new XCannotConnectException
								(String.Format(S._("X_CannotOpenTo"),
											   displayName));
						}
						else
						{
							throw new XCannotConnectException
								(S._("X_CannotOpen"));
						}
					}
				}
				catch(MissingMethodException)
				{
					// The engine was unable to locate "XOpenDisplay",
					// so we probably don't have an X library, or it
					// is not on the LD_LIBRARY_PATH.
					throw new XCannotConnectException
						(S._("X_LibraryNotPresent"));
				}
			}

	// Open a connection to a specific X display server.
	internal static Display Open(String displayName, Application app)
			{
				lock(typeof(Display))
				{
					return OpenInternal(displayName, app);
				}
			}

	/// <summary>
	/// <para>Close the X display connection if it is currently active.</para>
	/// </summary>
	public void Close()
			{
				lock(this)
				{
					if(dpy != IntPtr.Zero)
					{
						lock(typeof(Display))
						{
							// Disassociate window handles from all windows.
							for(int scr = 0; scr < screens.Length; ++scr)
							{
								screens[scr].RootWindow.Disassociate();
							}

							// Disassociate the fonts from this display.
							IDictionaryEnumerator e = fonts.GetEnumerator();
							while(e.MoveNext())
							{
								((Font)(e.Value)).Disassociate(this);
							}
							fonts.Clear();

							// Close the connection to the X server.
							Xlib.XCloseDisplay(dpy);
							dpy = IntPtr.Zero;
						}
					}
				}
			}

	// Lock this display and get the raw display pointer.
	internal IntPtr Lock()
			{
				Monitor.Enter(this);
				if(dpy == IntPtr.Zero)
				{
					throw new XInvalidOperationException
						(S._("X_ConnectionLost"));
				}
				return dpy;
			}

	// Unlock this display.  The correct way to use "Lock" and
	// "Unlock" is as follows:
	//
	//		try
	//		{
	//			IntPtr dpy = display.Lock();
	//			... use the display pointer ...
	//		}
	//		finally
	//		{
	//			display.Unlock();
	//		}
	//
	// This sequence ensures that there can be no race conditions
	// where a raw display pointer is used when the connection is
	// closed or in use by another thread.
	//
	internal void Unlock()
			{
				Monitor.Exit(this);
			}

	/// <summary>
	/// <para>Get the default screen for this display.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The default screen instance.</para>
	/// </value>
	public Screen DefaultScreenOfDisplay
			{
				get
				{
					return screens[defaultScreen];
				}
			}

	/// <summary>
	/// <para>Get the root window for the default screen.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The default root window instance.</para>
	/// </value>
	public RootWindow DefaultRootWindow
			{
				get
				{
					return DefaultScreenOfDisplay.RootWindow;
				}
			}

	/// <summary>
	/// <para>Get the display name associated with this display.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The display name string.</para>
	/// </value>
	public String DisplayString
			{
				get
				{
					try
					{
						return Xlib.XDisplayString(Lock());
					}
					finally
					{
						Unlock();
					}
				}
			}

	/// <summary>
	/// <para>Get the number of screens that are attached to
	/// this display.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The number of screens.</para>
	/// </value>
	public int ScreenCount
			{
				get
				{
					return screens.Length;
				}
			}

	/// <summary>
	/// <para>Get the index of the default screen that is attached
	/// to this display.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The default screen index.</para>
	/// </value>
	public int DefaultScreen
			{
				get
				{
					return defaultScreen;
				}
			}

	/// <summary>
	/// <para>Get the application object that owns this display.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The application object.</para>
	/// </value>
	public Application Application
			{
				get
				{
					return app;
				}
			}

	/// <summary>
	/// <para>Get a specific screen from this display, by index.</para>
	/// </summary>
	///
	/// <param name="scr">
	/// <para>The index of the screen to get.</para>
	/// </param>
	///
	/// <returns>
	/// <para>The <see cref="T:Xsharp.Screen"/> instance that corresponds
	/// to <paramref name="scr"/>.</para>
	/// </returns>
	///
	/// <exception cref="T:System.IndexOutOfRangeException">
	/// <para>Raised if <paramref name="scr"/> is less than zero or
	/// greater than or equal to <c>ScreenCount</c>.</para>
	/// </exception>
	public Screen ScreenOfDisplay(int scr)
			{
				return screens[scr];
			}

	/// <summary>
	/// <para>Flush all pending requests to the X display server.</para>
	/// </summary>
	public void Flush()
			{
				try
				{
					Xlib.XFlush(Lock());
				}
				finally
				{
					Unlock();
				}
			}

	/// <summary>
	/// <para>Synchronize operations against the X display server.</para>
	/// </summary>
	public void Sync()
			{
				try
				{
					Xlib.XSync(Lock(), Xlib.Bool.False);
				}
				finally
				{
					Unlock();
				}
			}

	/// <summary>
	/// <para>Ring the X display server bell.</para>
	/// </summary>
	///
	/// <param name="percent">
	/// <para>The percentage of the base bell volume to ring at.
	/// This must be between -100 and 100 inclusive.</para>
	/// </param>
	///
	/// <exception cref="T:System.ArgumentOfRangeException">
	/// <para>The <paramref name="percent"/> value is less than
	/// -100 or greater than 100.</para>
	/// </exception>
	public void Bell(int percent)
			{
				try
				{
					IntPtr dpy = Lock();
					if(percent >= -100 && percent <= 100)
					{
						Xlib.XBell(dpy, percent);
					}
					else
					{
						throw new ArgumentOutOfRangeException
							("percent", S._("X_BellPercent"));
					}
				}
				finally
				{
					Unlock();
				}
			}

	/// <summary>
	/// <para>Ring the X display server bell at top volume.</para>
	/// </summary>
	public void Bell()
			{
				Bell(100);
			}

	// Event types for "HandleNextEvent".
	private enum AppEvent
	{
		NoEvent,
		Regular,
		Timer,
		Quit,

	}; // enum AppEvent

	// Handle the next event and return what kind of event it was.
	private AppEvent HandleNextEvent(bool wait)
			{
				try
				{
					IntPtr dpy = Lock();
					XEvent xevent;
					int timeout;

					// Flush any requests that are in the outgoing queue.
					Xlib.XFlush(dpy);
	
					// Process "Quit".
					if(quit)
					{
						return AppEvent.Quit;
					}
	
					// Process timers that need to be activated.
					if(Timer.ActivateTimers(this))
					{
						return AppEvent.Timer;
					}
	
					// Do we have pending expose events to process?
					if(pendingExposes)
					{
						// If there are still events in the queue,
						// then process them before the exposes.
						if(Xlib.XEventsQueued
								(dpy, 2 /* QueuedAfterFlush */) != 0)
						{
							// Read the next event and dispatch it.
							Xlib.XNextEvent(dpy, out xevent);
							Unlock();
							try
							{
								DispatchEvent(ref xevent);
							}
							finally
							{
								dpy = Lock();
							}
						}
						else
						{
							// Process the pending expose events.
							InputOutputWidget widget;
							while(exposeList != null)
							{
								widget = exposeList;
								exposeList = exposeList.nextExpose;
								Unlock();
								try
								{
									widget.Expose();
								}
								finally
								{
									dpy = Lock();
								}
							}
							pendingExposes = false;
						}
						return AppEvent.Regular;
					}
					else
					{
						// Wait for the next event.
						if(wait)
						{
							timeout = Timer.GetNextTimeout(this);
						}
						else
						{
							timeout = 0;
						}
						if(timeout < 0)
						{
							Xlib.XNextEvent(dpy, out xevent);
							Unlock();
							try
							{
								DispatchEvent(ref xevent);
							}
							finally
							{
								dpy = Lock();
							}
							return AppEvent.Regular;
						}
						else
						{
							if(Xlib.XNextEventWithTimeout
								(dpy, out xevent, timeout) > 0)
							{
								Unlock();
								try
								{
									DispatchEvent(ref xevent);
								}
								finally
								{
									dpy = Lock();
								}
								return AppEvent.Regular;
							}
						}
					}
				}
				finally
				{
					Unlock();
				}

				// If we get here, then there were no events processed.
				return AppEvent.NoEvent;
			}

	/// <summary>
	/// <para>Process pending events, and return immediately.</para>
	/// </summary>
	///
	/// <returns>
	/// <para>Returns <see langword="true"/> if events were processed,
	/// or <see langword="false"/> if there are no pending events.</para>
	/// </returns>
	public bool ProcessPendingEvents()
			{
				AppEvent ev = HandleNextEvent(false);
				return (ev != AppEvent.NoEvent && ev != AppEvent.Quit);
			}

	/// <summary>
	/// <para>Wait for the next event, process it, and then return.</para>
	/// </summary>
	///
	/// <returns>
	/// <para>Returns <see langword="true"/> if an event was processed,
	/// or <see langword="false"/> if <c>Quit</c> was detected.</para>
	/// </returns>
	public bool WaitForEvent()
			{
				return (HandleNextEvent(true) != AppEvent.Quit);
			}

	/// <summary>
	/// <para>Run the main event loop on this display.</para>
	/// </summary>
	///
	/// <remarks>
	/// <para>The main event loop will run until the <c>Quit</c>
	/// method is called.</para>
	/// </remarks>
	public void Run()
			{
				while(HandleNextEvent(true) != AppEvent.Quit)
				{
					// Nothing to do here - just wait for the quit.
				}
			}

	/// <summary>
	/// <para>Tell the main event loop on this display to quit.</para>
	/// </summary>
	public void Quit()
			{
				quit = true;
			}

	// Dispatch an event that occurred on this display.  We currently
	// have the display lock.
	private void DispatchEvent(ref XEvent xevent)
			{
				// Record the time at which the event occurred.  We need
				// this to process keyboard and pointer grabs correctly.
				switch(xevent.type)
				{
					case EventType.KeyPress:
					case EventType.KeyRelease:
					{
						knownEventTime = xevent.xkey.time;
					}
					break;

					case EventType.ButtonPress:
					case EventType.ButtonRelease:
					{
						knownEventTime = xevent.xbutton.time;
					}
					break;

					case EventType.MotionNotify:
					{
						knownEventTime = xevent.xmotion.time;
					}
					break;

					case EventType.EnterNotify:
					case EventType.LeaveNotify:
					{
						knownEventTime = xevent.xcrossing.time;
					}
					break;

					case EventType.PropertyNotify:
					{
						knownEventTime = xevent.xproperty.time;
					}
					break;

					case EventType.SelectionClear:
					{
						knownEventTime = xevent.xselectionclear.time;
					}
					break;

					case EventType.SelectionNotify:
					{
						knownEventTime = xevent.xselection.time;
					}
					break;

					case EventType.SelectionRequest:
					{
						knownEventTime = xevent.xselectionrequest.time;
					}
					break;

					default:
					{
						// We don't have a time value for this event.
						knownEventTime = Xlib.Time.CurrentTime;
					}
					break;
				}

				// Find the widget that should process the event.
				Widget widget = (Widget)(handleMap[(int)(xevent.window)]);

				// Dispatch the event to the widget.
				if(widget != null)
				{
					widget.DispatchEvent(ref xevent);
				}
			}

	// Simulate an out of memory exception.
	internal static void OutOfMemory()
			{
				// Try to perform a memory allocation that we know
				// the engine will reject as being too big.
				int[] x = new int [0x7FFFFFFF];
			}

	// Retrieve or create a standard cursor.  Call with the display lock.
	internal Xlib.Cursor GetCursor(CursorType type)
			{
				uint shape = (uint)type;
				if(shape >= (uint)(CursorType.XC_num_glyphs))
				{
					shape = (uint)(CursorType.XC_X_cursor);
				}
				Xlib.Cursor cursor = cursors[(int)shape];
				if(cursor != Xlib.Cursor.Zero)
				{
					return cursor;
				}
				cursor = cursors[(int)shape] =
					Xlib.XCreateFontCursor(dpy, shape);
				return cursor;
			}

	// Add an input/output widget to the pending expose list.
	internal void AddPendingExpose(InputOutputWidget widget)
			{
				widget.nextExpose = exposeList;
				exposeList = widget;
				pendingExposes = true;
			}

	// Remove an input/output widget from the pending expose list.
	internal void RemovePendingExpose(InputOutputWidget widget)
			{
				InputOutputWidget current, prev;
				current = exposeList;
				prev = null;
				while(current != null && current != widget)
				{
					prev = current;
					current = current.nextExpose;
				}
				if(current != null)
				{
					if(prev != null)
					{
						prev.nextExpose = current.nextExpose;
					}
					else
					{
						exposeList = current.nextExpose;
					}
				}
			}

	// Get the maximum request size for the display.
	internal long MaxRequestSize()
			{
				try
				{
					IntPtr display = Lock();
					return Xlib.XMaxRequestSize(display);
				}
				finally
				{
					Unlock();
				}
			}

} // class Display

} // namespace Xsharp
