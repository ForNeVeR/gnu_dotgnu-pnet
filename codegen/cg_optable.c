/*
 * cg_optable.c - Table of built-in operator definitions.
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

#include "cg_nodes.h"
#include "cg_coerce.h"

#ifdef	__cplusplus
extern	"C" {
#endif

static ILBuiltinType const ILSystemBoolean =
	{"System", "Boolean",
	 ILType_Boolean,
	 ILMachineType_Boolean};

static ILBuiltinType const ILSystemDecimal =
	{"System", "Decimal",
	 ILType_Invalid	/* Decimal is represented as a struct */,
	 ILMachineType_Decimal};

static ILBuiltinType const ILSystemDouble =
	{"System", "Double",
	 ILType_Float64,
	 ILMachineType_Float64};

static ILBuiltinType const ILSystemSingle =
	{"System", "Single",
	 ILType_Float32,
	 ILMachineType_Float32};

static ILBuiltinType const ILSystemInt64 =
	{"System", "Int64",
	 ILType_Int64,
	 ILMachineType_Int64};

static ILBuiltinType const ILSystemUInt64 =
	{"System", "UInt64",
	 ILType_UInt64,
	 ILMachineType_UInt64};

static ILBuiltinType const ILSystemInt32 =
	{"System", "Int32",
	 ILType_Int32,
	 ILMachineType_Int32};

static ILBuiltinType const ILSystemUInt32 =
	{"System", "UInt32",
	 ILType_UInt32,
	 ILMachineType_UInt32};

static ILBuiltinType const ILSystemInt16 =
	{"System", "Int16",
	 ILType_Int16,
	 ILMachineType_Int16};

static ILBuiltinType const ILSystemUInt16 =
	{"System", "UInt16",
	 ILType_UInt16,
	 ILMachineType_UInt16};

static ILBuiltinType const ILSystemSByte =
	{"System", "SByte",
	 ILType_Int8,
	 ILMachineType_Int8};

static ILBuiltinType const ILSystemByte =
	{"System", "Byte",
	 ILType_UInt8,
	 ILMachineType_UInt8};

static ILBuiltinType const ILSystemChar =
	{"System", "Char",
	 ILType_Char,
	 ILMachineType_Char};

#define	IL_BEGIN_OPERATOR_TABLE(op)	\
	ILOperator const ILOp_##op[] = {
