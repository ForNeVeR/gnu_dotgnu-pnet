/*
 * Delegate.cs - Implementation of the "System.Delegate" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System
{

using System.Reflection;
using System.Runtime.CompilerServices;

public abstract class Delegate : ICloneable
{
	// Internal state.  These three fields must be present here
	// and in this order so that the runtime engine can create
	// delegates from function pointers correctly.
	internal Object     target;
	internal MethodInfo method;
	internal IntPtr		closure;

	// Constructors.
	protected Delegate(Object target, String method)
			{
				if(target == null)
				{
					throw new ArgumentNullException("target");
				}
				if(method == null)
				{
					throw new ArgumentNullException("method");
				}
				this.target = target;
				this.method =
					(target.GetType()).GetMethod(method,
												 BindingFlags.Public |
												 BindingFlags.NonPublic |
												 BindingFlags.Instance);
				if(this.method == null)
				{
					throw new MissingMethodException
						(_("Arg_DelegateMethod"));
				}
			}
	protected Delegate(Type target, String method)
			{
				if(target == null)
				{
					throw new ArgumentNullException("target");
				}
				if(method == null)
				{
					throw new ArgumentNullException("method");
				}
				this.target = null;
				this.method = target.GetMethod(method,
											   BindingFlags.Public |
											   BindingFlags.NonPublic |
											   BindingFlags.Static);
				if(this.method == null)
				{
					throw new MissingMethodException
						(_("Arg_DelegateMethod"));
				}
			}

	// Implement the ICloneable interface.
	public virtual Object Clone()
			{
				return MemberwiseClone();
			}

	// Combine the invocation lists of two delegates.
	public static Delegate Combine(Delegate a, Delegate b)
			{
				if(a == null)
				{
					return b;
				}
				else if(b == null)
				{
					return a;
				}
				else if(a.GetType() != b.GetType())
				{
					throw new ArgumentException(_("Arg_DelegateMismatch"));
				}
				else
				{
					return a.CombineImpl(b);
				}
			}

	// Combine an arbitrary number of delegates.
	public static Delegate Combine(Delegate[] delegates)
			{
				Delegate result;
				int posn;
				if(delegates == null)
				{
					return null;
				}
				result = null;
				for(posn = 0; posn < delegates.Length; ++posn)
				{
					result = Combine(result, delegates[posn]);
				}
				return result;
			}

	// Create a blank instance of a delegate type, and validate its signature.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static Delegate CreateBlankDelegate
				(Type type, ClrMethod method);

	// Create a delegate for an instance method.
	public static Delegate CreateDelegate
				(Type type, Object target, String method)
			{
				MethodInfo methodInfo;

				// Validate the parameters.
				if(type == null)
				{
					throw new ArgumentNullException("type");
				}
				if(target == null)
				{
					throw new ArgumentNullException("target");
				}
				if(method == null)
				{
					throw new ArgumentNullException("method");
				}
				if(!type.IsSubclassOf(typeof(Delegate)))
				{
					throw new ArgumentException("Arg_NotDelegate");
				}

				// Look up the method.
				methodInfo = (target.GetType()).GetMethod
								(method, BindingFlags.Public |
										 BindingFlags.NonPublic |
										 BindingFlags.Instance);
				if(methodInfo == null)
				{
					throw new ArgumentException(_("Arg_DelegateMethod"));
				}

				// Create a blank delegate to hold the method.
				Delegate d = CreateBlankDelegate(type, methodInfo as ClrMethod);
				if(d == null)
				{
					throw new ArgumentException(_("Arg_DelegateSignature"));
				}

				// Fill in the blanks and return the delegate.
				d.target = target;
				d.method = methodInfo;
				return d;
			}

	// Create a delegate for a named static method.
	public static Delegate CreateDelegate
				(Type type, Type target, String method)
			{
				MethodInfo methodInfo;

				// Validate the parameters.
				if(type == null)
				{
					throw new ArgumentNullException("type");
				}
				if(target == null)
				{
					throw new ArgumentNullException("target");
				}
				if(method == null)
				{
					throw new ArgumentNullException("method");
				}
				if(!type.IsSubclassOf(typeof(Delegate)))
				{
					throw new ArgumentException("Arg_NotDelegate");
				}

				// Look up the method.
				methodInfo = target.GetMethod
								(method, BindingFlags.Public |
										 BindingFlags.NonPublic |
										 BindingFlags.Static);
				if(methodInfo == null || !(methodInfo is ClrMethod))
				{
					throw new ArgumentException(_("Arg_DelegateMethod"));
				}

				// Create a blank delegate to hold the method.
				Delegate d = CreateBlankDelegate(type, methodInfo as ClrMethod);
				if(d == null)
				{
					throw new ArgumentException(_("Arg_DelegateSignature"));
				}

				// Fill in the blanks and return the delegate.
				d.target = null;
				d.method = methodInfo;
				return d;
			}

	// Create a delegate for a static method.
	public static Delegate CreateDelegate(Type type, MethodInfo method)
			{
				// Validate the parameters.
				if(type == null)
				{
					throw new ArgumentNullException("type");
				}
				if(method == null)
				{
					throw new ArgumentNullException("method");
				}
				if(!type.IsSubclassOf(typeof(Delegate)))
				{
					throw new ArgumentException("Arg_NotDelegate");
				}

				// The method must be static.
				if((method.Attributes & MethodAttributes.Static) == 0)
				{
					throw new ArgumentException(_("Arg_DelegateMethod"));
				}

				// Create a blank delegate to hold the method.
				Delegate d = CreateBlankDelegate(type, method as ClrMethod);
				if(d == null)
				{
					throw new ArgumentException(_("Arg_DelegateSignature"));
				}

				// Fill in the blanks and return the delegate.
				d.target = null;
				d.method = method;
				return d;
			}

	// Invoke this delegate dynamically.
	public Object DynamicInvoke(Object[] args)
			{
				return DynamicInvokeImpl(args);
			}

	// Determine if two delegates are equal.
	public override bool Equals(Object obj)
			{
				Delegate d = (obj as Delegate);
				if(d != null && GetType() == d.GetType())
				{
					return (target == d.target && method == d.method);
				}
				else
				{
					return false;
				}
			}

	// Get a hash code for a delegate.
	public override int GetHashCode()
			{
				return method.GetHashCode();
			}

	// Get the invocation list of the current delegate.
	public virtual Delegate[] GetInvocationList()
			{
				Delegate[] list = new Delegate [1];
				list[0] = this;
				return list;
			}

	// Remove a delegate from a source list.
	public static Delegate Remove(Delegate source, Delegate value)
			{
				if(source == null || source == value)
				{
					return null;
				}
				else if(value == null)
				{
					return source;
				}
				else
				{
					return source.RemoveImpl(value);
				}
			}

	// Remove an all instances of an invocation list from a source list.
	public static Delegate RemoveAll(Delegate source, Delegate value)
			{
				if(source == null || source == value)
				{
					return null;
				}
				else if(value == null)
				{
					return source;
				}
				else
				{
					return source.RemoveAllImpl(value);
				}
			}

	// Operators.
	public static bool operator==(Delegate d1, Delegate d2)
			{
				if(d1 == null)
				{
					return (d2 == null);
				}
				else
				{
					return d1.Equals(d2);
				}
			}
	public static bool operator!=(Delegate d1, Delegate d2)
			{
				if(d1 == null)
				{
					return (d2 != null);
				}
				else
				{
					return !(d1.Equals(d2));
				}
			}

	// Get the method within this delegate.
	public MethodInfo Method
			{
				get
				{
					ClrMethod clr = (method as ClrMethod);
					if(clr != null && ClrHelpers.CanAccess(clr.ClrHandle))
					{
						return method;
					}
					else
					{
						throw new MemberAccessException
							(_("Arg_DelegateAccess"));
					}
				}
			}

	// Get the target of this delegate.
	public Object Target
			{
				get
				{
					return target;
				}
			}

	// Implementation of delegate combination.  The real work
	// is done in the MulticastDelegate class.
	protected virtual Delegate CombineImpl(Delegate d)
			{
				throw new NotSupportedException(_("NotSupp_Multicast"));
			}

	// Implementation of dynamic invocation.  This handles
	// the unicast case.  MulticastDelegate handles multicast.
	protected virtual Object DynamicInvokeImpl(Object[] args)
			{
				return method.Invoke(target, args);
			}

	// Implementation of delegate removal.  The real work
	// is done in the MulticastDelegate class.  This handles
	// the unicast delegate case only.
	protected virtual Delegate RemoveImpl(Delegate d)
			{
				if(target == d.target && method.Equals(d.method))
				{
					return null;
				}
				else
				{
					return this;
				}
			}

	// Implementation of delegate "all" removal.  The real work
	// is done in the MulticastDelegate class.  This handles
	// the unicast delegate case only.
	protected virtual Delegate RemoveAllImpl(Delegate d)
			{
				return RemoveImpl(d);
			}

	// Get the method implementation for this delegate.
	protected virtual MethodInfo GetMethodImpl()
			{
				return Method;
			}

}; // class Delegate

}; // namespace System
