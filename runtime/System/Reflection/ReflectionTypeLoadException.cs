/*
 * ReflectionTypeLoadException.cs - Implementation of the
 *			"System.Reflection.ReflectionTypeLoadException" class.
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

namespace System.Reflection
{

#if CONFIG_REFLECTION && !ECMA_COMPAT

using System;
using System.Runtime.Serialization;

[Serializable]
public sealed class ReflectionTypeLoadException
	: SystemException, ISerializable
{
	// Internal state.
	private Type[] classes;
	private Exception[] exceptions;

	// Constructors.
	public ReflectionTypeLoadException(Type[] classes, Exception[] exceptions)
			: this(classes, exceptions, null) {}
	public ReflectionTypeLoadException
				(Type[] classes, Exception[] exceptions, String message)
			: base(message)
			{
				this.classes = classes;
				this.exceptions = exceptions;
			}
	internal ReflectionTypeLoadException(SerializationInfo info,
										 StreamingContext context)
			: base(info, context) {}

	// Exception properties.
	public Type[] Types
			{
				get
				{
					return classes;
				}
			}
	public Exception[] LoaderExceptions
			{
				get
				{
					return exceptions;
				}
			}

	// Get the default message to use for this exception type.
	internal override String MessageDefault
			{
				get
				{
					return _("Exception_ReflectionTypeLoad");
				}
			}

	// Get the default HResult value for this type of exception.
	internal override uint HResultDefault
			{
				get
				{
					return 0x80131602;
				}
			}

	// Implement the ISerializable interface.
	public override void GetObjectData(SerializationInfo info,
									   StreamingContext context)
			{
				base.GetObjectData(info, context);
				info.AddValue("Types", classes, typeof(Type[]));
				info.AddValue("Exceptions", exceptions, typeof(Exception[]));
			}

}; // class ReflectionTypeLoadException

#endif // CONFIG_REFLECTION && !ECMA_COMPAT

}; // namespace System.Reflection
