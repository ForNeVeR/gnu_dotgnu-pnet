.assembly '<Assembly>'
{
	.ver 0:0:0:0
}
.module '<Module>'
.custom instance void OpenSystem.C.MemoryModelAttribute::.ctor(int32) = (01 00 40 00 00 00 00 00)
.field public static valuetype 'union (T5xgHRfNMqBwUu1V7wuX_M)' 'untie'
.field public static valuetype 'struct (P2uJa1z6I7cuXWtWVAgYkM)' 't'
.class public explicit sealed serializable ansi 'union (T5xgHRfNMqBwUu1V7wuX_M)' extends ['.library']'System'.'ValueType'
{
.size 16
.pack 8
.field [0] public valuetype 'struct (E0EvaNELhKUmRf3e$EPHQA)' '__combined'
} // class union (T5xgHRfNMqBwUu1V7wuX_M)
.class public explicit sealed serializable ansi 'struct (P2uJa1z6I7cuXWtWVAgYkM)' extends ['.library']'System'.'ValueType'
{
.size 8
.pack 8
.field [0] public valuetype 'struct _GList' * 'data'
} // class struct (P2uJa1z6I7cuXWtWVAgYkM)
.class public explicit sealed serializable ansi 'struct (E0EvaNELhKUmRf3e$EPHQA)' extends ['.library']'System'.'ValueType'
{
.size 16
.pack 8
.field [0] public valuetype 'struct baz' 'abar'
} // class struct (E0EvaNELhKUmRf3e$EPHQA)
.class public explicit sealed serializable ansi 'struct _GList' extends ['.library']'System'.'ValueType'
{
.size 24
.pack 8
.field [0] public void * 'data'
.field [8] public valuetype 'struct _GList' * 'next'
.field [16] public valuetype 'struct _GList' * 'prev'
} // class struct _GList
.class public explicit sealed serializable ansi 'struct baz' extends ['.library']'System'.'ValueType'
{
.size 16
.pack 8
.field [0] public int32 'x'
.field [8] public method int32 *(int32 *, valuetype 'struct bar' *) modopt(['OpenSystem.C']'OpenSystem.C'.'IsFunctionPointer') * 'foo'
} // class struct baz
.class public explicit sealed serializable ansi 'struct bar' extends ['.library']'System'.'ValueType'
{
.size 8
.pack 8
.field [0] public method int32 *(int32 *, valuetype 'struct baz' *) modopt(['OpenSystem.C']'OpenSystem.C'.'IsFunctionPointer') * 'pfoo'
} // class struct bar
