/*
 * ControlToolkitManager.cs - Implementation ControlToolkitManager class.
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

using System;
using System.Threading;
using System.Drawing.Toolkit;

/*
	This class is for thread safe creating toolkit windows.
*/
internal class ControlToolkitManager : Control
{
	static ControlToolkitManager Instance; // The only Instance
	
	static ControlToolkitManager() {
		Instance =  new ControlToolkitManager();
	}
	
	
	Thread mCreateThread; // the thread, with the first control was created
	
	private ControlToolkitManager() {
		toolkitWindow = ToolkitManager.Toolkit.CreateChildWindow(null, 0, 0, 10, 10, this);
		mCreateThread  = Thread.CurrentThread;	// remember the thread which created the window
	}
	
	#region create normal child window
	
	delegate IToolkitWindow DelegateCreateChildWindow( Control control, IToolkitWindow parent, int x, int y, int w, int h );
	
	private IToolkitWindow InternalCreateChildWindow( Control control, IToolkitWindow parent, int x, int y, int w, int h ) 
	{
		// be ThreadSafe
		if( IsInvokeRequired ) {
			return (IToolkitWindow) Invoke( 
					new DelegateCreateChildWindow( this.InternalCreateChildWindow), 
					new object[] { control, parent, x, y, w, h } 
				);
		}
		
		if(parent != null )
		{
			return parent.Toolkit.CreateChildWindow
					(parent, x, y, w, h, control);
		}
		else
		{
			// Use the default toolkit to create.
			return ToolkitManager.Toolkit.CreateChildWindow
					(null, x, y, w, h, control);
		}
	}
	
	#endregion 
	
	#region create top level window
	
	delegate IToolkitTopLevelWindow DelegateCreateTopLevelWindow( Control control, int w, int h );
	
	private IToolkitTopLevelWindow InternalCreateTopLevelWindow(Control control, int w, int h)
	{
		// be ThreadSafe
		if( IsInvokeRequired ) {
			return (IToolkitTopLevelWindow) Invoke( 
					new DelegateCreateTopLevelWindow( this.InternalCreateTopLevelWindow), 
			new object[] { control, w, h } );
		}
		
		return ToolkitManager.Toolkit.CreateTopLevelWindow( w, h, control );
	}
	
	#endregion 
	
	#region create mdi child window as TopLevelWindow
	
	delegate IToolkitTopLevelWindow DelegateCreateMdiChildWindow( Control control, IToolkitMdiClient mdiClient, int x, int y, int w, int h );
	
	IToolkitTopLevelWindow InternalCreateMdiChildWindow( Control control, IToolkitMdiClient mdiClient, int x, int y, int w, int h ) 
	{
		// be ThreadSafe
		if( IsInvokeRequired ) {
			return (IToolkitTopLevelWindow) Invoke( 
					new DelegateCreateMdiChildWindow( this.InternalCreateMdiChildWindow), 
					new object[] { control, mdiClient, x, y, w, h } 
				);
		}
		
		return mdiClient.CreateChildWindow(x,y,w,h, control);
	}
	
	#endregion
	
	#region create mdi child window as cient window
	
	delegate IToolkitWindow DelegateCreateMdiClient( Control control, IToolkitWindow parent, int x, int y, int w, int h );
	
	IToolkitWindow InternalCreateMdiClient( Control control, IToolkitWindow parent, int x, int y, int w, int h )
	{
		// be ThreadSafe
		if( IsInvokeRequired ) {
			return (IToolkitTopLevelWindow) Invoke( 
					new DelegateCreateMdiClient( this.InternalCreateMdiClient), 
					new object[] { control, parent, x, y, w, h } 
				);
		}
		
		if( null != parent ) {
			return parent.Toolkit.CreateMdiClient( parent, x, y, w, h, control );
		}
		else {
			return ToolkitManager.Toolkit.CreateMdiClient(null, x, y, w, h, control);
		}
	}
	
	#endregion
	
	#region create popup window
	
	delegate IToolkitWindow DelegateCreatePopupWindow(Control control, int x, int y, int w, int h );
	
	private IToolkitWindow InternalCreatePopupWindow(Control control, int x, int y, int w, int h )
	{
		// be ThreadSafe
		if( IsInvokeRequired ) {
			return (IToolkitWindow) Invoke( 
					new DelegateCreatePopupWindow( this.InternalCreatePopupWindow ), 
					new object[] { control, x, y, w, h } 
				);
		}
		
		return ToolkitManager.Toolkit.CreatePopupWindow( x,y,w,h,control);
	}

	#endregion
	
	#region public methods
	
	internal static IToolkitWindow CreatePopupWindow( Control control, int x, int y, int w, int h )
	{
		return Instance.InternalCreatePopupWindow( control, x, y, w, h );
	}
	
	internal static IToolkitWindow CreateMdiClient( Control control, IToolkitWindow parent, int x, int y, int w, int h )
	{
		return Instance.InternalCreateMdiClient( control, parent, x, y, w, h );
	}
	
	internal static IToolkitTopLevelWindow CreateMdiChildWindow(Control control, IToolkitMdiClient mdiClient, int x, int y, int w, int h )
	{
		return Instance.InternalCreateMdiChildWindow( control, mdiClient, x,y,w,h );
	}
	
	internal static IToolkitTopLevelWindow CreateTopLevelWindow(Control control, int w, int h ) 
	{
		return Instance.InternalCreateTopLevelWindow(control, w, h);
	}
	
	internal static IToolkitWindow CreateChildWindow(Control control, IToolkitWindow parent, int x, int y, int w, int h ) 
	{
		return Instance.InternalCreateChildWindow(control, parent, x, y, w, h );
	}
	
	internal static bool IsInvokeRequired 
	{
		get {
			return ( Instance.mCreateThread != Thread.CurrentThread );
		}
	}
	
	#endregion

}

}