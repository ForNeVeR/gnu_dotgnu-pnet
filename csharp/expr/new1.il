.assembly extern '.library'
{
	.ver 0:0:0:0
}
.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.class private sequential value sealed serializable ansi 'X' extends ['.library']'System'.'ValueType'
{
.field private int32 'x'
.method public hidebysig specialname rtspecialname instance void '.ctor'(int32 '_x') cil managed 
{
	ldarg.0
	ldarg.1
	stfld	int32 'X'::'x'
	ret
	.maxstack 2
} // method .ctor
} // class X
.class private auto sealed serializable ansi 'Color' extends ['.library']'System'.'Enum'
{
.field public static literal valuetype 'Color' 'Red' = int32(0x00000000)
.field public static literal valuetype 'Color' 'Green' = int32(0x00000001)
.field public static literal valuetype 'Color' 'Blue' = int32(0x00000002)
.field public specialname rtspecialname int32 'value__'
} // class Color
.class private auto ansi 'Test' extends ['.library']'System'.'Object'
{
.method private hidebysig instance void 'm1'() cil managed 
{
	.locals	(int32, valuetype 'X', valuetype 'Color', class 'Test2', class ['.library']'System'.'Object')
	ldc.i4.0
	stloc.0
	.locals	(valuetype 'X')
	ldloca.s	5
	initobj	'X'
	ldloc.s	5
	stloc.1
	ldc.i4.0
	stloc.2
	ldc.i4.3
	newobj	instance void 'X'::'.ctor'(int32)
	stloc.1
	newobj	instance void 'Test2'::'.ctor'()
	stloc.3
	ldc.i4.3
	newobj	instance void 'Test2'::'.ctor'(int32)
	stloc.3
	ldc.i4.3
	conv.i8
	newobj	instance void 'Test2'::'.ctor'(int64)
	stloc.3
	newobj	instance void ['.library']'System'.'Object'::'.ctor'()
	stloc.s	4
	ret
	.maxstack 1
} // method m1
} // class Test
.class private auto ansi 'Test2' extends ['.library']'System'.'Object'
{
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
.method public hidebysig specialname rtspecialname instance void '.ctor'(int32 'x') cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
.method public hidebysig specialname rtspecialname instance void '.ctor'(int64 'x') cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class Test2
