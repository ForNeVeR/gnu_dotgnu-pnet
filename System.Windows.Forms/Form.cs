/*
 * Form.cs - Implementation of the
 *			"System.Windows.Forms.Form" class.
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

using System.Drawing;
using System.Drawing.Toolkit;

[TODO]
public class Form : ContainerControl
{
	// Internal state.
	private IButtonControl acceptButton;
	private IButtonControl cancelButton;
	private bool autoScale;
	private bool isMdiChild;
	private bool isMdiContainer;
	private bool keyPreview;
	private bool topLevel;
	private Size autoScaleBaseSize;
	private DialogResult dialogResult;
	private FormBorderStyle borderStyle;
	private Icon icon;
	private ToolkitWindowFlags windowFlags;
	private Size maximumSize;
	private Size minimumSize;
	private Form[] mdiChildren;
	private Form[] ownedForms;
	private Form mdiParent;
	private Form owner;
	private MainMenu menu;
	private MainMenu mergedMenu;
	private double opacity;
	private SizeGripStyle sizeGripStyle;
	private FormStartPosition formStartPosition;
	private Color transparencyKey;
	private FormWindowState windowState;
	internal static Form activeForm;

	// Constructor.
	public Form()
			{
				visible = false;
				autoScale = true;
				topLevel = true;
				borderStyle = FormBorderStyle.Sizable;
				mdiChildren = new Form [0];
				ownedForms = new Form [0];
				opacity = 1.0;
				windowFlags = ToolkitWindowFlags.Default;
				formStartPosition = FormStartPosition.WindowsDefaultLocation;
				windowState = FormWindowState.Normal;
			}

	// Get or set this control's properties.
	public IButtonControl AcceptButton
			{
				get
				{
					return acceptButton;
				}
				set
				{
					acceptButton = value;
				}
			}
	public static Form ActiveForm
			{
				get
				{
					return activeForm;
				}
			}
	[TODO]
	public Form ActiveMdiChild
			{
				get
				{
					// TODO
					return null;
				}
			}
	public bool AutoScale
			{
				get
				{
					return autoScale;
				}
				set
				{
					autoScale = value;
				}
			}
	public Size AutoScaleBaseSize
			{
				get
				{
					return autoScaleBaseSize;
				}
				set
				{
					autoScaleBaseSize = value;
				}
			}
	public override bool AutoScroll
			{
				get
				{
					return base.AutoScroll;
				}
				set
				{
					base.AutoScroll = value;
				}
			}
	public override Color BackColor
			{
				get
				{
					// The base implementation takes care of the default.
					return base.BackColor;
				}
				set
				{
					base.BackColor = value;
				}
			}
	public IButtonControl CancelButton
			{
				get
				{
					return cancelButton;
				}
				set
				{
					cancelButton = value;
				}
			}
	public Rectangle DesktopBounds
			{
				get
				{
					// Use the ordinary bounds.
					return Bounds;
				}
				set
				{
					Bounds = value;
				}
			}
	public Point DesktopLocation
			{
				get
				{
					// Use the ordinary location.
					return Location;
				}
				set
				{
					Location = value;
				}
			}
	public DialogResult DialogResult
			{
				get
				{
					return dialogResult;
				}
				set
				{
					dialogResult = value;
				}
			}
	public FormBorderStyle FormBorderStyle
			{
				get
				{
					return borderStyle;
				}
				set
				{
					if(borderStyle != value)
					{
						borderStyle = value;
						SetWindowFlag(0, true);
					}
				}
			}
	public bool HelpButton
			{
				get
				{
					return GetWindowFlag(ToolkitWindowFlags.Help);
				}
				set
				{
					SetWindowFlag(ToolkitWindowFlags.Help, value);
				}
			}
	[TODO]
	public Icon Icon
			{
				get
				{
					return icon;
				}
				set
				{
					// TODO: send the icon to the window manager.
					icon = value;
				}
			}
	public bool IsMdiChild
			{
				get
				{
					return isMdiChild;
				}
			}
	[TODO]
	public bool IsMdiContainer
			{
				get
				{
					return isMdiContainer;
				}
				set
				{
					// TODO: switch the form into MDI mode
					isMdiContainer = value;
				}
			}
	public bool KeyPreview
			{
				get
				{
					return keyPreview;
				}
				set
				{
					keyPreview = value;
				}
			}
	public bool MaximizeBox
			{
				get
				{
					return GetWindowFlag(ToolkitWindowFlags.Maximize);
				}
				set
				{
					SetWindowFlag(ToolkitWindowFlags.Maximize, value);
				}
			}
	[TODO]
	public Size MaximumSize
			{
				get
				{
					return maximumSize;
				}
				set
				{
					// TODO: send the maximum size to the window manager.
					if(value.Width < 0)
					{
						throw new ArgumentOutOfRangeException
							("value.Width", S._("SWF_NonNegative"));
					}
					if(value.Height < 0)
					{
						throw new ArgumentOutOfRangeException
							("value.Height", S._("SWF_NonNegative"));
					}
					maximumSize = value;
				}
			}
	public Form[] MdiChildren
			{
				get
				{
					return mdiChildren;
				}
			}
	[TODO]
	public Form MdiParent
			{
				get
				{
					return mdiParent;
				}
				set
				{
					// TODO: validate the parent
					mdiParent = value;
				}
			}
	public bool MinimizeBox
			{
				get
				{
					return GetWindowFlag(ToolkitWindowFlags.Minimize);
				}
				set
				{
					SetWindowFlag(ToolkitWindowFlags.Minimize, value);
				}
			}
	[TODO]
	public Size MinimumSize
			{
				get
				{
					return minimumSize;
				}
				set
				{
					// TODO: send the minimum size to the window manager.
					if(value.Width < 0)
					{
						throw new ArgumentOutOfRangeException
							("value.Width", S._("SWF_NonNegative"));
					}
					if(value.Height < 0)
					{
						throw new ArgumentOutOfRangeException
							("value.Height", S._("SWF_NonNegative"));
					}
					minimumSize = value;
				}
			}
	public MainMenu Menu
			{
				get
				{
					return menu;
				}
				set
				{
					menu = value;
				}
			}
	public MainMenu MergedMenu
			{
				get
				{
					return mergedMenu;
				}
			}
	public bool Modal
			{
				get
				{
					return GetWindowFlag(ToolkitWindowFlags.Modal);
				}
			}
	public double Opacity
			{
				get
				{
					return opacity;
				}
				set
				{
					// We don't support transparent windows,
					// so just save the value away for later.
					opacity = value;
				}
			}
	public Form[] OwnedForms
			{
				get
				{
					return ownedForms;
				}
			}
	public Form Owner
			{
				get
				{
					return owner;
				}
				set
				{
					owner = value;
				}
			}
	public bool ShowInTaskBar
			{
				get
				{
					return GetWindowFlag(ToolkitWindowFlags.ShowInTaskBar);
				}
				set
				{
					SetWindowFlag(ToolkitWindowFlags.ShowInTaskBar, value);
				}
			}
	public new Size Size
			{
				get
				{
					return base.Size;
				}
				set
				{
					base.Size = value;
				}
			}
	public SizeGripStyle SizeGripStyle
			{
				get
				{
					return sizeGripStyle;
				}
				set
				{
					sizeGripStyle = value;
				}
			}
	public FormStartPosition StartPosition
			{
				get
				{
					return formStartPosition;
				}
				set
				{
					formStartPosition = value;
				}
			}
	public bool TopLevel
			{
				get
				{
					return topLevel;
				}
				set
				{
					topLevel = value;
				}
			}
	public bool TopMost
			{
				get
				{
					return GetWindowFlag(ToolkitWindowFlags.TopMost);
				}
				set
				{
					SetWindowFlag(ToolkitWindowFlags.TopMost, value);
				}
			}
	public Color TransparencyKey
			{
				get
				{
					return transparencyKey;
				}
				set
				{
					transparencyKey = value;
				}
			}
	[TODO]
	public FormWindowState WindowState
			{
				get
				{
					return windowState;
				}
				set
				{
					// TODO: pass the state change to the window manager
					windowState = value;
				}
			}

	// Create the toolkit window underlying this control.
	internal override IToolkitWindow CreateToolkitWindow(IToolkitWindow parent)
			{
				// TODO: dialog forms
				CreateParams cp = CreateParams;
				IToolkitWindow window =
					ToolkitManager.Toolkit.CreateTopLevelWindow
						(cp.Width, cp.Height);
				window.SetTitle(cp.Caption);
				SetWindowFlags(window);
				return window;
			}

	// Determine if this is a top-level control which cannot have parents.
	internal override bool IsTopLevel
			{
				get
				{
					return true;
				}
			}

	// Handle the text changed event from "Control".
	protected override void OnTextChanged(EventArgs e)
			{
				if(toolkitWindow != null)
				{
					toolkitWindow.SetTitle(Text);
				}
				base.OnTextChanged(e);
			}

	// Get the current state of a window decoration flag.
	private bool GetWindowFlag(ToolkitWindowFlags flag)
			{
				return ((windowFlags & flag) == flag);
			}

	// Set the current state of the window decoration flags on a window.
	private void SetWindowFlags(IToolkitWindow window)
			{
				ToolkitWindowFlags flags = windowFlags;
				switch(borderStyle)
				{
					case FormBorderStyle.None:
					{
						flags &= ~(ToolkitWindowFlags.Maximize |
								   ToolkitWindowFlags.ResizeHandles |
								   ToolkitWindowFlags.Resize |
								   ToolkitWindowFlags.Border);
					}
					break;

					case FormBorderStyle.Fixed3D:
					case FormBorderStyle.FixedDialog:
					case FormBorderStyle.FixedSingle:
					{
						flags &= ~(ToolkitWindowFlags.Maximize |
								   ToolkitWindowFlags.ResizeHandles |
								   ToolkitWindowFlags.Resize);
					}
					break;

					case FormBorderStyle.FixedToolWindow:
					{
						flags &= ~(ToolkitWindowFlags.Maximize |
								   ToolkitWindowFlags.ResizeHandles |
								   ToolkitWindowFlags.Resize |
								   ToolkitWindowFlags.ShowInTaskBar);
						flags |= ToolkitWindowFlags.ToolWindow;
					}
					break;

					case FormBorderStyle.Sizable: break;

					case FormBorderStyle.SizableToolWindow:
					{
						flags &= ~(ToolkitWindowFlags.ShowInTaskBar);
						flags |= ToolkitWindowFlags.ToolWindow;
					}
					break;
				}
				window.SetWindowFlags(flags);
			}

	// Set the current state of a window decoration flag.
	private void SetWindowFlag(ToolkitWindowFlags flag, bool value)
			{
				// Alter the flag setting.
				if(value)
				{
					windowFlags |= flag;
				}
				else
				{
					windowFlags &= ~flag;
				}

				// Pass the flags to the window manager.
				if(toolkitWindow != null)
				{
					SetWindowFlags(toolkitWindow);
				}
			}

}; // class Form

}; // namespace System.Windows.Forms
