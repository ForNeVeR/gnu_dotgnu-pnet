/*
 * cast6.cs - Test the valid casts and coercions for reference types.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

interface I
{
	void m1();
}

interface J : I
{
	void m2();
}

interface K : I, J
{
	void m3();
}

class Test
{
	public void m1()
	{
		object o1;
		Test t1;
		Test2 t2;
		I i1;
		J j1;
		K k1;
		object[] ao1;
		Test[] at1;
		I[] ai1;

		// "null" can be coerced to any reference type.
		o1 = null;
		t1 = null;
		i1 = null;
		ao1 = null;
		at1 = null;
		ai1 = null;

		// Coercions are allowed up the inheritance tree.
		// Casts are allowed down the inheritance tree.
		o1 = t1;
		t1 = t2;
		t1 = (Test)o1;
		t2 = (Test2)o1;
		t2 = (Test2)t1;

		// Coercions are allowed to implemented interfaces.
		i1 = t2;

		// Coercions are allowed amongst derived interfaces.
		i1 = j1;
		i1 = k1;
		j1 = k1;
	}
}

class Test2 : Test, I
{
}
