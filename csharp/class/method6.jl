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
.method private static hidebysig void 'm1'(class ['.library']'System'.'String' 'x', class ['.library']'System'.'Object'[] 'y') cil managed java 
{
.param[2]
.custom instance void ['.library']'System'.'ParamArrayAttribute'::'.ctor'() = (01 00 00 00)
	return
	.locals 2
	.maxstack 0
} // method m1
.method private static hidebysig void 'm1'(class ['.library']'System'.'String' 'x') cil managed java 
{
	return
	.locals 1
	.maxstack 0
} // method m1
.method private static hidebysig void 'm1'(class ['.library']'System'.'Object' 'x') cil managed java 
{
	return
	.locals 1
	.maxstack 0
} // method m1
.method private static hidebysig void 'm2'() cil managed java 
{
	ldc	"hello"
	invokestatic	void 'Test'::'m1'(class ['.library']'System'.'String')
	ldc	"hello"
	iconst_1
	anewarray class ['.library']'System'.'Object'
	dup
	iconst_0
	ldc	"there"
	aastore
	invokestatic	void 'Test'::'m1'(class ['.library']'System'.'String', class ['.library']'System'.'Object'[])
	return
	.locals 0
	.maxstack 5
} // method m2
} // class Test
