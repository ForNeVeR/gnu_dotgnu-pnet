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
.method private hidebysig instance void 'm1'() cil managed 
{
	.locals init	(bool, int8, unsigned int8, int16, unsigned int16, int32, unsigned int32, int64, unsigned int64, char, float32, float64, valuetype ['.library']'System'.'Decimal', class ['.library']'System'.'Object', int8, unsigned int8, int16, unsigned int16, int32, unsigned int32, int64, unsigned int64, char, float32, float64, valuetype ['.library']'System'.'Decimal', class ['.library']'System'.'Object')
	ldc.i4.s	34
	conv.i1
	stloc.1
	ldc.i4.s	42
	conv.u1
	stloc.2
	ldc.i4.s	-126
	conv.i2
	stloc.3
	ldc.i4.s	67
	conv.u2
	stloc.s	4
	ldc.i4	4294966062
	stloc.s	5
	ldc.i4	54321
	stloc.s	6
	ldc.i8	0xfffffffdb34fe916
	stloc.s	7
	ldc.i8	0x000000024cb016ea
	stloc.s	8
	ldc.i4.s	65
	stloc.s	9
	ldc.r4	float32(0x3FC00000)
	stloc.s	10
	ldc.r8	float64(0x401ACCCCCCCCCCCD)
	stloc.s	11
	ldc.i4.s	35
	ldc.i4.0
	ldc.i4.0
	ldc.i4.0
	ldc.i4.1
	newobj	instance void [.library]System.Decimal::.ctor(int32, int32, int32, bool, unsigned int8)
	stloc.s	12
	ldnull
	stloc.s	13
	ldc.i4.s	34
	conv.i1
	stloc.s	14
	ldc.i4.s	42
	conv.u1
	stloc.s	15
	ldc.i4.s	-126
	conv.i2
	stloc.s	16
	ldc.i4.s	67
	conv.u2
	stloc.s	17
	ldc.i4	4294966062
	stloc.s	18
	ldc.i4	54321
	stloc.s	19
	ldc.i8	0xfffffffdb34fe916
	stloc.s	20
	ldc.i8	0x000000024cb016ea
	stloc.s	21
	ldc.i4.s	65
	stloc.s	22
	ldc.r4	float32(0x3FC00000)
	stloc.s	23
	ldc.r8	float64(0x401ACCCCCCCCCCCD)
	stloc.s	24
	ldc.i4.s	35
	ldc.i4.0
	ldc.i4.0
	ldc.i4.0
	ldc.i4.1
	newobj	instance void [.library]System.Decimal::.ctor(int32, int32, int32, bool, unsigned int8)
	stloc.s	25
	ldnull
	stloc.s	26
	ldloc.1
	ldloc.s	14
	ceq
	stloc.0
	ldloc.1
	ldloc.s	15
	ceq
	stloc.0
	ldloc.1
	ldloc.s	16
	ceq
	stloc.0
	ldloc.1
	ldloc.s	17
	ceq
	stloc.0
	ldloc.1
	ldloc.s	18
	ceq
	stloc.0
	ldloc.1
	conv.i8
	ldloc.s	19
	conv.u8
	ceq
	stloc.0
	ldloc.1
	conv.i8
	ldloc.s	20
	ceq
	stloc.0
	ldloc.1
	ldloc.s	22
	ceq
	stloc.0
	ldloc.1
	conv.r4
	ldloc.s	23
	beq	?L1
	ldc.i4.0
	br	?L2
?L1:
	ldc.i4.1
?L2:
	stloc.0
	ldloc.1
	conv.r8
	ldloc.s	24
	beq	?L3
	ldc.i4.0
	br	?L4
?L3:
	ldc.i4.1
?L4:
	stloc.0
	ldloc.1
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int8)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Equality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.2
	ldloc.s	14
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.2
	ldloc.s	15
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.2
	ldloc.s	16
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.2
	ldloc.s	17
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.2
	ldloc.s	18
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.2
	ldloc.s	19
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.2
	conv.i8
	ldloc.s	20
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.2
	conv.u8
	ldloc.s	21
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.2
	ldloc.s	22
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.2
	conv.r.un
	ldloc.s	23
	bne.un	?L5
	ldc.i4.0
	br	?L6
?L5:
	ldc.i4.1
?L6:
	stloc.0
	ldloc.2
	conv.r.un
	ldloc.s	24
	bne.un	?L7
	ldc.i4.0
	br	?L8
?L7:
	ldc.i4.1
?L8:
	stloc.0
	ldloc.2
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int8)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Inequality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.3
	ldloc.s	14
	clt
	stloc.0
	ldloc.3
	ldloc.s	15
	clt
	stloc.0
	ldloc.3
	ldloc.s	16
	clt
	stloc.0
	ldloc.3
	ldloc.s	17
	clt
	stloc.0
	ldloc.3
	ldloc.s	18
	clt
	stloc.0
	ldloc.3
	conv.i8
	ldloc.s	19
	conv.u8
	clt
	stloc.0
	ldloc.3
	conv.i8
	ldloc.s	20
	clt
	stloc.0
	ldloc.3
	ldloc.s	22
	clt
	stloc.0
	ldloc.3
	conv.r4
	ldloc.s	23
	blt	?L9
	ldc.i4.0
	br	?L10
?L9:
	ldc.i4.1
?L10:
	stloc.0
	ldloc.3
	conv.r8
	ldloc.s	24
	blt	?L11
	ldc.i4.0
	br	?L12
?L11:
	ldc.i4.1
?L12:
	stloc.0
	ldloc.3
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int16)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_LessThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	4
	ldloc.s	14
	cgt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	4
	ldloc.s	15
	cgt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	4
	ldloc.s	16
	cgt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	4
	ldloc.s	17
	cgt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	4
	ldloc.s	18
	cgt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	4
	ldloc.s	19
	cgt.un
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	4
	conv.i8
	ldloc.s	20
	cgt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	4
	conv.u8
	ldloc.s	21
	cgt.un
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	4
	ldloc.s	22
	cgt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	4
	conv.r.un
	ldloc.s	23
	ble	?L13
	ldc.i4.0
	br	?L14
?L13:
	ldc.i4.1
?L14:
	stloc.0
	ldloc.s	4
	conv.r.un
	ldloc.s	24
	ble	?L15
	ldc.i4.0
	br	?L16
?L15:
	ldc.i4.1
