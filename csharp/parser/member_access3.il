.assembly extern '.library'
{
	.ver 0:0:0:0
}
.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.class public auto ansi 'Test' extends ['.library']'System'.'Object'
{
.field private int32 * 'f1'
.method public hidebysig instance void 'm1'() cil managed 
{
	ret
	.maxstack 0
} // method m1
} // class Test
