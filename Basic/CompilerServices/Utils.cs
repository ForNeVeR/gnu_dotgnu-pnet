/*
 * Utils.cs - Implementation of the
 *			"Microsoft.VisualBasic.Utils" class.
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
using System.Globalization;

[StandardModule]
[EditorBrowsable(EditorBrowsableState.Never)]
public sealed class Utils
{
	// Cannot instantiate this class.
	private Utils() {}

	// Copy the contents of an array.
	[TODO]
	public static Array CopyArray(Array arySrc, Array aryDest)
			{
				// TODO
				return null;
			}

	// Convert a method name into a string, using VB-style syntax.
	[TODO]
	public static String MethodToString(MethodBase Method)
			{
				// TODO
				return null;
			}

	// Set the culture information to use.
	[TODO]
	public static Object SetCultureInfo(CultureInfo Culture)
			{
				// TODO
				return null;
			}

	// Throw an exception by HRESULT.
	[TODO]
	public static void ThrowException(int hr)
			{
				// TODO
			}

}; // class Utils

}; // namespace Microsoft.VisualBasic.CompilerServices
