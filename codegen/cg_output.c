/*
 * cg_output.c - Assembly code output routines.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "cg_nodes.h"
#include "il_dumpasm.h"

#ifdef	__cplusplus
extern	"C" {
#endif

void ILGenSimple(ILGenInfo *info, int opcode)
{
	if(info->asmOutput)
	{
		if(opcode < IL_OP_PREFIX)
		{
			fprintf(info->asmOutput, "\t%s\n",
					ILMainOpcodeTable[opcode].name);
		}
		else
		{
			fprintf(info->asmOutput, "\t%s\n",
					ILPrefixOpcodeTable[opcode - IL_OP_PREFIX].name);
		}
	}
}

void ILGenByteInsn(ILGenInfo *info, int opcode, int arg)
{
	if(info->asmOutput)
	{
		if(opcode < IL_OP_PREFIX)
		{
			fprintf(info->asmOutput, "\t%s\t%d\n",
					ILMainOpcodeTable[opcode].name, arg);
		}
		else
		{
			fprintf(info->asmOutput, "\t%s\t%d\n",
					ILPrefixOpcodeTable[opcode - IL_OP_PREFIX].name, arg);
		}
	}
}

void ILGenShortInsn(ILGenInfo *info, int opcode, ILUInt32 arg)
{
	if(info->asmOutput)
	{
		if(opcode < IL_OP_PREFIX)
		{
			fprintf(info->asmOutput, "\t%s\t%lu\n",
					ILMainOpcodeTable[opcode].name,
					(unsigned long)(arg & 0xFFFF));
		}
		else
		{
			fprintf(info->asmOutput, "\t%s\t%lu\n",
					ILPrefixOpcodeTable[opcode - IL_OP_PREFIX].name,
					(unsigned long)(arg & 0xFFFF));
		}
	}
}

void ILGenWordInsn(ILGenInfo *info, int opcode, ILUInt32 arg)
{
	if(info->asmOutput)
	{
		if(opcode < IL_OP_PREFIX)
		{
			fprintf(info->asmOutput, "\t%s\t%lu\n",
					ILMainOpcodeTable[opcode].name,
					(unsigned long)arg);
		}
		else
		{
			fprintf(info->asmOutput, "\t%s\t%lu\n",
					ILPrefixOpcodeTable[opcode - IL_OP_PREFIX].name,
					(unsigned long)arg);
		}
	}
}

void ILGenDWordInsn(ILGenInfo *info, int opcode, ILUInt64 arg)
{
	if(info->asmOutput)
	{
		if(opcode < IL_OP_PREFIX)
		{
			fprintf(info->asmOutput, "\t%s\t0x%08lx%08lx\n",
					ILMainOpcodeTable[opcode].name,
					(unsigned long)(arg >> 32),
					(unsigned long)(arg & 0xFFFFFFFF));
		}
		else
		{
			fprintf(info->asmOutput, "\t%s\t0x%08lx%08lx\n",
					ILPrefixOpcodeTable[opcode - IL_OP_PREFIX].name,
					(unsigned long)(arg >> 32),
					(unsigned long)(arg & 0xFFFFFFFF));
		}
	}
}

void ILGenLoadFloat32(ILGenInfo *info, ILFloat value)
{
	if(info->asmOutput)
	{
		union
		{
			unsigned char bytes[4];
			ILFloat		  value;
		} convert;
		convert.value = (ILFloat)1.0;
		if(convert.bytes[3] == (unsigned char)0x3F)
		{
			/* Little-endian host CPU */
			convert.value = value;
			fprintf(info->asmOutput, "\tldc.r4\t(%02X %02X %02X %02X)\n",
					convert.bytes[0], convert.bytes[1],
					convert.bytes[2], convert.bytes[3]);
		}
		else
		{
			/* Big-endian host CPU */
			convert.value = value;
			fprintf(info->asmOutput, "\tldc.r4\t(%02X %02X %02X %02X)\n",
					convert.bytes[3], convert.bytes[2],
					convert.bytes[1], convert.bytes[0]);
		}
	}
}

void ILGenLoadFloat64(ILGenInfo *info, ILDouble value)
{
	if(info->asmOutput)
	{
		union
		{
			unsigned char bytes[8];
			struct {
				unsigned long first, second;
			} words;
			ILDouble	  value;
		} convert;
		convert.value = (ILDouble)1.0;
		if(convert.words.first == 0 && convert.words.second == 0x3FF00000)
		{
			/* Little-endian host CPU */
			convert.value = value;
			fprintf(info->asmOutput,
					"\tldc.r8\t(%02X %02X %02X %02X %02X %02X %02X %02X)\n",
					convert.bytes[0], convert.bytes[1],
					convert.bytes[2], convert.bytes[3],
					convert.bytes[4], convert.bytes[5],
					convert.bytes[6], convert.bytes[7]);
		}
		else
		{
			/* Big-endian host CPU */
			convert.value = value;
			fprintf(info->asmOutput,
					"\tldc.r8\t(%02X %02X %02X %02X %02X %02X %02X %02X)\n",
					convert.bytes[7], convert.bytes[6],
					convert.bytes[5], convert.bytes[4],
					convert.bytes[3], convert.bytes[2],
					convert.bytes[1], convert.bytes[0]);
		}
	}
}

