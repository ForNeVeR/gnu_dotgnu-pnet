/*
 * CompilationRelaxationsAttribute.cs - Implementation of the
 *	"System.Runtime.CompilerServices.CompilationRelaxationsAttribute" class.
 *
 * Copyright (C) 2001, 2004  Southern Storm Software, Pty Ltd.
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

namespace System.Runtime.CompilerServices
{

#if CONFIG_FRAMEWORK_2_0

[AttributeUsage(AttributeTargets.Assembly |
				AttributeTargets.Class |
				AttributeTargets.Struct |
				AttributeTargets.Constructor |
				AttributeTargets.Method,
				AllowMultiple=false, Inherited=false)]
public class CompilationRelaxationsAttribute : Attribute
{

	private const Int32 invalidStringRelaxations = 0x000C;

	private const Int32 invalidNullReferenceRelaxations = 0x0030;

	private const Int32 invalidInvalidCastRelaxations = 0x00C0;

	private const Int32 invalidArrayRelaxations =  0x0300;

	private const Int32 invalidOverflowRelaxations = 0x0C00;

	// Internal state.
	private int value;

	private void Check(Int32 relaxation)
	{
		if((relaxation & invalidStringRelaxations) ==
											invalidStringRelaxations)
		{
			throw new ArgumentException();
		}
		if((relaxation & invalidNullReferenceRelaxations) == 
											invalidNullReferenceRelaxations)
		{
			throw new ArgumentException();
		}
		if((relaxation & invalidInvalidCastRelaxations) == 
											invalidInvalidCastRelaxations)
		{
			throw new ArgumentException();
		}
		if((relaxation & invalidArrayRelaxations) == 
											invalidArrayRelaxations)
		{
			throw new ArgumentException();
		}
		if((relaxation & invalidOverflowRelaxations) == 
											invalidOverflowRelaxations)
		{
			throw new ArgumentException();
		}
	}

	// Constructors.
	public CompilationRelaxationsAttribute(int relaxations)
			{
				Check(relaxations);
				value = relaxations;
			}

	public CompilationRelaxationsAttribute(CompilationRelaxations relaxations)
			{
				Check((int)relaxations);
				value = (int)relaxations;
			}

	// Properties.
	public int CompilationRelaxations
			{
				get
				{
					return value;
				}
			}

}; // class CompilationRelaxationsAttribute

#endif // !CONFIG_FRAMEWORK_2_0

}; // namespace System.Runtime.CompilerServices