?L16:
	stloc.0
	ldloc.s	4
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int16)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_LessThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	5
	ldloc.s	14
	cgt
	stloc.0
	ldloc.s	5
	ldloc.s	15
	cgt
	stloc.0
	ldloc.s	5
	ldloc.s	16
	cgt
	stloc.0
	ldloc.s	5
	ldloc.s	17
	cgt
	stloc.0
	ldloc.s	5
	ldloc.s	18
	cgt
	stloc.0
	ldloc.s	5
	conv.i8
	ldloc.s	19
	conv.u8
	cgt
	stloc.0
	ldloc.s	5
	conv.i8
	ldloc.s	20
	cgt
	stloc.0
	ldloc.s	5
	ldloc.s	22
	cgt
	stloc.0
	ldloc.s	5
	conv.r4
	ldloc.s	23
	bgt	?L17
	ldc.i4.0
	br	?L18
?L17:
	ldc.i4.1
?L18:
	stloc.0
	ldloc.s	5
	conv.r8
	ldloc.s	24
	bgt	?L19
	ldc.i4.0
	br	?L20
?L19:
	ldc.i4.1
?L20:
	stloc.0
	ldloc.s	5
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int32)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	6
	conv.u8
	ldloc.s	14
	conv.i8
	clt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	6
	ldloc.s	15
	clt.un
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	6
	conv.u8
	ldloc.s	16
	conv.i8
	clt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	6
	ldloc.s	17
	clt.un
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	6
	conv.u8
	ldloc.s	18
	conv.i8
	clt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	6
	ldloc.s	19
	clt.un
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	6
	conv.u8
	ldloc.s	20
	clt
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	6
	conv.u8
	ldloc.s	21
	clt.un
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	6
	ldloc.s	22
	clt.un
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	6
	conv.r.un
	conv.r4
	ldloc.s	23
	bge	?L21
	ldc.i4.0
	br	?L22
?L21:
	ldc.i4.1
?L22:
	stloc.0
	ldloc.s	6
	conv.r.un
	ldloc.s	24
	bge	?L23
	ldc.i4.0
	br	?L24
?L23:
	ldc.i4.1
?L24:
	stloc.0
	ldloc.s	6
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int32)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	7
	ldloc.s	14
	conv.i8
	ceq
	stloc.0
	ldloc.s	7
	ldloc.s	15
	conv.i8
	ceq
	stloc.0
	ldloc.s	7
	ldloc.s	16
	conv.i8
	ceq
	stloc.0
	ldloc.s	7
	ldloc.s	17
	conv.i8
	ceq
	stloc.0
	ldloc.s	7
	ldloc.s	18
	conv.i8
	ceq
	stloc.0
	ldloc.s	7
	ldloc.s	19
	conv.u8
	ceq
	stloc.0
	ldloc.s	7
	ldloc.s	20
	ceq
	stloc.0
	ldloc.s	7
	ldloc.s	22
	conv.i8
	ceq
	stloc.0
	ldloc.s	7
	conv.r4
	ldloc.s	23
	beq	?L25
	ldc.i4.0
	br	?L26
?L25:
	ldc.i4.1
?L26:
	stloc.0
	ldloc.s	7
	conv.r8
	ldloc.s	24
	beq	?L27
	ldc.i4.0
	br	?L28
?L27:
	ldc.i4.1
?L28:
	stloc.0
	ldloc.s	7
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int64)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Equality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	8
	ldloc.s	15
	conv.u8
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	8
	ldloc.s	17
	conv.u8
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	8
	ldloc.s	19
	conv.u8
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	8
	ldloc.s	21
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	8
	ldloc.s	22
	conv.u8
	ceq
	ldc.i4.1
	xor
	stloc.0
	ldloc.s	8
	conv.r.un
	conv.r4
	ldloc.s	23
	bne.un	?L29
	ldc.i4.0
	br	?L30
?L29:
	ldc.i4.1
?L30:
	stloc.0
	ldloc.s	8
	conv.r.un
	conv.r8
	ldloc.s	24
	bne.un	?L31
	ldc.i4.0
	br	?L32
?L31:
	ldc.i4.1
?L32:
	stloc.0
	ldloc.s	8
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int64)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Inequality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	10
	ldloc.s	14
	conv.r4
	blt	?L33
	ldc.i4.0
	br	?L34
?L33:
	ldc.i4.1
?L34:
	stloc.0
	ldloc.s	10
	ldloc.s	15
	conv.r.un
	blt	?L35
	ldc.i4.0
	br	?L36
?L35:
	ldc.i4.1
?L36:
	stloc.0
	ldloc.s	10
	ldloc.s	16
	conv.r4
	blt	?L37
	ldc.i4.0
	br	?L38
?L37:
	ldc.i4.1
?L38:
	stloc.0
	ldloc.s	10
	ldloc.s	17
	conv.r.un
	blt	?L39
	ldc.i4.0
	br	?L40
?L39:
	ldc.i4.1
?L40:
	stloc.0
	ldloc.s	10
	ldloc.s	18
	conv.r4
	blt	?L41
	ldc.i4.0
	br	?L42
?L41:
	ldc.i4.1
?L42:
	stloc.0
	ldloc.s	10
	ldloc.s	19
	conv.r.un
	conv.r4
	blt	?L43
	ldc.i4.0
	br	?L44
?L43:
	ldc.i4.1
?L44:
	stloc.0
	ldloc.s	10
	ldloc.s	20
	conv.r4
	blt	?L45
	ldc.i4.0
	br	?L46
?L45:
	ldc.i4.1
?L46:
	stloc.0
	ldloc.s	10
	ldloc.s	21
	conv.r.un
	conv.r4
	blt	?L47
	ldc.i4.0
	br	?L48
?L47:
	ldc.i4.1
?L48:
	stloc.0
	ldloc.s	10
	ldloc.s	22
	conv.r.un
	blt	?L49
	ldc.i4.0
	br	?L50
?L49:
	ldc.i4.1
?L50:
	stloc.0
	ldloc.s	10
	ldloc.s	23
	blt	?L51
	ldc.i4.0
	br	?L52
?L51:
	ldc.i4.1
?L52:
	stloc.0
	ldloc.s	10
	ldloc.s	24
	blt	?L53
	ldc.i4.0
	br	?L54
?L53:
	ldc.i4.1
?L54:
	stloc.0
	ldloc.s	11
	ldloc.s	14
	conv.r8
	ble	?L55
	ldc.i4.0
	br	?L56
?L55:
	ldc.i4.1
?L56:
	stloc.0
	ldloc.s	11
	ldloc.s	15
	conv.r.un
	ble	?L57
	ldc.i4.0
	br	?L58
?L57:
	ldc.i4.1
?L58:
	stloc.0
	ldloc.s	11
	ldloc.s	16
	conv.r8
	ble	?L59
	ldc.i4.0
	br	?L60
?L59:
	ldc.i4.1
?L60:
	stloc.0
	ldloc.s	11
	ldloc.s	17
	conv.r.un
	ble	?L61
	ldc.i4.0
	br	?L62
?L61:
	ldc.i4.1
?L62:
	stloc.0
	ldloc.s	11
	ldloc.s	18
	conv.r8
	ble	?L63
	ldc.i4.0
	br	?L64
