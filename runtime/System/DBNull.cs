/*
 * DBNull.cs - Implementation of the "System.DBNull" class.
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

using System.Runtime.Serialization;

public sealed class DBNull : ISerializable, IConvertible
{

	// The only DBNull object in the system.
	public readonly static DBNull Value = new DBNull();

	// Constructors.
	private DBNull() {}

	// Override inherited methods.
	public override String ToString() { return String.Empty; }

	// Serialization support.
	public void GetObjectData(SerializationInfo info, StreamingContext context)
			{
				// TODO.
			}

	// Implement the IConvertible interface.
	public TypeCode GetTypeCode() { return TypeCode.DBNull; }
	public Object ToType(Type ct) { return Convert.DefaultToType(this, ct); }
	public Boolean ToBoolean()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public Byte ToByte()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public SByte ToSByte()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public Int16 ToInt16()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public UInt16 ToUInt16()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public Int32 ToInt32() 
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public UInt32 ToUInt32()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public Int64 ToInt64()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public UInt64 ToUInt64()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public Char ToChar()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public Single ToSingle()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public Double ToDouble()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public Decimal ToDecimal()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}
	public DateTime ToDateTime()
			{
				throw new InvalidCastException
					(Environment.GetResourceString("InvalidCast_DBNull"));
			}

}; // class DBNull

}; // namespace System
