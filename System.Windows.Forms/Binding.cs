/*
 * Binding.cs - Implementation of the
 *			"System.Windows.Forms.Binding" class.
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

public class Binding
{
	// Internal state.
	private String propertyName;
	private Object dataSource;
	internal BindingManagerBase bindingManagerBase;
	private BindingMemberInfo bindingMemberInfo;
	private Control control;
	private bool isBinding;

	// Constructor.
	public Binding(String propertyName, Object dataSource, String dataMember)
			{
				this.propertyName = propertyName;
				this.dataSource = dataSource;
				this.bindingMemberInfo = new BindingMemberInfo(dataMember);
			}

	// Get this object's properties.
	public BindingManagerBase BindingManagerBase
			{
				get
				{
					return bindingManagerBase;
				}
			}
	public BindingMemberInfo BindingMemberInfo
			{
				get
				{
					return bindingMemberInfo;
				}
			}
	public Control Control
			{
				get
				{
					return control;
				}
			}
	public Object DataSource
			{
				get
				{
					return dataSource;
				}
			}
	public bool IsBinding
			{
				get
				{
					return isBinding;
				}
			}
	public String PropertyName
			{
				get
				{
					return propertyName;
				}
			}

	// Associate a control with this binding.
	internal void AssociateControl(Control control)
			{
				this.control = control;
			}

	// Event that is raised when a property's value is bound.
	public event ConvertEventHandler Format;

	// Event that is raised when a property is changed.
	public event ConvertEventHandler Parse;

	// Raise the "Format" event.
	protected virtual void OnFormat(ConvertEventArgs e)
			{
				if(Format != null)
				{
					Format(this, e);
				}
			}

	// Raise the "Parse" event.
	protected virtual void OnParse(ConvertEventArgs e)
			{
				if(Parse != null)
				{
					Parse(this, e);
				}
			}

	// Pull data from the data source (called from BindingManagerBase).
	[TODO]
	internal void PullData()
			{
				// TODO
			}

	// Push data to the data bound control (called from BindingManagerBase).
	[TODO]
	internal void PushData()
			{
				// TODO
			}

}; // class Binding

}; // namespace System.Windows.Forms
