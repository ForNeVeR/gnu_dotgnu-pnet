/*
 * LateBinding.cs - Implementation of the
 *			"Microsoft.VisualBasic.LateBinding" class.
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

namespace Microsoft.VisualBasic.CompilerServices
{

using System;
using System.ComponentModel;
using System.Reflection;

[StandardModule]
[EditorBrowsable(EditorBrowsableState.Never)]
public sealed class LateBinding
{
	// Cannot instantiate this class.
	private LateBinding() {}

	// Perform a late call.
	[TODO]
	public static void LateCall(Object o, Type objType,
								String name, Object[] args,
								String[] paramnames,
								bool[] CopyBack)
			{
				// TODO
			}

	// Perform a late property get.
	[TODO]
	public static Object LateGet(Object o, Type objType,
								 String name, Object[] args,
								 String[] paramnames, bool[] CopyBack)
			{
				// TODO
				return null;
			}

	// Perform a late index get.
	[TODO]
	public static Object LateIndexGet(Object o, Object[] args,
									  String[] paramnames)
			{
				// TODO
				return null;
			}

	// Perform a late index set.
	[TODO]
	public static void LateIndexSet(Object o, Object[] args,
									String[] paramnames)
			{
				// TODO
			}

	// Perform a complex late index set.
	[TODO]
	public static void LateIndexSetComplex(Object o, Object[] args,
										   String[] paramnames,
										   bool OptimisticSet,
										   bool RValueBase)
			{
				// TODO
			}

	// Perform a late property set.
	[TODO]
	public static void LateSet(Object o, Type objType, String name,
							   Object[] args, String[] paramnames)
			{
				// TODO
			}

	// Perform a complex late property set.
	[TODO]
	public static void LateSetComplex(Object o, Type objType, String name,
							          Object[] args, String[] paramnames,
									  bool OptimisticSet, bool RValueBase)
			{
				// TODO
			}

}; // class LateBinding

}; // namespace Microsoft.VisualBasic.CompilerServices