?L63:
	ldc.i4.1
?L64:
	stloc.0
	ldloc.s	11
	ldloc.s	19
	conv.r.un
	ble	?L65
	ldc.i4.0
	br	?L66
?L65:
	ldc.i4.1
?L66:
	stloc.0
	ldloc.s	11
	ldloc.s	20
	conv.r8
	ble	?L67
	ldc.i4.0
	br	?L68
?L67:
	ldc.i4.1
?L68:
	stloc.0
	ldloc.s	11
	ldloc.s	21
	conv.r.un
	conv.r8
	ble	?L69
	ldc.i4.0
	br	?L70
?L69:
	ldc.i4.1
?L70:
	stloc.0
	ldloc.s	11
	ldloc.s	22
	conv.r.un
	ble	?L71
	ldc.i4.0
	br	?L72
?L71:
	ldc.i4.1
?L72:
	stloc.0
	ldloc.s	11
	ldloc.s	23
	ble	?L73
	ldc.i4.0
	br	?L74
?L73:
	ldc.i4.1
?L74:
	stloc.0
	ldloc.s	11
	ldloc.s	24
	ble	?L75
	ldc.i4.0
	br	?L76
?L75:
	ldc.i4.1
?L76:
	stloc.0
	ldloc.s	12
	ldloc.s	14
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int8)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	12
	ldloc.s	15
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int8)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	12
	ldloc.s	16
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int16)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	12
	ldloc.s	17
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int16)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	12
	ldloc.s	18
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int32)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	12
	ldloc.s	19
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int32)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	12
	ldloc.s	20
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int64)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	12
	ldloc.s	21
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int64)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	12
	ldloc.s	22
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(char)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	12
	ldloc.s	25
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	stloc.0
	ldloc.s	13
	ldloc.s	26
	ceq
	stloc.0
	ldloc.s	13
	ldloc.s	26
	ceq
	ldc.i4.1
	xor
	stloc.0
	br	?L77
?L78:
	ldc.i4.1
	stloc.0
?L77:
	ldloc.1
	ldloc.s	14
	beq	?L78
?L79:
	br	?L80
?L81:
	ldc.i4.1
	stloc.0
?L80:
	ldloc.1
	ldloc.s	15
	beq	?L81
?L82:
	br	?L83
?L84:
	ldc.i4.1
	stloc.0
?L83:
	ldloc.1
	ldloc.s	16
	beq	?L84
?L85:
	br	?L86
?L87:
	ldc.i4.1
	stloc.0
?L86:
	ldloc.1
	ldloc.s	17
	beq	?L87
?L88:
	br	?L89
?L90:
	ldc.i4.1
	stloc.0
?L89:
	ldloc.1
	ldloc.s	18
	beq	?L90
?L91:
	br	?L92
?L93:
	ldc.i4.1
	stloc.0
?L92:
	ldloc.1
	conv.i8
	ldloc.s	19
	conv.u8
	beq	?L93
?L94:
	br	?L95
?L96:
	ldc.i4.1
	stloc.0
?L95:
	ldloc.1
	conv.i8
	ldloc.s	20
	beq	?L96
?L97:
	br	?L98
?L99:
	ldc.i4.1
	stloc.0
?L98:
	ldloc.1
	ldloc.s	22
	beq	?L99
?L100:
	br	?L101
?L102:
	ldc.i4.1
	stloc.0
?L101:
	ldloc.1
	conv.r4
	ldloc.s	23
	beq	?L102
?L103:
	br	?L104
?L105:
	ldc.i4.1
	stloc.0
?L104:
	ldloc.1
	conv.r8
	ldloc.s	24
	beq	?L105
?L106:
	br	?L107
?L108:
	ldc.i4.1
	stloc.0
?L107:
	ldloc.1
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int8)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Equality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L108
?L109:
	br	?L110
?L111:
	ldc.i4.1
	stloc.0
?L110:
	ldloc.2
	ldloc.s	14
	bne.un	?L111
?L112:
	br	?L113
?L114:
	ldc.i4.1
	stloc.0
?L113:
	ldloc.2
	ldloc.s	15
	bne.un	?L114
?L115:
	br	?L116
?L117:
	ldc.i4.1
	stloc.0
?L116:
	ldloc.2
	ldloc.s	16
	bne.un	?L117
?L118:
	br	?L119
?L120:
	ldc.i4.1
	stloc.0
?L119:
	ldloc.2
	ldloc.s	17
	bne.un	?L120
?L121:
	br	?L122
?L123:
	ldc.i4.1
	stloc.0
?L122:
	ldloc.2
	ldloc.s	18
	bne.un	?L123
?L124:
	br	?L125
?L126:
	ldc.i4.1
	stloc.0
?L125:
	ldloc.2
	ldloc.s	19
	bne.un	?L126
?L127:
	br	?L128
?L129:
	ldc.i4.1
	stloc.0
?L128:
	ldloc.2
	conv.i8
	ldloc.s	20
	bne.un	?L129
?L130:
	br	?L131
?L132:
	ldc.i4.1
	stloc.0
?L131:
	ldloc.2
	conv.u8
	ldloc.s	21
	bne.un	?L132
?L133:
	br	?L134
?L135:
	ldc.i4.1
	stloc.0
?L134:
	ldloc.2
	ldloc.s	22
	bne.un	?L135
?L136:
	br	?L137
?L138:
	ldc.i4.1
	stloc.0
?L137:
	ldloc.2
	conv.r.un
	ldloc.s	23
	bne.un	?L138
?L139:
	br	?L140
?L141:
	ldc.i4.1
	stloc.0
?L140:
	ldloc.2
	conv.r.un
	ldloc.s	24
	bne.un	?L141
?L142:
	br	?L143
?L144:
	ldc.i4.1
	stloc.0
?L143:
	ldloc.2
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int8)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Inequality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L144
?L145:
	br	?L146
?L147:
	ldc.i4.1
	stloc.0
?L146:
	ldloc.3
	ldloc.s	14
	blt	?L147
?L148:
	br	?L149
?L150:
	ldc.i4.1
	stloc.0
?L149:
	ldloc.3
	ldloc.s	15
	blt	?L150
?L151:
	br	?L152
?L153:
	ldc.i4.1
	stloc.0
?L152:
	ldloc.3
	ldloc.s	16
	blt	?L153
?L154:
	br	?L155
?L156:
	ldc.i4.1
	stloc.0
?L155:
	ldloc.3
	ldloc.s	17
	blt	?L156
?L157:
	br	?L158
?L159:
	ldc.i4.1
	stloc.0
?L158:
	ldloc.3
	ldloc.s	18
	blt	?L159
?L160:
	br	?L161
?L162:
	ldc.i4.1
	stloc.0
