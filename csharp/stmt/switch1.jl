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
.method private hidebysig instance void 'm1'(int32 'x') cil managed java 
{
	iload_1
	dup
	iconst_0
	if_icmpeq	?L1
	dup
	iconst_1
	if_icmpeq	?L2
	dup
	iconst_2
	if_icmpeq	?L3
	iconst_3
	if_icmpeq	?L4
	goto	?L5
?L1:
	iconst_0
	istore_2
	goto	?L5
?L2:
	iconst_1
	istore_2
	goto	?L5
?L3:
	iconst_2
	istore_2
	goto	?L5
?L4:
	iconst_3
	istore_2
	goto	?L5
?L5:
	iload_1
	tableswitch (
		?L6,
		?L7,
		?L8,
		?L9,
		?L10,
		?L11,
		?L12,
		?L13
	)
	goto	?L14
?L6:
	iconst_0
	istore_2
	goto	?L14
?L7:
	iconst_1
	istore_2
	goto	?L14
?L8:
	iconst_2
	istore_2
	goto	?L14
?L9:
	iconst_3
	istore_2
	goto	?L14
?L10:
	iconst_4
	istore_2
	goto	?L14
?L11:
	iconst_5
	istore_2
	goto	?L14
?L12:
	bipush	6
	istore_2
	goto	?L14
?L13:
	bipush	7
	istore_2
	goto	?L14
?L14:
	iload_1
	iconst_1
	isub
	tableswitch (
		?L15,
		?L16,
		?L17,
		?L18,
		?L19,
		?L20,
		?L21,
		?L22
	)
	goto	?L23
?L15:
	iconst_1
	istore_2
	goto	?L23
?L16:
	iconst_2
	istore_2
	goto	?L23
?L17:
	iconst_3
	istore_2
	goto	?L23
?L18:
	iconst_4
	istore_2
	goto	?L23
?L19:
	iconst_5
	istore_2
	goto	?L23
?L20:
	bipush	6
	istore_2
	goto	?L23
?L21:
	bipush	7
	istore_2
	goto	?L23
?L22:
	bipush	8
	istore_2
	goto	?L23
?L23:
	iload_1
	lookupswitch (
		100 : ?L24,
		200 : ?L25,
		300 : ?L26,
		400 : ?L27,
		500 : ?L28,
		600 : ?L29,
		700 : ?L30,
		800 : ?L31,
		900 : ?L32,
		1000 : ?L33,
		1100 : ?L34,
		1200 : ?L35,
		1300 : ?L36,
		1400 : ?L37,
		1500 : ?L38,
		1600 : ?L39
	)
	goto	?L40
?L39:
	sipush	1600
	istore_2
	goto	?L40
?L38:
	sipush	1500
	istore_2
	goto	?L40
?L37:
	sipush	1400
	istore_2
	goto	?L40
?L36:
	sipush	1300
	istore_2
	goto	?L40
?L35:
	sipush	1200
	istore_2
	goto	?L40
?L34:
	sipush	1100
	istore_2
	goto	?L40
?L33:
	sipush	1000
	istore_2
	goto	?L40
?L32:
	sipush	900
	istore_2
	goto	?L40
?L31:
	sipush	800
	istore_2
	goto	?L40
?L30:
	sipush	700
	istore_2
	goto	?L40
?L29:
	sipush	600
	istore_2
	goto	?L40
?L28:
	sipush	500
	istore_2
	goto	?L40
?L27:
	sipush	400
	istore_2
	goto	?L40
?L26:
	sipush	300
	istore_2
	goto	?L40
?L25:
	sipush	200
	istore_2
	goto	?L40
?L24:
	bipush	100
	istore_2
	goto	?L40
?L40:
	return
	.locals 3
	.maxstack 3
} // method m1
.method private hidebysig instance void 'm2'(int64 'x') cil managed java 
{
	lload_1
	dup2
	lconst_0
	lcmp
	ifeq	?L41
	dup2
	lconst_1
	lcmp
	ifeq	?L42
	dup2
	iconst_2
	i2l
	lcmp
	ifeq	?L43
	iconst_3
	i2l
	lcmp
	ifeq	?L44
	goto	?L45
?L41:
	iconst_0
	istore_3
	goto	?L45
?L42:
	iconst_1
	istore_3
	goto	?L45
?L43:
	iconst_2
	istore_3
	goto	?L45
?L44:
	iconst_3
	istore_3
	goto	?L45
?L45:
	lload_1
	dup2
	iconst_4
	i2l
	lcmp
	ifge	?L46
	dup2
	lconst_0
	lcmp
	ifeq	?L47
	dup2
	lconst_1
	lcmp
	ifeq	?L48
	dup2
	iconst_2
	i2l
	lcmp
	ifeq	?L49
	iconst_3
	i2l
	lcmp
	ifeq	?L50
	goto	?L51
?L46:
	dup2
	iconst_4
	i2l
	lcmp
	ifeq	?L52
	dup2
	iconst_5
	i2l
	lcmp
	ifeq	?L53
	dup2
	bipush	6
	i2l
	lcmp
	ifeq	?L54
	bipush	7
	i2l
	lcmp
	ifeq	?L55
	goto	?L51
?L47:
	iconst_0
	istore_3
	goto	?L51
?L48:
	iconst_1
	istore_3
	goto	?L51
?L49:
	iconst_2
	istore_3
	goto	?L51
?L50:
	iconst_3
	istore_3
	goto	?L51
?L52:
	iconst_4
	istore_3
	goto	?L51
?L53:
	iconst_5
	istore_3
	goto	?L51
?L54:
	bipush	6
	istore_3
	goto	?L51
?L55:
	bipush	7
	istore_3
	goto	?L51
?L51:
	return
	.locals 4
	.maxstack 6
} // method m2
.method private hidebysig instance void 'm3'(class ['.library']'System'.'String' 'x') cil managed java 
{
	aload_1
	dup
	ifnull	?L56
	dup
	ldc	"1"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "op_Equality" "(LSystem/String;LSystem/String;)Z"
	ifne	?L57
	dup
	ldc	"2"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "op_Equality" "(LSystem/String;LSystem/String;)Z"
	ifne	?L58
	ldc	"3"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "op_Equality" "(LSystem/String;LSystem/String;)Z"
	ifne	?L59
	goto	?L60
?L56:
	iconst_0
	istore_2
	goto	?L60
?L57:
	iconst_1
	istore_2
	goto	?L60
?L58:
	iconst_2
	istore_2
	goto	?L60
?L59:
	iconst_3
	istore_2
	goto	?L60
?L60:
	aload_1
	dup
	ldc	"4"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "CompareOrdinal" "(LSystem/String;LSystem/String;)I"
	ifge	?L61
	dup
	ifnull	?L62
	dup
	ldc	"1"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "op_Equality" "(LSystem/String;LSystem/String;)Z"
	ifne	?L63
	dup
	ldc	"2"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "op_Equality" "(LSystem/String;LSystem/String;)Z"
	ifne	?L64
	ldc	"3"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "op_Equality" "(LSystem/String;LSystem/String;)Z"
	ifne	?L65
	goto	?L66
?L61:
	dup
	ldc	"4"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "op_Equality" "(LSystem/String;LSystem/String;)Z"
	ifne	?L67
	dup
	ldc	"5"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "op_Equality" "(LSystem/String;LSystem/String;)Z"
	ifne	?L68
	dup
	ldc	"6"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "op_Equality" "(LSystem/String;LSystem/String;)Z"
	ifne	?L69
	ldc	"7"
	invokestatic	"System/String" "__FromJavaString" "(Ljava/lang/String;)LSystem/String;"
	invokestatic	"System/String" "op_Equality" "(LSystem/String;LSystem/String;)Z"
	ifne	?L70
	goto	?L66
?L70:
	bipush	7
	istore_2
	goto	?L66
?L69:
	bipush	6
	istore_2
	goto	?L66
?L68:
	iconst_5
	istore_2
	goto	?L66
?L67:
	iconst_4
	istore_2
	goto	?L66
?L65:
	iconst_3
	istore_2
	goto	?L66
?L64:
	iconst_2
	istore_2
	goto	?L66
?L63:
	iconst_1
	istore_2
	goto	?L66
?L62:
	iconst_0
	istore_2
	goto	?L66
?L66:
	return
	.locals 3
	.maxstack 3
} // method m3
.method private hidebysig instance void 'm4'(valuetype 'Color' 'x') cil managed java 
{
	iload_1
	dup
	iconst_0
	if_icmpeq	?L71
	dup
	iconst_1
	if_icmpeq	?L72
	dup
	iconst_2
	if_icmpeq	?L73
	iconst_3
	if_icmpeq	?L74
	goto	?L75
?L71:
	iconst_0
	istore_2
	goto	?L75
?L72:
	iconst_1
	istore_2
	goto	?L75
?L73:
	iconst_2
	istore_2
	goto	?L75
?L74:
	iconst_3
	istore_2
	goto	?L75
?L75:
	iload_1
	tableswitch (
		?L76,
		?L77,
		?L78,
		?L79,
		?L80,
		?L81,
		?L82,
		?L83
	)
	goto	?L84
?L76:
	iconst_0
	istore_2
	goto	?L84
?L77:
	iconst_1
	istore_2
	goto	?L84
?L78:
	iconst_2
	istore_2
	goto	?L84
?L79:
	iconst_3
	istore_2
	goto	?L84
?L80:
	iconst_4
	istore_2
	goto	?L84
?L81:
	iconst_5
	istore_2
	goto	?L84
?L82:
	bipush	6
	istore_2
	goto	?L84
?L83:
	bipush	7
	istore_2
	goto	?L84
?L84:
	return
	.locals 3
	.maxstack 3
} // method m4
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed java 
{
	aload_0
	invokespecial	instance void ['.library']'System'.'Object'::'.ctor'()
	return
	.locals 1
	.maxstack 1
} // method .ctor
} // class Test
