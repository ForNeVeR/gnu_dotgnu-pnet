/*
 * FileSystemWatcher.cs - Implementation of the
 *		"System.IO.FileSystemWatcher" class.
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

namespace System.IO
{

#if !ECMA_COMPAT

using System.ComponentModel;
using System.Threading;

// File system watching is highly platform-specific, and arguably a
// security weakness.  A program could watch for activity on selected
// files even if they cannot access the file's contents.  This may
// allow the program to perform traffic analysis to determine what is
// actually happening.
//
// Because of this, we allow programs to register to watch filesystem
// events, but we never actually deliver them.  This satisfies the API
// requirements in a platform-neutral fashion without creating a security
// hole at the same time.

public class FileSystemWatcher : Component, ISupportInitialize
{
	// Internal state.
	private String path;
	private String filter;
	private bool enableRaisingEvents;
	private bool includeSubdirectories;
	private int internalBufferSize;
	private NotifyFilters notifyFilter;
	private Object synchronizingObject;

	// Constructors.
	public FileSystemWatcher()
			{
				this.path = String.Empty;
				this.filter = "*.*";
				this.internalBufferSize = 8192;
				this.notifyFilter = NotifyFilters.LastWrite |
									NotifyFilters.FileName |
									NotifyFilters.DirectoryName;
			}
	public FileSystemWatcher(String path)
			{
				if(path == null)
				{
					throw new ArgumentNullException("path");
				}
				else if(path.Length == 0)
				{
					throw new ArgumentException(S._("IO_InvalidPathname"));
				}
				this.filter = "*.*";
				this.internalBufferSize = 8192;
				this.notifyFilter = NotifyFilters.LastWrite |
									NotifyFilters.FileName |
									NotifyFilters.DirectoryName;
			}
	public FileSystemWatcher(String path, String filter)
			{
				if(path == null)
				{
					throw new ArgumentNullException("path");
				}
				else if(path.Length == 0)
				{
					throw new ArgumentException(S._("IO_InvalidPathname"));
				}
				if(filter == null)
				{
					throw new ArgumentNullException("filter");
				}
				this.path = path;
				this.filter = filter;
				this.internalBufferSize = 8192;
				this.notifyFilter = NotifyFilters.LastWrite |
									NotifyFilters.FileName |
									NotifyFilters.DirectoryName;
			}

	// Get or set this object's properties.
	public bool EnableRaisingEvents
			{
				get
				{
					return enableRaisingEvents;
				}
				set
				{
					enableRaisingEvents = value;
				}
			}
	public String Filter
			{
				get
				{
					return filter;
				}
				set
				{
					if(filter == null || filter.Length == 0)
					{
						filter = "*.*";
					}
					else
					{
						filter = value;
					}
				}
			}
	public bool IncludeSubdirectories
			{
				get
				{
					return includeSubdirectories;
				}
				set
				{
					includeSubdirectories = value;
				}
			}
	public int InternalBufferSize
			{
				get
				{
					return internalBufferSize;
				}
				set
				{
					internalBufferSize = value;
				}
			}
	public NotifyFilters NotifyFilter
			{
				get
				{
					return notifyFilter;
				}
				set
				{
					notifyFilter = value;
				}
			}
	public String Path
			{
				get
				{
					return path;
				}
				set
				{
					if(path == null || path.Length == 0)
					{
						throw new ArgumentException(S._("IO_InvalidPathname"));
					}
					else
					{
						path = value;
					}
				}
			}
	public override ISite Site
			{
				get
				{
					return base.Site;
				}
				set
				{
					base.Site = value;
					if(value != null && DesignMode)
					{
						enableRaisingEvents = true;
					}
				}
			}
	public Object SynchronizingObject
			{
				get
				{
					return synchronizingObject;
				}
				set
				{
					synchronizingObject = value;
				}
			}

	// Begin initialization of a watcher.
	public virtual void BeginInit() {}

	// End initialization of a watcher.
	public virtual void EndInit() {}

	// Wait for a particular kind of change to occur.
	public WaitForChangedResult WaitForChanged(WatcherChangeTypes changeType)
			{
				return WaitForChanged(changeType, -1);
			}
	public WaitForChangedResult WaitForChanged
				(WatcherChangeTypes changeType, int timeout)
			{
				// Because we aren't actually watching, the wait
				// is equivalent to a simple timeout.
				Thread.Sleep(timeout);
				return new WaitForChangedResult(changeType, true);
			}

	// Events that are emitted for various filesystem operations.
	public event FileSystemEventHandler Changed;
	public event FileSystemEventHandler Created;
	public event FileSystemEventHandler Deleted;
	public event ErrorEventHandler Error;
	public event RenamedEventHandler Renamed;

	// Dispose of this object.
	protected override void Dispose(bool disposing)
			{
				// Nothing to do in this implementation.
			}

	// Raise various events.
	protected void OnChanged(FileSystemEventArgs e)
			{
				if(Changed != null)
				{
					Changed(this, e);
				}
			}
	protected void OnCreated(FileSystemEventArgs e)
			{
				if(Created != null)
				{
					Created(this, e);
				}
			}
	protected void OnDeleted(FileSystemEventArgs e)
			{
				if(Deleted != null)
				{
					Deleted(this, e);
				}
			}
	protected void OnError(ErrorEventArgs e)
			{
				if(Error != null)
				{
					Error(this, e);
				}
			}
	protected void OnRenamed(RenamedEventArgs e)
			{
				if(Renamed != null)
				{
					Renamed(this, e);
				}
			}

}; // class FileSystemWatcher

#endif // !ECMA_COMPAT

}; // namespace System.IO
