.assembly extern '.library'
{
	.ver 0:0:0:0
}
.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.namespace 'TestNS'
{
.class public auto ansi 'TestAccess' extends ['.library']'System'.'Object'
{
.class nested public auto ansi 'PublicTest' extends ['.library']'System'.'Object'
{
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class PublicTest
.class nested famorassem auto ansi 'ProtectedTest' extends ['.library']'System'.'Object'
{
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class ProtectedTest
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class TestAccess
} // namespace TestNS
.namespace 'TestNS'
{
.class public auto ansi 'Test' extends ['.library']'System'.'Object'
{
.class nested public auto ansi 'TestPublic' extends 'TestNS'.'TestAccess'/'PublicTest'
{
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void 'TestNS'.'TestAccess'/'PublicTest'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class TestPublic
.class nested public auto ansi 'TestProtected' extends 'TestNS'.'TestAccess'/'ProtectedTest'
{
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void 'TestNS'.'TestAccess'/'ProtectedTest'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class TestProtected
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class Test
} // namespace TestNS
