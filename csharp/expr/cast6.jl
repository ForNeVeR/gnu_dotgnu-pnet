.assembly extern '.library'
{
	.ver 0:0:0:0
}
.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.class private auto interface ansi 'I'
{
.method public virtual hidebysig abstract instance void 'm1'() cil managed java 
{
} // method m1
} // class I
.class private auto interface ansi 'J' implements 'I'
{
.method public virtual hidebysig abstract instance void 'm2'() cil managed java 
{
} // method m2
} // class J
.class private auto interface ansi 'K' implements 'J', 'I'
{
.method public virtual hidebysig abstract instance void 'm3'() cil managed java 
{
} // method m3
} // class K
.class private auto ansi 'Test' extends ['.library']'System'.'Object'
{
.method public hidebysig instance void 'm1'() cil managed java 
{
	aconst_null
	astore_1
	aconst_null
	astore_2
	aconst_null
	astore	4
	aconst_null
	astore	7
	aconst_null
	astore	8
	aconst_null
	astore	9
	aload_2
	astore_1
	aload_3
	astore_2
	aload_1
	checkcast	'Test'
	astore_2
	aload_1
	checkcast	'Test2'
	astore_3
	aload_2
	checkcast	'Test2'
	astore_3
	aload_3
	astore	4
	aload	5
	astore	4
	aload	6
	astore	4
	aload	6
	astore	5
	return
	.locals 10
	.maxstack 1
} // method m1
} // class Test
.class private auto ansi 'Test2' extends 'Test' implements 'I'
{
} // class Test2
