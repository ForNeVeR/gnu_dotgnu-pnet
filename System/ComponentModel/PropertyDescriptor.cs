/*
 * PropertyDescriptor.cs - Implementation of the
 *		"System.ComponentModel.ComponentModel.PropertyDescriptor" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace System.ComponentModel
{

#if CONFIG_COMPONENT_MODEL

using System.Collections;
using System.Reflection;
using System.Runtime.InteropServices;

[TODO]
[ComVisible(true)]
public abstract class PropertyDescriptor : MemberDescriptor
{
	// TODO
	public PropertyDescriptor(String name) : base(name) {}
	
	public PropertyDescriptor(String name, Attribute[] attrs) : base(name) {}

	protected PropertyDescriptor(MemberDescriptor desc, Attribute[] attrs) 
				: base(desc) {}

}; // class PropertyDescriptor

#endif // CONFIG_COMPONENT_MODEL

}; // namespace System.ComponentModel