?L161:
	ldloc.3
	conv.i8
	ldloc.s	19
	conv.u8
	blt	?L162
?L163:
	br	?L164
?L165:
	ldc.i4.1
	stloc.0
?L164:
	ldloc.3
	conv.i8
	ldloc.s	20
	blt	?L165
?L166:
	br	?L167
?L168:
	ldc.i4.1
	stloc.0
?L167:
	ldloc.3
	ldloc.s	22
	blt	?L168
?L169:
	br	?L170
?L171:
	ldc.i4.1
	stloc.0
?L170:
	ldloc.3
	conv.r4
	ldloc.s	23
	blt	?L171
?L172:
	br	?L173
?L174:
	ldc.i4.1
	stloc.0
?L173:
	ldloc.3
	conv.r8
	ldloc.s	24
	blt	?L174
?L175:
	br	?L176
?L177:
	ldc.i4.1
	stloc.0
?L176:
	ldloc.3
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int16)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_LessThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L177
?L178:
	br	?L179
?L180:
	ldc.i4.1
	stloc.0
?L179:
	ldloc.s	4
	ldloc.s	14
	ble	?L180
?L181:
	br	?L182
?L183:
	ldc.i4.1
	stloc.0
?L182:
	ldloc.s	4
	ldloc.s	15
	ble	?L183
?L184:
	br	?L185
?L186:
	ldc.i4.1
	stloc.0
?L185:
	ldloc.s	4
	ldloc.s	16
	ble	?L186
?L187:
	br	?L188
?L189:
	ldc.i4.1
	stloc.0
?L188:
	ldloc.s	4
	ldloc.s	17
	ble	?L189
?L190:
	br	?L191
?L192:
	ldc.i4.1
	stloc.0
?L191:
	ldloc.s	4
	ldloc.s	18
	ble	?L192
?L193:
	br	?L194
?L195:
	ldc.i4.1
	stloc.0
?L194:
	ldloc.s	4
	ldloc.s	19
	ble.un	?L195
?L196:
	br	?L197
?L198:
	ldc.i4.1
	stloc.0
?L197:
	ldloc.s	4
	conv.i8
	ldloc.s	20
	ble	?L198
?L199:
	br	?L200
?L201:
	ldc.i4.1
	stloc.0
?L200:
	ldloc.s	4
	conv.u8
	ldloc.s	21
	ble.un	?L201
?L202:
	br	?L203
?L204:
	ldc.i4.1
	stloc.0
?L203:
	ldloc.s	4
	ldloc.s	22
	ble	?L204
?L205:
	br	?L206
?L207:
	ldc.i4.1
	stloc.0
?L206:
	ldloc.s	4
	conv.r.un
	ldloc.s	23
	ble	?L207
?L208:
	br	?L209
?L210:
	ldc.i4.1
	stloc.0
?L209:
	ldloc.s	4
	conv.r.un
	ldloc.s	24
	ble	?L210
?L211:
	br	?L212
?L213:
	ldc.i4.1
	stloc.0
?L212:
	ldloc.s	4
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int16)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_LessThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L213
?L214:
	br	?L215
?L216:
	ldc.i4.1
	stloc.0
?L215:
	ldloc.s	5
	ldloc.s	14
	bgt	?L216
?L217:
	br	?L218
?L219:
	ldc.i4.1
	stloc.0
?L218:
	ldloc.s	5
	ldloc.s	15
	bgt	?L219
?L220:
	br	?L221
?L222:
	ldc.i4.1
	stloc.0
?L221:
	ldloc.s	5
	ldloc.s	16
	bgt	?L222
?L223:
	br	?L224
?L225:
	ldc.i4.1
	stloc.0
?L224:
	ldloc.s	5
	ldloc.s	17
	bgt	?L225
?L226:
	br	?L227
?L228:
	ldc.i4.1
	stloc.0
?L227:
	ldloc.s	5
	ldloc.s	18
	bgt	?L228
?L229:
	br	?L230
?L231:
	ldc.i4.1
	stloc.0
?L230:
	ldloc.s	5
	conv.i8
	ldloc.s	19
	conv.u8
	bgt	?L231
?L232:
	br	?L233
?L234:
	ldc.i4.1
	stloc.0
?L233:
	ldloc.s	5
	conv.i8
	ldloc.s	20
	bgt	?L234
?L235:
	br	?L236
?L237:
	ldc.i4.1
	stloc.0
?L236:
	ldloc.s	5
	ldloc.s	22
	bgt	?L237
?L238:
	br	?L239
?L240:
	ldc.i4.1
	stloc.0
?L239:
	ldloc.s	5
	conv.r4
	ldloc.s	23
	bgt	?L240
?L241:
	br	?L242
?L243:
	ldc.i4.1
	stloc.0
?L242:
	ldloc.s	5
	conv.r8
	ldloc.s	24
	bgt	?L243
?L244:
	br	?L245
?L246:
	ldc.i4.1
	stloc.0
?L245:
	ldloc.s	5
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int32)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L246
?L247:
	br	?L248
?L249:
	ldc.i4.1
	stloc.0
?L248:
	ldloc.s	6
	conv.u8
	ldloc.s	14
	conv.i8
	bge	?L249
?L250:
	br	?L251
?L252:
	ldc.i4.1
	stloc.0
?L251:
	ldloc.s	6
	ldloc.s	15
	bge.un	?L252
?L253:
	br	?L254
?L255:
	ldc.i4.1
	stloc.0
?L254:
	ldloc.s	6
	conv.u8
	ldloc.s	16
	conv.i8
	bge	?L255
?L256:
	br	?L257
?L258:
	ldc.i4.1
	stloc.0
?L257:
	ldloc.s	6
	ldloc.s	17
	bge.un	?L258
?L259:
	br	?L260
?L261:
	ldc.i4.1
	stloc.0
?L260:
	ldloc.s	6
	conv.u8
	ldloc.s	18
	conv.i8
	bge	?L261
?L262:
	br	?L263
?L264:
	ldc.i4.1
	stloc.0
?L263:
	ldloc.s	6
	ldloc.s	19
	bge.un	?L264
?L265:
	br	?L266
?L267:
	ldc.i4.1
	stloc.0
?L266:
	ldloc.s	6
	conv.u8
	ldloc.s	20
	bge	?L267
?L268:
	br	?L269
?L270:
	ldc.i4.1
	stloc.0
?L269:
	ldloc.s	6
	conv.u8
	ldloc.s	21
	bge.un	?L270
?L271:
	br	?L272
?L273:
	ldc.i4.1
	stloc.0
?L272:
	ldloc.s	6
	ldloc.s	22
	bge.un	?L273
?L274:
	br	?L275
?L276:
	ldc.i4.1
	stloc.0
