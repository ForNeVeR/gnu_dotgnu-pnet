/*
 * Matrix.cs - Implementation of the "System.Drawing.Drawing2D.Matrix" class.
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

namespace System.Drawing.Drawing2D
{

public sealed class Matrix : MarshalByRefObject, IDisposable
{
	// Internal state.
	private float m11, m12, m21, m22, dx, dy;

	// Constructors.
	public Matrix()
			{
				m11 = 1.0f;
				m12 = 0.0f;
				m21 = 0.0f;
				m22 = 0.1f;
				dx  = 0.0f;
				dy  = 0.0f;
			}
	[TODO]
	public Matrix(Rectangle rect, Point[] plgpts)
			{
				// TODO
			}
	[TODO]
	public Matrix(RectangleF rect, PointF[] plgpts)
			{
				// TODO
			}
	public Matrix(float m11, float m12, float m21, float m22,
				  float dx, float dy)
			{
				this.m11 = m11;
				this.m12 = m12;
				this.m21 = m21;
				this.m22 = m22;
				this.dx  = dx;
				this.dy  = dy;
			}

	// Destructor.
	~Matrix()
			{
				// Nothing to do here because there is no disposable state.
			}

	// Get the elements of this matrix.
	public float[] Elements
			{
				get
				{
					return new float[] {m11, m12, m21, m22, dx, dy};
				}
			}

	// Determine if this is the identity matrix.
	public bool IsIdentity
			{
				get
				{
					return (m11 == 1.0f && m12 == 0.0f &&
							m21 == 0.0f && m22 == 1.0f &&
							dx == 0.0f && dy == 0.0f);
				}
			}

	// Determine if the matrix is invertible.
	public bool IsInvertible
			{
				get
				{
                	return ((m11 * m22 - m21 * m11) != 0.0f);
				}
			}

	// Get the X offset value.
	public float OffsetX
			{
				get
				{
					return dx;
				}
			}

	// Get the Y offset value.
	public float OffsetY
			{
				get
				{
					return dy;
				}
			}

	// Dispose of this matrix.
	public void Dispose()
			{
				// Nothing to do here because there is no disposable state.
			}

	// Determine if two matrices are equal.
	public override bool Equals(Object obj)
			{
				Matrix other = (obj as Matrix);
				if(other != null)
				{
					return (other.m11 == m11 && other.m12 == m12 &&
							other.m21 == m21 && other.m22 == m22 &&
							other.dx == dx && other.dy == dy);
				}
				else
				{
					return false;
				}
			}

	// Get a hash code for this object.
	public override int GetHashCode()
			{
				return (int)(m11 + m12 + m21 + m22 + dx + dy);
			}

	// Invert this matrix.
	public void Invert()
			{
				float m11, m12, m21, m22, dx, dy;
				float determinant;

				// Compute the determinant and check it.
				determinant = this.m11 * this.m22 - this.m21 * this.m11;
				if(determinant != 0.0f)
				{
					// Get the answer into temporary variables.
					m11 = this.m22 / determinant;
					m12 = -(this.m12 / determinant);
					m21 = -(this.m21 / determinant);
					m22 = this.m11 / determinant;
					dx  = (this.m12 * dy - this.m22 * dx) / determinant;
					dy  = (this.m21 * dx - this.m11 * dy) / determinant;

					// Write the temporary variables back to the matrix.
					this.m11 = m11;
					this.m12 = m12;
					this.m21 = m21;
					this.m22 = m22;
					this.dx  = dx;
					this.dy  = dy;
				}
			}

	// Multiply two matrices and write the result into this one.
	private void Multiply(Matrix matrix1, Matrix matrix2)
			{
				float m11, m12, m21, m22, dx, dy;

				// Compute the result within temporary variables,
				// to prevent overwriting "matrix1" or "matrix2",
				// during the calculation, as either may be "this".
				m11 = matrix1.m11 * matrix2.m11 +
					  matrix1.m12 * matrix2.m21;
				m12 = matrix1.m11 * matrix2.m12 +
				      matrix1.m12 * matrix2.m22;
				m21 = matrix1.m21 * matrix2.m11 +
					  matrix1.m22 * matrix2.m21;
				m22 = matrix1.m21 * matrix2.m12 +
				      matrix1.m22 * matrix2.m22;
				dx  = matrix1.dx  * matrix2.m11 +
				      matrix1.dy  * matrix2.m21 +
					  matrix2.dx;
				dy  = matrix1.dx  * matrix2.m12 +
				      matrix1.dy  * matrix2.m22 +
					  matrix2.dy;

				// Write the result back into the "this" object.
				this.m11 = m11;
				this.m12 = m12;
				this.m21 = m21;
				this.m22 = m22;
				this.dx  = dx;
				this.dy  = dy;
			}

	// Multiply two matrices.
	public void Multiply(Matrix matrix)
			{
				if(matrix == null)
				{
					throw new ArgumentNullException("matrix");
				}
				Multiply(matrix, this);
			}
	public void Multiply(Matrix matrix, MatrixOrder order)
			{
				if(matrix == null)
				{
					throw new ArgumentNullException("matrix");
				}
				if(order == MatrixOrder.Prepend)
				{
					Multiply(matrix, this);
				}
				else
				{
					Multiply(this, matrix);
				}
			}

	// Reset this matrix to the identity matrix.
	public void Reset()
			{
				m11 = 1.0f;
				m12 = 0.0f;
				m21 = 0.0f;
				m22 = 0.1f;
				dx  = 0.0f;
				dy  = 0.0f;
			}

	// Perform a rotation on this matrix.
	public void Rotate(float angle)
			{
				float m11, m12, m21, m22;

				double radians = (angle * (Math.PI / 180.0));
				float cos = (float)(Math.Cos(radians));
				float sin = (float)(Math.Sin(radians));

				m11 = cos * this.m11 + sin * this.m21;
				m12 = cos * this.m12 + sin * this.m22;
				m21 = cos * this.m21 - sin * this.m11;
				m22 = cos * this.m22 - sin * this.m12;

				this.m11 = m11;
				this.m12 = m12;
				this.m21 = m21;
				this.m22 = m22;
			}
	public void Rotate(float angle, MatrixOrder order)
			{
				float m11, m12, m21, m22, dx, dy;

				double radians = (angle * (Math.PI / 180.0));
				float cos = (float)(Math.Cos(radians));
				float sin = (float)(Math.Sin(radians));
	
				if(order == MatrixOrder.Prepend)
				{
					m11 = cos * this.m11 + sin * this.m21;
					m12 = cos * this.m12 + sin * this.m22;
					m21 = cos * this.m21 - sin * this.m11;
					m22 = cos * this.m22 - sin * this.m12;

					this.m11 = m11;
					this.m12 = m12;
					this.m21 = m21;
					this.m22 = m22;
				}
				else
				{
					m11 = this.m11 * cos - this.m12 * sin;
					m12 = this.m11 * sin + this.m12 * cos;
					m21 = this.m21 * cos - this.m22 * sin;
					m22 = this.m21 * sin + this.m22 * cos;
					dx  = this.dx  * cos - this.dy  * sin;
					dy  = this.dx  * sin + this.dy  * cos;

					this.m11 = m11;
					this.m12 = m12;
					this.m21 = m21;
					this.m22 = m22;
					this.dx  = dx;
					this.dy  = dy;
				}
			}

	// Rotate about a specific point.
	public void RotateAt(float angle, PointF point)
			{
				Translate(point.X, point.Y);
				Rotate(angle);
				Translate(-point.X, -point.Y);
			}
	public void RotateAt(float angle, PointF point, MatrixOrder order)
			{
				if(order == MatrixOrder.Prepend)
				{
					Translate(point.X, point.Y);
					Rotate(angle);
					Translate(-point.X, -point.Y);
				}
				else
				{
					Translate(-point.X, -point.Y);
					Rotate(angle, MatrixOrder.Append);
					Translate(point.X, point.Y);
				}
			}

	// Apply a scale factor to this matrix.
	public void Scale(float scaleX, float scaleY)
			{
				m11 *= scaleX;
				m12 *= scaleX;
				m21 *= scaleY;
				m22 *= scaleY;
			}
	public void Scale(float scaleX, float scaleY, MatrixOrder order)
			{
				if(order == MatrixOrder.Prepend)
				{
					m11 *= scaleX;
					m12 *= scaleX;
					m21 *= scaleY;
					m22 *= scaleY;
				}
				else
				{
					m11 *= scaleX;
					m12 *= scaleY;
					m21 *= scaleX;
					m22 *= scaleY;
					dx  *= scaleX;
					dy  *= scaleY;
				}
			}

	// Apply a shear factor to this matrix.
	public void Shear(float shearX, float shearY)
			{
				float m11, m12, m21, m22;

				m11 = this.m11 + this.m21 * shearY;
				m12 = this.m12 + this.m22 * shearY;
				m21 = this.m11 * shearX + this.m21;
				m22 = this.m12 * shearX + this.m22;

				this.m11 = m11;
				this.m12 = m12;
				this.m21 = m21;
				this.m22 = m22;
			}
	public void Shear(float shearX, float shearY, MatrixOrder order)
			{
				if(order == MatrixOrder.Prepend)
				{
					float m11, m12, m21, m22;

					m11 = this.m11 + this.m21 * shearY;
					m12 = this.m12 + this.m22 * shearY;
					m21 = this.m11 * shearX + this.m21;
					m22 = this.m12 * shearX + this.m22;

					this.m11 = m11;
					this.m12 = m12;
					this.m21 = m21;
					this.m22 = m22;
				}
				else
				{
					float m11, m12, m21, m22, dx, dy;

					m11 = this.m11 + this.m12 * shearX;
					m12 = this.m11 * shearY + this.m12;
					m21 = this.m21 + this.m22 * shearX;
					m22 = this.m21 * shearY + this.m22;
					dx  = this.dx  + this.dy * shearX;
					dy  = this.dx  * shearY + this.dy;

					this.m11 = m11;
					this.m12 = m12;
					this.m21 = m21;
					this.m22 = m22;
					this.dx  = dx;
					this.dy  = dy;
				}
			}

	// Transform a list of points.
	public void TransformPoints(Point[] pts)
			{
				if(pts == null)
				{
					throw new ArgumentNullException("pts");
				}
				int posn;
				float x, y;
				for(posn = pts.Length - 1; posn >= 0; --posn)
				{
					x = (float)(pts[posn].X);
					y = (float)(pts[posn].Y);
					pts[posn].X = (int)(x * m11 + y * m21 + dx);
					pts[posn].Y = (int)(x * m12 + y * m22 + dy);
				}
			}
	public void TransformPoints(PointF[] pts)
			{
				if(pts == null)
				{
					throw new ArgumentNullException("pts");
				}
				int posn;
				float x, y;
				for(posn = pts.Length - 1; posn >= 0; --posn)
				{
					x = pts[posn].X;
					y = pts[posn].Y;
					pts[posn].X = x * m11 + y * m21 + dx;
					pts[posn].Y = x * m12 + y * m22 + dy;
				}
			}

	// Transform a list of vectors.
	public void TransformVectors(Point[] pts)
			{
				if(pts == null)
				{
					throw new ArgumentNullException("pts");
				}
				int posn;
				float x, y;
				for(posn = pts.Length - 1; posn >= 0; --posn)
				{
					x = (float)(pts[posn].X);
					y = (float)(pts[posn].Y);
					pts[posn].X = (int)(x * m11 + y * m21);
					pts[posn].Y = (int)(x * m12 + y * m22);
				}
			}
	public void TransformVectors(PointF[] pts)
			{
				if(pts == null)
				{
					throw new ArgumentNullException("pts");
				}
				int posn;
				float x, y;
				for(posn = pts.Length - 1; posn >= 0; --posn)
				{
					x = pts[posn].X;
					y = pts[posn].Y;
					pts[posn].X = x * m11 + y * m21;
					pts[posn].Y = x * m12 + y * m22;
				}
			}

	// Translate the matrix.
	public void Translate(float offsetX, float offsetY)
			{
				dx += offsetX * m11 + offsetY * m21;
				dy += offsetX * m12 + offsetY * m22;
			}
	public void Translate(float offsetX, float offsetY, MatrixOrder order)
			{
				if(order == MatrixOrder.Prepend)
				{
					dx += offsetX * m11 + offsetY * m21;
					dy += offsetX * m12 + offsetY * m22;
				}
				else
				{
					dx += offsetX;
					dy += offsetY;
				}
			}

	// Clone a matrix.
	public static Matrix Clone(Matrix matrix)
			{
				if(matrix != null)
				{
					return new Matrix(matrix.m11, matrix.m12,
									  matrix.m21, matrix.m22,
									  matrix.dx,  matrix.dy);
				}
				else
				{
					return null;
				}
			}

	// Transform a particular point - faster version for only one point.
	internal void TransformPoint(float x, float y, out float ox, out float oy)
			{
				ox = x * m11 + y * m21 + dx;
				oy = x * m12 + y * m22 + dy;
			}

}; // class Matrix

}; // namespace System.Drawing.Drawing2D
