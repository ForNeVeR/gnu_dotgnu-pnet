.assembly extern '.library'
{
	.ver 0:0:0:0
}
.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.class private sequential sealed serializable ansi 'X' extends ['.library']'System'.'ValueType'
{
.field public int32 'x'
.method public hidebysig specialname rtspecialname instance void '.ctor'(int32 '_x') cil managed 
{
	ldarg.0
	ldarg.1
	stfld	int32 'X'::'x'
	ret
	.maxstack 2
} // method .ctor
} // class X
.class private sequential sealed serializable ansi 'Y' extends ['.library']'System'.'ValueType'
{
.field private int32 'y'
.method public hidebysig specialname rtspecialname instance void '.ctor'(int32 '_y') cil managed 
{
	ldarg.0
	ldarg.1
	stfld	int32 'Y'::'y'
	ret
	.maxstack 2
} // method .ctor
.method public static hidebysig specialname valuetype 'Y' 'op_Subtraction'(valuetype 'Y' 'y', valuetype 'X' 'x') cil managed 
{
	ldarga.s	0
	ldfld	int32 'Y'::'y'
	ldarga.s	1
	ldfld	int32 'X'::'x'
	sub
	newobj	instance void 'Y'::'.ctor'(int32)
	ret
	.maxstack 2
} // method op_Subtraction
.method public static hidebysig specialname valuetype 'X' 'op_Subtraction'(valuetype 'Y' 'y1', valuetype 'Y' 'y2') cil managed 
{
	ldarga.s	0
	ldfld	int32 'Y'::'y'
	ldarga.s	1
	ldfld	int32 'Y'::'y'
	sub
	newobj	instance void 'X'::'.ctor'(int32)
	ret
	.maxstack 2
} // method op_Subtraction
} // class Y
.class private auto ansi 'Test' extends ['.library']'System'.'Object'
{
.method private hidebysig instance void 'm1'() cil managed 
{
	.locals	(valuetype 'Y', valuetype 'Y', valuetype 'X', valuetype 'Y')
	ldc.i4.1
	newobj	instance void 'Y'::'.ctor'(int32)
	stloc.0
	ldc.i4.2
	newobj	instance void 'Y'::'.ctor'(int32)
	stloc.1
	ldloc.0
	ldloc.1
	call	valuetype 'X' 'Y'::'op_Subtraction'(valuetype 'Y', valuetype 'Y')
	stloc.2
	ldloc.0
	ldloc.2
	call	valuetype 'Y' 'Y'::'op_Subtraction'(valuetype 'Y', valuetype 'X')
	stloc.3
	ret
	.maxstack 2
} // method m1
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class Test
