/*
 * TestDelegate.cs - Tests for the "Delegate" class.
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

using CSUnit;
using System;
using System.Threading;

public class TestDelegate : TestCase
{
	// Constructor.
	public TestDelegate(String name)
			: base(name)
			{
				// Nothing to do here.
			}

	// Set up for the tests.
	protected override void Setup()
			{
				// Nothing to do here.
			}

	// Clean up after the tests.
	protected override void Cleanup()
			{
				// Nothing to do here.
			}

	/*
	 * Test async calls with reference results.
	 */
	private delegate object ObjectMethod(object x, object y);
	
	private object ObjectAdd(object x, object y)
	{
		return (int)x + (int)y;
	}
	
	public void TestAsyncCallWithRefs()
	{
		int result;
		IAsyncResult ar;
		ObjectMethod m = new ObjectMethod(ObjectAdd);
		
		ar = m.BeginInvoke(10, 20, null, null);
		
		result = (int)m.EndInvoke(ar);
		
		AssertEquals("result==30", result, 30);
	}

	/*
	 * Test async calls with int results.
	 */
	private delegate int IntMethod(int x, int y);
	
	private int IntAdd(int x, int y)
	{
		return x + y;
	}

	public void TestAsyncCallWithInts()
	{
		int result;
		IAsyncResult ar;
		IntMethod m = new IntMethod(IntAdd);
		
		ar = m.BeginInvoke(10, 20, null, null);
		
		result = m.EndInvoke(ar);
		
		AssertEquals("result==30", result, 30);
	}
	
	/*
	 * Test async calls with double results.
	 */
	private delegate double DoubleMethod(double x, double y);
	
	private double DoubleAdd(double x, double y)
	{
		return x + y;
	}

	public void TestAsyncCallWithDouble()
	{
		double result;
		IAsyncResult ar;
		DoubleMethod m = new DoubleMethod(DoubleAdd);
		
		ar = m.BeginInvoke(10, 20, null, null);
		
		result = m.EndInvoke(ar);
		
		AssertEquals("result==30", result, (double)30);
	}	

	/*
	 * Test async calls with custom valuetype results.
	 */
	private delegate Point PointMethod(Point x, Point y);
	
	public struct Point
	{
		public int X;
		public int Y;
		
		public Point(int x, int y)
		{
			this.X = x;
			this.Y = y;
		}
		
	}
	
	private Point PointAdd(Point x, Point y)
	{
		return new Point(x.X + y.X, x.Y + y.Y);
	}

	public void TestAsyncCallWithValueType()
	{
		Point result;
		IAsyncResult ar;
		PointMethod m = new PointMethod(PointAdd);
		
		ar = m.BeginInvoke(new Point(10, 20), new Point(30, 40), null, null);
		
		result = m.EndInvoke(ar);
		
		AssertEquals("result==Point(40, 60)", result, new Point(40, 60));
	}	
}; // class TestArray