?L275:
	ldloc.s	6
	conv.r.un
	conv.r4
	ldloc.s	23
	bge	?L276
?L277:
	br	?L278
?L279:
	ldc.i4.1
	stloc.0
?L278:
	ldloc.s	6
	conv.r.un
	ldloc.s	24
	bge	?L279
?L280:
	br	?L281
?L282:
	ldc.i4.1
	stloc.0
?L281:
	ldloc.s	6
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int32)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L282
?L283:
	br	?L284
?L285:
	ldc.i4.1
	stloc.0
?L284:
	ldloc.s	7
	ldloc.s	14
	conv.i8
	beq	?L285
?L286:
	br	?L287
?L288:
	ldc.i4.1
	stloc.0
?L287:
	ldloc.s	7
	ldloc.s	15
	conv.i8
	beq	?L288
?L289:
	br	?L290
?L291:
	ldc.i4.1
	stloc.0
?L290:
	ldloc.s	7
	ldloc.s	16
	conv.i8
	beq	?L291
?L292:
	br	?L293
?L294:
	ldc.i4.1
	stloc.0
?L293:
	ldloc.s	7
	ldloc.s	17
	conv.i8
	beq	?L294
?L295:
	br	?L296
?L297:
	ldc.i4.1
	stloc.0
?L296:
	ldloc.s	7
	ldloc.s	18
	conv.i8
	beq	?L297
?L298:
	br	?L299
?L300:
	ldc.i4.1
	stloc.0
?L299:
	ldloc.s	7
	ldloc.s	19
	conv.u8
	beq	?L300
?L301:
	br	?L302
?L303:
	ldc.i4.1
	stloc.0
?L302:
	ldloc.s	7
	ldloc.s	20
	beq	?L303
?L304:
	br	?L305
?L306:
	ldc.i4.1
	stloc.0
?L305:
	ldloc.s	7
	ldloc.s	22
	conv.i8
	beq	?L306
?L307:
	br	?L308
?L309:
	ldc.i4.1
	stloc.0
?L308:
	ldloc.s	7
	conv.r4
	ldloc.s	23
	beq	?L309
?L310:
	br	?L311
?L312:
	ldc.i4.1
	stloc.0
?L311:
	ldloc.s	7
	conv.r8
	ldloc.s	24
	beq	?L312
?L313:
	br	?L314
?L315:
	ldc.i4.1
	stloc.0
?L314:
	ldloc.s	7
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int64)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Equality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L315
?L316:
	br	?L317
?L318:
	ldc.i4.1
	stloc.0
?L317:
	ldloc.s	8
	ldloc.s	15
	conv.u8
	bne.un	?L318
?L319:
	br	?L320
?L321:
	ldc.i4.1
	stloc.0
?L320:
	ldloc.s	8
	ldloc.s	17
	conv.u8
	bne.un	?L321
?L322:
	br	?L323
?L324:
	ldc.i4.1
	stloc.0
?L323:
	ldloc.s	8
	ldloc.s	19
	conv.u8
	bne.un	?L324
?L325:
	br	?L326
?L327:
	ldc.i4.1
	stloc.0
?L326:
	ldloc.s	8
	ldloc.s	21
	bne.un	?L327
?L328:
	br	?L329
?L330:
	ldc.i4.1
	stloc.0
?L329:
	ldloc.s	8
	ldloc.s	22
	conv.u8
	bne.un	?L330
?L331:
	br	?L332
?L333:
	ldc.i4.1
	stloc.0
?L332:
	ldloc.s	8
	conv.r.un
	conv.r4
	ldloc.s	23
	bne.un	?L333
?L334:
	br	?L335
?L336:
	ldc.i4.1
	stloc.0
?L335:
	ldloc.s	8
	conv.r.un
	conv.r8
	ldloc.s	24
	bne.un	?L336
?L337:
	br	?L338
?L339:
	ldc.i4.1
	stloc.0
?L338:
	ldloc.s	8
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int64)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Inequality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L339
?L340:
	br	?L341
?L342:
	ldc.i4.1
	stloc.0
?L341:
	ldloc.s	10
	ldloc.s	14
	conv.r4
	blt	?L342
?L343:
	br	?L344
?L345:
	ldc.i4.1
	stloc.0
?L344:
	ldloc.s	10
	ldloc.s	15
	conv.r.un
	blt	?L345
?L346:
	br	?L347
?L348:
	ldc.i4.1
	stloc.0
?L347:
	ldloc.s	10
	ldloc.s	16
	conv.r4
	blt	?L348
?L349:
	br	?L350
?L351:
	ldc.i4.1
	stloc.0
?L350:
	ldloc.s	10
	ldloc.s	17
	conv.r.un
	blt	?L351
?L352:
	br	?L353
?L354:
	ldc.i4.1
	stloc.0
?L353:
	ldloc.s	10
	ldloc.s	18
	conv.r4
	blt	?L354
?L355:
	br	?L356
?L357:
	ldc.i4.1
	stloc.0
?L356:
	ldloc.s	10
	ldloc.s	19
	conv.r.un
	conv.r4
	blt	?L357
?L358:
	br	?L359
?L360:
	ldc.i4.1
	stloc.0
?L359:
	ldloc.s	10
	ldloc.s	20
	conv.r4
	blt	?L360
?L361:
	br	?L362
?L363:
	ldc.i4.1
	stloc.0
?L362:
	ldloc.s	10
	ldloc.s	21
	conv.r.un
	conv.r4
	blt	?L363
?L364:
	br	?L365
?L366:
	ldc.i4.1
	stloc.0
?L365:
	ldloc.s	10
	ldloc.s	22
	conv.r.un
	blt	?L366
?L367:
	br	?L368
?L369:
	ldc.i4.1
	stloc.0
?L368:
	ldloc.s	10
	ldloc.s	23
	blt	?L369
?L370:
	br	?L371
?L372:
	ldc.i4.1
	stloc.0
?L371:
	ldloc.s	10
	ldloc.s	24
	blt	?L372
?L373:
	br	?L374
?L375:
	ldc.i4.1
	stloc.0
?L374:
	ldloc.s	11
	ldloc.s	14
	conv.r8
	ble	?L375
?L376:
	br	?L377
?L378:
	ldc.i4.1
	stloc.0
?L377:
	ldloc.s	11
	ldloc.s	15
	conv.r.un
	ble	?L378
?L379:
	br	?L380
?L381:
	ldc.i4.1
	stloc.0
?L380:
	ldloc.s	11
	ldloc.s	16
	conv.r8
	ble	?L381
?L382:
	br	?L383
?L384:
	ldc.i4.1
	stloc.0
?L383:
	ldloc.s	11
	ldloc.s	17
	conv.r.un
	ble	?L384
?L385:
	br	?L386
?L387:
	ldc.i4.1
	stloc.0
