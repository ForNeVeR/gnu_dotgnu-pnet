.assembly extern '.library'
{
	.ver 0:0:0:0
}
.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.class private auto sealed serializable ansi 'Color' extends ['.library']'System'.'Enum'
{
.field public static literal valuetype 'Color' 'Red' = int32(0x00000000)
.field public static literal valuetype 'Color' 'Green' = int32(0x00000001)
.field public static literal valuetype 'Color' 'Blue' = int32(0x00000002)
.field public static literal valuetype 'Color' 'Yellow' = int32(0x00000003)
.field public static literal valuetype 'Color' 'Cyan' = int32(0x00000004)
.field public static literal valuetype 'Color' 'Magenta' = int32(0x00000005)
.field public static literal valuetype 'Color' 'Black' = int32(0x00000006)
.field public static literal valuetype 'Color' 'White' = int32(0x00000007)
.field public specialname rtspecialname int32 'value__'
} // class Color
.class private auto ansi 'Test' extends ['.library']'System'.'Object'
{
.method private hidebysig instance void 'm1'(int32 'x') cil managed 
{
	.locals	(int32)
	ldarg.1
	dup
	ldc.i4.0
	beq	?L1
	dup
	ldc.i4.1
	beq	?L2
	dup
	ldc.i4.2
	beq	?L3
	ldc.i4.3
	beq	?L4
	br	?L5
?L1:
	ldc.i4.0
	stloc.0
	br	?L5
?L2:
	ldc.i4.1
	stloc.0
	br	?L5
?L3:
	ldc.i4.2
	stloc.0
	br	?L5
?L4:
	ldc.i4.3
	stloc.0
	br	?L5
?L5:
	ldarg.1
	switch (
		?L6,
		?L7,
		?L8,
		?L9,
		?L10,
		?L11,
		?L12,
		?L13
	)
	br	?L14
?L6:
	ldc.i4.0
	stloc.0
	br	?L14
?L7:
	ldc.i4.1
	stloc.0
	br	?L14
?L8:
	ldc.i4.2
	stloc.0
	br	?L14
?L9:
	ldc.i4.3
	stloc.0
	br	?L14
?L10:
	ldc.i4.4
	stloc.0
	br	?L14
?L11:
	ldc.i4.5
	stloc.0
	br	?L14
?L12:
	ldc.i4.6
	stloc.0
	br	?L14
?L13:
	ldc.i4.7
	stloc.0
	br	?L14
?L14:
	ldarg.1
	ldc.i4.1
	sub
	switch (
		?L15,
		?L16,
		?L17,
		?L18,
		?L19,
		?L20,
		?L21,
		?L22
	)
	br	?L23
?L15:
	ldc.i4.1
	stloc.0
	br	?L23
?L16:
	ldc.i4.2
	stloc.0
	br	?L23
?L17:
	ldc.i4.3
	stloc.0
	br	?L23
?L18:
	ldc.i4.4
	stloc.0
	br	?L23
?L19:
	ldc.i4.5
	stloc.0
	br	?L23
?L20:
	ldc.i4.6
	stloc.0
	br	?L23
?L21:
	ldc.i4.7
	stloc.0
	br	?L23
?L22:
	ldc.i4.8
	stloc.0
	br	?L23
?L23:
	ldarg.1
	dup
	ldc.i4	900
	bge	?L24
	dup
	ldc.i4	500
	bge	?L25
	dup
	ldc.i4.s	100
	beq	?L26
	dup
	ldc.i4	200
	beq	?L27
	dup
	ldc.i4	300
	beq	?L28
	ldc.i4	400
	beq	?L29
	br	?L30
?L25:
	dup
	ldc.i4	500
	beq	?L31
	dup
	ldc.i4	600
	beq	?L32
	dup
	ldc.i4	700
	beq	?L33
	ldc.i4	800
	beq	?L34
	br	?L30
?L24:
	dup
	ldc.i4	1300
	bge	?L35
	dup
	ldc.i4	900
	beq	?L36
	dup
	ldc.i4	1000
	beq	?L37
	dup
	ldc.i4	1100
	beq	?L38
	ldc.i4	1200
	beq	?L39
	br	?L30
?L35:
	dup
	ldc.i4	1300
	beq	?L40
	dup
	ldc.i4	1400
	beq	?L41
	dup
	ldc.i4	1500
	beq	?L42
	ldc.i4	1600
	beq	?L43
	br	?L30
?L43:
	ldc.i4	1600
	stloc.0
	br	?L30
?L42:
	ldc.i4	1500
	stloc.0
	br	?L30
?L41:
	ldc.i4	1400
	stloc.0
	br	?L30
?L40:
	ldc.i4	1300
	stloc.0
	br	?L30
?L39:
	ldc.i4	1200
	stloc.0
	br	?L30
?L38:
	ldc.i4	1100
	stloc.0
	br	?L30
?L37:
	ldc.i4	1000
	stloc.0
	br	?L30
?L36:
	ldc.i4	900
	stloc.0
	br	?L30
?L34:
	ldc.i4	800
	stloc.0
	br	?L30
?L33:
	ldc.i4	700
	stloc.0
	br	?L30
?L32:
	ldc.i4	600
	stloc.0
	br	?L30
?L31:
	ldc.i4	500
	stloc.0
	br	?L30
?L29:
	ldc.i4	400
	stloc.0
	br	?L30
?L28:
	ldc.i4	300
	stloc.0
	br	?L30
?L27:
	ldc.i4	200
	stloc.0
	br	?L30
?L26:
	ldc.i4.s	100
	stloc.0
	br	?L30
?L30:
	ret
	.maxstack 3
} // method m1
.method private hidebysig instance void 'm2'(int64 'x') cil managed 
{
	.locals	(int32)
	ldarg.1
	dup
	ldc.i4.0
	conv.i8
	beq	?L44
	dup
	ldc.i4.1
	conv.i8
	beq	?L45
	dup
	ldc.i4.2
	conv.i8
	beq	?L46
	ldc.i4.3
	conv.i8
	beq	?L47
	br	?L48
?L44:
	ldc.i4.0
	stloc.0
	br	?L48
?L45:
	ldc.i4.1
	stloc.0
	br	?L48
?L46:
	ldc.i4.2
	stloc.0
	br	?L48
?L47:
	ldc.i4.3
	stloc.0
	br	?L48
?L48:
	ldarg.1
	dup
	ldc.i4.4
	conv.i8
	bge	?L49
	dup
	ldc.i4.0
	conv.i8
	beq	?L50
	dup
	ldc.i4.1
	conv.i8
	beq	?L51
	dup
	ldc.i4.2
	conv.i8
	beq	?L52
	ldc.i4.3
	conv.i8
	beq	?L53
	br	?L54
?L49:
	dup
	ldc.i4.4
	conv.i8
	beq	?L55
	dup
	ldc.i4.5
	conv.i8
	beq	?L56
	dup
	ldc.i4.6
	conv.i8
	beq	?L57
	ldc.i4.7
	conv.i8
	beq	?L58
	br	?L54
?L50:
	ldc.i4.0
	stloc.0
	br	?L54
?L51:
	ldc.i4.1
	stloc.0
	br	?L54
?L52:
	ldc.i4.2
	stloc.0
	br	?L54
?L53:
	ldc.i4.3
	stloc.0
	br	?L54
?L55:
	ldc.i4.4
	stloc.0
	br	?L54
?L56:
	ldc.i4.5
	stloc.0
	br	?L54
?L57:
	ldc.i4.6
	stloc.0
	br	?L54
?L58:
	ldc.i4.7
	stloc.0
	br	?L54
?L54:
	ret
	.maxstack 3
} // method m2
.method private hidebysig instance void 'm3'(class ['.library']'System'.'String' 'x') cil managed 
{
	.locals	(int32)
	ldarg.1
	dup
	brfalse	?L59
	dup
	ldstr	"1"
	call	bool [.library]System.String::op_Equality(class [.library]System.String, class [.library]System.String)
	brtrue	?L60
	dup
	ldstr	"2"
	call	bool [.library]System.String::op_Equality(class [.library]System.String, class [.library]System.String)
	brtrue	?L61
	ldstr	"3"
	call	bool [.library]System.String::op_Equality(class [.library]System.String, class [.library]System.String)
	brtrue	?L62
	br	?L63
?L59:
	ldc.i4.0
	stloc.0
	br	?L63
?L60:
	ldc.i4.1
	stloc.0
	br	?L63
?L61:
	ldc.i4.2
	stloc.0
	br	?L63
?L62:
	ldc.i4.3
	stloc.0
	br	?L63
?L63:
	ldarg.1
	dup
	ldstr	"4"
	call	int32 [.library]System.String::CompareOrdinal(class [.library]System.String, class [.library]System.String)
	ldc.i4.0
	bge	?L64
	dup
	brfalse	?L65
	dup
	ldstr	"1"
	call	bool [.library]System.String::op_Equality(class [.library]System.String, class [.library]System.String)
	brtrue	?L66
	dup
	ldstr	"2"
	call	bool [.library]System.String::op_Equality(class [.library]System.String, class [.library]System.String)
	brtrue	?L67
	ldstr	"3"
	call	bool [.library]System.String::op_Equality(class [.library]System.String, class [.library]System.String)
	brtrue	?L68
	br	?L69
?L64:
	dup
	ldstr	"4"
	call	bool [.library]System.String::op_Equality(class [.library]System.String, class [.library]System.String)
	brtrue	?L70
	dup
	ldstr	"5"
	call	bool [.library]System.String::op_Equality(class [.library]System.String, class [.library]System.String)
	brtrue	?L71
	dup
	ldstr	"6"
	call	bool [.library]System.String::op_Equality(class [.library]System.String, class [.library]System.String)
	brtrue	?L72
	ldstr	"7"
	call	bool [.library]System.String::op_Equality(class [.library]System.String, class [.library]System.String)
	brtrue	?L73
	br	?L69
?L73:
	ldc.i4.7
	stloc.0
	br	?L69
?L72:
	ldc.i4.6
	stloc.0
	br	?L69
?L71:
	ldc.i4.5
	stloc.0
	br	?L69
?L70:
	ldc.i4.4
	stloc.0
	br	?L69
?L68:
	ldc.i4.3
	stloc.0
	br	?L69
?L67:
	ldc.i4.2
	stloc.0
	br	?L69
?L66:
	ldc.i4.1
	stloc.0
	br	?L69
?L65:
	ldc.i4.0
	stloc.0
	br	?L69
?L69:
	ret
	.maxstack 3
} // method m3
.method private hidebysig instance void 'm4'(valuetype 'Color' 'x') cil managed 
{
	.locals	(int32)
	ldarg.1
	dup
	ldc.i4.0
	beq	?L74
	dup
	ldc.i4.1
	beq	?L75
	dup
	ldc.i4.2
	beq	?L76
	ldc.i4.3
	beq	?L77
	br	?L78
?L74:
	ldc.i4.0
	stloc.0
	br	?L78
?L75:
	ldc.i4.1
	stloc.0
	br	?L78
?L76:
	ldc.i4.2
	stloc.0
	br	?L78
?L77:
	ldc.i4.3
	stloc.0
	br	?L78
?L78:
	ldarg.1
	switch (
		?L79,
		?L80,
		?L81,
		?L82,
		?L83,
		?L84,
		?L85,
		?L86
	)
	br	?L87
?L79:
	ldc.i4.0
	stloc.0
	br	?L87
?L80:
	ldc.i4.1
	stloc.0
	br	?L87
?L81:
	ldc.i4.2
	stloc.0
	br	?L87
?L82:
	ldc.i4.3
	stloc.0
	br	?L87
?L83:
	ldc.i4.4
	stloc.0
	br	?L87
?L84:
	ldc.i4.5
	stloc.0
	br	?L87
?L85:
	ldc.i4.6
	stloc.0
	br	?L87
?L86:
	ldc.i4.7
	stloc.0
	br	?L87
?L87:
	ret
	.maxstack 3
} // method m4
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class Test
