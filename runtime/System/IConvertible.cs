/*
 * IConvertible.cs - Implementation of the "System.IConvertible" interface.
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

public interface IConvertible
{

	TypeCode GetTypeCode();
	Object ToType(Type conversionType);
	String ToString();
	Boolean ToBoolean();
	Byte ToByte();
	SByte ToSByte();
	Int16 ToInt16();
	UInt16 ToUInt16();
	Int32 ToInt32();
	UInt32 ToUInt32();
	Int64 ToInt64();
	UInt64 ToUInt64();
	Char ToChar();
	Single ToSingle();
	Double ToDouble();
	Decimal ToDecimal();
	DateTime ToDateTime();

}; // interface IConvertible

}; // namespace System
