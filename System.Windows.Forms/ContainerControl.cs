/*
 * ContainerControl.cs - Implementation of the
 *			"System.Windows.Forms.ContainerControl" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2003  Neil Cawse.
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
	public bool ActivateControl(Control active)
			{
				bool activated = ActivateControlInternal(active);
				//TODO: Scroll active into view if its not visible.
				return activated;
			}

	private bool ActivateControlInternal(Control active)
			{
				if (Parent != null)
				{
					ContainerControl container = Parent.GetContainerControl() as ContainerControl;
					if (container != null && container.ActiveControl != this && !container.ActivateControlInternal(this))
						return false; 
				}
				if (active == this.activeControl)
					return true;
				
				bool activated;
				if (active == this)
					activated = FocusControl(null);
				else
					activated = FocusControl(active);
				Form form = FindForm();
				if (form != null)
				form.UpdateDefaultButton();
				return activated;
			}

	protected virtual void UpdateDefaultButton()
			{
			}

	private bool FocusControl(Control active)
			{
				activeControl = active;
				if (activeControl != null)
					activeControl.Focus();
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
					if (key == Keys.Tab)
					{
						if (ProcessTabKey((keyData & Keys.Shift) == 0))
							return true;
					}
					else if (key == Keys.Left || key == Keys.Up || key == Keys.Right || key == Keys.Down)
					{
						Control control;
						if (activeControl != null)
							control = activeControl.Parent;
						else
							control = this;
						bool forward = (key == Keys.Right || key == Keys.Down);
						if (control.SelectNextControl(activeControl, forward , false, false, true))
							return true;
					}
				}
				return base.ProcessDialogKey(keyData);
			}

	// Process a key mnemonic.
	protected override bool ProcessMnemonic(char charCode)
			{
				if (Controls.Count == 0)
				{
					return false; 
				}
				Control active = ActiveControl;
				// Find the bottom most active control or the container if there isn't one.
				while (!(active is ContainerControl) && active != null)
					active = (active as ContainerControl).ActiveControl;

				// Process the mnemonics if needed.
				bool back = false;
				Control mnemonicControl = active;

				do
				{
					mnemonicControl = GetNextControl(mnemonicControl, true);
					if (mnemonicControl == null)
					{
						if (back)
							break;
						back = true;
					}
					else
					{
						if (mnemonicControl.ProcessMnemonicInternal(charCode))
							return true; 
					}
				}
				while (mnemonicControl != active);
				return false; 

			}

	// Process the tab key.
	protected virtual bool ProcessTabKey(bool forward)
			{
				return SelectNextControl(activeControl, forward, true, true, false);
			}

	// Select this control.
	protected override void Select(bool directed, bool forward)
			{
				if (Parent != null)
				{
					IContainerControl container = Parent.GetContainerControl();
					if (container != null)
					{
						container.ActiveControl = this;
						if (directed && container.ActiveControl == this)
							SelectNextControl(null, forward, true, true, false);		 
					}
				}
				if (directed)
					SelectNextControl(null, forward, true, true, false);
			}

	private void SetFocus(Control control)
			{
				ContainerControl container;
				if (control != null && control.Visible)
				{
					// TODO
					control.Focus();
					return; 
				}
				// Find the first visible container.
				for (container = this; container != null; container = container.GetContainerControl() as ContainerControl)
				{
					if (container.Visible)
					{
						// TODO
						container.Focus();
						break;
					}
				}
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
