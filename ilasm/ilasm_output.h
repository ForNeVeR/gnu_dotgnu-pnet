/*
 * ilasm_output.h - Output method code.
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

#ifndef	_ILASM_OUTPUT_H
#define	_ILASM_OUTPUT_H

#include "il_program.h"
#include "il_utils.h"
#include "il_writer.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Global image writer that is in use.
 */
extern ILWriter *ILAsmWriter;

/*
 * Create the global image writer.
 */
void ILAsmOutCreate(FILE *stream, int seekable, int type, int flags);

/*
 * Flush and destroy the global image writer.
 */
int ILAsmOutDestroy(void);

/*
 * Output a simple instruction that has no arguments.
 */
void ILAsmOutSimple(ILInt32 opcode);

/*
 * Output an instruction that references a variable.
 */
void ILAsmOutVar(ILInt32 opcode, ILInt64 num);

/*
 * Output an instruction that takes an integer argument.
 */
void ILAsmOutInt(ILInt32 opcode, ILInt64 value);

/*
 * Push a 32-bit float value onto the stack.
 */
void ILAsmOutFloat(unsigned char *bytes);

/*
 * Push a 64-bit double value onto the stack.
 */
void ILAsmOutDouble(unsigned char *bytes);

/*
 * Output an instruction that takes a token as an argument.
 */
void ILAsmOutToken(ILInt32 opcode, ILUInt32 token);

/*
 * Output an instruction that pushes a string literal.
 */
void ILAsmOutString(ILIntString interned);

/*
 * Output a branch to a integer label within the current method.
 */
void ILAsmOutBranchInt(ILInt32 opcode, ILInt64 addr);

/*
 * Output a branch to a named label within the current method.
 */
void ILAsmOutBranch(ILInt32 opcode, char *label);

/*
 * Start output of a switch statement.
 */
void ILAsmOutSwitchStart(void);

/*
 * Output an integer switch label reference within the current method.
 */
void ILAsmOutSwitchRefInt(ILInt64 addr);

/*
 * Output a switch label reference within the current method.
 */
void ILAsmOutSwitchRef(char *label);

/*
 * End output of a switch statement.
 */
void ILAsmOutSwitchEnd(void);

/*
 * Output a named label at the current position within the method.
 */
void ILAsmOutLabel(char *label);

/*
 * Convert an integer label name into its string form.
 */
char *ILAsmOutIntToName(ILInt64 label);

/*
 * Output an integer label name at the current position.
 * Returns the string form of the label name.
 */
char *ILAsmOutIntLabel(ILInt64 label);

/*
 * Output a unique label name for the current position,
 * and return the string form of the label name.
 */
char *ILAsmOutUniqueLabel(void);

/*
 * Start output of an SSA instruction.
 */
void ILAsmOutSSAStart(ILInt32 opcode);

/*
 * Output an SSA value.
 */
void ILAsmOutSSAValue(ILInt64 value);

/*
 * End output of an SSA instruction.
 */
void ILAsmOutSSAEnd(void);

/*
 * Set the maximum stack size for the current method.
 */
void ILAsmOutMaxStack(ILUInt32 maxStack);

/*
 * Set the initialization flag for the local variables.
 */
void ILAsmOutZeroInit(void);

/*
 * Add a group of local variables to the current method.
 * This also free's the "vars" list.
 */
void ILAsmOutAddLocals(ILAsmParamInfo *vars);

/*
 * Add a group of parameters to the current method.
 */
void ILAsmOutAddParams(ILAsmParamInfo *vars, ILUInt32 callConv);

/*
 * Look up a local or parameter and return the index.
 */
ILUInt32 ILAsmOutLookupVar(char *name);

/*
 * Information that is stored for an exception handler block.
 */
typedef struct _tagILAsmOutException ILAsmOutException;
struct _tagILAsmOutException
{
	ILUInt32			flags;
	char			   *blockStart;
	char			   *blockEnd;
	ILUInt32			blockOffset;
	ILUInt32			blockLength;
	char			   *handlerStart;
	char			   *handlerEnd;
	ILUInt32			handlerOffset;
	ILUInt32			handlerLength;
	ILClass			   *classToCatch;
	char			   *filterLabel;
	ILUInt32			filterOffset;
	ILAsmOutException  *next;

};

/*
 * Make an exception handler block for the current method.
 */
ILAsmOutException *ILAsmOutMakeException(ILUInt32 flags, ILClass *classInfo,
									     char *filterLabel, char *handlerStart,
									     char *handlerEnd);

/*
 * Add a try block to the current method.
 */
void ILAsmOutAddTryBlock(char *blockStart, char *blockEnd,
						 ILAsmOutException *handlers);

/*
 * Finalize processing for a method.
 */
void ILAsmOutFinalizeMethod(ILMethod *method);

#ifdef	__cplusplus
};
#endif

#endif	/* _ILASM_OUTPUT_H */
