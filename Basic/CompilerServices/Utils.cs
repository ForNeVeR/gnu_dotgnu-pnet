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
using System.Threading;
using System.Text;

[StandardModule]
[EditorBrowsable(EditorBrowsableState.Never)]
public sealed class Utils
{
	// Cannot instantiate this class.
	private Utils() {}

	// Copy the contents of an array.
	public static Array CopyArray(Array arySrc, Array aryDest)
			{
				if(arySrc != null)
				{
					// Check that the arrays have the same rank and dimensions.
					int rank = arySrc.Rank;
					if(rank != aryDest.Rank)
					{
						ThrowException
							(new InvalidCastException
								(S._("VB_MismatchedRanks")));
					}
					for(int dim = 0; dim < rank; ++dim)
					{
						if(arySrc.GetUpperBound(dim) !=
						   aryDest.GetUpperBound(dim))
						{
							ThrowException
								(new ArrayTypeMismatchException
									(S._("VB_MismatchedDimensions")));
						}
					}
					Array.Copy(arySrc, aryDest, arySrc.Length);
				}
				return aryDest;
			}

	// Append the VB form of a type name to a string builder.
	internal static void AppendType(StringBuilder builder, Type type)
			{
				if(type == typeof(System.Byte))
				{
					builder.Append("Byte");
				}
				else if(type == typeof(System.Int16))
				{
					builder.Append("Short");
				}
				else if(type == typeof(System.Int32))
				{
					builder.Append("Integer");
				}
				else if(type == typeof(System.Int64))
				{
					builder.Append("Long");
				}
				else if(type == typeof(System.Single))
				{
					builder.Append("Single");
				}
				else if(type == typeof(System.Double))
				{
					builder.Append("Double");
				}
				else if(type == typeof(System.Boolean))
				{
					builder.Append("Boolean");
				}
				else if(type == typeof(System.Char))
				{
					builder.Append("Char");
				}
				else if(type == typeof(System.String))
				{
					builder.Append("String");
				}
				else if(type == typeof(System.DateTime))
				{
					builder.Append("Date");
				}
				else if(type == typeof(System.Decimal))
				{
					builder.Append("Decimal");
				}
				else if(type == typeof(System.Object))
				{
					builder.Append("Object");
				}
				else if(type.IsArray)
				{
					Type elemType = type.GetElementType();
					while(elemType.IsArray)
					{
						elemType = elemType.GetElementType();
					}
					AppendType(builder, elemType);
					int rank;
					while(type != elemType)
					{
						builder.Append('(');
						int rank = type.GetArrayRank();
						while(rank > 1)
						{
							builder.Append(',');
							--rank;
						}
						builder.Append(')');
						type = type.GetElementType();
					}
				}
				else
				{
					builder.Append(type.FullName);
				}
			}

	// Convert a method name into a string, using VB-style syntax.
	public static String MethodToString(MethodBase Method)
			{
				bool isCtor;
				Type returnType;

				// Determine if the method is a constructor and get
				// the return type if it isn't.
				if(Method is ConstructorInfo)
				{
					isCtor = true;
					returnType = typeof(Void);
				}
				else
				{
					isCtor = false;
					returnType = ((MethodInfo)Method).ReturnType;
				}

				// Create a string builder and output the access level.
				StringBuilder builder = new StringBuilder();
				if(Method.IsVirtual)
				{
					if(Method.DeclaringType.IsInterface)
					{
						builder.Append("Overrides ");
					}
				}
				switch(Method.Attributes & MethodAttributes.MemberAccessMask)
				{
					case MethodAttributes.Private:
						builder.Append("Private "); break;

					case MethodAttributes.Assembly:
						builder.Append("Friend "); break;

					case MethodAttributes.Family:
						builder.Append("Protected "); break;

					case MethodAttributes.FamORAssem:
						builder.Append("Protected Friend "); break;

					case MethodAttributes.Public:
						builder.Append("Public "); break;
				}

				// Output the method kind and its name.
				if(isCtor)
				{
					builder.Append("Sub New ");
				}
				else if(returnType == typeof(Void))
				{
					builder.Append("Sub ");
				}
				else
				{
					builder.Append("Function ");
				}
				builder.Append(Method.Name);

				// Output the method parameters.
				ParameterInfo[] parameters = Method.GetParameters();
				int index;
				Type type;
				String name;
				Object value;
				builder.Append(" (");
				for(index = 0; index < parameters.Length; ++index)
				{
					if(index > 0)
					{
						builder.Append(", ");
					}
					type = parameters[index].ParameterType;
					if(parameters[index].IsOptional)
					{
						builder.Append("Optional ");
					}
					if(type.IsByRef)
					{
						builder.Append("ByRef ");
					}
					else
					{
						builder.Append("ByVal ");
					}
					if(type.IsArray && parameters[index].IsDefined
							(typeof(ParamArrayAttribute), false))
					{
						builder.Append("ParamArray ");
					}
					name = parameters[index].Name;
					if(name != null)
					{
						builder.Append(name);
					}
					else
					{
						builder.Append(String.Format("_p{0}", index));
					}
					builder.Append(" As ");
					AppendType(builder, returnType);
					if(parameters[index].IsOptional)
					{
						builder.Append(" = ");
						value = parameters[index].DefaultValue;
						if(value != null)
						{
							if(value is String)
							{
								builder.Append('"');
								builder.Append(value);
								builder.Append('"');
							}
							else
							{
								builder.Append(value.ToString());
							}
						}
						else
						{
							builder.Append("Nothing");
						}
					}
				}
				builder.Append(")");

				// Output the return type for functions.
				if(returnType != typeof(Void))
				{
					builder.Append(" As ");
					AppendType(builder, returnType);
				}

				// Return the final string to the caller.
				return builder.ToString();
			}

	// Set the culture information to use.
	public static Object SetCultureInfo(CultureInfo Culture)
			{
				CultureInfo prev = CultureInfo.CurrentCulture;
				Thread.CurrentThread.CurrentCulture = Culture;
				return prev;
			}

	// Throw an exception by number.
	public static void ThrowException(int hr)
			{
				Exception exception =
					ErrObject.CreateExceptionFromNumber(hr, null);
				SetErrorNumber(hr);
				throw exception;
			}

	// Throw a particular exception, after setting the error number.
	internal static void ThrowException(Exception exception)
			{
				SetErrorNumber(ErrObject.GetNumberForException(exception));
				throw exception;
			}

	// Set a particular error number in the program's error object.
	private static void SetErrorNumber(int number)
			{
				ErrObject error = Information.Err();
				error.Clear();
				error.Number = number;
			}

	// Full width digit characters.
	private static char[] fullWidthDigits =
			{'\uFF10', '\uFF11', '\uFF12', '\uFF13', '\uFF14',
			 '\uFF15', '\uFF16', '\uFF17', '\uFF18', '\uFF19'};

	// Convert full-width digit characters into half-width digits.
	// This is useful in CJK locales where it is common for users to
	// expect that full-width forms of digits can be used interchangeably
	// with half-width forms.
	internal static String FixDigits(String str)
			{
				if(str == null)
				{
					return null;
				}
				if(str.IndexOfAny(fullWidthDigits) == -1)
				{
					return str;
				}
				StringBuilder builder = new StringBuilder(str.Length);
				foreach(char ch in str)
				{
					if(ch >= '\uFF10' && ch <= '\uFF19')
					{
						builder.Append((char)(ch - (0xFF10 + 0x0030)));
					}
					else
					{
						builder.Append(ch);
					}
				}
				return builder.ToString();
			}

}; // class Utils

}; // namespace Microsoft.VisualBasic.CompilerServices
