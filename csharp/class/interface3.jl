.assembly extern '.library'
{
	.ver 0:0:0:0
}
.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.class public auto interface abstract ansi 'IEnumerator'
{
.method public virtual hidebysig newslot abstract instance bool 'MoveNext'() cil managed java 
{
} // method MoveNext
.method public virtual hidebysig newslot abstract instance void 'Reset'() cil managed java 
{
} // method Reset
} // class IEnumerator
.class private auto ansi 'Test' extends ['.library']'System'.'Object' implements 'IEnumerator'
{
.method public virtual hidebysig newslot instance bool 'MoveNext'() cil managed java 
{
	iconst_0
	ireturn
	.locals 1
	.maxstack 1
} // method MoveNext
.method public virtual hidebysig newslot instance void 'Reset'() cil managed java 
{
	return
	.locals 1
	.maxstack 0
} // method Reset
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed java 
{
	aload_0
	invokespecial	instance void ['.library']'System'.'Object'::'.ctor'()
	return
	.locals 1
	.maxstack 1
} // method .ctor
} // class Test
.class private auto ansi 'Test2' extends ['.library']'System'.'Object' implements 'IEnumerator'
{
.method private final virtual hidebysig newslot instance bool 'IEnumerator.MoveNext'() cil managed java 
{
	.override	'IEnumerator'::'MoveNext'
	iconst_0
	ireturn
	.locals 1
	.maxstack 1
} // method IEnumerator.MoveNext
.method private final virtual hidebysig newslot instance void 'IEnumerator.Reset'() cil managed java 
{
	.override	'IEnumerator'::'Reset'
	return
	.locals 1
	.maxstack 0
} // method IEnumerator.Reset
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed java 
{
	aload_0
	invokespecial	instance void ['.library']'System'.'Object'::'.ctor'()
	return
	.locals 1
	.maxstack 1
} // method .ctor
} // class Test2
