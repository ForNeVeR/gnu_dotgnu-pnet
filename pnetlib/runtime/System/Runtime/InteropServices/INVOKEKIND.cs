/*
 * INVOKEKIND.cs - Implementation of the
 *			"System.Runtime.InteropServices.INVOKEKIND" class.
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

namespace System.Runtime.InteropServices
{

#if CONFIG_COM_INTEROP

[ComVisible(false)]
[Serializable]
#if CONFIG_FRAMEWORK_1_2
[Obsolete("Use the class in System.Runtime.InteropServices.ComTypes instead")]
#endif
public enum INVOKEKIND
{
	INVOKE_FUNC           = 0x0001,
	INVOKE_PROPERTYGET    = 0x0002,
	INVOKE_PROPERTYPUT    = 0x0004,
	INVOKE_PROPERTYPUTREF = 0x0008

}; // enum INVOKEKIND

#endif // CONFIG_COM_INTEROP

}; // namespace System.Runtime.InteropServices
