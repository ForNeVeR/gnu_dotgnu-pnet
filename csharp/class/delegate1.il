.assembly extern '.library'
{
	.ver 0:0:0:0
}
.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.class public auto sealed serializable ansi 'D1' extends ['.library']'System'.'MulticastDelegate'
{
.method public hidebysig specialname rtspecialname instance void '.ctor'(class ['.library']'System'.'Object' 'object', native int 'method') runtime managed {}
.method public virtual hidebysig newslot instance class ['.library']'System'.'String' 'Invoke'(int32 'x') runtime managed {}
} // class D1
.class public auto ansi 'Test' extends ['.library']'System'.'Object'
{
.method private hidebysig instance class ['.library']'System'.'String' 'm1'(class 'D1' 'd') cil managed 
{
	ldarg.1
	ldc.i4.3
	callvirt	instance class ['.library']'System'.'String' 'D1'::'Invoke'(int32)
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
