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
.method public virtual hidebysig abstract instance void 'm1'() cil managed 
{
} // method m1
} // class I
.class private auto interface ansi 'J' implements 'I'
{
.method public virtual hidebysig abstract instance void 'm2'() cil managed 
{
} // method m2
} // class J
.class private auto interface ansi 'K' implements 'J', 'I'
{
.method public virtual hidebysig abstract instance void 'm3'() cil managed 
{
} // method m3
} // class K
.class private auto ansi 'Test' extends ['.library']'System'.'Object'
{
.method public hidebysig instance void 'm1'() cil managed 
{
	.locals	(class ['.library']'System'.'Object', class 'Test', class 'Test2', class 'I', class 'J', class 'K', class ['.library']'System'.'Object'[], class 'Test'[], class 'I'[])
	ldnull
	stloc.0
	ldnull
	stloc.1
	ldnull
	stloc.3
	ldnull
	stloc.s	6
	ldnull
	stloc.s	7
	ldnull
	stloc.s	8
	ldloc.1
	stloc.0
	ldloc.2
	stloc.1
	ldloc.0
	castclass	'Test'
	stloc.1
	ldloc.0
	castclass	'Test2'
	stloc.2
	ldloc.1
	castclass	'Test2'
	stloc.2
	ldloc.2
	stloc.3
	ldloc.s	4
	stloc.3
	ldloc.s	5
	stloc.3
	ldloc.s	5
	stloc.s	4
	ret
	.maxstack 1
} // method m1
} // class Test
.class private auto ansi 'Test2' extends 'Test' implements 'I'
{
} // class Test2
