/*
 * Control.cs - Implementation of the
 *			"System.Windows.Forms.Control" class.
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

using System.ComponentModel;
using System.Drawing;
using System.Drawing.Toolkit;
using System.Drawing.Text;
using System.Collections;
using System.Windows.Forms.Toolkit;

#if CONFIG_COMPONENT_MODEL
public class Control : Component, ISynchronizeInvoke, IWin32Window
#else
public class Control : IWin32Window
#endif
	, IToolkitEventSink
{
	// Internal state.
	internal IToolkitWindow toolkitWindow;
	private Control parent;
	private int left, top, width, height;
	private String text;
	private String name;
	private HookedEvent hookedEvents;
	private Control[] children;
	private int numChildren;
	private BindingContext bindingContext;
	private bool enabled;
	private bool visible;
	private bool allowDrop;
	private bool causesValidation;
	private Color backColor;
	private Color foreColor;
	private Font font;
	private Image backgroundImage;
	private byte anchorStyles;
	private byte dockStyle;
	private byte imeMode;
	private byte rightToLeft;
	private int tabIndex;
	private bool tabStop;
	private bool disposed;
	private Object tag;
	private ControlStyles styles;
	private CreateParams createParams;
	private static Font defaultFont;

	// Constructors.
	public Control()
			{
				this.name = String.Empty;
				this.enabled = true;
				this.anchorStyles =
					(byte)(AnchorStyles.Top | AnchorStyles.Left);
				this.causesValidation = true;
				this.imeMode = (byte)DefaultImeMode;
				this.rightToLeft = (byte)(RightToLeft.Inherit);
				this.tabIndex = -1;
				this.tabStop = true;
				this.styles = ControlStyles.UserPaint |
							  ControlStyles.StandardClick |
							  ControlStyles.Selectable |
							  ControlStyles.StandardDoubleClick |
							  ControlStyles.AllPaintingInWmPaint;
				Size initialSize = DefaultSize;
				width = initialSize.Width;
				height = initialSize.Height;
			}
	public Control(String text) : this()
			{
				this.Text = text;
			}
	public Control(Control parent, String text) : this()
			{
				this.Text = text;
				if(parent != null)
				{
					this.Parent = parent;
				}
			}
	public Control(String text, int left, int top, int width, int height)
			: this()
			{
				this.Text = text;
				this.Bounds = new Rectangle(left, top, width, height);
			}
	public Control(Control parent, String text,
				   int left, int top, int width, int height)
			: this()
			{
				this.Text = text;
				if(parent != null)
				{
					this.Parent = parent;
				}
				this.Bounds = new Rectangle(left, top, width, height);
			}

#if !CONFIG_COMPONENT_MODEL

	// Destructor.
	~Control()
			{
				Dispose(false);
			}

#endif

	// Create the toolkit window underlying this control.
	internal virtual IToolkitWindow CreateToolkitWindow(IToolkitWindow parent)
			{
				// Because we use owner-draw in this implementation
				// for all widgets, we are only interested in the
				// position and size information from "CreateParams".
				CreateParams cp = CreateParams;
				if(parent != null)
				{
					// Use the parent's toolkit to create.
					return parent.Toolkit.CreateChildWindow
						(parent, cp.X, cp.Y, cp.Width, cp.Height);
				}
				else
				{
					// Use the default toolkit to create.
					return ToolkitManager.Toolkit.CreateChildWindow
						(null, cp.X, cp.Y, cp.Width, cp.Height);
				}
			}

#if CONFIG_COMPONENT_MODEL

	// Implement the ISynchronizeInvoke interface.
	[TODO]
	public IAsyncResult BeginInvoke(Delegate method, Object[] args)
			{
				// TODO
				return null;
			}
	public IAsyncResult BeginInvoke(Delegate method)
			{
				return BeginInvoke(method, null);
			}
	[TODO]
	public Object EndInvoke(IAsyncResult result)
			{
				// TODO
				return null;
			}
	[TODO]
	public Object Invoke(Delegate method, Object[] args)
			{
				// TODO
				return null;
			}
	[TODO]
	public bool InvokeRequired
			{
				get
				{
					// TODO
					return false;
				}
			}

#endif // CONFIG_COMPONENT_MODEL

	// Get or set the control's properties.
#if !CONFIG_COMPACT_FORMS
	[TODO]
	public AccessibleObject AccessibilityObject
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public String AccessibleDefaultActionDescription
			{
				get
				{
					// TODO
					return null;
				}
				set
				{
					// TODO
				}
			}
	[TODO]
	public String AccessibleDescription
			{
				get
				{
					// TODO
					return null;
				}
				set
				{
					// TODO
				}
			}
	[TODO]
	public String AccessibleName
			{
				get
				{
					// TODO
					return null;
				}
				set
				{
					// TODO
				}
			}
	[TODO]
	public AccessibleRole AccessibleRole
			{
				get
				{
					// TODO
					return AccessibleRole.Default;
				}
				set
				{
					// TODO
				}
			}
#endif
	public virtual bool AllowDrop
			{
				get
				{
					return allowDrop;
				}
				set
				{
					allowDrop = value;
				}
			}
	public virtual AnchorStyles Anchor
			{
				get
				{
					return (AnchorStyles)anchorStyles;
				}
				set
				{
					anchorStyles = (byte)value;
				}
			}
	public virtual Color BackColor
			{
				get
				{
					if(!(backColor.IsEmpty))
					{
						return backColor;
					}
					else if(parent != null)
					{
						return parent.BackColor;
					}
					else
					{
						return DefaultBackColor;
					}
				}
				set
				{
					if(value != backColor)
					{
						backColor = value;
						OnBackColorChanged(EventArgs.Empty);
					}
				}
			}
	public virtual Image BackgroundImage
			{
				get
				{
					return backgroundImage;
				}
				set
				{
					if(value != backgroundImage)
					{
						backgroundImage = value;
						OnBackgroundImageChanged(EventArgs.Empty);
					}
				}
			}
	public virtual BindingContext BindingContext
			{
				get
				{
					if(bindingContext != null)
					{
						return bindingContext;
					}
					else if(parent != null)
					{
						return parent.BindingContext;
					}
					else
					{
						return null;
					}
				}
				set
				{
					if(bindingContext != value)
					{
						bindingContext = value;
						OnBindingContextChanged(EventArgs.Empty);
					}
				}
			}
	public int Bottom
			{
				get
				{
					return top + height;
				}
			}
	public Rectangle Bounds
			{
				get
				{
					return new Rectangle(left, top, width, height);
				}
				set
				{
					SetBoundsCore(value.Left, value.Top, value.Width, 
									value.Height, BoundsSpecified.All);
				}
			}
	public bool CanFocus
			{
				get
				{
					return (Visible && Enabled);
				}
			}
	[TODO]
	public bool CanSelect
			{
				get
				{
					// TODO
					return false;
				}
			}
	public bool CausesValidation
			{
				get
				{
					return causesValidation;
				}
				set
				{
					if(causesValidation != value)
					{
						causesValidation = value;
						OnCausesValidationChanged(EventArgs.Empty);
					}
				}
			}
	[TODO]
	public bool Capture
			{
				get
				{
					// TODO
					return false;
				}
				set
				{
					// TODO
				}
			}
	public Rectangle ClientRectangle
			{
				get
				{
					Size size = ClientSize;
					return new Rectangle(0, 0, size.Width, size.Height);
				}
			}
	public Size ClientSize
			{
				get
				{
					CreateControl();
					int leftAdjust = 0;
					int topAdjust = 0;
					int rightAdjust = 0;
					int bottomAdjust = 0;
					if(toolkitWindow != null)
					{
						toolkitWindow.GetClientAreaAdjust
							(ref leftAdjust, ref topAdjust,
							 ref rightAdjust, ref bottomAdjust);
					}
					return new Size(width - leftAdjust - rightAdjust,
									height - topAdjust - bottomAdjust);
				}
				set
				{
					SetClientSizeCore(value.Width, value.Height);
				}
			}
	[TODO]
	public String CompanyName
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public bool ContainsFocus
			{
				get
				{
					// TODO
					return false;
				}
			}
	[TODO]
	public ContextMenu ContextMenu
			{
				get
				{
					// TODO
					return null;
				}
				set
				{
					// TODO
				}
			}
	public ControlCollection Controls
			{
				get
				{
					return new ControlCollection(this);
				}
			}
	public bool Created
			{
				get
				{
					return (toolkitWindow != null);
				}
			}
	protected virtual CreateParams CreateParams
			{
				get
				{
					if(createParams != null)
					{
						return createParams;
					}
					CreateParams cp = new CreateParams();
					cp.Caption = text;
					cp.X = left;
					cp.Y = top;
					cp.Width = width;
					cp.Height = height;
					cp.ClassStyle = Win32Constants.CS_DBLCLKS;
					cp.Style = Win32Constants.WS_CLIPCHILDREN;
					if((styles & ControlStyles.ContainerControl) != 0)
					{
						cp.ExStyle = Win32Constants.WS_EX_CONTROLPARENT;
					}
					if(!IsTopLevel)
					{
						cp.Style |= Win32Constants.WS_CHILD |
									Win32Constants.WS_CLIPSIBLINGS;
					}
					if(tabStop)
					{
						cp.Style |= Win32Constants.WS_TABSTOP;
					}
					if(visible)
					{
						cp.Style |= Win32Constants.WS_VISIBLE;
					}
					if(!Enabled)
					{
						cp.Style |= Win32Constants.WS_DISABLED;
					}
					if(RightToLeft == RightToLeft.Yes)
					{
						cp.ExStyle |= Win32Constants.WS_EX_LEFTSCROLLBAR |
									  Win32Constants.WS_EX_RTLREADING |
									  Win32Constants.WS_EX_RIGHT;
					}
					createParams = cp;
					return cp;
				}
			}
	public virtual Cursor Cursor
			{
				get
				{
					// TODO
					return null;
				}
				set
				{
					// TODO
				}
			}
	[TODO]
	public ControlBindingsCollection DataBindings
			{
				get
				{
					// TODO
					return null;
				}
			}
	public static Color DefaultBackColor
			{
				get
				{
					return SystemColors.Control;
				}
			}
	public static Font DefaultFont
			{
				get
				{
					lock(typeof(Control))
					{
						if(defaultFont == null)
						{
							defaultFont = new Font
								(new FontFamily
									(GenericFontFamilies.SansSerif), 12.0f);
						}
						return defaultFont;
					}
				}
			}
	public static Color DefaultForeColor
			{
				get
				{
					return SystemColors.ControlText;
				}
			}
	protected virtual ImeMode DefaultImeMode
			{
				get
				{
					return ImeMode.Inherit;
				}
			}
	protected virtual Size DefaultSize
			{
				get
				{
					return new Size(0, 0);
				}
			}
	protected int FontHeight
			{
				get
				{
					return Font.Height;
				}
				set
				{
					// The spec says that we can only set this to Font.Height,
					// or to -1 to clear the cached value.  Since we don't
					// cache font height values, we have nothing to do here.
				}
			}
	public virtual Rectangle DisplayRectangle
			{
				get
				{
					return ClientRectangle;
				}
			}
	[TODO]
	public bool Disposing
			{
				get
				{
					// TODO
					return false;
				}
			}
	public virtual DockStyle Dock
			{
				get
				{
					return (DockStyle)dockStyle;
				}
				set
				{
					if(dockStyle != (byte)value)
					{
						dockStyle = (byte)value;
						OnDockChanged(EventArgs.Empty);
					}
				}
			}
	public bool Enabled
			{
				get
				{
					if(!enabled)
					{
						// There is no point going further up the tree.
						return false;
					}
					else if(parent != null)
					{
						return parent.Enabled;
					}
					else
					{
						return true;
					}
				}
				set
				{
					if(enabled != value)
					{
						enabled = value;
						OnEnabledChanged(EventArgs.Empty);
					}
				}
			}
	public virtual bool Focused
			{
				get
				{
					if(toolkitWindow != null)
					{
						return toolkitWindow.Focused;
					}
					else
					{
						return false;
					}
				}
			}
	public virtual Font Font
			{
				get
				{
					// Inherit our parent's font if necessary.
					Control control = parent;
					Font font = this.font;
					while(font == null && control != null)
					{
						font = control.font;
						control = control.parent;
					}
					if(font != null)
					{
						return font;
					}
					else
					{
						return DefaultFont;
					}
				}
				set
				{
					if(font != value)
					{
						font = value;
						OnFontChanged(EventArgs.Empty);
					}
				}
			}
	public virtual Color ForeColor
			{
				get
				{
					if(!(foreColor.IsEmpty))
					{
						return foreColor;
					}
					else if(parent != null)
					{
						return parent.ForeColor;
					}
					else
					{
						return DefaultForeColor;
					}
				}
				set
				{
					if(value != foreColor)
					{
						foreColor = value;
						OnForeColorChanged(EventArgs.Empty);
					}
				}
			}
	public IntPtr Handle
			{
				get
				{
					if(toolkitWindow != null)
					{
						return toolkitWindow.GetHwnd();
					}
					else
					{
						return IntPtr.Zero;
					}
				}
			}
	public bool HasChildren
			{
				get
				{
					return (numChildren > 0);
				}
			}
	public int Height
			{
				get
				{
					return height;
				}
				set
				{
					SetBoundsCore(left, top, width, value,
								  BoundsSpecified.Height);
				}
			}
	public ImeMode ImeMode
			{
				get
				{
					return (ImeMode)imeMode;
				}
				set
				{
					if(imeMode != (byte)value)
					{
						imeMode = (byte)value;
						OnImeModeChanged(EventArgs.Empty);
					}
				}
			}
	[TODO]
	public bool IsAccessible
			{
				get
				{
					// TODO
					return false;
				}
				set
				{
					// TODO
				}
			}
	public bool IsDisposed
			{
				get
				{
					return disposed;
				}
			}
	public bool IsHandleCreated
			{
				get
				{
					return (toolkitWindow != null);
				}
			}
	public int Left
			{
				get
				{
					return left;
				}
				set
				{
					SetBoundsCore(value, top, width, height,
								  BoundsSpecified.X);
				}
			}
	public Point Location
			{
				get
				{
					return new Point(left, top);
				}
				set
				{
					SetBoundsCore(value.X, value.Y, width, height,
								  BoundsSpecified.Location);
				}
			}
	public String Name
			{
				get
				{
					return name;
				}
				set
				{
					name = value;
				}
			}
	public Control Parent
			{
				get
				{
					return parent;
				}
				set
				{
					// Bail out if setting the parent to the same value.
					if(value == parent)
					{
						return;
					}

					// Cannot set the parent of top-level controls.
					if(value != null && IsTopLevel)
					{
						throw new ArgumentException
							(S._("SWF_SettingTopLevelParent"));
					}

					// Check for circularities.
					if(value != null && OccursIn(value))
					{
						throw new ArgumentException
							(S._("SWF_CircularityDetected"));
					}

					// Remove the control from its current parent.
					int posn;
					if(parent != null)
					{
						posn = 0;
						while(posn < parent.numChildren)
						{
							if(parent.children[posn] == this)
							{
								--(parent.numChildren);
								while(posn < parent.numChildren)
								{
									parent.children[posn] =
										parent.children[posn + 1];
								}
								break;
							}
						}
					}

					// Add the control to its new parent.
					parent = value;
					if(value != null)
					{
						if(value.children == null)
						{
							value.children = new Control [4];
						}
						else if(value.numChildren >= value.children.Length)
						{
							Control[] newChildren;
							newChildren = new Control [value.numChildren * 2];
							Array.Copy(value.children, 0, newChildren, 0,
									   value.numChildren);
							value.children = newChildren;
						}
						value.children[(value.numChildren)++] = this;
					}

					// Reparent the control within the windowing system.
					Reparent(value);

					// Notify event handlers that the parent changed.
					OnParentChanged(EventArgs.Empty);

					// If we were using our parent's binding context,
					// then that has changed as well.
					if(bindingContext == null)
					{
						OnBindingContextChanged(EventArgs.Empty);
					}
				}
			}
	[TODO]
	public String ProductName
			{
				get
				{
					// TODO
					return null;
				}
			}
	[TODO]
	public String ProductVersion
			{
				get
				{
					// TODO
					return null;
				}
			}
	public bool RecreatingHandle
			{
				get
				{
					// We create it once and dispose it once.
					return false;
				}
			}
	[TODO]
	public Region Region
			{
				get
				{
					// TODO
					return null;
				}
			}
	protected bool ResizeRedraw
			{
				get
				{
					return GetStyle(ControlStyles.ResizeRedraw);
				}
				set
				{
					SetStyle(ControlStyles.ResizeRedraw, value);
				}
			}
	public int Right
			{
				get
				{
					return left + width;
				}
			}
	public virtual RightToLeft RightToLeft
			{
				get
				{
					if(rightToLeft != (byte)(RightToLeft.Inherit))
					{
						return (RightToLeft)rightToLeft;
					}
					else if(parent != null)
					{
						return parent.RightToLeft;
					}
					else
					{
						return RightToLeft.No;
					}
				}
				set
				{
					if(rightToLeft != (byte)value)
					{
						rightToLeft = (byte)value;
						OnRightToLeftChanged(EventArgs.Empty);
					}
				}
			}
	protected virtual bool ShowFocusCues
			{
				get
				{
					if(parent != null)
					{
						return parent.ShowFocusCues;
					}
					else
					{
						return true;
					}
				}
			}
	protected virtual bool ShowKeyboardCues
			{
				get
				{
					if(parent != null)
					{
						return parent.ShowKeyboardCues;
					}
					else
					{
						return true;
					}
				}
			}
#if CONFIG_COMPONENT_MODEL
	public override ISite Site
			{
				get
				{
					return base.Site;
				}
				set
				{
					base.Site = value;
				}
			}
#endif
	public Size Size
			{
				get
				{
					return new Size(width, height);
				}
				set
				{
					SetBoundsCore(left, top, value.Width, value.Height,
								  BoundsSpecified.Size);
				}
			}
	public int TabIndex
			{
				get
				{
					return tabIndex;
				}
				set
				{
					if(tabIndex != value)
					{
						tabIndex = value;
						OnTabIndexChanged(EventArgs.Empty);
					}
				}
			}
	public bool TabStop
			{
				get
				{
					return tabStop;
				}
				set
				{
					if(tabStop != value)
					{
						tabStop = value;
						OnTabStopChanged(EventArgs.Empty);
					}
				}
			}
	public Object Tag
			{
				get
				{
					return tag;
				}
				set
				{
					tag = value;
				}
			}
	public virtual String Text
			{
				get
				{
					return text;
				}
				set
				{
					if(text != value)
					{
						text = value;
						OnTextChanged(EventArgs.Empty);
					}
				}
			}
	public int Top
			{
				get
				{
					return top;
				}
				set
				{
					SetBoundsCore(left, value, width, height,
								  BoundsSpecified.Y);
				}
			}
	public Control TopLevelControl
			{
				get
				{
					Control ctrl = this;
					Control parent;
					while((parent = ctrl.Parent) != null)
					{
						ctrl = ctrl.Parent;
					}
					return ctrl;
				}
			}
	public bool Visible
			{
				get
				{
					if(!visible)
					{
						// There is no point going further up the tree.
						return false;
					}
					else if(parent != null)
					{
						return parent.Visible;
					}
					else
					{
						return true;
					}
				}
				set
				{
					SetVisibleCore(value);
				}
			}
	public int Width
			{
				get
				{
					return width;
				}
				set
				{
					SetBoundsCore(left, top, value, height,
								  BoundsSpecified.Width);
				}
			}
	internal virtual bool IsTopLevel
			{
				get
				{
					// "IsTopLevel" indicates if the control class
					// cannot support parents (e.g. app windows).
					// Most controls can support parents.
					return false;
				}
			}

	// Get the global state of the modifier keys.
	[TODO]
	public static Keys ModifierKeys
			{
				get
				{
					// TODO
					return Keys.None;
				}
			}

	// Get the global state of the mouse buttons.
	[TODO]
	public static MouseButtons MouseButtons
			{
				get
				{
					// TODO
					return MouseButtons.None;
				}
			}

	// Get the current screen position of the mouse.
	[TODO]
	public static Point MousePosition
			{
				get
				{
					// TODO
					return new Point(0, 0);
				}
			}

	// Determine if a control occurs in a tree of controls.
	// This is used to detect circularities.
	private bool OccursIn(Control control)
			{
				if(control == this)
				{
					return true;
				}
				int posn;
				for(posn = 0; posn < numChildren; ++posn)
				{
					if(children[posn].OccursIn(control))
					{
						return true;
					}
				}
				return false;
			}

#if !CONFIG_COMPACT_FORMS

	// Notify client applications of accessibility events.
	[TODO]
	protected void AccessibilityNotifyClients
				(AccessibleEvents accEvent, int childID)
			{
				// TODO
			}

	// Create the accessibility object for this control.
	[TODO]
	protected virtual AccessibleObject CreateAccessibilityInstance()
			{
				// TODO
				return null;
			}

#endif

	// Bring the control to the front of its sibling stack.
	public void BringToFront()
			{
				if(parent != null)
				{
					if(parent.children[0] != this)
					{
						Control sibling = parent.children[0];
						int posn = 0;
						while(parent.children[posn] != this)
						{
							++posn;
						}
						while(posn > 0)
						{
							parent.children[posn] = parent.children[posn - 1];
							--posn;
						}
						parent.children[0] = this;
					}
				}
				if(toolkitWindow != null)
				{
					toolkitWindow.Raise();
				}
			}

	// Determine if this control contains a particular child.
	public bool Contains(Control ctl)
			{
				return (ctl != null && ctl.Parent == this);
			}

	// Force the control to be created.
	public void CreateControl()
			{
				// Cannot create the control if it has been disposed.
				if(disposed)
				{
					throw new ObjectDisposedException
						("control", S._("SWF_ControlDisposed"));
				}

				// Find the highest ancestor that isn't created
				// and start building the control tree from there.
				Control control = this;
				while(control.parent != null &&
				      control.parent.toolkitWindow == null)
				{
					control = control.parent;
				}
				control.CreateControlInner();
			}
	private void CreateControlInner()
			{
				// Create the handle for this control.
				CreateHandle();

				// Create the child controls.
				int posn;
				for(posn = 0; posn < numChildren; ++posn)
				{
					children[posn].CreateControlInner();
				}

				// Map the control to the screen if it is visible.
				if(visible && toolkitWindow != null)
				{
					toolkitWindow.IsMapped = true;
				}

				// Notify subclasses that the create has occurred.
				OnCreateControl();

				// Record a change in binding context if necessary.
				if(bindingContext == null && parent != null)
				{
					OnBindingContextChanged(EventArgs.Empty);
				}
			}

	// Create a new control collection for this instance.
	protected virtual ControlCollection CreateControlsInstance()
			{
				return new ControlCollection(this);
			}

	// Create the handle for this control.
	protected virtual void CreateHandle()
			{
				// Cannot create the control if it has been disposed.
				if(disposed)
				{
					throw new ObjectDisposedException
						("control", S._("SWF_ControlDisposed"));
				}

				// Reparent underneath the new parent if the handle
				// already exists from some previous incarnation.
				if(toolkitWindow != null)
				{
					if(parent != null && toolkitWindow.Parent == null)
					{
						toolkitWindow.Reparent
							(parent.toolkitWindow, left, top);
					}
					return;
				}

				// Create the handle using the toolkit.
				if(parent != null)
				{
					toolkitWindow = CreateToolkitWindow(parent.toolkitWindow);
				}
				else
				{
					toolkitWindow = CreateToolkitWindow(null);
				}
				toolkitWindow.Lower();

				// Copy color information into the toolkit window.
				toolkitWindow.SetForeground(ForeColor);
				toolkitWindow.SetBackground(BackColor);
				// TODO: background images

				// Join the toolkit event handlers to this object.
				toolkitWindow.SetEventSink(this);

				// Notify subclasses that the handle has been created.
				OnHandleCreated(EventArgs.Empty);
			}

	// Create a graphics drawing object for the control.
	public Graphics CreateGraphics()
			{
				if(toolkitWindow != null)
				{
					return ToolkitManager.CreateGraphics
						(toolkitWindow.GetGraphics());
				}
				else
				{
					return null;
				}
			}

	// Destroy the handle associated with the control.
	[TODO]
	protected virtual void DestroyHandle()
			{
				// TODO
			}

#if !CONFIG_COMPACT_FORMS

	// Begin a drag and drop operation on this control.
	[TODO]
	public DragDropEffects DoDragDrop
				(Object data, DragDropEffects allowedEffects)
			{
				// TODO
				return allowedEffects;
			}

#endif // !CONFIG_COMPACT_FORMS

	// Dispose of this control.
#if CONFIG_COMPACT_FORMS
	public new void Dispose()
			{
				Dispose(true);
				GC.SuppressFinalize(this);
			}
#endif
	[TODO]
#if CONFIG_COMPONENT_MODEL
	protected override void Dispose(bool disposing)
#else
	protected virtual void Dispose(bool disposing)
#endif
			{
				// TODO
			}

	// Find the form that this control is a member of.
	public Form FindForm()
			{
				Control current = Parent;
				while(current != null && !(current is Form))
				{
					current = current.Parent;
				}
				return (Form)current;
			}

	// Set the input focus to this control.
	[TODO]
	public bool Focus()
			{
				// TODO
				return false;
			}

	// Convert a child HWND into the corresponding Control object.
	[TODO]
	public static Control FromChildHandle(IntPtr handle)
			{
				// TODO
				return FromHandle(handle);
			}

	// Convert a HWND into the corresponding Control object.
	[TODO]
	public static Control FromHandle(IntPtr handle)
			{
				// TODO
				return null;
			}

	// Get the child at a specific location within this control.
	public Control ChildAtPoint(Point pt)
			{
				int posn, x, y;
				Control child;
				x = pt.X;
				y = pt.Y;
				for(posn = 0; posn < numChildren; ++posn)
				{
					child = children[posn];
					if(!(child.visible))
					{
						continue;
					}
					if(x >= child.left && x < (child.left + child.width) &&
					   y >= child.top && y < (child.top + child.height))
					{
						return child;
					}
				}
				return null;
			}

	// Get the container control for this control.
	public IContainerControl GetContainerControl()
			{
				Control current = Parent;
				while(current != null && !(current is IContainerControl))
				{
					current = current.Parent;
				}
				return (IContainerControl)current;
			}

	// Get the next or previous control in the tab order.
	[TODO]
	public Control GetNextControl(Control ctl, bool forward)
			{
				// TODO
				return null;
			}

	// Get a particular style flag.
	protected bool GetStyle(ControlStyles flag)
			{
				return ((styles & flag) != 0);
			}

	// Determine if this is a top-level control.
	protected bool GetTopLevel()
			{
				return (parent == null);
			}

	// Hide the control.
	public void Hide()
			{
				Visible = false;
			}

	// Initialize layout as this control has just been added to a container.
	[TODO]
	protected virtual void InitLayout()
			{
				// TODO
			}

	// Invalidate a region of the control and queue up a repaint request.
	public void Invalidate()
			{
				if(toolkitWindow != null && Visible)
				{
					toolkitWindow.Invalidate();
				}
			}
	[TODO]
	public void Invalidate(bool invalidateChildren)
			{
				Invalidate();
				// TODO
			}
	public void Invalidate(Rectangle rc)
			{
				if(toolkitWindow != null && Visible)
				{
					toolkitWindow.Invalidate(rc.X, rc.Y, rc.Width, rc.Height);
				}
			}
	[TODO]
	public void Invalidate(Rectangle rc, bool invalidateChildren)
			{
				Invalidate(rc);
				// TODO
			}
	[TODO]
	public void Invalidate(Region region)
			{
				// TODO
			}
	[TODO]
	public void Invalidate(Region region, bool invalidateChildren)
			{
				// TODO
			}

	// Invoke a delegate on the thread that owns the low-level control.
	[TODO]
	public Object Invoke(Delegate method)
			{
				// TODO
				return null;
			}

	// Invoke the "GotFocus" event on a particular control.
	protected void InvokeGotFocus(Control toInvoke, EventArgs e)
			{
				toInvoke.OnGotFocus(e);
			}

	// Invoke the "LostFocus" event on a particular control.
	protected void InvokeLostFocus(Control toInvoke, EventArgs e)
			{
				toInvoke.OnLostFocus(e);
			}

	// Invoke the "Click" event on a particular control.
	protected void InvokeOnClick(Control toInvoke, EventArgs e)
			{
				toInvoke.OnClick(e);
			}

	// Invoke the "Paint" event on a particular control.
	protected void InvokePaint(Control c, PaintEventArgs e)
			{
				c.OnPaint(e);
			}

	// Invoke the "PaintBackground" event on a particular control.
	protected void InvokePaintBackground(Control c, PaintEventArgs e)
			{
				c.OnPaintBackground(e);
			}

	// Determine if a character is recognized by a control as an input char.
	[TODO]
	protected virtual bool IsInputChar(char c)
			{
				// TODO
				return false;
			}

	// Determine if a key is recognized by a control as an input key.
	[TODO]
	protected virtual bool IsInputKey(Keys keyData)
			{
				// TODO
				return false;
			}

	// Determine if a character is mnemonic in a string.
	public static bool IsMnemonic(char charCode, String text)
			{
				if(charCode == '&' || text == null)
				{
					return false;
				}
				int index = text.IndexOf('&') + 1;
				if(index < text.Length)
				{
					char ch = text[index];
					return (Char.ToUpper(ch) == Char.ToUpper(charCode));
				}
				return false;
			}

	// Force the child to perform layout.
	[TODO]
	public void PerformLayout()
			{
				// TODO
			}
	[TODO]
	public void PerformLayout
				(Control affectedControl, String affectedProperty)
			{
				// TODO
			}

	// Convert a screen point into client co-ordinates.
	public Point PointToClient(Point p)
			{
				// TODO
				return p;
			}

	// Convert a client point into screen co-ordinates.
	public Point PointToScreen(Point p)
			{
				// TODO
				return p;
			}

	// Process a command key.
	[TODO]
	protected virtual bool ProcessCmdKey(ref Message msg, Keys keyData)
			{
				// TODO
				return false;
			}

	// Process a dialog character.
	[TODO]
	protected virtual bool ProcessDialogChar(char charCode)
			{
				// TODO
				return false;
			}

	// Process a dialog key.
	[TODO]
	protected virtual bool ProcessDialogKey(Keys keyData)
			{
				// TODO
				return false;
			}

	// Process a key event.
	[TODO]
	protected virtual bool ProcessKeyEventArgs(ref Message msg)
			{
				// TODO
				return false;
			}

	// Process a keyboard message.
	[TODO]
	protected internal virtual bool ProcessKeyMessage(ref Message msg)
			{
				// TODO
				return false;
			}

	// Preview a keyboard message.
	[TODO]
	protected virtual bool ProcessKeyPreview(ref Message msg)
			{
				// TODO
				return false;
			}

	// Process a key mnemonic.
	[TODO]
	protected virtual bool ProcessMnemonic(char charCode)
			{
				// TODO
				return false;
			}

	// Force the handle to be recreated.
	[TODO]
	protected void RecreateHandle()
			{
				// TODO
			}

	// Convert a screen rectangle into client co-ordinates.
	public Rectangle RectangleToClient(Rectangle r)
			{
				return new Rectangle(PointToClient(r.Location), r.Size);
			}

	// Convert a client rectangle into screen co-ordinates.
	public Rectangle RectangleToScreen(Rectangle r)
			{
				return new Rectangle(PointToScreen(r.Location), r.Size);
			}

	// Reflect a message to the correct control.
	protected static bool ReflectMessage(IntPtr hWnd, ref Message m)
			{
				// We don't use this method in this implementation.
				return false;
			}

	// Force an immediate refresh on the control.
	[TODO]
	public virtual void Refresh()
			{
				// TODO
			}

	// Reset the background color to its default value.
	public virtual void ResetBackColor()
			{
				BackColor = Color.Empty;
			}

	// Reset the data bindings to its default value.
	[TODO]
	public void ResetBindings()
			{
				// TODO
			}

	// Reset the cursor to its default value.
	[TODO]
	public virtual void ResetCursor()
			{
				// TODO
			}

	// Reset the foreground color to its default value.
	public virtual void ResetForeColor()
			{
				ForeColor = Color.Empty;
			}

	// Reset the font to its default value.
	public virtual void ResetFont()
			{
				Font = null;
			}

	// Reset the input method mode to its default value.
	public void ResetImeMode()
			{
				ImeMode = DefaultImeMode;
			}

	// Reset the right to left property to its default value.
	public virtual void ResetRightToLeft()
			{
				RightToLeft = RightToLeft.Inherit;
			}

	// Reset the text property to its default value.
	public virtual void ResetText()
			{
				Text = null;
			}

	// Resume layout operations.
	public void ResumeLayout()
			{
				ResumeLayout(true);
			}
	[TODO]
	public void ResumeLayout(bool performLayout)
			{
				// TODO
			}

	// Translate an alignment value for right-to-left text.
	protected ContentAlignment RtlTranslateAlignment(ContentAlignment align)
			{
				if(RightToLeft == RightToLeft.No)
				{
					return align;
				}
				else
				{
					switch(align)
					{
						case ContentAlignment.TopLeft:
							return ContentAlignment.TopRight;
						case ContentAlignment.TopRight:
							return ContentAlignment.TopLeft;
						case ContentAlignment.MiddleLeft:
							return ContentAlignment.MiddleRight;
						case ContentAlignment.MiddleRight:
							return ContentAlignment.MiddleLeft;
						case ContentAlignment.BottomLeft:
							return ContentAlignment.BottomRight;
						case ContentAlignment.BottomRight:
							return ContentAlignment.BottomLeft;
						default:
							return align;
					}
				}
			}
	protected HorizontalAlignment RtlTranslateAlignment
				(HorizontalAlignment align)
			{
				if(RightToLeft == RightToLeft.No)
				{
					return align;
				}
				else if(align == HorizontalAlignment.Left)
				{
					return HorizontalAlignment.Right;
				}
				else if(align == HorizontalAlignment.Right)
				{
					return HorizontalAlignment.Left;
				}
				else
				{
					return align;
				}
			}
	protected LeftRightAlignment RtlTranslateAlignment
				(LeftRightAlignment align)
			{
				if(RightToLeft == RightToLeft.No)
				{
					return align;
				}
				else if(align == LeftRightAlignment.Left)
				{
					return LeftRightAlignment.Right;
				}
				else
				{
					return LeftRightAlignment.Left;
				}
			}
	protected ContentAlignment RtlTranslateContent(ContentAlignment align)
			{
				return RtlTranslateAlignment(align);
			}
	protected HorizontalAlignment RtlTranslateHorizontal
				(HorizontalAlignment align)
			{
				return RtlTranslateAlignment(align);
			}
	protected LeftRightAlignment RtlTranslateLeftRight
				(LeftRightAlignment align)
			{
				return RtlTranslateAlignment(align);
			}

	// Scale this control and its children.
	public void Scale(float ratio)
			{
				ScaleCore(ratio, ratio);
			}
	public void Scale(float dx, float dy)
			{
				ScaleCore(dx, dy);
			}

	// Inner core of "Scale".
	[TODO]
	protected virtual void ScaleCore(float dx, float dy)
			{
				// TODO
			}

	// Select this control.
	public void Select()
			{
				Select(false, false);
			}
	[TODO]
	protected virtual void Select(bool directed, bool forward)
			{
				// TODO
			}

	// Select the next control.
	[TODO]
	public bool SelectNextControl
				(Control ctl, bool forward, bool tabStopOnly,
			     bool nested, bool wrap)
			{
				// TODO
				return false;
			}

	// Send this control to the back of its sibling stack.
	public void SendToBack()
			{
				if(parent != null)
				{
					if(parent.children[parent.numChildren - 1] != this)
					{
						Control sibling;
						sibling = parent.children[parent.numChildren - 1];
						int posn = parent.numChildren - 1;
						while(parent.children[posn] != this)
						{
							--posn;
						}
						while(posn < (parent.numChildren - 1))
						{
							parent.children[posn] = parent.children[posn + 1];
							++posn;
						}
						parent.children[parent.numChildren - 1] = this;
					}
				}
				if(toolkitWindow != null)
				{
					toolkitWindow.Lower();
				}
			}

	// Set the bounds of the control.
	public void SetBounds(int x, int y, int width, int height)
			{
				SetBoundsCore(x, y, width, height, BoundsSpecified.All);
			}
	public void SetBounds(int x, int y, int width, int height,
						  BoundsSpecified specified)
			{
				SetBoundsCore(x, y, width, height, specified);
			}

	// Inner core of "SetBounds".
	[TODO]
	protected virtual void SetBoundsCore
				(int x, int y, int width, int height,
				 BoundsSpecified specified)
			{
				// Set unspecified components to the right values.
				if((specified & BoundsSpecified.X) == 0)
				{
					x = this.left;
				}
				if((specified & BoundsSpecified.Y) == 0)
				{
					y = this.top;
				}
				if((specified & BoundsSpecified.Width) == 0)
				{
					width = this.width;
				}
				if((specified & BoundsSpecified.Height) == 0)
				{
					height = this.height;
				}

				// Move and resize the toolkit version of the control.
				if(toolkitWindow != null)
				{
					if(x != this.left || y != this.top ||
					   width != this.width || height != this.height)
					{
						toolkitWindow.MoveResize(x, y, width, height);
					}
				}

				// Update the bounds and emit the necessary events.
				UpdateBounds(x, y, width, height);

				// TODO: layout the child controls, apply docking rules, etc
			}

	// Inner core of setting the client size.
	protected virtual void SetClientSizeCore(int x, int y)
			{
				CreateControl();
				int leftAdjust = 0;
				int topAdjust = 0;
				int rightAdjust = 0;
				int bottomAdjust = 0;
				if(toolkitWindow != null)
				{
					toolkitWindow.GetClientAreaAdjust
						(ref leftAdjust, ref topAdjust,
						 ref rightAdjust, ref bottomAdjust);
				}
				SetBoundsCore
					(left, top,
					 x + leftAdjust + rightAdjust,
					 y + topAdjust + bottomAdjust,
					 BoundsSpecified.Size);
			}

	// Set a style bit.
	protected void SetStyle(ControlStyles flag, bool value)
			{
				if(value)
				{
					styles |= flag;
				}
				else
				{
					styles &= ~flag;
				}
			}

	// Set the top-level property of this control.
	[TODO]
	protected void SetTopLevel(bool value)
			{
				// TODO
			}

	// Inner core of setting the visibility state.
	protected virtual void SetVisibleCore(bool value)
			{
				if(visible != value)
				{
					visible = value;
					OnVisibleChanged(EventArgs.Empty);
				}
			}

	// Show the control on-screen.
	public void Show()
			{
				Visible = true;
			}

	// Suspend layout for this control.
	[TODO]
	public void SuspendLayout()
			{
				// TODO
			}

	// Update the invalidated regions in this control.
	[TODO]
	public void Update()
			{
				// TODO
			}

	// Update the bounds of the control.
	protected void UpdateBounds()
			{
				UpdateBounds(left, top, width, height);
			}
	protected void UpdateBounds(int x, int y, int width, int height)
			{
				bool moved;
				bool resized;
				moved = (x != this.left || y != this.top);
				resized = (width != this.width || height != this.height);
				this.left = x;
				this.top = y;
				this.width = width;
				this.height = height;
				if(moved)
				{
					OnMove(EventArgs.Empty);
				}
				if(resized)
				{
					OnResize(EventArgs.Empty);
				}
			}
	protected void UpdateBounds(int x, int y, int width, int height,
								int clientWidth, int clientHeight)
			{
				// Ignore the client size information: we assume that
				// the client area remains fixed relative to the bounds.
				UpdateBounds(x, y, width, height);
			}

	// Apply the changed styles to the control.
	[TODO]
	protected void UpdateStyles()
			{
				// TODO
			}

	// Update the Z-order of a control.
	protected void UpdateZOrder()
			{
				// We don't use this - the child collection updates
				// the Z-order of child controls automatically.
			}

#if !CONFIG_COMPACT_FORMS

	// Default window procedure for this control class.
	protected virtual void DefWndProc(ref Message msg)
			{
				// Window procedures are not used in this implementation.
			}

	// Pre-process a message before it is dispatched by the event loop.
	public virtual bool PreProcessMessage(ref Message msg)
			{
				// Window procedures are not used in this implementation.
				return false;
			}

	// Process a message.
	public virtual void WndProc(ref Message m)
			{
				// Window procedures are not used in this implementation.
			}

#endif // !CONFIG_COMPACT_FORMS

	// Hooked event information.
	private sealed class HookedEvent
	{
		public EventId eventId;
		public Delegate handler;
		public HookedEvent next;

		public HookedEvent(EventId eventId, Delegate handler,
						   HookedEvent next)
				{
					this.eventId = eventId;
					this.handler = handler;
					this.next = next;
				}

	}; // class HookedEvent

	// Add a handler for a specific event.
	internal void AddHandler(EventId eventId, Delegate handler)
			{
				lock(this)
				{
					HookedEvent current = hookedEvents;
					while(current != null)
					{
						if(current.eventId == eventId)
						{
							current.handler =
								Delegate.Combine(current.handler, handler);
							return;
						}
						current = current.next;
					}
					hookedEvents = new HookedEvent
						(eventId, handler, hookedEvents);
				}
			}

	// Remove a handler from a specific event.
	internal void RemoveHandler(EventId eventId, Delegate handler)
			{
				lock(this)
				{
					HookedEvent current = hookedEvents;
					HookedEvent prev = null;
					while(current != null)
					{
						if(current.eventId == eventId)
						{
							current.handler =
								Delegate.Remove(current.handler, handler);
							if(current.handler == null)
							{
								if(prev != null)
								{
									prev.next = current.next;
								}
								else
								{
									hookedEvents = current.next;
								}
							}
							return;
						}
						prev = current;
						current = current.next;
					}
				}
			}

	// Get the handler for a specific event.
	internal Delegate GetHandler(EventId eventId)
			{
				lock(this)
				{
					HookedEvent current = hookedEvents;
					while(current != null)
					{
						if(current.eventId == eventId)
						{
							return current.handler;
						}
						current = current.next;
					}
					return null;
				}
			}

	// Events that may be emitted by this control.
	public event EventHandler BackColorChanged
			{
				add
				{
					AddHandler(EventId.BackColorChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.BackColorChanged, value);
				}
			}
	public event EventHandler BackgroundImageChanged
			{
				add
				{
					AddHandler(EventId.BackgroundImageChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.BackgroundImageChanged, value);
				}
			}
	public event EventHandler BindingContextChanged
			{
				add
				{
					AddHandler(EventId.BindingContextChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.BindingContextChanged, value);
				}
			}
	public event EventHandler CausesValidationChanged
			{
				add
				{
					AddHandler(EventId.CausesValidationChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.CausesValidationChanged, value);
				}
			}
	public event UICuesEventHandler ChangeUICues
			{
				add
				{
					AddHandler(EventId.ChangeUICues, value);
				}
				remove
				{
					RemoveHandler(EventId.ChangeUICues, value);
				}
			}
	public event EventHandler Click
			{
				add
				{
					AddHandler(EventId.Click, value);
				}
				remove
				{
					RemoveHandler(EventId.Click, value);
				}
			}
	public event EventHandler ContextMenuChanged
			{
				add
				{
					AddHandler(EventId.ContextMenuChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.ContextMenuChanged, value);
				}
			}
	public event ControlEventHandler ControlAdded
			{
				add
				{
					AddHandler(EventId.ControlAdded, value);
				}
				remove
				{
					RemoveHandler(EventId.ControlAdded, value);
				}
			}
	public event ControlEventHandler ControlRemoved
			{
				add
				{
					AddHandler(EventId.ControlRemoved, value);
				}
				remove
				{
					RemoveHandler(EventId.ControlRemoved, value);
				}
			}
	public event EventHandler CursorChanged
			{
				add
				{
					AddHandler(EventId.CursorChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.CursorChanged, value);
				}
			}
	public event EventHandler DockChanged
			{
				add
				{
					AddHandler(EventId.DockChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.DockChanged, value);
				}
			}
	public event EventHandler DoubleClick
			{
				add
				{
					AddHandler(EventId.DoubleClick, value);
				}
				remove
				{
					RemoveHandler(EventId.DoubleClick, value);
				}
			}
#if !CONFIG_COMPACT_FORMS
	public event DragEventHandler DragDrop
			{
				add
				{
					AddHandler(EventId.DragDrop, value);
				}
				remove
				{
					RemoveHandler(EventId.DragDrop, value);
				}
			}
	public event DragEventHandler DragEnter
			{
				add
				{
					AddHandler(EventId.DragEnter, value);
				}
				remove
				{
					RemoveHandler(EventId.DragEnter, value);
				}
			}
	public event EventHandler DragLeave
			{
				add
				{
					AddHandler(EventId.DragLeave, value);
				}
				remove
				{
					RemoveHandler(EventId.DragLeave, value);
				}
			}
	public event DragEventHandler DragOver
			{
				add
				{
					AddHandler(EventId.DragOver, value);
				}
				remove
				{
					RemoveHandler(EventId.DragOver, value);
				}
			}
#endif
	public event EventHandler EnabledChanged
			{
				add
				{
					AddHandler(EventId.EnabledChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.EnabledChanged, value);
				}
			}
	public event EventHandler Enter
			{
				add
				{
					AddHandler(EventId.Enter, value);
				}
				remove
				{
					RemoveHandler(EventId.Enter, value);
				}
			}
	public event EventHandler FontChanged
			{
				add
				{
					AddHandler(EventId.FontChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.FontChanged, value);
				}
			}
	public event EventHandler ForeColorChanged
			{
				add
				{
					AddHandler(EventId.ForeColorChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.ForeColorChanged, value);
				}
			}
#if !CONFIG_COMPACT_FORMS
	public event GiveFeedbackEventHandler GiveFeedback
			{
				add
				{
					AddHandler(EventId.GiveFeedback, value);
				}
				remove
				{
					RemoveHandler(EventId.GiveFeedback, value);
				}
			}
#endif
	public event EventHandler GotFocus
			{
				add
				{
					AddHandler(EventId.GotFocus, value);
				}
				remove
				{
					RemoveHandler(EventId.GotFocus, value);
				}
			}
	public event EventHandler HandleCreated
			{
				add
				{
					AddHandler(EventId.HandleCreated, value);
				}
				remove
				{
					RemoveHandler(EventId.HandleCreated, value);
				}
			}
	public event EventHandler HandleDestroyed
			{
				add
				{
					AddHandler(EventId.HandleDestroyed, value);
				}
				remove
				{
					RemoveHandler(EventId.HandleDestroyed, value);
				}
			}
	public event HelpEventHandler HelpRequested
			{
				add
				{
					AddHandler(EventId.HelpRequested, value);
				}
				remove
				{
					RemoveHandler(EventId.HelpRequested, value);
				}
			}
	public event EventHandler ImeModeChanged
			{
				add
				{
					AddHandler(EventId.ImeModeChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.ImeModeChanged, value);
				}
			}
	public event InvalidateEventHandler Invalidated
			{
				add
				{
					AddHandler(EventId.Invalidated, value);
				}
				remove
				{
					RemoveHandler(EventId.Invalidated, value);
				}
			}
	public event KeyEventHandler KeyDown
			{
				add
				{
					AddHandler(EventId.KeyDown, value);
				}
				remove
				{
					RemoveHandler(EventId.KeyDown, value);
				}
			}
	public event KeyPressEventHandler KeyPress
			{
				add
				{
					AddHandler(EventId.KeyPress, value);
				}
				remove
				{
					RemoveHandler(EventId.KeyPress, value);
				}
			}
	public event KeyEventHandler KeyUp
			{
				add
				{
					AddHandler(EventId.KeyUp, value);
				}
				remove
				{
					RemoveHandler(EventId.KeyUp, value);
				}
			}
	public event LayoutEventHandler Layout
			{
				add
				{
					AddHandler(EventId.Layout, value);
				}
				remove
				{
					RemoveHandler(EventId.Layout, value);
				}
			}
	public event EventHandler Leave
			{
				add
				{
					AddHandler(EventId.Leave, value);
				}
				remove
				{
					RemoveHandler(EventId.Leave, value);
				}
			}
	public event EventHandler LocationChanged
			{
				add
				{
					AddHandler(EventId.LocationChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.LocationChanged, value);
				}
			}
	public event EventHandler LostFocus
			{
				add
				{
					AddHandler(EventId.LostFocus, value);
				}
				remove
				{
					RemoveHandler(EventId.LostFocus, value);
				}
			}
	public event MouseEventHandler MouseDown
			{
				add
				{
					AddHandler(EventId.MouseDown, value);
				}
				remove
				{
					RemoveHandler(EventId.MouseDown, value);
				}
			}
	public event EventHandler MouseEnter
			{
				add
				{
					AddHandler(EventId.MouseEnter, value);
				}
				remove
				{
					RemoveHandler(EventId.MouseEnter, value);
				}
			}
	public event EventHandler MouseHover
			{
				add
				{
					AddHandler(EventId.MouseHover, value);
				}
				remove
				{
					RemoveHandler(EventId.MouseHover, value);
				}
			}
	public event EventHandler MouseLeave
			{
				add
				{
					AddHandler(EventId.MouseLeave, value);
				}
				remove
				{
					RemoveHandler(EventId.MouseLeave, value);
				}
			}
	public event MouseEventHandler MouseMove
			{
				add
				{
					AddHandler(EventId.MouseMove, value);
				}
				remove
				{
					RemoveHandler(EventId.MouseMove, value);
				}
			}
	public event MouseEventHandler MouseUp
			{
				add
				{
					AddHandler(EventId.MouseUp, value);
				}
				remove
				{
					RemoveHandler(EventId.MouseUp, value);
				}
			}
	public event MouseEventHandler MouseWheel
			{
				add
				{
					AddHandler(EventId.MouseWheel, value);
				}
				remove
				{
					RemoveHandler(EventId.MouseWheel, value);
				}
			}
	public event EventHandler Move
			{
				add
				{
					AddHandler(EventId.Move, value);
				}
				remove
				{
					RemoveHandler(EventId.Move, value);
				}
			}
	public event PaintEventHandler Paint
			{
				add
				{
					AddHandler(EventId.Paint, value);
				}
				remove
				{
					RemoveHandler(EventId.Paint, value);
				}
			}
	public event EventHandler ParentChanged
			{
				add
				{
					AddHandler(EventId.ParentChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.ParentChanged, value);
				}
			}
#if !CONFIG_COMPACT_FORMS
	public event QueryAccessibilityHelpEventHandler QueryAccessibilityHelp
			{
				add
				{
					AddHandler(EventId.QueryAccessibilityHelp, value);
				}
				remove
				{
					RemoveHandler(EventId.QueryAccessibilityHelp, value);
				}
			}
	public event QueryContinueDragEventHandler QueryContinueDrag
			{
				add
				{
					AddHandler(EventId.QueryContinueDrag, value);
				}
				remove
				{
					RemoveHandler(EventId.QueryContinueDrag, value);
				}
			}
#endif
	public event EventHandler Resize
			{
				add
				{
					AddHandler(EventId.Resize, value);
				}
				remove
				{
					RemoveHandler(EventId.Resize, value);
				}
			}
	public event EventHandler RightToLeftChanged
			{
				add
				{
					AddHandler(EventId.RightToLeftChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.RightToLeftChanged, value);
				}
			}
	public event EventHandler SizeChanged
			{
				add
				{
					AddHandler(EventId.SizeChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.SizeChanged, value);
				}
			}
	public event EventHandler StyleChanged
			{
				add
				{
					AddHandler(EventId.StyleChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.StyleChanged, value);
				}
			}
	public event EventHandler SystemColorsChanged
			{
				add
				{
					AddHandler(EventId.SystemColorsChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.SystemColorsChanged, value);
				}
			}
	public event EventHandler TabIndexChanged
			{
				add
				{
					AddHandler(EventId.TabIndexChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.TabIndexChanged, value);
				}
			}
	public event EventHandler TabStopChanged
			{
				add
				{
					AddHandler(EventId.TabStopChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.TabStopChanged, value);
				}
			}
	public event EventHandler TextChanged
			{
				add
				{
					AddHandler(EventId.TextChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.TextChanged, value);
				}
			}
	public event EventHandler Validated
			{
				add
				{
					AddHandler(EventId.Validated, value);
				}
				remove
				{
					RemoveHandler(EventId.Validated, value);
				}
			}
#if CONFIG_COMPONENT_MODEL
	public event CancelEventHandler Validating
			{
				add
				{
					AddHandler(EventId.Validating, value);
				}
				remove
				{
					RemoveHandler(EventId.Validating, value);
				}
			}
#endif
	public event EventHandler VisibleChanged
			{
				add
				{
					AddHandler(EventId.VisibleChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.VisibleChanged, value);
				}
			}

	// Virtual methods that deliver events in response to control behaviour.
	protected virtual void OnBackColorChanged(EventArgs e)
			{
				// Set the background color on the toolkit window.
				if(toolkitWindow != null)
				{
					toolkitWindow.SetBackground(backColor);
				}

				// Invalidate this control to repaint it.
				Invalidate();

				// Invoke the event handler.
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.BackColorChanged));
				if(handler != null)
				{
					handler(this, e);
				}

				// Pass the change notification to the children.
				int posn;
				for(posn = 0; posn < numChildren; ++posn)
				{
					children[posn].OnParentBackColorChanged(e);
				}
			}
	[TODO]
	protected virtual void OnBackgroundImageChanged(EventArgs e)
			{
				// TODO: set the background image on the toolkit window.

				// Invalidate this control to repaint it.
				Invalidate();

				// Invoke the event handler.
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.BackgroundImageChanged));
				if(handler != null)
				{
					handler(this, e);
				}

				// Pass the change notification to the children.
				int posn;
				for(posn = 0; posn < numChildren; ++posn)
				{
					children[posn].OnParentBackgroundImageChanged(e);
				}
			}
	protected virtual void OnBindingContextChanged(EventArgs e)
			{
				// Invoke the event handler.
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.BindingContextChanged));
				if(handler != null)
				{
					handler(this, e);
				}

				// Pass the change notification to the children.
				int posn;
				for(posn = 0; posn < numChildren; ++posn)
				{
					children[posn].OnParentBindingContextChanged(e);
				}
			}
	protected virtual void OnCausesValidationChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.CausesValidationChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnChangeUICues(UICuesEventArgs e)
			{
				UICuesEventHandler handler;
				handler = (UICuesEventHandler)
					(GetHandler(EventId.ChangeUICues));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnClick(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.Click));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnContextMenuChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.ContextMenuChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnControlAdded(ControlEventArgs e)
			{
				ControlEventHandler handler;
				handler = (ControlEventHandler)
					(GetHandler(EventId.ControlAdded));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnControlRemoved(ControlEventArgs e)
			{
				ControlEventHandler handler;
				handler = (ControlEventHandler)
					(GetHandler(EventId.ControlRemoved));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnCreateControl()
			{
				// Nothing to do in the base class.
			}
	protected virtual void OnCursorChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.CursorChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnDockChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.DockChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnDoubleClick(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.DoubleClick));
				if(handler != null)
				{
					handler(this, e);
				}
			}
#if !CONFIG_COMPACT_FORMS
	protected virtual void OnDragDrop(DragEventArgs e)
			{
				DragEventHandler handler;
				handler = (DragEventHandler)(GetHandler(EventId.DragDrop));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnDragEnter(DragEventArgs e)
			{
				DragEventHandler handler;
				handler = (DragEventHandler)(GetHandler(EventId.DragEnter));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnDragLeave(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.DragLeave));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnDragOver(DragEventArgs e)
			{
				DragEventHandler handler;
				handler = (DragEventHandler)(GetHandler(EventId.DragOver));
				if(handler != null)
				{
					handler(this, e);
				}
			}
#endif
	protected virtual void OnEnabledChanged(EventArgs e)
			{
				// Repaint the control.
				Invalidate();
				Update();

				// Invoke the event handler.
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.EnabledChanged));
				if(handler != null)
				{
					handler(this, e);
				}

				// Pass the change notification to the children.
				int posn;
				for(posn = 0; posn < numChildren; ++posn)
				{
					children[posn].OnParentEnabledChanged(e);
				}
			}
	protected virtual void OnEnter(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.Enter));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnFontChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.FontChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnForeColorChanged(EventArgs e)
			{
				// Set the foreground color on the toolkit window.
				if(toolkitWindow != null)
				{
					toolkitWindow.SetForeground(foreColor);
				}

				// Invalidate this control to repaint it.
				Invalidate();

				// Invoke the event handler.
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.ForeColorChanged));
				if(handler != null)
				{
					handler(this, e);
				}

				// Pass the change notification to the children.
				int posn;
				for(posn = 0; posn < numChildren; ++posn)
				{
					children[posn].OnParentForeColorChanged(e);
				}
			}
#if !CONFIG_COMPACT_FORMS
	protected virtual void OnGiveFeedback(GiveFeedbackEventArgs e)
			{
				GiveFeedbackEventHandler handler;
				handler = (GiveFeedbackEventHandler)
					(GetHandler(EventId.GiveFeedback));
				if(handler != null)
				{
					handler(this, e);
				}
			}
#endif
	protected virtual void OnGotFocus(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.GotFocus));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnHandleCreated(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.HandleCreated));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnHandleDestroyed(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.HandleDestroyed));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnHelpRequested(HelpEventArgs e)
			{
				HelpEventHandler handler;
				handler = (HelpEventHandler)
					(GetHandler(EventId.HelpRequested));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnImeModeChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.ImeModeChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnInvalidated(InvalidateEventArgs e)
			{
				InvalidateEventHandler handler;
				handler = (InvalidateEventHandler)
					(GetHandler(EventId.Invalidated));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnKeyDown(KeyEventArgs e)
			{
				KeyEventHandler handler;
				handler = (KeyEventHandler)(GetHandler(EventId.KeyDown));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnKeyPress(KeyPressEventArgs e)
			{
				KeyPressEventHandler handler;
				handler = (KeyPressEventHandler)(GetHandler(EventId.KeyPress));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnKeyUp(KeyEventArgs e)
			{
				KeyEventHandler handler;
				handler = (KeyEventHandler)(GetHandler(EventId.KeyUp));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnLayout(LayoutEventArgs e)
			{
				LayoutEventHandler handler;
				handler = (LayoutEventHandler)(GetHandler(EventId.Layout));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnLeave(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.Leave));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnLocationChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.LocationChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnLostFocus(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.LostFocus));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnMouseDown(MouseEventArgs e)
			{
				MouseEventHandler handler;
				handler = (MouseEventHandler)(GetHandler(EventId.MouseDown));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnMouseEnter(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.MouseEnter));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnMouseHover(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.MouseHover));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnMouseLeave(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.MouseLeave));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnMouseMove(MouseEventArgs e)
			{
				MouseEventHandler handler;
				handler = (MouseEventHandler)(GetHandler(EventId.MouseMove));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnMouseUp(MouseEventArgs e)
			{
				MouseEventHandler handler;
				handler = (MouseEventHandler)(GetHandler(EventId.MouseUp));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnMouseWheel(MouseEventArgs e)
			{
				MouseEventHandler handler;
				handler = (MouseEventHandler)(GetHandler(EventId.MouseWheel));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnMove(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.Move));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnPaint(PaintEventArgs e)
			{
				PaintEventHandler handler;
				handler = (PaintEventHandler)(GetHandler(EventId.Paint));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnPaintBackground(PaintEventArgs e)
			{
				// Nothing to do here in this implementation.
			}
	protected virtual void OnParentBackColorChanged(EventArgs e)
			{
				if(backColor.IsEmpty && backgroundImage == null)
				{
					OnBackColorChanged(e);
				}
			}
	protected virtual void OnParentBackgroundImageChanged(EventArgs e)
			{
				if(backColor.IsEmpty && backgroundImage == null)
				{
					OnBackgroundImageChanged(e);
				}
			}
	protected virtual void OnParentBindingContextChanged(EventArgs e)
			{
				if(bindingContext == null)
				{
					OnBindingContextChanged(e);
				}
			}
	protected virtual void OnParentChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.ParentChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnParentEnabledChanged(EventArgs e)
			{
				bool parentEnabled = parent.Enabled;
				if((!parentEnabled && enabled) != (parentEnabled && enabled))
				{
					OnEnabledChanged(e);
				}
			}
	protected virtual void OnParentFontChanged(EventArgs e)
			{
				if(font == null)
				{
					OnFontChanged(e);
				}
			}
	protected virtual void OnParentForeColorChanged(EventArgs e)
			{
				if(foreColor.IsEmpty)
				{
					OnForeColorChanged(e);
				}
			}
	protected virtual void OnParentRightToLeftChanged(EventArgs e)
			{
				if(rightToLeft == (byte)(RightToLeft.Inherit))
				{
					OnRightToLeftChanged(e);
				}
			}
	protected virtual void OnParentVisibleChanged(EventArgs e)
			{
				bool parentVisible = parent.Visible;
				if((!parentVisible && visible) != (parentVisible && visible))
				{
					OnVisibleChanged(e);
				}
			}
#if !CONFIG_COMPACT_FORMS
	protected virtual void OnQueryAccessibilityHelp
				(QueryAccessibilityHelpEventArgs e)
			{
				QueryAccessibilityHelpEventHandler handler;
				handler = (QueryAccessibilityHelpEventHandler)
					(GetHandler(EventId.QueryAccessibilityHelp));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnQueryContinueDrag
				(QueryContinueDragEventArgs e)
			{
				QueryContinueDragEventHandler handler;
				handler = (QueryContinueDragEventHandler)
					(GetHandler(EventId.QueryContinueDrag));
				if(handler != null)
				{
					handler(this, e);
				}
			}
#endif
	protected virtual void OnResize(EventArgs e)
			{
				// Force a repaint if "ResizeRedraw" is set.
				if(GetStyle(ControlStyles.ResizeRedraw))
				{
					Invalidate();
				}

				// Invoke the event handler.
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.Resize));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnRightToLeftChanged(EventArgs e)
			{
				// Invoke the event handler.
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.RightToLeftChanged));
				if(handler != null)
				{
					handler(this, e);
				}

				// Pass the change notification to the children.
				int posn;
				for(posn = 0; posn < numChildren; ++posn)
				{
					children[posn].OnParentRightToLeftChanged(e);
				}
			}
	protected virtual void OnSizeChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.SizeChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnStyleChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.StyleChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnSystemColorsChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.SystemColorsChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnTabIndexChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.TabIndexChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnTabStopChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)
					(GetHandler(EventId.TabStopChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnTextChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.TextChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}
	protected virtual void OnValidated(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.Validated));
				if(handler != null)
				{
					handler(this, e);
				}
			}
#if CONFIG_COMPONENT_MODEL
	protected virtual void OnValidating(CancelEventArgs e)
			{
				CancelEventHandler handler;
				handler = (CancelEventHandler)(GetHandler(EventId.Validating));
				if(handler != null)
				{
					handler(this, e);
				}
			}
#endif
	protected virtual void OnVisibleChanged(EventArgs e)
			{
				// Map or unmap the toolkit window.
				if(toolkitWindow != null)
				{
					toolkitWindow.IsMapped = visible;
				}
				else if(Visible && !disposed)
				{
					// Create the toolkit window for the first time.
					// This will also map the toolkit window to the screen.
					CreateControl();
				}

				// Invoke the event handler.
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.VisibleChanged));
				if(handler != null)
				{
					handler(this, e);
				}

				// Pass the change notification to the children.
				int posn;
				for(posn = 0; posn < numChildren; ++posn)
				{
					children[posn].OnParentVisibleChanged(e);
				}
			}

	// Begin a batch change operation, which suppresses updates until
	// all controls in the batch have been changed.
	internal virtual void BeginBatchChange() {}

	// End a batch change operation.
	internal virtual void EndBatchChange() {}

	// Move a child to below another.  Does not update "children".
	private static void MoveToBelow(Control after, Control child)
			{
				if(after.toolkitWindow != null &&
				   child.toolkitWindow != null)
				{
					child.toolkitWindow.MoveToBelow(after.toolkitWindow);
				}
			}

	// Move a child to above another.  Does not update "children".
	private static void MoveToAbove(Control before, Control child)
			{
				if(before.toolkitWindow != null &&
				   child.toolkitWindow != null)
				{
					child.toolkitWindow.MoveToAbove(before.toolkitWindow);
				}
			}

	// Reparent this control within the windowing system.
	// If "newParent" is null, then remove it from all parents.
	// The control will end up at the bottom of the sibling stack.
	private void Reparent(Control newParent)
			{
				if(toolkitWindow != null)
				{
					if(newParent == null)
					{
						toolkitWindow.Reparent(null, left, top);
					}
					else
					{
						toolkitWindow.Reparent
							(newParent.toolkitWindow, left, top);
					}
					toolkitWindow.Lower();
				}
			}

	// Collection of child controls.
	public class ControlCollection
		: IList, ICollection, IEnumerable
#if !CONFIG_COMPACT_FORMS
		, ICloneable
#endif
	{
		// Internal state.
		private Control owner;

		// Constructor.
		public ControlCollection(Control owner)
				{
					this.owner = owner;
				}

		// Get the control at a specific index.
		public virtual Control this[int index]
				{
					get
					{
						if(index < 0 || index >= owner.numChildren)
						{
							throw new ArgumentOutOfRangeException
								("index", S._("SWF_InvalidControlIndex"));
						}
						return owner.children[index];
					}
				}

		// Implement the ICollection interface.
		public void CopyTo(Array array, int index)
				{
					if(owner.numChildren > 0)
					{
						Array.Copy(owner.children, 0, array, index,
								   owner.numChildren);
					}
				}
		public virtual int Count
				{
					get
					{
						return owner.numChildren;
					}
				}
		bool ICollection.IsSynchronized
				{
					get
					{
						return false;
					}
				}
		Object ICollection.SyncRoot
				{
					get
					{
						return this;
					}
				}

		// Implement the IList interface.
		int IList.Add(Object value)
				{
					if(value is Control)
					{
						int count = Count;
						Add((Control)value);
						return count;
					}
					else
					{
						throw new ArgumentException(S._("SWF_NotAControl"));
					}
				}
		public virtual void Clear()
				{
					// TODO
				}
		bool IList.Contains(Object value)
				{
					if(value is Control)
					{
						return Contains((Control)value);
					}
					else
					{
						return false;
					}
				}
		int IList.IndexOf(Object value)
				{
					if(value is Control)
					{
						return IndexOf((Control)value);
					}
					else
					{
						return -1;
					}
				}
		void IList.Insert(int index, Object value)
				{
					throw new NotSupportedException();
				}
		void IList.Remove(Object value)
				{
					if(value is Control)
					{
						Remove((Control)value);
					}
				}
		public virtual void RemoveAt(int index)
				{
					Remove(this[index]);
				}
		bool IList.IsFixedSize
				{
					get
					{
						return false;
					}
				}
		public bool IsReadOnly
				{ 
					get
					{
						return false;
					}
				}
		Object IList.this[int index]
				{
					get
					{
						return this[index];
					}
					set
					{
						throw new NotSupportedException();
					}
				}

		// Implement the IEnumerable interface.
		public virtual IEnumerator GetEnumerator()
				{
					return new ControlCollectionEnumerator(owner);
				}

		// Add a control to this collection.
		public virtual void Add(Control value)
				{
					if(value != null)
					{
						if(value.Parent == owner)
						{
							value.SendToBack();
						}
						else
						{
							value.Parent = owner;
						}
					}
				}

		// Determine whether a specific control is in this collection
		public bool Contains(Control control)
				{
					return (control != null && control.Parent == owner);
				}

		// Get the index of a specific child in this collection.
		public int GetChildIndex(Control child)
				{
					int index = IndexOf(child);
					if(index == -1)
					{
						throw new ArgumentException
							(S._("SWF_ControlNotPresent"));
					}
					return index;
				}

		// Get the index of a specific child in the collection.
		public int IndexOf(Control control)
				{
					int index;
					for(index = 0; index < owner.numChildren; ++index)
					{
						if(owner.children[index] == control)
						{
							return index;
						}
					}
					return -1;
				}

		// Remove a specific control from the collection.
		public virtual void Remove(Control value)
				{
					if(value != null && value.Parent == owner)
					{
						value.Parent = null;
					}
				}

		// Change the index of a child in the collection.
		public void SetChildIndex(Control child, int newIndex)
				{
					// Validate the parameters.
					if(child == null)
					{
						throw new ArgumentNullException("child");
					}
					else if(child.Parent != owner)
					{
						throw new ArgumentException
							(S._("SWF_ControlNotAChild"));
					}
					if(newIndex < 0)
					{
						newIndex = 0;
					}
					else if(newIndex >= owner.numChildren)
					{
						newIndex = owner.numChildren - 1;
					}

					// Find the previous index of the control.
					int index = IndexOf(child);

					// Move the control.
					int posn;
					if(index < newIndex)
					{
						MoveToBelow(owner.children[newIndex], child);
						for(posn = index; posn < newIndex; ++index)
						{
							owner.children[posn] = owner.children[posn + 1];
						}
						owner.children[newIndex] = child;
					}
					else if(index > newIndex)
					{
						MoveToAbove(owner.children[newIndex], child);
						for(posn = newIndex; posn > index; --posn)
						{
							owner.children[posn] = owner.children[posn - 1];
						}
						owner.children[newIndex] = child;
					}
				}

#if !CONFIG_COMPACT_FORMS

		// Implement the ICloneable interface.
		Object ICloneable.Clone()
				{
					return this;
				}

		// Add a range of controls to this collection.
		public virtual void AddRange(Control[] controls)
				{
					if(controls == null)
					{
						throw new ArgumentNullException("controls");
					}
					owner.BeginBatchChange();
					foreach(Control control in controls)
					{
						Add(control);
					}
					owner.EndBatchChange();
				}

		// Determine whether two control collections are equal.
		public override bool Equals(Object obj)
				{
					if(obj is ControlCollection)
					{
						return (((ControlCollection)obj).owner == owner);
					}
					else
					{
						return false;
					}
				}

		// Get the hash code for this collection.
		public override int GetHashCode()
				{
					return owner.GetHashCode();
				}

		// Get the index of a specific child in this collection.
		public int GetChildIndex(Control child, bool throwException)
				{
					int index = IndexOf(child);
					if(index == -1 && throwException)
					{
						throw new ArgumentException
							(S._("SWF_ControlNotPresent"));
					}
					return index;
				}

#endif // !CONFIG_COMPACT_FORMS

	}; // class ControlCollection

	// Enumerator class for control collections.
	private sealed class ControlCollectionEnumerator : IEnumerator
	{
		// Internal state.
		private Control owner;
		private int index;

		// Constructor.
		public ControlCollectionEnumerator(Control owner)
				{
					this.owner = owner;
					this.index = -1;
				}

		// Implement the IEnumerator interface.
		public bool MoveNext()
				{
					++index;
					return (index < owner.numChildren);
				}
		public void Reset()
				{
					index = -1;
				}
		public Object Current
				{
					get
					{
						if(index < 0 || index >= owner.numChildren)
						{
							throw new InvalidOperationException
								(S._("SWF_InvalidControlIndex"));
						}
						return owner.children[index];
					}
				}

	}; // class ControlCollectionEnumerator

	// Toolkit event that is emitted for an expose on this window.
	void IToolkitEventSink.ToolkitExpose(Graphics graphics)
			{
				// TODO: clip rectangles
				PaintEventArgs e = new PaintEventArgs
					(graphics, new Rectangle(0, 0, width, height));
				OnPaint(e);
			}

	// Toolkit event that is emitted when the mouse enters this window.
	void IToolkitEventSink.ToolkitMouseEnter()
			{
				OnMouseEnter(EventArgs.Empty);
			}

	// Toolkit event that is emitted when the mouse leaves this window.
	void IToolkitEventSink.ToolkitMouseLeave()
			{
				OnMouseLeave(EventArgs.Empty);
			}

	// Toolkit event that is emitted when the focus enters this window.
	void IToolkitEventSink.ToolkitFocusEnter()
			{
				OnEnter(EventArgs.Empty);
			}

	// Toolkit event that is emitted when the focus leaves this window.
	void IToolkitEventSink.ToolkitFocusLeave()
			{
				OnLeave(EventArgs.Empty);
			}

	// Toolkit event that is emitted for a key down event.
	void IToolkitEventSink.ToolkitKeyDown(ToolkitKeys key)
			{
				OnKeyDown(new KeyEventArgs((Keys)key));
			}

	// Toolkit event that is emitted for a key up event.
	void IToolkitEventSink.ToolkitKeyUp(ToolkitKeys key)
			{
				OnKeyUp(new KeyEventArgs((Keys)key));
			}

	// Toolkit event that is emitted for a key character event.
	void IToolkitEventSink.ToolkitKeyChar(char charCode)
			{
				OnKeyPress(new KeyPressEventArgs(charCode));
			}

	// Toolkit event that is emitted for a mouse down event.
	void IToolkitEventSink.ToolkitMouseDown
				(ToolkitMouseButtons buttons, ToolkitKeys modifiers,
				 int clicks, int x, int y, int delta)
		 	{
				OnMouseDown(new MouseEventArgs
					((MouseButtons)buttons, clicks, x, y, delta));
			}

	// Toolkit event that is emitted for a mouse up event.
	void IToolkitEventSink.ToolkitMouseUp
				(ToolkitMouseButtons buttons, ToolkitKeys modifiers,
				 int clicks, int x, int y, int delta)
			{
				OnMouseUp(new MouseEventArgs
					((MouseButtons)buttons, clicks, x, y, delta));
			}

	// Toolkit event that is emitted for a mouse hover event.
	void IToolkitEventSink.ToolkitMouseHover
				(ToolkitMouseButtons buttons, ToolkitKeys modifiers,
				 int clicks, int x, int y, int delta)
			{
				OnMouseHover(new MouseEventArgs
					((MouseButtons)buttons, clicks, x, y, delta));
			}

	// Toolkit event that is emitted for a mouse move event.
	void IToolkitEventSink.ToolkitMouseMove
				(ToolkitMouseButtons buttons, ToolkitKeys modifiers,
				 int clicks, int x, int y, int delta)
			{
				OnMouseMove(new MouseEventArgs
					((MouseButtons)buttons, clicks, x, y, delta));
			}

	// Toolkit event that is emitted for a mouse wheel event.
	void IToolkitEventSink.ToolkitMouseWheel
				(ToolkitMouseButtons buttons, ToolkitKeys modifiers,
				 int clicks, int x, int y, int delta)
			{
				OnMouseWheel(new MouseEventArgs
					((MouseButtons)buttons, clicks, x, y, delta));
			}

	// Toolkit event that is emitted when the window is moved by
	// external means (e.g. the user dragging the window).
	void IToolkitEventSink.ToolkitExternalMove(int x, int y)
			{
				if(x != left || y != top)
				{
					Location = new Point(x, y);
				}
			}

	// Toolkit event that is emitted when the window is resized by
	// external means (e.g. the user resizing the window).
	void IToolkitEventSink.ToolkitExternalResize(int width, int height)
			{
				if(width != this.width || height != this.height)
				{
					Size = new Size(width, height);
				}
			}

	// Create a brush that can be used to fill with the background color/image.
	internal Brush CreateBackgroundBrush()
			{
				// TODO: background images
				return new SolidBrush(BackColor);
			}

}; // class Control

}; // namespace System.Windows.Forms