?L386:
	ldloc.s	11
	ldloc.s	18
	conv.r8
	ble	?L387
?L388:
	br	?L389
?L390:
	ldc.i4.1
	stloc.0
?L389:
	ldloc.s	11
	ldloc.s	19
	conv.r.un
	ble	?L390
?L391:
	br	?L392
?L393:
	ldc.i4.1
	stloc.0
?L392:
	ldloc.s	11
	ldloc.s	20
	conv.r8
	ble	?L393
?L394:
	br	?L395
?L396:
	ldc.i4.1
	stloc.0
?L395:
	ldloc.s	11
	ldloc.s	21
	conv.r.un
	conv.r8
	ble	?L396
?L397:
	br	?L398
?L399:
	ldc.i4.1
	stloc.0
?L398:
	ldloc.s	11
	ldloc.s	22
	conv.r.un
	ble	?L399
?L400:
	br	?L401
?L402:
	ldc.i4.1
	stloc.0
?L401:
	ldloc.s	11
	ldloc.s	23
	ble	?L402
?L403:
	br	?L404
?L405:
	ldc.i4.1
	stloc.0
?L404:
	ldloc.s	11
	ldloc.s	24
	ble	?L405
?L406:
	br	?L407
?L408:
	ldc.i4.1
	stloc.0
?L407:
	ldloc.s	12
	ldloc.s	14
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int8)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L408
?L409:
	br	?L410
?L411:
	ldc.i4.1
	stloc.0
?L410:
	ldloc.s	12
	ldloc.s	15
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int8)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L411
?L412:
	br	?L413
?L414:
	ldc.i4.1
	stloc.0
?L413:
	ldloc.s	12
	ldloc.s	16
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int16)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L414
?L415:
	br	?L416
?L417:
	ldc.i4.1
	stloc.0
?L416:
	ldloc.s	12
	ldloc.s	17
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int16)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L417
?L418:
	br	?L419
?L420:
	ldc.i4.1
	stloc.0
?L419:
	ldloc.s	12
	ldloc.s	18
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int32)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L420
?L421:
	br	?L422
?L423:
	ldc.i4.1
	stloc.0
?L422:
	ldloc.s	12
	ldloc.s	19
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int32)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L423
?L424:
	br	?L425
?L426:
	ldc.i4.1
	stloc.0
?L425:
	ldloc.s	12
	ldloc.s	20
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int64)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L426
?L427:
	br	?L428
?L429:
	ldc.i4.1
	stloc.0
?L428:
	ldloc.s	12
	ldloc.s	21
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int64)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L429
?L430:
	br	?L431
?L432:
	ldc.i4.1
	stloc.0
?L431:
	ldloc.s	12
	ldloc.s	22
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(char)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L432
?L433:
	br	?L434
?L435:
	ldc.i4.1
	stloc.0
?L434:
	ldloc.s	12
	ldloc.s	25
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brtrue	?L435
?L436:
	br	?L437
?L438:
	ldc.i4.1
	stloc.0
?L437:
	ldloc.s	13
	ldloc.s	26
	beq	?L438
?L439:
	br	?L440
?L441:
	ldc.i4.1
	stloc.0
?L440:
	ldloc.s	13
	ldloc.s	26
	bne.un	?L441
?L442:
	ldloc.1
	ldloc.s	14
	bne.un	?L443
	ldc.i4.1
	stloc.0
?L443:
	ldloc.1
	ldloc.s	15
	bne.un	?L444
	ldc.i4.1
	stloc.0
?L444:
	ldloc.1
	ldloc.s	16
	bne.un	?L445
	ldc.i4.1
	stloc.0
?L445:
	ldloc.1
	ldloc.s	17
	bne.un	?L446
	ldc.i4.1
	stloc.0
?L446:
	ldloc.1
	ldloc.s	18
	bne.un	?L447
	ldc.i4.1
	stloc.0
?L447:
	ldloc.1
	conv.i8
	ldloc.s	19
	conv.u8
	bne.un	?L448
	ldc.i4.1
	stloc.0
?L448:
	ldloc.1
	conv.i8
	ldloc.s	20
	bne.un	?L449
	ldc.i4.1
	stloc.0
?L449:
	ldloc.1
	ldloc.s	22
	bne.un	?L450
	ldc.i4.1
	stloc.0
?L450:
	ldloc.1
	conv.r4
	ldloc.s	23
	bne.un	?L451
	ldc.i4.1
	stloc.0
?L451:
	ldloc.1
	conv.r8
	ldloc.s	24
	bne.un	?L452
	ldc.i4.1
	stloc.0
?L452:
	ldloc.1
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int8)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Equality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L453
	ldc.i4.1
	stloc.0
?L453:
	ldloc.2
	ldloc.s	14
	beq	?L454
	ldc.i4.1
	stloc.0
?L454:
	ldloc.2
	ldloc.s	15
	beq	?L455
	ldc.i4.1
	stloc.0
?L455:
	ldloc.2
	ldloc.s	16
	beq	?L456
	ldc.i4.1
	stloc.0
?L456:
	ldloc.2
	ldloc.s	17
	beq	?L457
	ldc.i4.1
	stloc.0
?L457:
	ldloc.2
	ldloc.s	18
	beq	?L458
	ldc.i4.1
	stloc.0
?L458:
	ldloc.2
	ldloc.s	19
	beq	?L459
	ldc.i4.1
	stloc.0
?L459:
	ldloc.2
	conv.i8
	ldloc.s	20
	beq	?L460
	ldc.i4.1
	stloc.0
?L460:
	ldloc.2
	conv.u8
	ldloc.s	21
	beq	?L461
	ldc.i4.1
	stloc.0
?L461:
	ldloc.2
	ldloc.s	22
	beq	?L462
	ldc.i4.1
	stloc.0
?L462:
	ldloc.2
	conv.r.un
	ldloc.s	23
	beq	?L463
	ldc.i4.1
	stloc.0
?L463:
	ldloc.2
	conv.r.un
	ldloc.s	24
	beq	?L464
	ldc.i4.1
	stloc.0
?L464:
	ldloc.2
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int8)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Inequality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L465
	ldc.i4.1
	stloc.0
?L465:
	ldloc.3
	ldloc.s	14
	bge	?L466
	ldc.i4.1
	stloc.0
?L466:
	ldloc.3
	ldloc.s	15
	bge	?L467
	ldc.i4.1
	stloc.0
?L467:
	ldloc.3
	ldloc.s	16
	bge	?L468
	ldc.i4.1
	stloc.0
?L468:
	ldloc.3
	ldloc.s	17
	bge	?L469
	ldc.i4.1
	stloc.0
?L469:
	ldloc.3
	ldloc.s	18
	bge	?L470
	ldc.i4.1
	stloc.0
