/*
 * jv_output.h - Assembly code output routines for Java bytecode.
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

#ifndef	_CODEGEN_JV_OUTPUT_H
#define	_CODEGEN_JV_OUTPUT_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Initialize the Java-specific structures in an ILGenInfo structure.
 */
void JavaGenInit(ILGenInfo *info);

/*
 * Destroy the Java-specific structures in an ILGenInfo structure.
 */
void JavaGenDestroy(ILGenInfo *info);

/*
 * Clear the Java code output state, ready for a new .class file.
 */
void JavaGenClear(ILGenInfo *info);

/*
 * Output a simple instruction.
 */
void JavaGenSimple(ILGenInfo *info, int opcode);

/*
 * Load an "int32" value onto the stack.
 */
void JavaGenInt32(ILGenInfo *info, ILInt32 value);

/*
 * Load a "uint32" value onto the stack.
 */
void JavaGenUInt32(ILGenInfo *info, ILUInt32 value);

/*
 * Load an "int64" value onto the stack.
 */
void JavaGenInt64(ILGenInfo *info, ILInt64 value);

/*
 * Load a "uint64" value onto the stack.
 */
void JavaGenUInt64(ILGenInfo *info, ILUInt64 value);

/*
 * Load a "float32" value onto the stack.
 */
void JavaGenFloat32(ILGenInfo *info, ILFloat value);

/*
 * Load a "float64" value onto the stack.
 */
void JavaGenFloat64(ILGenInfo *info, ILDouble value);

/*
 * Load a string literal onto the stack.
 */
void JavaGenStringConst(ILGenInfo *info, const char *str, int len);

/*
 * Output a jump instruction.
 */
void JavaGenJump(ILGenInfo *info, int opcode, ILLabel *label);

/*
 * Output a label at the current code location.
 */
void JavaGenLabel(ILGenInfo *info, ILLabel *label);

/*
 * Store a value to a local variable.
 */
void JavaGenStoreLocal(ILGenInfo *info, unsigned varNum,
					   ILMachineType type);

/*
 * Load a value from a local variable.
 */
void JavaGenLoadLocal(ILGenInfo *info, unsigned varNum,
					  ILMachineType type);

/*
 * Get the number of stack positions occupied by a machine type.
 */
int JavaGenTypeSize(ILMachineType type);

/*
 * Output a call to a static method given its named signature.
 */
void JavaGenCallByName(ILGenInfo *info, const char *className,
					   const char *methodName, const char *signature);

/*
 * Output a call to a static intrinsic method given its named signature.
 */
void JavaGenCallIntrinsic(ILGenInfo *info, const char *methodName,
						  const char *signature);

/*
 * Output a call to a named virtual method.
 */
void JavaGenCallVirtual(ILGenInfo *info, const char *className,
						const char *methodName, const char *signature);

/*
 * Output a call to a named virtual method in an intrinsc class.
 */
void JavaGenCallVirtIntrinsic(ILGenInfo *info, const char *className,
						      const char *methodName, const char *signature);

/*
 * Output a call to a method given its description block.
 */
void JavaGenCallByMethod(ILGenInfo *info, ILMethod *method);

/*
 * Output a "newobj" instruction and call a constructor.
 */
void JavaGenNewObj(ILGenInfo *info, const char *className,
				   const char *initName, const char *signature);

/*
 * Output a "newobj" instruction and call a constructor
 * for an intrinsic class.
 */
void JavaGenNewIntrinsic(ILGenInfo *info, const char *className,
				   		 const char *initName, const char *signature);

/*
 * Generate an instruction that refers to a class.
 */
void JavaGenClassRef(ILGenInfo *info, int opcode, ILClass *classInfo);

/*
 * Begin the creation of a class.
 */
void JavaGenCreateClass(ILGenInfo *info, ILClass *classInfo);

/*
 * Create a new field within the current class.
 */
void JavaGenCreateField(ILGenInfo *info, ILField *fieldInfo);

/*
 * Create a new method within the current class with
 * the contents of the current code area.
 */
void JavaGenCreateMethod(ILGenInfo *info, ILMethod *methodInfo);

/*
 * Create a new property get method within the current
 * class with the contents of the current code area.
 */
void JavaGenCreateGetMethod(ILGenInfo *info, ILProperty *propInfo);

/*
 * Create a new property set method within the current
 * class with the contents of the current code area.
 */
void JavaGenCreateSetMethod(ILGenInfo *info, ILProperty *propInfo);

/*
 * Flush the peephole optimization queue.
 */
void JavaGenFlush(ILGenInfo *info);

/*
 * Adjust the height of the operand stack.
 */
#define	JavaGenAdjust(info,amount)	ILGenAdjust((info), (amount))

/*
 * Extend the height of the operand stack to account for temporary values.
 */
#define	JavaGenExtend(info,amount)	ILGenExtend((info), (amount))

/*
 * Convert the name of a class information block into
 * a Java class name.  The return value must be free'd
 * with "ILFree".
 */
char *JavaGetClassName(ILGenInfo *info, ILClass *classInfo);

/*
 * Append two strings.  "str1" is realloc'ed to be large enough.
 */
char *JavaStrAppend(ILGenInfo *info, char *str1, const char *str2);

#ifdef	__cplusplus
};
#endif

#endif	/* _CODEGEN_JV_OUTPUT_H */
