/*
 * BinaryValueReader.cs - Implementation of the
 *	"System.Runtime.Serialization.Formatters.Binary.BinaryValueReader" class.
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

namespace System.Runtime.Serialization.Formatters.Binary
{

#if CONFIG_SERIALIZATION

using System.IO;
using System.Reflection;
using System.Collections;

internal abstract class BinaryValueReader
{
	// Builtin writers.
	private static BinaryValueReader booleanReader = new BooleanReader();
	private static BinaryValueReader byteReader = new ByteReader();
	private static BinaryValueReader sbyteReader = new SByteReader();
	private static BinaryValueReader charReader = new CharReader();
	private static BinaryValueReader int16Reader = new Int16Reader();
	private static BinaryValueReader uint16Reader = new UInt16Reader();
	private static BinaryValueReader int32Reader = new Int32Reader();
	private static BinaryValueReader uint32Reader = new UInt32Reader();
	private static BinaryValueReader int64Reader = new Int64Reader();
	private static BinaryValueReader uint64Reader = new UInt64Reader();
	private static BinaryValueReader singleReader = new SingleReader();
	private static BinaryValueReader doubleReader = new DoubleReader();
	private static BinaryValueReader decimalReader = new DecimalReader();
	private static BinaryValueReader dateTimeReader = new DateTimeReader();
	private static BinaryValueReader timeSpanReader = new TimeSpanReader();
	private static BinaryValueReader stringReader = new StringReader();
	private static BinaryValueReader objectReader = new ObjectReader();
	private static BinaryValueReader infoReader = new SurrogateReader(null);
#if false
	// TODO
	private static BinaryValueReader arrayOfObjectReader
			= new ArrayReader(BinaryPrimitiveTypeCode.ArrayOfObject);
	private static BinaryValueReader arrayOfStringReader
			= new ArrayReader(BinaryPrimitiveTypeCode.ArrayOfString);
#endif

	// Context information for reading binary values.
	public class BinaryValueContext
	{
		public BinaryFormatter formatter;
		public BinaryReader reader;
		public ObjectManager mgr;

		// Constructor.
		public BinaryValueContext(BinaryFormatter formatter,
								  BinaryReader reader)
				{
					this.formatter = formatter;
					this.reader = reader;
					this.mgr = new ObjectManager(formatter.SurrogateSelector,
												 formatter.Context);
				}

	}; // class BinaryValueContext

	// Constructor.
	protected BinaryValueReader() {}

	// Read the inline form of values for a type.
	public abstract Object ReadInline(BinaryValueContext context,
									  Type type, Type fieldType);

	// Read the object form of values for a type.
	public abstract Object ReadObject(BinaryValueContext context, Type type);

	// Normalize field names for cross-CLR differences.
	public virtual String NormalizeFieldName(String name)
			{
				return name;
			}

	// Determine if a type is visible outside its defining assembly.
	private static bool IsVisibleOutside(Type type)
			{
				switch(type.Attributes & TypeAttributes.VisibilityMask)
				{
					case TypeAttributes.Public:
						return true;

					case TypeAttributes.NestedPublic:
					case TypeAttributes.NestedFamily:
					case TypeAttributes.NestedFamORAssem:
						return IsVisibleOutside(type.DeclaringType);

					default:	return false;
				}
			}

	// Map a runtime type name to the actual type.  Returns null if
	// the type name does not correspond to a public serializable type.
	public static Type GetRuntimeType(String name)
			{
				// Validate the type name to make sure that it doesn't
				// contain any characters that would be illegal in the
				// name of a runtime type.
				if(name == null || name == String.Empty)
				{
					return null;
				}
				foreach(char ch in name)
				{
					if(ch >= 'A' && ch <= 'Z')
					{
						continue;
					}
					if(ch >= 'a' && ch <= 'z')
					{
						continue;
					}
					if(ch >= '0' && ch <= '9')
					{
						continue;
					}
					if(ch == '_' || ch == '.' || ch == '+')
					{
						continue;
					}
					return null;
				}

				// Look up the type within the runtime library.
				Type type = Assembly.GetExecutingAssembly().GetType(name);
				if(type == null)
				{
					return null;
				}

				// Make sure that the type is public and serializable.
				if(!IsVisibleOutside(type))
				{
					return null;
				}
				if(type.IsSerializable)
				{
					return type;
				}
				if(typeof(ISerializable).IsAssignableFrom(type))
				{
					return type;
				}
				return null;
			}

	// Get the value reader for a particular type.
	public static BinaryValueReader GetReader
				(BinaryValueContext context, Type type)
			{
				BinaryPrimitiveTypeCode code;

				// Handle the primitive types first.
				code = BinaryValueWriter.GetPrimitiveTypeCode(type);
				switch(code)
				{
					case BinaryPrimitiveTypeCode.Boolean:
						return booleanReader;
					case BinaryPrimitiveTypeCode.Byte:
						return byteReader;
					case BinaryPrimitiveTypeCode.Char:
						return charReader;
					case BinaryPrimitiveTypeCode.Decimal:
						return decimalReader;
					case BinaryPrimitiveTypeCode.Double:
						return doubleReader;
					case BinaryPrimitiveTypeCode.Int16:
						return int16Reader;
					case BinaryPrimitiveTypeCode.Int32:
						return int32Reader;
					case BinaryPrimitiveTypeCode.Int64:
						return int64Reader;
					case BinaryPrimitiveTypeCode.SByte:
						return sbyteReader;
					case BinaryPrimitiveTypeCode.Single:
						return singleReader;
					case BinaryPrimitiveTypeCode.TimeSpan:
						return timeSpanReader;
					case BinaryPrimitiveTypeCode.DateTime:
						return dateTimeReader;
					case BinaryPrimitiveTypeCode.UInt16:
						return uint16Reader;
					case BinaryPrimitiveTypeCode.UInt32:
						return uint32Reader;
					case BinaryPrimitiveTypeCode.UInt64:
						return uint64Reader;
					case BinaryPrimitiveTypeCode.String:
						return stringReader;
				}

				// Handle special types that we recognize.
				if(type == typeof(Object))
				{
					return objectReader;
				}
#if false
// TODO
				if(type == typeof(Object[]))
				{
					return arrayOfObjectReader;
				}
				if(type == typeof(String[]))
				{
					return arrayOfStringReader;
				}
				if(type.IsArray && type.GetArrayRank() == 1)
				{
					code = GetPrimitiveTypeCode(type.GetElementType());
					if(code != (BinaryPrimitiveTypeCode)0)
					{
						return new ArrayReader
							(BinaryTypeTag.ArrayOfPrimitiveType, code);
					}
				}
#endif

				// Check for surrogates.
				ISurrogateSelector selector;
				ISerializationSurrogate surrogate;
				selector = context.formatter.SurrogateSelector;
				if(selector != null)
				{
					surrogate = selector.GetSurrogate
						(type, context.formatter.Context, out selector);
					if(surrogate != null)
					{
						return new SurrogateReader(surrogate);
					}
				}

				// Check for types that implement ISerializable.
				if(typeof(ISerializable).IsAssignableFrom(type))
				{
					return infoReader;
				}

				// Bail out if the type is not marked with the
				// "serializable" flag.
				if(!type.IsSerializable)
				{
					throw new SerializationException
						(String.Format
							(_("Serialize_CannotSerialize"), type));
				}

				// Everything else is handled as an object.
				return objectReader;
			}

	// Read object values.
	private class ObjectReader : BinaryValueReader
	{
		// Constructor.
		public ObjectReader() : base() {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return null;
				#if false
					BinaryPrimitiveTypeCode code;
					BinaryValueReader vw;
					bool firstTime;
					long objectID;
					long typeID;

					if(value == null)
					{
						// Write a null value.
						context.writer.Write
							((byte)(BinaryElementType.NullValue));
						return;
					}
					else if(type.IsValueType)
					{
						if(fieldType.IsValueType)
						{
							// Expand the value instance inline.
							vw = GetReader(context, type);
							typeID = context.gen.GetIDForType(type);
							objectID = context.gen.GetId(value, out firstTime);
							if(typeID == -1)
							{
								context.gen.RegisterType(type, objectID);
							}
							vw.WriteObjectHeader(context, value, type,
												 objectID, typeID);
							vw.WriteObject(context, value, type);
							return;
						}
						else if((code = GetPrimitiveTypeCode(type)) != 0)
						{
							// This is a boxed primitive value.
							context.writer.Write
								((byte)(BinaryElementType.
											BoxedPrimitiveTypeValue));
							vw = GetReader(context, type);
							vw.WriteTypeSpec(context, type);
							vw.WriteInline(context, value, type, type);
							return;
						}
					}

					// Queue the object to be expanded later.
					objectID = context.gen.GetId(value, out firstTime);
					context.writer.Write
						((byte)(BinaryElementType.ObjectReference));
					context.writer.Write((int)objectID);
					if(firstTime)
					{
						context.queue.Enqueue(value);
					}
				#endif
				}

		// Read the object form of values for a type.
		public override Object ReadObject(BinaryValueContext context,
									      Type type)
				{
					return null;
				#if false
					MemberInfo[] members =
						FormatterServices.GetSerializableMembers
							(type, context.formatter.Context);
					Object[] values =
						FormatterServices.GetObjectData(value, members);
					int index;
					Type fieldType;
					Type valueType;
					for(index = 0; index < members.Length; ++index)
					{
						if(members[index] is FieldInfo)
						{
							fieldType = ((FieldInfo)(members[index]))
											.FieldType;
						}
						else
						{
							fieldType = ((PropertyInfo)(members[index]))
											.PropertyType;
						}
						if(values[index] != null)
						{
							valueType = values[index].GetType();
						}
						else
						{
							valueType = fieldType;
						}
						GetReader(context, fieldType).WriteInline
							(context, values[index], valueType, fieldType);
					}
				#endif
				}

	}; // class ObjectReader

	// Read value type values.
	private class ValueTypeReader : ObjectReader
	{
		// Constructor.
		public ValueTypeReader() : base() {}

	}; // class ValueTypeReader

	// Read object values using serialization surrogates.
	private class SurrogateReader : ObjectReader
	{
		// Internal state.
		private ISerializationSurrogate surrogate;

		// Constructor.
		public SurrogateReader(ISerializationSurrogate surrogate)
				{
					this.surrogate = surrogate;
				}

		// Read the object form of values for a type.
		public override Object ReadObject(BinaryValueContext context,
										  Type type)
				{
					return null;
				#if false
					SerializationInfo info = GetObjectData
						(context, value, type);
					SerializationInfoEnumerator e = info.GetEnumerator();
					Type objectType;
					Type valueType;
					Object fieldValue;
					while(e.MoveNext())
					{
						objectType = e.ObjectType;
						fieldValue = e.Value;
						if(value == null)
						{
							valueType = objectType;
						}
						else
						{
							valueType = fieldValue.GetType();
						}
						GetReader(context, objectType).WriteInline
							(context, fieldValue, valueType, objectType);
					}
				#endif
				}

	}; // class SurrogateReader

	// Write primitive values.
	private abstract class PrimitiveReader : BinaryValueReader
	{
		// Internal state.
		private BinaryPrimitiveTypeCode code;

		// Constructor.
		public PrimitiveReader(BinaryPrimitiveTypeCode code)
				{
					this.code = code;
				}

		// Read the object form of values for a type.
		public override Object ReadObject(BinaryValueContext context,
										  Type type)
				{
					// The object field is just the primitive value itself.
					return ReadInline(context, type, type);
				}

		// Normalize field names for cross-CLR differences.
		public override String NormalizeFieldName(String name)
				{
					return "value_";
				}

	}; // class PrimitiveReader

	// Read boolean values.
	private class BooleanReader : PrimitiveReader
	{
		// Construtor.
		public BooleanReader() : base(BinaryPrimitiveTypeCode.Boolean) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadBoolean();
				}

	}; // class BooleanReader

	// Read byte values.
	private class ByteReader : PrimitiveReader
	{
		// Construtor.
		public ByteReader() : base(BinaryPrimitiveTypeCode.Byte) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadByte();
				}

	}; // class ByteReader

	// Read sbyte values.
	private class SByteReader : PrimitiveReader
	{
		// Construtor.
		public SByteReader() : base(BinaryPrimitiveTypeCode.SByte) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadSByte();
				}

	}; // class SByteReader

	// Read char values.
	private class CharReader : PrimitiveReader
	{
		// Constructor.
		public CharReader() : base(BinaryPrimitiveTypeCode.Char) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return (char)(context.reader.ReadUInt16());
				}

	}; // class CharReader

	// Read short values.
	private class Int16Reader : PrimitiveReader
	{
		// Constructor.
		public Int16Reader() : base(BinaryPrimitiveTypeCode.Int16) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return (char)(context.reader.ReadInt16());
				}

	}; // class Int16Reader

	// Read ushort values.
	private class UInt16Reader : PrimitiveReader
	{
		// Constructor.
		public UInt16Reader() : base(BinaryPrimitiveTypeCode.UInt16) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadUInt16();
				}

	}; // class UInt16Reader

	// Read int values.
	private class Int32Reader : PrimitiveReader
	{
		// Constructor.
		public Int32Reader() : base(BinaryPrimitiveTypeCode.Int32) {}

		// Write the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadInt32();
				}

	}; // class Int32Reader

	// Read uint values.
	private class UInt32Reader : PrimitiveReader
	{
		// Constructor.
		public UInt32Reader() : base(BinaryPrimitiveTypeCode.UInt32) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadUInt32();
				}

	}; // class UInt32Reader

	// Read long values.
	private class Int64Reader : PrimitiveReader
	{
		// Constructor.
		public Int64Reader() : base(BinaryPrimitiveTypeCode.Int64) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadInt64();
				}

	}; // class Int64Reader

	// Read ulong values.
	private class UInt64Reader : PrimitiveReader
	{
		// Constructor.
		public UInt64Reader() : base(BinaryPrimitiveTypeCode.UInt64) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadUInt64();
				}

	}; // class UInt64Reader

	// Read float values.
	private class SingleReader : PrimitiveReader
	{
		// Constructor.
		public SingleReader() : base(BinaryPrimitiveTypeCode.Single) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadSingle();
				}

	}; // class SingleReader

	// Read double values.
	private class DoubleReader : PrimitiveReader
	{
		// Constructor.
		public DoubleReader() : base(BinaryPrimitiveTypeCode.Double) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadDouble();
				}

	}; // class DoubleReader

	// Read Decimal values.
	private class DecimalReader : ValueTypeReader
	{
		// Constructor.
		public DecimalReader() {}

		// Normalize field names for cross-CLR differences.
		public override String NormalizeFieldName(String name)
				{
					switch(name)
					{
						// Microsoft CLR.
						case "hi":		return "high";
						case "mid":		return "middle";
						case "lo":		return "low";

						// Mono CLR.
						case "ss32":	return "flags";
						case "hi32":	return "high";
						case "mid32":	return "middle";
						case "lo32":	return "low";
					}
					return name;
				}

	}; // class DecimalReader

	// Read DateTime values.
	private class DateTimeReader : PrimitiveReader
	{
		// Constructor.
		public DateTimeReader() : base(BinaryPrimitiveTypeCode.DateTime) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return new DateTime(context.reader.ReadInt64());
				}

	}; // class DateTimeReader

	// Read TimeSpan values.
	private class TimeSpanReader : PrimitiveReader
	{
		// Constructor.
		public TimeSpanReader() : base(BinaryPrimitiveTypeCode.TimeSpan) {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return new TimeSpan(context.reader.ReadInt64());
				}

	}; // class TimeSpanReader

	// Read String values.
	private class StringReader : BinaryValueReader
	{
		// Constructor.
		public StringReader() : base() {}

		// Read the inline form of values for a type.
		public override Object ReadInline(BinaryValueContext context,
										  Type type, Type fieldType)
				{
					return context.reader.ReadString();
				}

		// Write the object form of values for a type.
		public override Object ReadObject(BinaryValueContext context,
										  Type type)
				{
					return context.reader.ReadString();
				}

	}; // class StringReader

}; // class BinaryValueReader

#endif // CONFIG_SERIALIZATION

}; // namespace System.Runtime.Serialization.Formatters.Binary
