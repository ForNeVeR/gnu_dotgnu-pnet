/*
 * ObjectConstructor.cs - Object that represents the "Object" constructor.
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

public sealed class ObjectConstructor : ScriptFunction
{
	// Constructor.
	internal ObjectConstructor() : base(null, "Object", 1) {}

	// Initialize this object, after all dependencies have been created.
	internal void Init(VsaEngine engine, ScriptObject parent)
			{
				// Set the prototype and engine.
				this.parent = parent;
				this.engine = engine;
				InitPrototype(parent);

				// Set the "prototype" property value.
				ScriptObject prototype =
					EngineInstance.GetEngineInstance(engine)
						.GetObjectPrototype();
				Put("prototype", prototype,
				    PropertyAttributes.ReadOnly |
					PropertyAttributes.DontEnum |
					PropertyAttributes.DontDelete);
			}

	// Construct a new "Object" instance.
	public JSObject ConstructObject()
			{
				ScriptObject prototype =
					EngineInstance.GetEngineInstance(engine)
						.GetObjectPrototype();
				return new JSObject(prototype);
			}
	[JSFunction(JSFunctionAttributeEnum.HasVarArgs)]
	public new Object CreateInstance(params Object[] args)
			{
				if(args.Length == 0)
				{
					return ConstructObject();
				}
				else if(args[0] == null || args[0] == DBNull.Value)
				{
					return ConstructObject();
				}
				else
				{
					// TODO
					return args[0];
				}
			}

	// Invoke this constructor.
	[JSFunction(JSFunctionAttributeEnum.HasVarArgs)]
	public Object Invoke(params Object[] args)
			{
				return Call(null, args);
			}

	// Perform a call on this object.
	internal override Object Call(Object thisob, Object[] args)
			{
				if(args.Length == 0)
				{
					return ConstructObject();
				}
				else if(args[0] == null || args[0] == DBNull.Value)
				{
					return CreateInstance(args);
				}
				else
				{
					// TODO
					return this;
				}
			}

	// Perform a constructor call on this object.
	internal override Object CallConstructor(Object[] args)
			{
				return CreateInstance(args);
			}

}; // class ObjectConstructor

}; // namespace Microsoft.JScript
