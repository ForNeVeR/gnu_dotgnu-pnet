/*
 * Delegate.cs - Implementation of the "System.Delegate" class.
 *
 * Copyright (C) 2001, 2002, 2003  Southern Storm Software, Pty Ltd.
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
using System.Runtime.Serialization;

public abstract class Delegate : ICloneable
#if !ECMA_COMPAT
	, ISerializable
#endif
{
	// Internal state.  These three fields must be present here
	// and in this order so that the runtime engine can create
	// delegates from function pointers correctly.
	internal Object     			target;
	internal RuntimeMethodHandle	method;
	internal IntPtr					closure;

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
				MethodInfo methodInfo =
					(target.GetType()).GetMethod(method,
												 BindingFlags.Public |
												 BindingFlags.NonPublic |
												 BindingFlags.Instance);
				if(methodInfo == null)
				{
					throw new MissingMethodException
						(_("Arg_DelegateMethod"));
				}
				this.method = methodInfo.MethodHandle;
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
				MethodInfo methodInfo =
						target.GetMethod(method,
									     BindingFlags.Public |
									     BindingFlags.NonPublic |
									     BindingFlags.Static);
				if(methodInfo == null)
				{
					throw new MissingMethodException
						(_("Arg_DelegateMethod"));
				}
				this.method = methodInfo.MethodHandle;
			}

	// Implement the ICloneable interface.
	public virtual Object Clone()
			{
				return MemberwiseClone();
			}

	// Combine the invocation lists of two delegates.
	public static Delegate Combine(Delegate a, Delegate b)
			{
				if(((Object)a) == null)
				{
					return b;
				}
				else if(((Object)b) == null)
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
	
	private static Type[] GetDelegateParams(Type type)
	{
		ParameterInfo [] args=type.GetMethod("Invoke").GetParameters();
		Type [] argTypes=new Type[args.Length];
		for(int i=args.Length-1;i>=0;i--)
		{
			argTypes[i] = args[i].ParameterType;
		}
		return argTypes;
	}

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
										 BindingFlags.Instance,
										 null, CallingConventions.Any,
								     	 GetDelegateParams(type),null);
				if(methodInfo == null)
				{
					throw new ArgumentException(_("Arg_DelegateMethod"));
				}

				// Create a blank delegate to hold the method.
				Delegate d = CreateBlankDelegate(type, methodInfo as ClrMethod);
				if(((Object)d) == null)
				{
					throw new ArgumentException(_("Arg_DelegateSignature"));
				}

				// Fill in the blanks and return the delegate.
				d.target = target;
				d.method = methodInfo.MethodHandle;
				d.closure = IntPtr.Zero;
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
										 BindingFlags.Static,
										 null, CallingConventions.Any,
								     	 GetDelegateParams(type),null);
				if(methodInfo == null || !(methodInfo is ClrMethod))
				{
					throw new ArgumentException(_("Arg_DelegateMethod"));
				}

				// Create a blank delegate to hold the method.
				Delegate d = CreateBlankDelegate(type, methodInfo as ClrMethod);
				if(((Object)d) == null)
				{
					throw new ArgumentException(_("Arg_DelegateSignature"));
				}

				// Fill in the blanks and return the delegate.
				d.target = null;
				d.method = methodInfo.MethodHandle;
				d.closure = IntPtr.Zero;
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
				if(((Object)d) == null)
				{
					throw new ArgumentException(_("Arg_DelegateSignature"));
				}

				// Fill in the blanks and return the delegate.
				d.target = null;
				d.method = method.MethodHandle;
				d.closure = IntPtr.Zero;
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
				if(((Object)d) != null && GetType() == d.GetType())
				{
					return (target == d.target &&
							method.Value == d.method.Value);
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
				if(((Object)source) == null ||
				   ((Object)source) == ((Object)value))
				{
					return null;
				}
				else if(((Object)value) == null)
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
				if(((Object)source) == null ||
				   ((Object)source) == ((Object)value))
				{
					return null;
				}
				else if(((Object)value) == null)
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
				if(((Object)d1) == null)
				{
					return (((Object)d2) == null);
				}
				else
				{
					return d1.Equals(d2);
				}
			}
	public static bool operator!=(Delegate d1, Delegate d2)
			{
				if(((Object)d1) == null)
				{
					return (((Object)d2) != null);
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
					ClrMethod methodInfo =
						(MethodBase.GetMethodFromHandle(method) as ClrMethod);
					if(methodInfo != null &&
					   ClrHelpers.CanAccess(methodInfo.ClrHandle))
					{
						return methodInfo;
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
				return Method.Invoke(target, args);
			}

	// Implementation of delegate removal.  The real work
	// is done in the MulticastDelegate class.  This handles
	// the unicast delegate case only.
	protected virtual Delegate RemoveImpl(Delegate d)
			{
				if(target == d.target && method.Value == d.method.Value)
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

#if !ECMA_COMPAT
	// Get the serialization data for this object.
	public virtual void GetObjectData(SerializationInfo info,
									  StreamingContext context)
			{
				// TODO
			}
#endif

}; // class Delegate

}; // namespace System
