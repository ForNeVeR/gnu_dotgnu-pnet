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
.method private hidebysig instance void 'm1'(class 'Test' 'x', class 'Test'[] 'y', class 'Test'[,] 'z') cil managed 
{
	.locals	(class 'Test', class 'Test'[], class 'Test'[,])
	ldarg.1
	stloc.0
	ldarg.2
	stloc.1
	ldarg.3
	stloc.2
	ret
	.maxstack 1
} // method m1
} // class Test
