/*
 * ContainerControl.cs - Implementation of the
 *			"System.Windows.Forms.ContainerControl" class.
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

public class ContainerControl : ScrollableControl, IContainerControl
{
	// Internal state.
	private Control activeControl;

	// Constructor.
	public ContainerControl() {}

	// Get or set this control's properties.
	public Control ActiveControl
			{
				get
				{
					return activeControl;
				}
				set
				{
					if(!ActivateControl(value))
					{
						throw new ArgumentException
							(S._("SWF_CouldNotActivate"));
					}
				}
			}
	public override BindingContext BindingContext
			{
				get
				{
					// Containers must always have a binding context.
					BindingContext context = base.BindingContext;
					if(context == null)
					{
						context = new BindingContext();
						base.BindingContext = context;
					}
					return context;
				}
				set
				{
					base.BindingContext = value;
				}
			}
	protected override CreateParams CreateParams
			{
				get
				{
					return base.CreateParams;
				}
			}
	public Form ParentForm
			{
				get
				{
					return FindForm();
				}
			}

	// Activate a specific control.
	[TODO]
	public bool ActivateControl(Control active)
			{
				// TODO
				return true;
			}

	// Dispose of this control.
	protected override void Dispose(bool disposing)
			{
				activeControl = null;
				base.Dispose(disposing);
			}

	// Validate the last invalidated control.
	[TODO]
	public bool Validate()
			{
				// TODO
				return true;
			}

	// Override the "control removed" event.
	protected override void OnControlRemoved(ControlEventArgs e)
			{
				if(activeControl == e.Control)
				{
					activeControl = null;
				}
				base.OnControlRemoved(e);
			}

	// Override the "control created" event.
	protected override void OnCreateControl()
			{
				base.OnCreateControl();
				OnBindingContextChanged(EventArgs.Empty);
			}

	// Process a dialog character.
	[TODO]
	protected override bool ProcessDialogChar(char charCode)
			{
				// TODO
				return base.ProcessDialogChar(charCode);
			}

	// Process a dialog key.
	protected override bool ProcessDialogKey(Keys keyData)
			{
				if ((keyData & (Keys.Alt | Keys.Control)) == 0)
				{
					Keys key = keyData & Keys.KeyCode;
					if (key != Keys.Tab)
					{
						switch (key)
						{
							case Keys.Left:
							case Keys.Up:
							case Keys.Right:
							case Keys.Down:
								Control control;
								if (activeControl != null)
									control = activeControl.Parent;
								else
									control = this;
								bool forward = (key == Keys.Right || key == Keys.Down);
								if (control.SelectNextControl(activeControl, forward , false, false, true))
									return true;
								break;
						}
						return base.ProcessDialogKey(keyData);

					}
					else if (ProcessTabKey((keyData & Keys.Shift) == 0))
						return true;
				}
				return base.ProcessDialogKey(keyData);
			}

	// Process a key mnemonic.
	[TODO]
	protected override bool ProcessMnemonic(char charCode)
			{
				// TODO
				return base.ProcessMnemonic(charCode);
			}

	// Process the tab key.
	protected virtual bool ProcessTabKey(bool forward)
			{
				return SelectNextControl(activeControl, forward, true, true, false);
			}

	// Select this control.
	[TODO]
	protected override void Select(bool directed, bool forward)
			{
				// TODO
			}

#if !CONFIG_COMPACT_FORMS

	// Process a message.
	protected override void WndProc(ref Message m)
			{
				base.WndProc(ref m);
			}

#endif // !CONFIG_COMPACT_FORMS

}; // class ContainerControl

}; // namespace System.Windows.Forms
