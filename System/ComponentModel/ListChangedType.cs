/*
 * ListChangedType.cs - Implementation of the
 *		"System.ComponentModel.ListChangedType" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * This program is free software, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace System.ComponentModel
{

#if !ECMA_COMPAT

public enum ListChangedType
{
	Reset                     = 0,
	ItemAdded                 = 1,
	ItemDeleted               = 2,
	ItemMoved                 = 3,
	ItemChanged               = 4,
	PropertyDescriptorAdded   = 5,
	PropertyDescriptorDeleted = 6,
	PropertyDescriptorChanged = 7

}; // enum ListChangedType

#endif // !ECMA_COMPAT

}; // namespace System.ComponentModel