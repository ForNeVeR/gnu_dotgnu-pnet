/*
 * ArrayConstructor.cs - Object that represents the "Array" constructor.
 *
 * Copyright (C) 2003 Southern Storm Software, Pty Ltd.
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
 
namespace Microsoft.JScript
{

using System;
using System.Reflection;
using System.Globalization;
using Microsoft.JScript.Vsa;

public sealed class ArrayConstructor : ScriptFunction
{
	// Constructor.
	internal ArrayConstructor(FunctionPrototype parent)
			: base(parent, "Array", 1) {}

	// Construct a new array from a list of elements.
	public ArrayObject ConstructArray(Object[] args)
			{
				// TODO
				return null;
			}

	// Construct a new "Array" instance.
	[JSFunction(JSFunctionAttributeEnum.HasVarArgs)]
	public new ArrayObject CreateInstance(params Object[] args)
			{
				// TODO
				return null;
			}

	// Invoke this constructor.
	[JSFunction(JSFunctionAttributeEnum.HasVarArgs)]
	public ArrayObject Invoke(params Object[] args)
			{
				if(args.Length == 1 && args[0] is Array)
				{
					// TODO: wrap the normal array in a JScript array object.
					return null;
				}
				else
				{
					return CreateInstance(args);
				}
			}

	// Perform a call on this object.
	internal override Object Call(Object thisob, Object[] args)
			{
				return Invoke(args);
			}

	// Perform a constructor call on this object.
	internal override Object CallConstructor(Object[] args)
			{
				return CreateInstance(args);
			}

}; // class ArrayConstructor

}; // namespace Microsoft.JScript