?L470:
	ldloc.3
	conv.i8
	ldloc.s	19
	conv.u8
	bge	?L471
	ldc.i4.1
	stloc.0
?L471:
	ldloc.3
	conv.i8
	ldloc.s	20
	bge	?L472
	ldc.i4.1
	stloc.0
?L472:
	ldloc.3
	ldloc.s	22
	bge	?L473
	ldc.i4.1
	stloc.0
?L473:
	ldloc.3
	conv.r4
	ldloc.s	23
	bge	?L474
	ldc.i4.1
	stloc.0
?L474:
	ldloc.3
	conv.r8
	ldloc.s	24
	bge	?L475
	ldc.i4.1
	stloc.0
?L475:
	ldloc.3
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int16)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_LessThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L476
	ldc.i4.1
	stloc.0
?L476:
	ldloc.s	4
	ldloc.s	14
	bgt	?L477
	ldc.i4.1
	stloc.0
?L477:
	ldloc.s	4
	ldloc.s	15
	bgt	?L478
	ldc.i4.1
	stloc.0
?L478:
	ldloc.s	4
	ldloc.s	16
	bgt	?L479
	ldc.i4.1
	stloc.0
?L479:
	ldloc.s	4
	ldloc.s	17
	bgt	?L480
	ldc.i4.1
	stloc.0
?L480:
	ldloc.s	4
	ldloc.s	18
	bgt	?L481
	ldc.i4.1
	stloc.0
?L481:
	ldloc.s	4
	ldloc.s	19
	bgt.un	?L482
	ldc.i4.1
	stloc.0
?L482:
	ldloc.s	4
	conv.i8
	ldloc.s	20
	bgt	?L483
	ldc.i4.1
	stloc.0
?L483:
	ldloc.s	4
	conv.u8
	ldloc.s	21
	bgt.un	?L484
	ldc.i4.1
	stloc.0
?L484:
	ldloc.s	4
	ldloc.s	22
	bgt	?L485
	ldc.i4.1
	stloc.0
?L485:
	ldloc.s	4
	conv.r.un
	ldloc.s	23
	bgt	?L486
	ldc.i4.1
	stloc.0
?L486:
	ldloc.s	4
	conv.r.un
	ldloc.s	24
	bgt	?L487
	ldc.i4.1
	stloc.0
?L487:
	ldloc.s	4
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int16)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_LessThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L488
	ldc.i4.1
	stloc.0
?L488:
	ldloc.s	5
	ldloc.s	14
	ble	?L489
	ldc.i4.1
	stloc.0
?L489:
	ldloc.s	5
	ldloc.s	15
	ble	?L490
	ldc.i4.1
	stloc.0
?L490:
	ldloc.s	5
	ldloc.s	16
	ble	?L491
	ldc.i4.1
	stloc.0
?L491:
	ldloc.s	5
	ldloc.s	17
	ble	?L492
	ldc.i4.1
	stloc.0
?L492:
	ldloc.s	5
	ldloc.s	18
	ble	?L493
	ldc.i4.1
	stloc.0
?L493:
	ldloc.s	5
	conv.i8
	ldloc.s	19
	conv.u8
	ble	?L494
	ldc.i4.1
	stloc.0
?L494:
	ldloc.s	5
	conv.i8
	ldloc.s	20
	ble	?L495
	ldc.i4.1
	stloc.0
?L495:
	ldloc.s	5
	ldloc.s	22
	ble	?L496
	ldc.i4.1
	stloc.0
?L496:
	ldloc.s	5
	conv.r4
	ldloc.s	23
	ble	?L497
	ldc.i4.1
	stloc.0
?L497:
	ldloc.s	5
	conv.r8
	ldloc.s	24
	ble	?L498
	ldc.i4.1
	stloc.0
?L498:
	ldloc.s	5
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int32)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L499
	ldc.i4.1
	stloc.0
?L499:
	ldloc.s	6
	conv.u8
	ldloc.s	14
	conv.i8
	blt	?L500
	ldc.i4.1
	stloc.0
?L500:
	ldloc.s	6
	ldloc.s	15
	blt.un	?L501
	ldc.i4.1
	stloc.0
?L501:
	ldloc.s	6
	conv.u8
	ldloc.s	16
	conv.i8
	blt	?L502
	ldc.i4.1
	stloc.0
?L502:
	ldloc.s	6
	ldloc.s	17
	blt.un	?L503
	ldc.i4.1
	stloc.0
?L503:
	ldloc.s	6
	conv.u8
	ldloc.s	18
	conv.i8
	blt	?L504
	ldc.i4.1
	stloc.0
?L504:
	ldloc.s	6
	ldloc.s	19
	blt.un	?L505
	ldc.i4.1
	stloc.0
?L505:
	ldloc.s	6
	conv.u8
	ldloc.s	20
	blt	?L506
	ldc.i4.1
	stloc.0
?L506:
	ldloc.s	6
	conv.u8
	ldloc.s	21
	blt.un	?L507
	ldc.i4.1
	stloc.0
?L507:
	ldloc.s	6
	ldloc.s	22
	blt.un	?L508
	ldc.i4.1
	stloc.0
?L508:
	ldloc.s	6
	conv.r.un
	conv.r4
	ldloc.s	23
	blt	?L509
	ldc.i4.1
	stloc.0
?L509:
	ldloc.s	6
	conv.r.un
	ldloc.s	24
	blt	?L510
	ldc.i4.1
	stloc.0
?L510:
	ldloc.s	6
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int32)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L511
	ldc.i4.1
	stloc.0
?L511:
	ldloc.s	7
	ldloc.s	14
	conv.i8
	bne.un	?L512
	ldc.i4.1
	stloc.0
?L512:
	ldloc.s	7
	ldloc.s	15
	conv.i8
	bne.un	?L513
	ldc.i4.1
	stloc.0
?L513:
	ldloc.s	7
	ldloc.s	16
	conv.i8
	bne.un	?L514
	ldc.i4.1
	stloc.0
?L514:
	ldloc.s	7
	ldloc.s	17
	conv.i8
	bne.un	?L515
	ldc.i4.1
	stloc.0
?L515:
	ldloc.s	7
	ldloc.s	18
	conv.i8
	bne.un	?L516
	ldc.i4.1
	stloc.0
?L516:
	ldloc.s	7
	ldloc.s	19
	conv.u8
	bne.un	?L517
	ldc.i4.1
	stloc.0
?L517:
	ldloc.s	7
	ldloc.s	20
	bne.un	?L518
	ldc.i4.1
	stloc.0
?L518:
	ldloc.s	7
	ldloc.s	22
	conv.i8
	bne.un	?L519
	ldc.i4.1
	stloc.0
?L519:
	ldloc.s	7
	conv.r4
	ldloc.s	23
	bne.un	?L520
	ldc.i4.1
	stloc.0
