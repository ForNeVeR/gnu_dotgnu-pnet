.assembly extern '.library'
{
	.ver 0:0:0:0
}
.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.class private auto ansi 'Test' extends ['.library']'System'.'Object'
{
.field public static int32[] 'x'
.field public static int64[] 'y'
.method public static hidebysig void 'test'() cil managed 
{
	.locals	(int32[])
	ldc.i4.4
	newarr	['.library']'System'.'Int32'
	dup
	ldc.i4.0
	ldc.i4.0
	stelem.i4
	dup
	ldc.i4.1
	ldc.i4.1
	stelem.i4
	dup
	ldc.i4.2
	ldc.i4.2
	stelem.i4
	dup
	ldc.i4.3
	ldc.i4.3
	stelem.i4
	stloc.0
	ret
	.maxstack 4
} // method test
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
.method private static hidebysig specialname rtspecialname void '.cctor'() cil managed 
{
	ldc.i4.4
	newarr	['.library']'System'.'Int32'
	dup
	ldc.i4.0
	ldc.i4.0
	stelem.i4
	dup
	ldc.i4.1
	ldc.i4.1
	stelem.i4
	dup
	ldc.i4.2
	ldc.i4.2
	stelem.i4
	dup
	ldc.i4.3
	ldc.i4.3
	stelem.i4
	stsfld	int32[] 'Test'::'x'
	ldc.i4.4
	newarr	['.library']'System'.'Int64'
	dup
	ldc.i4.0
	ldc.i4.0
	conv.i8
	stelem.i8
	dup
	ldc.i4.1
	ldc.i4.1
	conv.i8
	stelem.i8
	dup
	ldc.i4.2
	ldc.i4.2
	conv.i8
	stelem.i8
	dup
	ldc.i4.3
	ldc.i8	0x3000000000000000
	stelem.i8
	stsfld	int64[] 'Test'::'y'
	ret
	.maxstack 4
} // method .cctor
} // class Test
