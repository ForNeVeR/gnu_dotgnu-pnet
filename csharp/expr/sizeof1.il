.assembly extern '.library'
{
	.ver 0:0:0:0
}
.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.class private sequential value sealed serializable ansi 'S' extends ['.library']'System'.'ValueType'
{
.field private int32 'x'
.field private int32 'y'
} // class S
.class private auto ansi 'Test' extends ['.library']'System'.'Object'
{
.method private hidebysig instance void 'm1'() cil managed 
{
	.locals	(int32)
	ldc.i4.4
	stloc.0
	sizeof	['.library']'System'.'Decimal'
	stloc.0
	sizeof	'S'
	stloc.0
	ret
	.maxstack 1
} // method m1
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class Test