void ILGenLoadString(ILGenInfo *info, const char *str, int len)
{
	if(len < 0)
	{
		len = strlen(str);
	}
	if(info->asmOutput)
	{
		fprintf(info->asmOutput, "\tldstr\t\"");
		while(len > 0)
		{
			if(*str == '"' || *str == '\\')
			{
				putc('\\', info->asmOutput);
				putc(*str, info->asmOutput);
			}
			else if(*str >= ' ' && *str < (char)0x7F)
			{
				putc(*str, info->asmOutput);
			}
			else
			{
				putc('\\', info->asmOutput);
				putc('0' + ((*str >> 6) & 0x03), info->asmOutput);
				putc('0' + ((*str >> 3) & 0x07), info->asmOutput);
				putc('0' + (*str & 0x07), info->asmOutput);
			}
			++str;
			--len;
		}
		putc('"', info->asmOutput);
		putc('\n', info->asmOutput);
	}
}

void ILGenAllocLocal(ILGenInfo *info, ILType *type, const char *name)
{
	if(info->asmOutput)
	{
		fputs("\t.locals\t(", info->asmOutput);
		ILDumpType(info->asmOutput, 0, type, IL_DUMP_QUOTE_NAMES);
		if(name)
		{
			putc(' ', info->asmOutput);
			putc('\'', info->asmOutput);
			fputs(name, info->asmOutput);
			putc('\'', info->asmOutput);
		}
		putc(')', info->asmOutput);
		putc('\n', info->asmOutput);
	}
}

void ILGenJump(ILGenInfo *info, int opcode, ILLabel *label)
{
	if(*label == ILLabel_Undefined)
	{
		*label = (info->nextLabel)++;
	}
	if(info->asmOutput)
	{
		if(opcode < IL_OP_PREFIX)
		{
			fprintf(info->asmOutput, "\t%s\t?L%lu\n",
					ILMainOpcodeTable[opcode].name, *label);
		}
		else
		{
			fprintf(info->asmOutput, "\t%s\t?L%lu\n",
					ILPrefixOpcodeTable[opcode - IL_OP_PREFIX].name, *label);
		}
	}
}

void ILGenBrOrLeaveJump(ILGenInfo *info, ILLabel *label)
{
	if(*label == ILLabel_Undefined)
	{
		*label = (info->nextLabel)++;
	}
	if(info->asmOutput)
	{
		fprintf(info->asmOutput, "\tbr_or_leave\t?L%lu\n", *label);
	}
}

void ILGenLabel(ILGenInfo *info, ILLabel *label)
{
	if(*label == ILLabel_Undefined)
	{
		*label = (info->nextLabel)++;
	}
	if(info->asmOutput)
	{
		fprintf(info->asmOutput, "?L%lu:\n", *label);
	}
}

void ILGenLeaveLabel(ILGenInfo *info, ILLabel *label)
{
	if(*label == ILLabel_Undefined)
	{
		*label = (info->nextLabel)++;
	}
	if(info->asmOutput)
	{
		fprintf(info->asmOutput, ".leave ?L%lu:\n", *label);
	}
}

void ILGenCallByName(ILGenInfo *info, const char *name)
{
	if(info->asmOutput)
	{
		fprintf(info->asmOutput, "\tcall\t%s\n", name);
	}
}

void ILGenCallVirtual(ILGenInfo *info, const char *name)
{
	if(info->asmOutput)
	{
		fprintf(info->asmOutput, "\tcallvirt\tinstance %s\n", name);
	}
}

void ILGenCallByMethod(ILGenInfo *info, ILMethod *method)
{
	if(info->asmOutput)
	{
		fputs("\tcall\t", info->asmOutput);
		ILDumpMethodType(info->asmOutput, 0, ILMethod_Signature(method),
						 IL_DUMP_QUOTE_NAMES, ILMethod_Owner(method),
						 ILMethod_Name(method), 0);
		putc('\n', info->asmOutput);
	}
}

void ILGenNewObj(ILGenInfo *info, const char *className,
				 const char *signature)
{
	if(info->asmOutput)
	{
		fprintf(info->asmOutput, "\tnewobj\tinstance void %s::.ctor%s\n",
				className, signature);
	}
}

void ILGenClassToken(ILGenInfo *info, int opcode, ILClass *classInfo)
{
	if(info->asmOutput)
	{
		putc('\t', info->asmOutput);
		if(opcode < IL_OP_PREFIX)
		{
			fputs(ILMainOpcodeTable[opcode].name, info->asmOutput);
		}
		else
		{
			fputs(ILPrefixOpcodeTable[opcode - IL_OP_PREFIX].name,
				  info->asmOutput);
		}
		putc('\t', info->asmOutput);
		ILDumpClassName(info->asmOutput, 0, classInfo, IL_DUMP_QUOTE_NAMES);
	}
}

void ILGenFlush(ILGenInfo *info)
{
	/* Peephole optimization not yet implemented */
}

#ifdef	__cplusplus
};
#endif
