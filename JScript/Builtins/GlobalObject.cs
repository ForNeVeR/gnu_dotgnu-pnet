/*
 * GlobalObject.cs - Implementation of the global JScript object.
 *
 * Copyright (C) 2003 Southern Storm Software, Pty Ltd.
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
 
namespace Microsoft.JScript
{

using System;
using System.Reflection;
using Microsoft.JScript.Vsa;

public class GlobalObject
{
	// Actual storage for the global object.
	internal VsaEngine engine;
	internal JSObject globalObject;

	// Constructor.
	internal GlobalObject(VsaEngine engine)
			{
				// Record the engine for later.
				this.engine = engine;

				// Create the actual storage value, with no prototype.
				globalObject = new JSObject(null, engine);

				// Get the instance object for the engine.
				EngineInstance inst = EngineInstance.GetEngineInstance(engine);

				// Add all of the properties to the global object.
				AddBuiltin(inst, "CollectGarbage");
				AddBuiltin(inst, "decodeURI");
				AddBuiltin(inst, "decodeURIComponent");
				AddBuiltin(inst, "encodeURI");
				AddBuiltin(inst, "encodeURIComponent");
				AddBuiltin(inst, "escape");
				AddBuiltin(inst, "eval");
				AddBuiltin(inst, "isFinite");
				AddBuiltin(inst, "isNaN");
				AddBuiltin(inst, "parseFloat");
				AddBuiltin(inst, "parseInt");
				AddBuiltin(inst, "ScriptEngine");
				AddBuiltin(inst, "ScriptEngineBuildVersion");
				AddBuiltin(inst, "ScriptEngineMajorVersion");
				AddBuiltin(inst, "ScriptEngineMinorVersion");
				AddBuiltin(inst, "unescape");
				globalObject.Put("Infinity", Double.PositiveInfinity,
								 PropertyAttributes.DontEnum |
								 PropertyAttributes.DontDelete);
				globalObject.Put("NaN", Double.NaN,
								 PropertyAttributes.DontEnum |
								 PropertyAttributes.DontDelete);
				globalObject.Put("undefined", null,
								 PropertyAttributes.DontEnum |
								 PropertyAttributes.DontDelete);
#if false
				AddProperty("ActiveXObject", ActiveXObject);
#endif
				AddProperty("Array", Array);
#if false
				AddProperty("Boolean", Boolean);
				AddProperty("Date", Date);
				AddProperty("Enumerator", Enumerator);
				AddProperty("Error", Error);
				AddProperty("EvalError", EvalError);
#endif
				AddProperty("Function", Function);
#if false
				AddProperty("Math", Math);
				AddProperty("Number", Number);
#endif
				AddProperty("Object", Object);
#if false
				AddProperty("RangeError", RangeError);
				AddProperty("ReferenceError", ReferenceError);
				AddProperty("RegExp", RegExp);
				AddProperty("String", String);
				AddProperty("SyntaxError", SyntaxError);
				AddProperty("TypeError", TypeError);
				AddProperty("URIError", URIError);
				AddProperty("VBArray", VBArray);
#endif
			}

	// Add a builtin function to the global object.
	private void AddBuiltin(EngineInstance inst, string name)
			{
				MethodInfo method = typeof(GlobalObject).GetMethod(name);
				globalObject.Put(name, new BuiltinFunction
					(inst.GetFunctionPrototype(), name, method),
					PropertyAttributes.None);
			}

	// Add a property to the global object.
	private void AddProperty(string name, object value)
			{
				globalObject.Put(name, value, PropertyAttributes.None);
			}

#if false
	// Return various type constructor objects.  Do not use these,
	// as they aren't reentrant-safe.
	public static ActiveXObjectConstructor ActiveXObject
			{
				get
				{
					return ActiveXObjectConstructor.constructor;
				}
			}
#endif
	public static ArrayConstructor Array
			{
				get
				{
					return EngineInstance.Default.GetArrayConstructor();
				}
			}
#if false
	public static BooleanConstructor Boolean
			{
				get
				{
					return BooleanConstructor.constructor;
				}
			}
	public static DateConstructor Date
			{
				get
				{
					return DateConstructor.constructor;
				}
			}
	public static EnumeratorConstructor Enumerator
			{
				get
				{
					return EnumeratorConstructor.constructor;
				}
			}
	public static ErrorConstructor Error
			{
				get
				{
					return ErrorConstructor.constructor;
				}
			}
	public static ErrorConstructor EvalError
			{
				get
				{
					return ErrorConstructor.evalError;
				}
			}
#endif
	public static FunctionConstructor Function
			{
				get
				{
					return EngineInstance.Default.GetFunctionConstructor();
				}
			}
#if false
	public static MathObject Math
			{
				get
				{
					// TODO
					return null;
				}
			}
	public static NumberConstructor Number
			{
				get
				{
					return NumberConstructor.constructor;
				}
			}
#endif
	public static ObjectConstructor Object
			{
				get
				{
					return EngineInstance.Default.GetObjectConstructor();
				}
			}
#if false
	public static ErrorConstructor RangeError
			{
				get
				{
					return ErrorConstructor.rangeError;
				}
			}
	public static ErrorConstructor ReferenceError
			{
				get
				{
					return ErrorConstructor.referenceError;
				}
			}
	public static RegExpConstructor RegExp
			{
				get
				{
					return RegExpConstructor.constructor;
				}
			}
	public static StringConstructor String
			{
				get
				{
					return StringConstructor.constructor;
				}
			}
	public static ErrorConstructor SyntaxError
			{
				get
				{
					return ErrorConstructor.syntaxError;
				}
			}
	public static ErrorConstructor TypeError
			{
				get
				{
					return ErrorConstructor.typeError;
				}
			}
	public static ErrorConstructor URIError
			{
				get
				{
					return ErrorConstructor.uriError;
				}
			}
	public static VBArrayConstructor VBArray
			{
				get
				{
					return VBArrayConstructor.constructor;
				}
			}
#endif

	// Return various system types.
	public static Type boolean
			{
				get
				{
					return typeof(bool);
				}
			}
	public static Type @byte
			{
				get
				{
					return typeof(Byte);
				}
			}
	public static Type @sbyte
			{
				get
				{
					return typeof(SByte);
				}
			}
	public static Type @char
			{
				get
				{
					return typeof(Char);
				}
			}
	public static Type @short
			{
				get
				{
					return typeof(Int16);
				}
			}
	public static Type @ushort
			{
				get
				{
					return typeof(UInt16);
				}
			}
	public static Type @int
			{
				get
				{
					return typeof(Int32);
				}
			}
	public static Type @uint
			{
				get
				{
					return typeof(UInt32);
				}
			}
	public static Type @long
			{
				get
				{
					return typeof(Int64);
				}
			}
	public static Type @ulong
			{
				get
				{
					return typeof(UInt64);
				}
			}
	public static Type @float
			{
				get
				{
					return typeof(Single);
				}
			}
	public static Type @double
			{
				get
				{
					return typeof(Double);
				}
			}
	public static Type @decimal
			{
				get
				{
					return typeof(Decimal);
				}
			}
	public static Type @void
			{
				get
				{
					return typeof(Void);
				}
			}

	// Perform garbage collection.
	[JSFunction(0, JSBuiltin.Global_CollectGarbage)]
	public static void CollectGarbage()
			{
			#if !ECMA_COMPAT
				GC.Collect();
			#endif
			}

	// Decode a URI.
	[JSFunction(0, JSBuiltin.Global_decodeURI)]
	public static string decodeURI(object encodedURI)
			{
				// TODO
				return string.Empty;
			}

	// Decode a URI component.
	[JSFunction(0, JSBuiltin.Global_decodeURIComponent)]
	public static string decodeURIComponent(object encodedURI)
			{
				// TODO
				return string.Empty;
			}

	// Encode a URI.
	[JSFunction(0, JSBuiltin.Global_encodeURI)]
	public static string encodeURI(object uri)
			{
				// TODO
				return string.Empty;
			}

	// Encode a URI component.
	[JSFunction(0, JSBuiltin.Global_encodeURIComponent)]
	public static string encodeURIComponent(object uri)
			{
				// TODO
				return string.Empty;
			}

	// Escape a string.
	[JSFunction(0, JSBuiltin.Global_escape)]
	[NotRecommended("escape")]
	public static string escape(object str)
			{
				// TODO
				return Convert.ToString(str);
			}

	// Evaluate a string.
	[JSFunction(0, JSBuiltin.Global_escape)]
	public static object eval(object str)
			{
				// The parser recognizes "eval" as a special node type,
				// so this method should never be called.
				throw new JScriptException(JSError.IllegalEval);
			}

	// Determine if a number is finite.
	[JSFunction(0, JSBuiltin.Global_isFinite)]
	public static bool isFinite(double number)
			{
				return !(Double.IsNaN(number) || Double.IsInfinity(number));
			}

	// Determine if a number is NaN.
	[JSFunction(0, JSBuiltin.Global_isNaN)]
	public static bool isNaN(object num)
			{
				return Double.IsNaN(Convert.ToNumber(num));
			}

	// Parse a floating-point value.
	[JSFunction(0, JSBuiltin.Global_parseFloat)]
	public static double parseFloat(object str)
			{
				// TODO
				return 0.0;
			}

	// Parse an integer value.
	[JSFunction(0, JSBuiltin.Global_parseInt)]
	public static double parseInt(object str, object radix)
			{
				// TODO
				return 0.0;
			}

	// Get the name and version information for the script engine.
	//
	// Note: we return the version of the runtime that we are running
	// against, instead of the version that we were compiled against.
	// This is because compile versions make no sense in our environment.
	//
	// Because the JScript library will normally be installed at the
	// same time as the corresponding runtime, using the runtime version
	// will normally give what we expect anyway.
	//
	[JSFunction(0, JSBuiltin.Global_ScriptEngine)]
	public static string ScriptEngine()
			{
				return "JScript";
			}
	[JSFunction(0, JSBuiltin.Global_ScriptEngineBuildVersion)]
	public static int ScriptEngineBuildVersion()
			{
				return Environment.Version.Build;
			}
	[JSFunction(0, JSBuiltin.Global_ScriptEngineMajorVersion)]
	public static int ScriptEngineMajorVersion()
			{
				return Environment.Version.Major;
			}
	[JSFunction(0, JSBuiltin.Global_ScriptEngineMinorVersion)]
	public static int ScriptEngineMinorVersion()
			{
				return Environment.Version.Minor;
			}

	// Unescape a string.
	[JSFunction(0, JSBuiltin.Global_unescape)]
	[NotRecommended("unescape")]
	public static string unescape(object str)
			{
				// TODO
				return Convert.ToString(str);
			}

}; // class GlobalObject

// "Lenient" version of the above class which exports all of the
// object's properties to the user level.
public sealed class LenientGlobalObject : GlobalObject
{
	// Accessible properties.
	public new object boolean;
	public new object @byte;
	public new object @sbyte;
	public new object @char;
	public new object @short;
	public new object @ushort;
	public new object @int;
	public new object @uint;
	public new object @long;
	public new object @ulong;
	public new object @float;
	public new object @double;
	public new object @decimal;
	public new object @void;
	public new object CollectGarbage;
	public new object decodeURI;
	public new object decodeURIComponent;
	public new object encodeURI;
	public new object encodeURIComponent;
	[NotRecommended("escape")] public new object escape;
	public new object eval;
	public new object isFinite;
	public new object isNaN;
	public new object parseFloat;
	public new object parseInt;
	public new object ScriptEngine;
	public new object ScriptEngineBuildVersion;
	public new object ScriptEngineMajorVersion;
	public new object ScriptEngineMinorVersion;
	[NotRecommended("unescape")] public new object unescape;
	public object Infinity;
	public object NaN;
	public object undefined;

	// Constructor.
	internal LenientGlobalObject(VsaEngine engine)
			: base(engine)
			{
				boolean = GlobalObject.boolean;
				@byte = GlobalObject.@byte;
				@sbyte = GlobalObject.@sbyte;
				@char = GlobalObject.@char;
				@short = GlobalObject.@short;
				@ushort = GlobalObject.@ushort;
				@int = GlobalObject.@int;
				@uint = GlobalObject.@uint;
				@long = GlobalObject.@long;
				@ulong = GlobalObject.@ulong;
				@float = GlobalObject.@float;
				@double = GlobalObject.@double;
				@decimal = GlobalObject.@decimal;
				@void = GlobalObject.@void;
				CollectGarbage = globalObject.Get("CollectGarbage");
				decodeURI = globalObject.Get("decodeURI");
				decodeURIComponent =
					globalObject.Get("decodeURIComponent");
				encodeURI = globalObject.Get("encodeURI");
				encodeURIComponent =
					globalObject.Get("encodeURIComponent");
				escape = globalObject.Get("escape");
				eval = globalObject.Get("eval");
				isFinite = globalObject.Get("isFinite");
				isNaN = globalObject.Get("isNaN");
				parseFloat = globalObject.Get("parseFloat");
				parseInt = globalObject.Get("parseInt");
				ScriptEngine = globalObject.Get("ScriptEngine");
				ScriptEngineBuildVersion =
					globalObject.Get("ScriptEngineBuildVersion");
				ScriptEngineMajorVersion =
					globalObject.Get("ScriptEngineMajorVersion");
				ScriptEngineMinorVersion =
					globalObject.Get("ScriptEngineMinorVersion");
				unescape = globalObject.Get("unescape");
				Infinity = Double.PositiveInfinity;
				NaN = Double.NaN;
				undefined = null;
			}

	// Get or set the class constructors.
#if false
	private object activeXObject;
	public new object ActiveXObject
			{
				get
				{
					lock(this)
					{
						if(activeXObject == null ||
						   activeXObject is Missing)
						{
							activeXObject = GlobalObject.ActiveXObject;
						}
						return activeXObject;
					}
				}
				set
				{
					lock(this)
					{
						activeXObject = value;
					}
				}
			}
	private object array;
	public new object Array
			{
				get
				{
					lock(this)
					{
						if(array == null ||
						   array is Missing)
						{
							array = GlobalObject.Array;
						}
						return array;
					}
				}
				set
				{
					lock(this)
					{
						array = value;
					}
				}
			}
	private object booleanConstructor;
	public new object Boolean
			{
				get
				{
					lock(this)
					{
						if(booleanConstructor == null ||
						   booleanConstructor is Missing)
						{
							booleanConstructor = GlobalObject.Boolean;
						}
						return booleanConstructor;
					}
				}
				set
				{
					lock(this)
					{
						booleanConstructor = value;
					}
				}
			}
	private object date;
	public new object Date
			{
				get
				{
					lock(this)
					{
						if(date == null ||
						   date is Missing)
						{
							date = GlobalObject.Date;
						}
						return date;
					}
				}
				set
				{
					lock(this)
					{
						date = value;
					}
				}
			}
	private object enumerator;
	public new object Enumerator
			{
				get
				{
					lock(this)
					{
						if(enumerator == null ||
						   enumerator is Missing)
						{
							enumerator = GlobalObject.Enumerator;
						}
						return enumerator;
					}
				}
				set
				{
					lock(this)
					{
						enumerator = value;
					}
				}
			}
	private object error;
	public new object Error
			{
				get
				{
					lock(this)
					{
						if(error == null ||
						   error is Missing)
						{
							error = GlobalObject.Error;
						}
						return error;
					}
				}
				set
				{
					lock(this)
					{
						error = value;
					}
				}
			}
	private object evalError;
	public new object EvalError
			{
				get
				{
					lock(this)
					{
						if(evalError == null ||
						   evalError is Missing)
						{
							evalError = GlobalObject.EvalError;
						}
						return evalError;
					}
				}
				set
				{
					lock(this)
					{
						evalError = value;
					}
				}
			}
#endif
	private object function;
	public new object Function
			{
				get
				{
					lock(this)
					{
						if(function == null ||
						   function is Missing)
						{
							function = EngineInstance.GetEngineInstance
									(engine).GetFunctionConstructor();
						}
						return function;
					}
				}
				set
				{
					lock(this)
					{
						function = value;
					}
				}
			}
#if false
	private object math;
	public new object Math
			{
				get
				{
					lock(this)
					{
						if(math == null ||
						   math is Missing)
						{
							math = GlobalObject.Math;
						}
						return math;
					}
				}
				set
				{
					lock(this)
					{
						math = value;
					}
				}
			}
	private object number;
	public new object Number
			{
				get
				{
					lock(this)
					{
						if(number == null ||
						   number is Missing)
						{
							number = GlobalObject.Number;
						}
						return number;
					}
				}
				set
				{
					lock(this)
					{
						number = value;
					}
				}
			}
#endif
	private object objectConstructor;
	public new object Object
			{
				get
				{
					lock(this)
					{
						if(objectConstructor == null ||
						   objectConstructor is Missing)
						{
							objectConstructor =
								EngineInstance.GetEngineInstance
									(engine).GetObjectConstructor();
						}
						return objectConstructor;
					}
				}
				set
				{
					lock(this)
					{
						objectConstructor = value;
					}
				}
			}
#if false
	private object rangeError;
	public new object RangeError
			{
				get
				{
					lock(this)
					{
						if(rangeError == null ||
						   rangeError is Missing)
						{
							rangeError = GlobalObject.RangeError;
						}
						return rangeError;
					}
				}
				set
				{
					lock(this)
					{
						rangeError = value;
					}
				}
			}
	private object referenceError;
	public new object ReferenceError
			{
				get
				{
					lock(this)
					{
						if(referenceError == null ||
						   referenceError is Missing)
						{
							referenceError = GlobalObject.ReferenceError;
						}
						return referenceError;
					}
				}
				set
				{
					lock(this)
					{
						referenceError = value;
					}
				}
			}
	private object regexp;
	public new object RegExp
			{
				get
				{
					lock(this)
					{
						if(regexp == null ||
						   regexp is Missing)
						{
							regexp = GlobalObject.RegExp;
						}
						return regexp;
					}
				}
				set
				{
					lock(this)
					{
						regexp = value;
					}
				}
			}
	private object stringConstructor;
	public new object String
			{
				get
				{
					lock(this)
					{
						if(stringConstructor == null ||
						   stringConstructor is Missing)
						{
							stringConstructor = GlobalObject.String;
						}
						return stringConstructor;
					}
				}
				set
				{
					lock(this)
					{
						stringConstructor = value;
					}
				}
			}
	private object syntaxError;
	public new object SyntaxError
			{
				get
				{
					lock(this)
					{
						if(syntaxError == null ||
						   syntaxError is Missing)
						{
							syntaxError = GlobalObject.SyntaxError;
						}
						return syntaxError;
					}
				}
				set
				{
					lock(this)
					{
						syntaxError = value;
					}
				}
			}
	private object typeError;
	public new object TypeError
			{
				get
				{
					lock(this)
					{
						if(typeError == null ||
						   typeError is Missing)
						{
							typeError = GlobalObject.TypeError;
						}
						return typeError;
					}
				}
				set
				{
					lock(this)
					{
						syntaxError = value;
					}
				}
			}
	private object uriError;
	public new object URIError
			{
				get
				{
					lock(this)
					{
						if(uriError == null ||
						   uriError is Missing)
						{
							uriError = GlobalObject.URIError;
						}
						return uriError;
					}
				}
				set
				{
					lock(this)
					{
						uriError = value;
					}
				}
			}
	private object vbArray;
	public new object VBArray
			{
				get
				{
					lock(this)
					{
						if(vbArray == null ||
						   vbArray is Missing)
						{
							vbArray = GlobalObject.VBArray;
						}
						return vbArray;
					}
				}
				set
				{
					lock(this)
					{
						vbArray = value;
					}
				}
			}
#endif

}; // class LenientGlobalObject

}; // namespace Microsoft.JScript
