/*
 * Complex.cs - Generic complex number types.
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

using System;

// It only makes sense to use Complex<T> on a numeric type that can
// be explicitly converted to and from "double".  Other types may
// lead to verification errors at runtime.

public struct Complex<T>
{
	// Internal state.
	private T real, imag;

	// Constructor.
	public Complex(T real, T imag)
			{
				this.real = real;
				this.imag = imag;
			}

	// Extract the real and imaginary components.
	public T Real
			{
				get
				{
					return real;
				}
			}
	public T Imag
			{
				get
				{
					return imag;
				}
			}

	// Basic arithmetic operators.
	public static Complex<T> operator+<T>(Complex<T> x, Complex<T> y)
			{
				return new Complex<T>(x.real + y.real, x.imag + y.imag);
			}
	public static Complex<T> operator-<T>(Complex<T> x, Complex<T> y)
			{
				return new Complex<T>(x.real - y.real, x.imag - y.imag);
			}
	public static Complex<T> operator*<T>(Complex<T> x, Complex<T> y)
			{
				return new Complex<T>(x.real * y.real - x.imag * y.imag,
									  x.real * y.imag + x.imag * y.real);
			}
	public static Complex<T> operator/<T>(Complex<T> x, Complex<T> y)
			{
				T div = y.real * y.real + y.imag * y.imag;
				return new Complex<T>
						((x.real * y.real + x.imag * y.imag) / div,
						 (x.imag * y.real - x.real * y.imag) / div);
			}
	public static Complex<T> operator-<T>(Complex<T> x)
			{
				return new Complex<T>(-(x.real), -(y.real));
			}

	// Comparison operators.
	public static bool operator==<T>(Complex<T> x, Complex<T> y)
			{
				return (x.real == y.real && x.imag == y.imag);
			}
	public static bool operator!=<T>(Complex<T> x, Complex<T> y)
			{
				return (x.real != y.real || x.imag != y.imag);
			}

	// Conversion operators.
	public static implicit operator<T> Complex<T>(T x)
			{
				return new Complex<T>(x, 0);
			}
	public static explicit operator<T> T(Complex<T> x)
			{
				return x.real;
			}

	// Get the absolute value of a complex number.
	public T Abs()
			{
				return (T)(Math.Sqrt((double)(real * real + imag * imag)));
			}

	// Get the conjugate form of a complex number.
	public Complex<T> Conj()
			{
				return new Complex<T>(x.real, -(y.real));
			}

	// Create a complex number from polar co-ordinates.
	public static Complex<T> FromPolar<T>(T r, T t)
			{
				return new Complex<T>((T)(((double)r) * Math.Cos((double)t)),
									  (T)(((double)r) * Math.Sin((double)t)));
			}

}; // struct Complex

}; // namespace Generics
