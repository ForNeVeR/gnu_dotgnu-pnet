/*
 * il_coder.h - Interface to the "coder" within the runtime engine.
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

/*
The coder interface provides hooks for "Just In Time" compilers
to add themselves to the runtime engine.  The bytecode verifier
makes callbacks into the coder to generate the final code.

Coders do not have to be JIT's.  They can be any back-end that
wants to rebuild the structure of a method's code given the type
information that is inferred by the bytecode verifier.
*/

#ifndef	_IL_CODER_H
#define	_IL_CODER_H

#include "il_engine.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Forward declarations.
 */
typedef struct _tagILCoder      ILCoder;
typedef struct _tagILCoderClass ILCoderClass;

/*
 * Engine types.
 */
typedef enum
{
	ILEngineType_I4,
	ILEngineType_I8,
	ILEngineType_I,
	ILEngineType_F,
	ILEngineType_M,
	ILEngineType_O,
	ILEngineType_T,
	ILEngineType_MV,
	ILEngineType_Invalid

} ILEngineType;
#define	ILEngineType_ValidTypes	8

/*
 * Common parent fields that are shared by all coder instances.
 */
struct _tagILCoder
{
	const ILCoderClass *classInfo;

};

/*
 * Coder class definition.
 */
struct _tagILCoderClass
{
	/*
	 * Create a coder instance.  Returns NULL if not possible.
	 */
	ILCoder *(*create)(ILExecThread *thread, ILUInt32 size);
	
	/*
	 * Set up a coder instance for processing a specific method.
	 * Returns zero if not possible.
	 */
	int (*setup)(ILCoder *coder, ILMethod *method, ILMethodCode *code);

	/*
	 * Destroy a coder instance.
	 */
	void (*destroy)(ILCoder *coder);

	/*
	 * Finish processing on the method.  Returns zero if
	 * something went wrong during coding.
	 */
	int (*finish)(ILCoder *coder);

	/*
	 * Determine if a restart is needed because the coder
	 * ran out of cache space on the first pass.
	 */
	int (*restart)(ILCoder *coder);

	/*
	 * Output a label to the coder.  "offset" is the
	 * position within the IL bytecode of the label.
	 */
	void (*label)(ILCoder *coder, ILUInt32 offset);

	/*
	 * Record the height of a stack item.  This is called
	 * just after a label is output to notify the coder as
	 * to what values are on the stack at the label.
	 */
	void (*stackItem)(ILCoder *coder, ILEngineType engineType, ILType *type);

	/*
	 * Handle a constant value.
	 */
	void (*constant)(ILCoder *coder, int opcode, unsigned char *arg);

	/*
	 * Handle a binary operator.
	 */
	void (*binary)(ILCoder *coder, int opcode, ILEngineType type1,
				   ILEngineType type2);

	/*
	 * Handle a binary operator when pointer arithmetic is involved.
	 */
	void (*binaryPtr)(ILCoder *coder, int opcode, ILEngineType type1,
				      ILEngineType type2);

	/*
	 * Handle a shift operator.
	 */
	void (*shift)(ILCoder *coder, int opcode, ILEngineType type1,
				  ILEngineType type2);

	/*
	 * Handle a unary operator.
	 */
	void (*unary)(ILCoder *coder, int opcode, ILEngineType type);

	/*
	 * Load an argument onto the stack.
	 */
	void (*loadArg)(ILCoder *coder, ILUInt32 num, ILType *type);

	/*
	 * Store the top of the stack into an argument.
	 */
	void (*storeArg)(ILCoder *coder, ILUInt32 num,
					 ILEngineType engineType, ILType *type);

	/*
	 * Load the address of an argument onto the stack.
	 */
	void (*addrOfArg)(ILCoder *coder, ILUInt32 num);

	/*
	 * Load a local variable onto the stack.
	 */
	void (*loadLocal)(ILCoder *coder, ILUInt32 num, ILType *type);

	/*
	 * Store the top of the stack into a local variable.
	 */
	void (*storeLocal)(ILCoder *coder, ILUInt32 num,
					   ILEngineType engineType, ILType *type);

	/*
	 * Load the address of a local variable onto the stack.
	 */
	void (*addrOfLocal)(ILCoder *coder, ILUInt32 num);

	/*
	 * Duplicate the top of stack.
	 */
	void (*dup)(ILCoder *coder, ILEngineType valueType, ILType *type);

	/*
	 * Pop the top of stack.
	 */
	void (*pop)(ILCoder *coder, ILEngineType valueType, ILType *type);