?L520:
	ldloc.s	7
	conv.r8
	ldloc.s	24
	bne.un	?L521
	ldc.i4.1
	stloc.0
?L521:
	ldloc.s	7
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int64)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Equality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L522
	ldc.i4.1
	stloc.0
?L522:
	ldloc.s	8
	ldloc.s	15
	conv.u8
	beq	?L523
	ldc.i4.1
	stloc.0
?L523:
	ldloc.s	8
	ldloc.s	17
	conv.u8
	beq	?L524
	ldc.i4.1
	stloc.0
?L524:
	ldloc.s	8
	ldloc.s	19
	conv.u8
	beq	?L525
	ldc.i4.1
	stloc.0
?L525:
	ldloc.s	8
	ldloc.s	21
	beq	?L526
	ldc.i4.1
	stloc.0
?L526:
	ldloc.s	8
	ldloc.s	22
	conv.u8
	beq	?L527
	ldc.i4.1
	stloc.0
?L527:
	ldloc.s	8
	conv.r.un
	conv.r4
	ldloc.s	23
	beq	?L528
	ldc.i4.1
	stloc.0
?L528:
	ldloc.s	8
	conv.r.un
	conv.r8
	ldloc.s	24
	beq	?L529
	ldc.i4.1
	stloc.0
?L529:
	ldloc.s	8
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int64)
	ldloc.s	25
	call	bool [.library]System.Decimal::op_Inequality(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L530
	ldc.i4.1
	stloc.0
?L530:
	ldloc.s	10
	ldloc.s	14
	conv.r4
	bge	?L531
	ldc.i4.1
	stloc.0
?L531:
	ldloc.s	10
	ldloc.s	15
	conv.r.un
	bge	?L532
	ldc.i4.1
	stloc.0
?L532:
	ldloc.s	10
	ldloc.s	16
	conv.r4
	bge	?L533
	ldc.i4.1
	stloc.0
?L533:
	ldloc.s	10
	ldloc.s	17
	conv.r.un
	bge	?L534
	ldc.i4.1
	stloc.0
?L534:
	ldloc.s	10
	ldloc.s	18
	conv.r4
	bge	?L535
	ldc.i4.1
	stloc.0
?L535:
	ldloc.s	10
	ldloc.s	19
	conv.r.un
	conv.r4
	bge	?L536
	ldc.i4.1
	stloc.0
?L536:
	ldloc.s	10
	ldloc.s	20
	conv.r4
	bge	?L537
	ldc.i4.1
	stloc.0
?L537:
	ldloc.s	10
	ldloc.s	21
	conv.r.un
	conv.r4
	bge	?L538
	ldc.i4.1
	stloc.0
?L538:
	ldloc.s	10
	ldloc.s	22
	conv.r.un
	bge	?L539
	ldc.i4.1
	stloc.0
?L539:
	ldloc.s	10
	ldloc.s	23
	bge	?L540
	ldc.i4.1
	stloc.0
?L540:
	ldloc.s	10
	ldloc.s	24
	bge	?L541
	ldc.i4.1
	stloc.0
?L541:
	ldloc.s	11
	ldloc.s	14
	conv.r8
	bgt	?L542
	ldc.i4.1
	stloc.0
?L542:
	ldloc.s	11
	ldloc.s	15
	conv.r.un
	bgt	?L543
	ldc.i4.1
	stloc.0
?L543:
	ldloc.s	11
	ldloc.s	16
	conv.r8
	bgt	?L544
	ldc.i4.1
	stloc.0
?L544:
	ldloc.s	11
	ldloc.s	17
	conv.r.un
	bgt	?L545
	ldc.i4.1
	stloc.0
?L545:
	ldloc.s	11
	ldloc.s	18
	conv.r8
	bgt	?L546
	ldc.i4.1
	stloc.0
?L546:
	ldloc.s	11
	ldloc.s	19
	conv.r.un
	bgt	?L547
	ldc.i4.1
	stloc.0
?L547:
	ldloc.s	11
	ldloc.s	20
	conv.r8
	bgt	?L548
	ldc.i4.1
	stloc.0
?L548:
	ldloc.s	11
	ldloc.s	21
	conv.r.un
	conv.r8
	bgt	?L549
	ldc.i4.1
	stloc.0
?L549:
	ldloc.s	11
	ldloc.s	22
	conv.r.un
	bgt	?L550
	ldc.i4.1
	stloc.0
?L550:
	ldloc.s	11
	ldloc.s	23
	bgt	?L551
	ldc.i4.1
	stloc.0
?L551:
	ldloc.s	11
	ldloc.s	24
	bgt	?L552
	ldc.i4.1
	stloc.0
?L552:
	ldloc.s	12
	ldloc.s	14
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int8)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L553
	ldc.i4.1
	stloc.0
?L553:
	ldloc.s	12
	ldloc.s	15
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int8)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L554
	ldc.i4.1
	stloc.0
?L554:
	ldloc.s	12
	ldloc.s	16
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int16)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L555
	ldc.i4.1
	stloc.0
?L555:
	ldloc.s	12
	ldloc.s	17
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int16)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L556
	ldc.i4.1
	stloc.0
?L556:
	ldloc.s	12
	ldloc.s	18
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int32)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L557
	ldc.i4.1
	stloc.0
?L557:
	ldloc.s	12
	ldloc.s	19
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int32)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L558
	ldc.i4.1
	stloc.0
?L558:
	ldloc.s	12
	ldloc.s	20
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(int64)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L559
	ldc.i4.1
	stloc.0
?L559:
	ldloc.s	12
	ldloc.s	21
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(unsigned int64)
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L560
	ldc.i4.1
	stloc.0
?L560:
	ldloc.s	12
	ldloc.s	22
	call	valuetype [.library]System.Decimal [.library]System.Decimal::op_Implicit(char)
	call	bool [.library]System.Decimal::op_GreaterThan(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L561
	ldc.i4.1
	stloc.0
?L561:
	ldloc.s	12
	ldloc.s	25
	call	bool [.library]System.Decimal::op_GreaterThanOrEqual(valuetype [.library]System.Decimal, valuetype [.library]System.Decimal)
	brfalse	?L562
	ldc.i4.1
	stloc.0
?L562:
	ldloc.s	13
	ldloc.s	26
	bne.un	?L563
	ldc.i4.1
	stloc.0
?L563:
	ldloc.s	13
	ldloc.s	26
	beq	?L564
	ldc.i4.1
	stloc.0
?L564:
	ret
	.maxstack 5
} // method m1
.method public hidebysig specialname rtspecialname instance void '.ctor'() cil managed 
{
	ldarg.0
	call	instance void ['.library']'System'.'Object'::'.ctor'()
	ret
	.maxstack 1
} // method .ctor
} // class Test
