/*
 * FlowControl.cs - Implementation of the
 *			"Microsoft.VisualBasic.FlowControl" class.
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
using System.Collections;

[StandardModule]
[EditorBrowsable(EditorBrowsableState.Never)]
public sealed class FlowControl
{
	// Cannot instantiate this class.
	private FlowControl() {}

	// Throw an exception if the argument is a value type.
	public static void CheckForSyncLockOnValueType(Object obj)
			{
				if(obj != null && obj.GetType().IsValueType)
				{
					throw new ArgumentException(S._("VB_IsValueType"));
				}
			}

	// Get the enumerator for an array.
	public static IEnumerator ForEachInArr(Array ary)
			{
				return ary.GetEnumerator();
			}

	// Get the enumerator for an object.
	[TODO]
	public static IEnumerator ForEachInObj(Object obj)
			{
				// TODO
				return null;
			}

	// Get the next object in an enumeration sequence.
	public static bool ForEachNextObj(ref Object obj, IEnumerator enumerator)
			{
				if(enumerator.MoveNext())
				{
					obj = enumerator.Current;
					return true;
				}
				else
				{
					obj = null;
					return false;
				}
			}

	// Initialize a "for" loop.
	[TODO]
	public static bool ForLoopInitObj
				(Object Counter, Object Start, Object Limit,
				 Object StepValue, ref Object LoopForResult,
				 ref Object CounterResult)
			{
				// TODO
				return false;
			}

	// Check for the end of a "decimal" iteration.
	public static bool ForNextCheckDec
				(Decimal count, Decimal limit, Decimal StepValue)
			{
				if(StepValue >= 0.0m)
				{
					return (count <= limit);
				}
				else
				{
					return (count >= limit);
				}
			}

	// Check for the end of an object iteration.
	[TODO]
	public static bool ForNextCheckObj
				(Object Counter, Object LoopObj, ref Object CounterResult)
			{
				// TODO
				return false;
			}

	// Check for the end of a "float" iteration.
	public static bool ForNextCheckR4
				(float count, float limit, float StepValue)
			{
				if(StepValue >= 0.0f)
				{
					return (count <= limit);
				}
				else
				{
					return (count >= limit);
				}
			}

	// Check for the end of a "double" iteration.
	public static bool ForNextCheckR8
				(double count, double limit, double StepValue)
			{
				if(StepValue >= 0.0)
				{
					return (count <= limit);
				}
				else
				{
					return (count >= limit);
				}
			}

}; // class FlowControl

}; // namespace Microsoft.VisualBasic.CompilerServices