	/*
	 * Access an element within an array.
	 */
	void (*arrayAccess)(ILCoder *coder, int opcode,
						ILEngineType indexType, ILType *elemType);

	/*
	 * Access a value by dereferencing a pointer.
	 */
	void (*ptrAccess)(ILCoder *coder, int opcode);

	/*
	 * Output a branch instruction.
	 */
	void (*branch)(ILCoder *coder, int opcode, ILUInt32 dest,
				   ILEngineType type1, ILEngineType type2);

	/*
	 * Output a branch instruction that involves binary pointer comparisons.
	 */
	void (*branchPtr)(ILCoder *coder, int opcode, ILUInt32 dest,
				      ILEngineType type1, ILEngineType type2);

	/*
	 * Output a conversion instruction.
	 */
	void (*conv)(ILCoder *coder, int opcode, ILEngineType type);

};

/*
 * Helper macros for calling coder methods.
 */
#define	ILCoderCreate(classInfo,thread)	\
			((*((classInfo)->create))((thread)))
#define	ILCoderSetup(coder,method,code) \
			((*((coder)->classInfo->setup))((coder), (method), (code)))
#define	ILCoderDestroy(coder) \
			((*((coder)->classInfo->destroy))((coder)))
#define	ILCoderFinish(coder) \
			((*((coder)->classInfo->finish))((coder)))
#define	ILCoderRestart(coder) \
			((*((coder)->classInfo->restart))((coder)))
#define	ILCoderLabel(coder,offset) \
			((*((coder)->classInfo->label))((coder), (offset)))
#define	ILCoderStackItem(coder,vtype,type) \
			((*((coder)->classInfo->stackItem))((coder), (vtype), (type)))
#define	ILCoderConstant(coder,opcode,arg) \
			((*((coder)->classInfo->constant))((coder), (opcode), (arg)))
#define	ILCoderBinary(coder,opcode,type1,type2) \
			((*((coder)->classInfo->binary))((coder), (opcode), (type1), \
											 (type2)))
#define	ILCoderBinaryPtr(coder,opcode,type1,type2) \
			((*((coder)->classInfo->binaryPtr))((coder), (opcode), \
												(type1), (type2)))
#define	ILCoderShift(coder,opcode,type1,type2) \
			((*((coder)->classInfo->shift))((coder), (opcode), \
											(type1), (type2)))
#define	ILCoderUnary(coder,opcode,type) \
			((*((coder)->classInfo->unary))((coder), (opcode), (type)))
#define	ILCoderLoadArg(coder,num,type)	\
			((*((coder)->classInfo->loadArg))((coder), (num), (type)))
#define	ILCoderStoreArg(coder,num,etype,type)	\
			((*((coder)->classInfo->storeArg))((coder), (num), \
											   (etype), (type)))
#define	ILCoderAddrOfArg(coder,num)	\
			((*((coder)->classInfo->addrOfArg))((coder), (num)))
#define	ILCoderLoadLocal(coder,num,type)	\
			((*((coder)->classInfo->loadLocal))((coder), (num), (type)))
#define	ILCoderStoreLocal(coder,num,etype,type)	\
			((*((coder)->classInfo->storeLocal))((coder), (num), \
												 (etype), (type)))
#define	ILCoderAddrOfLocal(coder,num)	\
			((*((coder)->classInfo->addrOfLocal))((coder), (num)))
#define	ILCoderDup(coder,etype,type)	\
			((*((coder)->classInfo->dup))((coder), (etype), (type)))
#define	ILCoderPop(coder,etype,type)	\
			((*((coder)->classInfo->pop))((coder), (etype), (type)))
#define	ILCoderArrayAccess(coder,opcode,itype,etype)	\
			((*((coder)->classInfo->arrayAccess))((coder), (opcode), \
												  (itype), (etype)))
#define	ILCoderPtrAccess(coder,opcode)	\
			((*((coder)->classInfo->ptrAccess))((coder), (opcode)))
#define	ILCoderBranch(coder,opcode,dest,type1,type2)	\
			((*((coder)->classInfo->branch))((coder), (opcode), (dest), \
											 (type1), (type2)))
#define	ILCoderBranchPtr(coder,opcode,dest,type1,type2)	\
			((*((coder)->classInfo->branchPtr))((coder), (opcode), (dest), \
											    (type1), (type2)))
#define	ILCoderConv(coder,opcode,type) \
			((*((coder)->classInfo->conv))((coder), (opcode), (type)))

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_CODER_H */
