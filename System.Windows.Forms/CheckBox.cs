/*
 * CheckBox.cs - Implementation of the
 *			"System.Windows.Forms.CheckBox" class.
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

using System.Windows.Forms.Toolkit;

public class CheckBox : ButtonBase
{
	// Internal result.
	private bool autoCheck;
	private bool threeState;
	private CheckState state;

	// Contructor.
	public CheckBox() {}

	// Get or set the "auto check" style for this check box.
	public bool AutoCheck
			{
				get
				{
					return autoCheck;
				}
				set
				{
					if(autoCheck != value)
					{
						autoCheck = value;
					}
				}
			}

	// Get or set the "three state" style for this check box.
	public bool ThreeState
			{
				get
				{
					return threeState;
				}
				set
				{
					if(threeState != value)
					{
						threeState = value;
					}
				}
			}

	// Get or set the check box state.
	public CheckState CheckState
			{
				get
				{
					return state;
				}
				set
				{
					if(state != value)
					{
						state = value;
						OnCheckStateChanged(EventArgs.Empty);
					}
				}
			}

	// Get or set the checked state as a simple boolean value.
	public bool Checked
			{
				get
				{
					return (CheckState != CheckState.Unchecked);
				}
				set
				{
					if(value)
					{
						CheckState = CheckState.Checked;
					}
					else
					{
						CheckState = CheckState.Unchecked;
					}
				}
			}

	// Set the check box state - used by the toolkit in response to events.
	internal void SetCheckState(CheckState state)
			{
				this.state = state;
			}

	// Event that is emitted when the check state changes.
	public event EventHandler CheckStateChanged
			{
				add
				{
					AddHandler(EventId.CheckStateChanged, value);
				}
				remove
				{
					RemoveHandler(EventId.CheckStateChanged, value);
				}
			}

	// Emit the CheckStateChanged event.
	protected virtual void OnCheckStateChanged(EventArgs e)
			{
				EventHandler handler;
				handler = (EventHandler)(GetHandler(EventId.CheckStateChanged));
				if(handler != null)
				{
					handler(this, e);
				}
			}

}; // class CheckBox

}; // namespace System.Windows.Forms
