/*
 * S.cs - String resources.
 *
 * Copyright (c) 2003  Southern Storm Software, Pty Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

namespace Generics
{

using System.Reflection;
using System.Resources;

// This class provides string resource support to the rest
// of the "Generics" library assembly.  It is accessed using
// the "S._(tag)" convention.

internal sealed class S
{
	// Cached copy of the resources for this assembly.
	private static ResourceManager genericsResources = null;

	// Helper for obtaining string resources for this assembly.
	public static String _(String tag)
			{
				lock(typeof(S))
				{
					String value;
					if(genericsResources == null)
					{
						genericsResources = new ResourceManager
							("Generics", (typeof(S)).Assembly);
					}
					return genericsResources.GetString(tag, null);
				}
			}

}; // class S

}; // namespace Generics