#define	IL_UNARY_OPERATOR(outtype,intype)	\
	{&ILSystem##outtype, &ILSystem##intype, 0},
#define	IL_BINARY_OPERATOR(outtype,intype1,intype2)	\
	{&ILSystem##outtype, &ILSystem##intype1, &ILSystem##intype2},
#define	IL_END_OPERATOR_TABLE	\
	{0, 0, 0}};

#define	IL_BEGIN_CONVERT_TABLE(type)	\
	static ILConversion const ILConvert_##type[] = {
#define	IL_IMPLICIT_OPERATOR(type)	\
	{&ILSystem##type, 0},
#define	IL_EXPLICIT_OPERATOR(type)	\
	{&ILSystem##type, 1},
#define	IL_END_CONVERT_TABLE	\
	{0, 0}};

/*

int operator+(int x);
uint operator+(uint x);
long operator+(long x);
ulong operator+(ulong x);
float operator+(float x);
double operator+(double x);
decimal operator+(decimal x);

*/

IL_BEGIN_OPERATOR_TABLE(UnaryPlus)
	IL_UNARY_OPERATOR(Int32, Int32)
	IL_UNARY_OPERATOR(UInt32, UInt32)
	IL_UNARY_OPERATOR(Int64, Int64)
	IL_UNARY_OPERATOR(UInt64, UInt64)
	IL_UNARY_OPERATOR(Single, Single)
	IL_UNARY_OPERATOR(Double, Double)
	IL_UNARY_OPERATOR(Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

int operator-(int x);
long operator-(long x);
float operator-(float x);
double operator-(double x);
decimal operator-(decimal x);

*/

IL_BEGIN_OPERATOR_TABLE(Neg)
	IL_UNARY_OPERATOR(Int32, Int32)
	IL_UNARY_OPERATOR(Int64, Int64)
	IL_UNARY_OPERATOR(Single, Single)
	IL_UNARY_OPERATOR(Double, Double)
	IL_UNARY_OPERATOR(Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

bool operator!(bool x);

*/

IL_BEGIN_OPERATOR_TABLE(LogicalNot)
	IL_UNARY_OPERATOR(Boolean, Boolean)
IL_END_OPERATOR_TABLE

/*

int operator~(int x);
uint operator~(uint x);
long operator~(long x);
ulong operator~(ulong x);

*/

IL_BEGIN_OPERATOR_TABLE(Not)
	IL_UNARY_OPERATOR(Int32, Int32)
	IL_UNARY_OPERATOR(UInt32, UInt32)
	IL_UNARY_OPERATOR(Int64, Int64)
	IL_UNARY_OPERATOR(UInt64, UInt64)
IL_END_OPERATOR_TABLE

/*

int operator*(int x, int y);
uint operator*(uint x, uint y);
long operator*(long x, long y);
ulong operator*(ulong x, ulong y);
float operator*(float x, float y);
double operator*(double x, double y);
decimal operator*(decimal x, decimal y);

*/

IL_BEGIN_OPERATOR_TABLE(Mul)
	IL_BINARY_OPERATOR(Int32, Int32, Int32)
	IL_BINARY_OPERATOR(UInt32, UInt32, UInt32)
	IL_BINARY_OPERATOR(Int64, Int64, Int64)
	IL_BINARY_OPERATOR(UInt64, UInt64, UInt64)
	IL_BINARY_OPERATOR(Single, Single, Single)
	IL_BINARY_OPERATOR(Double, Double, Double)
	IL_BINARY_OPERATOR(Decimal, Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

int operator/(int x, int y);
uint operator/(uint x, uint y);
long operator/(long x, long y);
ulong operator/(ulong x, ulong y);
float operator/(float x, float y);
double operator/(double x, double y);
decimal operator/(decimal x, decimal y);

*/

IL_BEGIN_OPERATOR_TABLE(Div)
	IL_BINARY_OPERATOR(Int32, Int32, Int32)
	IL_BINARY_OPERATOR(UInt32, UInt32, UInt32)
	IL_BINARY_OPERATOR(Int64, Int64, Int64)
	IL_BINARY_OPERATOR(UInt64, UInt64, UInt64)
	IL_BINARY_OPERATOR(Single, Single, Single)
	IL_BINARY_OPERATOR(Double, Double, Double)
	IL_BINARY_OPERATOR(Decimal, Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

int operator%(int x, int y);
uint operator%(uint x, uint y);
long operator%(long x, long y);
ulong operator%(ulong x, ulong y);
float operator%(float x, float y);
double operator%(double x, double y);
decimal operator%(decimal x, decimal y);

*/

IL_BEGIN_OPERATOR_TABLE(Rem)
	IL_BINARY_OPERATOR(Int32, Int32, Int32)
	IL_BINARY_OPERATOR(UInt32, UInt32, UInt32)
	IL_BINARY_OPERATOR(Int64, Int64, Int64)
	IL_BINARY_OPERATOR(UInt64, UInt64, UInt64)
	IL_BINARY_OPERATOR(Single, Single, Single)
	IL_BINARY_OPERATOR(Double, Double, Double)
	IL_BINARY_OPERATOR(Decimal, Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

int operator+(int x, int y);
uint operator+(uint x, uint y);
long operator+(long x, long y);
ulong operator+(ulong x, ulong y);
float operator+(float x, float y);
double operator+(double x, double y);
decimal operator+(decimal x, decimal y);
string operator+(string x, string y);
string operator+(string x, object y);
string operator+(object x, string y);

*/

IL_BEGIN_OPERATOR_TABLE(Add)
	IL_BINARY_OPERATOR(Int32, Int32, Int32)
	IL_BINARY_OPERATOR(UInt32, UInt32, UInt32)
	IL_BINARY_OPERATOR(Int64, Int64, Int64)
	IL_BINARY_OPERATOR(UInt64, UInt64, UInt64)
	IL_BINARY_OPERATOR(Single, Single, Single)
	IL_BINARY_OPERATOR(Double, Double, Double)
	IL_BINARY_OPERATOR(Decimal, Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

int operator-(int x, int y);
uint operator-(uint x, uint y);
long operator-(long x, long y);
ulong operator-(ulong x, ulong y);
float operator-(float x, float y);
double operator-(double x, double y);
decimal operator-(decimal x, decimal y);

*/

IL_BEGIN_OPERATOR_TABLE(Sub)
	IL_BINARY_OPERATOR(Int32, Int32, Int32)
	IL_BINARY_OPERATOR(UInt32, UInt32, UInt32)
	IL_BINARY_OPERATOR(Int64, Int64, Int64)
	IL_BINARY_OPERATOR(UInt64, UInt64, UInt64)
	IL_BINARY_OPERATOR(Single, Single, Single)
	IL_BINARY_OPERATOR(Double, Double, Double)
	IL_BINARY_OPERATOR(Decimal, Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

int operator<<(int x, int count);
uint operator<<(uint x, int count);
long operator<<(long x, int count);
ulong operator<<(ulong x, int count);

*/

IL_BEGIN_OPERATOR_TABLE(Shl)
	IL_BINARY_OPERATOR(Int32, Int32, Int32)
	IL_BINARY_OPERATOR(UInt32, UInt32, Int32)
	IL_BINARY_OPERATOR(Int64, Int64, Int32)
	IL_BINARY_OPERATOR(UInt64, UInt64, Int32)
IL_END_OPERATOR_TABLE

/*

int operator>>(int x, int count);
uint operator>>(uint x, int count);
long operator>>(long x, int count);
ulong operator>>(ulong x, int count);

*/

IL_BEGIN_OPERATOR_TABLE(Shr)
	IL_BINARY_OPERATOR(Int32, Int32, Int32)
	IL_BINARY_OPERATOR(UInt32, UInt32, Int32)
	IL_BINARY_OPERATOR(Int64, Int64, Int32)
	IL_BINARY_OPERATOR(UInt64, UInt64, Int32)
IL_END_OPERATOR_TABLE

/*

bool operator==(int x, int y);
bool operator==(uint x, uint y);
bool operator==(long x, long y);
bool operator==(ulong x, ulong y);
bool operator==(float x, float y);
bool operator==(double x, double y);
bool operator==(decimal x, decimal y);
bool operator==(bool x, bool y);
bool operator==(object x, object y);
bool operator==(string x, string y);

*/

IL_BEGIN_OPERATOR_TABLE(Eq)
	IL_BINARY_OPERATOR(Boolean, Int32, Int32)
	IL_BINARY_OPERATOR(Boolean, UInt32, UInt32)
	IL_BINARY_OPERATOR(Boolean, Int64, Int64)
	IL_BINARY_OPERATOR(Boolean, UInt64, UInt64)
	IL_BINARY_OPERATOR(Boolean, Single, Single)
	IL_BINARY_OPERATOR(Boolean, Double, Double)
	IL_BINARY_OPERATOR(Boolean, Decimal, Decimal)
	IL_BINARY_OPERATOR(Boolean, Boolean, Boolean)
IL_END_OPERATOR_TABLE

/*

bool operator!=(int x, int y);
bool operator!=(uint x, uint y);
bool operator!=(long x, long y);
bool operator!=(ulong x, ulong y);
bool operator!=(float x, float y);
bool operator!=(double x, double y);
bool operator!=(decimal x, decimal y);
bool operator!=(bool x, bool y);
bool operator!=(object x, object y);
bool operator!=(string x, string y);

*/

IL_BEGIN_OPERATOR_TABLE(Ne)
	IL_BINARY_OPERATOR(Boolean, Int32, Int32)
	IL_BINARY_OPERATOR(Boolean, UInt32, UInt32)
	IL_BINARY_OPERATOR(Boolean, Int64, Int64)
	IL_BINARY_OPERATOR(Boolean, UInt64, UInt64)
	IL_BINARY_OPERATOR(Boolean, Single, Single)
	IL_BINARY_OPERATOR(Boolean, Double, Double)
	IL_BINARY_OPERATOR(Boolean, Decimal, Decimal)
	IL_BINARY_OPERATOR(Boolean, Boolean, Boolean)
IL_END_OPERATOR_TABLE

/*

bool operator<(int x, int y);
bool operator<(uint x, uint y);
bool operator<(long x, long y);
bool operator<(ulong x, ulong y);
bool operator<(float x, float y);
bool operator<(double x, double y);
bool operator<(decimal x, decimal y);

*/

IL_BEGIN_OPERATOR_TABLE(Lt)
	IL_BINARY_OPERATOR(Boolean, Int32, Int32)
	IL_BINARY_OPERATOR(Boolean, UInt32, UInt32)
	IL_BINARY_OPERATOR(Boolean, Int64, Int64)
	IL_BINARY_OPERATOR(Boolean, UInt64, UInt64)
	IL_BINARY_OPERATOR(Boolean, Single, Single)
	IL_BINARY_OPERATOR(Boolean, Double, Double)
	IL_BINARY_OPERATOR(Boolean, Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

bool operator>(int x, int y);
bool operator>(uint x, uint y);
bool operator>(long x, long y);
bool operator>(ulong x, ulong y);
bool operator>(float x, float y);
bool operator>(double x, double y);
bool operator>(decimal x, decimal y);

*/

IL_BEGIN_OPERATOR_TABLE(Gt)
	IL_BINARY_OPERATOR(Boolean, Int32, Int32)
	IL_BINARY_OPERATOR(Boolean, UInt32, UInt32)
	IL_BINARY_OPERATOR(Boolean, Int64, Int64)
	IL_BINARY_OPERATOR(Boolean, UInt64, UInt64)
	IL_BINARY_OPERATOR(Boolean, Single, Single)
	IL_BINARY_OPERATOR(Boolean, Double, Double)
	IL_BINARY_OPERATOR(Boolean, Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

bool operator<=(int x, int y);
bool operator<=(uint x, uint y);
bool operator<=(long x, long y);
bool operator<=(ulong x, ulong y);
bool operator<=(float x, float y);
bool operator<=(double x, double y);
bool operator<=(decimal x, decimal y);

*/

IL_BEGIN_OPERATOR_TABLE(Le)
	IL_BINARY_OPERATOR(Boolean, Int32, Int32)
	IL_BINARY_OPERATOR(Boolean, UInt32, UInt32)
	IL_BINARY_OPERATOR(Boolean, Int64, Int64)
	IL_BINARY_OPERATOR(Boolean, UInt64, UInt64)
	IL_BINARY_OPERATOR(Boolean, Single, Single)
	IL_BINARY_OPERATOR(Boolean, Double, Double)
	IL_BINARY_OPERATOR(Boolean, Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

bool operator>=(int x, int y);
bool operator>=(uint x, uint y);
bool operator>=(long x, long y);
bool operator>=(ulong x, ulong y);
bool operator>=(float x, float y);
bool operator>=(double x, double y);
bool operator>=(decimal x, decimal y);

*/

IL_BEGIN_OPERATOR_TABLE(Ge)
	IL_BINARY_OPERATOR(Boolean, Int32, Int32)
	IL_BINARY_OPERATOR(Boolean, UInt32, UInt32)
	IL_BINARY_OPERATOR(Boolean, Int64, Int64)
	IL_BINARY_OPERATOR(Boolean, UInt64, UInt64)
	IL_BINARY_OPERATOR(Boolean, Single, Single)
	IL_BINARY_OPERATOR(Boolean, Double, Double)
	IL_BINARY_OPERATOR(Boolean, Decimal, Decimal)
IL_END_OPERATOR_TABLE

/*

bool operator&(bool x, bool y);
int operator&(int x, int y);
uint operator&(uint x, uint y);
long operator&(long x, long y);
ulong operator&(ulong x, ulong y);

*/

IL_BEGIN_OPERATOR_TABLE(And)
	IL_BINARY_OPERATOR(Boolean, Boolean, Boolean)
	IL_BINARY_OPERATOR(Int32, Int32, Int32)
	IL_BINARY_OPERATOR(UInt32, UInt32, UInt32)
	IL_BINARY_OPERATOR(Int64, Int64, Int64)
	IL_BINARY_OPERATOR(UInt64, UInt64, UInt64)
IL_END_OPERATOR_TABLE

/*

bool operator|(bool x, bool y);
int operator|(int x, int y);
uint operator|(uint x, uint y);
long operator|(long x, long y);
ulong operator|(ulong x, ulong y);

*/

IL_BEGIN_OPERATOR_TABLE(Or)
	IL_BINARY_OPERATOR(Boolean, Boolean, Boolean)
	IL_BINARY_OPERATOR(Int32, Int32, Int32)
	IL_BINARY_OPERATOR(UInt32, UInt32, UInt32)
	IL_BINARY_OPERATOR(Int64, Int64, Int64)
	IL_BINARY_OPERATOR(UInt64, UInt64, UInt64)
IL_END_OPERATOR_TABLE

/*

bool operator^(bool x, bool y);
int operator^(int x, int y);
uint operator^(uint x, uint y);
long operator^(long x, long y);
ulong operator^(ulong x, ulong y);

*/

IL_BEGIN_OPERATOR_TABLE(Xor)
	IL_BINARY_OPERATOR(Boolean, Boolean, Boolean)
	IL_BINARY_OPERATOR(Int32, Int32, Int32)
	IL_BINARY_OPERATOR(UInt32, UInt32, UInt32)
	IL_BINARY_OPERATOR(Int64, Int64, Int64)
	IL_BINARY_OPERATOR(UInt64, UInt64, UInt64)
IL_END_OPERATOR_TABLE

/*

implicit operator short(sbyte x);
implicit operator int(sbyte x);
implicit operator long(sbyte x);
implicit operator float(sbyte x);
implicit operator double(sbyte x);
implicit operator decimal(sbyte x);

explicit operator byte(sbyte x);
explicit operator ushort(sbyte x);
explicit operator uint(sbyte x);
explicit operator ulong(sbyte x);
explicit operator char(sbyte x);

*/

IL_BEGIN_CONVERT_TABLE(SByte)
	IL_IMPLICIT_OPERATOR(Int16)
	IL_IMPLICIT_OPERATOR(Int32)
	IL_IMPLICIT_OPERATOR(Int64)
	IL_IMPLICIT_OPERATOR(Single)
	IL_IMPLICIT_OPERATOR(Double)
	IL_IMPLICIT_OPERATOR(Decimal)

	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(UInt16)
	IL_EXPLICIT_OPERATOR(UInt32)
	IL_EXPLICIT_OPERATOR(UInt64)
	IL_EXPLICIT_OPERATOR(Char)
IL_END_CONVERT_TABLE

/*

implicit operator short(byte x);
implicit operator ushort(byte x);
implicit operator int(byte x);
implicit operator uint(byte x);
implicit operator long(byte x);
implicit operator ulong(byte x);
implicit operator float(byte x);
implicit operator double(byte x);
implicit operator decimal(byte x);

explicit operator sbyte(byte x);
explicit operator char(byte x);

*/

IL_BEGIN_CONVERT_TABLE(Byte)
	IL_IMPLICIT_OPERATOR(Int16)
	IL_IMPLICIT_OPERATOR(UInt16)
	IL_IMPLICIT_OPERATOR(Int32)
	IL_IMPLICIT_OPERATOR(UInt32)
	IL_IMPLICIT_OPERATOR(Int64)
	IL_IMPLICIT_OPERATOR(UInt64)
	IL_IMPLICIT_OPERATOR(Single)
	IL_IMPLICIT_OPERATOR(Double)
	IL_IMPLICIT_OPERATOR(Decimal)

	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Char)
IL_END_CONVERT_TABLE

/*

implicit operator int(short x);
implicit operator long(short x);
implicit operator float(short x);
implicit operator double(short x);
implicit operator decimal(short x);

explicit operator sbyte(short x);
explicit operator byte(short x);
explicit operator ushort(short x);
explicit operator uint(short x);
explicit operator ulong(short x);
explicit operator char(short x);

*/

IL_BEGIN_CONVERT_TABLE(Int16)
	IL_IMPLICIT_OPERATOR(Int32)
	IL_IMPLICIT_OPERATOR(Int64)
	IL_IMPLICIT_OPERATOR(Single)
	IL_IMPLICIT_OPERATOR(Double)
	IL_IMPLICIT_OPERATOR(Decimal)

	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(UInt16)
	IL_EXPLICIT_OPERATOR(UInt32)
	IL_EXPLICIT_OPERATOR(UInt64)
	IL_EXPLICIT_OPERATOR(Char)
IL_END_CONVERT_TABLE

/*

implicit operator int(ushort x);
implicit operator uint(ushort x);
implicit operator long(ushort x);
implicit operator ulong(ushort x);
implicit operator float(ushort x);
implicit operator double(ushort x);
implicit operator decimal(ushort x);

explicit operator sbyte(ushort x);
explicit operator byte(ushort x);
explicit operator short(ushort x);
explicit operator char(ushort x);

*/

IL_BEGIN_CONVERT_TABLE(UInt16)
	IL_IMPLICIT_OPERATOR(Int32)
	IL_IMPLICIT_OPERATOR(UInt32)
	IL_IMPLICIT_OPERATOR(Int64)
	IL_IMPLICIT_OPERATOR(UInt64)
	IL_IMPLICIT_OPERATOR(Single)
	IL_IMPLICIT_OPERATOR(Double)
	IL_IMPLICIT_OPERATOR(Decimal)

	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(Int16)
	IL_EXPLICIT_OPERATOR(Char)
IL_END_CONVERT_TABLE

/*

implicit operator long(int x);
implicit operator float(int x);
implicit operator double(int x);
implicit operator decimal(int x);

explicit operator sbyte(int x);
explicit operator byte(int x);
explicit operator short(int x);
explicit operator ushort(int x);
explicit operator uint(int x);
explicit operator ulong(int x);
explicit operator char(int x);

*/

IL_BEGIN_CONVERT_TABLE(Int32)
	IL_IMPLICIT_OPERATOR(Int64)
	IL_IMPLICIT_OPERATOR(Single)
	IL_IMPLICIT_OPERATOR(Double)
	IL_IMPLICIT_OPERATOR(Decimal)

	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(Int16)
	IL_EXPLICIT_OPERATOR(UInt16)
	IL_EXPLICIT_OPERATOR(UInt32)
	IL_EXPLICIT_OPERATOR(UInt64)
	IL_EXPLICIT_OPERATOR(Char)
IL_END_CONVERT_TABLE

/*

implicit operator long(uint x);
implicit operator ulong(uint x);
implicit operator float(uint x);
implicit operator double(uint x);
implicit operator decimal(uint x);

explicit operator sbyte(uint x);
explicit operator byte(uint x);
explicit operator short(uint x);
explicit operator ushort(uint x);
explicit operator int(uint x);
explicit operator char(uint x);

*/

IL_BEGIN_CONVERT_TABLE(UInt32)
	IL_IMPLICIT_OPERATOR(Int64)
	IL_IMPLICIT_OPERATOR(UInt64)
	IL_IMPLICIT_OPERATOR(Single)
	IL_IMPLICIT_OPERATOR(Double)
	IL_IMPLICIT_OPERATOR(Decimal)

	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(Int16)
	IL_EXPLICIT_OPERATOR(UInt16)
	IL_EXPLICIT_OPERATOR(Int32)
	IL_EXPLICIT_OPERATOR(Char)
IL_END_CONVERT_TABLE

/*

implicit operator float(long x);
implicit operator double(long x);
implicit operator decimal(long x);

explicit operator sbyte(long x);
explicit operator byte(long x);
explicit operator short(long x);
explicit operator ushort(long x);
explicit operator int(long x);
explicit operator uint(long x);
explicit operator ulong(long x);
explicit operator char(long x);

*/

IL_BEGIN_CONVERT_TABLE(Int64)
	IL_IMPLICIT_OPERATOR(Single)
	IL_IMPLICIT_OPERATOR(Double)
	IL_IMPLICIT_OPERATOR(Decimal)

	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(Int16)
	IL_EXPLICIT_OPERATOR(UInt16)
	IL_EXPLICIT_OPERATOR(Int32)
	IL_EXPLICIT_OPERATOR(UInt32)
	IL_EXPLICIT_OPERATOR(UInt64)
	IL_EXPLICIT_OPERATOR(Char)
IL_END_CONVERT_TABLE

/*

implicit operator float(ulong x);
implicit operator double(ulong x);
implicit operator decimal(ulong x);

explicit operator sbyte(ulong x);
explicit operator byte(ulong x);
explicit operator short(ulong x);
explicit operator ushort(ulong x);
explicit operator int(ulong x);
explicit operator uint(ulong x);
explicit operator long(ulong x);
explicit operator char(ulong x);

*/

IL_BEGIN_CONVERT_TABLE(UInt64)
	IL_IMPLICIT_OPERATOR(Single)
	IL_IMPLICIT_OPERATOR(Double)
	IL_IMPLICIT_OPERATOR(Decimal)

	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(Int16)
	IL_EXPLICIT_OPERATOR(UInt16)
	IL_EXPLICIT_OPERATOR(Int32)
	IL_EXPLICIT_OPERATOR(UInt32)
	IL_EXPLICIT_OPERATOR(Int64)
	IL_EXPLICIT_OPERATOR(Char)
IL_END_CONVERT_TABLE

/*

implicit operator ushort(char x);
implicit operator int(char x);
implicit operator uint(char x);
implicit operator long(char x);
implicit operator ulong(char x);
implicit operator float(char x);
implicit operator double(char x);
implicit operator decimal(char x);

explicit operator sbyte(char x);
explicit operator byte(char x);
explicit operator short(char x);

*/

IL_BEGIN_CONVERT_TABLE(Char)
	IL_IMPLICIT_OPERATOR(UInt16)
	IL_IMPLICIT_OPERATOR(Int32)
	IL_IMPLICIT_OPERATOR(UInt32)
	IL_IMPLICIT_OPERATOR(Int64)
	IL_IMPLICIT_OPERATOR(UInt64)
	IL_IMPLICIT_OPERATOR(Single)
	IL_IMPLICIT_OPERATOR(Double)
	IL_IMPLICIT_OPERATOR(Decimal)

	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(Int16)
IL_END_CONVERT_TABLE

/*

implicit operator double(float x);

explicit operator sbyte(float x);
explicit operator byte(float x);
explicit operator short(float x);
explicit operator ushort(float x);
explicit operator int(float x);
explicit operator uint(float x);
explicit operator long(float x);
explicit operator ulong(float x);
explicit operator char(float x);
explicit operator decimal(float x);

*/

IL_BEGIN_CONVERT_TABLE(Single)
	IL_IMPLICIT_OPERATOR(Double)

	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(Int16)
	IL_EXPLICIT_OPERATOR(UInt16)
	IL_EXPLICIT_OPERATOR(Int32)
	IL_EXPLICIT_OPERATOR(UInt32)
	IL_EXPLICIT_OPERATOR(Int64)
	IL_EXPLICIT_OPERATOR(UInt64)
	IL_EXPLICIT_OPERATOR(Char)
	IL_EXPLICIT_OPERATOR(Decimal)
IL_END_CONVERT_TABLE

/*

explicit operator sbyte(double x);
explicit operator byte(double x);
explicit operator short(double x);
explicit operator ushort(double x);
explicit operator int(double x);
explicit operator uint(double x);
explicit operator long(double x);
explicit operator ulong(double x);
explicit operator char(double x);
explicit operator float(double x);
explicit operator decimal(double x);

*/

IL_BEGIN_CONVERT_TABLE(Double)
	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(Int16)
	IL_EXPLICIT_OPERATOR(UInt16)
	IL_EXPLICIT_OPERATOR(Int32)
	IL_EXPLICIT_OPERATOR(UInt32)
	IL_EXPLICIT_OPERATOR(Int64)
	IL_EXPLICIT_OPERATOR(UInt64)
	IL_EXPLICIT_OPERATOR(Char)
	IL_EXPLICIT_OPERATOR(Single)
	IL_EXPLICIT_OPERATOR(Decimal)
IL_END_CONVERT_TABLE

/*

explicit operator sbyte(decimal x);
explicit operator byte(decimal x);
explicit operator short(decimal x);
explicit operator ushort(decimal x);
explicit operator int(decimal x);
explicit operator uint(decimal x);
explicit operator long(decimal x);
explicit operator ulong(decimal x);
explicit operator char(decimal x);
explicit operator float(decimal x);
explicit operator double(decimal x);

*/

IL_BEGIN_CONVERT_TABLE(Decimal)
	IL_EXPLICIT_OPERATOR(SByte)
	IL_EXPLICIT_OPERATOR(Byte)
	IL_EXPLICIT_OPERATOR(Int16)
	IL_EXPLICIT_OPERATOR(UInt16)
	IL_EXPLICIT_OPERATOR(Int32)
	IL_EXPLICIT_OPERATOR(UInt32)
	IL_EXPLICIT_OPERATOR(Int64)
	IL_EXPLICIT_OPERATOR(UInt64)
	IL_EXPLICIT_OPERATOR(Char)
	IL_EXPLICIT_OPERATOR(Single)
	IL_EXPLICIT_OPERATOR(Double)
IL_END_CONVERT_TABLE

/*
 * Index that maps builtin types to the corresponding conversion table.
 */
static struct
{
	const ILBuiltinType *type;
	const ILConversion  *table;

} const ConvertIndex[] = {
		{&ILSystemSByte,   ILConvert_SByte},
		{&ILSystemByte,    ILConvert_Byte},
		{&ILSystemInt16,   ILConvert_Int16},
		{&ILSystemUInt16,  ILConvert_UInt16},
		{&ILSystemInt32,   ILConvert_Int32},
		{&ILSystemUInt32,  ILConvert_UInt32},
		{&ILSystemInt64,   ILConvert_Int64},
		{&ILSystemUInt64,  ILConvert_UInt64},
		{&ILSystemChar,    ILConvert_Char},
		{&ILSystemSingle,  ILConvert_Single},
		{&ILSystemDouble,  ILConvert_Double},
		{&ILSystemDecimal, ILConvert_Decimal},
		{0, 0}
};

/*
 * Convert an external type representation into a builtin type.
 */
static const ILBuiltinType *GetBuiltinType(ILType *type)
{
	ILClass *info;
	if(type == ILType_Boolean)
	{
		return &ILSystemBoolean;
	}
	else if(type == ILType_Int8)
	{
		return &ILSystemSByte;
	}
	else if(type == ILType_UInt8)
	{
		return &ILSystemByte;
	}
	else if(type == ILType_Int16)
	{
		return &ILSystemInt16;
	}
	else if(type == ILType_UInt16)
	{
		return &ILSystemUInt16;
	}
	else if(type == ILType_Int32)
	{
		return &ILSystemInt32;
	}
	else if(type == ILType_UInt32)
	{
		return &ILSystemUInt32;
	}
	else if(type == ILType_Int64)
	{
		return &ILSystemInt64;
	}
	else if(type == ILType_UInt64)
	{
		return &ILSystemUInt64;
	}
	else if(type == ILType_Float32)
	{
		return &ILSystemSingle;
	}
	else if(type == ILType_Float64)
	{
		return &ILSystemDouble;
	}
	else if(type == ILType_Char)
	{
		return &ILSystemChar;
	}
	else if(ILType_IsValueType(type))
	{
		info = ILType_ToValueType(type);
		if(info &&
		   !strcmp(ILClass_Name(info), "Decimal") &&
		   ILClass_Namespace(info) &&
		   !strcmp(ILClass_Namespace(info), "System"))
		{
			return &ILSystemDecimal;
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

const ILConversion *ILFindConversion(ILType *fromType, ILType *toType,
								     int explicit)
{
	const ILBuiltinType *type1 = GetBuiltinType(fromType);
	const ILBuiltinType *type2 = GetBuiltinType(toType);
	int posn;
	const ILConversion *conv;
	if(type1 && type2)
	{
		posn = 0;
		while(ConvertIndex[posn].type != 0)
		{
			if(ConvertIndex[posn].type == type1)
			{
				conv = ConvertIndex[posn].table;
				while(conv->outtype != 0)
				{
					if(conv->outtype == type2)
					{
						if(!(conv->explicit))
						{
							return conv;
						}
						else if(explicit)
						{
							return conv;
						}
					}
					++conv;
				}
			}
			++posn;
		}
	}
	return 0;
}

/*
 * Insert a primitive type cast to up-convert a node
 * if the node does not already have the requested type.
 */
static void InsertPrimitive(ILGenInfo *info, ILNode *node, ILNode **parent,
							ILMachineType type)
{
	if(node != 0 && ILNode_GetType(node, info) != type)
	{
		*parent = ILNode_CastSimple_create(node, type);
		yysetfilename(*parent, yygetfilename(node));
		yysetlinenum(*parent, yygetlinenum(node));
	}
}

void ILApplyConversion(ILGenInfo *info, ILNode *node, ILNode **parent,
					   const ILConversion *conv)
{
	InsertPrimitive(info, node, parent, conv->outtype->valueType);
}

const ILOperator *ILFindUnaryOperator(const ILOperator *table,
								 	  ILType *argType1)
{
	/* Convert the type into its builtin form */
	const ILBuiltinType *type1 = GetBuiltinType(argType1);
	if(!type1)
	{
		return 0;
	}

	/* Apply numeric promotions to the type */
	if(type1 == &ILSystemSByte || type1 == &ILSystemByte ||
	   type1 == &ILSystemInt16 || type1 == &ILSystemUInt16 ||
	   type1 == &ILSystemChar)
	{
		type1 = &ILSystemInt32;
	}
	else if(type1 == &ILSystemUInt32 && table == ILOp_Neg)
	{
		type1 = &ILSystemInt64;
	}

	/* Search for a matching operator */
	while(table->outtype != 0)
	{
		if(table->intype1 == type1)
		{
			return table;
		}
		++table;
	}
	return 0;
}

void ILApplyUnaryOperator(ILGenInfo *info, ILNode *node, ILNode **parent,
					      const ILOperator *oper)
{
	/* Cast the argument to the correct type */
	ILNode_UnaryExpression *unary = (ILNode_UnaryExpression *)node;
	InsertPrimitive(info, unary->expr, &(unary->expr),
					oper->intype1->valueType);

	/* Insert a primitive type cast to adjust the result */
	InsertPrimitive(info, node, parent, oper->outtype->valueType);
}

const ILOperator *ILFindBinaryOperator(const ILOperator *table,
								 	   ILType *argType1, ILType *argType2)
{
	/* Convert the types into their builtin forms */
	const ILBuiltinType *type1 = GetBuiltinType(argType1);
	const ILBuiltinType *type2 = GetBuiltinType(argType2);
	if(!type1 || !type2)
	{
		return 0;
	}

	/* Apply numeric promotions */
	if(table != ILOp_Shl && table != ILOp_Shr)
	{
		/* Arithmetic or bitwise operator */
		if(type1 == &ILSystemDecimal)
		{
			if(type2 == &ILSystemSingle || type2 == &ILSystemDouble)
			{
				return 0;
			}
			type2 = &ILSystemDecimal;
		}
		else if(type2 == &ILSystemDecimal)
		{
			if(type1 == &ILSystemSingle || type1 == &ILSystemDouble)
			{
				return 0;
			}
			type1 = &ILSystemDecimal;
		}
		else if(type1 == &ILSystemDouble)
		{
			type2 = &ILSystemDouble;
		}
		else if(type2 == &ILSystemDouble)
		{
			type1 = &ILSystemDouble;
		}
		else if(type1 == &ILSystemSingle)
		{
			type2 = &ILSystemSingle;
		}
		else if(type2 == &ILSystemSingle)
		{
			type1 = &ILSystemSingle;
		}
		else if(type1 == &ILSystemUInt64)
		{
			if(type2 == &ILSystemSByte || type2 == &ILSystemInt16 ||
			   type2 == &ILSystemInt32 || type2 == &ILSystemInt64)
			{
				return 0;
			}
			type2 = &ILSystemUInt64;
		}
		else if(type2 == &ILSystemUInt64)
		{
			if(type1 == &ILSystemSByte || type1 == &ILSystemInt16 ||
			   type1 == &ILSystemInt32 || type1 == &ILSystemInt64)
			{
				return 0;
			}
			type1 = &ILSystemUInt64;
		}
		else if(type1 == &ILSystemInt64)
		{
			type2 = &ILSystemInt64;
		}
		else if(type2 == &ILSystemInt64)
		{
			type1 = &ILSystemInt64;
		}
		else if(type1 == &ILSystemUInt32)
		{
			if(type2 == &ILSystemSByte || type2 == &ILSystemInt16 ||
			   type2 == &ILSystemInt32)
			{
				type1 = &ILSystemInt64;
				type2 = &ILSystemInt64;
			}
			else
			{
				type2 = &ILSystemUInt32;
			}
		}
		else if(type2 == &ILSystemUInt32)
		{
			if(type1 == &ILSystemSByte || type1 == &ILSystemInt16 ||
			   type1 == &ILSystemInt32)
			{
				type1 = &ILSystemInt64;
				type2 = &ILSystemInt64;
			}
			else
			{
				type1 = &ILSystemUInt32;
			}
		}
		else
		{
			type1 = &ILSystemInt32;
			type2 = &ILSystemInt32;
		}
	}
	else
	{
		/* Shift operator: promote the arguments separately */
		if(type1 == &ILSystemSByte || type1 == &ILSystemByte ||
		   type1 == &ILSystemInt16 || type1 == &ILSystemUInt16 ||
		   type1 == &ILSystemChar)
		{
			type1 = &ILSystemInt32;
		}
		if(type2 == &ILSystemSByte || type2 == &ILSystemByte ||
		   type2 == &ILSystemInt16 || type2 == &ILSystemUInt16 ||
		   type2 == &ILSystemChar)
		{
			type2 = &ILSystemInt32;
		}
	}

	/* Search for a matching operator */
	while(table->outtype != 0)
	{
		if(table->intype1 == type1 && table->intype2 == type2)
		{
			return table;
		}
		++table;
	}
	return 0;
}

void ILApplyBinaryOperator(ILGenInfo *info, ILNode *node, ILNode **parent,
					       const ILOperator *oper)
{
	/* Cast the arguments to the correct type */
	ILNode_BinaryExpression *binary = (ILNode_BinaryExpression *)node;
	InsertPrimitive(info, binary->expr1, &(binary->expr1),
					oper->intype1->valueType);
	InsertPrimitive(info, binary->expr2, &(binary->expr2),
					oper->intype2->valueType);

	/* Insert a primitive type cast to adjust the result */
	InsertPrimitive(info, node, parent, oper->outtype->valueType);
}

int ILIsBuiltinNumeric(ILType *type)
{
	return (GetBuiltinType(type) != 0 && type != ILType_Boolean);
}

#ifdef	__cplusplus
};
#endif
